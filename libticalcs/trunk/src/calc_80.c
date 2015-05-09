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
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmdz80.h"
//#include "rom80.h"
#include "romdump.h"

// Screen coordinates of the TI80
#define TI80_ROWS  48
#define TI80_COLS  64

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
	sc->pixel_format = CALC_PIXFMT_MONO;

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
	 "",     /* dump_rom_1 */
	 "",     /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 ""      /* recv_all_vars_backup */ },
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var,
	&noop_recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_flash,
	&noop_recv_idlist,
	&noop_dump_rom_1,
	&noop_dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&noop_del_var,
	&noop_new_folder,
	&noop_get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup
};
