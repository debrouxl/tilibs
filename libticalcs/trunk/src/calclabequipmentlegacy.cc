/* Hey EMACS -*- linux-c -*- */

/*  libticalcs2 - hand-helds support library, a part of the TILP project
 *  Copyright (C) 2019-2026  Lionel Debroux
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
#include <clocale>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "cmd68k.h"
#include "cmdz80.h"

/*
	Communication with Vernier CBL / CBR / CBL2 / CBR2 / LabPro over legacy I/O.
*/

// Send a legacy lab equipment (CBL / CBR / CBL2 / CBR2 / LabPro) control command, i.e. a "{...}" list,
// through the supplied send routine (TI-68k packet format, or the LabPro's USB ASCII protocol).
static int tixx_send_lab_equipment_list_command(CalcHandle* handle, CalcModel model, const char * command,
                                                int (*send)(CalcHandle *, CalcModel, CalcLabEquipmentData *))
{
	CalcLabEquipmentData data;
	ticalcs_fill_lab_equipment_data(&data, CALC_LAB_EQUIPMENT_DATA_TYPE_STRING, (uint16_t)(strlen(command) + 1), 0, (const uint8_t *)command, nullptr, 4, 0, 0);
	return send(handle, model, &data);
}

// Format a floating-point number into @buf, using '.' as the decimal point regardless of the locale,
// so that the resulting "{...}" command is accepted by the list parser.
static void tixx_format_period(char * buf, size_t buflen, double period)
{
	const char * decimal_point = localeconv()->decimal_point;
	const size_t decimal_point_len = strlen(decimal_point);
	ticalcs_slprintf(buf, buflen, "%g", period);
	char * p = strstr(buf, decimal_point);
	if (nullptr != p && (decimal_point_len != 1 || decimal_point[0] != '.'))
	{
		memmove(p + 1, p + decimal_point_len, strlen(p + decimal_point_len) + 1);
		*p = '.';
	}
}

// Shared control logic for the lab equipments that take "{...}" list commands. The actual transmission
// is delegated to @send (the legacy DBUS routine, or the LabPro's USB ASCII one). Command 0 resets the
// device, Command 3 (sample/trigger setup) arms data collection, Command 6 (system command 0) aborts
// sampling, and Command 1998 sets the LEDs (CBL2 and LabPro only).
int tixx_control_lab_equipment_impl(CalcHandle* handle, CalcModel model, CalcLabEquipmentParameters * params,
                                    int (*send)(CalcHandle *, CalcModel, CalcLabEquipmentData *))
{
	int ret = 0;
	char command[64];

	VALIDATE_HANDLE(handle);

	if (nullptr == params)
	{
		// Default: reset the device, then arm realtime data collection with the default sample time.
		ret = tixx_send_lab_equipment_list_command(handle, model, "{0}", send);
		if (!ret)
		{
			// Real-time mode is selected by a sample count of -1, except on the CBL2, which uses 0.1.
			ret = tixx_send_lab_equipment_list_command(handle, model, (model == CALC_CBL2) ? "{3,0.5,0.1,0,0,0,0,0,0,0}" : "{3,0.5,-1,0,0,0,0,0,0,0}", send);
		}
		return ret;
	}

	if (params->reset)
	{
		ret = tixx_send_lab_equipment_list_command(handle, model, "{0}", send);
	}
	if (!ret && params->led_color != 0xFF && (model == CALC_CBL2 || model == CALC_LABPRO || model == CALC_LABPRO_USB))
	{
		// Command 1998: LED 1 (red), 2 (yellow) or 3 (green); 0 = off, 1 = on.
		ticalcs_slprintf(command, sizeof(command), "{1998,%u,%u}", (params->led_color - 1) % 3 + 1, (params->led_brightness != 0xFF && params->led_brightness == 0) ? 0 : 1);
		ret = tixx_send_lab_equipment_list_command(handle, model, command, send);
	}
	if (!ret && (params->measurement_period != 0.0 || params->start_measurements))
	{
		// Command 3: sample/trigger setup, realtime mode; this arms data collection.
		const double period = (params->measurement_period != 0.0) ? params->measurement_period : 0.5;
		char period_buf[32];
		tixx_format_period(period_buf, sizeof(period_buf), period);
		if (model == CALC_CBL2)
		{
			ticalcs_slprintf(command, sizeof(command), "{3,%s,0.1,0,0,0,0,0,0,0}", period_buf);
		}
		else
		{
			ticalcs_slprintf(command, sizeof(command), "{3,%s,-1,0,0,0,0,0,0,0}", period_buf);
		}
		ret = tixx_send_lab_equipment_list_command(handle, model, command, send);
	}
	if (!ret && params->stop_measurements)
	{
		// Command 6, system command 0: abort sampling.
		ret = tixx_send_lab_equipment_list_command(handle, model, "{6,0}", send);
	}

	return ret;
}

