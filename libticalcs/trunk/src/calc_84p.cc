/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Liévin
 *  Copyright (c) 2005  Benjamin Moody (ROM dumper)
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
	TI84+ support thru DirectUsb link.
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
#include "rom84p.h"
#include "rom84pcu.h"
#include "rom82a.h"
#include "rom834pceu.h"
#include "romdump.h"
#include "keys83p.h"

// Screen coordinates of the TI83+
#define TI84P_ROWS  64
#define TI84P_COLS  96
#define TI84PC_ROWS 240
#define TI84PC_COLS 320

static int		is_ready	(CalcHandle* handle)
{
	int ret;
	static const DUSBModeSet mode = DUSB_MODE_NORMAL;

	ret = dusb_cmd_s_mode_set(handle, mode);
	if (!ret)
	{
		// use PID_HOMESCREEN to return status ?
		ret = dusb_cmd_r_mode_ack(handle);
	}

	return ret;
}

static int		send_key_noack	(CalcHandle* handle, uint32_t key)
{
	int ret;

	ret = dusb_cmd_s_execute(handle, "", "", DUSB_EID_KEY, "", (uint16_t)key);
	if (!ret)
	{
		ret = dusb_cmd_r_delay_ack(handle);
	}

	return ret;
}

static int		send_key	(CalcHandle* handle, uint32_t key)
{
	int ret;

	ret = send_key_noack(handle, key);
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
		case TI84p_ASM:  action = DUSB_EID_ASM; break;
		case TI84p_APPL: action = DUSB_EID_APP; break;
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
	uint32_t size;
	uint8_t *data;
	int ret;

	ret = dusb_cmd_s_param_request(handle, 1, pid);
	if (!ret)
	{
		ret = dusb_cmd_r_screenshot(handle, &size, &data);
		if (!ret)
		{
			switch (handle->model)
			{
			case CALC_TI84P_USB:
			case CALC_TI82A_USB:
			{
				if (size == TI84P_ROWS * TI84P_COLS / 8)
				{
					*bitmap = data;
					sc->width = TI84P_COLS;
					sc->height = TI84P_ROWS;
					sc->clipped_width = TI84P_COLS;
					sc->clipped_height = TI84P_ROWS;
					sc->pixel_format = CALC_PIXFMT_MONO;
				}
				else
				{
					g_free(data);
					*bitmap = NULL;
					ret = ERR_INVALID_SCREENSHOT;
				}
				break;
			}
			case CALC_TI84PC_USB:
			{
				size -= 4;
				*bitmap = (uint8_t *)ticalcs_alloc_screen(TI84PC_ROWS * TI84PC_COLS * 2);
				ret = ticalcs_screen_84pcse_rle_uncompress(data, size, *bitmap, TI84PC_ROWS * TI84PC_COLS * 2);
				g_free(data);
				if (ret)
				{
					ticalcs_free_screen(*bitmap);
					*bitmap = NULL;
					ret = ERR_INVALID_SCREENSHOT;
				}
				else
				{
					sc->width = TI84PC_COLS;
					sc->height = TI84PC_ROWS;
					sc->clipped_width = TI84PC_COLS;
					sc->clipped_height = TI84PC_ROWS;
					sc->pixel_format = CALC_PIXFMT_RGB_565_LE;
				}
				break;
			}
			case CALC_TI83PCE_USB:
			case CALC_TI84PCE_USB:
			{
				if (size == TI84PC_ROWS * TI84PC_COLS * 2)
				{
					// 83PCE, 84PCE
					*bitmap = data;
					sc->width = TI84PC_COLS;
					sc->height = TI84PC_ROWS;
					sc->clipped_width = TI84PC_COLS;
					sc->clipped_height = TI84PC_ROWS;
					sc->pixel_format = CALC_PIXFMT_RGB_565_LE;
					break;
				}
				// else fall through.
			}
			default:
			{
				g_free(data);
				*bitmap = NULL;
				ret = ERR_INVALID_SCREENSHOT;
				break;
			}
			}
		}
	}

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	static const uint16_t aids[] = { DUSB_AID_VAR_SIZE, DUSB_AID_VAR_TYPE, DUSB_AID_ARCHIVED };
	const int size = sizeof(aids) / sizeof(uint16_t);
	int ret;
	DUSBCalcAttr **attr;
	GNode *folder, *root, *node;
	char fldname[40], varname[40];
	char *utf8;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}

	folder = dirlist_create_append_node(NULL, vars);
	if (!folder)
	{
		return ERR_MALLOC;
	}
	root = dirlist_create_append_node(NULL, apps);
	if (!root)
	{
		return ERR_MALLOC;
	}

	// Add permanent variables (Window, RclWin / RclWindw, TblSet aka WINDW, ZSTO, TABLE)
	{
		VarEntry *ve;

		ve = tifiles_ve_create();
		ticalcs_strlcpy(ve->name, "Window", sizeof(ve->name));
		ve->type = TI84p_WINDW;
		node = dirlist_create_append_node(ve, &folder);
		if (node != NULL)
		{
			ve = tifiles_ve_create();
			// Actually, "RclWindw" works even on an old 84+ running OS 2.43, but libticalcs
			// has been using "RclWin" successfully on TI-Z80 DUSB models since the beginning...
			ticalcs_strlcpy(ve->name, (handle->model == CALC_TI84PC_USB || handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB) ? "RclWindw" : "RclWin", sizeof(ve->name));
			ve->type = TI84p_ZSTO;
			node = dirlist_create_append_node(ve, &folder);
			if (node != NULL)
			{
				ve = tifiles_ve_create();
				ticalcs_strlcpy(ve->name, "TblSet", sizeof(ve->name));
				ve->type = TI84p_TABLE;
				node = dirlist_create_append_node(ve, &folder);
			}
		}
	}

	if (!node)
	{
		return ERR_MALLOC;
	}

	ret = dusb_cmd_s_dirlist_request(handle, size, aids);
	if (!ret)
	{
		for (;;)
		{
			VarEntry *ve;

			attr = dusb_ca_new_array(handle, size);
			ret = dusb_cmd_r_var_header(handle, fldname, varname, attr);
			if (ret)
			{
				// Not a real error.
				if (ret == ERR_EOT)
				{
					ret = 0;
				}
				dusb_ca_del_array(handle, size, attr);
				break;
			}

			ve = tifiles_ve_create();
			ticalcs_strlcpy(ve->name, varname, sizeof(ve->name));
			ve->size = (  (((uint32_t)(attr[0]->data[0])) << 24)
			            | (((uint32_t)(attr[0]->data[1])) << 16)
			            | (((uint32_t)(attr[0]->data[2])) <<  8)
			            | (((uint32_t)(attr[0]->data[3]))      ));
			ve->type = (uint32_t)(attr[1]->data[3]);
			ve->attr = attr[2]->data[0] ? ATTRB_ARCHIVED : ATTRB_NONE;
			dusb_ca_del_array(handle, size, attr);

			node = dirlist_create_append_node(ve, (ve->type != TI73_APPL) ? &folder : &root);
			if (!node)
			{
				ret = ERR_MALLOC;
				break;
			}

			utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
			ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s"), utf8);
			ticonv_utf8_free(utf8);
			ticalcs_update_label(handle);
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
	dusb_cp_del_array(handle, size, params);

	return ret;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	DUSBCalcAttr **attrs;
	const int nattrs = 3;
	uint32_t length;
	uint8_t *data, *p;
	int ret = 0;
	static const uint16_t keys[] = {
		KEY83P_Quit, KEY83P_LinkIO, KEY83P_Right, KEY83P_1
	};
	unsigned int i;

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = TI84p_BKUP;

	attrs[1] = dusb_ca_new(handle, DUSB_AID_VAR_VERSION, 4);
	attrs[1]->data[0] = 0x00; attrs[1]->data[1] = 0x00;
	attrs[1]->data[2] = 0x00; attrs[1]->data[3] = content->version;

	attrs[2] = dusb_ca_new(handle, DUSB_AID_BACKUP_HEADER, 8);
	attrs[2]->data[0] = MSB(content->data_length1);
	attrs[2]->data[1] = LSB(content->data_length1);
	attrs[2]->data[2] = MSB(content->data_length2);
	attrs[2]->data[3] = LSB(content->data_length2);
	attrs[2]->data[4] = MSB(content->data_length3);
	attrs[2]->data[5] = LSB(content->data_length3);
	attrs[2]->data[6] = MSB(content->mem_address);
	attrs[2]->data[7] = LSB(content->mem_address);

	length = content->data_length1 + content->data_length2 + content->data_length3;
	data = p = (uint8_t *)g_malloc(length);
	memcpy(p, content->data_part1, content->data_length1);
	p += content->data_length1;
	memcpy(p, content->data_part2, content->data_length2);
	p += content->data_length2;
	memcpy(p, content->data_part3, content->data_length3);

	// enter manual link mode
	for (i = 0; i < sizeof(keys) / sizeof(keys[0]) - 1; i++)
	{
		ret = send_key(handle, (uint32_t)(keys[i]));
		if (ret)
		{
			goto end;
		}
	}
	ret = send_key_noack(handle, (uint32_t)(keys[i]));
	if (ret)
	{
		goto end;
	}

	// send backup header
	ret = dusb_cmd_s_rts_ns(handle, "", "!", length, 3, CA(attrs));
	if (ret)
	{
		goto end;
	}
	ret = dusb_cmd_r_delay_ack(handle);
	if (ret)
	{
		goto end;
	}

	// press key to accept the backup
	ret = send_key_noack(handle, KEY83P_1);
	if (ret)
	{
		goto end;
	}

	// acknowledgement of RTS
	ret = dusb_cmd_r_data_ack(handle);
	if (ret)
	{
		goto end;
	}

	// send backup contents
	ret = dusb_cmd_s_var_content(handle, length, data);
	if (ret)
	{
		goto end;
	}
	ret = dusb_cmd_r_data_ack(handle);
	if (ret)
	{
		goto end;
	}
	ret = dusb_cmd_s_eot(handle);

 end:
	dusb_ca_del_array(handle, nattrs, attrs);
	g_free(data);
	
	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	unsigned int i;
	int ret = 0;

	for (i = 0; i < content->num_entries; i++)
	{
		DUSBCalcAttr **attrs;
		const int nattrs = 3;
		VarEntry *entry = content->entries[i];
		uint32_t size;

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

		ticonv_varname_to_utf8_sn(handle->model, entry->name, handle->updat->text, sizeof(handle->updat->text), entry->type);
		ticalcs_update_label(handle);

		attrs = dusb_ca_new_array(handle, nattrs);
		attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = entry->type;
		attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
		attrs[1]->data[0] = entry->attr == ATTRB_ARCHIVED ? 1 : 0;
		attrs[2] = dusb_ca_new(handle, DUSB_AID_VAR_VERSION, 4);
		attrs[2]->data[3] = entry->version;

		size = entry->size;
		if (entry->size >= 65536U)
		{
			ticalcs_critical("%s: variable size %u is suspiciously large", __FUNCTION__, size);
		}

		ret = dusb_cmd_s_rts(handle, "", entry->name, size, nattrs, CA(attrs));
		dusb_ca_del_array(handle, nattrs, attrs);
		if (ret)
		{
			break;
		}
		ret = dusb_cmd_r_data_ack(handle);
		if (ret)
		{
			break;
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
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION, DUSB_AID_VAR_SIZE };
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
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
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

				ve->size = (  (((uint32_t)(attrs[2]->data[0])) << 24)
					    | (((uint32_t)(attrs[2]->data[1])) << 16)
					    | (((uint32_t)(attrs[2]->data[2])) <<  8)
					    | (((uint32_t)(attrs[2]->data[3]))      ));

				if (attrs[0]->size == 1)
				{
					ve->attr = (attrs[0]->data[0] ? ATTRB_ARCHIVED : 0);
				}
				if (attrs[1]->size == 4)
				{
					ve->version = attrs[1]->data[3];
				}

				if (ve->type == TI83p_PIC)
				{
					if (ve->version >= 0xa)
					{
						content->model = CALC_TI84PC_USB;
					}
					else
					{
						content->model = CALC_TI84P_USB;
					}
				}

				ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size);
				memcpy(ve->data, data, ve->size);

				g_free(data);
			}
		}
		dusb_ca_del_array(handle, naids, attrs);
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
	unsigned int i;
	DUSBCalcAttr **attrs;
	const int nattrs = 2;
	int ret = 0;

	uint8_t *data;
	uint32_t size;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
		{
			break;
		}
	}
	if (   ptr == NULL
	    || ptr->data_type != TI83p_APPL
	    || ptr->pages == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}

