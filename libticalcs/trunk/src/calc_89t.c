/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Lievin
 *  Copyright (c) 2005, Benjamin Moody (ROM dumper)
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include "ticonv.h"
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dusb_vpkt.h"
#include "dusb_cmd.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the Titanium
#define TI89T_ROWS			128
#define TI89T_COLS			240
#define TI89T_ROWS_VISIBLE	100
#define TI89T_COLS_VISIBLE	160

static int		is_ready	(CalcHandle* handle)
{
	ModeSet mode = MODE_NORMAL;

	TRYF(cmd_s_mode_set(handle, mode));
	TRYF(cmd_r_mode_ack(handle));

	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t pid[] = { PID_SCREENSHOT };
	CalcParam **param;

	sc->width = TI89T_COLS;
	sc->height = TI89T_ROWS;
	sc->clipped_width = TI89T_COLS_VISIBLE;
	sc->clipped_height = TI89T_ROWS_VISIBLE;
    
	param = cp_new_array(1);
	TRYF(cmd_s_param_request(handle, 1, pid));
	//TRYF(cmd_r_param_ack(handle));
	TRYF(cmd_r_param_data(handle, 1, param));
	if(!param[0]->ok)
		return ERR_INVALID_PACKET;
	
	*bitmap = (uint8_t *) malloc(TI89T_COLS * TI89T_ROWS / 8);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, param[0]->data, TI89T_COLS * TI89T_ROWS / 8);

	{
		int i, j, k;

		for(i = 0, j = 0; j < TI89T_ROWS_VISIBLE; j++)
			for(k = 0; k < (TI89T_COLS_VISIBLE >> 3); k++)
				(*bitmap)[i++] = (*bitmap)[j * (TI89T_COLS >> 3) + k];
	}

	cp_del_array(1, param);
	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	uint16_t aids[] = { AID_VAR_TYPE, AID_ARCHIVED, AID_4APPVAR, AID_VAR_SIZE, AID_LOCKED, AID_UNKNOWN_42 };
	const int size = sizeof(aids) / sizeof(uint16_t);
	TreeInfo *ti;
	int err;
	CalcAttr **attr;
	TNode *folder = NULL;	
	char fldname[40];
	char varname[40];
	char folder_name[40] = "";
	char *u1, *u2;

	(*apps) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

    (*vars) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	folder = t_node_new(NULL);
	t_node_append(*vars, folder);

	TRYF(cmd_s_dirlist_request(handle, size, aids));
	for(;;)
	{
		VarEntry *ve = tifiles_ve_create();
		TNode *node;

		attr = ca_new_array(size);
		err = cmd_r_var_header(handle, fldname, varname, attr);
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		strcpy(ve->folder, fldname);
		strcpy(ve->name, varname);
		ve->size = GINT32_FROM_BE(*((uint32_t *)(attr[3]->data)));
		ve->type = GINT32_FROM_BE(*((uint32_t *)(attr[0]->data))) & 0xff;
		ve->attr = attr[1]->data[0] ? ATTRB_ARCHIVED : attr[4]->data[0] ? ATTRB_LOCKED : ATTRB_NONE;
		ca_del_array(size, attr);

		if(ve->type == TI89_DIR)
		{
			strcpy(folder_name, ve->folder);
			strcpy(ve->name, ve->folder);
			strcpy(ve->folder, "");
			
			node = t_node_new(ve);
			folder = t_node_append(*vars, node);
		}
		else
		{
			if(!strcmp(ve->folder, "main") && (!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
				tifiles_ve_delete(ve);
			else
			{
				node = t_node_new(ve);
				if (ve->type != TI73_APPL)
					t_node_append(folder, node);
				else
					t_node_append(*apps, node);
			}
		}

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			ve->name, 
			tifiles_vartype2string(handle->model, ve->type),
			ve->attr,
			ve->size);

		u1 = ticonv_varname_to_utf8(handle->model, ((VarEntry *) (folder->data))->name);
		u2 = ticonv_varname_to_utf8(handle->model, ve->name);
			snprintf(update_->text, sizeof(update_->text), _("Reading of '%s/%s'"), u1, u2);
			g_free(u1); g_free(u2);
			update_label();
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t pids[] = { PID_FREE_RAM, PID_FREE_FLASH };
	const int size = sizeof(pids) / sizeof(uint16_t);
	CalcParam **params;
	int i = 0;

	params = cp_new_array(size);
	TRYF(cmd_s_param_request(handle, size, pids));
	TRYF(cmd_r_param_data(handle, size, params));

	*ram = (uint32_t)GINT64_FROM_BE(*((uint64_t *)(params[0]->data)));
	*flash = (uint32_t)GINT64_FROM_BE(*((uint64_t *)(params[1]->data)));

	cp_del_array(size, params);
	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	char *utf8;
	CalcAttr **attrs;
	const int nattrs = 4;

	update_->max2 = content->num_entries;
	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *ve = content->entries[i];
		
		if(ve->action == ACT_SKIP)
			continue;

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name);
		snprintf(update_->text, sizeof(update_->text), _("Sending '%s'"), utf8);
		g_free(utf8);
		update_label();

		attrs = ca_new_array(nattrs);
		attrs[0] = ca_new(AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ve->type;
		attrs[1] = ca_new(AID_ARCHIVED, 1);
		attrs[1]->data[0] = ve->attr == ATTRB_ARCHIVED ? 1 : 0;
		attrs[2] = ca_new(AID_VAR_VERSION, 4);
		attrs[2]->data[0] = 0;
		attrs[3] = ca_new(AID_LOCKED, 1);
		attrs[3]->data[0] = ve->attr == ATTRB_LOCKED ? 1 : 0;

		TRYF(cmd_s_rts(handle, ve->folder, ve->name, ve->size, nattrs, attrs));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_var_content(handle, ve->size, ve->data));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_eot(handle));

		if(mode & MODE_BACKUP) 
		{
			update_->cnt2 = i+1;
			update_->max2 = content->num_entries;
			update_->pbar();
		}

		PAUSE(50);	// needed
	}

	return 0;	
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION, AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	CalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	VarEntry *ve;

	snprintf(update_->text, sizeof(update_->text), _("Receiving '%s'"), vr->name);
    update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	TRYF(cmd_s_var_request(handle, vr->folder, vr->name, naids, aids, nattrs, attrs));
	ca_del_array(nattrs, attrs);
	attrs = ca_new_array(nattrs);
	TRYF(cmd_r_var_header(handle, fldname, varname, attrs));
	TRYF(cmd_r_var_content(handle, NULL, &data));

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_single());
    content->num_entries = 1;

    content->entries = tifiles_ve_create_array(1);	
    ve = content->entries[0] = tifiles_ve_create();
    memcpy(ve, vr, sizeof(VarEntry));

	ve->data = tifiles_ve_alloc_data(ve->size);
	memcpy(ve->data, data, ve->size);
	free(data);	

	ca_del_array(nattrs, attrs);
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	TRYF(send_var(handle, MODE_BACKUP, (FileContent *)content));
	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** ve)
{
	return 0;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	char *utf8;
	CalcAttr **attrs;
	const int nattrs = 4;

	// send all headers except license
	for(ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if(ptr->data_type == TI89_LICENSE)
			continue;

		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		utf8 = ticonv_varname_to_utf8(handle->model, ptr->name);
		snprintf(update_->text, sizeof(update_->text), _("Sending '%s'"), utf8);
		g_free(utf8);
		update_label();

		attrs = ca_new_array(nattrs);
		attrs[0] = ca_new(AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
		attrs[1] = ca_new(AID_ARCHIVED, 1);
		attrs[1]->data[0] = 0;
		attrs[2] = ca_new(AID_VAR_VERSION, 4);
		attrs[2]->data[3] = 1;
		attrs[3] = ca_new(AID_LOCKED, 1);
		attrs[3]->data[0] = 0;
		
		TRYF(cmd_s_rts(handle, "", ptr->name, ptr->data_length, nattrs, attrs));
		TRYF(cmd_r_param_ack(handle));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_var_content(handle, ptr->data_length, ptr->data_part));
		TRYF(cmd_r_param_ack(handle));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_eot(handle));
	}

	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION, AID_LOCKED };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	CalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	
	snprintf(update_->text, sizeof(update_->text), _("Receiving '%s'"), vr->name);
    update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	TRYF(cmd_s_var_request(handle, "", vr->name, naids, aids, nattrs, attrs));
	ca_del_array(nattrs, attrs);
	attrs = ca_new_array(nattrs);
	TRYF(cmd_r_var_header(handle, fldname, varname, attrs));
	TRYF(cmd_r_var_content(handle, NULL, &data));

	content->model = handle->model;
	strcpy(content->name, vr->name);
	content->data_length = vr->size;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(vr->size);

	memcpy(content->data_part, data, content->data_length);
	free(data);

	ca_del_array(nattrs, attrs);
	return 0;
}

