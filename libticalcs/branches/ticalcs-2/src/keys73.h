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

#ifndef __TICALCS_KEYS73__
#define __TICALCS_KEYS73__

// Scan codes of the TI73 (TI's ti73.inc, line 2167)

#define KEY73_Down          	01h
#define KEY73_Left          	02h
#define KEY73_Right         	03h
#define KEY73_Up            	04h
#define KEY73_Enter         	09h
#define KEY73_Add           	0Ah
#define KEY73_Sub           	0Bh
#define KEY73_Mul           	0Ch
#define KEY73_Div           	0Dh
#define KEY73_Const                       0eh
#define KEY73_Clear                       0fh
#define KEY73_Chs                         11h
#define KEY73_3                           12h
#define KEY73_6                           13h
#define KEY73_9                           14h
#define KEY73_RParen                      15h
#define KEY73_MixSimp                     16h
#define KEY73_AppsMenu                    17h
#define KEY73_DecPnt                      19h
#define KEY73_2                           1ah
#define KEY73_5                           1bh
#define KEY73_8                           1ch
#define KEY73_LParen                      1dh
#define KEY73_FracDec                     1eh
#define KEY73_Prgm                        1fh
#define KEY73_StatEd                      20h
#define KEY73_0                           21h
#define KEY73_1                           22h
#define KEY73_4                           23h
#define KEY73_7                           24h
#define KEY73_Percent                     25h
#define KEY73_FracSlash                   26h
#define KEY73_Expon                       27h
#define KEY73_Draw                        28h
#define KEY73_Store                       2ah
#define KEY73_Comma                       2bh
#define KEY73_VarX                        2ch
#define KEY73_Simp                        2dh
#define KEY73_Unit                        2eh
#define KEY73_Square                      2fh
#define KEY73_Math                        30h
#define KEY73_Graph                       31h
#define KEY73_Trace                       32h
#define KEY73_Zoom                        33h
#define KEY73_Window                      34h
#define KEY73_Y                        35h
#define KEY73_2nd                         36h
#define KEY73_Mode                        37h
#define KEY73_Del                         38h

// Array of keys

extern const CalcKey TI73_KEYS[];

#endif
