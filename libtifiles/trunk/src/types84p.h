/* Hey EMACS -*- linux-c -*- */
/* $Id: types84p.h 368 2004-03-22 18:42:08Z roms $ */

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

#ifndef __TIFILES_DEFS84p__
#define __TIFILES_DEFS84p__

#include "types83p.h"

#define TI84p_MAXTYPES 48
#define TI84pc_MAXTYPES 48
#define TI83pce_MAXTYPES 48
#define TI84pce_MAXTYPES 48
#define TI82a_MAXTYPES 48
#define TI84pt_MAXTYPES 48
#define TI82aep_MAXTYPES 48

#define TI84p_REAL   0x00
#define TI84p_LIST   0x01
#define TI84p_MATRX	 0x01	//deprecated
#define TI84p_MAT    0x02
#define TI84p_EQU    0x03
#define TI84p_STRNG	 0x03	//deprecated
#define TI84p_STR    0x04
#define TI84p_PRGM   0x05
#define TI84p_ASM    0x06
#define TI84p_PIC    0x07
#define TI84p_GDB    0x08
#define TI84p_CPLX   0x0C
#define TI84p_WDW	 0x0F	//deprecated
#define TI84p_WINDW  0x0F
#define TI84p_ZSTO   0x10
#define TI84p_TAB	 0x11	//deprecated
#define TI84p_TABLE  0x11
#define TI84p_BKUP   0x13
#define TI84p_APPOBJ 0x14
#define TI84p_APPVAR 0x15	//deprecated
#define TI84p_APPV   0x15
#define TI84p_TEMPPROGOBJ 0x16
#define TI84p_GROUP  0x17
#define TI83pce_SIMPLEFRAC 0x18
#define TI84p_DIR    0x19
#define TI84p_IMAGE  0x1A
#define TI83pce_CPLXSIMPLEFRAC 0x1B
#define TI83pce_RADICAL 0x1C
#define TI83pce_CPLXRADICAL 0x1D
#define TI83pce_CPLXPI 0x1E
#define TI83pce_CPLXPISIMPLEFRAC 0x1F
#define TI83pce_PI 0x20
#define TI83pce_PISIMPLEFRAC 0x21
#define TI84p_AMS    0x23
#define TI84p_APPL   0x24
#define TI84p_CERTIF 0x25	//deprecated
#define TI84p_CERT	 0x25
#define TI84p_IDLIST 0x26
#define TI84p_GETCERT 0x27
#define TI84p_CLK    0x29
#define TI84p_LICENSE	0x3e

#endif
