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

#ifndef DEFS83P_H
#define DEFS83P_H

// Devicetype

#define PC_TI83  0x03
#define PC_TI83p 0x23
#define TI83p_PC 0x73
#define TI83_PC  0x83

// Commands

#define CMD83p_VAR_HEADER  0x06 // 0x06 or 0xC9
#define CMD83p_WAIT_DATA   0x09
#define CMD83p_DATA_PART   0x15
#define CMD83p_SEND_FLASH  0x2D
#define CMD83p_REFUSED     0x36
#define CMD83p_REJECTED    0x36
#define CMD83p_TI_OK       0x56
#define CMD83p_PC_OK       0x56
#define CMD83p_CHK_ERROR   0x5A
#define CMD83p_ISREADY     0x68
#define CMD83p_SCREEN_DUMP 0x6D
#define CMD83p_CONTINUE    0x78
#define CMD83p_DIRECT_CMD  0x87
#define CMD83p_EOT         0x92
#define CMD83p_REQUEST     0xA2
#define CMD83p_VAR_HEADER2 0xC9

// Rejection codes for the CMD83p_REJECTED command
#define CMD83p_REJ_NONE     0x00
#define CMD83p_REJ_EXIT     0x01
#define CMD83p_REJ_SKIP     0x02
#define CMD83p_REJ_OUTOFMEM 0x03

// Types of variable

#define TI83p_REAL   0x00
#define TI83p_LIST   0x01
#define TI83p_MATRX  0x02
#define TI83p_EQU    0x03
#define TI83p_STRNG  0x04
#define TI83p_PRGM   0x05
#define TI83p_ASM    0x06
#define TI83p_PIC    0x07
#define TI83p_GDB    0x08
#define TI83p_CPLX   0x0C
#define TI83p_WDW    0x0F
#define TI83p_ZSTO   0x10
#define TI83p_TAB    0x11
#define TI83p_BKUP   0x13
#define TI83p_APPVAR 0x15
#define TI83p_GROUP  0x17
#define TI83p_DIR    0x19
#define TI83p_AMS    0x23
#define TI83p_FLASH  0x24
#define TI83p_IDLIST 0x26

// Screen coordinates of the TI83+

#define TI83p_ROWS  64
#define TI83p_COLS  96

#endif
