/* Hey EMACS -*- linux-c -*- */
/* $Id: data_log.c 1720 2006-01-20 22:34:58Z roms $ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* 
	D-USB logging (taken from my TI link guide (hex2dusb program).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "logging.h"
#include "data_log.h"
#include "log_dusb.h"
#include "internal.h"

#define LOG_DUSB_FILE	"ticables-dusb.pkt"

static char *ifn = nullptr;
static char *ofn = nullptr;

int log_dusb_start(void)
{
	ifn = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, G_DIR_SEPARATOR_S, HEX_FILE, NULL);
	ofn = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, G_DIR_SEPARATOR_S, LOG_DUSB_FILE, NULL);

	return 0;
}

int log_dusb_N(int dir, const uint8_t * data, uint32_t len)
{
	return 0;
}

int log_dusb_stop(void)
{
	if (!ifn || ! ofn)
	{
		return 0;
	}

	char* r = strrchr(ifn, '.');
	if(r)
	{
		*r = '\0';
	}

	dusb_decomp(ifn);
	char* ifn2 = g_strconcat(ifn, ".pkt", NULL);
	g_free(ifn);
	ifn = ifn2;

	g_unlink(ofn);
	if (g_rename(ifn, ofn) < 0)
	{
		fprintf(stderr, "Failed to rename output file\n");
	}

	g_free(ifn); 
	ifn = nullptr;
	g_free(ofn); 
	ofn = nullptr;

	return 0;
}
