/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Lievin
 *  Copyright (c) 2005  Benjamin Moody (ROM dumper)
 *  Copyright (c) 2006  Tyler Cassidy
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
	TI73/TI83+/TI84+ support.
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
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd73.h"
#include "rom73.h"
#include "rom83p.h"
#include "romdump.h"
#include "keys83p.h"

// Screen coordinates of the TI83+
#define TI73_ROWS  64
#define TI73_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	TRYF(ti73_send_RDY(handle));
	TRYF(ti73_recv_ACK(handle, &status));

	return (MSB(status) & 0x01) ? ERR_NOT_READY : 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti73_send_KEY(handle, key));
	TRYF(ti73_recv_ACK(handle, &key));	// when the key is received
	TRYF(ti73_recv_ACK(handle, NULL));	// after it completes the resulting action

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	unsigned int i;

	if (handle->model == CALC_TI73 && ve->type == TI73_ASM)
		return ERR_VOID_FUNCTION;

	// Go back to homescreen
	PAUSE(200);
	TRYF(send_key(handle, KEY83P_Quit));
	TRYF(send_key(handle, KEY83P_Clear));
	TRYF(send_key(handle, KEY83P_Clear));

	// Launch program by remote control
	if(ve->type == TI83p_ASM)
	{
		TRYF(send_key(handle, KEY83P_Asm));
	}
	TRYF(send_key(handle, KEY83P_Exec));

	for(i = 0; i < strlen(ve->name); i++)
	{
		const CalcKey *ck = ticalcs_keys_83p((ve->name)[i]);
		TRYF(send_key(handle, ck->normal.value));
	}

	TRYF(send_key(handle, KEY83P_Enter));
	PAUSE(200);

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;
	uint8_t buf[TI73_COLS * TI73_ROWS / 8];

	sc->width = TI73_COLS;
	sc->height = TI73_ROWS;
	sc->clipped_width = TI73_COLS;
	sc->clipped_height = TI73_ROWS;

	TRYF(ti73_send_SCR(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	err = ti73_recv_XDP(handle, &max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti73_send_ACK(handle));

	*bitmap = (uint8_t *)g_malloc(TI73_COLS * TI73_ROWS / 8);
	if(*bitmap == NULL)
		return ERR_MALLOC;
	memcpy(*bitmap, buf, TI73_COLS * TI73_ROWS  / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	TreeInfo *ti;
	uint16_t unused;
	uint32_t memory;
	GNode *folder, *root;	
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

	TRYF(ti73_send_REQ(handle, 0x0000, TI73_DIR, "", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_XDP(handle, &unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti73_send_ACK(handle));
	ti->mem_free = memory;

	folder = g_node_new(NULL);
	g_node_append(*vars, folder);

	root = g_node_new(NULL);
	g_node_append(*apps, root);

	// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
	{
		GNode *node;
		VarEntry *ve;

		ve = tifiles_ve_create();
		ve->type = TI84p_WINDW;
		node = g_node_new(ve);
		g_node_append(folder, node);

		if(handle->model != CALC_TI73)
		{
			ve = tifiles_ve_create();
			ve->type = TI84p_ZSTO;
			node = g_node_new(ve);
			g_node_append(folder, node);
		}

		ve = tifiles_ve_create();
		ve->type = TI84p_TABLE;
		node = g_node_new(ve);
		g_node_append(folder, node);
	}

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		GNode *node;
		int err;
		uint16_t ve_size;

		err = ti73_recv_VAR(handle, &ve_size, &ve->type, ve->name, &ve->attr);
		ve->size = ve_size;
		TRYF(ti73_send_ACK(handle));
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		if (ve->type == TI73_APPL)
		{
			/* Size is reported as a number of pages -- compute amount
			   of space consumed (the actual application may be
			   somewhat smaller.)  Note: the MSB of the "size" word is
			   the application's starting page number. */
			ve->size = (ve->size & 0xff) * 0x4000;
		}

		node = g_node_new(ve);
		if (ve->type != TI73_APPL)
			g_node_append(folder, node);
		else
			g_node_append(root, node);

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), _("Parsing %s"), utf8);
		g_free(utf8);
		update_label();
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t unused;
	uint32_t memory;

	TRYF(ti73_send_REQ(handle, 0x0000, TI73_DIR, "", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_XDP(handle, &unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti73_send_EOT(handle));
	*ram = memory;
	*flash = -1;

	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	uint16_t length;
	char varname[9];
	uint8_t rej_code;

	length = content->data_length1;
	varname[0] = LSB(content->data_length2);
	varname[1] = MSB(content->data_length2);
	varname[2] = LSB(content->data_length3);
	varname[3] = MSB(content->data_length3);
	varname[4] = LSB(content->mem_address);
	varname[5] = MSB(content->mem_address);

	TRYF(ti73_send_RTS(handle, content->data_length1, TI73_BKUP, varname, 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_SKP(handle, &rej_code))
	TRYF(ti73_send_ACK(handle));
	switch (rej_code) 
	{
	case REJ_EXIT:
	case REJ_SKIP:
		return ERR_ABORT;
	case REJ_MEMORY:
		return ERR_OUT_OF_MEMORY;
	default:			// RTS
		break;
	}

	update_->cnt2 = 0;
	update_->max2 = 3;
	update_->pbar();

	TRYF(ti73_send_XDP(handle, content->data_length1, content->data_part1));
	TRYF(ti73_recv_ACK(handle, NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_XDP(handle, content->data_length2, content->data_part2));
	TRYF(ti73_recv_ACK(handle, NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_XDP(handle, content->data_length3, content->data_part3));
	TRYF(ti73_recv_ACK(handle, NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_ACK(handle));

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };
	uint8_t attr;

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_backup());

	TRYF(ti73_send_REQ(handle, 0x0000, TI73_BKUP, "", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_VAR(handle, &content->data_length1, &content->type, varname, &attr));
	content->data_length2 = (uint16_t)varname[0] | (((uint16_t)(varname[1])) << 8);
	content->data_length3 = (uint16_t)varname[2] | (((uint16_t)(varname[3])) << 8);
	content->mem_address  = (uint16_t)varname[4] | (((uint16_t)(varname[5])) << 8);
	TRYF(ti73_send_ACK(handle));

	TRYF(ti73_send_CTS(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	update_->cnt2 = 0;
	update_->max2 = 3;
	update_->pbar();

	content->data_part1 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(handle, &content->data_length1, content->data_part1));
	TRYF(ti73_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part2 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(handle, &content->data_length2, content->data_part2));
	TRYF(ti73_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part3 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(handle, &content->data_length3, content->data_part3));
	TRYF(ti73_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();
  
	content->data_part4 = NULL;
  
	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint8_t rej_code;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		
		if(entry->action == ACT_SKIP)
			continue;

		TRYF(ti73_send_RTS(handle, (uint16_t)entry->size, entry->type, entry->name, entry->attr));
		TRYF(ti73_recv_ACK(handle, NULL));

		TRYF(ti73_recv_SKP(handle, &rej_code));
		TRYF(ti73_send_ACK(handle));

		switch (rej_code) 
		{
		case REJ_EXIT:
		  return ERR_ABORT;
		case REJ_SKIP:
		  continue;
		case REJ_MEMORY:
		  return ERR_OUT_OF_MEMORY;
		default:			// RTS
		  break;
		}

		utf8 = ticonv_varname_to_utf8(handle->model, entry->name, entry->type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti73_send_XDP(handle, entry->size, entry->data));
		TRYF(ti73_recv_ACK(handle, NULL));

		TRYF(ti73_send_EOT(handle));
		ticalcs_info("");

		update_->cnt2 = i+1;
		update_->max2 = content->num_entries;
		update_->pbar();
  }

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	VarEntry *ve;
	char *utf8;
	uint16_t ve_size;

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_single());
	content->num_entries = 1;

	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	// silent request
	TRYF(ti73_send_REQ(handle, (uint16_t)vr->size, vr->type, vr->name, vr->attr));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_VAR(handle, &ve_size, &ve->type, ve->name, &vr->attr));
	ve->size = ve_size;
	TRYF(ti73_send_ACK(handle));

	TRYF(ti73_send_CTS(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	ve->data = tifiles_ve_alloc_data(ve->size);
	TRYF(ti73_recv_XDP(handle, &ve_size, ve->data));
	ve->size = ve_size;
	TRYF(ti73_send_ACK(handle));

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

static int		get_version	(CalcHandle* handle, CalcInfos* infos);

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	int i, j, k;
	int size;
	char *utf8;
	int se = 0;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
		if(ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
			break;
	if(ptr == NULL)
		return -1;

	if(ptr->data_type == TI83p_AMS)
		size = 0x100;
	else if(ptr->data_type == TI83p_APPL)
		size = 0x80;
	else
		return -1;

	// check for 83+ Silver Edition (not usable in boot mode, sic!)
	if(handle->model != CALC_TI73 && ptr->data_type == TI83p_APPL)
	{
		CalcInfos infos;

		TRYF(get_version(handle, &infos));
		se = infos.hw_version & 1;
	}



	ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
	ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

	utf8 = ticonv_varname_to_utf8(handle->model, ptr->name, ptr->data_type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	update_->cnt2 = 0;
	update_->max2 = ptr->data_length;

	for (k = i = 0; i < ptr->num_pages; i++) 
	{
		FlashPage *fp = ptr->pages[i];

		if((ptr->data_type == TI83p_AMS) && (i == 1))	// need relocation ?
			fp->addr = 0x4000;

		for(j = 0; j < fp->size; j += size)
		{
			uint16_t addr = fp->addr + j;
			uint8_t* data = fp->data + j;

			TRYF(ti73_send_VAR2(handle, size, ptr->data_type, fp->flag, addr, fp->page));
			TRYF(ti73_recv_ACK(handle, NULL));

			if(handle->model == CALC_TI73 && ptr->data_type == TI83p_APPL)
				{TRYF(ti73_recv_CTS(handle, 0));}	// is depending of OS version?
			else
				{TRYF(ti73_recv_CTS(handle, 10));}
			TRYF(ti73_send_ACK(handle));

			TRYF(ti73_send_XDP(handle, size, data));
			TRYF(ti73_recv_ACK(handle, NULL));

			update_->cnt2 += size;
			update_->pbar();
		}

		/* Note: 
			TI83+/SE and TI84+/SE don't need a pause (otherwise transfer fails).
			TI73,TI83+,TI84+ need a pause (otherwise transfer fails).
		*/
		if(!se)
		{
			if (i == 1)
			  PAUSE(1000);		// This pause is NEEDED !
			if (i == ptr->num_pages - 2)
			  PAUSE(2500);		// This pause is NEEDED !
		}
	}

	TRYF(ti73_send_EOT(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	FlashPage *fp;
	uint16_t data_addr;
	uint16_t old_page = 0;
	uint16_t data_page;
	uint16_t data_length;
	uint8_t data_type;
	uint32_t size;
	int first_block;
	int page;
	int offset;
	uint8_t buf[FLASH_PAGE_SIZE + 4];
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	content->model = handle->model;
	strcpy(content->name, vr->name);
	content->data_type = vr->type;
	content->device_type = handle->model == CALC_TI73 ? DEVICE_TYPE_73 : DEVICE_TYPE_83P;
	content->num_pages = 2048;	// TI83+ has 512 KB of FLASH max
	content->pages = tifiles_fp_create_array(content->num_pages);

	page = 0;
	fp = content->pages[page] = tifiles_fp_create();

	TRYF(ti73_send_REQ2(handle, 0x00, TI73_APPL, vr->name, 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	update_->cnt2 = 0;
	update_->max2 = vr->size;

	for(size = 0, first_block = 1, offset = 0;;)
	{
		int err;
		char name[9];

		err = ti73_recv_VAR2(handle, &data_length, &data_type, name, &data_addr, &data_page);
		TRYF(ti73_send_ACK(handle));
		if (err == ERR_EOT)
			goto exit;
		TRYF(err);

		if(first_block)
		{
			old_page = data_page;

			fp->addr = data_addr & 0x4000;
			fp->page = data_page;
		}
		if(old_page != data_page)
		{
			fp->addr = data_addr & 0x4000;
			fp->page = old_page;
			fp->flag = 0x80;
			fp->size = offset;
			fp->data = tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
			memcpy(fp->data, buf, fp->size);

			page++;
			offset = 0;
			old_page = data_page;

			fp = content->pages[page] = tifiles_fp_create();
		}

		TRYF(ti73_send_CTS(handle));
		TRYF(ti73_recv_ACK(handle, NULL));

		TRYF(ti73_recv_XDP(handle, &data_length, &buf[offset]));
		TRYF(ti73_send_ACK(handle));

		if (first_block)
		{
			first_block = 0;

			/* compute actual application size */
			if (buf[0] == 0x80 && buf[1] == 0x0f)
			{
				uint32_t len = ((uint32_t)(buf[2])) << 24 | ((uint32_t)(buf[3])) << 16 | ((uint32_t)(buf[4])) << 8 | (uint32_t)(buf[5]);
				update_->max2 = len + 75;
			}
		}

		size += data_length;
		offset += data_length;

		update_->cnt2 = size;
		update_->pbar();
	}

exit:
	{
		fp->addr = data_addr & 0x4000;
		fp->page = old_page;
		fp->flag = 0x80;
		fp->size = offset;
		fp->data = tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
		memcpy(fp->data, buf, fp->size);
		page++;
	}

	content->num_pages = page;

	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	uint16_t unused;
	uint16_t varsize;
	uint8_t vartype;
	char varname[9];
	uint8_t varattr;
	uint8_t data[16];
	int i;

	g_snprintf(update_->text, sizeof(update_->text), "ID-LIST");
	update_label();

	TRYF(ti73_send_REQ(handle, 0x0000, TI73_IDLIST, "", 0x00));
	TRYF(ti73_recv_ACK(handle, &unused));

	TRYF(ti73_recv_VAR(handle, &varsize, &vartype, varname, &varattr));
	TRYF(ti73_send_ACK(handle));

	TRYF(ti73_send_CTS(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_XDP(handle, &varsize, data));
	TRYF(ti73_send_ACK(handle));

	i = data[9];
	data[9] = data[10];
	data[10] = i;

	for(i = 4; i < varsize; i++)
		sprintf((char *)&id[2 * (i-4)], "%02x", data[i]);
	id[7*2] = '\0';

	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	if(handle->model == CALC_TI73)
	{
		TRYF(rd_send(handle, "romdump.73p", romDumpSize73, romDump73));
	}
	else
	{
		TRYF(rd_send(handle, "romdump.8Xp", romDumpSize8Xp, romDump8Xp));
	}

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	static const uint16_t keys_83p[] = {
		0x40, 0x09, 0x09, 0xFC9C, /* Quit, Clear, Clear, Asm( */
		0xDA, 0xAB, 0xA8, 0xA6,   /* prgm, R, O, M */
		0x9D, 0xAE, 0xA6, 0xA9,   /* D, U, M, P */
		0x86, 0x05 };             /* ), Enter */

	static const uint16_t keys_73[] = {
		0x40, 0x09, 0x09, 0xDA,   /* Quit, Clear, Clear, prgm */
		0xAB, 0xA8, 0xA6, 0x9D,   /* R, O, M, D, */
		0xAE, 0xA6, 0xA9, 0x05 }; /* U, M, P, Enter */

	const uint16_t *keys;
	int nkeys, i;

	if (handle->model == CALC_TI73)
	{
		keys = keys_73;
		nkeys = G_N_ELEMENTS(keys_73);
	}
	else
	{
		keys = keys_83p;
		nkeys = G_N_ELEMENTS(keys_83p);
	}

	// Launch program by remote control
	PAUSE(200);
	for(i = 0; i < nkeys - 1; i++)
	{
		TRYF(send_key(handle, keys[i]));
		PAUSE(100);
	}

	// This fixes a 100% reproducable timeout: send_key normally requests an ACK,
	// but when the program is running, no ACK is sent. Therefore, hit the Enter key
	// without requesting an ACK.
	TRYF(ti73_send_KEY(handle, keys[i]));
	TRYF(ti73_recv_ACK(handle, NULL)); // when the key is received
	PAUSE(200);

	// Get dump
	TRYF(rd_dump(handle, filename));

	// (Normally there would be another ACK after the program exits,
	// but the ROM dumper disables that behavior)

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint8_t buffer[16] = { 0 };
	uint32_t calc_time;

	struct tm ref, cur;
	time_t r, c, now;

	time(&now);	// retrieve current DST setting
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
	//printf("%s\n", asctime(&ref));

	cur.tm_year = _clock->year - 1900;
	cur.tm_mon = _clock->month - 1;
	cur.tm_mday = _clock->day;
	cur.tm_hour = _clock->hours;
	cur.tm_min = _clock->minutes;
	cur.tm_sec = _clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);
	//printf("%s\n", asctime(&cur));
	
	calc_time = (uint32_t)difftime(c, r);

	buffer[2] = MSB(MSW(calc_time));
	buffer[3] = LSB(MSW(calc_time));
	buffer[4] = MSB(LSW(calc_time));
	buffer[5] = LSB(LSW(calc_time));
	buffer[6] = _clock->date_format;
	buffer[7] = _clock->time_format;
	buffer[8] = 0xff;

	g_snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
	update_label();

	TRYF(ti73_send_RTS(handle, 13, TI73_CLK, "\0x08", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_CTS(handle, 13));
	TRYF(ti73_send_ACK(handle));

	TRYF(ti73_send_XDP(handle, 9, buffer));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_send_EOT(handle));

	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint16_t varsize;
	uint8_t vartype;
	uint8_t varattr;
	char varname[9];
	uint8_t buffer[32];
	uint32_t calc_time;

	struct tm ref, *cur;
	time_t r, c, now;

	g_snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
	update_label();

	TRYF(ti73_send_REQ(handle, 0x0000, TI73_CLK, "\0x08", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_VAR(handle, &varsize, &vartype, varname, &varattr));
	TRYF(ti73_send_ACK(handle));

	TRYF(ti73_send_CTS(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_XDP(handle, &varsize, buffer));
	TRYF(ti73_send_ACK(handle));

	calc_time = (((uint32_t)(buffer[2])) << 24) | (((uint32_t)(buffer[3])) << 16) | (((uint32_t)(buffer[4])) << 8) | (uint32_t)(buffer[5]);
	//printf("<%08x>\n", time);

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
	//printf("%s\n", asctime(&ref));

	c = r + calc_time;
	cur = localtime(&c);
	//printf("%s\n", asctime(cur));

	_clock->year = cur->tm_year + 1900;
	_clock->month = cur->tm_mon + 1;
	_clock->day = cur->tm_mday;
	_clock->hours = cur->tm_hour;
	_clock->minutes = cur->tm_min;
	_clock->seconds = cur->tm_sec;

	_clock->date_format = buffer[6];
	_clock->time_format = buffer[7];

	return 0;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	return 0;
}

static int		change_attr	(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	TRYF(ti73_send_DEL(handle, (uint16_t)vr->size, vr->type, vr->name, vr->attr));
	TRYF(ti73_recv_ACK(handle, NULL));
	TRYF(ti73_recv_ACK(handle, NULL));

	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint16_t length;
	uint8_t buf[32];

	TRYF(ti73_send_VER(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_send_CTS(handle));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ti73_recv_XDP(handle, &length, buf));
	TRYF(ti73_send_ACK(handle));

	memset(infos, 0, sizeof(CalcInfos));
	if(handle->model == CALC_TI73)
	{
		g_snprintf(infos->os_version, 5, "%1x.%02x", buf[0], buf[1]);
		g_snprintf(infos->boot_version, 5, "%1x.%02x", buf[2], buf[3]);
	}
	else
	{
		g_snprintf(infos->os_version, 5, "%1i.%02i", buf[0], buf[1]);
		g_snprintf(infos->boot_version, 5, "%1i.%02i", buf[2], buf[3]);
	}
	infos->battery = (buf[4] & 1) ? 0 : 1;
	infos->hw_version = buf[5];
	switch(buf[5])
	{
	case 0: infos->model = CALC_TI83P; break;
	case 1: infos->model = CALC_TI83P; break;
	case 2: infos->model = CALC_TI84P; break;
	case 3: infos->model = CALC_TI84P; break;
	case 5: infos->model = CALC_TI84P; break; // 84+CSE
	default: infos->model = CALC_TI84P; break; // If new models ever arise, they'll probably be 84+ anyway.
	}
	infos->language_id = buf[6];
	infos->sub_lang_id = buf[7];
	infos->mask = INFOS_BOOT_VERSION | INFOS_OS_VERSION | INFOS_BATTERY | INFOS_HW_VERSION | INFOS_CALC_MODEL | INFOS_LANG_ID | INFOS_SUB_LANG_ID;

	tifiles_hexdump(buf, length);
	ticalcs_info(_("  OS: %s"), infos->os_version);
	ticalcs_info(_("  BIOS: %s"), infos->boot_version);
	ticalcs_info(_("  HW: %i"), infos->hw_version);
	ticalcs_info(_("  Battery: %s"), infos->battery ? _("good") : _("low"));

	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	int i, nblocks;
	uint16_t size = 0xE8;

	// search for cert header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
		if(ptr->data_type == TI83p_CERT)
			break;

	if (ptr != NULL)
	{
		// send content
		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		nblocks = ptr->data_length / size;
		update_->max2 = nblocks;

		TRYF(ti73_send_VAR2(handle, size, ptr->data_type, 0x04, 0x4000, 0x00));
		TRYF(ti73_recv_ACK(handle, NULL));

		TRYF(ti73_recv_CTS(handle, 10));
		TRYF(ti73_send_ACK(handle));

		for(i = 0; i <= nblocks; i++) 
		{
			uint32_t length = size;

			TRYF(ti73_send_XDP(handle, length, (ptr->data_part) + length * i))
			TRYF(ti73_recv_ACK(handle, NULL));

			TRYF(ti73_recv_CTS(handle, size));
			TRYF(ti73_send_ACK(handle));

			update_->cnt2 = i;
			update_->pbar();
		}

		TRYF(ti73_send_EOT(handle));

		ticalcs_info(_("Header sent completely."));

	}
	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	int i;
	uint8_t buf[256];

	g_snprintf(update_->text, sizeof(update_->text), _("Receiving certificate"));
	update_label();

	content->model = handle->model;
	strcpy(content->name, "");
	content->data_type = TI83p_CERT;
	content->device_type = 0x73;
	content->num_pages = 0;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(2 * 1024 * 1024);	// 2MB max

	TRYF(ti73_send_REQ2(handle, 0x00, TI83p_GETCERT, "", 0x00));
	TRYF(ti73_recv_ACK(handle, NULL));

	TRYF(ticables_cable_recv(handle->cable, buf, 4));	//VAR w/ no header
	ticalcs_info(" TI->PC: VAR");
	TRYF(ti73_send_ACK(handle));

	for(i = 0, content->data_length = 0;; i++) 
	{
		int err;
		uint16_t block_size;

		TRYF(ti73_send_CTS(handle));
		TRYF(ti73_recv_ACK(handle, NULL));

		err = ti73_recv_XDP(handle, &block_size, content->data_part);
		TRYF(ti73_send_ACK(handle));

		content->data_length += block_size;

		if (err == ERR_EOT)
			goto exit;
		TRYF(err);

		update_->cnt2 += block_size;
		update_->pbar();
	}

exit:
	return 0;
}

const CalcFncts calc_73 = 
{
	CALC_TI73,
	"TI73",
	"TI-73",
	"TI-73",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_BACKUP,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P",   /* send_backup */
	 "2P",   /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
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
	&rename_var,
	&change_attr
};

const CalcFncts calc_83p = 
{
	CALC_TI83P,
	"TI83+",
	"TI-83 Plus",
	"TI-83 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_DELVAR | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P",   /* send_backup */
	 "2P",   /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
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
	&rename_var,
	&change_attr
};

const CalcFncts calc_84p = 
{
	CALC_TI84P,
	"TI84+",
	"TI-84 Plus",
	"TI-84 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P",   /* send_backup */
	 "2P",   /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
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
	&rename_var,
	&change_attr
};