#if 0
	ticalcs_debug("#pages: %i", ptr->num_pages);
	ticalcs_debug("type: %02x", ptr->data_type);
	for (i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		ticalcs_debug("page #%i: %04x %02x %02x %04x", i, fp->addr, fp->page, fp->flag, fp->size);
	}
	ticalcs_debug("data length: %08x", ptr->data_length);
#endif

	size = ptr->num_pages * FLASH_PAGE_SIZE;
	data = (uint8_t *)tifiles_fp_alloc_data(size);	// must be rounded-up
	if (data == NULL)
	{
		return ERR_MALLOC;
	}

	handle->updat->cnt2 = 0;
	handle->updat->max2 = ptr->num_pages;

	for (i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];
		memcpy(data + i*FLASH_PAGE_SIZE, fp->data, FLASH_PAGE_SIZE);

		handle->updat->cnt2 = i;
		ticalcs_update_pbar(handle);
	}
	{
		FlashPage *fp = ptr->pages[--i];
		memset(data + i*FLASH_PAGE_SIZE + fp->size, 0x00, FLASH_PAGE_SIZE - fp->size);

		handle->updat->cnt2 = i;
		ticalcs_update_pbar(handle);
	}

	// send
	ticonv_varname_to_utf8_sn(handle->model, ptr->name, handle->updat->text, sizeof(handle->updat->text), ptr->data_type);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
	attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
	attrs[1]->data[0] = 0;

	ret = dusb_cmd_s_rts(handle, "", ptr->name, size, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			ret = dusb_cmd_s_var_content(handle, size, data);
			if (!ret)
			{
				ret = dusb_cmd_r_data_ack(handle);
				if (!ret)
				{
					ret = dusb_cmd_s_eot(handle);
				}
			}
		}
	}

	g_free(data);

	return ret;
}

