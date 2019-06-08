/* Hey EMACS -*- linux-c -*- */

/*  libticalcs2 - hand-helds support library, a part of the TILP project
 *  Copyright (C) 2026  Lionel Debroux
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

/*
	Communication with Vernier Go! Temp / Go! Link / Go! Motion over direct USB.
*/

// The Go! Temp (Jonah), Go! Link (Skip) and Go! Motion (Cyclops) devices all speak the same protocol,
// documented by Vernier's GoIO SDK (src/GoIO_cpp, GSkipComm.h / GSkipCommExt.h):
// - HID-style transfers.
// - commands are fixed-size 8-byte packets: [command][parameters...];
// - every command gets an 8-byte response, whose first byte distinguishes the packet type:
//   bits 7-6: 00 = measurement packet, 01 = command response, 10 = INIT response, 11 = notification;
//   bit 5 (0x20): error flag (command responses have header 0x5A on success, 0x7A on error);
// - measurement packets: [nMeasurements 0-3][rolling counter][meas0 LE16][meas1 LE16][meas2 LE16].
//
// The command IDs below are shared with the LabQuest family devices (LabQuest, LabQuest Mini, LabQuest 2,
// LabQuest 3, LabQuest Stream), whose wire protocol is implemented in Vernier's closed-source NGIO library
// used by the public ngio_sdk, but uses larger frames up to 14 parameter bytes.
//
// While the device is powering up, all commands except INIT are ignored; the INIT command may need to be
// retried until the slave CPU has started (error statuses 0x40 and 0x41).
enum
{
	NGIO_CMD_ID_GET_STATUS                = 0x10,
	NGIO_CMD_ID_START_MEASUREMENTS        = 0x18,
	NGIO_CMD_ID_STOP_MEASUREMENTS         = 0x19,
	NGIO_CMD_ID_INIT                      = 0x1A,
	NGIO_CMD_ID_SET_MEASUREMENT_PERIOD    = 0x1B, // 16-bit LE number of ticks; tick is 128 us for Go! Temp, 1 ms for Go! Link and Go! Motion.
	NGIO_CMD_ID_SET_LED_STATE             = 0x1D, // (color, brightness).
	NGIO_CMD_ID_SET_ANALOG_INPUT          = 0x21, // NGIO family: analog input range (5 V or 10 V).
	NGIO_CMD_ID_GET_SENSOR_ID             = 0x28,
	// The 0x29 ID is used with two different meanings, depending on the device family:
	NGIO_CMD_ID_SET_SAMPLING_MODE         = 0x29, // NGIO family: digital sampling mode.
	NGIO_CMD_ID_SET_ANALOG_INPUT_CHANNEL  = 0x29, // Go! family: analog input channel (Go! Link only).
	NGIO_CMD_ID_SET_SENSOR_CHANNEL_ENABLE_MASK = 0x2C,
	NGIO_CMD_ID_SET_DIGITAL_COUNTER       = 0x32,
	NGIO_CMD_ID_WRITE_IO_CONFIG           = 0x37,
	NGIO_CMD_ID_WRITE_IO                  = 0x39,
	NGIO_CMD_ID_SET_PWM_CONFIG            = 0x40, // DCU output.
	// The 0x50-0x56 range (GET_SENSOR_INFO, GET_SENSOR_AVAILABLE_MASK, GET_DEVICE_INFO,
	// GET_DEFAULT_SENSORS_MASK) is used by the Go Direct devices, see calcgdx.cc.
};

