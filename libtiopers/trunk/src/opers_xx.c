/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 2013-2016  Lionel Debroux
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gettext.h"
#include "tiopers.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

/**
 * tiopers_calc_recv_idlist:
 * @handle: a previously allocated handle
 * @idlist: buffer (at least 32 bytes wide) where to store ID-LIST
 * @printable_version: return area for a dynamically allocated (g_malloc) printable version of the ID list.
 * @filename: name of the file to where to store ID-LIST, if any
 *
 * Request ID-LIST of hand-held, optionally making a printable version available, and optionally storing the printable version to a file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT5 int TICALL tiopers_recv_idlist(OperHandle * handle, uint8_t * idlist, char ** printable_version, const char * filename)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(idlist);

	ret = ticalcs_calc_recv_idlist(handle->calc, idlist);
	if (!ret)
	{
		char * buffer;
		char buf[32 + 2];

		if (handle->options.calc_model != CALC_NSPIRE)
		{
			strncpy(buf, (char *)idlist, 5);
			buf[5] = '-';
			strncat(buf, (char *)idlist + 5, 5);
			buf[11] = '-';
			strncat(buf, (char *)idlist + 5 + 5, 4);
		}
		else
		{
			strncpy(buf, (char *)idlist, 32);
			buf[32] = 0;
		}
		buffer = g_strconcat(_("ID-LIST : "), buf, NULL);

		if (filename != NULL)
		{
			FILE *f;

			f = fopen(filename, "wt");

			if (f != NULL)
			{
				if (fwrite(buffer, strlen(buffer), 1, f) < 1)
				{
					ret = ERR_SAVE_FILE;
				}
				fclose(f);
			}
			else
			{
				ret = ERR_SAVE_FILE;
			}
		}

		if (printable_version != NULL)
		{
			*printable_version = buffer;
		}
		else
		{
			g_free(buffer);
		}
	}

	return ret;
}

/**
 * tiopers_dump_rom:
 * @handle: a previously allocated handle
 * @size: optional size of dump
 * @filename: where to store the dump
 *
 * Handle dumper transfer + dumping, if possible.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT5 int TICALL tiopers_dump_rom(OperHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	ret = ticalcs_calc_dump_rom_1(handle->calc);
	if (!ret)
	{
		ret = ticalcs_calc_dump_rom_2(handle->calc, size, filename);
	}

	return ret;
}

/**
 * tiopers_get_info:
 * @handle: a previously allocated handle
 * @infos: pointer to structure where information will be stored.
 * @str: storage area for printable version of the calculator information, can be NULL.
 * @maxlen: maximum number of bytes written to the printable_version area.
 *
 * Request version info and capabilities.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT5 int TICALL tiopers_get_infos(OperHandle* handle, CalcInfos *infos, char * str, uint32_t maxlen)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(infos);

	if (!(ticalcs_calc_features(handle->calc) & OPS_VERSION))
	{
		return ERR_UNSUPPORTED;
	}

	ret = ticalcs_calc_get_version(handle->calc, infos);
	if (!ret)
	{
		ret = ticalcs_infos_to_string(infos, str, maxlen);
	}

	return ret;
}

/**
 * tiopers_format_bytes:
 * @value: the value to be formatted.
 * @buffer: output buffer, at least 32 bytes wide.
 *
 * Format value depending on its range to output buffer.
 **/
TIEXPORT5 void TICALL tiopers_format_bytes(unsigned long value, char * buffer)
{
	if (buffer == NULL)
	{
		tiopers_critical("%s: buffer is NULL", __FUNCTION__);
		return;
	}

	if (value < 64UL*1024)
	{
		g_snprintf(buffer, 32, _("%lu bytes"), value);
	}
	else if (value < 1024UL*1024)
	{
		g_snprintf(buffer, 32, _("%lu KB"), value >> 10);
	}
	else
	{
		g_snprintf(buffer, 32, _("%lu MB"), value >> 20);
	}
}
