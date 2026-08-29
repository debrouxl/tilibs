/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
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

#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>

#include "tifiles.h"
#include "memreader.h"

/*
   Open a file and build a reader over (at most) the first @p min_size bytes of
   its contents. A @p min_size of 0 means the whole file is read. Callers that
   only need to recognise a file format read a small prefix (TIFILES_HEAD_READ_CAP)
   and never copy megabytes of FLASH OS/app data around; the few callers that
   must walk a multi-entry FLASH container pass TIFILES_FLASH_READ_CAP (no
   released TI-Z80/68k/eZ80 model has more than 4 MB of Flash).

   The buffer is owned by the reader: tifiles_mem_reader_destroy() is the only
   place that releases it, so the allocation never leaks into callers. On any
   failure the reader is left untouched and -1 is returned, so callers can treat
   a missing/unreadable/empty file exactly like the previous fopen()-based code
   path (which reported "not a TI file").
 */
int TICALL tifiles_mem_reader_create_from_file(TiMemReader *reader, const char *filename, size_t min_size)
{
	int        fd;
	struct stat st;
	uint8_t   *data = nullptr;
	size_t     to_read;
	uint8_t   *p;
	size_t     remaining;

	if (reader == nullptr || filename == nullptr)
	{
		return -1;
	}
	tifiles_mem_reader_init(reader, nullptr, 0);

#if defined(_WIN32)
	fd = g_open(filename, O_RDONLY | O_BINARY);
#else
	fd = open(filename, O_RDONLY);
#endif
	if (fd < 0)
	{
		return -1;
	}
	if (fstat(fd, &st) != 0)
	{
		close(fd);
		return -1;
	}
	if (st.st_size <= 0)
	{
		/* Nothing to classify. */
		close(fd);
		return -1;
	}

	to_read = (size_t)st.st_size;
	if (min_size != 0 && min_size < to_read)
	{
		to_read = min_size;
	}

	data = (uint8_t *)g_malloc(to_read);
	if (data == nullptr)
	{
		close(fd);
		return -1;
	}

	p = data;
	remaining = to_read;
	while (remaining > 0)
	{
		ssize_t n = read(fd, p, remaining);
		if (n < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			break;
		}
		if (n == 0)
		{
			/* Unexpected end of file. */
			break;
		}
		p += (size_t)n;
		remaining -= (size_t)n;
	}
	close(fd);

	if (remaining != 0)
	{
		g_free(data);
		return -1;
	}

	reader->data  = data;
	reader->size  = to_read;
	reader->pos   = 0;
	reader->owned = data;
	return 0;
}

void TICALL tifiles_mem_reader_destroy(TiMemReader *reader)
{
	if (reader != nullptr)
	{
		if (reader->owned != nullptr)
		{
			g_free(reader->owned);
		}
		reader->data  = nullptr;
		reader->size  = 0;
		reader->pos   = 0;
		reader->owned = nullptr;
	}
}
