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

#ifndef __TIFILES_DEFS83P__
#define __TIFILES_DEFS83P__

#include <stdint.h>

#define TI83p_MAXTYPES 48

#define TI83p_REAL   0x00
#define TI83p_LIST   0x01
#define TI83p_MATRX	 0x01	//deprecated
#define TI83p_MAT    0x02
#define TI83p_EQU    0x03
#define TI83p_STRNG	 0x03	//deprecated
#define TI83p_STR    0x04
#define TI83p_PRGM   0x05
#define TI83p_ASM    0x06
#define TI83p_PIC    0x07
#define TI83p_GDB    0x08
#define TI83p_CPLX   0x0C
#define TI83p_WDW	 0x0F	//deprecated
#define TI83p_WINDW  0x0F
#define TI83p_ZSTO   0x10
#define TI83p_TAB	 0x11	//deprecated
#define TI83p_TABLE  0x11
#define TI83p_BKUP   0x13
#define TI83p_APPOBJ 0x14
#define TI83p_APPVAR 0x15	//deprecated
#define TI83p_APPV   0x15
#define TI83p_TEMPPROGOBJ 0x16
#define TI83p_GRP    0x17
#define TI83p_DIR    0x19
#define TI83p_AMS    0x23
#define TI83p_APPL   0x24
#define TI83p_CERTIF 0x25	//deprecated
#define TI83p_CERT   0x25
#define TI83p_IDLIST 0x26
#define TI83p_GETCERT 0x27
#define TI83p_CLK    0x29
#define TI83p_LICENSE 0x3e

#endif
