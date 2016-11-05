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

#ifndef __TIFILES_DEFS73__
#define __TIFILES_DEFS73__

#include <stdint.h>

#define TI73_MAXTYPES 48

#define TI73_REAL   0x00
#define TI73_LIST   0x01
#define TI73_MATRX  0x02	//deprecated
#define TI73_MAT	0x02
#define TI73_EQU    0x03
#define TI73_STRNG  0x04	//deprecated
#define TI73_STR	0x04
#define TI73_PRGM   0x05
#define TI73_ASM    0x06
#define TI73_PIC    0x07
#define TI73_GDB    0x08
#define TI73_CPLX   0x0C
#define TI73_WDW    0x0F	//deprecated
#define TI73_WINDW	0x0F
#define TI73_ZSTO   0x10
#define TI73_TAB    0x11	//deprecated
#define TI73_TABLE	0x11
#define TI73_BKUP   0x13
#define TI73_DIR    0x19
#define TI73_AVAR	0x1A
#define TI73_AMS    0x23
#define TI73_APPL   0x24
#define TI73_CERT	 0x25
#define TI73_IDLIST 0x26
#define TI73_GETCERT	 0x27
#define TI73_CLK	0x29
#define TI73_LICENSE 0x3E

#endif
