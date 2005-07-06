/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "packets.h"
#include "cmd82.h"
#include "rom83.h"
#include "keys83p.h"

// Screen coordinates of the TI83
#define TI83_ROWS  64
#define TI83_COLS  96

#define DUMP_ROM83_FILE "dumprom.83p"
#define ROMSIZE 256		// 256 KB

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti83_send_KEY(key));
	TRYF(ti82_recv_ACK(NULL));

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;

	sc->width = TI83_COLS;
	sc->height = TI83_ROWS;
	sc->clipped_width = TI83_COLS;
	sc->clipped_height = TI83_ROWS;

	*bitmap = (uint8_t *)malloc(TI83_COLS * TI83_ROWS * sizeof(uint8_t) / 8);
	if(*bitmap == NULL) return ERR_MALLOC;

	TRYF(ti82_send_SCR());
	TRYF(ti82_recv_ACK(NULL));

	err = ti82_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti82_send_ACK());

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	TreeInfo *ti;
	TNode *folder;
	uint16_t unused;	

	uint32_t memory;
	char utf8[10];

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

	TRYF(ti82_send_REQ(0x0000, TI83_DIR, ""));
	TRYF(ti82_recv_ACK(&unused));

	TRYF(ti82_recv_XDP(&unused, (uint8_t *)&memory));
	fixup(memory);
	TRYF(ti82_send_ACK());
	handle->priv = GUINT_TO_POINTER(memory);



	
	folder = t_node_new(NULL);
	t_node_append(*vars, folder);

	for (;;) 
	{
		VarEntry *ve = calloc(1, sizeof(VarEntry));
		TNode *node;
		int err;

		err = ti82_recv_VAR((uint16_t *) & ve->size, &ve->type, ve->name);
		fixup(ve->size);
		TRYF(ti82_send_ACK());
		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		node = t_node_new(ve);
		t_node_append(folder, node);

		tifiles_transcode_varname(handle->model, utf8, ve->name, ve->type);
		sprintf(update->text, _("Reading of '%s'"), utf8);
		update_label();
		if (update->cancel)
			return ERR_ABORT;
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* mem)
{
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
    uint16_t length;
    uint8_t varname[9];
    uint8_t rej_code;
    uint16_t status;

    length = content->data_length1;
    varname[0] = LSB(content->data_length2);
    varname[1] = MSB(content->data_length2);
    varname[2] = LSB(content->data_length3);
    varname[3] = MSB(content->data_length3);
    varname[4] = LSB(content->mem_address);
    varname[5] = MSB(content->mem_address);

    TRYF(ti82_send_RTS(content->data_length1, TI83_BKUP, varname));
    TRYF(ti82_recv_ACK(&status));

    TRYF(ti82_recv_SKP(&rej_code))
    TRYF(ti82_send_ACK());

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

	update->max2 = 3;
	update->cnt2 = 0;

	TRYF(ti82_send_XDP(content->data_length1, content->data_part1));
	TRYF(ti82_recv_ACK(&status));
	update->cnt2++;

	TRYF(ti82_send_XDP(content->data_length2, content->data_part2));
	TRYF(ti82_recv_ACK(&status));
	update->cnt2++;

	TRYF(ti82_send_XDP(content->data_length3, content->data_part3));
	TRYF(ti82_recv_ACK(&status));
	update->cnt2++;

	TRYF(ti82_send_ACK());

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	uint8_t varname[9] = { 0 };
	uint16_t unused;

	content->model = CALC_TI83;
	strcpy(content->comment, tifiles_comment_set_backup());

	// silent request
	TRYF(ti82_send_REQ(0x0000, TI83_BKUP, ""));
	TRYF(ti82_recv_ACK(&unused));

	TRYF(ti82_recv_VAR(&(content->data_length1), &content->type, varname));
	content->data_length2 = varname[0] | (varname[1] << 8);
	content->data_length3 = varname[2] | (varname[3] << 8);
	content->mem_address = varname[4] | (varname[5] << 8);
	TRYF(ti82_send_ACK());

	TRYF(ti82_send_CTS());
	TRYF(ti82_recv_ACK(NULL));

	update->max2 = 3;
	update->cnt2 = 0;
	content->data_part4 = NULL;

	content->data_part1 = calloc(65536, 1);
	TRYF(ti82_recv_XDP(&content->data_length1, content->data_part1));
	TRYF(ti82_send_ACK());
	update->cnt2++;

	content->data_part2 = calloc(65536, 1);
	TRYF(ti82_recv_XDP(&content->data_length2, content->data_part2));
	TRYF(ti82_send_ACK());
	update->cnt2++;

	content->data_part3 = calloc(65536, 1);
	TRYF(ti82_recv_XDP(&content->data_length3, content->data_part3));
	TRYF(ti82_send_ACK());
	update->cnt2++;

	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint8_t rej_code;
	uint16_t status;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = &(content->entries[i]);

		if(entry->action == ACT_SKIP)
			continue;

		TRYF(ti82_send_RTS((uint16_t)entry->size, entry->type, entry->name));
		TRYF(ti82_recv_ACK(&status));

		TRYF(ti82_recv_SKP(&rej_code));
		TRYF(ti82_send_ACK());

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
		sprintf(update->text, _("Sending '%s'"),
			tifiles_transcode_varname_static(handle->model, entry->name, entry->type));
		update_label();

		TRYF(ti82_send_XDP(entry->size, entry->data));
		TRYF(ti82_recv_ACK(&status));

		ticalcs_info("");
  }

  return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
  uint16_t unused;
  VarEntry *ve;

	content->model = CALC_TI83;
	strcpy(content->comment, tifiles_comment_set_single());
	content->num_entries = 1;
	content->entries = (VarEntry *) calloc(1, sizeof(VarEntry));
	ve = &(content->entries[0]);
	memcpy(ve, vr, sizeof(VarEntry));

	sprintf(update->text, _("Receiving '%s'"),
		tifiles_transcode_varname_static(handle->model, vr->name, vr->type));
	update_label();

	// silent request
	TRYF(ti82_send_REQ((uint16_t)vr->size, vr->type, vr->name));
	TRYF(ti82_recv_ACK(&unused));

	TRYF(ti82_recv_VAR((uint16_t *) & ve->size, &ve->type, ve->name));
	TRYF(ti82_send_ACK());
	fixup(ve->size);

	TRYF(ti82_send_CTS());
	TRYF(ti82_recv_ACK(NULL));

	ve->data = calloc(ve->size, 1);
	TRYF(ti82_recv_XDP((uint16_t *) & ve->size, ve->data));
	TRYF(ti82_send_ACK());

	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry* ve)
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

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int i, j;
	uint8_t data;
	time_t start, elapsed, estimated, remaining;
	char buffer[256];
	char tmp[256];
	int pad;
	FILE *f, *file;
	uint16_t checksum, sum;
	int err;
	int b = 0;
	FileContent content;

	// Copies ROM dump program into a file
	f = fopen(DUMP_ROM83_FILE, "wb");
	if (f == NULL)
		return ERR_FILE_OPEN;
	fwrite(romDump83, sizeof(unsigned char), romDumpSize83, f);
	fclose(f);

	// Transfer program to calc
	tifiles_file_read_regular(DUMP_ROM83_FILE, &content);
	TRYF(send_var(handle, MODE_SEND_ONE_VAR, &content));
	tifiles_content_free_regular(&content);
	unlink(DUMP_ROM83_FILE);

	// Open file
	file = fopen(filename, "wb");
	if (file == NULL)
		return ERR_OPEN_FILE;

	// Wait for user's action (execing program)
	sprintf(handle->updat->text, _("Waiting user's action..."));
	handle->updat->label();
	do 
	{
		handle->updat->refresh();
		if (handle->updat->cancel)
			return ERR_ABORT;
		err = ticables_cable_get(handle->cable, &data);
		sum = data;
	}
	while (err == ERROR_READ_TIMEOUT);
	fprintf(file, "%c", data);

	// Receive it now blocks per blocks (1024 + CHK)
	sprintf(handle->updat->text, _("Receiving..."));
	handle->updat->label();

	start = time(NULL);
	handle->updat->max1 = 1024;
	handle->updat->max2 = ROMSIZE;

	for (i = 0; i < ROMSIZE; i++) 
	{
		if (b)
			sum = 0;

		for (j = 0; j < 1023 + b; j++) 
		{
			TRYF(ticables_cable_get(handle->cable, &data));
			fprintf(file, "%c", data);
			sum += data;

			handle->updat->cnt1 = j;
			handle->updat->pbar();
			if (handle->updat->cancel)
				return -1;
		}
		b = 1;

		TRYF(ticables_cable_get(handle->cable, &data));
		checksum = data << 8;
		TRYF(ticables_cable_get(handle->cable, &data));
		checksum |= data;
		if (sum != checksum)
		  return ERR_CHECKSUM;
		TRYF(ticables_cable_put(handle->cable, 0xDA));

		handle->updat->cnt2 = i;
		if (handle->updat->cancel)
		  return -1;

		elapsed = (long) difftime(time(NULL), start);
		estimated = (long) (elapsed * (float) (ROMSIZE) / i);
		remaining = (long) difftime(estimated, elapsed);
		sprintf(buffer, "%s", ctime(&remaining));
		sscanf(buffer, "%3s %3s %i %s %i", tmp, tmp, &pad, tmp, &pad);
		sprintf(handle->updat->text, _("Remaining (mm:ss): %s"), tmp + 3);
		handle->updat->label();
	}

	fclose(file);	

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
	unsigned int i;

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
		char c = vr->name[i];

		if(isdigit(c))
			send_key(handle, (uint16_t)(0x008e + c));
		else
			send_key(handle, (uint16_t)(0x009a + c));
	}

	send_key(handle, 0x0005);	// Enter

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

const CalcFncts calc_83 = 
{
	CALC_TI83,
	"TI83",
	N_("TI-83"),
	N_("TI-83"),
	OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR |
	FTS_SILENT | FTS_MEMFREE,
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
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
};
/*
const CalcFncts calc_82s = 
{
	CALC_TI82S,
	"TI82 STATS",
	N_("TI-82 STATS"),
	N_("TI-82 STATS"),
	OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	FTS_SILENT | FTS_MEMFREE,
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
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
};
*/
