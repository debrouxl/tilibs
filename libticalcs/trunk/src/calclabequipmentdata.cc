/* Hey EMACS -*- linux-c -*- */

/*  libticalcs2 - hand-helds support library, a part of the TILP project
 *  Copyright (C) 2019  Lionel Debroux
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
	Send({...}) / Get <varname> support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <clocale>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "cmd68k.h"
#include "cmdz80.h"

/////////////----------------

int tixx_convert_lab_equipment_data_string_to_ti8586_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	const int ret = ERR_UNSUPPORTED;

	return ret;
}

int tixx_convert_lab_equipment_data_string_to_tiz80_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	const int ret = ERR_UNSUPPORTED;

	return ret;
}

int tixx_convert_lab_equipment_data_string_to_ti68k_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data)
{
	int ret;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	uint8_t* ptr = (uint8_t*)strdup(lab_equipment_data);
	if (nullptr == ptr)
	{
		return ERR_MALLOC;
	}
	//fprintf(stderr, "%s\n", ptr);

	// Parse a reduced version of the original input form, i.e. the {...} inside Send(), without spaces or anything else (for now), and produce the
	// [number of elements as 32-bit LE], [0x20 header], [characters making up the list with ',' replaced by ' '], [trailing 0x00]
	// form which is transmitted on the wire by at least the 89, 92+, V200 and 89T (not checked the 92) for integer items.
	// Simplified grammar:
	// LIST: '{' BODY '}'
	// BODY: NUMBER | (NUMBER ',')+ NUMBER
	// NUMBER: [0-9]+
	//
	// TODO perform more RE of the protocol using TIEmu, especially for floating-point numbers.
	// TODO FIXME parse floating-point numbers as well.
	// TODO FIXME convert leading - to unary minus, if necessary.
	unsigned int state = 0;
	uint32_t items = 0;
	const uint8_t* orig_ptr = ptr;
	do
	{
		const uint8_t c = *ptr;
		switch (state)
		{
			// We'd need more states if we wanted to cope with spaces before and after '{', ',' and '}'.
			case 0:
				if (c == '{') { *ptr = ' '; state = 1; }           // Read leading {, turn it into space and go on.
				else state = 4;                                    // else error
				break;
			case 1:
				if (c >= '0' && c <= '9') { items++; state = 2; }  // Read first digit, try to read more digits.
				else state = 4;                                    // else error
				break;
			case 2:
				if (c >= '0' && c <= '9') state = 2;               // Read more digits, try to read yet more digits.
				else if (c == ',') { *ptr = ' '; state = 1; }      // Read comma, turn it into space and go on.
				else if (c == '}') { *ptr =   0; state = 3; }      // Read trailing }, success
				else state = 4;                                    // else error.
				break;
		}
		ptr++;
	} while (state != 3 && state != 4);

	if (state == 3)
	{
		ticalcs_info("%s", _("Successfully parsed lab equipment data list string"));
		out_data->type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
		out_data->size = ptr - orig_ptr;
		out_data->items = items;
		out_data->data = (const uint8_t *)orig_ptr;
		out_data->vartype = 4; // List.
		ret = 0;
	}
	else
	{
		ticalcs_warning("%s", _("Failed to parse lab equipment data list string"));
		ret = ERR_INVALID_PACKET;
	}

	return ret;
}

int tixx_convert_lab_equipment_data_ti8586_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, double ** raw_values, const char ** out_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	const int ret = ERR_UNSUPPORTED;

	return ret;
}

int tixx_convert_lab_equipment_data_tiz80_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, double ** raw_values, const char ** out_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	const int ret = ERR_UNSUPPORTED;

	return ret;
}

int tixx_convert_lab_equipment_data_ti68k_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, double ** raw_values, const char ** out_data)
{
	int ret;
	uint16_t items;
	const uint8_t * ptr;
	uint8_t * ptr2;
	char * deststr = nullptr;
	size_t deststrsize;
	size_t offset;
	double * fpvals = nullptr;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	*item_count = 0;
	*raw_values = nullptr;
	*out_data = nullptr;

	items = lab_equipment_data->items;
	// A list of 32K items would be over 64 KB, which is invalid, due to max memory block / variable size.
	if (items >= 32768)
	{
		return ERR_INVALID_PARAMETER;
	}
	else if (items == 0)
	{
		deststr = strdup("{}");
		if (nullptr == deststr)
		{
			return ERR_MALLOC;
		}
		*item_count = items;
		// *raw_values already set above.
		*out_data = deststr;
		return 0;
	}

	const struct lconv* lc = localeconv();
	const char* decimal_point_str = lc->decimal_point;
	const size_t decimal_point_len = strlen(decimal_point_str);

	ptr = lab_equipment_data->data + 4;
	deststrsize = strlen((const char*)ptr);
	deststrsize += items * (decimal_point_len - 1);
	deststr = (char *)malloc(deststrsize + 1);
	if (nullptr == deststr)
	{
		return ERR_MALLOC;
	}
	strncpy(deststr, (const char *)ptr, deststrsize);
	deststr[deststrsize] = 0;

	fpvals = (double *)malloc(items * sizeof(double));
	if (nullptr == fpvals)
	{
		ret = ERR_MALLOC;
		goto err;
	}

	ptr2 = (uint8_t*)strchr(deststr, '.');
	// Need to replace the decimal point.
	if (decimal_point_str[0] != '.' || decimal_point_str[1] != 0)
	{
		while (nullptr != ptr2)
		{
			memmove(ptr2 + decimal_point_len, ptr2 + 1, (uint8_t *)deststr + deststrsize - ptr2 - 1);
			strncpy((char *)ptr2, decimal_point_str, decimal_point_len);
			ptr2 = (uint8_t *)strchr((const char *)ptr2 + 1, '.');
		}
		//fprintf(stdout, "MM \"%s\"\n", deststr);
	}

	ptr = (const uint8_t *)deststr;
	while (*ptr == ' ') ptr++;
	for (uint16_t i = 0; i < items && nullptr != ptr; i++)
	{
		char * endptr;
		errno = 0;
		fpvals[i] = strtod((const char *)ptr, &endptr);
		if (errno != 0)
		{
			ret = ERR_INVALID_PACKET;
			goto err;
		}
		ptr = (const uint8_t *)strchr((const char *)ptr + 1, ' ');
		//ticalcs_info("L %g \"%s\" \"%s\"", fpvals[i], endptr, ptr);
	}
	ret = 0;
	deststr[0] = '{';
	offset = 1;
	for (uint16_t i = 0; i < items; i++)
	{
		const int printed = sprintf(deststr + offset, "%g,", fpvals[i]);
		offset += printed;
	}
	deststr[offset - 1] = '}';
	deststr[offset] = 0;
	deststr = (char *)realloc(deststr, offset + 1);

	if (!ret)
	{
		deststr[offset] = '}';
		deststr[offset + 1] = 0;
		deststr = (char *)realloc(deststr, offset + 2);
		*item_count = items;
		*raw_values = fpvals;
		*out_data = deststr;
	}

	return ret;

err:
	free(fpvals);
	free(deststr);
	return ret;
}

void tixx_free_converted_lab_equipment_data_item(CalcLabEquipmentData * lab_equipment_data)
{
	if (nullptr != lab_equipment_data)
	{
		free((void *)(lab_equipment_data->data));
	}
}

void tixx_free_converted_lab_equipment_data_string(void * lab_equipment_data)
{
	free(lab_equipment_data);
}

void tixx_free_converted_lab_equipment_data_fpvals(double * raw_values)
{
	free(raw_values);
}

/////////////----------------

int tixx_send_lab_equipment_data(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret;
	uint32_t size = 0;
	uint32_t items = 0;
	const uint8_t * ptr = nullptr;
	int needs_destruction = 0;
	uint8_t vartype;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(lab_equipment_data);

	const uint8_t target1 = ti68k_model_to_dbus_mid(model);
	const uint8_t target2 = tiz80_model_to_dbus_mid(model);

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
		ret = ERR_INVALID_PARAMETER;
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
	{
		// Wrong format.
		ticalcs_warning("%s", _("Expected TI-Z80 or TI-85/86 list or string format with this calculator model"));
		ret = ERR_INVALID_PARAMETER;
	}
	else if (target1 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
	{
		// Will use TI-68k packet format.
		// TODO check data format and item count.
		ret = 0;
		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
		vartype = lab_equipment_data->vartype;

		if (vartype != 4)
		{
			ticalcs_critical("Variable type %u not supported for now", lab_equipment_data->vartype);
			return ERR_INVALID_PARAMETER;
		}
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST)
	{
		// Will use TI-85/86 packet format.
		// TODO check data format and item count.
		ret = 0;
		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
		vartype = lab_equipment_data->vartype;
	}
	else if (target2 != DBUS_MID_PC_TIXX && lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST)
	{
		// Will use TI-Z80 packet format.
		// TODO check data format and item count.
		ret = 0;
		size = lab_equipment_data->size;
		items = lab_equipment_data->items;
		ptr = lab_equipment_data->data;
		vartype = lab_equipment_data->vartype;
	}
	else if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_STRING)
	{
		CalcLabEquipmentData converted;
		if (target1 != DBUS_MID_PC_TIXX)
		{
			ret = tixx_convert_lab_equipment_data_string_to_ti68k_raw_list((const char *)lab_equipment_data->data, &converted);
		}
		else
		{
			if (target2 == DBUS_MID_PC_TI85 || target2 == DBUS_MID_PC_TI86)
			{
				ret = tixx_convert_lab_equipment_data_string_to_ti8586_raw_list((const char *)lab_equipment_data->data, &converted);
			}
			else
			{
				ret = tixx_convert_lab_equipment_data_string_to_tiz80_raw_list((const char *)lab_equipment_data->data, &converted);
			}
		}
		if (!ret)
		{
			needs_destruction = 1;
			size = converted.size;
			items = converted.items;
			ptr = converted.data;
			vartype = converted.vartype;
		}
	}
	else
	{
		ticalcs_critical("This shouldn't occur, please report the issue");
		ret = ERR_INVALID_PARAMETER;
	}

	if (size >= 65536)
	{
		ticalcs_critical("%s", _("List data too large, will not be sent"));
		ret = ERR_INVALID_PARAMETER;
	}

	if (ret)
	{
		goto end;
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
		do
		{
			ret = ti68k_send_VAR_lab_equipment_data(handle, size, vartype, target1);
			if (ret) break;

			ret = ti89_recv_ACK(handle, nullptr);
			if (ret) break;

			ret = ti92_recv_CTS(handle);
			if (ret) break;

			ret = ti68k_send_ACK(handle, target1);
			if (ret) break;

			ticalcs_info("Sending \"%s\"", ptr);

			// Build number of items as 32-bit little-endian.
			uint8_t* ptr2 = (uint8_t*)handle->buffer2;
			ptr2[0] =  items        & 0xFF;
			ptr2[1] = (items >>  8) & 0xFF;
			ptr2[2] = (items >> 16) & 0xFF;
			ptr2[3] = (items >> 24) & 0xFF;
			memcpy(ptr2 + 4, ptr, size);
			ret = ti68k_send_XDP(handle, size + 4, ptr2, target1);
			if (ret) break;

			ret = ti89_recv_ACK(handle, nullptr);
			if (ret) break;

			ret = ti68k_send_EOT(handle, target1);
			if (ret) break;

			ret = ti89_recv_ACK(handle, nullptr);
		}
		while(0);
	}
	else
	{
		if (model == CALC_TI73)
		{
			// Will use TI-73 / TI-82 / TI-83 / TI-83+ packet format with TI-73 list numbering.
			// TODO serialize data to newer TI-Z80 format.
			// TODO implement sending newer TI-Z80 format packets.
// Packets for Send({7 from a TI-73:
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
		}
		else if (model == CALC_TI82 || model == CALC_TI83)
		{
			// Will use TI-73 / TI-82 / TI-83 / TI-83+ packet format with TI-82/83 list numbering.
			// TODO serialize data to newer TI-Z80 format.
			// TODO implement sending newer TI-Z80 format packets.
// Packets for Send({7 from a TI-82 or TI-83:
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
		}
		else if (model == CALC_TI85 || model == CALC_TI86)
		{
			// Will use TI-85/86 packet format.
			// TODO serialize data to TI-85/86 format.
			// TODO implement sending TI-85/86 format packets.
// Packets for Send({7 from a TI-85 or TI-86:
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
			ret = ERR_UNSUPPORTED;
		}
		else
		{
			// Will use newer TI-Z80 packet format.
			// TODO serialize data to newer TI-Z80 format.
			// TODO implement sending newer TI-Z80 format packets.
// Packets for Send({7 from a 83+-class calculator.
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
			ret = ERR_UNSUPPORTED;
		}
	}

end:
	if (needs_destruction)
	{
		tixx_free_converted_lab_equipment_data_string((void *)ptr);
	}

	return ret;
}

int tixx_get_lab_equipment_data(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(lab_equipment_data);

	const uint8_t target1 = ti68k_model_to_dbus_mid(model);
	const uint8_t target2 = tiz80_model_to_dbus_mid(model);

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
		do
		{
			uint32_t varsize;
			uint8_t vartype;
			char varname[256 + 1];
			uint16_t length;

			ret = ti92_send_REQ(handle, 0, 0x4, "");
			if (ret) break;

			ret = ti89_recv_ACK(handle, nullptr);
			if (ret) break;

			varname[0] = 0;
			ret = ti92_recv_VAR(handle, &varsize, &vartype, varname);
			if (ret) break;
			if (vartype != 4 || varname[0] != 0)
			{
				ticalcs_critical("Unhandled VAR data format");
				break;
			}

			ret = ti68k_send_ACK(handle, target1);
			if (ret) break;

			ret = ti68k_send_CTS(handle, target1);
			if (ret) break;

			ret = ti89_recv_ACK(handle, nullptr);
			if (ret) break;

			ret = ti68k_recv_XDP(handle, &length, (uint8_t *)handle->buffer2);
			if (ret) break;
			if (varsize != (uint32_t) length)
			{
				ticalcs_critical("%s", _("Expected declared packet sizes to match"));
				ret = ERR_INVALID_PACKET;
			}

			ret = ti68k_send_ACK(handle, target1);
			if (ret) break;

			uint8_t* ptr = (uint8_t*)handle->buffer2;
			lab_equipment_data->type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
			lab_equipment_data->size = varsize;
			lab_equipment_data->items = ptr[0] | (((uint16_t)ptr[1]) << 8);
			lab_equipment_data->index = 0;
			lab_equipment_data->unknown = 0;
			lab_equipment_data->vartype = 4;
			if (lab_equipment_data->items < 32768)
			{
				ptr = (uint8_t *)malloc(varsize);
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
			else
			{
				ret = ERR_INVALID_PACKET;
			}
		}
		while(0);
	}
	else
	{
		if (model == CALC_TI82)
		{
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
		}
		else if (model == CALC_TI85 || model == CALC_TI86)
		{
			// Will use TI-85/86 packet format.
			// TODO implement code based on TilEm dumps.
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
			ret = ERR_UNSUPPORTED;
		}
		else
		{
			// Will use TI-73 / TI-83 / TI-83+ packet format.
			// TODO implement code based on TilEm + Wireshark USB dumps.
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
			ret = ERR_UNSUPPORTED;
		}
	}

	return ret;
}
