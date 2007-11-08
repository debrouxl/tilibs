/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include <ticonv.h>
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dusb_vpkt.h"
#include "dusb_cmd.h"

#include "keys83p.h"

// Screen coordinates of the TI83+
#define TI84P_ROWS  64
#define TI84P_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	ModeSet mode = MODE_NORMAL;

	TRYF(cmd_s_mode_set(handle, mode));
	TRYF(cmd_r_mode_ack(handle));
	// use PID_HOMESCREEN to return status ?

	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(cmd_s_execute(handle, "", "", EID_KEY, NULL, key));
	TRYF(cmd_r_delay_ack(handle));
	TRYF(cmd_r_data_ack(handle));

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	uint8_t action;

	switch(ve->type)
	{
	case TI84p_ASM:  action = EID_ASM; break;
	case TI84p_APPL: action = EID_APP; break;
	default:		 action = EID_PRGM; break;
	}

	TRYF(cmd_s_execute(handle, ve->folder, ve->name, action, args, 0));
	TRYF(cmd_r_data_ack(handle));

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t pid[] = { PID_SCREENSHOT };
	CalcParam **param;

	sc->width = TI84P_COLS;
	sc->height = TI84P_ROWS;
	sc->clipped_width = TI84P_COLS;
	sc->clipped_height = TI84P_ROWS;

	param = cp_new_array(1);
	TRYF(cmd_s_param_request(handle, 1, pid));
	TRYF(cmd_r_param_data(handle, 1, param));
	if(!param[0]->ok)
		return ERR_INVALID_PACKET;
	
	*bitmap = (uint8_t *)g_malloc(TI84P_COLS * TI84P_ROWS / 8);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, param[0]->data, TI84P_COLS * TI84P_ROWS / 8);

	cp_del_array(1, param);
	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	uint16_t aids[] = { AID_VAR_SIZE, AID_VAR_TYPE, AID_ARCHIVED, };
	const int size = sizeof(aids) / sizeof(uint16_t);
	TreeInfo *ti;
	int err;
	CalcAttr **attr;
	GNode *folder, *root;	
	char fldname[40], varname[40];
	char *utf8;

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

	folder = g_node_new(NULL);
	g_node_append(*vars, folder);

	root = g_node_new(NULL);
	g_node_append(*apps, root);

	// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
	{
		GNode *node;
		VarEntry *ve;

		ve = tifiles_ve_create();
		strcpy(ve->name, "Window");
		ve->type = TI84p_WINDW;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		strcpy(ve->name, "RclWin");
		ve->type = TI84p_ZSTO;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		strcpy(ve->name, "TblSet");
		ve->type = TI84p_TABLE;
		node = g_node_new(ve);
		g_node_append(folder, node);
	}

	TRYF(cmd_s_dirlist_request(handle, size, aids));
	do
	{
		VarEntry *ve = tifiles_ve_create();
		GNode *node;

		attr = ca_new_array(size);
		err = cmd_r_var_header(handle, fldname, varname, attr);
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		strcpy(ve->name, varname);
		ve->size = GINT32_FROM_BE(*((uint32_t *)(attr[0]->data)));
		ve->type = GINT32_FROM_BE(*((uint32_t *)(attr[1]->data))) & 0xff;
		ve->attr = attr[2]->data[0] ? ATTRB_ARCHIVED : ATTRB_NONE;
		ca_del_array(size, attr);

		node = g_node_new(ve);
		if (ve->type != TI73_APPL)
			g_node_append(folder, node);
		else
			g_node_append(root, node);

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), _("Parsing %s"), utf8);
		g_free(utf8);
		update_label();
	} while(1);

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t pids[] = { PID_FREE_RAM, PID_FREE_FLASH };
	const int size = sizeof(pids) / sizeof(uint16_t);
	CalcParam **params;

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
	const int nattrs = 3;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *ve = content->entries[i];
		
		if(ve->action == ACT_SKIP)
			continue;

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		attrs = ca_new_array(nattrs);
		attrs[0] = ca_new(AID_VAR_TYPE, 4);
		attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
		attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ve->type;
		attrs[1] = ca_new(AID_ARCHIVED, 1);
		attrs[1]->data[0] = ve->attr == ATTRB_ARCHIVED ? 1 : 0;
		attrs[2] = ca_new(AID_VAR_VERSION, 4);

		TRYF(cmd_s_rts(handle,"",ve->name,ve->size,nattrs,CA(attrs)));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_var_content(handle, ve->size, ve->data));
		TRYF(cmd_r_data_ack(handle));
		TRYF(cmd_s_eot(handle));

		PAUSE(50);	// needed
	}

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION, AID_VAR_SIZE };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	CalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	VarEntry *ve;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	TRYF(cmd_s_var_request(handle,"",vr->name,naids,aids,nattrs,CA(attrs)));
	ca_del_array(nattrs, attrs);
	attrs = ca_new_array(naids);
	TRYF(cmd_r_var_header(handle, fldname, varname, attrs));
	TRYF(cmd_r_var_content(handle, NULL, &data));

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_single());
    content->num_entries = 1;

    content->entries = tifiles_ve_create_array(1);	
    ve = content->entries[0] = tifiles_ve_create();
    memcpy(ve, vr, sizeof(VarEntry));

	ve->size = GINT32_FROM_BE(*((uint32_t *)(attrs[2]->data)));
	ve->data = tifiles_ve_alloc_data(ve->size);
	memcpy(ve->data, data, ve->size);
	
	g_free(data);	
	ca_del_array(naids, attrs);
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	TRYF(send_var(handle, MODE_BACKUP, (FileContent *)content));
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
	int i;
	char *utf8;
	CalcAttr **attrs;
	const int nattrs = 2;

	uint8_t *data;
	uint32_t size;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
		if(ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
			break;
	if(ptr == NULL)
		return -1;
	if(ptr->data_type != TI83p_APPL)
		return -1;

#if 0
	printf("#pages: %i\n", ptr->num_pages);
	printf("type: %02x\n", ptr->data_type);
	for (i = 0; i < ptr->num_pages; i++) 
	{
		FlashPage *fp = ptr->pages[i];

		printf("page #%i: %04x %02x %02x %04x\n", i,
			fp->addr, fp->page, fp->flag, fp->size);		
	}
	printf("data length: %08x\n", ptr->data_length);

	return 0;
#endif

	size = ptr->num_pages * FLASH_PAGE_SIZE;
	data = tifiles_fp_alloc_data(size);	// must be rounded-up

	update_->cnt2 = 0;
	update_->max2 = ptr->num_pages;

	for (i = 0; i < ptr->num_pages; i++) 
	{
		FlashPage *fp = ptr->pages[i];
		memcpy(data + i*FLASH_PAGE_SIZE, fp->data, FLASH_PAGE_SIZE);

		update_->cnt2 = i;
		update_->pbar();
	}
	{
		FlashPage *fp = ptr->pages[--i];
		memset(data + i*FLASH_PAGE_SIZE + fp->size, 0x00, FLASH_PAGE_SIZE - fp->size); 

		update_->cnt2 = i;
		update_->pbar();
	}

	// send
	utf8 = ticonv_varname_to_utf8(handle->model, ptr->name, ptr->data_type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = ptr->data_type;
	attrs[1] = ca_new(AID_ARCHIVED, 1);
	attrs[1]->data[0] = 0;
	
	TRYF(cmd_s_rts(handle, "", ptr->name, size, nattrs, CA(attrs)));
	TRYF(cmd_r_data_ack(handle));
	TRYF(cmd_s_var_content(handle, size, data));
	TRYF(cmd_r_data_ack(handle));
	TRYF(cmd_s_eot(handle));

	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	CalcAttr **attrs;
	const int nattrs = 1;
	char fldname[40], varname[40];
	uint8_t *data;
	char *utf8;
	
	int page;
	uint16_t data_addr = 0x4000;
	uint16_t data_page = 0;
	int r, q;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = vr->type;

	TRYF(cmd_s_var_request(handle, "", vr->name, naids, aids, 
			       nattrs, CA(attrs)));
	ca_del_array(nattrs, attrs);
	attrs = ca_new_array(naids);
	TRYF(cmd_r_var_header(handle, fldname, varname, attrs));
	TRYF(cmd_r_var_content(handle, NULL, &data));

	content->model = handle->model;
	strcpy(content->name, vr->name);
	content->data_type = vr->type;
	content->device_type = DEVICE_TYPE_83P;
	content->num_pages = 2048;	// TI83+ has 512 KB of FLASH max
	content->pages = tifiles_fp_create_array(content->num_pages);

	q = vr->size / FLASH_PAGE_SIZE;
	r = vr->size % FLASH_PAGE_SIZE;

	update_->cnt2 = 0;
	update_->max2 = q;

	for(page = 0; page < q; page++)
	{
		FlashPage *fp = content->pages[page] = tifiles_fp_create();

		fp->addr = data_addr;
		fp->page = data_page++;
		fp->flag = 0x80;
		fp->size = FLASH_PAGE_SIZE;			
		fp->data = tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
		memcpy(fp->data, data + FLASH_PAGE_SIZE*page, FLASH_PAGE_SIZE);

		update_->cnt2 = page;
		update_->pbar();
	}
	{
		FlashPage *fp = content->pages[page] = tifiles_fp_create();

		fp->addr = data_addr;
		fp->page = data_page++;
		fp->flag = 0x80;
		fp->size = r;			
		fp->data = tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
		memcpy(fp->data, data + FLASH_PAGE_SIZE*page, r);

		update_->cnt2 = page;
		update_->pbar();
	}
	content->num_pages = page+1;

	g_free(data);	
	ca_del_array(naids, attrs);
	return 0;
}

static int		send_os    (CalcHandle* handle, FlashContent* content)
{
	ModeSet mode = { 2, 1, 0, 0, 0x0fa0 }; //MODE_BASIC;
	uint32_t pkt_size = 266;
	uint32_t os_size = 0;
	FlashContent *ptr;
	int i, j;
	int boot = 0;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
		if(ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
			break;
	if(ptr == NULL)
		return -1;
	if(ptr->data_type != TI83p_AMS)
		return -1;

#if 0
	printf("#pages: %i\n", ptr->num_pages);
	printf("type: %02x\n", ptr->data_type);
	for (i = 0; i < ptr->num_pages; i++) 
	{
		FlashPage *fp = ptr->pages[i];

		printf("page #%i: %04x %02x %02x %04x\n", i,
			fp->addr, fp->page, fp->flag, fp->size);		
		//tifiles_hexdump(fp->data, 16);
	}
	printf("data length = %08x %i\n", ptr->data_length, ptr->data_length);
#endif

	for(i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		if(fp->size < 256)
			os_size += 4;
		else
			os_size += 4*(fp->size / 260);
	}
	printf("os_size overhead = %i\n", os_size);
	os_size += ptr->data_length;
	printf("os_size new = %i\n", os_size);

	// switch to BASIC mode
	TRYF(cmd_s_mode_set(handle, mode));
	TRYF(cmd_r_mode_ack(handle));

	// test for boot mode
	{
		uint16_t pids[] = { PID_OS_MODE };
		const int size = sizeof(pids) / sizeof(uint16_t);
		CalcParam **params;

		params = cp_new_array(size);
		TRYF(cmd_s_param_request(handle, size, pids));
		TRYF(cmd_r_param_data(handle, size, params));
		boot = !params[0]->data[0];
	}

	// start OS transfer
	TRYF(cmd_s_os_begin(handle, os_size));
	if(!boot)
	{
		TRYF(dusb_recv_buf_size_request(handle, &pkt_size));
		TRYF(dusb_send_buf_size_alloc(handle, pkt_size));
	}
	TRYF(cmd_r_os_ack(handle, &pkt_size));	// this pkt_size is important

	// send OS header/signature
	TRYF(cmd_s_os_header(handle, 0x4000, 0x7A, 0x80, pkt_size-4, ptr->pages[0]->data));
	TRYF(cmd_r_os_ack(handle, &pkt_size));

	// send OS data
	update_->cnt2 = 0;
	update_->max2 = ptr->num_pages;

	for(i = 0; i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		fp->addr = 0x4000;

		if(i == 0)	// need relocation
		{
			TRYF(cmd_s_os_data(handle, 0x4000, 0x7A, 0x80, pkt_size-4, fp->data));
			TRYF(cmd_r_data_ack(handle));
		}
		else if(i == ptr->num_pages-1)	// idem
		{
			TRYF(cmd_s_os_data(handle, 0x4100, 0x7A, 0x80, pkt_size-4, fp->data));
			TRYF(cmd_r_data_ack(handle));
		}
		else
		{
			for(j = 0; j < fp->size; j += 256/*(pkt_size-4)*/)
			{
				TRYF(cmd_s_os_data(handle, 
					(uint16_t)(fp->addr + j), (uint8_t)fp->page, fp->flag, 
					pkt_size-4, fp->data + j));
				TRYF(cmd_r_data_ack(handle));
			}
		}

		update_->cnt2 = i;
		update_->pbar();
	}
	
	TRYF(cmd_s_eot(handle));
	PAUSE(500);
	TRYF(cmd_r_eot_ack(handle));

	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	uint16_t aids[] = { AID_ARCHIVED, AID_VAR_VERSION };
	const int naids = sizeof(aids) / sizeof(uint16_t);
	CalcAttr **attrs;
	const int nattrs = 1;
	char folder[40], name[40];
	uint8_t *data;
	int i, varsize;

	g_snprintf(update_->text, sizeof(update_->text), "ID-LIST");
	update_label();

	attrs = ca_new_array(nattrs);
	attrs[0] = ca_new(AID_VAR_TYPE2, 4);
	attrs[0]->data[0] = 0xF0; attrs[0]->data[1] = 0x07;
	attrs[0]->data[2] = 0x00; attrs[0]->data[3] = TI83p_IDLIST;

	TRYF(cmd_s_var_request(handle, "", "IDList", naids, aids, 
			       nattrs, CA(attrs)));
	ca_del_array(nattrs, attrs);
	attrs = ca_new_array(naids);
	TRYF(cmd_r_var_header(handle, folder, name, attrs));
	TRYF(cmd_r_var_content(handle, (uint32_t *)&varsize, &data));

	i = data[9];
	data[9] = data[10];
	data[10] = i;

	for(i = 4; i < varsize && i < 16; i++)
		sprintf((char *)&id[2 * (i-4)], "%02x", data[i]);
	id[7*2] = '\0';

	g_free(data);
	ca_del_array(naids, attrs);
	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	return 0;
}
static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
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

    g_snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
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
	g_snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
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
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	attr = ca_new_array(size);
	attr[0] = ca_new(0x0011, 4);
	attr[0]->data[0] = 0xF0; attr[0]->data[1] = 0x0B;
	attr[0]->data[2] = 0x00; attr[0]->data[3] = vr->type;	
	attr[1] = ca_new(0x0013, 1);
	attr[1]->data[0] = vr->attr == ATTRB_ARCHIVED ? 1 : 0;

	TRYF(cmd_s_var_delete(handle, "", vr->name, size, CA(attr)));
	TRYF(cmd_r_data_ack(handle));

	ca_del_array(size, attr);
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint16_t pids[] = { 
		PID_PRODUCT_NAME, PID_MAIN_PART_ID,
		PID_HW_VERSION, PID_LANGUAGE_ID, PID_SUBLANG_ID, PID_DEVICE_TYPE,
		PID_BOOT_VERSION, PID_OS_VERSION, 
		PID_PHYS_RAM, PID_USER_RAM, PID_FREE_RAM,
		PID_PHYS_FLASH, PID_USER_FLASH, PID_FREE_FLASH,
		PID_LCD_WIDTH, PID_LCD_HEIGHT, PID_BATTERY, PID_OS_MODE,
	};
	const int size = sizeof(pids) / sizeof(uint16_t);
	CalcParam **params;
	int i = 0;

	g_snprintf(update_->text, sizeof(update_->text), _("Getting version..."));
    update_label();

	memset(infos, 0, sizeof(CalcInfos));
	params = cp_new_array(size);

	TRYF(cmd_s_param_request(handle, size, pids));
	TRYF(cmd_r_param_data(handle, size, params));

	strncpy(infos->product_name, (char*)params[i]->data, params[i]->size);
	infos->mask |= INFOS_PRODUCT_NAME;
	i++;

	g_snprintf(infos->main_calc_id, 10, "%02X%02X%02X%02X%02X", 
		params[i]->data[0], params[i]->data[1], params[i]->data[2], params[i]->data[3], params[i]->data[4]);
	infos->mask |= INFOS_MAIN_CALC_ID;
	strcpy(infos->product_id, infos->main_calc_id);
	infos->mask |= INFOS_PRODUCT_ID;
	i++;

	infos->hw_version = (params[i]->data[0] << 8) | params[i]->data[1];
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

	g_snprintf(infos->boot_version, 5, "%1i.%02i", params[i]->data[1], params[i]->data[2]);
	infos->mask |= INFOS_BOOT_VERSION;
	i++;

	g_snprintf(infos->os_version, 5, "%1i.%02i", params[i]->data[1], params[i]->data[2]);
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

	infos->bits_per_pixel = 1;
	infos->mask |= INFOS_BPP;

	infos->battery = params[i]->data[0];
	infos->mask |= INFOS_BATTERY;
	i++;

	infos->run_level = params[i]->data[0];
	infos->mask |= INFOS_RUN_LEVEL;
	i++;

	switch(infos->hw_version)
	{
		case 0: infos->model = CALC_TI83P; break;
		case 1: infos->model = CALC_TI83P; break;
		case 2: infos->model = CALC_TI84P; break;
		case 3: infos->model = CALC_TI84P; break;
	}
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

const CalcFncts calc_84p_usb = 
{
	CALC_TI84P_USB,
	"TI84+",
	"TI-84 Plus",
	N_("TI-84 Plus thru DirectLink"),
	OPS_ISREADY | OPS_SCREEN | OPS_DIRLIST | OPS_VARS | OPS_FLASH | OPS_OS |
	OPS_IDLIST | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_BACKUP | OPS_KEYS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH,
	{"", "", "1P", "1L", "", "2P", "2P", "2P1L", "1P1L", "2P1L", "1P1L", "2P1L", "2P1L",
		"2P", "1L", "2P", "", "", "1L", "1L", "", "1L", "1L" },
	&is_ready,
	&send_key,
	&execute,
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
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