static int		send_flash_834pce	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	DUSBCalcAttr **attrs;
	const int nattrs = 2;
	int ret = 0;

	uint8_t *data;
	uint32_t size;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
		{
			break;
		}
	}
	if (   ptr == NULL
	    || ptr->data_type != TI83p_APPL
	    || ptr->data_part == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}

	size = ptr->data_length;
	data = ptr->data_part;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = 0;

	// send
	ticonv_varname_to_utf8_sn(handle->model, ptr->name, handle->updat->text, sizeof(handle->updat->text), ptr->data_type);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0F;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
	attrs[1] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
	attrs[1]->data[0] = 1;

	ret = dusb_cmd_s_rts(handle, "", ptr->name, size, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
		if (!ret)
		{
			ret = dusb_cmd_s_var_content(handle, size, data);
			if (!ret)
			{
				ret = dusb_cmd_r_data_ack(handle);
				if (!ret)
				{
					ret = dusb_cmd_s_eot(handle);
				}
			}
		}
	}

	return ret;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	uint32_t data_length;
	int page;
	uint16_t data_addr = 0x4000;
	uint16_t data_page = 0;
	int r, q;
	int ret;

	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
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
				content->device_type = DEVICE_TYPE_83P;

				q = data_length / FLASH_PAGE_SIZE;
				r = data_length % FLASH_PAGE_SIZE;
				content->num_pages = q + 1;
				content->pages = tifiles_fp_create_array(content->num_pages);

				handle->updat->cnt2 = 0;
				handle->updat->max2 = q;

				for (page = 0; page < q; page++)
				{
					FlashPage *fp = content->pages[page] = tifiles_fp_create();

					fp->addr = data_addr;
					fp->page = data_page++;
					fp->flag = 0x80;
					fp->size = FLASH_PAGE_SIZE;
					fp->data = (uint8_t *)tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
					memcpy(fp->data, data + FLASH_PAGE_SIZE*page, FLASH_PAGE_SIZE);

					handle->updat->cnt2 = page;
					ticalcs_update_pbar(handle);
				}
				{
					FlashPage *fp = content->pages[page] = tifiles_fp_create();

					fp->addr = data_addr;
					fp->page = data_page++;
					fp->flag = 0x80;
					fp->size = r;
					fp->data = (uint8_t *)tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
					memcpy(fp->data, data + FLASH_PAGE_SIZE*page, r);

					handle->updat->cnt2 = page;
					ticalcs_update_pbar(handle);
				}
				content->num_pages = page+1;

				g_free(data);
			}
		}
		dusb_ca_del_array(handle, naids, attrs);
	}

	return ret;
}

static int		recv_flash_834pce	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION };
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
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0F;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
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
				content->device_type = DEVICE_TYPE_83P;
				content->data_length = data_length;
				content->data_part = data; // Borrow this memory block.
				content->hw_id = handle->calc->product_id;

				// Do NOT g_free(data);
			}
		}
		dusb_ca_del_array(handle, naids, attrs);
	}

	return ret;
}

static int		send_os    (CalcHandle* handle, FlashContent* content)
{
	DUSBModeSet mode = { 2, 1, 0, 0, 0x0fa0 }; //MODE_BASIC;
	uint32_t pkt_size = 266;
	uint32_t os_size = 0;
	FlashContent *ptr;
	unsigned int i, j;
	int boot = 0;
	int ret;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
		{
			break;
		}
	}
	if (ptr == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->data_type != TI83p_AMS)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->pages == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}

#if 0
	ticalcs_debug("#pages: %i", ptr->num_pages);
	ticalcs_debug("type: %02x", ptr->data_type);
	for (i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		ticalcs_debug("page #%i: %04x %02x %02x %04x", i,
			fp->addr, fp->page, fp->flag, fp->size);
		//tifiles_hexdump(fp->data, 16);
	}
	ticalcs_debug("data length = %08x %i", ptr->data_length, ptr->data_length);
