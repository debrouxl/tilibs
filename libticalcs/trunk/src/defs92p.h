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

#ifndef DEFS92P_H
#define DEFS92P_H

// Devicetype7

#define PC_TI92p 0x08
#define TI92p_PC 0x88

// Command

#define CMD92p_VAR_HEADER  0x06
#define CMD92p_WAIT_DATA   0x09
#define CMD92p_SEND        0x0D
#define CMD92p_DATA_PART   0x15
#define CMD92p_REFUSED     0x36
#define CMD92p_REJECTED    0x36
#define CMD92p_TI_OK       0x56
#define CMD92p_PC_OK       0x56
#define CMD92p_CHK_ERROR   0x5A
#define CMD92p_ISREADY     0x68
#define CMD92p_SCREEN_DUMP 0x6D
#define CMD92p_CONTINUE    0x78
#define CMD92p_DIRECT_CMD  0x87
#define CMD92p_EOT         0x92
#define CMD92p_REQUEST     0xA2
#define CMD92p_VAR_HEADER2 0xC9

// Types of variable

#define TI92p_EXPR 0x00
#define TI92p_LIST 0x04
#define TI92p_MAT  0x06
#define TI92p_DATA 0x0A
#define TI92p_TEXT 0x0B
#define TI92p_STR  0x0C
#define TI92p_GDB  0x0D
#define TI92p_FIG  0x0E
#define TI92p_PIC  0x10
#define TI92p_PRGM 0x12
#define TI92p_FUNC 0x13
#define TI92p_MAC  0x14
#define TI92p_DIRL 0x1A
#define TI92p_LDIR 0x1B
#define TI92p_ZIP  0x1C
#define TI92p_BKUP 0x1D
#define TI92p_DIR  0x1F
#define TI92p_ASM  0x21
#define TI92p_IDLIST 0x22
#define TI92p_AMS    0x23
#define TI92p_FLASH  0x24

// Locking variables

#define TI92p_VNONE 0
#define TI92p_VLOCK 1
#define TI92p_VARCH 3

// Keys codes of the TI92/TI92+

#define KEY92p_F1     268
#define KEY92p_F2     269
#define KEY92p_F3     270
#define KEY92p_F4     271
#define KEY92p_F5     272
#define KEY92p_F6     273
#define KEY92p_F7     274
#define KEY92p_F8     275
#define KEY92p_MODE   266
#define KEY92p_CLEAR  263
#define KEY92p_LN     262
#define KEY92p_ESC    264
#define KEY92p_APPS   265
#define KEY92p_CR     13
#define KEY92p_ENTER  13
#define KEY92p_SIN    259
#define KEY92p_COS    260
#define KEY92p_TAN    261
#define KEY92p_POWER  94
#define KEY92p_LP     40  // Left parenthesis
#define KEY92p_RP     41  // Right parenthesis
#define KEY92p_COMMA  44
#define KEY92p_DIVIDE 47
#define KEY92p_MULT   42
#define KEY92p_MINUS  45
#define KEY92p_PLUS   43
#define KEY92p_STO    258
#define KEY92p_SPACE  32
#define KEY92p_EQUALS 61
#define KEY92p_BS     257
#define KEY92p_THETA  136
#define KEY92p_NEG    173
#define KEY92p_DOT    46
#define KEY92p_0      48
#define KEY92p_1      49
#define KEY92p_2      50
#define KEY92p_3      51
#define KEY92p_4      52
#define KEY92p_5      53
#define KEY92p_6      54
#define KEY92p_7      55
#define KEY92p_8      56
#define KEY92p_9      57
#define KEY92p_a      97
#define KEY92p_b      98
#define KEY92p_c      99
#define KEY92p_d      100
#define KEY92p_e      101
#define KEY92p_f      102
#define KEY92p_g      103
#define KEY92p_h      104
#define KEY92p_i      105
#define KEY92p_j      106
#define KEY92p_k      107
#define KEY92p_l      108
#define KEY92p_m      109
#define KEY92p_n      110
#define KEY92p_o      111
#define KEY92p_p      112
#define KEY92p_q      113
#define KEY92p_r      114
#define KEY92p_s      115
#define KEY92p_t      116
#define KEY92p_u      117
#define KEY92p_v      118
#define KEY92p_w      119
#define KEY92p_x      120
#define KEY92p_y      121
#define KEY92p_z      122
#define KEY92p_UP     338
#define KEY92p_UP_RG  342
#define KEY92p_RIGHT  340
#define KEY92p_DW_RG  348
#define KEY92p_DOWN   344
#define KEY92p_DW_LF  345
#define KEY92p_LEFT   337
#define KEY92p_UP_LF  339

// Key modifiers

#define KEY92p_2ND    4096
#define KEY92p_CTRL   8192
#define KEY92p_SHIFT  16384
#define KEY92p_LOCK   32768

// Screen coordinates of the TI92p/TI92+

#define TI92p_ROWS  128
#define TI92p_COLS  240

#endif