int TICALL tixx_control_lab_equipment_legacy(CalcHandle* handle, CalcModel model, CalcLabEquipmentParameters * params)
{
	return tixx_control_lab_equipment_impl(handle, model, params, tixx_send_lab_equipment_data_legacy);
}

// Send a list through the z80-family packet flows, per the protocol captures in
// tixx_send_lab_equipment_data_legacy() below: RTS, ACK, CTS, ACK, XDP, ACK, EOT, ACK.
static int tixx_send_lab_equipment_data_z80ish(CalcHandle * handle, uint8_t target2,
                                               int (*send_rts)(CalcHandle *, uint16_t, uint8_t, const char *),
                                               uint16_t varsize, const uint8_t * data, const char * varname)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ret = send_rts(handle, varsize, target2, varname);
	if (!ret)
	{
		ret = tiz80_recv_ACK(handle, NULL);
		if (!ret)
		{
			ret = tiz80_recv_CTS(handle, 0);
			if (!ret)
			{
				ret = tiz80_send_ACK(handle, target2);
				if (!ret)
				{
					ticalcs_info("Sending %u-byte list payload", varsize);

					ret = tiz80_send_XDP(handle, varsize, (uint8_t *)data, target2);
					if (!ret)
					{
						ret = tiz80_recv_ACK(handle, NULL);
						if (!ret)
						{
							ret = tiz80_send_EOT(handle, target2);
							if (!ret)
							{
								ret = tiz80_recv_ACK(handle, NULL);
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

int TICALL tixx_send_lab_equipment_data_legacy(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret = 0;
	uint8_t target1;
	uint8_t target2;
	uint32_t size = 0;
	uint32_t items = 0;
	const uint8_t * ptr = nullptr;
	int needs_destruction = 0;
	uint8_t vartype;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(lab_equipment_data);

	target1 = ti68k_model_to_dbus_mid(model);
	target2 = tiz80_model_to_dbus_mid(model);

	if (target1 == DBUS_MID_PC_TIXX && target2 == DBUS_MID_PC_TIXX)
	{
		ticalcs_critical(_("Model %d is not known to support list data"), model);
		return ERR_INVALID_PARAMETER;
	}
	// target1 != 0 && target2 != 0 is an internal error.

	if (target1 != DBUS_MID_PC_TIXX && (   lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST
	                                    || lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST))
	{
		// Wrong format.
		ticalcs_warning("%s", _("Expected TI-68k list or string format with this calculator model"));
		return ERR_INVALID_PARAMETER;
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
	{
		// Wrong format.
		ticalcs_warning("%s", _("Expected TI-Z80 or TI-85/86 list or string format with this calculator model"));
		return ERR_INVALID_PARAMETER;
	}
	else if (target1 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
	{
		// Will use TI-68k packet format.
		vartype = lab_equipment_data->vartype;

		if (vartype != TI89_LIST)
		{
			ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
			return ERR_INVALID_PARAMETER;
		}

		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST)
	{
		// Will use TI-85/86 packet format: [number of elements as 16-bit LE], [10-byte real per element].
		vartype = lab_equipment_data->vartype;

		if (vartype != TI85_LIST) // Real list.
		{
			ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
			return ERR_INVALID_PARAMETER;
		}

		if (   nullptr == lab_equipment_data->data
		    || lab_equipment_data->size < 2
		    || (uint32_t)lab_equipment_data->size != 2 + (uint32_t)lab_equipment_data->items * 10)
		{
			ticalcs_critical("%s", _("Inconsistent size fields in lab equipment list data"));
			return ERR_INVALID_PARAMETER;
		}

		const uint16_t count = (uint16_t)(   lab_equipment_data->data[0]
		                                  | (((uint16_t)lab_equipment_data->data[1]) << 8));
		if (count != lab_equipment_data->items)
		{
			ticalcs_critical("%s", _("Inconsistent item count in lab equipment list data"));
			return ERR_INVALID_PARAMETER;
		}

		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST)
	{
		// Will use TI-Z80 packet format: [number of elements as 16-bit LE], [9-byte real per element].
		vartype = lab_equipment_data->vartype;

		if (vartype != TI83_LIST) // Real list.
		{
			ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
			return ERR_INVALID_PARAMETER;
		}

		if (   nullptr == lab_equipment_data->data
		    || lab_equipment_data->size < 2
		    || (uint32_t)lab_equipment_data->size != 2 + (uint32_t)lab_equipment_data->items * 9)
		{
			ticalcs_critical("%s", _("Inconsistent size fields in lab equipment list data"));
			return ERR_INVALID_PARAMETER;
		}

		const uint16_t count = (uint16_t)(   lab_equipment_data->data[0]
		                                  | (((uint16_t)lab_equipment_data->data[1]) << 8));
		if (count != lab_equipment_data->items)
		{
			ticalcs_critical("%s", _("Inconsistent item count in lab equipment list data"));
			return ERR_INVALID_PARAMETER;
		}

		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
	}
	else if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_STRING)
	{
		CalcLabEquipmentData converted;
		converted.type = (target1 != DBUS_MID_PC_TIXX) ? CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST
		                : ((target2 == DBUS_MID_PC_TI85 || target2 == DBUS_MID_PC_TI86) ? CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST
		                                                                                : CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST);
		ret = ticalcs_convert_lab_equipment_data_from_string(&converted, (const char *)lab_equipment_data->data);
		if (ret)
		{
			return ret;
		}

		needs_destruction = 1;
		vartype = converted.vartype;
		size = converted.size;
		items = converted.items;
		ptr = converted.data;
	}
	else
	{
		ticalcs_critical("This shouldn't occur, please report the issue");
		return ERR_INVALID_PARAMETER;
	}

	// The TI-68k wire format carries the item count right before the payload, and the XDP packet length is 16-bit.
	if (target1 != DBUS_MID_PC_TIXX && (size > 65535 || size < 4 || nullptr == ptr))
	{
		ticalcs_critical("%s", _("List data too large, will not be sent"));
		ret = ERR_INVALID_PARAMETER;
		goto end;
	}

	// Cross-check the item count embedded in the raw data against the @items field.
	if (target1 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
	{
		const uint32_t count = (uint32_t)ptr[0] | (((uint32_t)ptr[1]) << 8) | (((uint32_t)ptr[2]) << 16) | (((uint32_t)ptr[3]) << 24);
		if (count > 0xFFFF || (items != 0 && items != (uint16_t)count))
		{
			ticalcs_critical("%s", _("Inconsistent item count in lab equipment list data"));
			ret = ERR_INVALID_PARAMETER;
			goto end;
		}
	}

	if (target1 != DBUS_MID_PC_TIXX)
	{
// Packets for Send({7}) from a TI-89.
/*
89 06 08 00  03 00 00 00  04  01  FF  00  07 01
19 56 00 00
19 09 00 00
89 56 00 00
89 15 07 00  01 00 00 00  20 37 00  58 00
19 56 00 00
89 92 00 00
19 56 00 00
*/
		ret = ti68k_send_VAR_lab_equipment_data(handle, size - 4, vartype, target1);
		if (!ret)
		{
			ret = ti89_recv_ACK(handle, NULL);
			if (!ret)
			{
				ret = ti92_recv_CTS(handle);
				if (!ret)
				{
					ret = ti68k_send_ACK(handle, target1);
					if (!ret)
					{
						// The payload may not be NUL-terminated for caller-supplied raw data: print it in a bounded way.
						ticalcs_info("Sending \"%.*s\"", (int)((size - 4 < 64) ? size - 4 : 64), (const char *)ptr + 4);

						// The raw list data carries its own 32-bit little-endian item count, right before the payload.
						ret = ti68k_send_XDP(handle, size, (uint8_t *)ptr, target1);
						if (!ret)
						{
							ret = ti89_recv_ACK(handle, NULL);
							if (!ret)
							{
								ret = ti68k_send_EOT(handle, target1);
								if (!ret)
								{
									ret = ti89_recv_ACK(handle, NULL);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if (model == CALC_TI73)
		{
			// Uses the TI-73 / TI-82 / TI-83 / TI-83+ packet format, with TI-73 list numbering (∟1).
// Packets for Send({7}) from a TI-73:
/*
82 C9 0B 00  0B 00 01 5D 00 00 00 00 00 00 00  69 00
12 56 00 00
12 09 00 00
82 56 00 00
82 15 0B 00  01 00  00 80 70 00 00 00 00 00 00  F1 00
12 56 00 00
82 92 0B 00  [improper length]
12 56 00 00
*/
			const char * tok = (lab_equipment_data->varname != nullptr) ? ticonv_varname_tokenize(model, lab_equipment_data->varname, TI83_LIST) : nullptr;
			ret = tixx_send_lab_equipment_data_z80ish(handle, target2, ti73_send_RTS_lab_equipment_data, (uint16_t)size, ptr, tok);
			g_free((void *)tok);
		}
		else if (model == CALC_TI82 || model == CALC_TI83)
		{
			// Uses the TI-73 / TI-82 / TI-83 / TI-83+ packet format, with TI-82/83 list numbering (∟1).
// Packets for Send({7}) from a TI-82 or TI-83:
/*
82 C9 0B 00  0B 00 01 5D 01 00 00 00 00 00 00  6A 00
12 56 00 00
12 09 00 00
82 56 00 00
82 15 0B 00  01 00  00 80 70 00 00 00 00 00 00  F1 00
12 56 00 00
82 92 0B 00  [improper length]
12 56 00 00
*/
			const char * tok = (lab_equipment_data->varname != nullptr) ? ticonv_varname_tokenize(model, lab_equipment_data->varname, TI83_LIST) : nullptr;
			ret = tixx_send_lab_equipment_data_z80ish(handle, target2, ti8283_send_RTS_lab_equipment_data, (uint16_t)size, ptr, tok);
			g_free((void *)tok);
		}
		else if (model == CALC_TI85 || model == CALC_TI86)
		{
			// Uses the TI-85/86 packet format.
// Packets for Send({7}) from a TI-85 or TI-86:
/*
85 C9 0C 00  0C 00 04 01 42 00 00 00 00 00 00 00  53 00
15 56 00 00
15 09 00 00
85 56 00 00
85 15 0C 00  01 00  00 00 FC 70 00 00 00 00 00 00  6D 01
15 56 00 00
85 92 0C 00  [improper length]
15 56 00 00
*/
			const char * tok = (lab_equipment_data->varname != nullptr) ? ticonv_varname_tokenize(model, lab_equipment_data->varname, TI85_LIST) : nullptr;
			ret = tixx_send_lab_equipment_data_z80ish(handle, target2, ti8586_send_RTS_lab_equipment_data, (uint16_t)size, ptr, tok);
			g_free((void *)tok);
		}
		else
		{
			// Uses the newer TI-Z80 packet format.
// Packets for Send({7}) from a 83+-class calculator:
/*
82 C9 0B 00  0B 00 01 24 00 00 00 00 00 00 00  30 00
12 56 00 00
12 09 00 00
82 56 00 00
82 15 0B 00  01 00  00 80 70 00 00 00 00 00 00  F1 00
12 56 00 00
82 92 0B 00  [improper length]
12 56 00 00
*/
			const char * tok = (lab_equipment_data->varname != nullptr) ? ticonv_varname_tokenize(model, lab_equipment_data->varname, TI83_LIST) : nullptr;
			ret = tixx_send_lab_equipment_data_z80ish(handle, target2, tiz80_send_RTS_lab_equipment_data, (uint16_t)size, ptr, tok);
			g_free((void *)tok);
		}
	}

end:
	if (needs_destruction)
	{
		ticalcs_free_lab_equipment_data_related((void *)ptr);
	}

	return ret;
}

// Get a list from a calculator through the z80-family packet flows, per the protocol captures
// in tixx_get_lab_equipment_data_legacy() below: REQ, ACK, VAR, ACK, CTS, ACK, XDP, ACK.
// The VAR header is parsed directly rather than through the family-specific recv_VAR wrappers:
// the captured headers are 5-byte length-prefixed forms even on TI-73/83/83+ calculators, which
// ti73_recv_VAR() would reject. @realsize is 9 for the TI-82 flow and 10 for the other flows,
// matching the element sizes seen in the captures; @out_type/@out_vartype describe the received
// raw list data.
static int tixx_get_lab_equipment_data_z80ish(CalcHandle * handle, uint8_t target2,
                                              int (*send_req)(CalcHandle *, uint8_t, const char *),
                                              int (*recv_xdp)(CalcHandle *, uint16_t *, uint8_t *),
                                              uint16_t realsize, CalcLabEquipmentDataType out_type,
                                              uint8_t out_vartype, const char * varname,
                                              CalcLabEquipmentData * lab_equipment_data)
{
	int ret;
	uint8_t host = 0, cmd = 0;
	uint16_t length = 0;
	uint16_t varsize = 0;
	uint8_t * buffer;

	VALIDATE_HANDLE(handle);

	buffer = (uint8_t *)handle->buffer;

	do
	{
		ret = send_req(handle, target2, varname);
		if (ret) break;

		ret = tiz80_recv_ACK(handle, NULL);
		if (ret) break;

		ret = dbus_recv(handle, &host, &cmd, &length, buffer);
		if (ret) break;

		if (cmd == DBUS_CMD_SKP)
		{
			ret = ERR_VAR_REJECTED;
			break;
		}
		else if (cmd != DBUS_CMD_VAR)
		{
			ret = ERR_INVALID_CMD;
			break;
		}
		else if (length < 3) // need at least the 16-bit size and the type byte
		{
			ticalcs_critical("%s", _("Invalid variable header returned by the lab equipment"));
			ret = ERR_INVALID_PACKET;
			break;
		}
		varsize = (uint16_t)(buffer[0] | (((uint16_t)buffer[1]) << 8));

		ret = tiz80_send_ACK(handle, target2);
		if (ret) break;

		ret = tiz80_send_CTS(handle, target2);
		if (ret) break;

		ret = tiz80_recv_ACK(handle, NULL);
		if (ret) break;

		ret = recv_xdp(handle, &length, (uint8_t *)handle->buffer2);
		if (ret) break;

		const int size_mismatch = (varsize != length);
		if (size_mismatch)
		{
			ticalcs_critical("%s", _("Expected declared packet sizes to match"));
			ret = ERR_INVALID_PACKET;
		}

		// Complete the handshake with an ACK, but preserve a size mismatch error.
		const int ack_ret = tiz80_send_ACK(handle, target2);
		if (!ret)
		{
			ret = ack_ret;
		}
		if (ret || size_mismatch)
		{
			break;
		}

		const uint8_t * payload = (const uint8_t *)handle->buffer2;
		const uint16_t count = (uint16_t)(payload[0] | (((uint16_t)payload[1]) << 8));
		if ((uint32_t)length != 2 + (uint32_t)count * realsize || count >= 32768)
		{
			ticalcs_critical("%s", _("Invalid list data returned by the lab equipment"));
			ret = ERR_INVALID_PACKET;
			break;
		}

		uint8_t * copy = (uint8_t *)g_malloc(length);
		if (nullptr == copy)
		{
			ret = ERR_MALLOC;
			break;
		}
		memcpy(copy, handle->buffer2, length);
		ticalcs_fill_lab_equipment_data(lab_equipment_data, out_type, length, count, copy, nullptr, out_vartype, 0, 0);
	}
	while(0);

	return ret;
}

int TICALL tixx_get_lab_equipment_data_legacy(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret;
	uint8_t target1;
	uint8_t target2;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(lab_equipment_data);


	target1 = ti68k_model_to_dbus_mid(model);
	target2 = tiz80_model_to_dbus_mid(model);

	if (target1 == DBUS_MID_PC_TIXX && target2 == DBUS_MID_PC_TIXX)
	{
		ticalcs_critical(_("Model %d is not known to support list data"), model);
		return ERR_INVALID_PARAMETER;
	}
	// target1 != 0 && target2 != 0 is an internal error.

	if (target1 != DBUS_MID_PC_TIXX)
	{
		// Will use TI-68k packet format.
// Packets for the Get corresponding to Send({7})
/*
89 A2 06 00  00 00 00 00 04 00  04 00
19 56 00 00
19 06 06 00  E2 00 00 00 04 00  E6 00
89 56 00 00
89 09 00 00
19 56 00 00

19 15 e2 00  11 00 00 00
20 2b 35 2e 30 31 31 33 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 38 2e 38 38 38 30 30 45 2b 30 33
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 31 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30
20 2b 30 2e 30 30 30 30 30 45 2b 30 30  00  38 29
89 56 00 00
*/
		if (lab_equipment_data->vartype != TI89_LIST)
		{
			ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
			return ERR_INVALID_PARAMETER;
		}

		do
		{
			uint32_t varsize;
			uint8_t vartype;
			char varname[256 + 1];
			uint16_t length;
			uint8_t * ptr;
			int size_mismatch;

			ret = ti92_send_REQ(handle, 0, TI89_LIST, (nullptr != lab_equipment_data->varname) ? lab_equipment_data->varname : "");
			if (ret) break;

			ret = ti89_recv_ACK(handle, NULL);
			if (ret) break;

			varname[0] = 0;
			ret = ti92_recv_VAR(handle, &varsize, &vartype, varname);
			if (ret) break;
			if (vartype != 4)
			{
				ticalcs_critical("Unhandled VAR data format");
				ret = ERR_INVALID_PACKET;
				break;
			}

			ret = ti68k_send_ACK(handle, target1);
			if (ret) break;

			ret = ti68k_send_CTS(handle, target1);
			if (ret) break;

			ret = ti89_recv_ACK(handle, NULL);
			if (ret) break;

			ret = ti68k_recv_XDP(handle, &length, (uint8_t *)handle->buffer2);
			if (ret) break;
			size_mismatch = (varsize != (uint32_t)length);
			if (size_mismatch)
			{
				ticalcs_critical("%s", _("Expected declared packet sizes to match"));
				ret = ERR_INVALID_PACKET;
			}

			// Complete the handshake with an ACK, but preserve a size mismatch error.
			const int ack_ret = ti68k_send_ACK(handle, target1);
			if (!ret)
			{
				ret = ack_ret;
			}
			if (ret) break;

			if (size_mismatch)
			{
				break;
			}

			ptr = (uint8_t *)handle->buffer2;
			// The items count is refined below, once the payload checks have passed.
			ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, (uint16_t)varsize, ptr[0] | (((uint16_t)ptr[1]) << 8), nullptr, nullptr, 4, 0, 0);
			if (ptr[2] != 0 || ptr[3] != 0 || varsize < 4)
			{
				// The 32-bit item count doesn't fit in the 16-bit @items field, or the packet is too small.
				ticalcs_critical("%s", _("Invalid list data returned by the lab equipment"));
				ret = ERR_INVALID_PACKET;
			}
			else if (lab_equipment_data->items >= 32768)
			{
				ret = ERR_INVALID_PACKET;
			}
			else
			{
				ptr = (uint8_t *)g_malloc(varsize);
				if (nullptr != ptr)
				{
					memcpy(ptr, handle->buffer2, varsize);
					lab_equipment_data->data = ptr;
				}
				else
				{
					ret = ERR_MALLOC;
				}
			}
		}
		while(0);
	}
	else
	{
		if (model == CALC_TI82)
		{
			if (lab_equipment_data->vartype != TI82_LIST)
			{
				ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
				return ERR_INVALID_PARAMETER;
			}

// Packets for the Get(L1 corresponding to Send({7 from a TI-82:
/*
82 A2 0B 00  0B 00 00 5D 00 00 00 00 00 00 00  68 00
12 56 00 00
12 06 0B 00  9B 00 01 5D 00 00 00 00 00 00 00  F9 00
82 56 0B 00  [improper length]
82 09 0B 00  [improper length]
12 56 00 00
12 15 9B 00  11 00
00 80 50 11 30 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 83 88 88 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 10 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00
00 80 00 00 00 00 00 00 00  45 0a
82 56 9B 00  [improper length]
*/
			ret = tixx_get_lab_equipment_data_z80ish(handle, target2, ti8283_send_REQ_lab_equipment_data, ti82_recv_XDP,
			                                        9, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, TI82_LIST,
			                                        lab_equipment_data->varname, lab_equipment_data);
		}
		else if (model == CALC_TI85 || model == CALC_TI86)
		{
			if (lab_equipment_data->vartype != TI85_LIST)
			{
				ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
				return ERR_INVALID_PARAMETER;
			}

			// Uses the TI-85/86 packet format.
// Packets for the Get(L1 (hopefully) corresponding to Send({7 from a TI-85 or TI-86:
/*
85 A2 0C 00  0C 00 04 01 43 00 00 00 00 00 00 00  54 00
15 56 00 00
15 06 05 00  AC 00 04 01 43  F4 00
85 56 05 00  [improper length]
85 09 05 00  [improper length]
15 56 00 00
15 15 AC 00  11 00
00 00 FC 50 11 30 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 03 FC 88 88 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 10 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00  81 12
85 56 AC 00  [improper length]
*/
			ret = tixx_get_lab_equipment_data_z80ish(handle, target2, ti8586_send_REQ_lab_equipment_data, ti85_recv_XDP,
			                                        10, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, TI85_LIST,
			                                        lab_equipment_data->varname, lab_equipment_data);
		}
		else
		{
			if (lab_equipment_data->vartype != TI83_LIST)
			{
				ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
				return ERR_INVALID_PARAMETER;
			}

			// Uses the TI-73 / TI-83 / TI-83+ packet format.
// Packets for the Get(L1 corresponding to Send({7 from a TI-83:
/*
95 A2 0B 00  0B 00 04 01 41 00 00 00 00 00 00  51 00
15 56 00 00
15 06 05 00  AC 00 04 01 41  F2 00
95 56 05 00  [improper length]
95 09 05 00  [improper length]
15 56 00 00
15 15 AC 00  11 00
00 00 FC 50 11 30 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 03 FC 88 88 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 10 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00  81 12
95 56 AC 00  [improper length]
*/
// Packets for the Get(L1 corresponding to Send({7 from a TI-73 or a TI-83+:
/*
95 A2 0B 00  0B 00 04 01 41 00 00 00 00 00 00  51 00
15 56 00 00
15 06 05 00  AC 00 04 01 41  F2 00
95 56 00 00
95 09 00 00
15 56 00 00
15 15 AC 00  11 00
00 00 FC 50 11 30 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 03 FC 88 88 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 10 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00
00 00 FC 00 00 00 00 00 00 00  81 12
95 56 00 00
*/
			// IMPORTANT NOTE: per the packet captures above, the Get direction of these models carries
			// 10-byte elements (i.e. the TI-85/86 element layout), unlike the Send direction
			// which uses the 9-byte form; hence the TI8586_RAW_LIST tag below.
			ret = tixx_get_lab_equipment_data_z80ish(handle, target2, ti73_send_REQ_lab_equipment_data, ti73_recv_XDP,
			                                        10, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, TI83_LIST,
			                                        lab_equipment_data->varname, lab_equipment_data);
		}
	}

	return ret;
}
