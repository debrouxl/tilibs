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
	Hexadecimal logging.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <glib.h>

#include "logging.h"
#include "data_log.h"
#include "log_hex.h"

static char *ofn = nullptr;
static FILE *logfile = nullptr;

int log_hex_start(void)
{
	int ret;

	ofn = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, G_DIR_SEPARATOR_S, HEX_FILE, NULL);

	logfile = fopen(ofn, "wt");
	if (logfile != nullptr)
	{
		fprintf(logfile, "TiCables-2 data logger\n");	// needed by log_dbus.c
		fprintf(logfile, "Version %s\n", ticables_version_get());
		fprintf(logfile, "\n");
		ret = 0;
	}
	else
	{
		ticables_critical("Unable to open %s for logging.\n", ofn);
		ret = 1;
	}

	return ret;
}

int log_hex_1(int dir, uint8_t data)
{
	static int array[20];
	static int i = 0;

	if (logfile == nullptr)
	{
		return 1;
	}

	array[i++] = data;
	fprintf(logfile, "%02X ", data);

	if ((i != 0) && !(i % 16))
	{
		fprintf(logfile, "| ");
		for (int j = 0; j < 16; j++) 
		{
			const int c = array[j];
			if ((c < 32) || (c > 127))
			{
				fprintf(logfile, " ");
			}
			else
			{
				fprintf(logfile, "%c", c);
			}
		}
		fprintf(logfile, "\n");
		i = 0;
	}

	return 0;
}

int log_hex_N(int dir, const uint8_t * data, uint32_t len)
{
	uint32_t i = 0;

	for (i = 0; i < len; i++)
	{
		log_hex_1(dir, data[i]);
	}

	return 0;
}

int log_hex_stop(void)
{
	if (logfile != nullptr)
	{
		fclose(logfile);
		logfile = nullptr;
	}

	g_free(ofn);
	ofn = nullptr;

	return 0;
}
