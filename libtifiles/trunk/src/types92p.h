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

#ifndef __TIFILES_DEFS92P__
#define __TIFILES_DEFS92P__

#include <stdint.h>

#define TI92p_MAXTYPES 48

#define TI92p_EXPR    0x00
#define TI92p_LIST    0x04
#define TI92p_MAT     0x06
#define TI92p_DATA    0x0A
#define TI92p_TEXT    0x0B
#define TI92p_STR     0x0C
#define TI92p_GDB     0x0D
#define TI92p_FIG     0x0E
#define TI92p_PIC     0x10
#define TI92p_PRGM    0x12
#define TI92p_FUNC    0x13
#define TI92p_MAC     0x14
#define TI92p_CLK     0x18       // clock
#define TI92p_RDIR    0x1A       // request
#define TI92p_LDIR    0x1B       // local
#define TI92p_OTH     0x1C
#define TI92p_ZIP     0x1C       // for backwards compatibility
#define TI92p_BKUP    0x1D
#define TI92p_FDIR    0x1F       // full
#define TI92p_DIR     TI92p_FDIR // type
#define TI92p_GETCERT 0x20
#define TI92p_ASM     0x21
#define TI92p_IDLIST  0x22
#define TI92p_AMS     0x23
#define TI92p_APPL    0x24
#define TI92p_CERTIF  0x25
#define TI92p_LICENSE 0x3E

#define TI92p_VNONE   0
#define TI92p_VLOCK   1
#define TI92p_VARCH   3

#endif
