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

#ifndef TI92P_H
#define TI92P_H

#include <stdio.h>
#include "calc_ext.h"
#include "calc_def.h"

const char *ti92p_byte2type(byte data);
byte ti92p_type2byte(char *s);
const char *ti92p_byte2fext(byte data);
byte ti92p_fext2byte(char *s);

int ti92p_isready(void);
int ti92p_send_key(word key);
int ti92p_remote_control(void);
int ti92p_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc);
int ti92p_directorylist(struct varinfo *list, int *n_elts);
int ti92p_recv_backup(FILE *file, int mask_mode, longword *version);
int ti92p_send_backup(FILE *file, int mask_mode);
int ti92p_recv_var(FILE *file, int mask_mode, 
		      char *varname, byte vartype, byte varlock);
int ti92p_send_var(FILE *file, int mask_mode);
int ti92p_send_flash(FILE *file, int mask_mode);
int ti92p_recv_flash(FILE *file, int mask_mode, char *appname, int appsize);
int ti92p_dump_rom(FILE *file, int mask_mode);
int ti92p_get_rom_version(char *version);
int ti92p_get_idlist(char *idlist);

int ti92p_supported_operations(void);

#endif




