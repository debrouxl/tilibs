/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
 *  Copyright (C) 2006  Kevin Kofler
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
	TI89 Titanium support thru DirectUsb link.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "dusb_vpkt.h"
#include "dusb_cmd.h"

#include "rom89t.h"
#include "romdump.h"
#include "keys89.h"

// Screen coordinates of the Titanium
#define TI89T_ROWS			128
#define TI89T_COLS			240
#define TI89T_ROWS_VISIBLE	100
#define TI89T_COLS_VISIBLE	160

static int		is_ready	(CalcHandle* handle)
{
	int ret;
	static const DUSBModeSet mode = DUSB_MODE_NORMAL;

	ret = dusb_cmd_s_mode_set(handle, mode);
	if (!ret)
	{
		ret = dusb_cmd_r_mode_ack(handle);
	}

	return ret;
}

static int		send_key	(CalcHandle* handle, uint32_t key)
{
	int ret;

	PAUSE(25);	// this pause is needed between 2 keys
	ret = dusb_cmd_s_execute(handle, "", "", DUSB_EID_KEY, "", (uint16_t)key);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char *args)
{
	uint8_t action;
	int ret;

	switch (ve->type)
	{
		case TI89t_ASM:  action = DUSB_EID_ASM; break;
		case TI89t_APPL: action = DUSB_EID_APP; break;
		default:         action = DUSB_EID_PRGM; break;
	}

	ret = dusb_cmd_s_execute(handle, ve->folder, ve->name, action, args, 0);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	static const uint16_t pid[] = { DUSB_PID_SCREENSHOT };
	const int size = 1;
	DUSBCalcParam **param;
	int ret;

	*bitmap = (uint8_t *)ticalcs_alloc_screen(TI89T_COLS * TI89T_ROWS / 8);
	if (*bitmap == NULL)
	{
		return ERR_MALLOC;
	}

	sc->width = TI89T_COLS;
	sc->height = TI89T_ROWS;
	sc->clipped_width = TI89T_COLS_VISIBLE;
	sc->clipped_height = TI89T_ROWS_VISIBLE;
	sc->pixel_format = CALC_PIXFMT_MONO;

	param = dusb_cp_new_array(handle, size);
	ret = dusb_cmd_s_param_request(handle, size, pid);
	while (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, size, param);
		if (!ret)
		{
			if (!param[0]->ok || param[0]->size != TI89T_COLS * TI89T_ROWS / 8)
			{
				ticalcs_free_screen(*bitmap);
				ret = ERR_INVALID_PACKET;
				break;
			}

			memcpy(*bitmap, param[0]->data, TI89T_COLS * TI89T_ROWS / 8);

			// Clip the unused part of the screen (nevertheless usable with asm programs)
			if (sc->format == SCREEN_CLIPPED)
			{
				int i, j, k;

				for (i = 0, j = 0; j < TI89T_ROWS_VISIBLE; j++)
				{
					for (k = 0; k < (TI89T_COLS_VISIBLE >> 3); k++)
					{
						(*bitmap)[i++] = (*bitmap)[j * (TI89T_COLS >> 3) + k];
					}
				}
			}
		}
		break;
	}
	dusb_cp_del_array(handle, param, size);

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	static const uint16_t aids[] = { DUSB_AID_VAR_TYPE, DUSB_AID_ARCHIVED, DUSB_AID_4APPVAR, DUSB_AID_VAR_SIZE, DUSB_AID_LOCKED, DUSB_AID_UNKNOWN_42 };
	const int size = sizeof(aids) / sizeof(uint16_t);
	int ret;
	DUSBCalcAttr **attr;
	GNode *root, *folder = NULL;
	char fldname[40];
	char varname[40];
	char folder_name[40] = "";
	char *u1, *u2;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}

	root = dirlist_create_append_node(NULL, apps);
	if (!root)
	{
		return ERR_MALLOC;
	}

	ret = dusb_cmd_s_dirlist_request(handle, size, aids);
	if (!ret)
	{
		for (;;)
		{
			VarEntry *ve;
			GNode *node;

			attr = dusb_ca_new_array(handle, size);
			ret = dusb_cmd_r_var_header(handle, fldname, varname, attr);
			if (ret)
			{
				// Not a real error.
				if (ret == ERR_EOT)
				{
					ret = 0;
				}
				dusb_ca_del_array(handle, attr, size);
				break;
			}

			ve = tifiles_ve_create();
			ticalcs_strlcpy(ve->folder, fldname, sizeof(ve->folder));
			ticalcs_strlcpy(ve->name, varname, sizeof(ve->name));
			ve->size = (  (((uint32_t)(attr[3]->data[0])) << 24)
				    | (((uint32_t)(attr[3]->data[1])) << 16)
				    | (((uint32_t)(attr[3]->data[2])) <<  8)
				    | (((uint32_t)(attr[3]->data[3]))      ));
			ve->type = (uint32_t)(attr[0]->data[3]);
			ve->attr = attr[1]->data[0] ? ATTRB_ARCHIVED : attr[4]->data[0] ? ATTRB_LOCKED : ATTRB_NONE;
			dusb_ca_del_array(handle, attr, size);

			if (ve->type == TI89_DIR)
			{
				ticalcs_strlcpy(folder_name, ve->folder, sizeof(folder_name));
				ticalcs_strlcpy(ve->name, ve->folder, sizeof(ve->name));
				ve->folder[0] = 0;

				folder = dirlist_create_append_node(ve, vars);
				if (!folder)
				{
					ret = ERR_MALLOC;
					break;
				}
			}
			else
			{
				if (!strcmp(ve->folder, "main") && (!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
				{
					tifiles_ve_delete(ve);
				}
				else
				{
					node = dirlist_create_append_node(ve, (ve->type != TI89_APPL) ? &folder : &root);
					if (!node)
					{
						ret = ERR_MALLOC;
						break;
					}
				}
			}
	/*
			ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"),
				ve->name,
				tifiles_vartype2string(handle->model, ve->type),
				ve->attr,
				ve->size);
	*/
			if (NULL != folder)
			{
				u1 = ticonv_varname_to_utf8(handle->model, ((VarEntry *) (folder->data))->name, -1);
				u2 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
				ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s/%s"), u1, u2);
				ticonv_utf8_free(u2);
				ticonv_utf8_free(u1);
				ticalcs_update_label(handle);
			}
		}
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	static const uint16_t pids[] = { DUSB_PID_FREE_RAM, DUSB_PID_FREE_FLASH };
	const int size = sizeof(pids) / sizeof(uint16_t);
	DUSBCalcParam **params;
	int ret;

	params = dusb_cp_new_array(handle, size);
	if (params != NULL)
	{
		ret = dusb_cmd_s_param_request(handle, size, pids);
		if (!ret)
		{
			ret = dusb_cmd_r_param_data(handle, size, params);
			if (!ret)
			{
				if (params[0]->ok && params[0]->size == 8 && params[1]->ok && params[1]->size == 8)
				{
					*ram = (  (((uint32_t)(params[0]->data[4])) << 24)
						| (((uint32_t)(params[0]->data[5])) << 16)
						| (((uint32_t)(params[0]->data[6])) <<  8)
						| (((uint32_t)(params[0]->data[7]))      ));
					*flash = (  (((uint32_t)(params[1]->data[4])) << 24)
						  | (((uint32_t)(params[1]->data[5])) << 16)
						  | (((uint32_t)(params[1]->data[6])) <<  8)
						  | (((uint32_t)(params[1]->data[7]))      ));
				}
				else
				{
					ret = ERR_INVALID_PACKET;
				}
			}
		}
	}
	else
	{
		ret = ERR_MALLOC;
	}
	dusb_cp_del_array(handle, params, size);

	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	unsigned int i;
	int ret = 0;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++)
	{
		DUSBCalcAttr **attrs;
		const int nattrs = 4;
		VarEntry * entry = content->entries[i];
		uint32_t pkt_size;
		uint32_t size;
		char varname[18];

		if (!ticalcs_validate_varentry(entry))
		{
			ticalcs_critical("%s: skipping invalid content entry #%u", __FUNCTION__, i);
			continue;
		}

		if (entry->action == ACT_SKIP)
		{
			ticalcs_info("%s: skipping variable #%u because requested", __FUNCTION__, i);
			continue;
		}

		if (entry->folder[0])
		{
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}
		else
		{
			ticalcs_strlcpy(varname, entry->name, sizeof(varname));
		}

		ticonv_varname_to_utf8_sn(handle->model, varname, handle->updat->text, sizeof(handle->updat->text), entry->type);
		ticalcs_update_label(handle);

		attrs = dusb_ca_new_array(handle, nattrs);
		attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = entry->type;
		attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
		attrs[1]->data[0] = entry->attr == ATTRB_ARCHIVED ? 1 : 0;
		attrs[2] = dusb_ca_new(handle, DUSB_AID_VAR_VERSION, 4);
		attrs[2]->data[0] = 0;
		attrs[3] = dusb_ca_new(handle, DUSB_AID_LOCKED, 1);
		attrs[3]->data[0] = entry->attr == ATTRB_LOCKED ? 1 : 0;

		size = entry->size;
		if (entry->size >= 65536U)
		{
			ticalcs_critical("%s: variable size %u is suspiciously large", __FUNCTION__, size);
		}

		if (!(size & 1))
		{
			ret = is_ready(handle);
		}

		if (ret)
		{
			dusb_ca_del_array(handle, attrs, nattrs);
			break;
		}

		ret = dusb_cmd_s_rts(handle, entry->folder, entry->name, size, nattrs, CA(attrs));
		dusb_ca_del_array(handle, attrs, nattrs);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}
		/*
			When sending variables with an odd varsize, bufer has to be negotiatied again with an even value.
			Moreover, buffer has to be smaller. Ti-Connect always use 0x3A which is very small for big variables.
			I prefer using an heuristic value to optimize data rate.
		*/
		if (size & 1)
		{
			pkt_size = size / 10;
			pkt_size >>= 1;
			pkt_size <<= 1;

			if (pkt_size < 0x3a)
			{
				pkt_size = 0x3a;
			}

			ret = dusb_send_buf_size_request(handle, pkt_size);
			if (ret)
			{
				break;
			}
			ret = dusb_recv_buf_size_alloc(handle, NULL);
			if (ret)
			{
				break;
			}
		}

		ret = dusb_cmd_s_var_content(handle, size, entry->data);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_s_eot(handle);
		if (ret)
		{
			break;
		}

		handle->updat->cnt2 = i + 1;
		handle->updat->max2 = content->num_entries;
		ticalcs_update_pbar(handle);

		PAUSE(50);	// needed
	}

	return ret;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION, DUSB_AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	VarEntry *ve;
	int ret;

	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, vr->folder, vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, attrs, nattrs);
	if (!ret)
	{
		attrs = dusb_ca_new_array(handle, naids);
		ret = dusb_cmd_r_var_header(handle, fldname, varname, attrs);
		if (!ret)
		{
			ret = dusb_cmd_r_var_content(handle, NULL, &data);
			if (!ret)
			{
				content->model = handle->model;
				tifiles_comment_set_single_sn(content->comment, sizeof(content->comment));
				content->num_entries = 1;

				content->entries = tifiles_ve_create_array(1);
				ve = content->entries[0] = tifiles_ve_create();
				memcpy(ve, vr, sizeof(VarEntry));

				ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size);
				memcpy(ve->data, data, ve->size);
				g_free(data);
			}
		}

		dusb_ca_del_array(handle, attrs, naids);
	}

	return ret;
}

