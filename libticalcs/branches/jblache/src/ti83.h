/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef TI83_H
#define TI83_H

#include <stdio.h>
#include "calc_ext.h"

const char *ti83_byte2type(byte data);
byte ti83_type2byte(char *s);
const char *ti83_byte2fext(byte data);
byte ti83_fext2byte(char *s);

int ti83_isready(void);
int ti83_send_key(int key);
int ti83_remote_control(void);
int ti83_screendump(byte **bitmap, int mask_mode,
		    struct screen_coord *sc);
int ti83_receive_backup(FILE *file, int mask_mode, longword *version);
int ti83_send_backup(FILE *file, int mask_mode);
int ti83_directorylist(struct varinfo *list, int *n_elts);
int ti83_receive_var(FILE *file, int mask_mode, 
		     char *varname, byte vartype, byte varlock);
int ti83_send_var(FILE *file, int mask_mode);
int ti83_dump_rom(FILE *file, int mask_mode);
int ti83_get_rom_version(char *version);
int ti83_send_flash(FILE *file, int mask_mode);

#endif
