/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include "ticonv.h"
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd73.h"
#include "rom73.h"
#include "rom83p.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the TI83+
#define TI73_ROWS  64
#define TI73_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	TRYF(ti73_send_RDY());
	TRYF(ti73_recv_ACK(&status));

	return (MSB(status) & 0x01) ? ERR_NOT_READY : 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti73_send_KEY(key));
	TRYF(ti73_recv_ACK(&key));	// when the key is received
	TRYF(ti73_recv_ACK(NULL));	// after it completes the resulting action

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

	TRYF(ti73_send_SCR());
	TRYF(ti73_recv_ACK(NULL));

	err = ti73_recv_XDP(&max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti73_send_ACK());

	*bitmap = (uint8_t *) malloc(TI73_COLS * TI73_ROWS / 8);
	if(*bitmap == NULL)
		return ERR_MALLOC;
	memcpy(*bitmap, buf, TI73_COLS * TI73_ROWS  / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	TreeInfo *ti;
	uint16_t unused;
	uint32_t memory;
	TNode *folder, *root;	
	char *utf8;

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

	TRYF(ti73_send_REQ(0x0000, TI73_DIR, "", 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_XDP(&unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti73_send_ACK());
	ti->mem_free = (uint32_t)GUINT_TO_POINTER(memory);

	folder = t_node_new(NULL);
	t_node_append(*vars, folder);

	root = t_node_new(NULL);
	t_node_append(*apps, root);

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		TNode *node;
		int err;
		uint16_t ve_size;

		err = ti73_recv_VAR(&ve_size, &ve->type, ve->name, &ve->attr);
		ve->size = ve_size;
		TRYF(ti73_send_ACK());
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		node = t_node_new(ve);
		if (ve->type != TI73_APPL)
			t_node_append(folder, node);
		else
			t_node_append(root, node);

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name);
		snprintf(update_->text, sizeof(update_->text), _("Parsing %s"), utf8);
		g_free(utf8);
		update_label();
  }

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t unused;
	uint32_t memory;

	TRYF(ti73_send_REQ(0x0000, TI73_DIR, "", 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_XDP(&unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti73_send_EOT());
	*ram = (uint32_t)GUINT_TO_POINTER(memory);
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

	TRYF(ti73_send_RTS(content->data_length1, TI73_BKUP, varname, 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_SKP(&rej_code))
    TRYF(ti73_send_ACK());
	switch (rej_code) 
	{
	case REJ_EXIT:
	case REJ_SKIP:
		return ERR_ABORT;
    break;
	case REJ_MEMORY:
		return ERR_OUT_OF_MEMORY;
    break;
	default:			// RTS
    break;
	}

	update_->cnt2 = 0;
	update_->max2 = 3;
	update_->pbar();

	TRYF(ti73_send_XDP(content->data_length1, content->data_part1));
	TRYF(ti73_recv_ACK(NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_XDP(content->data_length2, content->data_part2));
	TRYF(ti73_recv_ACK(NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_XDP(content->data_length3, content->data_part3));
	TRYF(ti73_recv_ACK(NULL));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti73_send_ACK());

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };
	uint8_t attr;

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_backup());

	TRYF(ti73_send_REQ(0x0000, TI73_BKUP, "", 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_VAR(&content->data_length1, &content->type, varname, &attr));
	content->data_length2 = (uint8_t)varname[0] | ((uint8_t)varname[1] << 8);
	content->data_length3 = (uint8_t)varname[2] | ((uint8_t)varname[3] << 8);
	content->mem_address  = (uint8_t)varname[4] | ((uint8_t)varname[5] << 8);
	TRYF(ti73_send_ACK());

	TRYF(ti73_send_CTS());
	TRYF(ti73_recv_ACK(NULL));

	update_->cnt2 = 0;
	update_->max2 = 3;
	update_->pbar();

	content->data_part1 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(&content->data_length1, content->data_part1));
	TRYF(ti73_send_ACK());
	update_->cnt2++;
	update_->pbar();

	content->data_part2 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(&content->data_length2, content->data_part2));
	TRYF(ti73_send_ACK());
	update_->cnt2++;
	update_->pbar();

	content->data_part3 = tifiles_ve_alloc_data(65536);
	TRYF(ti73_recv_XDP(&content->data_length3, content->data_part3));
	TRYF(ti73_send_ACK());
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

		TRYF(ti73_send_RTS((uint16_t)entry->size, entry->type, entry->name, entry->attr));
		TRYF(ti73_recv_ACK(NULL));

		TRYF(ti73_recv_SKP(&rej_code));
		TRYF(ti73_send_ACK());

		switch (rej_code) 
		{
		case REJ_EXIT:
		  return ERR_ABORT;
		  break;
		case REJ_SKIP:
		  continue;
		  break;
		case REJ_MEMORY:
		  return ERR_OUT_OF_MEMORY;
		  break;
		default:			// RTS
		  break;
		}

		utf8 = ticonv_varname_to_utf8(handle->model,entry->name);
		snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti73_send_XDP(entry->size, entry->data));
		TRYF(ti73_recv_ACK(NULL));

		TRYF(ti73_send_EOT());
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

    utf8 = ticonv_varname_to_utf8(handle->model, vr->name);
    snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
    update_label();

    // silent request
    TRYF(ti73_send_REQ((uint16_t)vr->size, vr->type, vr->name, vr->attr));
    TRYF(ti73_recv_ACK(NULL));

    TRYF(ti73_recv_VAR(&ve_size, &ve->type, ve->name, &vr->attr));
    ve->size = ve_size;
    TRYF(ti73_send_ACK());

    TRYF(ti73_send_CTS());
    TRYF(ti73_recv_ACK(NULL));

    ve->data = tifiles_ve_alloc_data(ve->size);
    TRYF(ti73_recv_XDP(&ve_size, ve->data));
    ve->size = ve_size;
    TRYF(ti73_send_ACK());

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
	int i, j, k;
	int size;
	char *utf8;

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

#if 0
	printf("size = %08x\n", ptr->data_length);
	printf("#pages: %i\n", ptr->num_pages);
	printf("type: %02x\n", ptr->data_type);
	for (i = 0; i < ptr->num_pages; i++) 
	{
		FlashPage *fp = ptr->pages[i];

		printf("page #%i: %04x %02x %02x %04x\n", i,
			fp->addr, fp->page, fp->flag, fp->size);		
	}

	return 0;
#endif

	ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
	ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

	utf8 = ticonv_varname_to_utf8(handle->model, ptr->name);
	snprintf(update_->text, sizeof(update_->text), "%s", utf8);
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

			TRYF(ti73_send_VAR2(size, ptr->data_type, fp->flag, addr, fp->page));
			TRYF(ti73_recv_ACK(NULL));

			TRYF(ti73_recv_CTS((uint16_t)(handle->model == CALC_TI73 ? 0 : 10)));
			TRYF(ti73_send_ACK());

			TRYF(ti73_send_XDP(size, data));
			TRYF(ti73_recv_ACK(NULL));

			update_->cnt2 += size;
			update_->pbar();
		}

		if(handle->model != CALC_TI84P)
		{
			if (i == 1)
			  PAUSE(1000);		// This pause is NEEDED !
			if (i == ptr->num_pages - 2)
			  PAUSE(2500);		// This pause is NEEDED !
		}
	}

	TRYF(ti73_send_EOT());
	TRYF(ti73_recv_ACK(NULL));

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

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name);
	snprintf(update_->text, sizeof(update_->text), "%s", utf8);
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

	TRYF(ti73_send_REQ2(0x00, TI73_APPL, vr->name, 0x00));
	TRYF(ti73_recv_ACK(NULL));

	update_->cnt2 = 0;
	update_->max2 = handle->model == CALC_TI73 ? vr->size * 8 : vr->size;

	for(size = 0, first_block = 1, offset = 0;;)
	{
		int err;
		char name[9];

		err = ti73_recv_VAR2(&data_length, &data_type, name, &data_addr, &data_page);
		TRYF(ti73_send_ACK());
		if (err == ERR_EOT)
			goto exit;
		TRYF(err);

		if(first_block)
		{
			old_page = data_page;
			first_block = 0;

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

		TRYF(ti73_send_CTS());
		TRYF(ti73_recv_ACK(NULL));

		TRYF(ti73_recv_XDP((uint16_t *)&data_length, &buf[offset]));
		TRYF(ti73_send_ACK());

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

	snprintf(update_->text, sizeof(update_->text), "ID-LIST");
	update_label();

	TRYF(ti73_send_REQ(0x0000, TI73_IDLIST, "", 0x00));
	TRYF(ti73_recv_ACK(&unused));

	TRYF(ti73_recv_VAR(&varsize, &vartype, varname, &varattr));
	TRYF(ti73_send_ACK());

	TRYF(ti73_send_CTS());
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_XDP(&varsize, data));
	TRYF(ti73_send_ACK());

	i = data[9];
	data[9] = data[10];
	data[10] = i;

	for(i = 4; i < varsize; i++)
		sprintf((char *)&id[2 * (i-4)], "%02x", data[i]);
	id[7*2] = '\0';

	return 0;
}