static int		send_all_vars_backup	(CalcHandle* handle, FileContent* content)
{
	return send_var(handle, MODE_BACKUP, content);
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	DUSBCalcAttr **attrs;
	const int nattrs = 4;
	int ret = 0;

	// send all headers except license
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI89_LICENSE)
		{
			continue;
		}

		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		ticonv_varname_to_utf8_sn(handle->model, ptr->name, handle->updat->text, sizeof(handle->updat->text), ptr->data_type);
		ticalcs_update_label(handle);

		attrs = dusb_ca_new_array(handle, nattrs);
		attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
		attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
		attrs[1]->data[0] = 0;
		attrs[2] = dusb_ca_new(handle, DUSB_AID_VAR_VERSION, 4);
		attrs[2]->data[3] = 1;
		attrs[3] = dusb_ca_new(handle, DUSB_AID_LOCKED, 1);
		attrs[3]->data[0] = 0;

		ret = dusb_cmd_s_rts(handle, "", ptr->name, ptr->data_length, nattrs, CA(attrs));
		dusb_ca_del_array(handle, attrs, nattrs);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_s_var_content(handle, ptr->data_length, ptr->data_part);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_s_eot(handle);
		if (ret)
		{
			break;
		}
	}

	return ret;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION, DUSB_AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	uint32_t data_length;
	int ret;

	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, attrs, nattrs);
	if (!ret)
	{
		attrs = dusb_ca_new_array(handle, naids);
		ret = dusb_cmd_r_var_header(handle, fldname, varname, attrs);
		if (!ret)
		{
			ret = dusb_cmd_r_var_content(handle, &data_length, &data);
			if (!ret)
			{
				content->model = handle->model;
				ticalcs_strlcpy(content->name, vr->name, sizeof(content->name));
				content->data_type = vr->type;
				content->device_type = DEVICE_TYPE_89;
				content->data_length = data_length;
				content->data_part = data; // Borrow this memory block.
				content->hw_id = handle->calc->product_id;

				// Do NOT g_free(data);
			}
		}
		dusb_ca_del_array(handle, attrs, naids);
	}

	return ret;
}

