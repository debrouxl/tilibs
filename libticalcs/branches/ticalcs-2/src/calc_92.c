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

/* Initialize the LinkCable structure with default functions */
/* This module can be used as sample code.*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>

#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"

#include "cmd92.h"
#include "keys92p.h"
#include "rom92f2.h"

// Screen coordinates of the TI92
#define TI92_ROWS  128
#define TI92_COLS  240

// Rom dumping
#define DUMP_ROM92_FILE "dumprom.92p"
#define ROMSIZE (1024*1024)

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	TRYF(ti92_send_RDY());
	TRYF(ti92_recv_ACK(&status));

	return (status & 0x01) ? ERR_NOT_READY : 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti92_send_KEY(key));
	TRYF(ti92_recv_ACK(NULL));
	
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint32_t max_cnt;
	int err;

	sc->width = TI92_COLS;
	sc->height = TI92_ROWS;
	sc->clipped_width = TI92_COLS;
	sc->clipped_height = TI92_ROWS;

	*bitmap = (uint8_t *) malloc(TI92_COLS * TI92_ROWS * sizeof(uint8_t) / 8);
	if(*bitmap == NULL)
		return ERR_MALLOC;

	TRYF(ti92_send_SCR());
	TRYF(ti92_recv_ACK(NULL));

	err = ti92_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti92_send_ACK());

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	uint32_t unused;
	uint8_t buffer[65536];
	int err;
	VarEntry info;
	char folder_name[9] = "";
	TNode *folder = NULL;

	TRYF(ti92_send_REQ(0, TI92_RDIR, ""));
	TRYF(ti92_recv_ACK(NULL));
	TRYF(ti92_recv_VAR(&info.size, &info.type, info.name));

	*vars = t_node_new(NULL);
	(*vars)->data = strdup(VAR_NODE_NAME);
	*apps = t_node_new(NULL);
	(*apps)->data = strdup(APP_NODE_NAME);

	for (;;) 
	{
		VarEntry *ve = calloc(1, sizeof(VarEntry));
		TNode *node;

		TRYF(ti92_send_ACK());
		TRYF(ti92_send_CTS());

		TRYF(ti92_recv_ACK(NULL));
		TRYF(ti92_recv_XDP(&unused, buffer));
		memcpy(ve->name, buffer + 4, 8);	// skip 4 extra 00s
		ve->name[8] = '\0';
		ve->type = buffer[12];
		ve->attr = buffer[13];
		ve->size = buffer[14] | (buffer[15] << 8) | (buffer[16] << 16) | (buffer[17] << 24);
		strcpy(ve->fld_name, "");

		tifiles_transcode_detokenize(handle->model, ve->var_name, ve->name, ve->type);
		node = t_node_new(ve);

		if (ve->type == TI92_DIR) 
		{
			strcpy(folder_name, ve->var_name);
			folder = t_node_append(*vars, node);
		} 
		else 
		{
			strcpy(ve->fld_name, folder_name);
			t_node_append(folder, node);
		}

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			ve->name, 
			tifiles_vartype2string(handle->model, ve->type),
			ve->attr,
			ve->size);

		TRYF(ti92_send_ACK());
		err = ti92_recv_CNT();
		if (err == ERR_EOT)
			break;
		TRYF(err);

		sprintf(update->text, _("Reading of '%s/%s'"),
			((VarEntry *) (folder->data))->var_name, ve->var_name);
		update->label();
		if (update->cancel)
			return -1;
	}

	TRYF(ti92_send_ACK());

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* mem)
{
	*mem = -1;
	return ERR_UNSUPPORTED;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	int i;
	int nblocks;

	sprintf(update->text, _("Sending backup..."));
	update_label();

	TRYF(ti92_send_VAR(content->data_length, TI92_BKUP, content->rom_version));
	TRYF(ti92_recv_ACK(NULL));

	nblocks = content->data_length / 1024;
	handle->updat->max2 = nblocks;

	for (i = 0; i <= nblocks; i++) 
	{
		uint32_t length = (i != nblocks) ? 1024 : content->data_length % 1024;

		TRYF(ti92_send_VAR(length, TI92_BKUP, content->rom_version));
		TRYF(ti92_recv_ACK(NULL));

		TRYF(ti92_recv_CTS());
		TRYF(ti92_send_ACK());

		TRYF(ti92_send_XDP(length, content->data_part + 1024 * i));
		TRYF(ti92_recv_ACK(NULL));

		handle->updat->cnt2 = i;
		update_pbar();
		if (update->cancel)
			return -1;
	}

	TRYF(ti92_send_EOT());

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	uint32_t block_size;
	int block, err;
	uint32_t unused;
	uint16_t unused2;
	uint8_t *ptr;

	content->model = CALC_TI92;
	sprintf(update->text, _("Receiving backup..."));
	update_label();

	// silent request
	TRYF(ti92_send_REQ(0, TI92_BKUP, "main\\backup"));
	TRYF(ti92_recv_ACK(&unused2));

	content->data_part = (uint8_t *) tifiles_calloc(128 * 1024, 1);
	content->type = TI92_BKUP;
	content->data_length = 0;

	for (block = 0;; block++) 
	{
		sprintf(update->text, _("Receiving block %2i"), block);
		update_label();
    
		err = ti92_recv_VAR(&block_size, &content->type, content->rom_version);
		TRYF(ti92_send_ACK());

		if (err == ERR_EOT)
			break;
		TRYF(err);

		TRYF(ti92_send_CTS());
		TRYF(ti92_recv_ACK(NULL));

		ptr = content->data_part + content->data_length;
		TRYF(ti92_recv_XDP(&unused, ptr));
		memmove(ptr, ptr + 4, block_size);
		TRYF(ti92_send_ACK());
		content->data_length += block_size;
	}

	strcpy(content->comment, "Backup file received by TiLP");

	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint16_t status;

	sprintf(update->text, _("Sending variable(s)..."));
	update_label();

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = &(content->entries[i]);
		uint8_t buffer[65536 + 4] = { 0 };
		uint8_t full_name[18], utf8[35];

		if (mode & MODE_LOCAL_PATH)
		{
		  strcpy(full_name, entry->var_name);
		}
		else 
		{
		  strcpy(full_name, entry->fld_name);
		  strcat(full_name, "\\");
		  strcat(full_name, entry->var_name);
		}

		tifiles_transcode_detokenize(handle->model, utf8, full_name, entry->type);
		sprintf(update->text, _("Sending '%s'"), utf8);
		update_label();

		TRYF(ti92_send_VAR(entry->size, entry->type, full_name));
		TRYF(ti92_recv_ACK(NULL));

		TRYF(ti92_recv_CTS());
		TRYF(ti92_send_ACK());

		memcpy(buffer + 4, entry->data, entry->size);
		TRYF(ti92_send_XDP(entry->size + 4, buffer));
		TRYF(ti92_recv_ACK(&status));

		TRYF(ti92_send_EOT());
		TRYF(ti92_recv_ACK(NULL));

		ticalcs_info("");
	}

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	uint16_t status;
	VarEntry *ve;
	uint32_t unused;
	uint8_t varname[18], utf8[35];

	strcpy(content->comment, "Single file received by TiLP");
	content->model = CALC_TI92;
	content->num_entries = 1;
	content->entries = (VarEntry *) tifiles_calloc(1, sizeof(VarEntry));
	ve = &(content->entries[0]);
	memcpy(ve, vr, sizeof(VarEntry));

	strcpy(varname, vr->fld_name);
	strcat(varname, "\\");
	strcat(varname, vr->var_name);

	tifiles_transcode_detokenize(handle->model, utf8, vr->name, vr->type);
	sprintf(update->text, _("Receiving '%s'"), utf8);
	update_label();

	TRYF(ti92_send_REQ(0, vr->type, varname));
	TRYF(ti92_recv_ACK(&status));
	if (status != 0)
		return ERR_MISSING_VAR;

	TRYF(ti92_recv_VAR(&ve->size, &ve->type, ve->name));
	TRYF(ti92_send_ACK());

	TRYF(ti92_send_CTS());
	TRYF(ti92_recv_ACK(NULL));

	ve->data = tifiles_calloc(ve->size + 4, 1);
	TRYF(ti92_recv_XDP(&unused, ve->data));
	memmove(ve->data, ve->data + 4, ve->size);
	TRYF(ti92_send_ACK());

	TRYF(ti92_recv_EOT());
	TRYF(ti92_send_ACK());

	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	return ERR_UNSUPPORTED;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return ERR_UNSUPPORTED;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry* ve)
{
	return ERR_UNSUPPORTED;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	return ERR_UNSUPPORTED;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	return ERR_UNSUPPORTED;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	return ERR_UNSUPPORTED;
}

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int i, j, k;
	uint8_t data;
	time_t start, elapsed, estimated, remaining;
	char buffer[257];
	char tmp[257];
	int pad;
	FILE *f, *file;
	uint16_t checksum, sum;
	FileContent content;

	// Copies ROM dump program into a file
	f = fopen(DUMP_ROM92_FILE, "wb");
	if (f == NULL)
		return ERR_FILE_OPEN;
	fwrite(romDump92f2, sizeof(unsigned char), romDumpSize92f2, f);
	fclose(f);

	// Transfer program to calc
	tifiles_file_read_regular(DUMP_ROM92_FILE, &content);
	TRYF(send_var(handle, MODE_SEND_ONE_VAR, &content));
	tifiles_content_free_regular(&content);
	unlink(DUMP_ROM92_FILE);

	// Launch calculator program by remote control
	sprintf(update->text, _("Launching..."));
	update->label();

	TRYF(send_key(handle, KEY92P_CLEAR));
	PAUSE(50);
	TRYF(send_key(handle, KEY92P_CLEAR));
	PAUSE(50);
    TRYF(send_key(handle, 'm'));
    TRYF(send_key(handle, 'a'));
    TRYF(send_key(handle, 'i'));
    TRYF(send_key(handle, 'n'));
    TRYF(send_key(handle, '\\'));
    TRYF(send_key(handle, 'd'));
    TRYF(send_key(handle, 'u'));
    TRYF(send_key(handle, 'm'));
    TRYF(send_key(handle, 'p'));
    TRYF(send_key(handle, 'r'));
    TRYF(send_key(handle, 'o'));
    TRYF(send_key(handle, 'm'));
    TRYF(send_key(handle, KEY92P_LP));
    TRYF(send_key(handle, KEY92P_RP));
    TRYF(send_key(handle, KEY92P_ENTER));

	// Open file
	file = fopen(filename, "wb");
	if (file == NULL)
		return ERR_OPEN_FILE;

	// Receive it now blocks per blocks (1024 + CHK)
	sprintf(update->text, _("Receiving..."));
	update->label();

	start = time(NULL);
	update->max1 = 1024;
	update->max2 = 1024 * size;

	for (i = 0, k = 0; i < size * 1024; i++) 
	{
		sum = 0;
		for (j = 0; j < 1024; j++) 
		{
			TRYF(ticables_cable_get(handle->cable, &data));
			fprintf(file, "%c", data);
			sum += data;

			update->cnt1 = j;
			update->pbar();
			if (update->cancel)
				return -1;
		}

		TRYF(ticables_cable_get(handle->cable, &data));
		checksum = data << 8;
		TRYF(ticables_cable_get(handle->cable, &data));
		checksum |= data;
		if (sum != checksum)
		  return ERR_CHECKSUM;
		TRYF(ticables_cable_put(handle->cable, 0xDA));
		
		update->cnt2 = i;
		if (update->cancel)
			return -1;

		elapsed = (long) difftime(time(NULL), start);
		estimated = (long) (elapsed * (float) (1024 * size) / i);
		remaining = (long) difftime(estimated, elapsed);
		sprintf(buffer, "%s", ctime(&remaining));
		sscanf(buffer, "%3s %3s %i %s %i", tmp, tmp, &pad, tmp, &pad);
		sprintf(update->text, _("Remaining (mm:ss): %s"), tmp + 3);
		update->label();
	}

	// make ROM dumping program exit.
	TRYF(ticables_cable_put(handle->cable, 0xCC));
	fclose(file);

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{
	return ERR_UNSUPPORTED;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	return ERR_UNSUPPORTED;
}

const CalcFncts calc_92 = 
{
	CALC_TI92,
	"TI92",
	N_("TI-92"),
	N_("TI-92"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	FTS_SILENT | FTS_FOLDER,
	&is_ready,
	&send_key,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&del_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
};
