/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifndef TI89_H
#define TI89_H

#include <stdio.h>
#include "calc_ext.h"

const char *ti89_byte2type(byte data);
byte ti89_type2byte(char *s);
const char *ti89_byte2fext(byte data);
byte ti89_fext2byte(char *s);

int ti89_isready(void);
int ti89_send_key(word key);
int ti89_remote_control(void);
int ti89_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc);
int ti89_directorylist(struct varinfo *list, int *n_elts);
int ti89_recv_backup(FILE *file, int mask_mode, longword *version);
int ti89_send_backup(FILE *file, int mask_mode);
int ti89_recv_var(FILE *file, int mask_mode, 
		      char *varname, byte vartype, byte varlock);
int ti89_send_var(FILE *file, int mask_mode);
int ti89_send_flash(FILE *file, int mask_mode);
int ti89_recv_flash(FILE *file, int mask_mode, char *appname, int appsize);
int ti89_dump_rom(FILE *file, int mask_mode);
int ti89_get_rom_version(char *version);
int ti89_get_idlist(char *idlist);

#endif




