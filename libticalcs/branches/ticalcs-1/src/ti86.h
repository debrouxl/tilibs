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

#ifndef __TICALCS__TI86__
#define __TICALCS__TI86__

#include <stdio.h>
#include "headers.h"

int ti86_supported_operations(void);

int ti86_isready(void);

int ti86_send_key(uint16_t key);

int ti86_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc);

int ti86_directorylist(TNode ** tree, uint32_t * memory);

int ti86_send_backup(const char *filename, int mask_mode);
int ti86_recv_backup(const char *filename, int mask_mode);

int ti86_send_var(const char *filename, int mask_mode, char **actions);
int ti86_recv_var(char *filename, int mask_mode, TiVarEntry * ve);

int ti86_send_flash(const char *filename, int mask_mode);
int ti86_recv_flash(const char *filename, int mask_mode, TiVarEntry * ve);
int ti86_get_idlist(char *idlist);

int ti86_dump_rom(const char *filename, int mask_mode);

int ti86_recv_var_2(char *filename, int mask_mode, TiVarEntry * ve);

#endif
