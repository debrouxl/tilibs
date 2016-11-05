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

#ifndef __TIFILES_DEFS82__
#define __TIFILES_DEFS82__

#include <stdint.h>

#define TI82_MAXTYPES  48

#define TI82_REAL 0x00
#define TI82_LIST 0x01
#define TI82_MAT  0x02
#define TI82_YVAR 0x03
#define TI82_PRGM 0x05
#define TI82_PPGM 0x06
#define TI82_PIC  0x07
#define TI82_GDB  0x08
#define TI82_WDW  0x0B
#define TI82_ZSTO 0x0C
#define TI82_TAB  0x0D
#define TI82_LCD  0x0E
#define TI82_BKUP 0x0F

#endif
