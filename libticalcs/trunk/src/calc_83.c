/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

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
	TI83 support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd82.h"
#include "rom83.h"
#include "romdump.h"
#include "keys83.h"

// Screen coordinates of the TI83
#define TI83_ROWS  64
#define TI83_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti83_send_KEY(handle, key));
	TRYF(ti82_recv_ACK(handle, &key));

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	unsigned int i;

	// Go back to homescreen
	PAUSE(200);
	TRYF(send_key(handle, KEY83_Quit));
	TRYF(send_key(handle, KEY83_Clear));
	TRYF(send_key(handle, KEY83_Clear));

	// Launch program by remote control
	if(ve->type == TI83_ASM)
	{
		TRYF(send_key(handle, KEY83_SendMBL));
		TRYF(send_key(handle, KEY83_9));
	}
	TRYF(send_key(handle, KEY83_Exec));

	for(i = 0; i < strlen(ve->name); i++)
	{
		const CalcKey *ck = ticalcs_keys_83((ve->name)[i]);
		TRYF(send_key(handle, ck->normal.value));
	}

	TRYF(send_key(handle, KEY83_Enter));
	PAUSE(200);

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;
	uint8_t buf[TI83_COLS * TI83_ROWS / 8];

	sc->width = TI83_COLS;
	sc->height = TI83_ROWS;
	sc->clipped_width = TI83_COLS;
	sc->clipped_height = TI83_ROWS;

	TRYF(ti82_send_SCR(handle));
	TRYF(ti82_recv_ACK(handle, NULL));

	err = ti82_recv_XDP(handle, &max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti82_send_ACK(handle));

	*bitmap = (uint8_t *)g_malloc(TI83_COLS * TI83_ROWS / 8);
	if(*bitmap == NULL) return ERR_MALLOC;
	memcpy(*bitmap, buf, TI83_COLS * TI83_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	TreeInfo *ti;
	GNode *folder;
	uint16_t unused;
	uint32_t memory;
	char *utf8;

	// get list of folders & FLASH apps
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

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_DIR, ""));
	TRYF(ti82_recv_ACK(handle, &unused));

	TRYF(ti82_recv_XDP(handle, &unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti82_send_ACK(handle));
	ti->mem_free = memory;
	
	folder = g_node_new(NULL);
	g_node_append(*vars, folder);

	// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
	{
		GNode *node;
		VarEntry *ve;

		ve = tifiles_ve_create();
		ve->type = TI83_WINDW;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI83_ZSTO;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI83_TABLE;
		node = g_node_new(ve);
		g_node_append(folder, node);
	}

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		GNode *node;
		int err;
		uint16_t ve_size;

		err = ti82_recv_VAR(handle, &ve_size, &ve->type, ve->name);
		ve->size = ve_size;
		TRYF(ti82_send_ACK(handle));
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		node = g_node_new(ve);
		g_node_append(folder, node);

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

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_DIR, ""));
	TRYF(ti82_recv_ACK(handle, &unused));

	TRYF(ti82_recv_XDP(handle, &unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti82_send_EOT(handle));
	*ram = memory;
	*flash = -1;

	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
    uint16_t length;
    char varname[9];
    uint8_t rej_code;
    uint16_t status;

    length = content->data_length1;
    varname[0] = LSB(content->data_length2);
    varname[1] = MSB(content->data_length2);
    varname[2] = LSB(content->data_length3);
    varname[3] = MSB(content->data_length3);
    varname[4] = LSB(content->mem_address);
    varname[5] = MSB(content->mem_address);

    TRYF(ti82_send_RTS(handle, content->data_length1, TI83_BKUP, varname));
    TRYF(ti82_recv_ACK(handle, &status));

    TRYF(ti82_recv_SKP(handle, &rej_code))
    TRYF(ti82_send_ACK(handle));

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

	TRYF(ti82_send_XDP(handle, content->data_length1, content->data_part1));
	TRYF(ti82_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti82_send_XDP(handle, content->data_length2, content->data_part2));
	TRYF(ti82_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti82_send_XDP(handle, content->data_length3, content->data_part3));
	TRYF(ti82_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti82_send_ACK(handle));

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };
	uint16_t unused;

	content->model = CALC_TI83;
	strcpy(content->comment, tifiles_comment_set_backup());

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_BKUP, ""));
	TRYF(ti82_recv_ACK(handle, &unused));

	TRYF(ti82_recv_VAR(handle, &(content->data_length1), &content->type, varname));
	content->data_length2 = (uint16_t)varname[0] | (((uint16_t)(varname[1])) << 8);
	content->data_length3 = (uint16_t)varname[2] | (((uint16_t)(varname[3])) << 8);
	content->mem_address  = (uint16_t)varname[4] | (((uint16_t)(varname[5])) << 8);
	TRYF(ti82_send_ACK(handle));

	TRYF(ti82_send_CTS(handle));
	TRYF(ti82_recv_ACK(handle, NULL));

	update_->cnt2 = 0;
	update_->max2 = 3;

	content->data_part1 = tifiles_ve_alloc_data(65536);
	TRYF(ti82_recv_XDP(handle, &content->data_length1, content->data_part1));
	TRYF(ti82_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part2 = tifiles_ve_alloc_data(65536);
	TRYF(ti82_recv_XDP(handle, &content->data_length2, content->data_part2));
	TRYF(ti82_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part3 = tifiles_ve_alloc_data(65536);
	TRYF(ti82_recv_XDP(handle, &content->data_length3, content->data_part3));
	TRYF(ti82_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part4 = NULL;

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

		TRYF(ti82_send_RTS(handle, (uint16_t)entry->size, entry->type, entry->name));
		TRYF(ti82_recv_ACK(handle, &status));

		TRYF(ti82_recv_SKP(handle, &rej_code));
		TRYF(ti82_send_ACK(handle));

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

		TRYF(ti82_send_XDP(handle, entry->size, entry->data));
		TRYF(ti82_recv_ACK(handle, &status));

		TRYF(ti82_send_EOT(handle));
		ticalcs_info("");

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

	content->model = CALC_TI83;
	strcpy(content->comment, tifiles_comment_set_single());
	content->num_entries = 1;
	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	// silent request
	TRYF(ti82_send_REQ(handle, (uint16_t)vr->size, vr->type, vr->name));
	TRYF(ti82_recv_ACK(handle, &unused));

	TRYF(ti82_recv_VAR(handle, &ve_size, &ve->type, ve->name));
	ve->size = ve_size;
	TRYF(ti82_send_ACK(handle));

	TRYF(ti82_send_CTS(handle));
	TRYF(ti82_recv_ACK(handle, NULL));

	ve->data = tifiles_ve_alloc_data(ve->size);
	TRYF(ti82_recv_XDP(handle, &ve_size, ve->data));
	ve->size = ve_size;
	TRYF(ti82_send_ACK(handle));

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
	TRYF(rd_send(handle, "romdump.83p", romDumpSize83, romDump83));

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int i;
	static const uint16_t keys[] = {				
		0x40, 0x09, 0x09,			/* Quit, Clear, Clear, */
		0xFE63, 0x97, 0xDA,			/* Send(, 9, prgm */
		0xAB, 0xA8, 0xA6, 0x9D,		/* R, O, M, D */
		0xAE, 0xA6, 0xA9, 0x05		/* U, M, P, Enter */
	};

	// Launch program by remote control
	for(i = 0; i < (int)(sizeof(keys) / sizeof(keys[0])); i++)
	{
		TRYF(send_key(handle, keys[i]));
		PAUSE(100);
	}

	// Get dump
	TRYF(rd_dump(handle, filename));

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	unsigned int i;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	send_key(handle, 0x0040);	// Quit
	send_key(handle, 0x0009);	// Clear
	send_key(handle, 0x0009);	// Clear
	send_key(handle, 0x003e);	// Catalog
	send_key(handle, 0x009d);	// D
	send_key(handle, 0x0004);	// Down
	send_key(handle, 0x0004);	// Down
	send_key(handle, 0x0004);	// Down
	send_key(handle, 0x0005);	// Enter
	
	for(i = 0; i < strlen(vr->name); i++)
	{
		char c = toupper(vr->name[i]);

		if(isdigit(c))
			send_key(handle, (uint16_t)(0x008e + c - '0'));
		else
			send_key(handle, (uint16_t)(0x009a + c - 'A'));
	}

	send_key(handle, 0x0005);	// Enter

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

const CalcFncts calc_83 = 
{
	CALC_TI83,
	"TI83",
	"TI-83",
	"TI-83",
	OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR |
	FTS_SILENT | FTS_MEMFREE | FTS_BACKUP,
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
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
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
