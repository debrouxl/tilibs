/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#include <ticonv.h>
#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

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
	DUSBModeSet mode = MODE_NORMAL;
	int ret;

	ret = dusb_cmd_s_mode_set(handle, mode);
	if (!ret)
	{
		ret = dusb_cmd_r_mode_ack(handle);
	}

	return ret;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	int ret;

	PAUSE(25);	// this pause is needed between 2 keys
	ret = dusb_cmd_s_execute(handle, "", "", EID_KEY, "", key);
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
		case TI89t_ASM:  action = EID_ASM; break;
		case TI89t_APPL: action = EID_APP; break;
		default:         action = EID_PRGM; break;
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
	static const uint16_t pid[] = { PID_SCREENSHOT };
	const int size = 1;
	DUSBCalcParam **param;
	int ret;

	sc->width = TI89T_COLS;
	sc->height = TI89T_ROWS;
	sc->clipped_width = TI89T_COLS_VISIBLE;
	sc->clipped_height = TI89T_ROWS_VISIBLE;
	sc->pixel_format = CALC_PIXFMT_MONO;
    
	param = dusb_cp_new_array(size);
	ret = dusb_cmd_s_param_request(handle, size, pid);
	while (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, 1, param);
		if (!ret)
		{
			if (!param[0]->ok || param[0]->size != TI89T_COLS * TI89T_ROWS / 8)
			{
				ret = ERR_INVALID_PACKET;
				break;
			}
			
			*bitmap = (uint8_t *)g_malloc(TI89T_COLS * TI89T_ROWS / 8);
			if (*bitmap == NULL)
			{
				ret = ERR_MALLOC;
				break;
			}
			memcpy(*bitmap, param[0]->data, TI89T_COLS * TI89T_ROWS / 8);

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
	dusb_cp_del_array(size, param);

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	static const uint16_t aids[] = { AID_VAR_TYPE, AID_ARCHIVED, AID_4APPVAR, AID_VAR_SIZE, AID_LOCKED, AID_UNKNOWN_42 };
	const int size = sizeof(aids) / sizeof(uint16_t);
	TreeInfo *ti;
	int ret;
	DUSBCalcAttr **attr;
	GNode *root, *folder = NULL;
	char fldname[40];
	char varname[40];
	char folder_name[40] = "";
	char *u1, *u2;

	(*apps) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

	(*vars) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	root = g_node_new(NULL);
	g_node_append(*apps, root);

	ret = dusb_cmd_s_dirlist_request(handle, size, aids);
	if (!ret)
	{
		for(;;)
		{
			VarEntry *ve = tifiles_ve_create();
			GNode *node;

			attr = dusb_ca_new_array(size);
			ret = dusb_cmd_r_var_header(handle, fldname, varname, attr);
			if (ret)
			{
				// Not a real error.
				if (ret == ERR_EOT)
				{
					ret = 0;
				}
				dusb_ca_del_array(size, attr);
				break;
			}

			strncpy(ve->folder, fldname, sizeof(ve->folder) - 1);
			ve->folder[sizeof(ve->folder) - 1] = 0;
			strncpy(ve->name, varname, sizeof(ve->name) - 1);
			ve->name[sizeof(ve->name) - 1] = 0;
			ve->size = (  (((uint32_t)(attr[3]->data[0])) << 24)
				    | (((uint32_t)(attr[3]->data[1])) << 16)
				    | (((uint32_t)(attr[3]->data[2])) <<  8)
				    | (((uint32_t)(attr[3]->data[3]))      ));
			ve->type = (uint32_t)(attr[0]->data[3]);
			ve->attr = attr[1]->data[0] ? ATTRB_ARCHIVED : attr[4]->data[0] ? ATTRB_LOCKED : ATTRB_NONE;
			dusb_ca_del_array(size, attr);

			if (ve->type == TI89_DIR)
			{
				strncpy(folder_name, ve->folder, sizeof(folder_name) - 1);
				folder_name[sizeof(folder_name) - 1] = 0;
				strncpy(ve->name, ve->folder, sizeof(ve->name) - 1);
				ve->name[sizeof(ve->name) - 1] = 0;
				ve->folder[0] = 0;

				node = g_node_new(ve);
				folder = g_node_append(*vars, node);
			}
			else
			{
				if(!strcmp(ve->folder, "main") && (!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
				{
					tifiles_ve_delete(ve);
				}
				else
				{
					node = g_node_new(ve);
					if (ve->type != TI73_APPL)
					{
						g_node_append(folder, node);
					}
					else
					{
						g_node_append(root, node);
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
			u1 = ticonv_varname_to_utf8(handle->model, ((VarEntry *) (folder->data))->name, -1);
			u2 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
			g_snprintf(update_->text, sizeof(update_->text), _("Parsing %s/%s"), u1, u2);
			g_free(u1); g_free(u2);
			update_label();
		}
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	static const uint16_t pids[] = { PID_FREE_RAM, PID_FREE_FLASH };
	const int size = sizeof(pids) / sizeof(uint16_t);
	DUSBCalcParam **params;
	int ret;

	params = dusb_cp_new_array(size);
	ret = dusb_cmd_s_param_request(handle, size, pids);
	if (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, size, params);
		if (!ret)
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
	}
	dusb_cp_del_array(size, params);

	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	char *utf8;
	DUSBCalcAttr **attrs;
	const int nattrs = 4;
	uint32_t pkt_size;
	int ret = 0;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *ve = content->entries[i];
		char varname[18];

		if (ve->action == ACT_SKIP)
		{
			continue;
		}

		if (ve->folder[0])
		{
			tifiles_build_fullname(handle->model, varname, ve->folder, ve->name);
		}
		else
		{
			strncpy(varname, ve->name, sizeof(varname) - 1);
			varname[sizeof(varname) - 1] = 0;
		}

		utf8 = ticonv_varname_to_utf8(handle->model, varname, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		attrs = dusb_ca_new_array(nattrs);
		attrs[0] = dusb_ca_new(AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ve->type;
		attrs[1] = dusb_ca_new(AID_ARCHIVED, 1);
		attrs[1]->data[0] = ve->attr == ATTRB_ARCHIVED ? 1 : 0;
		attrs[2] = dusb_ca_new(AID_VAR_VERSION, 4);
		attrs[2]->data[0] = 0;
		attrs[3] = dusb_ca_new(AID_LOCKED, 1);
		attrs[3]->data[0] = ve->attr == ATTRB_LOCKED ? 1 : 0;

		if(!(ve->size & 1))
		{
			ret = is_ready(handle);
		}

		if (ret)
		{
			dusb_ca_del_array(nattrs, attrs);
			break;
		}

		ret = dusb_cmd_s_rts(handle, ve->folder, ve->name, ve->size, nattrs, CA(attrs));
		dusb_ca_del_array(nattrs, attrs);
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
		if (ve->size & 1)
		{
			pkt_size = ve->size / 10;
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

		ret = dusb_cmd_s_var_content(handle, ve->size, ve->data);
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

		update_->cnt2 = i+1;
		update_->max2 = content->num_entries;
		update_->pbar();

		PAUSE(50);	// needed
	}

	return ret;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	static const uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION, AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	VarEntry *ve;
	char *utf8;
	int ret;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	attrs = dusb_ca_new_array(nattrs);
	attrs[0] = dusb_ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, vr->folder, vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(nattrs, attrs);
	if (!ret)
	{
		attrs = dusb_ca_new_array(naids);
		ret = dusb_cmd_r_var_header(handle, fldname, varname, attrs);
		if (!ret)
		{
			ret = dusb_cmd_r_var_content(handle, NULL, &data);
			if (!ret)
			{
				content->model = handle->model;
				strncpy(content->comment, tifiles_comment_set_single(), sizeof(content->comment) - 1);
				content->comment[sizeof(content->comment) - 1] = 0;
				content->num_entries = 1;

				content->entries = tifiles_ve_create_array(1);
				ve = content->entries[0] = tifiles_ve_create();
				memcpy(ve, vr, sizeof(VarEntry));

				ve->data = tifiles_ve_alloc_data(ve->size);
				memcpy(ve->data, data, ve->size);
				g_free(data);
			}
		}

		dusb_ca_del_array(naids, attrs);
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
	char *utf8;
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

		utf8 = ticonv_varname_to_utf8(handle->model, ptr->name, ptr->data_type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		attrs = dusb_ca_new_array(nattrs);
		attrs[0] = dusb_ca_new(AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
		attrs[1] = dusb_ca_new(AID_ARCHIVED, 1);
		attrs[1]->data[0] = 0;
		attrs[2] = dusb_ca_new(AID_VAR_VERSION, 4);
		attrs[2]->data[3] = 1;
		attrs[3] = dusb_ca_new(AID_LOCKED, 1);
		attrs[3]->data[0] = 0;

		ret = dusb_cmd_s_rts(handle, "", ptr->name, ptr->data_length, nattrs, CA(attrs));
		dusb_ca_del_array(nattrs, attrs);
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
	static const uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION, AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	char *utf8;
	int ret;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	attrs = dusb_ca_new_array(nattrs);
	attrs[0] = dusb_ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(nattrs, attrs);
	if (!ret)
	{
		attrs = dusb_ca_new_array(naids);
		ret = dusb_cmd_r_var_header(handle, fldname, varname, attrs);
		if (!ret)
		{
			ret = dusb_cmd_r_var_content(handle, NULL, &data);
			if (!ret)
			{
				content->model = handle->model;
				strncpy(content->name, vr->name, sizeof(content->name) - 1);
				content->name[sizeof(content->name) - 1] = 0;
				content->data_length = vr->size;
				content->data_part = (uint8_t *)tifiles_ve_alloc_data(vr->size);
				content->data_type = vr->type;
				content->device_type = DEVICE_TYPE_89;

				memcpy(content->data_part, data, content->data_length);
				g_free(data);
			}
		}
		dusb_ca_del_array(naids, attrs);
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
	for(i = 6, d = ptr->data_part; (d[i] != 0xCC) || (d[i+1] != 0xCC) || (d[i+2] != 0xCC) || (d[i+3] != 0xCC); i++);
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

		update_->cnt2 = 0;
		update_->max2 = q;

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

			update_->cnt2 = i;
			update_->pbar();
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

		update_->cnt2 = i;
		update_->pbar();

		ret = dusb_cmd_s_eot(handle);
		if (ret)
		{
			break;
		}
		PAUSE(500);
		ret = dusb_cmd_r_eot_ack(handle);
	} while(0);
end:

	return ret;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	static const uint16_t pid[] = { PID_FULL_ID };
	const int size = 1;
	DUSBCalcParam **param;
	int ret;

	g_snprintf(update_->text, sizeof(update_->text), "ID-LIST");
	update_label();

	param = dusb_cp_new_array(size);
	ret = dusb_cmd_s_param_request(handle, size, pid);
	if (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, 1, param);
		if (!ret)
		{
			if (!param[0]->ok)
			{
				ret = ERR_INVALID_PACKET;
			}
			else
			{
				memcpy(&id[0], &(param[0]->data[1]), 5);
				memcpy(&id[5], &(param[0]->data[7]), 5);
				memcpy(&id[10], &(param[0]->data[13]), 5);
				id[14] = '\0';
			}
		}
	}
	dusb_cp_del_array(size, param);

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	DUSBCalcParam *param;
	int ret;

	// Go back to HOME screen
	param = dusb_cp_new(PID_HOMESCREEN, 1);
	param->data[0] = 1;
	ret = dusb_cmd_s_param_set(handle, param);
	dusb_cp_del(param);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			// Send dumping program
			ret = rd_send(handle, "romdump.89z", romDumpSize89t, romDump89t);
			PAUSE(1000);
		}
	}

	return ret;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret;

	ret = dusb_cmd_s_execute(handle, "main", "romdump", EID_ASM, "", 0);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			// Get dump
			ret = rd_dump(handle, filename);
		}
	}

	return ret;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	DUSBCalcParam *param;
	uint32_t calc_time;
	struct tm ref, cur;
	time_t r, c, now;
	int ret;

	time(&now);
	memcpy(&ref, localtime(&now), sizeof(struct tm));

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

	g_snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
	update_label();

	do
	{
		param = dusb_cp_new(PID_CLK_SEC, 4);
		param->data[0] = MSB(MSW(calc_time));
		param->data[1] = LSB(MSW(calc_time));
		param->data[2] = MSB(LSW(calc_time));
		param->data[3] = LSB(LSW(calc_time));
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(param);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		param = dusb_cp_new(PID_CLK_DATE_FMT, 1);
		param->data[0] = _clock->date_format == 3 ? 0 : _clock->date_format;
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(param);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		param = dusb_cp_new(PID_CLK_TIME_FMT, 1);
		param->data[0] = _clock->time_format == 24 ? 1 : 0;
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(param);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
		}

		param = dusb_cp_new(PID_CLK_ON, 1);
		param->data[0] = _clock->state;
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(param);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_data_ack(handle);
	} while(0);

	return ret;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	static const uint16_t pids[4] = { PID_CLK_SEC, PID_CLK_DATE_FMT, PID_CLK_TIME_FMT, PID_CLK_ON };
	const int size = sizeof(pids) / sizeof(uint16_t);
	DUSBCalcParam **params;
	uint32_t calc_time;
	struct tm ref, *cur;
	time_t r, c, now;
	int ret;

	// get raw clock
	g_snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
	update_label();

	params = dusb_cp_new_array(size);
	ret = dusb_cmd_s_param_request(handle, size, pids);
	if (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, size, params);
		if (!ret)
		{
			if (!params[0]->ok)
			{
				ret = ERR_INVALID_PACKET;
			}
			else
			{
				// and computes
				calc_time = (((uint32_t)params[0]->data[0]) << 24) | (((uint32_t)params[0]->data[1]) << 16) | (((uint32_t)params[0]->data[2]) <<  8) | (params[0]->data[3] <<  0);

				time(&now);	// retrieve current DST setting
				memcpy(&ref, localtime(&now), sizeof(struct tm));;
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
				cur = localtime(&c);

				_clock->year = cur->tm_year + 1900;
				_clock->month = cur->tm_mon + 1;
				_clock->day = cur->tm_mday;
				_clock->hours = cur->tm_hour;
				_clock->minutes = cur->tm_min;
				_clock->seconds = cur->tm_sec;

				_clock->date_format = params[1]->data[0] == 0 ? 3 : params[1]->data[0];
				_clock->time_format = params[2]->data[0] ? 24 : 12;
				_clock->state = params[3]->data[0];
			}
		}
	}
	dusb_cp_del_array(size, params);

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	DUSBCalcAttr **attr;
	const int size = 2;
	char varname[68];
	char *utf8;
	int ret;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	attr = dusb_ca_new_array(size);
	attr[0] = dusb_ca_new(AID_VAR_TYPE2, 4);
	attr[0]->data[0] = 0xF0; attr[0]->data[1] = 0x0C;
	attr[0]->data[2] = 0x00; attr[0]->data[3] = vr->type;
	attr[1] = dusb_ca_new(AID_UNKNOWN_13, 1);
	attr[1]->data[0] = 0;

	ret = dusb_cmd_s_var_delete(handle, vr->folder, vr->name, size, CA(attr));
	dusb_ca_del_array(size, attr);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	DUSBCalcAttr **attrs;
	int ret;
	char varname1[68], varname2[68];
	char *utf81, *utf82;

	tifiles_build_fullname(handle->model, varname1, oldname->folder, oldname->name);
	tifiles_build_fullname(handle->model, varname2, newname->folder, newname->name);
	utf81 = ticonv_varname_to_utf8(handle->model, varname1, oldname->type);
	utf82 = ticonv_varname_to_utf8(handle->model, varname2, newname->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Renaming %s to %s..."), utf81, utf82);
	g_free(utf81);
	g_free(utf82);
	update_label();

	attrs = dusb_ca_new_array(1);
	attrs[0] = dusb_ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = oldname->type;

	ret = dusb_cmd_s_var_modify(handle, oldname->folder, oldname->name, 1, CA(attrs), newname->folder, newname->name, 0, NULL);
	dusb_ca_del_array(1, attrs);
	if(!ret)
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
	g_snprintf(update_->text, sizeof(update_->text), _("Changing attributes of %s..."), utf8);
	g_free(utf8);
	update_label();

	srcattrs = dusb_ca_new_array(1);
	srcattrs[0] = dusb_ca_new(AID_VAR_TYPE2, 4);
	srcattrs[0]->data[0] = 0xF0; srcattrs[0]->data[1] = 0x0C;
	srcattrs[0]->data[2] = 0x00; srcattrs[0]->data[3] = vr->type;

	dstattrs = dusb_ca_new_array(2);
	dstattrs[0] = dusb_ca_new(AID_ARCHIVED, 1);
	dstattrs[0]->data[0] = (attr == ATTRB_ARCHIVED ? 0x01 : 0x00);
	dstattrs[1] = dusb_ca_new(AID_LOCKED, 1);
	dstattrs[1]->data[0] = (attr == ATTRB_LOCKED ? 0x01 : 0x00);

	ret = dusb_cmd_s_var_modify(handle, vr->folder, vr->name, 1, CA(srcattrs), vr->folder, vr->name, 2, CA(dstattrs));
	dusb_ca_del_array(2, dstattrs);
	dusb_ca_del_array(1, srcattrs);
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
	g_snprintf(update_->text, sizeof(update_->text), _("Creating %s..."), utf8);
	g_free(utf8);
	update_label();

	// send empty expression in specified folder
	attrs = dusb_ca_new_array(nattrs);
	attrs[0] = dusb_ca_new(AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = 0x00;
	attrs[1] = dusb_ca_new(AID_ARCHIVED, 1);
	attrs[1]->data[0] = 0;
	attrs[2] = dusb_ca_new(AID_VAR_VERSION, 4);
	attrs[2]->data[0] = 0;
	attrs[3] = dusb_ca_new(AID_LOCKED, 1);
	attrs[3]->data[0] = 0;

	do
	{
		ret = dusb_cmd_s_rts(handle, fldname, "a1234567", sizeof(data), nattrs, CA(attrs));
		dusb_ca_del_array(nattrs, attrs);
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
		param = dusb_cp_new(PID_HOMESCREEN, 1);
		param->data[0] = 1;
		ret = dusb_cmd_s_param_set(handle, param);
		dusb_cp_del(param);
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
		strncpy(vr->name, "a1234567", sizeof(vr->name) - 1);
		vr->name[sizeof(vr->name) - 1] = 0;
		ret = del_var(handle, vr);
	} while(0);

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	static const uint16_t pids1[] = {
		PID_PRODUCT_NAME, PID_MAIN_PART_ID,
		PID_HW_VERSION, PID_LANGUAGE_ID, PID_SUBLANG_ID, PID_DEVICE_TYPE,
		PID_BOOT_VERSION, PID_OS_VERSION, 
		PID_PHYS_RAM, PID_USER_RAM, PID_FREE_RAM,
		PID_PHYS_FLASH, PID_FREE_FLASH, PID_FREE_FLASH,
		PID_LCD_WIDTH, PID_LCD_HEIGHT,
	};
	static const uint16_t pids2[] = {
		 PID_BATTERY, PID_OS_MODE,
	};	// Titanium can't manage more than 16 parameters at a time
	const int size1 = sizeof(pids1) / sizeof(uint16_t);
	const int size2 = sizeof(pids2) / sizeof(uint16_t);
	DUSBCalcParam **params1;
	DUSBCalcParam **params2;
	int i = 0;
	int ret = 0;

	g_snprintf(update_->text, sizeof(update_->text), _("Getting version..."));
	update_label();

	memset(infos, 0, sizeof(CalcInfos));
	params1 = dusb_cp_new_array(size1);
	params2 = dusb_cp_new_array(size2);

	do
	{
		ret = dusb_cmd_s_param_request(handle, size1, pids1);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_param_data(handle, size1, params1);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_s_param_request(handle, size2, pids2);
		if (ret)
		{
			break;
		}

		ret = dusb_cmd_r_param_data(handle, size2, params2);
		if (ret)
		{
			break;
		}

		strncpy(infos->product_name, (char *)params1[i]->data, params1[i]->size);
		infos->mask |= INFOS_PRODUCT_NAME;
		i++;

		strncpy(infos->main_calc_id, (char*)&(params1[i]->data[1]), 5);
		strncpy(infos->main_calc_id+5, (char*)&(params1[i]->data[7]), 5);
		strncpy(infos->main_calc_id+10, (char*)&(params1[i]->data[13]), 4);
		infos->main_calc_id[14] = '\0';
		infos->mask |= INFOS_MAIN_CALC_ID;
		strncpy(infos->product_id, infos->main_calc_id, sizeof(infos->product_id) - 1);
		infos->product_id[sizeof(infos->product_id) - 1] = 0;
		infos->mask |= INFOS_PRODUCT_ID;
		i++;

		infos->hw_version = ((((uint16_t)params1[i]->data[0]) << 8) | params1[i]->data[1]) + 1;
		infos->mask |= INFOS_HW_VERSION; // hw version or model ?
		i++;

		infos->language_id = params1[i]->data[0];
		infos->mask |= INFOS_LANG_ID;
		i++;

		infos->sub_lang_id = params1[i]->data[0];
		infos->mask |= INFOS_SUB_LANG_ID;
		i++;

		infos->device_type = params1[i]->data[1];
		infos->mask |= INFOS_DEVICE_TYPE;
		i++;

		g_snprintf(infos->boot_version, 5, "%1i.%02i", params1[i]->data[1], params1[i]->data[2]);
		infos->mask |= INFOS_BOOT_VERSION;
		i++;

		g_snprintf(infos->os_version, 5, "%1i.%02i", params1[i]->data[1], params1[i]->data[2]);
		infos->mask |= INFOS_OS_VERSION;
		i++;

		infos->ram_phys = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				   | (((uint64_t)(params1[i]->data[ 1])) << 48)
				   | (((uint64_t)(params1[i]->data[ 2])) << 40)
				   | (((uint64_t)(params1[i]->data[ 3])) << 32)
				   | (((uint64_t)(params1[i]->data[ 4])) << 24)
				   | (((uint64_t)(params1[i]->data[ 5])) << 16)
				   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				   | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_RAM_PHYS;
		i++;
		infos->ram_user = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				   | (((uint64_t)(params1[i]->data[ 1])) << 48)
				   | (((uint64_t)(params1[i]->data[ 2])) << 40)
				   | (((uint64_t)(params1[i]->data[ 3])) << 32)
				   | (((uint64_t)(params1[i]->data[ 4])) << 24)
				   | (((uint64_t)(params1[i]->data[ 5])) << 16)
				   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				   | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_RAM_USER;
		i++;
		infos->ram_free = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				   | (((uint64_t)(params1[i]->data[ 1])) << 48)
				   | (((uint64_t)(params1[i]->data[ 2])) << 40)
				   | (((uint64_t)(params1[i]->data[ 3])) << 32)
				   | (((uint64_t)(params1[i]->data[ 4])) << 24)
				   | (((uint64_t)(params1[i]->data[ 5])) << 16)
				   | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				   | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_RAM_FREE;
		i++;

		infos->flash_phys = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				     | (((uint64_t)(params1[i]->data[ 1])) << 48)
				     | (((uint64_t)(params1[i]->data[ 2])) << 40)
				     | (((uint64_t)(params1[i]->data[ 3])) << 32)
				     | (((uint64_t)(params1[i]->data[ 4])) << 24)
				     | (((uint64_t)(params1[i]->data[ 5])) << 16)
				     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				     | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_FLASH_PHYS;
		i++;
		infos->flash_user = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				     | (((uint64_t)(params1[i]->data[ 1])) << 48)
				     | (((uint64_t)(params1[i]->data[ 2])) << 40)
				     | (((uint64_t)(params1[i]->data[ 3])) << 32)
				     | (((uint64_t)(params1[i]->data[ 4])) << 24)
				     | (((uint64_t)(params1[i]->data[ 5])) << 16)
				     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				     | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_FLASH_USER;
		i++;
		infos->flash_free = (  (((uint64_t)(params1[i]->data[ 0])) << 56)
				     | (((uint64_t)(params1[i]->data[ 1])) << 48)
				     | (((uint64_t)(params1[i]->data[ 2])) << 40)
				     | (((uint64_t)(params1[i]->data[ 3])) << 32)
				     | (((uint64_t)(params1[i]->data[ 4])) << 24)
				     | (((uint64_t)(params1[i]->data[ 5])) << 16)
				     | (((uint64_t)(params1[i]->data[ 6])) <<  8)
				     | (((uint64_t)(params1[i]->data[ 7]))      ));
		infos->mask |= INFOS_FLASH_FREE;
		i++;

		infos->lcd_width = (  (((uint16_t)params1[i]->data[ 0]) <<  8)
				    | (((uint16_t)params1[i]->data[ 1])      ));
		infos->mask |= INFOS_LCD_WIDTH;
		i++;
		infos->lcd_height = (  (((uint16_t)params1[i]->data[ 0]) <<  8)
				     | (((uint16_t)params1[i]->data[ 1])      ));
		infos->mask |= INFOS_LCD_HEIGHT;
		i++;

		infos->bits_per_pixel = 1;
		infos->mask |= INFOS_BPP;

		i = 0;
		infos->battery = params2[i]->data[0];
		infos->mask |= INFOS_BATTERY;
		i++;

		infos->run_level = params2[i]->data[0];
		infos->mask |= INFOS_RUN_LEVEL;
		i++;

		infos->model = CALC_TI89T;
		infos->mask |= INFOS_CALC_MODEL;
	} while(0);

	dusb_cp_del_array(size2, params2);
	dusb_cp_del_array(size1, params1);

	return ret;
}

const CalcFncts calc_89t_usb = 
{
	CALC_TI89T_USB,
	"Titanium",
	"TI-89 Titanium",
	N_("TI-89 Titanium thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS | OPS_ROMDUMP |
	OPS_IDLIST | OPS_CLOCK | OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_FOLDER,
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
	 "2P1L"  /* recv_all_vars_backup */ },
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
	&tixx_recv_all_vars_backup
};
