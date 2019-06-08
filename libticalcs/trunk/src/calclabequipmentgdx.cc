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

/*
	Communication with Vernier Go! Direct (GDX) devices over direct USB.
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

// The Go Direct protocol is documented by Vernier's godirect-py module (godirect/device.py).
// - Commands are GDX packets: [0x58][len][rolling counter][checksum][cmd][params...]. The checksum
//   is the sum of all bytes except the checksum byte, modulo 256, and the rolling counter starts
//   at 0xFF and decrements with every packet sent.
// - Every command gets a response, whose first byte is 0x58 as well, with the command byte echoed
//   at offset 4. Measurement packets, whose first byte is 0x20, arrive asynchronously once
//   measurements have been started, and must be skipped while awaiting a command response.
// - Over USB, the device is a HID device (VID 0x08F7, PID 0x0010): the host sends 64-byte reports
//   whose first byte is the GDX packet length, and the device sends 64-byte reports starting with
//   the GDX packet itself (whose length is at byte 1).

enum
{
	GDX_CMD_GET_STATUS                = 0x10,
	GDX_CMD_START_MEASUREMENTS        = 0x18,
	GDX_CMD_STOP_MEASUREMENTS         = 0x19,
	GDX_CMD_INIT                      = 0x1A,
	GDX_CMD_SET_MEASUREMENT_PERIOD    = 0x1B,
	GDX_CMD_GET_SENSOR_INFO           = 0x50,
	GDX_CMD_GET_SENSOR_AVAILABLE_MASK = 0x51,
	GDX_CMD_DISCONNECT                = 0x54,
	GDX_CMD_GET_DEVICE_INFO           = 0x55,
	GDX_CMD_GET_DEFAULT_SENSORS_MASK  = 0x56,
};

#define GDX_RESPONSE_MEASUREMENT 0x20

enum
{
	GDX_MEASUREMENT_TYPE_NORMAL_REAL32         = 0x06,
	GDX_MEASUREMENT_TYPE_WIDE_REAL32           = 0x07,
	GDX_MEASUREMENT_TYPE_SINGLE_CHANNEL_REAL32 = 0x08,
	GDX_MEASUREMENT_TYPE_SINGLE_CHANNEL_INT32  = 0x09,
	GDX_MEASUREMENT_TYPE_APERIODIC_REAL32      = 0x0A,
	GDX_MEASUREMENT_TYPE_APERIODIC_INT32       = 0x0B,
	GDX_MEASUREMENT_TYPE_START_TIME            = 0x0C,
	GDX_MEASUREMENT_TYPE_DROPPED               = 0x0D,
	GDX_MEASUREMENT_TYPE_PERIOD                = 0x0E,
};

// Build a GDX packet: [0x58][len][counter][checksum][cmd][params...].
static void tixx_gdx_build_packet(CalcHandle* handle, uint8_t * packet, uint8_t cmd, const uint8_t * params, uint8_t param_len)
{
	const uint8_t len = 5 + param_len;
	uint8_t checksum = 0;

	packet[0] = 0x58;
	packet[1] = len;
	packet[2] = handle->priv.gdx_rolling_counter--;
	packet[3] = 0; // Checksum, computed below.
	packet[4] = cmd;
	if (param_len > 0)
	{
		memcpy(packet + 5, params, param_len);
	}
	for (uint8_t i = 0; i < len; i++)
	{
		if (i != 3)
		{
			checksum += packet[i];
		}
	}
	packet[3] = checksum;
}

// Send a GDX packet: a 64-byte report whose first byte is the packet length.
static int tixx_gdx_send_packet(CalcHandle* handle, const uint8_t * packet, uint8_t len)
{
	uint8_t report[64];
	if (len > 63)
	{
		// The 64-byte report only has room for a 63-byte packet after the length byte.
		ticalcs_critical("%s", _("GDX packet too large"));
		return ERR_INVALID_PARAMETER;
	}
	report[0] = len;
	memcpy(report + 1, packet, len);
	memset(report + 1 + len, 0, sizeof(report) - len - 1);
	return ticables_cable_send(handle->cable, report, sizeof(report));
}

// Read one 64-byte report and extract the GDX packet from it.
static int tixx_gdx_read_packet(CalcHandle* handle, uint8_t * packet, uint8_t max_len, uint8_t * out_len)
{
	uint8_t report[64];
	int ret = ticables_cable_recv(handle->cable, report, sizeof(report));
	if (!ret)
	{
		// The report starts with the GDX packet itself, whose length is at byte 1.
		if (report[0] != 0x58 && report[0] != GDX_RESPONSE_MEASUREMENT)
		{
			ticalcs_critical("Unexpected GDX packet type %02X", report[0]);
			ret = ERR_INVALID_PACKET;
		}
		else
		{
			if (report[1] < 5 || report[1] > max_len)
			{
				ticalcs_critical("Invalid GDX packet length %u", report[1]);
				ret = ERR_INVALID_PACKET;
			}
			else
			{
				// Validate the checksum: the sum of all bytes except the checksum byte, modulo 256.
				uint8_t checksum = 0;
				for (uint8_t i = 0; i < report[1]; i++)
				{
					if (i != 3)
					{
						checksum += report[i];
					}
				}
				if (checksum != report[3])
				{
					ticalcs_critical("%s", _("GDX packet checksum mismatch"));
					ret =  ERR_INVALID_PACKET;
				}
				else
				{
					memcpy(packet, report, report[1]);
					*out_len = report[1];
					ret = 0;
				}
			}
		}
	}

	return ret;
}

// Send a GDX command and read its response, skipping any measurement packets in between.
static int tixx_gdx_command(CalcHandle* handle, uint8_t cmd, const uint8_t * params, uint8_t param_len, uint8_t * resp, uint8_t * resp_len)
{
	uint8_t packet[64];
	uint8_t len;
	int ret;

	tixx_gdx_build_packet(handle, packet, cmd, params, param_len);
	ret = tixx_gdx_send_packet(handle, packet, 5 + param_len);
	if (!ret)
	{
		for (unsigned int skipped = 0; !ret;)
		{
			ret = tixx_gdx_read_packet(handle, packet, sizeof(packet), &len);
			if (!ret)
			{
				if (packet[0] == GDX_RESPONSE_MEASUREMENT)
				{
					// A measurement packet: it belongs to the measurement stream, skip it. Bound the
					// number of skipped packets, in case the command response never comes.
					if (++skipped > 512)
					{
						ticalcs_critical("%s", _("Timed out waiting for a GDX command response"));
						ret = ERROR_READ_TIMEOUT;
					}
					continue;
				}
				if (packet[4] != cmd)
				{
					ticalcs_critical("Unexpected reply to GDX command %02X: %02X", cmd, packet[4]);
					ret = ERR_INVALID_PACKET;
				}
				else
				{
					if (resp)
					{
						memcpy(resp, packet, len);
					}
					if (resp_len)
					{
						*resp_len = len;
					}
					ret = 0;
				}
			}
		}
	}

	return ret;
}

int TICALL tixx_get_version_lab_equipment_usb_gdx(CalcHandle* handle, CalcInfos* infos)
{
	uint8_t resp[64];
	uint8_t resp_len;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(infos);

	memset((void *)infos, 0, sizeof(*infos));
	infos->model = handle->model;
	infos->mask = INFOS_CALC_MODEL;

	// GET_STATUS (0x10) response:
	// [0x58][len][ctr][csum][0x10 echo][spare][status][spare][major primary][minor primary]
	// [build primary LE16][major secondary][minor secondary][build secondary LE16][battery %][charger state].
	ret = tixx_gdx_command(handle, GDX_CMD_GET_STATUS, nullptr, 0, resp, &resp_len);
	if (!ret)
	{
		if (resp_len >= 18)
		{
			g_snprintf(infos->os_version, sizeof(infos->os_version), "%u.%u.%u", resp[8], resp[9], resp[10] | (resp[11] << 8));
			infos->battery = (resp[16] > 0) ? 1 : 0;
			infos->mask |= INFOS_OS_VERSION | INFOS_BATTERY;
		}
		else
		{
			ticalcs_critical("%s", _("Malformed GDX status response"));
			ret = ERR_INVALID_PACKET;
		}
	}

	return ret;
}

int TICALL tixx_control_lab_equipment_usb_gdx(CalcHandle* handle, CalcModel model, CalcLabEquipmentParameters * params)
{
	int ret = 0;
	uint8_t resp[64];
	uint8_t resp_len;

	VALIDATE_HANDLE(handle);
	(void)model; // All Go Direct devices are handled the same way.

	const int do_reset = (nullptr == params) || params->reset;
	const double requested_period = (nullptr == params) ? 0.0 : params->measurement_period;
	const int do_start = (nullptr == params) || params->start_measurements;
	const int do_stop = (nullptr == params) ? 0 : params->stop_measurements;
	uint32_t sensor_mask = (nullptr == params) ? 0 : params->sensor_mask;

	if (do_reset)
	{
		// INIT, with the sensor number table used by godirect-py, then the status and device info
		// queries which make up the initialization handshake.
		static const uint8_t init_params[20] =
		{
			0xa5, 0x4a, 0x06, 0x49, 0x07, 0x48, 0x08, 0x47,
			0x09, 0x46, 0x0a, 0x45, 0x0b, 0x44, 0x0c, 0x43,
			0x0d, 0x42, 0x0e, 0x41
		};
		handle->priv.gdx_rolling_counter = 0xFF;
		ticalcs_info("Sending GDX INIT command");
		ret = tixx_gdx_command(handle, GDX_CMD_INIT, init_params, sizeof(init_params), nullptr, nullptr);
		if (!ret)
		{
			ret = tixx_gdx_command(handle, GDX_CMD_GET_STATUS, nullptr, 0, nullptr, nullptr);
			if (!ret)
			{
				ret = tixx_gdx_command(handle, GDX_CMD_GET_DEVICE_INFO, nullptr, 0, nullptr, nullptr);
			}
		}
	}
	if (!ret && requested_period != 0.0)
	{
		// SET_MEASUREMENT_PERIOD, in microseconds (32-bit little-endian). Check the range before
		// converting, so that pathological periods can't cause undefined behaviour in the
		// floating-point to integer conversion.
		const double us_d = requested_period * 1000000.0;
		if (us_d < 1.0 || us_d + 0.5 >= 4294967295.0)
		{
			ticalcs_critical("%s", _("Measurement period out of range"));
			return ERR_OUT_OF_RANGE;
		}
		const uint32_t period_us = (uint32_t)(us_d + 0.5);
		uint8_t cmd_params[10] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0, 0, 0 };
		cmd_params[2] = period_us & 0xFF;
		cmd_params[3] = (period_us >> 8) & 0xFF;
		cmd_params[4] = (period_us >> 16) & 0xFF;
		cmd_params[5] = (period_us >> 24) & 0xFF;
		ticalcs_info("Sending GDX SET_MEASUREMENT_PERIOD command (%u us)", period_us);
		ret = tixx_gdx_command(handle, GDX_CMD_SET_MEASUREMENT_PERIOD, cmd_params, sizeof(cmd_params), nullptr, nullptr);
	}
	if (!ret && do_stop)
	{
		// STOP_MEASUREMENTS.
		static const uint8_t cmd_params[6] = { 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };
		ticalcs_info("Sending GDX STOP_MEASUREMENTS command");
		ret = tixx_gdx_command(handle, GDX_CMD_STOP_MEASUREMENTS, cmd_params, sizeof(cmd_params), nullptr, nullptr);
	}
	if (!ret && do_start)
	{
		// START_MEASUREMENTS with the requested sensor mask, or the device's default sensors.
		uint8_t cmd_params[13] = { 0xFF, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		if (sensor_mask == 0)
		{
			ret = tixx_gdx_command(handle, GDX_CMD_GET_DEFAULT_SENSORS_MASK, nullptr, 0, resp, &resp_len);
			if (!ret)
			{
				if (resp_len < 10)
				{
					ticalcs_critical("%s", _("Malformed GDX default sensors mask response"));
					ret =  ERR_INVALID_PACKET;
				}
				else
				{
					// The mask is a 32-bit little-endian value at offset 6.
					sensor_mask = (uint32_t)resp[6] | ((uint32_t)resp[7] << 8) | ((uint32_t)resp[8] << 16) | ((uint32_t)resp[9] << 24);
				}
			}
		}
		if (!ret)
		{
			cmd_params[2] = sensor_mask & 0xFF;
			cmd_params[3] = (sensor_mask >> 8) & 0xFF;
			cmd_params[4] = (sensor_mask >> 16) & 0xFF;
			cmd_params[5] = (sensor_mask >> 24) & 0xFF;
			ticalcs_info("Sending GDX START_MEASUREMENTS command (sensor mask %08X)", sensor_mask);
			ret = tixx_gdx_command(handle, GDX_CMD_START_MEASUREMENTS, cmd_params, sizeof(cmd_params), nullptr, nullptr);
		}
	}

	return ret;
}

// Read one measurement packet and decode it into the data structure: the values are stored as
// 32-bit little-endian floats in @data, with @items giving their count. The START_TIME, DROPPED
// and PERIOD packets which may be interleaved with the measurements are skipped, like godirect-py does.
static int tixx_gdx_read_measurement(CalcHandle* handle, CalcLabEquipmentData * lab_equipment_data)
{
	uint8_t packet[64];
	uint8_t len;
	uint8_t type;
	int ret = 0;

	for (;!ret;)
	{
		ret = tixx_gdx_read_packet(handle, packet, sizeof(packet), &len);
		if (!ret)
		{
			if (packet[0] != GDX_RESPONSE_MEASUREMENT)
			{
				// A command response: it doesn't belong to the measurement stream, skip it.
				continue;
			}
			type = packet[4];
			if (   type != GDX_MEASUREMENT_TYPE_NORMAL_REAL32
			    && type != GDX_MEASUREMENT_TYPE_WIDE_REAL32
			    && type != GDX_MEASUREMENT_TYPE_SINGLE_CHANNEL_REAL32
			    && type != GDX_MEASUREMENT_TYPE_SINGLE_CHANNEL_INT32
			    && type != GDX_MEASUREMENT_TYPE_APERIODIC_REAL32
			    && type != GDX_MEASUREMENT_TYPE_APERIODIC_INT32)
			{
				// START_TIME, DROPPED, PERIOD and unknown packet types carry no measurement values.
				continue;
			}
			break;
		}
	}
	if (!ret)
	{

		uint8_t value_count = 0;
		uint8_t index = 0;
		if (type == GDX_MEASUREMENT_TYPE_NORMAL_REAL32)
		{
			// [0x20][len][ctr][csum][06][sensor mask LE16][value count][values LE32 float...]
			value_count = packet[7];
			index = 9;
		}
		else if (type == GDX_MEASUREMENT_TYPE_WIDE_REAL32)
		{
			// [0x20][len][ctr][csum][07][sensor mask LE32][value count][values LE32 float...]
			value_count = packet[9];
			index = 11;
		}
		else
		{
			// SINGLE_CHANNEL_REAL32 / INT32 and APERIODIC_REAL32 / INT32:
			// [0x20][len][ctr][csum][type][sensor number][value count][values LE32...]
			value_count = packet[7];
			index = 8;
		}

		if (index + (uint16_t)value_count * 4 <= len)
		{
			lab_equipment_data->type = CALC_LAB_EQUIPMENT_DATA_TYPE_GDX_MEASUREMENT;
			lab_equipment_data->sensor_mask = 0;
			lab_equipment_data->sensor_number = 0;
			if (type == GDX_MEASUREMENT_TYPE_NORMAL_REAL32)
			{
				// [0x20][len][ctr][csum][06][sensor mask LE16][...]
				lab_equipment_data->sensor_mask = (uint32_t)packet[5] | ((uint32_t)packet[6] << 8);
			}
			else if (type == GDX_MEASUREMENT_TYPE_WIDE_REAL32)
			{
				// [0x20][len][ctr][csum][07][sensor mask LE32][...]
				lab_equipment_data->sensor_mask = (uint32_t)packet[5] | ((uint32_t)packet[6] << 8) | ((uint32_t)packet[7] << 16) | ((uint32_t)packet[8] << 24);
			}
			else
			{
				// SINGLE_CHANNEL_* and APERIODIC_*: [0x20][len][ctr][csum][type][sensor number][...]
				lab_equipment_data->sensor_number = packet[5];
			}
			uint8_t * copy = (uint8_t *)g_malloc(value_count * 4);
			if (nullptr != copy)
			{
				memcpy(copy, packet + index, value_count * 4);
				ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_GDX_MEASUREMENT, (uint16_t)(value_count * 4), value_count, copy, nullptr, 0, lab_equipment_data->sensor_number, lab_equipment_data->sensor_mask);
			}
			else
			{
				ret = ERR_MALLOC;
			}
		}
		else
		{
			ticalcs_critical("%s", _("Malformed GDX measurement packet"));
			ret = ERR_INVALID_PACKET;
		}
	}

	return ret;
}

int TICALL tixx_send_lab_equipment_data_usb_gdx(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	if (model != CALC_GODIRECT_USB)
	{
		ticalcs_warning("Model %d is not a Go Direct device", model);
	}
	VALIDATE_NONNULL(lab_equipment_data);

	if (lab_equipment_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_RAW)
	{
		ticalcs_critical("%s", _("Expected raw data with this lab equipment"));
		return ERR_INVALID_PARAMETER;
	}
	// The GDX packet has at most 63 bytes after the report length byte: command + up to 58 parameter bytes.
	if (lab_equipment_data->size < 1 || lab_equipment_data->size > 58 || nullptr == lab_equipment_data->data)
	{
		ticalcs_critical("%s", _("Invalid size for a GDX command"));
		return ERR_INVALID_PARAMETER;
	}
	// The raw data is a GDX command followed by its parameters: the library adds the packet header.
	uint8_t packet[64];
	tixx_gdx_build_packet(handle, packet, lab_equipment_data->data[0], lab_equipment_data->data + 1, (uint8_t)(lab_equipment_data->size - 1));
	return tixx_gdx_send_packet(handle, packet, 5 + lab_equipment_data->size - 1);
}

int TICALL tixx_get_lab_equipment_data_usb_gdx(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret = ERR_INVALID_PARAMETER;

	VALIDATE_NONNULL(lab_equipment_data);

	if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_GDX_MEASUREMENT)
	{
		return tixx_gdx_read_measurement(handle, lab_equipment_data);
	}
	else if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_RAW)
	{
		// Return the next packet verbatim (including its GDX header).
		uint8_t packet[64];
		uint8_t len;
		ret = tixx_gdx_read_packet(handle, packet, sizeof(packet), &len);
		if (!ret)
		{
			uint8_t * copy = (uint8_t *)g_malloc(len);
			if (nullptr != copy)
			{
				memcpy(copy, packet, len);
				ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_RAW, len, 0, copy, nullptr, 0, 0, 0);
			}
			else
			{
				ret = ERR_MALLOC;
			}
		}
	}
	else
	{
		ticalcs_critical("%s", _("Expected raw or GDX measurement data with this lab equipment"));
		ret = ERR_INVALID_PARAMETER;
	}
	return ret;
}

extern const CalcFncts calc_godirect_usb =
{
	CALC_GODIRECT_USB,
	"GODIRECT_USB",
	"Go Direct (USB)",
	"Go Direct (USB)",
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
	&tixx_get_version_lab_equipment_usb_gdx,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_control_lab_equipment_usb_gdx,
	&tixx_send_lab_equipment_data_usb_gdx,
	&tixx_get_lab_equipment_data_usb_gdx
};
