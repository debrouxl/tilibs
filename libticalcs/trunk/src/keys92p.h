/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#ifndef __TICALCS_KEYS92P__
#define __TICALCS_KEYS92P__

// Keys codes of the TI92/TI92+


#define KEY92P_CR     13
#define KEY92P_ENTER  13
#define KEY92P_SPACE  32

#define KEY92P_LP     40	// Left parenthesis
#define KEY92P_RP     41	// Right parenthesis
#define KEY92P_MULT   42
#define KEY92P_PLUS   43
#define KEY92P_COMMA  44
#define KEY92P_MINUS  45
#define KEY92P_DOT    46
#define KEY92P_DIVIDE 47
#define KEY92P_0      48
#define KEY92P_1      49
#define KEY92P_2      50
#define KEY92P_3      51
#define KEY92P_4      52
#define KEY92P_5      53
#define KEY92P_6      54
#define KEY92P_7      55
#define KEY92P_8      56
#define KEY92P_9      57

#define KEY92P_EQUALS 61

#define KEY92P_POWER  94

#define KEY92P_a      97
#define KEY92P_b      98
#define KEY92P_c      99
#define KEY92P_d      100
#define KEY92P_e      101
#define KEY92P_f      102
#define KEY92P_g      103
#define KEY92P_h      104
#define KEY92P_i      105
#define KEY92P_j      106
#define KEY92P_k      107
#define KEY92P_l      108
#define KEY92P_m      109
#define KEY92P_n      110
#define KEY92P_o      111
#define KEY92P_p      112
#define KEY92P_q      113
#define KEY92P_r      114
#define KEY92P_s      115
#define KEY92P_t      116
#define KEY92P_u      117
#define KEY92P_v      118
#define KEY92P_w      119
#define KEY92P_x      120
#define KEY92P_y      121
#define KEY92P_z      122

#define KEY92P_THETA  136
#define KEY92P_NEG    173

#define KEY92P_DELETE 257
#define KEY92P_STO    258
#define KEY92P_SIN    259
#define KEY92P_COS    260
#define KEY92P_TAN    261
#define KEY92P_LN     262
#define KEY92P_CLEAR  263
#define KEY92P_ESC    264
#define KEY92P_MENU   265
#define KEY92P_MODE   266
#define KEY92P_ON	  267
#define KEY92P_F1     268
#define KEY92P_F2     269
#define KEY92P_F3     270
#define KEY92P_F4     271
#define KEY92P_F5     272
#define KEY92P_F6     273
#define KEY92P_F7     274
#define KEY92P_F8     275
#define KEY92P_CHS	  276

#define KEY92P_LEFT   337
#define KEY92P_UP     338
#define KEY92P_UP_LF  339
#define KEY92P_RIGHT  340
#define KEY92P_UP_RG  342
#define KEY92P_DOWN   344
#define KEY92P_DW_LF  345
#define KEY92P_DW_RG  348

// Key modifiers

#define KEY92P_2ND    4096
#define KEY92P_CTRL   8192
#define KEY92P_SHIFT  16384
#define KEY92P_DRAG   32768

// Array of keys

extern const CalcKey TI92P_KEYS[];

#endif
