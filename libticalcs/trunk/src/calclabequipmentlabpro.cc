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
	Communication with Vernier LabPro over direct USB.
*/

// The LabPro speaks a variant of its RS-232 ASCII command protocol over USB, seen in the LabPro Technical Manual,
// "USB Communication Details" section: VID 0x08F7, PID 0x0001, bulk transfers in multiples of 64 bytes, NAK-blocking reads.

// LabPro: send an ASCII command, i.e. "s{...}<CR>", or raw bytes (binary mode, unframed).

#define LABPRO_BUFFER_SIZE (300)

int TICALL tixx_control_lab_equipment_labpro_usb(CalcHandle* handle, CalcModel model, CalcLabEquipmentParameters * params)
{
	return tixx_control_lab_equipment_impl(handle, model, params, tixx_send_lab_equipment_data_labpro_usb);
}

int TICALL tixx_send_lab_equipment_data_labpro_usb(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	(void)model; // Dialect is irrelevant for this protocol.

	int ret = ERR_INVALID_PARAMETER;

	VALIDATE_NONNULL(lab_equipment_data);

	if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_STRING)
	{
		const uint8_t * data = lab_equipment_data->data;
		size_t len;
		uint8_t buf[LABPRO_BUFFER_SIZE];
		if (nullptr != data)
		{
			// The string data usually carries its own terminating NUL, like
			// #ticalcs_calc_send_lab_equipment_datastr() provides: don't rely on it, bound by @size.
			len = (lab_equipment_data->size > 0 && data[lab_equipment_data->size - 1] == 0) ? (size_t)(lab_equipment_data->size - 1) : (size_t)lab_equipment_data->size;
			if (len <= LABPRO_BUFFER_SIZE - 2)
			{
				buf[0] = 's';
				memcpy(buf + 1, data, len);
				buf[len + 1] = '\r';
				ticalcs_info("Sending \"%.*s\"", (int)((len < 64) ? len : 64), buf + 1);
				ret = ticables_cable_send(handle->cable, buf, len + 2);
			}
			else
			{
				ticalcs_critical("%s", _("LabPro command too long, will not be sent"));
			}
		}
		else
		{
			ticalcs_critical("%s", _("No lab equipment data to send"));
		}
	}
	else if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_RAW)
	{
		// Binary mode: the data is sent unframed.
		if (nullptr != lab_equipment_data->data && lab_equipment_data->size > 0)
		{
			ret = ticables_cable_send(handle->cable, (uint8_t *)lab_equipment_data->data, lab_equipment_data->size);
		}
		else
		{
			ticalcs_critical("%s", _("No lab equipment data to send"));
		}
	}
	else
	{
		ticalcs_critical("%s", _("Expected string or raw data with this lab equipment"));
	}

	return ret;
}

// LabPro: request data with "g" and read the response: an ASCII list, or raw bytes (binary mode).
int TICALL tixx_get_lab_equipment_data_labpro_usb(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * lab_equipment_data)
{
	int ret;
	char * resp_str = nullptr;
	uint8_t * resp_raw = nullptr;
	uint8_t request = 'g';

	VALIDATE_NONNULL(lab_equipment_data);

	ret = ticables_cable_send(handle->cable, &request, 1);
	if (!ret)
	{
		if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_STRING)
		{
			// The LabPro always sends transfers in multiples of 64 bytes: read 64-byte packets until the
			// terminating carriage return, with an overall bound of 64 KB.
			size_t offset = 0;
			resp_str = (char *)g_malloc(65536);
			if (nullptr != resp_str)
			{
				do
				{
					uint8_t pkt[64];
					ret = ticables_cable_recv(handle->cable, pkt, sizeof(pkt));
					if (!ret)
					{
						if (offset + sizeof(pkt) > 65535)
						{
							ticalcs_critical("%s", _("Oversized response from lab equipment"));
							ret = ERR_INVALID_PACKET;
							break;
						}
						memcpy(resp_str + offset, pkt, sizeof(pkt));
						offset += sizeof(pkt);
					}
				}
				while (!ret && nullptr == memchr(resp_str, '\r', offset));
				if (!ret)
				{
					resp_str[offset] = 0;
					// Strip the trailing carriage return.
					if (offset > 0 && resp_str[offset - 1] == '\r')
					{
						resp_str[offset - 1] = 0;
					}
					ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_STRING, (uint16_t)strlen(resp_str), 0, (const uint8_t *)resp_str, nullptr, 0, 0, 0);
					resp_str = nullptr; // Ownership transferred to lab_equipment_data.
				}
			}
			else
			{
				ret = ERR_MALLOC;
			}
		}
		else if (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_RAW)
		{
			// Binary mode: the caller announces the expected number of bytes through size.
			resp_raw = (uint8_t *)g_malloc(lab_equipment_data->size);
			if (nullptr != resp_raw)
			{
				ret = ticables_cable_recv(handle->cable, resp_raw, lab_equipment_data->size);
				if (!ret)
				{
					ticalcs_fill_lab_equipment_data(lab_equipment_data, CALC_LAB_EQUIPMENT_DATA_TYPE_RAW, lab_equipment_data->size, 0, (const uint8_t *)resp_raw, nullptr, 0, 0, 0);
					resp_raw = nullptr; // Ownership transferred to lab_equipment_data.
				}
			}
			else
			{
				ret = ERR_MALLOC;
			}
		}
		else
		{
			ticalcs_critical("%s", _("Expected string or raw data with this lab equipment"));
			ret = ERR_INVALID_PARAMETER;
		}
	}

	g_free(resp_str);
	g_free(resp_raw);

	return ret;
}

extern const CalcFncts calc_labpro_usb =
{
	CALC_LABPRO_USB,
	"LABPRO_USB",
	"LabPro (USB)",
	"LabPro (USB)",
	OPS_LABEQUIPMENTDATA |
	/* OPS_VERSION*/ /*| OPS_OS*/
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
	&noop_get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_control_lab_equipment_labpro_usb,
	&tixx_send_lab_equipment_data_labpro_usb,
	&tixx_get_lab_equipment_data_labpro_usb
};