static int		send_os    (CalcHandle* handle, FlashContent* content)
{
	DUSBModeSet mode = { 2, 1, 0, 0, 0x0fa0 }; //MODE_BASIC;
	uint32_t pkt_size = 0x3ff;
	uint32_t hdr_size = 0;
	uint32_t hdr_offset = 0;
	FlashContent *ptr;
	uint8_t *d;
	int i, r, q;
	int ret;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI89_AMS || ptr->data_type == TI89_APPL)
		{
			break;
		}
	}
	if (ptr == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->data_type != TI89_AMS)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->data_part == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}

	// search for OS header (offset & size)
	hdr_offset = 2+4;
	for (i = hdr_offset, d = ptr->data_part; (d[i] != 0xCC) || (d[i+1] != 0xCC) || (d[i+2] != 0xCC) || (d[i+3] != 0xCC); i++);
	hdr_size = i - hdr_offset - 6;

	do
	{
		// switch to BASIC mode
		ret = dusb_cmd_s_mode_set(handle, mode);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_mode_ack(handle);
		if (ret)
		{
			break;
		}

		// start OS transfer
		ret = dusb_cmd_s_os_begin(handle, ptr->data_length);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_os_ack(handle, &pkt_size);
		if (ret)
		{
			break;
		}

		// send OS header/signature
		ret = dusb_cmd_s_os_header_89(handle, hdr_size, ptr->data_part + hdr_offset);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_os_ack(handle, &pkt_size);
		if (ret)
		{
			break;
		}

		// send OS data
		q = ptr->data_length / 0x2000;
		r = ptr->data_length % 0x2000;

		handle->updat->cnt2 = 0;
		handle->updat->max2 = q;

		for (i = 0; i < q; i++)
		{
			ret = dusb_cmd_s_os_data_89(handle, 0x2000, ptr->data_part + i*0x2000);
			if (ret)
			{
				goto end;
			}
			ret = dusb_cmd_r_data_ack(handle);
			if (ret)
			{
				goto end;
			}

			handle->updat->cnt2 = i;
			ticalcs_update_pbar(handle);
		}

		ret = dusb_cmd_s_os_data_89(handle, r, ptr->data_part + i*0x2000);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		handle->updat->cnt2 = i;
		ticalcs_update_pbar(handle);

		ret = dusb_cmd_s_eot(handle);
		if (ret)
		{
			break;
		}
		PAUSE(500);
		ret = dusb_cmd_r_eot_ack(handle);
	} while (0);
