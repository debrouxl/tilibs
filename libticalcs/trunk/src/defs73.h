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

#ifndef DEFS73_H
#define DEFS73_H

// Devicetype

#define PC_TI73 0x23
#define TI73_PC 0x73

// Commands

#define CMD73_VAR_HEADER  0x06 // 0x06 or 0xC9
#define CMD73_WAIT_DATA   0x09
#define CMD73_DATA_PART   0x15
#define CMD73_SEND_FLASH  0x2D
#define CMD73_REFUSED     0x36
#define CMD73_REJECTED    0x36
#define CMD73_TI_OK       0x56
#define CMD73_PC_OK       0x56
#define CMD73_CHK_ERROR   0x5A
#define CMD73_ISREADY     0x68
#define CMD73_SCREEN_DUMP 0x6D
#define CMD73_CONTINUE    0x78
#define CMD73_DIRECT_CMD  0x87
#define CMD73_EOT         0x92
#define CMD73_REQUEST     0xA2
#define CMD73_VAR_HEADER2 0xC9

// Rejection codes for the CMD73_REJECTED command
#define CMD73_REJ_NONE     0x00
#define CMD73_REJ_EXIT     0x01
#define CMD73_REJ_SKIP     0x02
#define CMD73_REJ_OUTOFMEM 0x03

// Types of variable

#define TI73_REAL   0x00
#define TI73_LIST   0x01
#define TI73_MATRX  0x02
#define TI73_EQU    0x03
#define TI73_STRNG  0x04
#define TI73_PRGM   0x05
#define TI73_ASM    0x06
#define TI73_PIC    0x07
#define TI73_GDB    0x08
#define TI73_CPLX   0x0C
#define TI73_WDW    0x0F
#define TI73_ZSTO   0x10
#define TI73_TAB    0x11

#define TI73_BKUP   0x13
#define TI73_DIR    0x19
#define TI73_AMS    0x23
#define TI73_FLASH  0x24
#define TI73_IDLIST 0x26

// Screen coordinates of the TI83+

#define TI73_ROWS  64
#define TI73_COLS  96

#endif
