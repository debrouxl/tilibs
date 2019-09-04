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

#ifndef __TIFILES_DEFS89__
#define __TIFILES_DEFS89__

#include <stdint.h>

#define TI89_MAXTYPES 48

#define TI89_EXPR     0x00
#define TI89_LIST     0x04
#define TI89_MAT      0x06
#define TI89_DATA     0x0A
#define TI89_TEXT     0x0B
#define TI89_STR      0x0C
#define TI89_GDB      0x0D
#define TI89_FIG      0x0E
#define TI89_PIC      0x10
#define TI89_PRGM     0x12
#define TI89_FUNC     0x13
#define TI89_MAC      0x14
#define TI89_CLK      0x18       // clock
#define TI89_RDIR     0x1A       // request
#define TI89_LDIR     0x1B       // local
#define TI89_OTH      0x1C
#define TI89_ZIP      0x1C       // for backwards compatibility
#define TI89_BKUP     0x1D
#define TI89_FDIR     0x1F       // full
#define TI89_DIR      TI89_FDIR  // type
#define TI89_GETCERT  0x20
#define TI89_ASM      0x21
#define TI89_IDLIST   0x22
#define TI89_AMS      0x23
#define TI89_APPL     0x24
#define TI89_CERTIF   0x25
#define TI89_LICENSE  0x3E

#define TI89_VNONE    0
#define TI89_VLOCK    1
#define TI89_VARCH    3

#endif
