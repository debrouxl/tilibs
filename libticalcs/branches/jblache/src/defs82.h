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

#ifndef DEFS82_H
#define DEFS82_H

// Devicetype

#define PC_TI82 0x02
#define TI82_PC 0x82

// Commands

#define CMD82_VAR_HEADER  0x06
#define CMD82_WAIT_DATA   0x09
#define CMD82_DATA_PART   0x15
#define CMD82_REFUSED     0x36
#define CMD82_REJECTED    0x36
#define CMD82_TI_OK       0x56
#define CMD82_PC_OK       0x56
#define CMD82_CHK_ERROR   0x5A
#define CMD82_SCREEN_DUMP 0x6D
#define CMD82_CONTINUE    0x78
#define CMD82_EOT         0x92

// Rejection codes for the CMD82_REJECTED command
#define CMD82_REJ_NONE     0x00
#define CMD82_REJ_EXIT     0x01
#define CMD82_REJ_SKIP     0x02
#define CMD82_REJ_OUTOFMEM 0x03

// Types of variable

#define TI82_REAL 0x00
#define TI82_LIST 0x01
#define TI82_MAT  0x02
#define TI82_EQU  0x03
#define TI82_PRGM 0x05
#define TI82_PPGM 0x06
#define TI82_PIC  0x07
#define TI82_GDB  0x08
#define TI82_WDW  0x0B
#define TI82_ZSTO 0x0C
#define TI82_TAB  0x0D
#define TI82_LCD  0x0E
#define TI82_BKUP 0x0F

// Screen coordinates of the TI82

#define TI82_ROWS  64
#define TI82_COLS  96

#endif
