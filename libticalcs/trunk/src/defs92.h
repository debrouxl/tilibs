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

#ifndef DEFS92_H
#define DEFS92_H

// Devicetype

#define PC_TI92 0x09
#define TI92_PC 0x89

// Commands

#define CMD92_VAR_HEADER  0x06
#define CMD92_WAIT_DATA   0x09
#define CMD92_SEND        0x0D
#define CMD92_DATA_PART   0x15
#define CMD92_REFUSED     0x36
#define CMD92_REJECTED    0x36
#define CMD92_TI_OK       0x56
#define CMD92_PC_OK       0x56
#define CMD92_CHK_ERROR   0x5A
#define CMD92_ISREADY     0x68
#define CMD92_SCREEN_DUMP 0x6D
#define CMD92_CONTINUE    0x78
#define CMD92_DIRECT_CMD  0x87
#define CMD92_EOT         0x92
#define CMD92_REQUEST     0xA2

// Types of variable

#define TI92_EXPR 0x00
#define TI92_LIST 0x04
#define TI92_MAT  0x06
#define TI92_DATA 0x0A
#define TI92_TEXT 0x0B
#define TI92_STR  0x0C
#define TI92_GDB  0x0D
#define TI92_FIG  0x0E
#define TI92_PIC  0x10
#define TI92_PRGM 0x12
#define TI92_FUNC 0x13
#define TI92_MAC  0x14
#define TI92_BKUP 0x1D
#define TI92_DIR  0x1F

// Locking variables

#define TI92_VNONE 0
#define TI92_VLOCK 1

// Keys codes of the TI92/TI92+

#define KEY92_F1     268
#define KEY92_F2     269
#define KEY92_F3     270
#define KEY92_F4     271
#define KEY92_F5     272
#define KEY92_F6     273
#define KEY92_F7     274
#define KEY92_F8     275
#define KEY92_MODE   266
#define KEY92_CLEAR  263
#define KEY92_LN     262
#define KEY92_ESC    264
#define KEY92_APPS   265
#define KEY92_CR     13
#define KEY92_ENTER  13
#define KEY92_SIN    259
#define KEY92_COS    260
#define KEY92_TAN    261
#define KEY92_POWER  94
#define KEY92_LP     40  // Left parenthesis
#define KEY92_RP     41  // Right parenthesis
#define KEY92_COMMA  44
#define KEY92_DIVIDE 47
#define KEY92_MULT   42
#define KEY92_MINUS  45
#define KEY92_PLUS   43
#define KEY92_STO    258
#define KEY92_SPACE  32
#define KEY92_EQUALS 61
#define KEY92_BS     257
#define KEY92_THETA  136
#define KEY92_NEG    173
#define KEY92_DOT   46
#define KEY92_0      48
#define KEY92_1      49
#define KEY92_2      50
#define KEY92_3      51
#define KEY92_4      52
#define KEY92_5      53
#define KEY92_6      54
#define KEY92_7      55
#define KEY92_8      56
#define KEY92_9      57
#define KEY92_a      97
#define KEY92_b      98
#define KEY92_c      99
#define KEY92_d      100
#define KEY92_e      101
#define KEY92_f      102
#define KEY92_g      103
#define KEY92_h      104
#define KEY92_i      105
#define KEY92_j      106
#define KEY92_k      107
#define KEY92_l      108
#define KEY92_m      109
#define KEY92_n      110
#define KEY92_o      111
#define KEY92_p      112
#define KEY92_q      113
#define KEY92_r      114
#define KEY92_s      115
#define KEY92_t      116
#define KEY92_u      117
#define KEY92_v      118
#define KEY92_w      119
#define KEY92_x      120
#define KEY92_y      121
#define KEY92_z      122
#define KEY92_UP     338
#define KEY92_UP_RG  342
#define KEY92_RIGHT  340
#define KEY92_DW_RG  348
#define KEY92_DOWN   344
#define KEY92_DW_LF  345
#define KEY92_LEFT   337
#define KEY92_UP_LF  339

// Key modifiers

#define KEY92_2ND    4096
#define KEY92_CTRL   8192
#define KEY92_SHIFT  16384
#define KEY92_LOCK   32768

// Screen coordinates of the TI92

#define TI92_ROWS  128
#define TI92_COLS  240

#endif

