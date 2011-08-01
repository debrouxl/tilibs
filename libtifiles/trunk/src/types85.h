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

#ifndef __TIFILES_DEFS85__
#define __TIFILES_DEFS85__

#include "stdints2.h"

#define TI85_MAXTYPES 48

#define TI85_REAL  0x00
#define TI85_CPLX  0x01
#define TI85_VECTR 0x02
#define TI85_CVECT 0x03
#define TI85_LIST  0x04
#define TI85_CLIST 0x05
#define TI85_MATRX 0x06
#define TI85_CMATR 0x07
#define TI85_CONS  0x08
#define TI85_CCONS 0x09
#define TI85_EQU   0x0A
#define TI85_STRNG 0x0C
#define TI85_GDB   0x0D
#define TI85_OGDB  0x0E
#define TI85_AGDB  0x0F
#define TI85_DGDB  0x10
#define TI85_PICT  0x11
#define TI85_PRGM  0x12
#define TI85_RANGE 0x13
#define TI85_SCRN  0x14
#define TI85_DIR   0x15
#define TI85_FUNC  0x17
#define TI85_POL   0x18
#define TI85_PARAM 0x19
#define TI85_DIFEQ 0x1A
#define TI85_ZRCL  0x1B
#define TI85_BKUP  0x1D
#define TI85_UNKN  0x1E

// libtifiles: for internal use only, not exported !

extern const char *TI85_CONST[TI85_MAXTYPES + 1][4];

const char *ti85_byte2type(uint8_t data);
uint8_t ti85_type2byte(const char *s);
const char *ti85_byte2fext(uint8_t data);
uint8_t ti85_fext2byte(const char *s);

const char *ti85_byte2desc(uint8_t data);
const char *ti85_byte2icon(uint8_t data);

#endif
