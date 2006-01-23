/* Hey EMACS -*- linux-c -*- */
/* $Id: grouped.c 1737 2006-01-23 12:54:47Z roms $ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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
	TiGroup (*.tig) management
	A TiGroup file is in fact a ZIP archive with no compression (stored).
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_ZLIB
//#include <zlib.h>
#include "minizip/zip.h"
#include "minizip/unzip.h"
#endif

#include "tifiles.h"

TIEXPORT int TICALL tifiles_file_read_tigroup(const char *filename, FileContent *content)
{
	return 0;
}

TIEXPORT int TICALL tifiles_file_write_tigroup(const char *filename, FileContent *content)
{
	return 0;
}