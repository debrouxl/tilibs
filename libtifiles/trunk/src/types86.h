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

#ifndef __TIFILES_DEFS86__
#define __TIFILES_DEFS86__

#include <stdint.h>

#define TI86_MAXTYPES 48

#define TI86_REAL  0x00
#define TI86_CPLX  0x01
#define TI86_VECTR 0x02
#define TI86_CVECT 0x03
#define TI86_LIST  0x04
#define TI86_CLIST 0x05
#define TI86_MATRX 0x06
#define TI86_CMATR 0x07
#define TI86_CONS  0x08
#define TI86_CCONS 0x09
#define TI86_EQU   0x0A
#define TI86_STRNG 0x0C
#define TI86_GDB   0x0D
#define TI86_OGDB  0x0E
#define TI86_AGDB  0x0F
#define TI86_DGDB  0x10
#define TI86_PICT  0x11
#define TI86_PRGM  0x12
#define TI86_RANGE 0x13
#define TI86_SCRN  0x14
#define TI86_DIR   0x15
#define TI86_FUNC  0x17
#define TI86_POL   0x18
#define TI86_PARAM 0x19
#define TI86_DIFEQ 0x1A
#define TI86_ZRCL  0x1B
#define TI86_BKUP  0x1D
#define TI86_UNKN  0x1E

#endif
