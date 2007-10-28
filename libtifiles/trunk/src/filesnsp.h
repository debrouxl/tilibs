/* Hey EMACS -*- linux-c -*- */
/* $Id: files9x.h 3420 2007-06-13 20:41:37Z roms $ */

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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TIFILES_FILES_NSP_H__
#define __TIFILES_FILES_NSP_H__

#include "stdints2.h"
#include "tifiles.h"

/* Functions */

// allocating
FileContent* tnsp_content_create_regular(void);

// freeing
void tnsp_content_free_regular(FileContent *content);

// displaying
int tnsp_content_display_regular(FileContent *content);

// reading
int tnsp_file_read_regular(const char *filename, FileContent *content);

// writing
int tnsp_file_write_regular(const char *filename, FileContent *content, char **filename2);

// displaying
int tnsp_file_display(const char *filename);

#endif
