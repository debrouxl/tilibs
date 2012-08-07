/* Hey EMACS -*- linux-c -*- */
/* $Id: typesv2.h 3056 2006-11-06 17:12:50Z roms $ */

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

#ifndef __TIFILES_DEFS_NSP__
#define __TIFILES_DEFS_NSP__

#include "stdints2.h"

#define NSP_MAXTYPES 2

#define NSP_TNS		0x00
#define NSP_DIR		0x01

// libtifiles: for internal use only, not exported !

extern const char *NSP_CONST[NSP_MAXTYPES + 1][4];

const char *nsp_byte2type(uint8_t data);
uint8_t		nsp_type2byte(const char *s);
const char *nsp_byte2fext(uint8_t data);
uint8_t		nsp_fext2byte(const char *s);

const char *nsp_byte2desc(uint8_t data);
const char *nsp_byte2icon(uint8_t data);

#endif