end:

	return ret;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	static const uint16_t pid[] = { DUSB_PID_FULL_ID };
	const int size = 1;
	DUSBCalcParam **params;
	int ret;

	ticalcs_strlcpy(handle->updat->text, "ID-LIST", sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	params = dusb_cp_new_array(handle, size);
	if (params != NULL)
	{
		ret = dusb_cmd_s_param_request(handle, size, pid);
		if (!ret)
		{
			ret = dusb_cmd_r_param_data(handle, size, params);
			if (!ret)
			{
				if (params[0]->ok && params[0]->size == 18)
				{
					memcpy(&id[0], &(params[0]->data[1]), 5);
					memcpy(&id[5], &(params[0]->data[7]), 5);
					memcpy(&id[10], &(params[0]->data[13]), 5);
					id[14] = '\0';
				}
				else
				{
					ret = ERR_INVALID_PACKET;
				}
			}
		}
	}
	else
	{
		ret = ERR_MALLOC;
	}
	dusb_cp_del_array(handle, params, size);

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	DUSBCalcParam *param;
	int ret;

	// Go back to HOME screen
	param = dusb_cp_new(handle, DUSB_PID_HOMESCREEN, 1);
	param->data[0] = 1;
	ret = dusb_cmd_s_param_set(handle, param);
	dusb_cp_del(handle, param);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			// Send dumping program
			ret = rd_send_dumper(handle, "romdump.89z", romDumpSize89t, romDump89t);
			PAUSE(1000);
		}
	}

	return ret;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret;

	ret = dusb_cmd_s_execute(handle, "main", "romdump", DUSB_EID_ASM, "", 0);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			// Get dump
			ret = rd_read_dump(handle, filename);
		}
	}

	return ret;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint32_t calc_time;
	struct tm ref, cur;
	time_t r, c, now;
	uint8_t data[4];
	int ret;

	time(&now);