int tixx_get_version_lab_equipment_usb_go(CalcHandle* handle, CalcInfos* infos)
{
	int ret;
	uint8_t resp[8];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(infos);

	memset((void *)infos, 0, sizeof(*infos));
	infos->model = handle->model;
	infos->mask = INFOS_CALC_MODEL;

	// GET_STATUS (0x10) response:
	// [header][0x10 echo][status][minor master BCD][major master BCD][minor slave BCD][major slave BCD][reserved].
	static uint8_t query[8] = { NGIO_CMD_ID_GET_STATUS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	ret = ticables_cable_send(handle->cable, query, sizeof(query));
	if (!ret)
	{
		// Skip any measurement packets interleaved with the response.
		uint8_t resp_type;
		for (unsigned int skipped = 0; !ret;)
		{
			ret = ticables_cable_recv(handle->cable, resp, sizeof(resp));
			if (!ret)
			{
				resp_type = resp[0] & 0xC0;
				if (resp_type == 0x00)
				{
					if (++skipped > 512)
					{
						ticalcs_critical("%s", _("Timed out waiting for a Go! device response"));
						ret = ERROR_READ_TIMEOUT;
					}
					continue;
				}
				break;
			}
		}
		if (!ret)
		{
			if (resp_type != 0x40 || (resp[0] & 0x20) || resp[1] != 0x10)
			{
				ticalcs_critical("Unexpected reply to version query: %02X %02X", resp[0], resp[1]);
				ret = ERR_INVALID_PACKET;
			}
			else
			{
				ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%u.%03u", resp[4], resp[3]);
				infos->mask |= INFOS_OS_VERSION;
				if (handle->model == CALC_CBR2_GOMOTION_USB)
				{
					// Only Go! Motion reports a battery state, in bits 3-2 of the status byte.
					infos->battery = ((resp[2] & 0x0C) == 0) ? 1 : 0;
					infos->mask |= INFOS_BATTERY;
				}
				ret = 0;
			}
		}
	}

	return ret;
}

// Send one 8-byte command and read the corresponding 8-byte response, skipping any measurement
// packets (header type bits 7-6 = 0) which may be interleaved once measurements have started.
static int tixx_send_gotemp_golink_command(CalcHandle* handle, const uint8_t * cmd, uint8_t resp[8])
{
	int ret;
	ret = ticables_cable_send(handle->cable, (uint8_t *)cmd, 8);
	if (!ret)
	{
		for (unsigned int skipped = 0; !ret;)
		{
			ret = ticables_cable_recv(handle->cable, resp, 8);
			if (!ret)
			{
				if ((resp[0] & 0xC0) == 0x00)
				{
					// A measurement packet: it belongs to the measurement stream, skip it. Bound the
					// number of skipped packets, in case the command response never comes.
					if (++skipped > 512)
					{
						ticalcs_critical("%s", _("Timed out waiting for a Go! device response"));
						ret = ERROR_READ_TIMEOUT;
					}
				}
				else
				{
					// A command response (header type bits 7-6 != 0): stop reading.
					break;
				}
			}
		}
	}
	return ret;
}

// Send an 8-byte Go! family command and validate its command response.
static int tixx_send_gotemp_golink_command_checked(CalcHandle* handle, const uint8_t * cmd, uint8_t resp[8])
{
	int ret = tixx_send_gotemp_golink_command(handle, cmd, resp);
	if (!ret)
	{
		if ((resp[0] & 0xC0) != 0x40 || (resp[0] & 0x20) || resp[1] != cmd[0])
		{
			ticalcs_critical("Unexpected reply to command %02X: %02X %02X", cmd[0], resp[0], resp[1]);
			ret = ERR_INVALID_PACKET;
		}
	}
	return ret;
}

int TICALL tixx_control_lab_equipment_usb_go(CalcHandle* handle, CalcModel model, CalcLabEquipmentParameters * params)
{
	int ret = 0;
	uint8_t resp[8];
	const int gotemp = (model == CALC_EASYTEMP_GOTEMP_USB);
	const int golink = (model == CALC_EASYLINK_GOLINK_USB);
	// The tick length is 128 us for Go! Temp, 1 ms for Go! Link and Go! Motion.
	const double tick_seconds = gotemp ? 0.000128 : 0.001;
	// Default periods: 0.5 s for Go! Temp and Go! Link, 0.1 s for Go! Motion.
	const double default_period = (gotemp || golink) ? 0.5 : 0.1;
	double requested_period;
	uint8_t led_color;
	uint8_t led_brightness;
	uint8_t analog_channel;
	int do_start;
	int do_stop;

	VALIDATE_HANDLE(handle);

	if (nullptr == params)
	{
		// Default: full initialization sequence, with the default period and measurements started.
		requested_period = default_period;
		led_color = 0x80;
		led_brightness = 0x02;
		analog_channel = (uint8_t)(golink ? 2 : 0xFF);
		do_start = 1;
		do_stop = 0;
	}
	else
	{
		requested_period = (params->measurement_period != 0.0) ? params->measurement_period : 0.0;
		led_color = params->led_color;
		led_brightness = params->led_brightness;
		analog_channel = params->analog_input_channel;
		do_start = params->start_measurements;
		do_stop = params->stop_measurements;
	}

	if (nullptr == params || params->reset)
	{
		// Startup delay, then INIT: it may be ignored while the device is powering up, so retry it
		// until the device acknowledges it, or until the error status is something other than the
		// power-up ones.
		static const uint8_t init_cmd[8] = { NGIO_CMD_ID_INIT, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		g_usleep((gotemp ? 100 : 1500) * 1000);
		int init_ok = 0;
		for (int attempt = 0; attempt < 10 && !init_ok; attempt++)
		{
			ticalcs_info("Sending INIT command");
			ret = tixx_send_gotemp_golink_command(handle, init_cmd, resp);
			if (ret)
			{
				if (ret != ERROR_READ_TIMEOUT)
				{
					break;
				}
				// The device didn't answer yet: it may still be powering up.
				continue;
			}
			if ((resp[0] & 0xC0) != 0x80)
			{
				ticalcs_critical("Unexpected reply to INIT command: %02X", resp[0]);
				ret = ERR_INVALID_PACKET;
				break;
			}
			if (resp[2] != 0 && resp[2] != 0x40 && resp[2] != 0x41)
			{
				ticalcs_critical("INIT failed with status %02X", resp[2]);
				ret = ERR_INVALID_PACKET;
				break;
			}
			if (resp[2] == 0)
			{
				init_ok = 1;
				ret = 0;
			}
			// Otherwise the device is still powering up: retry.
		}
		if (!init_ok && !ret)
		{
			ticalcs_critical("%s", _("Timed out waiting for the device to initialize"));
			ret = ERROR_READ_TIMEOUT;
		}
	}

	if (!ret && led_color != 0xFF)
	{
		// SET_LED_STATE (color, brightness).
		uint8_t cmd[8] = { NGIO_CMD_ID_SET_LED_STATE, led_color, (uint8_t)((led_brightness != 0xFF) ? led_brightness : 0x02), 0, 0, 0, 0, 0 };
		ticalcs_info("Sending SET_LED_STATE command");
		ret = tixx_send_gotemp_golink_command_checked(handle, cmd, resp);
	}
	if (!ret && analog_channel != 0xFF && golink)
	{
		// SET_ANALOG_INPUT_CHANNEL (Go! Link only).
		uint8_t cmd[8] = { NGIO_CMD_ID_SET_ANALOG_INPUT_CHANNEL, analog_channel, 0, 0, 0, 0, 0, 0 };
		ticalcs_info("Sending SET_ANALOG_INPUT_CHANNEL command");
		ret = tixx_send_gotemp_golink_command_checked(handle, cmd, resp);
	}
	if (!ret && requested_period != 0.0)
	{
		// SET_MEASUREMENT_PERIOD, in ticks of the device's clock (16-bit little-endian).
		// Note that the device rejects the change while collecting (status 0x34): stop first.
		// Check the range before converting to ticks, so that pathological periods can't cause
		// undefined behaviour in the floating-point to integer conversion.
		const double ticks_d = requested_period / tick_seconds;
		if (ticks_d < 1.0 || ticks_d > 65535.0)
		{
			ticalcs_critical("%s", _("Measurement period out of range"));
			ret = ERR_OUT_OF_RANGE;
		}
		else
		{
			const uint32_t ticks = (uint32_t)(ticks_d + 0.5);
			uint8_t cmd[8] = { NGIO_CMD_ID_SET_MEASUREMENT_PERIOD, (uint8_t)(ticks & 0xFF), (uint8_t)((ticks >> 8) & 0xFF), 0, 0, 0, 0, 0 };
			ticalcs_info("Sending SET_MEASUREMENT_PERIOD command (%u ticks)", ticks);
			ret = tixx_send_gotemp_golink_command_checked(handle, cmd, resp);
		}
	}
	if (!ret && do_stop)
	{
		// STOP_MEASUREMENTS.
		static const uint8_t cmd[8] = { NGIO_CMD_ID_STOP_MEASUREMENTS, 0, 0, 0, 0, 0, 0, 0 };
		ticalcs_info("Sending STOP_MEASUREMENTS command");
		ret = tixx_send_gotemp_golink_command_checked(handle, cmd, resp);
	}
	if (!ret && do_start)
	{
		// START_MEASUREMENTS (real time by default; Go! Motion also accepts a parameter block).
		static const uint8_t cmd[8] = { NGIO_CMD_ID_START_MEASUREMENTS, 0, 0, 0, 0, 0, 0, 0 };
		ticalcs_info("Sending START_MEASUREMENTS command");
		ret = tixx_send_gotemp_golink_command_checked(handle, cmd, resp);
	}

	return ret;
}

// Go! Temp / Go! Link / Go! Motion: fixed-size 8-byte command and response packets.
int TICALL tixx_send_lab_equipment_data_usb_go(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	if (model != CALC_EASYTEMP_GOTEMP_USB && model != CALC_EASYLINK_GOLINK_USB && model != CALC_CBR2_GOMOTION_USB)
	{
		ticalcs_warning("Model %d is not a Go! family device", model);
	}
	VALIDATE_NONNULL(lab_equipment_data);

	if (lab_equipment_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_RAW)
	{
		ticalcs_critical("%s", _("Expected raw data with this lab equipment"));
		return ERR_INVALID_PARAMETER;
	}
	if (lab_equipment_data->size != 8 || nullptr == lab_equipment_data->data)
	{
		ticalcs_critical("%s", _("Expected an 8-byte command"));
		return ERR_INVALID_PARAMETER;
	}
	return ticables_cable_send(handle->cable, (uint8_t *)lab_equipment_data->data, 8);
}

int TICALL tixx_get_lab_equipment_data_usb_go(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	uint8_t resp[8];

	VALIDATE_NONNULL(lab_equipment_data);
	int ret = ticables_cable_recv(handle->cable, resp, sizeof(resp));
	if (!ret)
	{
		uint8_t * copy = (uint8_t *)g_malloc(sizeof(resp));
		if (nullptr != copy)
		{
			memcpy(copy, resp, sizeof(resp));
			ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_RAW, sizeof(resp), 0, copy, nullptr, 0, 0, 0);
		}
		else
		{
			ret = ERR_MALLOC;
		}
	}
	return ret;
}

extern const CalcFncts calc_easytemp_gotemp_usb =
{
	CALC_EASYTEMP_GOTEMP_USB,
	"EASYTEMP_GOTEMP_USB",
	"EasyTemp / Go! Temp (USB)",
	"EasyTemp / Go! Temp (USB)",
	OPS_ISREADY | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "",     /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "",     /* dump_rom_1 */
	 "",     /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* control_lab_equipment */
	 "",     /* send_lab_equipment_data */
	 ""      /* get_lab_equipment_data */ },
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
	&noop_recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&noop_dump_rom_1,
	&noop_dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&noop_del_var,
	&noop_new_folder,
	&tixx_get_version_lab_equipment_usb_go,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_control_lab_equipment_usb_go,
	&tixx_send_lab_equipment_data_usb_go,
	&tixx_get_lab_equipment_data_usb_go
};