static int		send_os    (CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	uint16_t pid[] = { PID_FULL_ID };
	CalcParam **param;

	param = cp_new_array(1);
	TRYF(cmd_s_param_request(handle, 1, pid));
	TRYF(cmd_r_param_data(handle, 1, param));
	if(!param[0]->ok)
		return ERR_INVALID_PACKET;

	memcpy(&id[0], &(param[0]->data[1]), 5);
	memcpy(&id[5], &(param[0]->data[7]), 5);
	memcpy(&id[10], &(param[0]->data[13]), 5);
	id[17] = '\0';

	return 0;
}

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{
	CalcParam *param;

	uint32_t calc_time;
	struct tm ref, cur;
	time_t r, c, now;

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

	cur.tm_year = clock->year - 1900;
	cur.tm_mon = clock->month - 1;
	cur.tm_mday = clock->day;	
	cur.tm_hour = clock->hours;
	cur.tm_min = clock->minutes;
	cur.tm_sec = clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);
	
	calc_time = (uint32_t)difftime(c, r);

    snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
    update_label();

	param = cp_new(PID_CLK_SEC, 4);
	param->data[0] = MSB(MSW(calc_time));
    param->data[1] = LSB(MSW(calc_time));
    param->data[2] = MSB(LSW(calc_time));
    param->data[3] = LSB(LSW(calc_time));
	TRYF(cmd_s_param_set(handle, param));
	TRYF(cmd_r_data_ack(handle));
	cp_del(param);

	param = cp_new(PID_CLK_DATE_FMT, 1);
	param->data[0] = clock->date_format == 3 ? 0 : clock->date_format;
	TRYF(cmd_s_param_set(handle, param));
	TRYF(cmd_r_data_ack(handle));
	cp_del(param);

	param = cp_new(PID_CLK_TIME_FMT, 1);
	param->data[0] = clock->time_format == 24 ? 1 : 0;
	TRYF(cmd_s_param_set(handle, param));
	TRYF(cmd_r_data_ack(handle));
	cp_del(param);

	param = cp_new(PID_CLK_ON, 1);
	param->data[0] = clock->state;
	TRYF(cmd_s_param_set(handle, param));	
	TRYF(cmd_r_data_ack(handle));
	cp_del(param);

	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	uint16_t pids[4] = { PID_CLK_SEC, PID_CLK_DATE_FMT, PID_CLK_TIME_FMT, PID_CLK_ON };
	const int size = sizeof(pids) / sizeof(uint16_t);
	CalcParam **params;

	uint32_t calc_time;
	struct tm ref, *cur;
	time_t r, c, now;

	// get raw clock
	snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
    update_label();

	params = cp_new_array(size);
	TRYF(cmd_s_param_request(handle, size, pids));
	TRYF(cmd_r_param_data(handle, size, params));
	if(!params[0]->ok)
		return ERR_INVALID_PACKET;
	
	// and computes
	calc_time = (params[0]->data[0] << 24) | (params[0]->data[1] << 16) | 
				(params[0]->data[2] <<  8) | (params[0]->data[3] <<  0);

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

	clock->year = cur->tm_year + 1900;
	clock->month = cur->tm_mon + 1;
	clock->day = cur->tm_mday;
	clock->hours = cur->tm_hour;
	clock->minutes = cur->tm_min;
	clock->seconds = cur->tm_sec;

    clock->date_format = params[1]->data[0] == 0 ? 3 : params[1]->data[0];
    clock->time_format = params[2]->data[0] ? 24 : 12;
	clock->state = params[3]->data[0];

	cp_del_array(1, params);

	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	CalcAttr **attr;
	const int size = 2;

	attr = ca_new_array(size);

	attr[0] = ca_new(AID_VAR_TYPE2, 4);
	attr[0]->data[0] = 0xF0; attr[0]->data[1] = 0x0C;
	attr[0]->data[2] = 0x00; attr[0]->data[3] = vr->type;
	
	attr[1] = ca_new(AID_UNKNOWN_13, 1);
	attr[1]->data[0] = 0;

	TRYF(cmd_s_var_delete(handle, vr->folder, vr->name, size, attr));
	TRYF(cmd_r_param_ack(handle));
	TRYF(cmd_r_data_ack(handle));	

	ca_del_array(size, attr);
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char *fldname = vr->folder;
	char varname[40] = "a1234567";
	CalcParam *param;
	CalcAttr **attrs;
	const int nattrs = 4;

	// send empty expression in specified folder
	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x0C;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = 0x00;
	attrs[1] = ca_new(AID_ARCHIVED, 1);
	attrs[1]->data[0] = 0;
	attrs[2] = ca_new(AID_VAR_VERSION, 4);
	attrs[2]->data[0] = 0;
	attrs[3] = ca_new(AID_LOCKED, 1);
	attrs[3]->data[0] = 0;

	TRYF(cmd_s_rts(handle, fldname, varname, sizeof(data), nattrs, attrs));
	TRYF(cmd_r_data_ack(handle));
	TRYF(cmd_s_var_content(handle, sizeof(data), data));
	TRYF(cmd_r_data_ack(handle));
	TRYF(cmd_s_eot(handle));

	// go back to HOME screen
	param = cp_new(PID_HOMESCREEN, 1);
	param->data[0] = 1;
	TRYF(cmd_s_param_set(handle, param));
	TRYF(cmd_r_data_ack(handle));
	cp_del(param);

	// delete 'a1234567' variable
	strcpy(vr->name, "a1234567");
	TRYF(del_var(handle, vr));

	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint16_t pids[] = { 
		PID_PRODUCT_NAME, PID_MAIN_PART_ID,
		PID_HW_VERSION, PID_LANGUAGE_ID, PID_SUBLANG_ID, PID_DEVICE_TYPE,
		PID_BOOT_VERSION, PID_OS_VERSION, 
		PID_PHYS_RAM, PID_USER_RAM, PID_FREE_RAM,
		PID_PHYS_FLASH, PID_FREE_FLASH, PID_FREE_FLASH,
		PID_LCD_WIDTH, PID_LCD_HEIGHT, PID_BATTERY,
	};
	const int size = sizeof(pids) / sizeof(uint16_t);
	CalcParam **params;
	int i = 0;

	snprintf(update_->text, sizeof(update_->text), _("Getting version..."));
    update_label();

	memset(infos, 0, sizeof(CalcInfos));
	params = cp_new_array(size);

	TRYF(cmd_s_param_request(handle, size, pids));
	TRYF(cmd_r_param_data(handle, size, params));

	strncpy(infos->product_name, params[i]->data, params[i]->size);
	infos->mask |= INFOS_PRODUCT_NAME;
	i++;

	strncpy(infos->main_calc_id, &(params[i]->data[1]), 5);
	strncpy(infos->main_calc_id+5, &(params[i]->data[7]), 5);
	infos->mask |= INFOS_MAIN_CALC_ID;
	i++;

	infos->hw_version = ((params[i]->data[0] << 8) | params[i]->data[1]) + 1;
	infos->mask |= INFOS_HW_VERSION; // hw version or model ?
	i++;

	infos->language_id = params[i]->data[0];
	infos->mask |= INFOS_LANG_ID;
	i++;

	infos->sub_lang_id = params[i]->data[0];
	infos->mask |= INFOS_SUB_LANG_ID;
	i++;

	infos->device_type = params[i]->data[1];
	infos->mask |= INFOS_DEVICE_TYPE;
	i++;

	snprintf(infos->boot_version, 4, "%1i.%02i", params[i]->data[1], params[i]->data[2]);
	infos->mask |= INFOS_BOOT_VERSION;
	i++;

	snprintf(infos->os_version, 4, "%1i.%02i", params[i]->data[1], params[i]->data[2]);
	infos->mask |= INFOS_OS_VERSION;
	i++;

	infos->ram_phys = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_RAM_PHYS;
	i++;
	infos->ram_user = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_RAM_USER;
	i++;
	infos->ram_free = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_RAM_FREE;
	i++;

	infos->flash_phys = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_FLASH_PHYS;
	i++;
		infos->flash_user = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_FLASH_USER;
	i++;
	infos->flash_free = GINT64_FROM_BE(*((uint64_t *)(params[i]->data)));
	infos->mask |= INFOS_FLASH_FREE;
	i++;

	infos->lcd_width = GINT16_FROM_BE(*((uint16_t *)(params[i]->data)));
	infos->mask |= INFOS_LCD_WIDTH;
	i++;
	infos->lcd_height = GINT16_FROM_BE(*((uint16_t *)(params[i]->data)));
	infos->mask |= INFOS_LCD_HEIGHT;
	i++;

	infos->battery = params[i]->data[0];
	infos->mask |= INFOS_BATTERY;
	i++;

	infos->model = CALC_TI89T;
	infos->mask |= INFOS_CALC_MODEL;

	cp_del_array(size, params);
	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

extern int tixx_recv_backup(CalcHandle* handle, BackupContent* content);

const CalcFncts calc_89t_usb = 
{
	CALC_TI89T_USB,
	"Titanium (USB)",
	N_("TI-89 Titanium thru DirectLink USB"),
	N_("TI-89 Titanium thru DirectLink USB"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	OPS_IDLIST | OPS_CLOCK | OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_BACKUP | 
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	&is_ready,
	&send_key,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&tixx_recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_os,
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
