/*  libtifiles - TI File Format library
 *  Copyright (C) 2002-2003  Romain Lievin
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TIFILES_DEFS83__
#define __TIFILES_DEFS83__

#include "stdints.h"

#define TI83_MAXTYPES 48

#define TI83_REAL  0x00
#define TI83_LIST  0x01
#define TI83_MATRX 0x02
#define TI83_EQU   0x03
#define TI83_STRNG 0x04
#define TI83_PRGM  0x05
#define TI83_PIC   0x07
#define TI83_GDB   0x08
#define TI83_CPLX  0x0C
#define TI83_BKUP  0x13
#define TI83_DIR   0x19

// libtifiles: for internal use only, not exported !

extern const char *TI83_CONST[TI83_MAXTYPES + 1][4];

const char *ti83_byte2type(uint8_t data);
uint8_t ti83_type2byte(const char *s);
const char *ti83_byte2fext(uint8_t data);
uint8_t ti83_fext2byte(const char *s);

const char *ti83_byte2desc(uint8_t data);
const char *ti83_byte2icon(uint8_t data);

#endif