extern int rom_dump(CalcHandle* h, FILE* f);
extern int rom_dump_ready(CalcHandle* h);

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	const char *prgname = (handle->model == CALC_TI73) ? "romdump.73p" : "romdump.8Xp";
	FILE *f;
	int err, i;
	uint16_t keys[] = { 
        0x40, 0x09, 0x09, 0xFC9C,	/* Quit, Clear, Clear, Asm( */
        0xDA, 0xAB, 0xA8, 0xA6,     /* prgm, R, O, M */
        0x9D, 0xAE, 0xA6, 0xA9,     /* D, U, M, P */
		0x86, 0x05 };               /* ), Enter */

	// Copies ROM dump program into a file
	f = fopen(prgname, "wb");
	if (f == NULL)
		return ERR_FILE_OPEN;
	if ((handle->model == CALC_TI73)
	    ? (fwrite(romDump73, sizeof(uint8_t), romDumpSize73, f) < romDumpSize73)
	    : (fwrite(romDump8Xp, sizeof(uint8_t), romDumpSize8Xp, f) < romDumpSize8Xp))
	{
		fclose(f);
		return ERR_SAVE_FILE;
	}
	if (fclose(f))
		return ERR_SAVE_FILE;

	// Transfer program to calc
	handle->busy = 0;
	TRYF(ticalcs_calc_send_var2(handle, MODE_NORMAL, prgname));
	unlink(prgname);

	// Launch program by remote control
	if (handle->model != CALC_TI73)
    {
		// Launch program by remote control
        PAUSE(200);
        for(i = 0; i < sizeof(keys) / sizeof(uint16_t) - 1; i++)
        {
			TRYF(send_key(handle, keys[i]));
            PAUSE(100);
		}

		// This fixes the bug with it always timing out, send_key normally requests an ACK
		// but when the program is running it will timeout, so lets do it this way.
        // Anyways, lets Hit that Enter Key!
		TRYF(ti73_send_KEY(keys[i]));
	    TRYF(ti73_recv_ACK(&keys[i]));	// when the key is received
		PAUSE(200);
	}
	else
	{
		// else wait for user's action
		sprintf(update_->text, _("Waiting for user's action..."));
		update_label();

		do
		{
			handle->updat->refresh();
			if (handle->updat->cancel)
				return ERR_ABORT;
			
			//send RDY request ???
			PAUSE(1000);
			err = rom_dump_ready(handle);
		}
		while (err == ERROR_READ_TIMEOUT);
	}

	// Get dump
	f = fopen(filename, "wb");
	if (f == NULL)
		return ERR_OPEN_FILE;

	err = rom_dump(handle, f);
	if(err)
	{
		fclose(f);
		return err;
	}

	fclose(f);
	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
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
	//printf("<%s>\n", asctime(&ref));

	cur.tm_year = clock->year - 1900;
	cur.tm_mon = clock->month - 1;
	cur.tm_mday = clock->day;	
	cur.tm_hour = clock->hours;
	cur.tm_min = clock->minutes;
	cur.tm_sec = clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);
	//printf("<%s>\n", asctime(&cur));
	
	calc_time = (uint32_t)difftime(c, r);

    buffer[2] = MSB(MSW(calc_time));
    buffer[3] = LSB(MSW(calc_time));
    buffer[4] = MSB(LSW(calc_time));
    buffer[5] = LSB(LSW(calc_time));
    buffer[6] = clock->date_format;
    buffer[7] = clock->time_format;
    buffer[8] = 0xff;

    snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
    update_label();

	TRYF(ti73_send_RTS(13, TI73_CLK, "\0x08", 0x00));
    TRYF(ti73_recv_ACK(NULL));

    TRYF(ti73_recv_CTS(13));
    TRYF(ti73_send_ACK());

    TRYF(ti73_send_XDP(9, buffer));
    TRYF(ti73_recv_ACK(NULL));

    TRYF(ti73_send_EOT());

	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	uint16_t varsize;
    uint8_t vartype;
	uint8_t varattr;
    char varname[9];
    uint8_t buffer[32];
	uint32_t calc_time;

	struct tm ref, *cur;
	time_t r, c, now;

    snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
    update_label();

	TRYF(ti73_send_REQ(0x0000, TI73_CLK, "\0x08", 0x00));
    TRYF(ti73_recv_ACK(NULL));

    TRYF(ti73_recv_VAR(&varsize, &vartype, varname, &varattr));
    TRYF(ti73_send_ACK());

    TRYF(ti73_send_CTS());
    TRYF(ti73_recv_ACK(NULL));

    TRYF(ti73_recv_XDP(&varsize, buffer));
    TRYF(ti73_send_ACK());

	calc_time = (buffer[2] << 24) | (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];
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
	//printf("<%s>\n", asctime(&ref));

	c = r + calc_time;
	cur = localtime(&c);
	//printf("<%s>\n", asctime(cur));

	clock->year = cur->tm_year + 1900;
	clock->month = cur->tm_mon + 1;
	clock->day = cur->tm_mday;
	clock->hours = cur->tm_hour;
	clock->minutes = cur->tm_min;
	clock->seconds = cur->tm_sec;

    clock->date_format = buffer[6];
    clock->time_format = buffer[7];

	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name);
	snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	TRYF(ti73_send_DEL((uint16_t)vr->size, vr->type, vr->name, vr->attr));
	TRYF(ti73_recv_ACK(NULL));
	TRYF(ti73_recv_ACK(NULL));

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

	TRYF(ti73_send_VER());
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_send_CTS());
    TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_XDP(&length, buf));
    TRYF(ti73_send_ACK());

	memset(infos, 0, sizeof(CalcInfos));
	snprintf(infos->os_version, 4, "%1i.%02i", buf[0], buf[1]);
	snprintf(infos->boot_version, 4, "%1i.%02i", buf[2], buf[3]);
	infos->battery = (buf[4] & 1) ? 0 : 1;
	infos->hw_version = buf[5];
	switch(buf[5])
	{
	case 0: infos->model = CALC_TI83P; break;
	case 1: infos->model = CALC_TI83P; break;
	case 2: infos->model = CALC_TI84P; break;
	case 3: infos->model = CALC_TI84P; break;
	}
	infos->language_id = buf[6];
	infos->sub_lang_id = buf[7];
	infos->mask = INFOS_BOOT_VERSION | INFOS_OS_VERSION | INFOS_BATTERY | INFOS_HW_VERSION | INFOS_CALC_MODEL | INFOS_LANG_ID | INFOS_SUB_LANG_ID;

	tifiles_hexdump(buf, length);
	ticalcs_info(_("  OS: %s"), infos->os_version);
	ticalcs_info(_("  BIOS: %s"), infos->boot_version);
	ticalcs_info(_("  HW: %i"), infos->hw_version);
	ticalcs_info(_("  Battery: %s"), (infos->battery & 1) ? "low" : "good");

	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	FlashContent *ptr;
	int i, nblocks;
	uint16_t size = 0xE8;

	// search for cert header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
		if(ptr->data_type == TI83p_CERTIF)
			break;

	// send content
	ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
	ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

	nblocks = ptr->data_length / size;
	update_->max2 = nblocks;

	TRYF(ti73_send_VAR2(size, ptr->data_type, 0x04, 0x4000, 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ti73_recv_CTS(10));
	TRYF(ti73_send_ACK());

	for(i = 0; i <= nblocks; i++) 
	{
		uint32_t length = size;

		TRYF(ti73_send_XDP(length, (ptr->data_part) + length * i))
		TRYF(ti73_recv_ACK(NULL));

		TRYF(ti73_recv_CTS(size));
		TRYF(ti73_send_ACK());

		update_->cnt2 = i;
		update_->pbar();
	}

	TRYF(ti73_send_EOT());

	ticalcs_info(_("Header sent completely."));

	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	int i;
	uint8_t buf[256];

	snprintf(update_->text, sizeof(update_->text), _("Receiving certificate"));
	update_label();

	content->model = handle->model;
	strcpy(content->name, "");
	content->data_type = TI83p_CERTIF;
	content->device_type = 0x73;
	content->num_pages = 0;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(2 * 1024 * 1024);	// 2MB max

	TRYF(ti73_send_REQ2(0x00, TI83p_GETCERT, "", 0x00));
	TRYF(ti73_recv_ACK(NULL));

	TRYF(ticables_cable_recv(handle->cable, buf, 4));	//VAR w/ no header
	ticalcs_info(" TI->PC: VAR");
	TRYF(ti73_send_ACK());

	for(i = 0, content->data_length = 0;; i++) 
	{
		int err;
		uint16_t block_size;

		TRYF(ti73_send_CTS());
		TRYF(ti73_recv_ACK(NULL));

		err = ti73_recv_XDP(&block_size, content->data_part);
		TRYF(ti73_send_ACK());

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
	N_("TI-73"),
	N_("TI-73"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_BACKUP,
	{"", "", "1P", "1L", "", "2P", "2P", "2P1L", "1P1L", "2P1L", "1P1L", "2P1L", "2P1L",
		"2P", "1L", "2P1L", "", "", "1L", "1L", "", "1L", "1L" },
	&is_ready,
	&send_key,
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
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
        &new_folder,
        &get_version,
	&send_cert,
	&recv_cert,
};

const CalcFncts calc_83p = 
{
	CALC_TI83P,
	"TI83+",
	N_("TI-83 Plus"),
	N_("TI-83 Plus"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	{"", "", "1P", "1L", "", "2P", "2P", "2P1L", "1P1L", "2P1L", "1P1L", "2P1L", "2P1L",
		"2P", "1L", "2P", "", "", "1L", "1L", "", "1L", "1L" },
	&is_ready,
	&send_key,
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
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};

const CalcFncts calc_84p = 
{
	CALC_TI84P,
	"TI84+",
	N_("TI-84 Plus"),
	N_("TI-84 Plus"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	{"", "", "1P", "1L", "", "2P", "2P", "2P1L", "1P1L", "2P1L", "1P1L", "2P1L", "2P1L",
		"2P", "1L", "2P", "", "", "1L", "1L", "", "1L", "1L" },
	&is_ready,
	&send_key,
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
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
