/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2000, Romain Lievin, David Kuder
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
~ *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef DEFS86_H
#define DEFS86_H

// Devicetype

#define PC_TI86 0x06
#define TI86_PC 0x86

// Commands

#define CMD86_VAR_HEADER  0x06 // 0x06 or 0xC9
#define CMD86_WAIT_DATA   0x09
#define CMD86_DATA_PART   0x15
#define CMD86_REFUSED     0x36
#define CMD86_REJECTED    0x36
#define CMD86_TI_OK       0x56
#define CMD86_PC_OK       0x56
#define CMD86_CHK_ERROR   0x5A
#define CMD86_SCREEN_DUMP 0x6D
#define CMD86_EOT         0x92
#define CMD86_REQUEST     0xA2 // TI's software uses 0x22, Others say 0xA2...
#define CMD86_VAR_HEADER2 0xC9

// Rejection codes for the CMD86_REJECTED command
#define CMD86_REJ_NONE     0x00
#define CMD86_REJ_EXIT     0x01
#define CMD86_REJ_SKIP     0x02
#define CMD86_REJ_OUTOFMEM 0x03

// Types of variable

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
// Something here
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
// Something here
#define TI86_FUNC  0x17
#define TI86_POL   0x18
#define TI86_PARAM 0x19
#define TI86_DIFEQ 0x1A
#define TI86_ZRCL  0x1B
// Something here
#define TI86_BKUP  0x1D
#define TI86_UNKN  0x1E
	
// Screen coordinates of the TI86

#define TI86_ROWS  64
#define TI86_COLS  128

#endif
