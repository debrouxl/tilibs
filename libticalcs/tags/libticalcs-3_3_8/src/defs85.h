/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifndef DEFS85_H
#define DEFS85_H

// Devicetype

#define PC_TI85 0x05
#define TI85_PC 0x85

// Commands

#define CMD85_VAR_HEADER  0x06
#define CMD85_WAIT_DATA   0x09
#define CMD85_DATA_PART   0x15
#define CMD85_REFUSED     0x36
#define CMD85_REJECTED    0x36
#define CMD85_TI_OK       0x56
#define CMD85_PC_OK       0x56
#define CMD85_CHK_ERROR   0x5A
#define CMD85_SCREEN_DUMP 0x6D
#define CMD85_CONTINUE    0x78
#define CMD85_EOT         0x92

// Rejection codes for the CMD85_REJECTED command
#define CMD85_REJ_NONE     0x00
#define CMD85_REJ_EXIT     0x01
#define CMD85_REJ_SKIP     0x02
#define CMD85_REJ_OUTOFMEM 0x03

// Types of variable

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
// Something here
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
// Something here
#define TI85_FUNC  0x17
#define TI85_POL   0x18
#define TI85_PARAM 0x19
#define TI85_DIFEQ 0x1A
#define TI85_ZRCL  0x1B
// Something here
#define TI85_BKUP  0x1D
#define TI85_UNKN  0x1E

// Screen coordinates of the TI85

#define TI85_ROWS  64
#define TI85_COLS  128

#endif
