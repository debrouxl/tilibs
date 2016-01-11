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
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmdz80.h"
#include "rom83.h"
#include "romdump.h"
#include "keys83.h"

// Screen coordinates of the TI83
#define TI83_ROWS  64
#define TI83_COLS  96

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	int ret;

	ret = ti82_send_KEY(handle, key);
	if (!ret)
	{
		ret = ti82_recv_ACK(handle, &key);
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	int ret;

	// Go back to homescreen
	PAUSE(200);
	ret = send_key(handle, KEY83_Quit);
	if (!ret)
	{
		ret = send_key(handle, KEY83_Clear);
		if (!ret)
		{
			ret = send_key(handle, KEY83_Clear);
		}
	}

	if (!ret)
	{
		// Launch program by remote control
		if (ve->type == TI83_ASM)
		{
			ret = send_key(handle, KEY83_SendMBL);
			if (!ret)
			{
				ret = send_key(handle, KEY83_9);
			}
		}
		if (!ret)
		{
			ret = send_key(handle, KEY83_Exec);
			if (!ret)
			{
				unsigned int i;
				for (i = 0; !ret && i < strlen(ve->name); i++)
				{
					const CalcKey *ck = ticalcs_keys_83((ve->name)[i]);
					ret = send_key(handle, ck->normal.value);
				}

				if (!ret)
				{
					ret = send_key(handle, KEY83_Enter);

					PAUSE(200);
				}
			}
		}
	}

	return ret;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;

	*bitmap = (uint8_t *)ticalcs_alloc_screen(65537U);
	if (*bitmap == NULL)
	{
		return ERR_MALLOC;
	}

	sc->width = TI83_COLS;
	sc->height = TI83_ROWS;
	sc->clipped_width = TI83_COLS;
	sc->clipped_height = TI83_ROWS;
	sc->pixel_format = CALC_PIXFMT_MONO;

	ret = ti82_send_SCR(handle);
	if (!ret)
	{
		ret = ti82_recv_ACK(handle, NULL);
		if (!ret)
		{
			uint16_t max_cnt;
			ret = ti82_recv_XDP(handle, &max_cnt, *bitmap);
			if (!ret || ret == ERR_CHECKSUM) // problem with checksum
			{
				*bitmap = ticalcs_realloc_screen(*bitmap, TI83_COLS * TI83_ROWS / 8);
				ret = ti82_send_ACK(handle);
			}
		}
	}

	if (ret)
	{
		ticalcs_free_screen(*bitmap);
		*bitmap = NULL;
	}

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	int ret;
	TreeInfo *ti;
	GNode *folder, *node;
	uint16_t unused;
	uint32_t memory;
	char *utf8;

	ret = dirlist_init_trees(handle, vars, apps, VAR_NODE_NAME);
	if (ret)
	{
		return ret;
	}
	ti = (*vars)->data;

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_DIR, "\0\0\0\0\0\0\0"));
	TRYF(ti82_recv_ACK(handle, &unused));

	TRYF(ti82_recv_XDP(handle, &unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti82_send_ACK(handle));
	ti->mem_free = memory;

	folder = dirlist_create_append_node(NULL, vars);

	// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
	{
		VarEntry *ve;

		ve = tifiles_ve_create();
		ve->type = TI83_WINDW;
		node = dirlist_create_append_node(ve, &folder);
		if (node != NULL)
		{
			ve = tifiles_ve_create();
			ve->type = TI83_ZSTO;
			node = dirlist_create_append_node(ve, &folder);
			if (node != NULL)
			{
				ve = tifiles_ve_create();
				ve->type = TI83_TABLE;
				node = dirlist_create_append_node(ve, &folder);
			}
		}
	}

	if (!node)
	{
		return ERR_MALLOC;
	}

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		int err;
		uint16_t ve_size;

		err = ti82_recv_VAR(handle, &ve_size, &ve->type, ve->name);
		ve->size = ve_size;
		TRYF(ti82_send_ACK(handle));
		if (err == ERR_EOT)
		{
			break;
		}
		else if (err != 0)
		{
			return err;
		}

		node = dirlist_create_append_node(ve, &folder);
		if (!node)
		{
			return ERR_MALLOC;
		}

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		snprintf(update_->text, sizeof(update_->text) - 1, _("Parsing %s"), utf8);
		update_->text[sizeof(update_->text) - 1] = 0;
		ticonv_utf8_free(utf8);
		update_label();
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint16_t unused;
	uint32_t memory;

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_DIR, "\0\0\0\0\0\0\0"));
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

	return ti82_send_ACK(handle);
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	char varname[9] = { 0 };
	uint16_t unused;

	content->model = CALC_TI83;
	strncpy(content->comment, tifiles_comment_set_backup(), sizeof(content->comment) - 1);
	content->comment[sizeof(content->comment) - 1] = 0;

	TRYF(ti82_send_REQ(handle, 0x0000, TI83_BKUP, "\0\0\0\0\0\0\0"));
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
	unsigned int i;
	uint8_t rej_code;
	uint16_t status;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];

		if (entry->action == ACT_SKIP)
		{
			continue;
		}

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
		strncpy(update_->text, utf8, sizeof(update_->text) - 1);
		update_->text[sizeof(update_->text) - 1] = 0;
		ticonv_utf8_free(utf8);
		update_label();

		TRYF(ti82_send_XDP(handle, entry->size, entry->data));
		TRYF(ti82_recv_ACK(handle, &status));

		TRYF(ti82_send_EOT(handle));
		ticalcs_info("Sent variable #%u", i);

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
	strncpy(content->comment, tifiles_comment_set_single(), sizeof(content->comment) - 1);
	content->comment[sizeof(content->comment) - 1] = 0;
	content->num_entries = 1;
	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
	strncpy(update_->text, utf8, sizeof(update_->text) - 1);
	update_->text[sizeof(update_->text) - 1] = 0;
	ticonv_utf8_free(utf8);
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
	return ti82_send_ACK(handle);
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	return rd_send(handle, "romdump.83p", romDumpSize83, romDump83);
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
	for (i = 0; i < (int)(sizeof(keys) / sizeof(keys[0])); i++)
	{
		TRYF(send_key(handle, keys[i]));
		PAUSE(100);
	}

	// Get dump
	return rd_dump(handle, filename);
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	unsigned int i;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	snprintf(update_->text, sizeof(update_->text) - 1, _("Deleting %s..."), utf8);
	update_->text[sizeof(update_->text) - 1] = 0;
	ticonv_utf8_free(utf8);
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

	for (i = 0; i < strlen(vr->name); i++)
	{
		char c = toupper(vr->name[i]);

		if (isdigit(c))
		{
			send_key(handle, (uint16_t)(0x008e + c - '0'));
		}
		else
		{
			send_key(handle, (uint16_t)(0x009a + c - 'A'));
		}
	}

	send_key(handle, 0x0005);	// Enter

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
	PRODUCT_ID_NONE,
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
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 ""      /* recv_all_vars_backup */ },
	&noop_is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&del_var,
	&noop_new_folder,
	&noop_get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup
};
