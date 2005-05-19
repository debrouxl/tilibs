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

#include "cmd82.h"
#include "rom83.h"

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
	if(*bitmap != NULL)
		return ERR_MALLOC;

	TRYF(ti82_send_SCR());
	TRYF(ti82_recv_ACK(NULL));

	err = ti82_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti82_send_ACK());

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* mem)
{
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
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
	sprintf(handle->update->text, _("Waiting user's action..."));
	handle->update->label();
	do 
	{
		handle->update->refresh();
		if (handle->update->cancel)
			return ERR_ABORT;
		err = ticables_cable_get(handle->cable, &data);
		sum = data;
	}
	while (err == ERR_READ_TIMEOUT);
	fprintf(file, "%c", data);

	// Receive it now blocks per blocks (1024 + CHK)
	sprintf(handle->update->text, _("Receiving..."));
	handle->update->label();

	start = time(NULL);
	handle->update->max1 = 1024;
	handle->update->max2 = ROMSIZE;

	for (i = 0; i < ROMSIZE; i++) 
	{
		if (b)
			sum = 0;

		for (j = 0; j < 1023 + b; j++) 
		{
			TRYF(ticables_cable_get(handle->cable, &data));
			fprintf(file, "%c", data);
			sum += data;

			handle->update->cnt1 = j;
			handle->update->pbar();
			if (handle->update->cancel)
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

		handle->update->cnt2 = i;
		if (handle->update->cancel)
		  return -1;

		elapsed = (long) difftime(time(NULL), start);
		estimated = (long) (elapsed * (float) (ROMSIZE) / i);
		remaining = (long) difftime(estimated, elapsed);
		sprintf(buffer, "%s", ctime(&remaining));
		sscanf(buffer, "%3s %3s %i %s %i", tmp, tmp, &pad, tmp, &pad);
		sprintf(handle->update->text, _("Remaining (mm:ss): %s"), tmp + 3);
		handle->update->label();
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

const CalcFncts calc_83 = 
{
	CALC_TI83,
	"TI83",
	N_("TI-83"),
	N_("TI-83"),
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