#endif

	for (i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		if (fp->size < 256)
		{
			os_size += 4;
		}
		else
		{
			os_size += 4*(fp->size / 260);
		}
	}
	ticalcs_debug("os_size overhead = %i", os_size);
	os_size += ptr->data_length;
	ticalcs_debug("os_size new = %i", os_size);

	do
	{
		static const uint16_t pids[] = { DUSB_PID_OS_MODE };
		const int size = sizeof(pids) / sizeof(uint16_t);
		DUSBCalcParam **params;

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

		// test for boot mode
		ret = dusb_cmd_s_param_request(handle, size, pids);
		if (ret)
		{
			break;
		}
		params = dusb_cp_new_array(handle, size);
		ret = dusb_cmd_r_param_data(handle, size, params);
		if (ret)
		{
			dusb_cp_del_array(handle, size, params);
			break;
		}
		boot = !params[0]->data[0];
		dusb_cp_del_array(handle, size, params);

		// start OS transfer
		ret = dusb_cmd_s_os_begin(handle, os_size);
		if (ret)
		{
			break;
		}
		if (!boot)
		{
			ret = dusb_recv_buf_size_request(handle, &pkt_size);
			if (ret)
			{
				break;
			}
			ret = dusb_send_buf_size_alloc(handle, pkt_size);
			if (ret)
			{
				break;
			}
		}
		ret = dusb_cmd_r_os_ack(handle, &pkt_size);	// this pkt_size is important
		if (ret)
		{
			break;
		}

		// send OS header/signature
		ret = dusb_cmd_s_os_header(handle, 0x4000, 0x7A, 0x80, pkt_size-4, ptr->pages[0]->data);
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
		handle->updat->cnt2 = 0;
		handle->updat->max2 = ptr->num_pages;

		for (i = 0; i < ptr->num_pages; i++)
		{
			FlashPage *fp = ptr->pages[i];

			fp->addr = 0x4000;

			if (i == 0)	// need relocation
			{
				ret = dusb_cmd_s_os_data(handle, 0x4000, 0x7A, 0x80, pkt_size-4, fp->data);
				if (ret)
				{
					goto end;
				}
				ret = dusb_cmd_r_data_ack(handle);
				if (ret)
				{
					goto end;
				}
			}
			else if (i == ptr->num_pages-1)	// idem
			{
				ret = dusb_cmd_s_os_data(handle, 0x4100, 0x7A, 0x80, pkt_size-4, fp->data);
				if (ret)
				{
					goto end;
				}
				ret = dusb_cmd_r_data_ack(handle);
				if (ret)
				{
					goto end;
				}
			}
			else
			{
				for (j = 0; j < fp->size; j += 256)
				{
					ret = dusb_cmd_s_os_data(handle,
						(uint16_t)(fp->addr + j), (uint8_t)fp->page, fp->flag,
						pkt_size-4, fp->data + j);
					if (ret)
					{
						goto end;
					}
					ret = dusb_cmd_r_data_ack(handle);
					if (ret)
					{
						goto end;
					}
				}
			}

			handle->updat->cnt2 = i;
			ticalcs_update_pbar(handle);
		}

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

static int		send_os_834pce    (CalcHandle* handle, FlashContent* content)
{
	DUSBModeSet mode = { 2, 1, 0, 0, 0x0fa0 }; //MODE_BASIC;
	uint32_t pkt_size = 0x3ff;
	uint32_t hdr_size = 0;
	uint32_t hdr_offset = 0;
	const uint32_t memory_offset = 0x30000U;
	FlashContent *ptr;
	uint8_t *d;
	int i, r, q;
	int ret;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
		{
			break;
		}
	}
	if (ptr == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->data_type != TI83p_AMS)
	{
		return ERR_INVALID_PARAMETER;
	}
	if (ptr->data_part == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}

	// search for OS header (offset & size)
	hdr_offset = 0;
	for (i = 0, d = ptr->data_part; (d[i] != 0xFF) || (d[i+1] != 0xFF) || (d[i+2] != 0xFF) || (d[i+3] != 0xFF); i++);
	hdr_size = i - hdr_offset;

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
		q = ptr->data_length / (pkt_size - 4);
		r = ptr->data_length % (pkt_size - 4);

		handle->updat->cnt2 = 0;
		handle->updat->max2 = q;

		for (i = 0; i < q; i++)
		{
			ret = dusb_cmd_s_os_data_834pce(handle, memory_offset + i*(pkt_size - 4), (pkt_size - 4), ptr->data_part + i*(pkt_size - 4));
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

		ret = dusb_cmd_s_os_data_834pce(handle, memory_offset + q*(pkt_size - 4), r, ptr->data_part + i*(pkt_size - 4));
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
	static const uint16_t aids[] = { DUSB_AID_ARCHIVED, DUSB_AID_VAR_VERSION };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	DUSBCalcAttr **attrs;
	const int nattrs = 1;
	char folder[40], name[40];
	uint8_t *data;
	uint32_t i, varsize;
	int ret;

	ticalcs_strlcpy(handle->updat->text, "ID-LIST", sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	attrs = dusb_ca_new_array(handle, nattrs);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = TI83p_IDLIST;

	ret = dusb_cmd_s_var_request(handle, "", "IDList", naids, aids, nattrs, CA(attrs));
	dusb_ca_del_array(handle, nattrs, attrs);
	if (!ret)
	{
		attrs = dusb_ca_new_array(handle, naids);
		ret = dusb_cmd_r_var_header(handle, folder, name, attrs);
		if (!ret)
		{
			ret = dusb_cmd_r_var_content(handle, &varsize, &data);
			if (!ret)
			{
				i = data[9];
				data[9] = data[10];
				data[10] = i;

				for (i = 4; i < varsize && i < 16; i++)
				{
					sprintf((char *)&id[2 * (i-4)], "%02x", data[i]);
				}
				id[7*2] = '\0';

				g_free(data);
			}
		}
		dusb_ca_del_array(handle, naids, attrs);
	}

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos);

static int		dump_rom_1	(CalcHandle* handle)
{
	CalcInfos infos;
	int ret;

	ret = get_version(handle, &infos);
	if (!ret)
	{
		PAUSE(100);
		if (infos.model == CALC_TI84P_USB)
		{
			ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize84p, romDump84p);
		}
		else if (infos.model == CALC_TI84PC_USB)
		{
			ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize84pcu, romDump84pcu);
		}
		else if (infos.model == CALC_TI82A_USB)
		{
			ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize82a, romDump82a);
		}
		else if (infos.model == CALC_TI84PCE_USB || infos.model == CALC_TI83PCE_USB)
		{
			ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize834pceu, romDump834pceu);
		}
		else
		{
			ret = 0;
		}
	}

	return ret;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	CalcInfos infos;
	int ret;

	ret = get_version(handle, &infos);
	if (!ret)
	{
		PAUSE(100);
		if (infos.model == CALC_TI84PCE_USB || infos.model == CALC_TI83PCE_USB)
		{
			// The TI-eZ80 series does no longer provide direct remote program launch...
			// Therefore, use a less sophisticated and more complicated way to queue keypresses.
			unsigned int i;
			static const uint16_t keys[] = {
				0x40, 0x09, 0x09, 0xFC9C, /* Quit, Clear, Clear, Asm( */
				0xDA, 0xAB, 0xA8, 0xA6,   /* prgm, R, O, M */
				0x9D, 0xAE, 0xA6, 0xA9,   /* D, U, M, P */
				0x86, 0x05                /* ), Enter */
			};

			// Launch program by remote control
			PAUSE(200);
			for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
			{
				ret = send_key(handle, (uint32_t)(keys[i]));
				if (ret)
				{
					break;
				}
				PAUSE(100);
			}
		}
		else
		{
			ret = dusb_cmd_s_execute(handle, "", "ROMDUMP", DUSB_EID_PRGM, "", 0);
			if (!ret)
			{
				ret = dusb_cmd_r_data_ack(handle);
			}
		}

		if (!ret)
		{
			PAUSE(3000);

			// Get dump
			ret = rd_read_dump(handle, filename);
		}
	}

	return ret;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	static const uint16_t pids[2] = { DUSB_PID_CLASSIC_CLK_SUPPORT, DUSB_PID_NEW_CLK_SUPPORT };
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
			int classic_clock = !!params[0]->ok && !!!params[1]->ok;
			int new_clock = !!!params[0]->ok && !!params[1]->ok;
			if ((!classic_clock && !new_clock) || (classic_clock && new_clock))
			{
				ticalcs_warning(_("Could not determine clock type: %u %u"), params[0]->ok, params[1]->ok);
			}
			else if (classic_clock)
			{
				uint32_t calc_time;
				struct tm ref, cur;
				time_t r, c, now;
				uint8_t data[4];

				ticalcs_info("%s", _("Will set classic clock"));

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
			}
			else /* if (new_clock) */
			{
				uint8_t data[4];

				ticalcs_info("%s", _("Will set new clock"));

				ticalcs_strlcpy(handle->updat->text, _("Setting clock..."), sizeof(handle->updat->text));
				ticalcs_update_label(handle);

				data[0] = _clock->date_format == 3 ? 0 : _clock->date_format;
				ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_DATE_FMT, 1, data);
				if (!ret)
				{
					data[0] = _clock->time_format == 24 ? 0x81 : 0x80;
					ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_TIME_FMT, 1, data);
					if (!ret)
					{
						data[0] = (uint8_t)(_clock->year >> 8);
						data[1] = _clock->year & 0xFF;
						ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_YEAR, 2, data);
						if (!ret)
						{
							data[0] = _clock->month;
							ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_MONTH, 1, data);
							if (!ret)
							{
								data[0] = _clock->day;
								ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_DAY, 1, data);
								if (!ret)
								{
									data[0] = _clock->hours;
									ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_HOURS, 1, data);
									if (!ret)
									{
										data[0] = _clock->minutes;
										ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_MINUTES, 1, data);
										if (!ret)
										{
											data[0] = _clock->seconds;
											ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_SECONDS, 1, data);
											if (!ret)
											{
												data[0] = _clock->state;
												ret = dusb_cmd_s_param_set_r_data_ack(handle, DUSB_PID_CLK_ON, 1, data);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	dusb_cp_del_array(handle, size, params);

	return ret;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	static const uint16_t pids[12] = {
		DUSB_PID_CLASSIC_CLK_SUPPORT, DUSB_PID_NEW_CLK_SUPPORT,
		DUSB_PID_CLK_ON, DUSB_PID_CLK_SEC_SINCE_1997, DUSB_PID_CLK_DATE_FMT, DUSB_PID_CLK_TIME_FMT,
		DUSB_PID_CLK_SECONDS, DUSB_PID_CLK_MINUTES, DUSB_PID_CLK_HOURS, DUSB_PID_CLK_DAY, DUSB_PID_CLK_MONTH, DUSB_PID_CLK_YEAR
	};
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
			int classic_clock = !!params[0]->ok && !!!params[1]->ok;
			int new_clock = !!!params[0]->ok && !!params[1]->ok;
			if ((!classic_clock && !new_clock) || (classic_clock && new_clock))
			{
				ticalcs_warning(_("Could not determine clock type: %u %u"), params[0]->ok, params[1]->ok);
			}
			else if (classic_clock)
			{
				if (   params[2]->ok && params[2]->size == 1
				    && params[3]->ok && params[3]->size == 4
				    && params[4]->ok && params[4]->size == 1
				    && params[5]->ok && params[5]->size == 1)
				{
					struct tm ref, cur;
					time_t r, c, now;
					uint8_t * data = params[3]->data;
					uint32_t calc_time = (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[2]) << 8) | (data[3] << 0);

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

					_clock->date_format = params[4]->data[0] == 0 ? 3 : params[4]->data[0];
					_clock->time_format = params[5]->data[0] ? 24 : 12;
					_clock->state = params[2]->data[0];
				}
				else
				{
					ticalcs_warning(_("Found classic clock but failed to retrieve its parameters: %u %u %u %u"),
					                params[2]->ok, params[3]->ok, params[4]->ok, params[5]->ok);
				}
			}
			else /* if (new_clock) */
			{
				if (   params[2]->ok && params[2]->size == 1
				    && params[4]->ok && params[4]->size == 1
				    && params[5]->ok && params[5]->size == 1
				    && params[6]->ok && params[6]->size == 1
				    && params[7]->ok && params[7]->size == 1
				    && params[8]->ok && params[8]->size == 1
				    && params[9]->ok && params[9]->size == 1
				    && params[10]->ok && params[10]->size == 1
				    && params[11]->ok && params[11]->size == 2)
				{
					uint8_t * data = params[11]->data;

					ticalcs_info("%s", _("Found valid new clock"));

					_clock->year = (((uint16_t)data[0]) << 8) | (data[1] << 0);
					_clock->month = params[10]->data[0];
					_clock->day = params[9]->data[0];
					_clock->hours = params[8]->data[0];
					_clock->minutes = params[7]->data[0];
					_clock->seconds = params[6]->data[0];

					_clock->date_format = params[4]->data[0] == 0 ? 3 : params[4]->data[0];
					_clock->time_format = params[5]->data[0] == 0x80 ? 12 : 24;
					_clock->state = params[2]->data[0];
				}
				else
				{
					ticalcs_warning(_("Found new clock but failed to retrieve its parameters: %u %u %u %u %u %u"),
					                params[6]->ok, params[7]->ok, params[8]->ok, params[9]->ok, params[10]->ok, params[11]->ok);
				}
			}
		}
	}
	dusb_cp_del_array(handle, size, params);

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	DUSBCalcAttr **attr;
	const int size = 1;
	char *utf8;
	int ret;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	attr = dusb_ca_new_array(handle, size);
	attr[0] = dusb_ca_new(handle, 0x0011, 4);
	attr[0]->data[0] = 0xF0; attr[0]->data[1] = 0x0B;
	attr[0]->data[2] = 0x00; attr[0]->data[3] = vr->type;

	ret = dusb_cmd_s_var_delete(handle, "", vr->name, size, CA(attr));
	dusb_ca_del_array(handle, size, attr);
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
	int ret;

	attrs = dusb_ca_new_array(handle, size);
	attrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = oldname->type;

	ret = dusb_cmd_s_var_modify(handle, "", oldname->name, 1, CA(attrs), "", newname->name, 0, NULL);
	dusb_ca_del_array(handle, size, attrs);
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
	int ret;

	srcattrs = dusb_ca_new_array(handle, 1);
	srcattrs[0] = dusb_ca_new(handle, DUSB_AID_VAR_TYPE2, 4);
	srcattrs[0]->data[0] = 0xF0; srcattrs[0]->data[1] = 0x07;
	srcattrs[0]->data[2] = 0x00; srcattrs[0]->data[3] = vr->type;

	dstattrs = dusb_ca_new_array(handle, 1);
	dstattrs[0] = dusb_ca_new(handle, DUSB_AID_ARCHIVED, 1);
	/* use 0xff here rather than 0x01 to work around an OS bug */
	dstattrs[0]->data[0] = (attr == ATTRB_ARCHIVED ? 0xff : 0x00);

	ret = dusb_cmd_s_var_modify(handle, "", vr->name, 1, CA(srcattrs), "", vr->name, 1, CA(dstattrs));
	dusb_ca_del_array(handle, 1, dstattrs);
	dusb_ca_del_array(handle, 1, srcattrs);
	if (!ret)
	{
		ret = dusb_cmd_r_data_ack(handle);
	}

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	static const uint16_t pids[] = {
		DUSB_PID_OS_MODE, DUSB_PID_DEVICE_TYPE, DUSB_PID_PRODUCT_NUMBER, DUSB_PID_PRODUCT_NAME, DUSB_PID_MAIN_PART_ID,
		DUSB_PID_HW_VERSION, DUSB_PID_LANGUAGE_ID, DUSB_PID_SUBLANG_ID,
		DUSB_PID_BOOT_BUILD_NUMBER, DUSB_PID_BOOT_VERSION, DUSB_PID_OS_BUILD_NUMBER, DUSB_PID_OS_VERSION,
		DUSB_PID_PHYS_RAM, DUSB_PID_USER_RAM, DUSB_PID_FREE_RAM,
		DUSB_PID_PHYS_FLASH, DUSB_PID_USER_FLASH, DUSB_PID_FREE_FLASH,
		DUSB_PID_LCD_WIDTH, DUSB_PID_LCD_HEIGHT, DUSB_PID_BITS_PER_PIXEL, DUSB_PID_COLOR_AVAILABLE,
		DUSB_PID_BATTERY_ENOUGH, DUSB_PID_MATH_CAPABILITIES, DUSB_PID_PYTHON_ON_BOARD, DUSB_PID_CLASSIC_CLK_SUPPORT
	};
	const int size = sizeof(pids) / sizeof(uint16_t);
	DUSBCalcParam **params;
	int i = 0;
	int ret;

	ticalcs_strlcpy(handle->updat->text, _("Getting version..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	memset(infos, 0, sizeof(CalcInfos));
	params = dusb_cp_new_array(handle, size);

	// TODO rewrite this function to ask for parameters in multiple phases, starting with 0x000A, then
	// model-dependent sets of parameters. That's how TI-Connect CE 5.x does.
	ret = dusb_cmd_s_param_request(handle, size, pids);
	if (!ret)
	{
		ret = dusb_cmd_r_param_data(handle, size, params);
		if (!ret)
		{
			uint8_t product_id = 0;
			uint8_t has_boot_build_number = 0;
			uint8_t has_os_build_number = 0;
			unsigned int infos_mask = 0;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->run_level = params[i]->data[0];
				infos_mask |= INFOS_RUN_LEVEL;
			}
			i++;

			if (params[i]->ok && params[i]->size == 2)
			{
				infos->device_type = params[i]->data[1];
				infos_mask |= INFOS_DEVICE_TYPE;
			}
			i++;

			if (params[i]->ok && params[i]->size == 4)
			{
				product_id = params[i]->data[3];
				infos_mask |= INFOS_PRODUCT_ID;
			}
			i++;

			if (params[i]->ok)
			{
				const uint32_t maxsize = params[i]->size < sizeof(infos->product_name) ? params[i]->size + 1 : sizeof(infos->product_name);
				ticalcs_strlcpy(infos->product_name, (char *)params[i]->data, maxsize);
				infos_mask |= INFOS_PRODUCT_NAME;
			}
			i++;

			if (params[i]->ok && params[i]->size == 5)
			{
				if ((infos_mask & INFOS_PRODUCT_ID) && product_id != params[i]->data[0])
				{
					ticalcs_warning("%s", _("That's odd, product ID and calc ID do not match ?"));
					// Nevertheless, we'll trust the product ID information (which tends to be hard-coded)
					// instead of the calc ID information (which is normally extracted from the cert memory).
				}
				ticalcs_slprintf(infos->main_calc_id, sizeof(infos->main_calc_id), "%02X%02X%02X%02X%02X",
				                 product_id, params[i]->data[1], params[i]->data[2], params[i]->data[3], params[i]->data[4]);
				infos_mask |= INFOS_MAIN_CALC_ID;
				ticalcs_strlcpy(infos->product_id, infos->main_calc_id, sizeof(infos->product_id));
				infos_mask |= INFOS_PRODUCT_ID;
			}
			i++;

			if (params[i]->ok && params[i]->size == 2)
			{
				infos->hw_version = (((uint16_t)params[i]->data[0]) << 8) | params[i]->data[1];
				infos_mask |= INFOS_HW_VERSION;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->language_id = params[i]->data[0];
				infos_mask |= INFOS_LANG_ID;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->sub_lang_id = params[i]->data[0];
				infos_mask |= INFOS_SUB_LANG_ID;
			}
			i++;

			if (params[i]->ok && params[i]->size == 2)
			{
				has_boot_build_number = 1;
			}
			i++;

			if (params[i]->ok && params[i]->size >= 3)
			{
				if (!has_boot_build_number)
				{
					ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1u.%02u", params[i]->data[1], params[i]->data[2]);
					infos_mask |= INFOS_BOOT_VERSION;
				}
				else if (params[i]->size == 4)
				{
					ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1u.%1u.%1u.%04u",
					                 params[i]->data[1], params[i]->data[2], params[i]->data[3],
					                 (((uint16_t)(params[i - 1]->data[0])) << 8) | params[i - 1]->data[1]);
					infos_mask |= INFOS_BOOT_VERSION;
				}
				// else do nothing.
			}
			i++;

			if (params[i]->ok && params[i]->size == 2)
			{
				has_os_build_number = 1;
			}
			i++;

			if (params[i]->ok && params[i]->size >= 3)
			{
				if (!has_os_build_number)
				{
					ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1u.%02u", params[i]->data[1], params[i]->data[2]);
					infos_mask |= INFOS_OS_VERSION;
				}
				else if (params[i]->size == 4)
				{
					ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1u.%1u.%1u.%04u",
					                 params[i]->data[1], params[i]->data[2], params[i]->data[3],
					                 (((uint16_t)(params[i - 1]->data[0])) << 8) | params[i - 1]->data[1]);
					infos_mask |= INFOS_OS_VERSION;
				}
				// else do nothing.
			}
			i++;

			if (params[i]->ok && params[i]->size == 8)
			{
				infos->ram_phys = (  (((uint64_t)(params[i]->data[0])) << 56)
				                   | (((uint64_t)(params[i]->data[1])) << 48)
				                   | (((uint64_t)(params[i]->data[2])) << 40)
				                   | (((uint64_t)(params[i]->data[3])) << 32)
				                   | (((uint64_t)(params[i]->data[4])) << 24)
				                   | (((uint64_t)(params[i]->data[5])) << 16)
				                   | (((uint64_t)(params[i]->data[6])) <<  8)
				                   | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_RAM_PHYS;
			}
			i++;
			if (params[i]->ok && params[i]->size == 8)
			{
				infos->ram_user = (  (((uint64_t)(params[i]->data[0])) << 56)
				                   | (((uint64_t)(params[i]->data[1])) << 48)
				                   | (((uint64_t)(params[i]->data[2])) << 40)
				                   | (((uint64_t)(params[i]->data[3])) << 32)
				                   | (((uint64_t)(params[i]->data[4])) << 24)
				                   | (((uint64_t)(params[i]->data[5])) << 16)
				                   | (((uint64_t)(params[i]->data[6])) <<  8)
				                   | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_RAM_USER;
			}
			i++;
			if (params[i]->ok && params[i]->size == 8)
			{
				infos->ram_free = (  (((uint64_t)(params[i]->data[0])) << 56)
				                   | (((uint64_t)(params[i]->data[1])) << 48)
				                   | (((uint64_t)(params[i]->data[2])) << 40)
				                   | (((uint64_t)(params[i]->data[3])) << 32)
				                   | (((uint64_t)(params[i]->data[4])) << 24)
				                   | (((uint64_t)(params[i]->data[5])) << 16)
				                   | (((uint64_t)(params[i]->data[6])) <<  8)
				                   | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_RAM_FREE;
			}
			i++;

			if (params[i]->ok && params[i]->size == 8)
			{
				infos->flash_phys = (  (((uint64_t)(params[i]->data[0])) << 56)
				                     | (((uint64_t)(params[i]->data[1])) << 48)
				                     | (((uint64_t)(params[i]->data[2])) << 40)
				                     | (((uint64_t)(params[i]->data[3])) << 32)
				                     | (((uint64_t)(params[i]->data[4])) << 24)
				                     | (((uint64_t)(params[i]->data[5])) << 16)
				                     | (((uint64_t)(params[i]->data[6])) <<  8)
				                     | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_FLASH_PHYS;
			}
			i++;
			if (params[i]->ok && params[i]->size == 8)
			{
				infos->flash_user = (  (((uint64_t)(params[i]->data[0])) << 56)
				                     | (((uint64_t)(params[i]->data[1])) << 48)
				                     | (((uint64_t)(params[i]->data[2])) << 40)
				                     | (((uint64_t)(params[i]->data[3])) << 32)
				                     | (((uint64_t)(params[i]->data[4])) << 24)
				                     | (((uint64_t)(params[i]->data[5])) << 16)
				                     | (((uint64_t)(params[i]->data[6])) <<  8)
				                     | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_FLASH_USER;
			}
			i++;
			if (params[i]->ok && params[i]->size == 8)
			{
				infos->flash_free = (  (((uint64_t)(params[i]->data[0])) << 56)
				                     | (((uint64_t)(params[i]->data[1])) << 48)
				                     | (((uint64_t)(params[i]->data[2])) << 40)
				                     | (((uint64_t)(params[i]->data[3])) << 32)
				                     | (((uint64_t)(params[i]->data[4])) << 24)
				                     | (((uint64_t)(params[i]->data[5])) << 16)
				                     | (((uint64_t)(params[i]->data[6])) <<  8)
				                     | (((uint64_t)(params[i]->data[7]))      ));
				infos_mask |= INFOS_FLASH_FREE;
			}
			i++;

			if (params[i]->ok && params[i]->size == 2)
			{
				infos->lcd_width = (  (((uint16_t)(params[i]->data[ 0])) <<  8)
				                    | (((uint16_t)(params[i]->data[ 1]))      ));
				infos_mask |= INFOS_LCD_WIDTH;
			}
			i++;
			if (params[i]->ok && params[i]->size == 2)
			{
				infos->lcd_height = (  (((uint16_t)(params[i]->data[ 0])) <<  8)
				                     | (((uint16_t)(params[i]->data[ 1]))      ));
				infos_mask |= INFOS_LCD_HEIGHT;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->bits_per_pixel = params[i]->data[0];
				infos_mask |= INFOS_BPP;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->color_screen = params[i]->data[0];
				infos_mask |= INFOS_COLOR_SCREEN;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->battery = params[i]->data[0];
				infos_mask |= INFOS_BATTERY_ENOUGH;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->exact_math = params[i]->data[0];
				infos_mask |= INFOS_MATH_CAPABILITIES;
			}
			i++;

			if (params[i]->ok && params[i]->size == 1)
			{
				infos->python_on_board = params[i]->data[0];
				infos_mask |= INFOS_PYTHON_ON_BOARD;
			}
			i++;

			switch (product_id)
			{
				case PRODUCT_ID_TI84P:
				{
					infos->model = CALC_TI84P_USB;
					if (infos->hw_version >= 4)
					{
						ticalcs_warning(_("Unhandled 84+ family member with product_id=%d hw_version=%d"), product_id, infos->hw_version);
					}
					break;
				}
				case PRODUCT_ID_TI82A:
				{
					infos->model = CALC_TI82A_USB;
					if (infos->hw_version >= 4)
					{
						ticalcs_warning(_("Unhandled 84+ family member with product_id=%d hw_version=%d"), product_id, infos->hw_version);
					}
					break;
				}
				case PRODUCT_ID_TI84PCSE:
				{
					infos->model = CALC_TI84PC_USB;
					if (infos->hw_version < 4)
					{
						ticalcs_warning(_("Unhandled 84+ family member with product_id=%d hw_version=%d"), product_id, infos->hw_version);
					}
					break;
				}
				case PRODUCT_ID_TI83PCE: // and case PRODUCT_ID_TI84PCE:
				{
					if (infos->exact_math)
					{
						infos->model = CALC_TI83PCE_USB;
					}
					else
					{
						infos->model = CALC_TI84PCE_USB;
					}
					if (infos->hw_version < 6)
					{
						ticalcs_warning(_("Unhandled 84+ family member with product_id=%d hw_version=%d"), product_id, infos->hw_version);
					}
					break;
				}
				default:
				{
					// Default to generic 84+(SE).
					infos->model = CALC_TI84P_USB;
					ticalcs_warning(_("Unhandled 84+ family member with product_id=%d hw_version=%d"), product_id, infos->hw_version);
					break;
				}
			}
			infos_mask |= INFOS_CALC_MODEL;
			infos->mask = (InfosMask)infos_mask;
		}
	}
	dusb_cp_del_array(handle, size, params);

	return ret;
}

extern const CalcFncts calc_84p_usb =
{
	CALC_TI84P_USB,
	"TI84+",
	"TI-84 Plus",
	N_("TI-84 Plus thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI84P,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "1P",   /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
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
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};

extern const CalcFncts calc_84pcse_usb =
{
	CALC_TI84PC_USB,
	"TI84+CSE",
	"TI-84 Plus C Silver Edition",
	N_("TI-84 Plus C Silver Edition thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI84PCSE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "1P",   /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
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
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};

extern const CalcFncts calc_83pce_usb =
{
	CALC_TI83PCE_USB,
	"TI83PCE",
	"TI-83 Premium CE",
	N_("TI-83 Premium CE thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	/*OPS_IDLIST |*/ OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI83PCE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
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
	&send_flash_834pce,
	&recv_flash_834pce,
	&send_os_834pce,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};

extern const CalcFncts calc_84pce_usb =
{
	CALC_TI84PCE_USB,
	"TI84+CE",
	"TI-84 Plus CE",
	N_("TI-84 Plus CE thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	/*OPS_IDLIST |*/ OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI84PCE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
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
	&send_flash_834pce,
	&recv_flash_834pce,
	&send_os_834pce,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};

extern const CalcFncts calc_82a_usb =
{
	CALC_TI82A_USB,
	"TI82A",
	"TI-82 Advanced",
	N_("TI-82 Advanced thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | /*OPS_FLASH |*/ OPS_OS |
	OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI82A,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "1P",   /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
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
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};

extern const CalcFncts calc_84pt_usb =
{
	CALC_TI84PT_USB,
	"TI84PT",
	"TI-84 Plus T",
	N_("TI-84 Plus T thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | /*OPS_FLASH |*/ OPS_OS |
	OPS_IDLIST | /*OPS_ROMDUMP |*/ OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	OPS_RENAME | OPS_CHATTR |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	PRODUCT_ID_TI84PT,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "1P",   /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
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
	 "2P",   /* send_all_vars_backup */
	 "2P",   /* recv_all_vars_backup */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
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
	&noop_new_folder,
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&rename_var,
	&change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup
};
