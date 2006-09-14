/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	TI86 support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "ticonv.h"
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd85.h"
#include "rom86.h"
#include "romdump.h"
//#include "keys83p.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the TI86
#define TI86_ROWS  64
#define TI86_COLS  128

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti85_send_KEY(key));
	TRYF(ti85_recv_ACK(&key));

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;
	uint8_t buf[TI86_COLS * TI86_ROWS / 8];

	sc->width = TI86_COLS;
	sc->height = TI86_ROWS;
	sc->clipped_width = TI86_COLS;
	sc->clipped_height = TI86_ROWS;

	TRYF(ti85_send_SCR());
	TRYF(ti85_recv_ACK(NULL));

	err = ti85_recv_XDP(&max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti85_send_ACK());

	*bitmap = (uint8_t *)malloc(TI86_COLS * TI86_ROWS / 8);
	if(*bitmap == NULL) return ERR_MALLOC;
	memcpy(*bitmap, buf, TI86_COLS * TI86_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	TreeInfo *ti;
	TNode *folder;
	uint16_t unused;
	uint8_t hl, ll, lh;
	uint8_t mem[8];
	char *utf8;

	// get list of folders & FLASH apps
	(*vars) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	(*apps) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

	TRYF(ti85_send_REQ(0x0000, TI86_DIR, ""));
	TRYF(ti85_recv_ACK(&unused));

	TRYF(ti85_recv_XDP(&unused, mem));
	TRYF(ti85_send_ACK());

	hl = mem[0];
	ll = mem[1];
	lh = mem[2];
	ti->mem_free = (hl << 16) | (lh << 8) | ll;

	folder = t_node_new(NULL);
	t_node_append(*vars, folder);

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		TNode *node;
		int err;
		uint16_t ve_size;

		err = ti85_recv_VAR(&ve_size, &ve->type, ve->name);
		ve->size = ve_size;
		TRYF(ti85_send_ACK());
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		node = t_node_new(ve);
		t_node_append(folder, node);

		utf8 = ticonv_varname_to_utf8(handle->model,ve->name);
		snprintf(update_->text, sizeof(update_->text), _("Parsing %s"), utf8);
		g_free(utf8);
		update_label();
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t unused;
	uint8_t hl, ll, lh;
	uint8_t mem[8];

	TRYF(ti85_send_REQ(0x0000, TI86_DIR, ""));
	TRYF(ti85_recv_ACK(&unused));

	TRYF(ti85_recv_XDP(&unused, mem));
	TRYF(ti85_send_EOT());

	hl = mem[0];
	ll = mem[1];
	lh = mem[2];
	*ram = (hl << 16) | (lh << 8) | ll;
	*flash = -1;

	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
    int err = 0;
    uint16_t length;
    char varname[9] = { 0 };
    uint8_t rej_code;
    uint16_t status;

    length = content->data_length1;
    varname[0] = LSB(content->data_length2);
    varname[1] = MSB(content->data_length2);
    varname[2] = LSB(content->data_length3);
    varname[3] = MSB(content->data_length3);
    varname[4] = LSB(content->data_length4);
    varname[5] = MSB(content->data_length4);

    TRYF(ti85_send_VAR(content->data_length1, TI86_BKUP, varname));
    TRYF(ti85_recv_ACK(&status));

    snprintf(update_->text, sizeof(update_->text), _("Waiting for user's action..."));
    update_label();

    do 
	{	// wait user's action
		if (update_->cancel)
			return ERR_ABORT;
		err = ti85_recv_SKP(&rej_code);
    }
    while (err == ERROR_READ_TIMEOUT);
    TRYF(ti85_send_ACK());

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
    
    strcpy(update_->text, "");
    update_label();

	update_->cnt2 = 0;
	update_->max2 = 4;

    TRYF(ti85_send_XDP(content->data_length1, content->data_part1));
    TRYF(ti85_recv_ACK(&status));
    update_->cnt2++;
	update_->pbar();

    TRYF(ti85_send_XDP(content->data_length2, content->data_part2));
    TRYF(ti85_recv_ACK(&status));
    update_->cnt2++;
	update_->pbar();

    if (content->data_length3) 
	{
      TRYF(ti85_send_XDP(content->data_length3, content->data_part3));
      TRYF(ti85_recv_ACK(&status));
    }
    update_->cnt2++;
	update_->pbar();

    TRYF(ti85_send_XDP(content->data_length4, content->data_part4));
    TRYF(ti85_recv_ACK(&status));
    update_->cnt2++;
	update_->pbar();

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };

	snprintf(update_->text, sizeof(update_->text), _("Waiting for backup..."));
    update_label();

	content->model = CALC_TI86;
	strcpy(content->comment, tifiles_comment_set_backup());

    TRYF(ti85_recv_VAR(&(content->data_length1), &content->type, varname));
    content->data_length2 = (uint8_t)varname[0] | ((uint8_t)varname[1] << 8);
    content->data_length3 = (uint8_t)varname[2] | ((uint8_t)varname[3] << 8);
    content->data_length4 = (uint8_t)varname[4] | ((uint8_t)varname[5] << 8);
    TRYF(ti85_send_ACK());

    TRYF(ti85_send_CTS());
    TRYF(ti85_recv_ACK(NULL));

    strcpy(update_->text, "");
	update_label();

	update_->cnt2 = 0;
	update_->max2 = 4;
	update_->pbar();

    content->data_part1 = tifiles_ve_alloc_data(65536);
    TRYF(ti85_recv_XDP(&content->data_length1, content->data_part1));
    TRYF(ti85_send_ACK());
    update_->cnt2++;
	update_->pbar();

    content->data_part2 = tifiles_ve_alloc_data(65536);
    TRYF(ti85_recv_XDP(&content->data_length2, content->data_part2));
    TRYF(ti85_send_ACK());
    update_->cnt2++;
	update_->pbar();

    if (content->data_length3) 
	{
      content->data_part3 = tifiles_ve_alloc_data(65536);
      TRYF(ti85_recv_XDP(&content->data_length3, content->data_part3));
      TRYF(ti85_send_ACK());
    } else
      content->data_part3 = NULL;
    update_->cnt2++;
	update_->pbar();

    content->data_part4 = tifiles_ve_alloc_data(65536);
    TRYF(ti85_recv_XDP(&content->data_length4, content->data_part4));
    TRYF(ti85_send_ACK());
    update_->cnt2++;
	update_->pbar();

	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint8_t rej_code;
	uint16_t status;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		
		if(entry->action == ACT_SKIP)
			continue;

		TRYF(ti85_send_RTS((uint16_t)entry->size, entry->type, entry->name));
		TRYF(ti85_recv_ACK(&status));

		TRYF(ti85_recv_SKP(&rej_code));
		TRYF(ti85_send_ACK());

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

		utf8 = ticonv_varname_to_utf8(handle->model, entry->name);
		snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti85_send_XDP(entry->size, entry->data));
		TRYF(ti85_recv_ACK(&status));

		TRYF(ti85_send_EOT());
		ticalcs_info("\n");

		update_->cnt2 = i+1;
		update_->max2 = content->num_entries;
		update_->pbar();
	}

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	uint16_t unused;
	VarEntry *ve;
	char *utf8;
	uint16_t ve_size;

	content->model = CALC_TI86;
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
	TRYF(ti85_send_REQ((uint16_t)vr->size, vr->type, vr->name));
	TRYF(ti85_recv_ACK(&unused));

	TRYF(ti85_recv_VAR(&ve_size, &ve->type, ve->name));
	ve->size = ve_size;
	TRYF(ti85_send_ACK());

	TRYF(ti85_send_CTS());
	TRYF(ti85_recv_ACK(NULL));

	ve->data = tifiles_ve_alloc_data(ve->size);
	TRYF(ti85_recv_XDP(&ve_size, ve->data));
	ve->size = ve_size;
	TRYF(ti85_send_ACK());

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
	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	TRYF(rd_send(handle, "romdump.86p", romDumpSize86, romDump86));
	PAUSE(1000);

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int err;
	//uint16_t keys[] = { 
	//    0x76, 0x08, 0x08, 		/* Quit, Clear, Clear,	*/
	//	  0x28, 0x3A, 0x34,	0x11,	/* A, S, M, (,			*/
	//    0x39, 0x36, 0x34, 0x2B,   /* R, O, M, D	*/
	//    0x56, 0x4E, 0x51, 0x12,	/* u, m, p, )	*/
	//    0x06						/* Enter		*/
	//};               

	// Send dumping program
	TRYF(rd_send(handle, "romdump.86p", romDumpSize86, romDump86));

	// Wait for user's action (execing program)
	sprintf(handle->updat->text, _("Waiting for user's action..."));
	handle->updat->label();

	do
	{
		handle->updat->refresh();
		if (handle->updat->cancel)
			return ERR_ABORT;
		
		//send RDY request ???
		PAUSE(1000);
		err = rd_is_ready(handle);
	}
	while (err == ERROR_READ_TIMEOUT);

	// Get dump
	TRYF(rd_dump(handle, filename));

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
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

const CalcFncts calc_86 = 
{
	CALC_TI86,
	"TI86",
	N_("TI-86"),
	N_("TI-86"),
	OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	FTS_SILENT | FTS_MEMFREE | FTS_BACKUP,
	{"", "", "1P", "1L", "", "2P1L", "2P1L", "2P1L", "1P1L", "2P1L", "1P1L", "", "",
		"", "1L", "2P1L", "", "", "1L", "1L", "", "1L", "1L" },
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
