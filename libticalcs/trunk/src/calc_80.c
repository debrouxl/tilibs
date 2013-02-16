/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
	TI80 support. Note: the source code is the SAME as the TI85 support (same indentation).
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

#include "dbus_pkt.h"
#include "cmd80.h"
//#include "rom80.h"
#include "romdump.h"

// Screen coordinates of the TI80
#define TI80_ROWS  48
#define TI80_COLS  64

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	uint8_t buf[TI80_COLS * TI80_ROWS / 8];
	int stripe, row, i = 0;
	int retval = 0;

	sc->width = TI80_COLS;
	sc->height = TI80_ROWS;
	sc->clipped_width = TI80_COLS;
	sc->clipped_height = TI80_ROWS;

	retval = ti80_send_SCR(handle);
	if (!retval)
	{
		retval = ti80_recv_ACK(handle, NULL);

		if (!retval)
		{
			retval = ti80_recv_XDP(handle, &max_cnt, buf);
			if (!retval)
			{
				*bitmap = (uint8_t *)g_malloc(TI80_COLS * TI80_ROWS / 8);
				if (*bitmap == NULL)
				{
					return ERR_MALLOC;
				}

				for(stripe = 7; stripe >= 0; stripe--)
				{
					for(row = 0; row < TI80_ROWS; row++)
					{
						(*bitmap)[row * TI80_COLS / 8 + stripe] = buf[i++];
					}
				}
			}
		}
	}

	return retval;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
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

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
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
	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
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

const CalcFncts calc_80 = 
{
	CALC_TI80,
	"TI80",
	"TI-80",
	"TI-80 ViewScreen",
	OPS_SCREEN,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "",     /* dump_rom1 */
	 "",     /* dump_rom2 */
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
