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

#ifndef __TIFILES_DEFSV2__
#define __TIFILES_DEFSV2__

#include "stdints2.h"

#define V200_MAXTYPES 48

#define V200_EXPR 0x00
#define V200_LIST 0x04
#define V200_MAT  0x06
#define V200_DATA 0x0A
#define V200_TEXT 0x0B
#define V200_STR  0x0C
#define V200_GDB  0x0D
#define V200_FIG  0x0E
#define V200_PIC  0x10
#define V200_PRGM 0x12
#define V200_FUNC 0x13
#define V200_MAC  0x14
#define V200_CLK  0x18
#define V200_RDIR 0x1A		// request
#define V200_LDIR 0x1B		// local
#define V200_ZIP  0x1C
#define V200_BKUP 0x1D
#define V200_FDIR 0x1F		// full
#define V200_DIR  V200_FDIR	//type
#define V200_GETCERT 0x20
#define V200_ASM     0x21
#define V200_IDLIST  0x22
#define V200_AMS     0x23
#define V200_APPL    0x24
#define V200_CERTIF  0x25
#define V200_LICENSE 0x3E

#define V200_VNONE 0
#define V200_VLOCK 1
#define V200_VARCH 3

// libtifiles: for internal use only, not exported !

extern const char *V200_CONST[V200_MAXTYPES + 1][4];

const char *v200_byte2type(uint8_t data);
uint8_t v200_type2byte(const char *s);
const char *v200_byte2fext(uint8_t data);
uint8_t v200_fext2byte(const char *s);

const char *v200_byte2desc(uint8_t data);
const char *v200_byte2icon(uint8_t data);

#endif
