/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef DEFS83_H
#define DEFS83_H

// Devicetype

#define PC_TI83 0x03
#define TI83_PC 0x83

// Commands

#define CMD83_VAR_HEADER  0x06 // 0x06 or 0xC9
#define CMD83_WAIT_DATA   0x09
#define CMD83_DATA_PART   0x15
#define CMD83_TI_OK       0x56
#define CMD83_PC_OK       0x56
#define CMD83_CHK_ERROR   0x5A
#define CMD83_ISREADY     0x68
#define CMD83_SCREEN_DUMP 0x6D
#define CMD83_EOT         0x92
#define CMD83_REQUEST     0xA2
#define CMD83_VAR_HEADER2 0xC9

// Types of variable

#define TI83_REAL  0x00
#define TI83_LIST  0x01
#define TI83_MATRX 0x02
#define TI83_EQU   0x03
#define TI83_STRNG 0x04
#define TI83_PRGM  0x05
#define TI83_PIC   0x07
#define TI83_GDB   0x08
#define TI83_CPLX  0x0C

#define TI83_BKUP 0x13
#define TI83_DIR  0x19

// Screen coordinates of the TI83

#define TI83_ROWS  64
#define TI83_COLS  96

#endif