extern const CalcFncts calc_easylink_golink_usb =
{
	CALC_EASYLINK_GOLINK_USB,
	"EASYLINK_GOLINK_USB",
	"EasyLink / Go! Link (USB)",
	"EasyLink / Go! Link (USB)",
	OPS_ISREADY | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "",     /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "",     /* dump_rom_1 */
	 "",     /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* control_lab_equipment */
	 "",     /* send_lab_equipment_data */
	 ""      /* get_lab_equipment_data */ },
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
	&noop_recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&noop_dump_rom_1,
	&noop_dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&noop_del_var,
	&noop_new_folder,
	&tixx_get_version_lab_equipment_usb_go,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_control_lab_equipment_usb_go,
	&tixx_send_lab_equipment_data_usb_go,
	&tixx_get_lab_equipment_data_usb_go
};

extern const CalcFncts calc_cbr2_gomotion_usb =
{
	CALC_CBR2_GOMOTION_USB,
	"CBR2_GOMOTION_USB",
	"Go! Motion (USB)",
	"Go! Motion (USB)",
	OPS_ISREADY | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "",     /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "",     /* dump_rom_1 */
	 "",     /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* control_lab_equipment */
	 "",     /* send_lab_equipment_data */
	 ""      /* get_lab_equipment_data */ },
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
	&noop_recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&noop_dump_rom_1,
	&noop_dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&noop_del_var,
	&noop_new_folder,
	&tixx_get_version_lab_equipment_usb_go,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_control_lab_equipment_usb_go,
	&tixx_send_lab_equipment_data_usb_go,
	&tixx_get_lab_equipment_data_usb_go
};
