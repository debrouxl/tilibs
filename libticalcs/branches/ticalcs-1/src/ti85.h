/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#ifndef __TICALCS__TI85__
#define __TICALCS__TI85__

#include <stdio.h>
#include "headers.h"

int ti85_supported_operations(void);

int ti85_isready(void);

int ti85_send_key(uint16_t key);

int ti85_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc);

int ti85_directorylist(TNode ** tree, uint32_t * memory);

int ti85_send_backup(const char *filename, int mask_mode);
int ti85_recv_backup(const char *filename, int mask_mode);

int ti85_send_var(const char *filename, int mask_mode, char **actions);
int ti85_recv_var(char *filename, int mask_mode, TiVarEntry * ve);

int ti85_send_flash(const char *filename, int mask_mode);
int ti85_recv_flash(const char *filename, int mask_mode, TiVarEntry * ve);
int ti85_get_idlist(char *idlist);

int ti85_dump_rom(const char *filename, int mask_mode);

int ti85_recv_var_2(char *filename, int mask_mode, TiVarEntry * ve);

#endif
