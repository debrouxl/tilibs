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

#ifndef __TIFILES_MEMREADER__
#define __TIFILES_MEMREADER__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "export2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ordinary header detection only needs a small prefix of the file. Flash OS/app
   detection walks a multi-entry container, but no released TI-Z80/68k/eZ80 model
   offers more than 4 MB of Flash, so capping the read there is always enough. */
#ifndef TIFILES_HEAD_READ_CAP
#define TIFILES_HEAD_READ_CAP 4096
#endif
#ifndef TIFILES_FLASH_READ_CAP
#define TIFILES_FLASH_READ_CAP (4 * 1024 * 1024)
#endif

/*
   Bounds-checked reader over an in-memory buffer (pointer + size), providing an
   fread()/fseek()-like API used by the tifiles_buffer_* detection helpers. This
    makes it possible to classify files loaded from a file or fuzzed buffers
    without touching the filesystem, while keeping every read within the bounds
   of the supplied buffer.

   A reader wraps either a caller-owned buffer (built with tifiles_mem_reader_init)
   or a buffer it allocated itself from a file (tifiles_mem_reader_create_from_file);
   in the latter case tifiles_mem_reader_destroy() releases it. Callers thus never
   have to manage the allocation performed by the file-loading path.
 */
typedef struct
{
	const uint8_t *data;   /* buffer being read (non-owned unless owned != NULL) */
	size_t        size;   /* total size of the buffer */
	size_t        pos;    /* current read offset */
	uint8_t      *owned;  /* non-NULL iff the reader allocated 'data' and must free it */
} TiMemReader;

/*
   Initialize a reader over a caller-owned buffer. The buffer must outlive the
   reader; tifiles_mem_reader_destroy() is then a no-op for freeing.
 */
static inline void tifiles_mem_reader_init(TiMemReader *reader, const uint8_t *data, size_t size)
{
	if (reader != NULL)
	{
		reader->data  = data;
		reader->size  = size;
		reader->pos   = 0;
		reader->owned = NULL;
	}
}

/*
   Read up to 'n' bytes into 'out', advancing the position. Returns the number
   of bytes actually copied (0 at end of buffer). Never reads past 'size'.
 */
static inline size_t tifiles_mem_reader_read(TiMemReader *reader, void *out, size_t n)
{
	size_t avail;

	if (reader == NULL)
	{
		return 0;
	}
	if (reader->pos > reader->size)
	{
		return 0;
	}

	avail = reader->size - reader->pos;
	if (n > avail)
	{
		n = avail;
	}
	if (n > 0 && out != NULL)
	{
		memcpy(out, reader->data + reader->pos, n);
	}
	reader->pos += n;

	return n;
}

/*
   Skip 'n' bytes forward (bounds-checked). Returns 0 on success, -1 if not
   enough bytes remain (position is left at the end of the buffer in that case).
 */
static inline int tifiles_mem_reader_skip(TiMemReader *reader, size_t n)
{
	if (reader == NULL)
	{
		return -1;
	}
	if (n > (reader->size - reader->pos))
	{
		reader->pos = reader->size;
		return -1;
	}
	reader->pos += n;

	return 0;
}

/*
   Seek to an absolute offset. Returns 0 on success, -1 if out of range.
 */
static inline int tifiles_mem_reader_seek(TiMemReader *reader, size_t off)
{
	if (reader == NULL || off > reader->size)
	{
		return -1;
	}
	reader->pos = off;

	return 0;
}

/* Open a file and build a reader over (at most) the first @p min_size bytes of
   its contents; a @p min_size of 0 reads the whole file. The buffer is owned by
   the reader and freed by tifiles_mem_reader_destroy(). Returns 0 on success,
   -1 on any failure (missing/unreadable/empty file). */
TIEXPORT2 int TICALL tifiles_mem_reader_create_from_file(TiMemReader *reader, const char *filename, size_t min_size);

/* Free the buffer owned by a reader (if any) and reset it. Safe to call on a
   non-owning or NULL reader. */
TIEXPORT2 void TICALL tifiles_mem_reader_destroy(TiMemReader *reader);

#ifdef __cplusplus
}
#endif

#endif
