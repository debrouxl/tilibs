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

#ifndef DEFS89_H
#define DEFS89_H

// Devicetype

#define PC_TI89 0x08
#define TI89_PC 0x98

// Command

#define CMD89_VAR_HEADER  0x06
#define CMD89_WAIT_DATA   0x09
#define CMD89_SEND        0x0D
#define CMD89_DATA_PART   0x15
#define CMD89_REFUSED     0x36
#define CMD89_REJECTED    0x36
#define CMD89_TI_OK       0x56
#define CMD89_PC_OK       0x56
#define CMD89_CHK_ERROR   0x5A
#define CMD89_ISREADY     0x68
#define CMD89_SCREEN_DUMP 0x6D
#define CMD89_CONTINUE    0x78
#define CMD89_DIRECT_CMD  0x87
#define CMD89_EOT         0x92
#define CMD89_REQUEST     0xA2
#define CMD89_VAR_HEADER2 0xC9 //extended var header

// Types of variable

#define TI89_EXPR 0x00
#define TI89_LIST 0x04
#define TI89_MAT  0x06
#define TI89_DATA 0x0A
#define TI89_TEXT 0x0B
#define TI89_STR  0x0C
#define TI89_GDB  0x0D
#define TI89_FIG  0x0E
#define TI89_PIC  0x10
#define TI89_PRGM 0x12
#define TI89_FUNC 0x13
#define TI89_MAC  0x14
#define TI89_DIRL 0x1A
#define TI89_LDIR 0x1B
#define TI89_ZIP  0x1C
#define TI89_BKUP 0x1D
#define TI89_DIR  0x1F
#define TI89_ASM  0x21
#define TI89_IDLIST 0x22
#define TI89_AMS    0x23
#define TI89_FLASH  0x24

// Locking variables

#define TI89_VNONE 0
#define TI89_VLOCK 1
#define TI89_VARCH 3

// Keys of the TI89

#define KEY89_F1     268
#define KEY89_F2     269
#define KEY89_F3     270
#define KEY89_F4     271
#define KEY89_F5     272
#define KEY89_F6     273
#define KEY89_F7     274
#define KEY89_F8     275
#define KEY89_ESC    264
#define KEY89_APPS   265
#define KEY89_HOME   277
#define KEY89_MODE   266
#define KEY89_CATLG  278
#define KEY89_BS	 257
#define KEY89_CLEAR  263
#define KEY89_X      120
#define KEY89_Y      121
#define KEY89_Z      122
#define KEY89_T      116
#define KEY89_POWER  94
#define KEY89_TUBE	 124
#define KEY89_LP     40  // Left parenthesis
#define KEY89_RP     41  // Right parenthesis
#define KEY89_COMMA  44
#define KEY89_DIVIDE 47
#define KEY89_MULT   42
#define KEY89_MINUS  45
#define KEY89_PLUS   43
#define KEY89_CR     13
#define KEY89_ENTER  13
#define KEY89_STO    258
#define KEY89_EQUALS 61
#define KEY89_EE     149
#define KEY89_NEG    173
#define KEY89_DOT    46
#define KEY89_0      48
#define KEY89_1      49
#define KEY89_2      50
#define KEY89_3      51
#define KEY89_4      52
#define KEY89_5      53
#define KEY89_6      54
#define KEY89_7      55
#define KEY89_8      56
#define KEY89_9      57

#define KEY89_UP     338
#define KEY89_RIGHT  340
#define KEY89_DOWN   344
#define KEY89_LEFT   337

#define KEY89_2ND    4096
#define KEY89_CTRL   8192
#define KEY89_SHIFT  16384
#define KEY89_ALPHA  32768

// Screen coordinates of the TI89

#define TI89_ROWS          128
#define TI89_COLS          240
#define TI89_ROWS_VISIBLE  100
#define TI89_COLS_VISIBLE  160

// FLASH support
#include "calc_def.h"

struct ti89_flash_header
{
  byte id[8];
  word revision;
  byte flags;
  byte object_type;
  byte revision_day;
  byte revision_month;
  word revision_year;
  byte name_length;
  byte name[8];
  byte filler[23];
  byte device_type;
  byte data_type;
  byte filler2[24];
  byte data_length[4]; //longword removed: problem of data alignment ?!
};
typedef struct ti89_flash_header Ti89FlashHeader;

#define DEVICE_TYPE_89 0x98
#define DEVICE_TYPE_92 0x88

#endif
