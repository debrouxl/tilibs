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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TICALCS_KEYS89__
#define __TICALCS_KEYS89__

// Keys of the TI89 (to update: tiams.h, line 38)

#define KEY89_CR     13
#define KEY89_ENTER  13

#define KEY89_LP     40		// Left parenthesis
#define KEY89_RP     41		// Right parenthesis
#define KEY89_MULT   42
#define KEY89_PLUS   43
#define KEY89_COMMA  44
#define KEY89_MINUS  45
#define KEY89_DOT    46
#define KEY89_DIVIDE 47
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

#define KEY89_EQUALS 61

#define KEY89_POWER  94

#define KEY89_T      116
#define KEY89_X      120
#define KEY89_Y      121
#define KEY89_Z      122
#define KEY89_TUBE	 124

#define KEY89_EE     149
#define KEY89_NEG    173

#define KEY89_BS	 257
#define KEY89_STO    258

#define KEY89_CLEAR  263
#define KEY89_ESC    264
#define KEY89_APPS   265
#define KEY89_MODE   266
#define KEY89_ON	 267
#define KEY89_F1     268
#define KEY89_F2     269
#define KEY89_F3     270
#define KEY89_F4     271
#define KEY89_F5     272
#define KEY89_F6     273
#define KEY89_F7     274
#define KEY89_F8     275
#define KEY89_CHS	 276
#define KEY89_HOME   277
#define KEY89_CATLG  278

#define KEY89_LEFT   337
#define KEY89_UP     338
#define KEY89_RIGHT  340
#define KEY89_DOWN   344

// Key modifiers

#define KEY89_2ND    4096
#define KEY89_CTRL   8192
#define KEY89_SHIFT  16384
#define KEY89_ALPHA  32768

#endif