#ifdef HAVE_LOCALTIME_R
	localtime_r(&now, &ref);
#else
	memcpy(&ref, localtime(&now), sizeof(struct tm));
#endif

	ref.tm_year = 1997 - 1900;
	ref.tm_mon = 0;
	ref.tm_yday = 0;
	ref.tm_mday = 1;
	ref.tm_wday = 3;
	ref.tm_hour = 0;
	ref.tm_min = 0;
	ref.tm_sec = 0;
	//ref.tm_isdst = 1;
	r = mktime(&ref);

	cur.tm_year = _clock->year - 1900;
	cur.tm_mon = _clock->month - 1;
	cur.tm_mday = _clock->day;
	cur.tm_hour = _clock->hours;
	cur.tm_min = _clock->minutes;
	cur.tm_sec = _clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);

	calc_time = (uint32_t)difftime(c, r);

	ticalcs_strlcpy(handle->updat->text, _("Setting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	data[0] = MSB(MSW(calc_time));
	data[1] = LSB(MSW(calc_time));
	data[2] = MSB(LSW(calc_time));
	data[3] = LSB(LSW(calc_time));
	ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_SEC_SINCE_1997, 4, data);
	if (!ret)
	{
		data[0] = _clock->date_format == 3 ? 0 : _clock->date_format;
		ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_DATE_FMT, 1, data);
		if (!ret)
		{
			data[0] = _clock->time_format == 24 ? 1 : 0;
			ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_TIME_FMT, 1, data);
			if (!ret)
			{
				data[0] = _clock->state;
				ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_ON, 1, data);
			}
		}
	}

	return ret;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	static const uint16_t pids[5] = { DUSB_PID_CLASSIC_CLK_SUPPORT, DUSB_PID_CLK_ON, DUSB_PID_CLK_SEC_SINCE_1997, DUSB_PID_CLK_DATE_FMT, DUSB_PID_CLK_TIME_FMT };
	const int size = sizeof(pids) / sizeof(uint16_t);
	DUSBCalcParam **params;
	int ret;

	// get raw clock
	ticalcs_strlcpy(handle->updat->text, _("Getting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	params = dusb_cp_new_array(handle, size);
	ret = dusb_cmd_s_param_request(handle, size, pids);
	if (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, size, params);
		if (!ret)
		{
			if (params[0]->ok && params[0]->size == 1)
			{
				if (!params[0]->data[0])
				{
					ret = ERR_UNSUPPORTED;
				}
				else
				{
					if (   params[1]->ok && params[1]->size == 1
					    && params[2]->ok && params[2]->size == 4
					    && params[3]->ok && params[3]->size == 1
					    && params[4]->ok && params[4]->size == 1)
					{
						struct tm ref, cur;
						time_t r, c, now;
						uint8_t * data = params[2]->data;
						uint32_t calc_time = (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[2]) << 8) | (data[3] <<  0);

						ticalcs_info("%s", _("Found valid classic clock"));

						time(&now);	// retrieve current DST setting
#ifdef HAVE_LOCALTIME_R
						localtime_r(&now, &ref);
#else
						memcpy(&ref, localtime(&now), sizeof(struct tm));
#endif
						ref.tm_year = 1997 - 1900;
						ref.tm_mon = 0;
						ref.tm_yday = 0;
						ref.tm_mday = 1;
						ref.tm_wday = 3;
						ref.tm_hour = 0;
						ref.tm_min = 0;
						ref.tm_sec = 0;
						//ref.tm_isdst = 1;
						r = mktime(&ref);

						c = r + calc_time;
#ifdef HAVE_LOCALTIME_R
						localtime_r(&c, &cur);
#else
						memcpy(&cur, localtime(&c), sizeof(struct tm));
#endif

						_clock->year = cur.tm_year + 1900;
						_clock->month = cur.tm_mon + 1;
						_clock->day = cur.tm_mday;
						_clock->hours = cur.tm_hour;
						_clock->minutes = cur.tm_min;
						_clock->seconds = cur.tm_sec;

						_clock->date_format = params[3]->data[0] == 0 ? 3 : params[3]->data[0];
						_clock->time_format = params[4]->data[0] ? 24 : 12;
						_clock->state = params[1]->data[0];
					}
					else
					{
						ticalcs_warning(_("Found classic clock but failed to retrieve its parameters: %u %u %u %u"),
								params[1]->ok, params[2]->ok, params[3]->ok, params[4]->ok);
					}
				}
			}
		}
	}
	dusb_cp_del_array(handle, params, size);

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	DUSBCalcAttr **attr;
	const int size = 1;
	char varname[68];
	char *utf8;
	int ret;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	attr = dusb_ca_new_array(handle, size);
	attr[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attr[0]->data[0] = 0xF0; attr[0]->data[1] = 0x0C;
	attr[0]->data[2] = 0x00; attr[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_delete(handle, vr->folder, vr->name, size, CA(attr));
	dusb_ca_del_array(handle, attr, size);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	DUSBCalcAttr **attrs;
	const int size = 1;
	char varname1[68], varname2[68];
	char *utf81, *utf82;
	int ret;

	tifiles_build_fullname(handle->model, varname1, oldname->folder, oldname->name);
	tifiles_build_fullname(handle->model, varname2, newname->folder, newname->name);
	utf81 = ticonv_varname_to_utf8(handle->model, varname1, oldname->type);
	utf82 = ticonv_varname_to_utf8(handle->model, varname2, newname->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Renaming %s to %s..."), utf81, utf82);
	ticonv_utf8_free(utf82);
	ticonv_utf8_free(utf81);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, size);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = oldname->type;

	ret = dusb_cmd_s_var_modify(handle, oldname->folder, oldname->name, 1, CA(attrs), newname->folder, newname->name, 0, NULL);
	dusb_ca_del_array(handle, attrs, size);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		change_attr	(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	DUSBCalcAttr **srcattrs;
	DUSBCalcAttr **dstattrs;
	int ret = 0;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->folder, -1);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Changing attributes of %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	srcattrs = dusb_ca_new_array(handle, 1);
	srcattrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	srcattrs[0]->data[0] = 0xF0; srcattrs[0]->data[1] = 0x0C;
	srcattrs[0]->data[2] = 0x00; srcattrs[0]->data[3] = vr->type;

	dstattrs = dusb_ca_new_array(handle, 2);
	dstattrs[0] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
	dstattrs[0]->data[0] = (attr == ATTRB_ARCHIVED ? 0x01 : 0x00);
	dstattrs[1] = dusb_ca_new(handle, DUSB_AID_LOCKED, 1);
	dstattrs[1]->data[0] = (attr == ATTRB_LOCKED ? 0x01 : 0x00);

	ret = dusb_cmd_s_var_modify(handle, vr->folder, vr->name, 1, CA(srcattrs), vr->folder, vr->name, 2, CA(dstattrs));
	dusb_ca_del_array(handle, dstattrs, 2);
	dusb_ca_del_array(handle, srcattrs, 1);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char *fldname = vr->folder;
	DUSBCalcParam *param;
	DUSBCalcAttr **attrs;
	const int nattrs = 4;
	char *utf8;
	int ret;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->folder, -1);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Creating %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	// send empty expression in specified folder
	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = 0x00;
	attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
	attrs[1]->data[0] = 0;
	attrs[2] = dusb_ca_new(handle, DUSB_AID_VAR_VERSION, 4);
	attrs[2]->data[0] = 0;
	attrs[3] = dusb_ca_new(handle, DUSB_AID_LOCKED, 1);
	attrs[3]->data[0] = 0;

	do
	{
		ret = dusb_cmd_s_rts(handle, fldname, "a1234567", sizeof(data), nattrs, CA(attrs));
		dusb_ca_del_array(handle, attrs, nattrs);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_s_var_content(handle, sizeof(data), data);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_s_eot(handle);
		if (ret)
		{
			break;
		}

		// go back to HOME screen
		param = dusb_cp_new(handle, DUSB_PID_HOMESCREEN, 1);
		param->data[0] = 1;
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(handle, param);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		// delete 'a1234567' variable
		ticalcs_strlcpy(vr->name, "a1234567", sizeof(vr->name));
		ret = del_var(handle, vr);
	} while (0);

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	static const uint16_t pids1[] = {
		DUSB_PID_PRODUCT_NAME, DUSB_PID_MAIN_PART_ID,
		DUSB_PID_HW_VERSION, DUSB_PID_LANGUAGE_ID, DUSB_PID_SUBLANG_ID, DUSB_PID_DEVICE_TYPE,
		DUSB_PID_BOOT_VERSION, DUSB_PID_OS_VERSION,
		DUSB_PID_PHYS_RAM, DUSB_PID_USER_RAM, DUSB_PID_FREE_RAM,
		DUSB_PID_PHYS_FLASH, DUSB_PID_USER_FLASH, DUSB_PID_FREE_FLASH,
		DUSB_PID_LCD_WIDTH, DUSB_PID_LCD_HEIGHT,
	};
	static const uint16_t pids2[] = {
		DUSB_PID_BITS_PER_PIXEL, DUSB_PID_BATTERY_ENOUGH, DUSB_PID_OS_MODE, DUSB_PID_CLASSIC_CLK_SUPPORT
	};	// Titanium can't manage more than 16 parameters at a time
	const int size1 = sizeof(pids1) / sizeof(uint16_t);
	const int size2 = sizeof(pids2) / sizeof(uint16_t);
	DUSBCalcParam **params1;
	DUSBCalcParam **params2;
	int i = 0;
	int ret = 0;

	ticalcs_strlcpy(handle->updat->text, _("Getting version..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	memset(infos, 0, sizeof(CalcInfos));
	params1 = dusb_cp_new_array(handle, size1);
	params2 = dusb_cp_new_array(handle, size2);

	do
	{
		unsigned int infos_mask = 0;
		ret = dusb_cmd_s_param_request(handle, size1, pids1);
		if (!ret)
		{
			ret = dusb_cmd_r_param_data(handle, size1, params1);
			if (!ret)
			{
				ret = dusb_cmd_s_param_request(handle, size2, pids2);
				if (!ret)
				{
					ret = dusb_cmd_r_param_data(handle, size2, params2);
				}
			}
		}

		if (ret)
		{
			break;
		}

		if (params1[i]->ok)
		{
			const uint32_t maxsize = params1[i]->size < sizeof(infos->product_name) ? params1[i]->size + 1 : sizeof(infos->product_name);
			ticalcs_strlcpy(infos->product_name, (char *)params1[i]->data, maxsize);
			infos_mask |= INFOS_PRODUCT_NAME;
		}
		i++;

		if (params1[i]->ok && params1[i]->size >= 18)
		{
			strncpy(infos->main_calc_id, (char*)&(params1[i]->data[1]), 5);
			strncpy(infos->main_calc_id+5, (char*)&(params1[i]->data[7]), 5);
			strncpy(infos->main_calc_id+10, (char*)&(params1[i]->data[13]), 4);
			infos->main_calc_id[14] = 0;
			infos_mask |= INFOS_MAIN_CALC_ID;
			ticalcs_strlcpy(infos->product_id, infos->main_calc_id, sizeof(infos->product_id));
			infos_mask |= INFOS_PRODUCT_ID;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 2)
		{
			infos->hw_version = ((((uint16_t)params1[i]->data[0]) << 8) | params1[i]->data[1]) + 1;
			infos_mask |= INFOS_HW_VERSION; // hw version or model ?
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 1)
		{
			infos->language_id = params1[i]->data[0];
			infos_mask |= INFOS_LANG_ID;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 1)
		{
			infos->sub_lang_id = params1[i]->data[0];
			infos_mask |= INFOS_SUB_LANG_ID;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 2)
		{
			infos->device_type = params1[i]->data[1];
			infos_mask |= INFOS_DEVICE_TYPE;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 4)
		{
			ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1u.%02ub%u", params1[i]->data[1], params1[i]->data[2], params1[i]->data[3]);
			infos_mask |= INFOS_BOOT_VERSION;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 4)
		{
			ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1u.%02u", params1[i]->data[1], params1[i]->data[2]);
			infos_mask |= INFOS_OS_VERSION;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->ram_phys = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                   | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                   | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                   | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                   | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                   | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                   | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_RAM_PHYS;
		}
		i++;
		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->ram_user = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                   | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                   | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                   | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                   | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                   | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                   | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_RAM_USER;
		}
		i++;
		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->ram_free = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                   | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                   | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                   | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                   | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                   | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                   | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_RAM_FREE;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->flash_phys = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                     | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                     | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                     | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                     | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                     | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                     | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_FLASH_PHYS;
		}
		i++;
		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->flash_user = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                     | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                     | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                     | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                     | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                     | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                     | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_FLASH_USER;
		}
		i++;
		if (params1[i]->ok && params1[i]->size == 8)
		{
			infos->flash_free = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
			                     | (((uint64_t)(params1[i]->data[ 1])) << 48)
			                     | (((uint64_t)(params1[i]->data[ 2])) << 40)
			                     | (((uint64_t)(params1[i]->data[ 3])) << 32)
			                     | (((uint64_t)(params1[i]->data[ 4])) << 24)
			                     | (((uint64_t)(params1[i]->data[ 5])) << 16)
			                     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
			                     | (((uint64_t)(params1[i]->data[ 7]))      ));
			infos_mask |= INFOS_FLASH_FREE;
		}
		i++;

		if (params1[i]->ok && params1[i]->size == 2)
		{
			infos->lcd_width = (  (((uint16_t)params1[i]->data[ 0]) <<  8)
			                    | (((uint16_t)params1[i]->data[ 1])      ));
			infos_mask |= INFOS_LCD_WIDTH;
		}
		i++;
		if (params1[i]->ok && params1[i]->size == 2)
		{
			infos->lcd_height = (  (((uint16_t)params1[i]->data[ 0]) <<  8)
			                     | (((uint16_t)params1[i]->data[ 1])      ));
			infos_mask |= INFOS_LCD_HEIGHT;
		}
		i++;

		i = 0;
		if (params2[i]->ok && params2[i]->size == 1)
		{
			infos->bits_per_pixel = params2[i]->data[0];
			infos_mask |= INFOS_BPP;
		}
		i++;

		if (params2[i]->ok && params2[i]->size == 1)
		{
			infos->battery = params2[i]->data[0];
			infos_mask |= INFOS_BATTERY_ENOUGH;
		}
		i++;

		if (params2[i]->ok && params2[i]->size == 1)
		{
			infos->run_level = params2[i]->data[0];
			infos_mask |= INFOS_RUN_LEVEL;
		}
		i++;

		if (params2[i]->ok && params2[i]->size == 1)
		{
			infos->clock_support = params2[i]->data[0];
			infos_mask |= INFOS_CLOCK_SUPPORT;
		}
		i++;

		infos->model = CALC_TI89T;
		infos_mask |= INFOS_CALC_MODEL;
		infos->mask = (InfosMask)infos_mask;
	} while (0);

	dusb_cp_del_array(handle, params2, size2);
	dusb_cp_del_array(handle, params1, size1);

	return ret;
}

extern const CalcFncts calc_89t_usb =
{
	CALC_TI89T_USB,
	"Titanium",
	"TI-89 Titanium",
	N_("TI-89 Titanium thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS | OPS_ROMDUMP |
	OPS_IDLIST | OPS_CLOCK | OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_FOLDER,
	PRODUCT_ID_TI89T,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "2P1L", /* send_all_vars_backup */
	 "2P1L", /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 ""      /* get_lab_equipment_data */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_os,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data
};
