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

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd85.h"
#include "rom86.h"
#include "romdump.h"
#include "keys86.h"

// Screen coordinates of the TI86
#define TI86_ROWS  64
#define TI86_COLS  128

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti85_send_KEY(handle, key));
	TRYF(ti85_recv_ACK(handle, &key));
	TRYF(ti85_recv_ACK(handle, &key));

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
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

	TRYF(ti85_send_SCR(handle));
	TRYF(ti85_recv_ACK(handle, NULL));

	err = ti85_recv_XDP(handle, &max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti85_send_ACK(handle));

	*bitmap = (uint8_t *)g_malloc(TI86_COLS * TI86_ROWS / 8);
	if(*bitmap == NULL) return ERR_MALLOC;
	memcpy(*bitmap, buf, TI86_COLS * TI86_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	TreeInfo *ti;
	GNode *folder;
	uint16_t unused;
	uint8_t hl, ll, lh;
	uint8_t mem[8];
	char *utf8;

	// get list of folders & FLASH apps
	(*vars) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	(*apps) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

	TRYF(ti85_send_REQ(handle, 0x0000, TI86_DIR, ""));
	TRYF(ti85_recv_ACK(handle, &unused));

	TRYF(ti85_recv_XDP(handle, &unused, mem));
	TRYF(ti85_send_ACK(handle));

	hl = mem[0];
	ll = mem[1];
	lh = mem[2];
	ti->mem_free = ((uint32_t)hl << 16) | ((uint32_t)lh << 8) | ll;

	folder = g_node_new(NULL);
	g_node_append(*vars, folder);

	// Add permanent variables (Func, Pol, Param, DifEq, ZRCL as WIND, WIND, WIND, WIND, WIND)
	{
		GNode *node;
		VarEntry *ve;

		ve = tifiles_ve_create();
		ve->type = TI86_FUNC;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI86_POL;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI86_PARAM;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI86_DIFEQ;
		node = g_node_new(ve);
		g_node_append(folder, node);

		ve = tifiles_ve_create();
		ve->type = TI86_ZRCL;
		node = g_node_new(ve);
		g_node_append(folder, node);
	}

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		GNode *node;
		int err;
		uint16_t ve_size;

		err = ti85_recv_VAR(handle, &ve_size, &ve->type, ve->name);
		ve->size = ve_size;
		TRYF(ti85_send_ACK(handle));
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
	uint8_t hl, ll, lh;
	uint8_t mem[8];

	TRYF(ti85_send_REQ(handle, 0x0000, TI86_DIR, ""));
	TRYF(ti85_recv_ACK(handle, &unused));

	TRYF(ti85_recv_XDP(handle, &unused, mem));
	TRYF(ti85_send_EOT(handle));

	hl = mem[0];
	ll = mem[1];
	lh = mem[2];
	*ram = ((uint32_t)hl << 16) | ((uint32_t)lh << 8) | ll;
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

	TRYF(ti85_send_VAR(handle, content->data_length1, TI86_BKUP, varname));
	TRYF(ti85_recv_ACK(handle, &status));

	g_snprintf(update_->text, sizeof(update_->text), _("Waiting for user's action..."));
	update_label();

	do
	{	// wait user's action
		if (update_->cancel)
		{
			return ERR_ABORT;
		}
		err = ti85_recv_SKP(handle, &rej_code);
	}
	while (err == ERROR_READ_TIMEOUT);
	TRYF(ti85_send_ACK(handle));

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

	strcpy(update_->text, "");
	update_label();

	update_->cnt2 = 0;
	update_->max2 = 4;

	TRYF(ti85_send_XDP(handle, content->data_length1, content->data_part1));
	TRYF(ti85_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	TRYF(ti85_send_XDP(handle, content->data_length2, content->data_part2));
	TRYF(ti85_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	if (content->data_length3)
	{
		TRYF(ti85_send_XDP(handle, content->data_length3, content->data_part3));
		TRYF(ti85_recv_ACK(handle, &status));
	}
	update_->cnt2++;
	update_->pbar();

	TRYF(ti85_send_XDP(handle, content->data_length4, content->data_part4));
	TRYF(ti85_recv_ACK(handle, &status));
	update_->cnt2++;
	update_->pbar();

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };

	g_snprintf(update_->text, sizeof(update_->text), _("Waiting for backup..."));
	update_label();

	content->model = CALC_TI86;
	strcpy(content->comment, tifiles_comment_set_backup());

	TRYF(ti85_recv_VAR(handle, &(content->data_length1), &content->type, varname));
	content->data_length2 = (uint16_t)varname[0] | (((uint16_t)(varname[1])) << 8);
	content->data_length3 = (uint16_t)varname[2] | (((uint16_t)(varname[3])) << 8);
	content->data_length4 = (uint16_t)varname[4] | (((uint16_t)(varname[5])) << 8);
	TRYF(ti85_send_ACK(handle));

	TRYF(ti85_send_CTS(handle));
	TRYF(ti85_recv_ACK(handle, NULL));

	strcpy(update_->text, "");
	update_label();

	update_->cnt2 = 0;
	update_->max2 = 4;
	update_->pbar();

	content->data_part1 = tifiles_ve_alloc_data(65536);
	TRYF(ti85_recv_XDP(handle, &content->data_length1, content->data_part1));
	TRYF(ti85_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	content->data_part2 = tifiles_ve_alloc_data(65536);
	TRYF(ti85_recv_XDP(handle, &content->data_length2, content->data_part2));
	TRYF(ti85_send_ACK(handle));
	update_->cnt2++;
	update_->pbar();

	if (content->data_length3)
	{
		content->data_part3 = tifiles_ve_alloc_data(65536);
		TRYF(ti85_recv_XDP(handle, &content->data_length3, content->data_part3));
		TRYF(ti85_send_ACK(handle));
	}
	else
	{
		content->data_part3 = NULL;
	}
	update_->cnt2++;
	update_->pbar();

	content->data_part4 = tifiles_ve_alloc_data(65536);
	TRYF(ti85_recv_XDP(handle, &content->data_length4, content->data_part4));
	TRYF(ti85_send_ACK(handle));
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

		TRYF(ti85_send_RTS(handle, (uint16_t)entry->size, entry->type, entry->name));
		TRYF(ti85_recv_ACK(handle, &status));

		TRYF(ti85_recv_SKP(handle, &rej_code));
		TRYF(ti85_send_ACK(handle));

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

		TRYF(ti85_send_XDP(handle, entry->size, entry->data));
		TRYF(ti85_recv_ACK(handle, &status));

		TRYF(ti85_send_EOT(handle));
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

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	// silent request
	TRYF(ti85_send_REQ(handle, (uint16_t)vr->size, vr->type, vr->name));
	TRYF(ti85_recv_ACK(handle, &unused));

	TRYF(ti85_recv_VAR(handle, &ve_size, &ve->type, ve->name));
	ve->size = ve_size;
	TRYF(ti85_send_ACK(handle));

	TRYF(ti85_send_CTS(handle));
	TRYF(ti85_recv_ACK(handle, NULL));

	ve->data = tifiles_ve_alloc_data(ve->size);
	TRYF(ti85_recv_XDP(handle, &ve_size, ve->data));
	ve->size = ve_size;
	TRYF(ti85_send_ACK(handle));

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

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	static const uint16_t keys[] = {
		0x76, 0x08, 0x08,       /* Quit, Clear, Clear,	*/
		0x28, 0x3A, 0x34, 0x11, /* A, S, M, (,			*/
		0x39, 0x36, 0x34, 0x2B, /* R, O, M, D	*/
		0x56, 0x4E, 0x51, 0x12, /* u, m, p, )	*/
		0x06 };                 /* Enter		*/
	uint16_t dummy;
	int i;

	// Launch program by remote control
	for(i = 0; i < (int) G_N_ELEMENTS(keys) - 1; i++)
	{
		TRYF(send_key(handle, keys[i]));
	}

	TRYF(ti85_send_KEY(handle, keys[i]));
	TRYF(ti85_recv_ACK(handle, &dummy));
	PAUSE(200);

	// Get dump
	TRYF(rd_dump(handle, filename));

	// (Normally there would be another ACK after the program exits,
	// but the ROM dumper disables that behavior)

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
	"TI-86",
	"TI-86",
	OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	FTS_SILENT | FTS_MEMFREE | FTS_BACKUP,
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
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
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
