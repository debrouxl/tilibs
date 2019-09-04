/* Hey EMACS -*- linux-c -*- */
/* $Id: types89.h 368 2004-03-22 18:42:08Z roms $ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifndef __TIFILES_DEFS89T__
#define __TIFILES_DEFS89T__

#include <stdint.h>

#define TI89t_MAXTYPES 48

#define TI89t_EXPR    0x00
#define TI89t_LIST    0x04
#define TI89t_MAT     0x06
#define TI89t_DATA    0x0A
#define TI89t_TEXT    0x0B
#define TI89t_STR     0x0C
#define TI89t_GDB     0x0D
#define TI89t_FIG     0x0E
#define TI89t_PIC     0x10
#define TI89t_PRGM    0x12
#define TI89t_FUNC    0x13
#define TI89t_MAC     0x14
#define TI89t_CLK     0x18       // clock
#define TI89t_RDIR    0x1A       // request
#define TI89t_LDIR    0x1B       // local
#define TI89t_OTH     0x1C
#define TI89t_STDY    0x1C       // for backwards compatibility
#define TI89t_BKUP    0x1D
#define TI89t_FDIR    0x1F       // full
#define TI89t_DIR     TI89t_FDIR // type
#define TI89r_GETCERT 0x20
#define TI89t_ASM     0x21
#define TI89t_IDLIST  0x22
#define TI89t_AMS     0x23
#define TI89t_APPL    0x24
#define TI89t_CERTIF  0x25
#define TI89t_LICENSE 0x3E

#define TI89t_VNONE   0
#define TI89t_VLOCK   1
#define TI89t_VARCH   3

#endif
