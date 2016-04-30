/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 2016  Lionel Debroux
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

#ifndef __TICALCS_KEYSNSP__
#define __TICALCS_KEYSNSP__

// Keys codes of the Nspire

#define KEYNSP_ESC                     0x1B9600
#define KEYNSP_CTRL_ESC                0x00EF00
#define KEYNSP_TAB                     0x099500
#define KEYNSP_SHIFT_TAB               0x7C9500
#define KEYNSP_CTRL_TAB                0x00ED00
#define KEYNSP_HOME                    0x00FD00
#define KEYNSP_CTRL_HOME               0x00FE00
#define KEYNSP_MENU                    0x003600
#define KEYNSP_CTRL_MENU               0x00CE00
#define KEYNSP_MOUSE_CONTEXT_MENU      0x00A000
#define KEYNSP_CLICK                   0x00AD00
#define KEYNSP_CTRL_CLICK              0x00AC00
#define KEYNSP_SHIFT_GRAB              0x00F900
#define KEYNSP_LEFT                    0x000700
#define KEYNSP_SHIFT_LEFT              0x00F100
#define KEYNSP_SHIFT_HOLD_LEFT         0x000703
#define KEYNSP_CTRL_LEFT               0x00DA00
#define KEYNSP_RIGHT                   0x002700
#define KEYNSP_SHIFT_RIGHT             0x00F200
#define KEYNSP_SHIFT_HOLD_RIGHT        0x002703
#define KEYNSP_CTRL_RIGHT              0x00DB00
#define KEYNSP_UP                      0x001700
#define KEYNSP_SHIFT_UP                0x00F300
#define KEYNSP_SHIFT_HOLD_UP           0x001703
#define KEYNSP_CTRL_UP                 0x00DC00
#define KEYNSP_DOWN                    0x003700
#define KEYNSP_SHIFT_DOWN              0x00F400
#define KEYNSP_SHIFT_HOLD_DOWN         0x003703
#define KEYNSP_CTRL_DOWN               0x00DD00
#define KEYNSP_EQUAL                   0x3D7500
#define KEYNSP_CTRL_EQUAL              0x00E900
#define KEYNSP_SUCH_THAT               0x7CED00
#define KEYNSP_A                       0x616600
#define KEYNSP_SHIFT_A                 0x416600
#define KEYNSP_CTRL_A                  0x616600
#define KEYNSP_B                       0x624600
#define KEYNSP_SHIFT_B                 0x424600
#define KEYNSP_CTRL_B                  0x02B100
#define KEYNSP_C                       0x632600
#define KEYNSP_SHIFT_C                 0x432600
#define KEYNSP_CTRL_C                  0x03B200
#define KEYNSP_FLAG                    0x00A700
#define KEYNSP_CTRL_FLAG               0x00F800
#define KEYNSP_LESS_THAN               0x3CA600
#define KEYNSP_CTRL_LESS_THAN          0x00CF00
#define KEYNSP_D                       0x648500
#define KEYNSP_SHIFT_D                 0x448500
#define KEYNSP_CTRL_D                  0x04B300
#define KEYNSP_E                       0x656500
#define KEYNSP_SHIFT_E                 0x456500
#define KEYNSP_CTRL_E                  0x05B400
#define KEYNSP_F                       0x664500
#define KEYNSP_SHIFT_F                 0x464500
#define KEYNSP_CTRL_F                  0x06B500
#define KEYNSP_G                       0x672500
#define KEYNSP_SHIFT_G                 0x472500
#define KEYNSP_CTRL_G                  0x07B600
#define KEYNSP_TILDE                   0x27F500
#define KEYNSP_CTRL_TILDE              0x240500
#define KEYNSP_GREATER_THAN            0x3E8600
#define KEYNSP_CTRL_GREATER_THAN       0x00DE00
#define KEYNSP_H                       0x688400
#define KEYNSP_SHIFT_H                 0x488400
#define KEYNSP_CTRL_H                  0x08B700
#define KEYNSP_I                       0x696400
#define KEYNSP_SHIFT_I                 0x496400
#define KEYNSP_CTRL_I                  0x09B800
#define KEYNSP_J                       0x6A4400
#define KEYNSP_SHIFT_J                 0x4A4400
#define KEYNSP_CTRL_J                  0x0AB900
#define KEYNSP_K                       0x6B2400
#define KEYNSP_SHIFT_K                 0x4B2400
#define KEYNSP_CTRL_K                  0x0BBA00
#define KEYNSP_QUOTE                   0x22A100
#define KEYNSP_IMAGINARY               0x00A200
#define KEYNSP_CTRL_IMAGINARY          0x00A100
#define KEYNSP_L                       0x6C8300
#define KEYNSP_SHIFT_L                 0x4C8300
#define KEYNSP_CTRL_L                  0x0CBB00
#define KEYNSP_M                       0x6D6300
#define KEYNSP_SHIFT_M                 0x4D6300
#define KEYNSP_CTRL_M                  0x0DBC00
#define KEYNSP_N                       0x6E4300
#define KEYNSP_SHIFT_N                 0x4E4300
#define KEYNSP_CTRL_N                  0x0EBD00
#define KEYNSP_O                       0x6F2300
#define KEYNSP_SHIFT_O                 0x4F2300
#define KEYNSP_CTRL_O                  0x0FBE00
#define KEYNSP_COLON                   0x3A0100
#define KEYNSP_CTRL_COLON              0x3B0200
#define KEYNSP_EXP                     0x00A400
#define KEYNSP_P                       0x708200
#define KEYNSP_SHIFT_P                 0x508200
#define KEYNSP_CTRL_P                  0x10BF00
#define KEYNSP_Q                       0x716200
#define KEYNSP_SHIFT_Q                 0x516200
#define KEYNSP_CTRL_Q                  0x11C000
#define KEYNSP_R                       0x724200
#define KEYNSP_SHIFT_R                 0x524200
#define KEYNSP_CTRL_R                  0x12C100
#define KEYNSP_S                       0x732200
#define KEYNSP_SHIFT_S                 0x532200
#define KEYNSP_CTRL_S                  0x13C200
#define KEYNSP_QUESTION_MARK           0x3F0300
#define KEYNSP_CTRL_QUESTION_MARK      0x00CB00
#define KEYNSP_PI                      0x00A300
#define KEYNSP_CTRL_PI                 0x00F400
#define KEYNSP_T                       0x748100
#define KEYNSP_SHIFT_T                 0x548100
#define KEYNSP_CTRL_T                  0x14C300
#define KEYNSP_U                       0x756100
#define KEYNSP_SHIFT_U                 0x556100
#define KEYNSP_CTRL_U                  0x15C400
#define KEYNSP_V                       0x764100
#define KEYNSP_SHIFT_V                 0x564100
#define KEYNSP_CTRL_V                  0x16C500
#define KEYNSP_W                       0x772100
#define KEYNSP_SHIFT_W                 0x572100
#define KEYNSP_CTRL_W                  0x17C600
#define KEYNSP_COMMA                   0x2CA000
#define KEYNSP_THETA                   0x880600
#define KEYNSP_X                       0x788000
#define KEYNSP_SHIFT_X                 0x588000
#define KEYNSP_CTRL_X                  0x18C700
#define KEYNSP_Y                       0x796000
#define KEYNSP_SHIFT_Y                 0x596000
#define KEYNSP_CTRL_Y                  0x19C800
#define KEYNSP_Z                       0x7A4000
#define KEYNSP_SHIFT_Z                 0x5A4000
#define KEYNSP_CTRL_Z                  0x1AC900
#define KEYNSP_SPACE                   0x202000
#define KEYNSP_CTRL_SPACE              0x00CD00
#define KEYNSP_NEW_LINE                0x000A00
#define KEYNSP_CTRL                    0x00AA04
#define KEYNSP_SHIFT                   0x00AB03
#define KEYNSP_CTRL_SHIFT              0x00AB07
#define KEYNSP_BACK_SPACE              0x081500
#define KEYNSP_SHIFT_BACK_SPACE        0x081500
#define KEYNSP_CTRL_BACK_SPACE         0x00E304
#define KEYNSP_VAR                     0x00AF00
#define KEYNSP_CTRL_VAR                0x00A800
#define KEYNSP_LEFT_PARENTHESES        0x285500
#define KEYNSP_CTRL_LEFT_PARENTHESES   0x00E700
#define KEYNSP_RIGHT_PARENTHESES       0x293500
#define KEYNSP_CTRL_RIGHT_PARENTHESES  0x00E500
#define KEYNSP_CATALOG                 0x009100
#define KEYNSP_CTRL_CATALOG            0x00EE00
#define KEYNSP_POWER                   0x5E9300
#define KEYNSP_CTRL_POWER              0x00EB00
#define KEYNSP_SIN                     0x007400
#define KEYNSP_CTRL_SIN                0x00E800
#define KEYNSP_COS                     0x005400
#define KEYNSP_CTRL_COS                0x00E600
#define KEYNSP_TAN                     0x003400
#define KEYNSP_CTRL_TAN                0x00E400
#define KEYNSP_SLASH                   0x2F1400
#define KEYNSP_CTRL_SLASH              0x00E200
#define KEYNSP_SQUARE                  0x009300
#define KEYNSP_CTRL_SQUARE             0x009200
#define KEYNSP_SEVEN                   0x377100
#define KEYNSP_CTRL_SEVEN              0x00D700
#define KEYNSP_EIGHT                   0x385100
#define KEYNSP_CTRL_EIGHT              0x00D800
#define KEYNSP_NINE                    0x393100
#define KEYNSP_CTRL_NINE               0x00D900
#define KEYNSP_TIMES                   0x2A1300
#define KEYNSP_CTRL_TIMES              0x00E100
#define KEYNSP_TEN_POWER               0x00EC00
#define KEYNSP_CTRL_TEN_POWER          0x009400
#define KEYNSP_FOUR                    0x347200
#define KEYNSP_CTRL_FOUR               0x00D400
#define KEYNSP_FIVE                    0x355200
#define KEYNSP_CTRL_FIVE               0x00D500
#define KEYNSP_SIX                     0x363200
#define KEYNSP_CTRL_SIX                0x00D600
#define KEYNSP_MINUS                   0x2D1200
#define KEYNSP_CTRL_MINUS              0x00E000
#define KEYNSP_E_POWER                 0x00CA00
#define KEYNSP_CTRL_E_POWER            0x00A900
#define KEYNSP_ONE                     0x317300
#define KEYNSP_CTRL_ONE                0x00D100
#define KEYNSP_TWO                     0x325300
#define KEYNSP_CTRL_TWO                0x00D200
#define KEYNSP_THREE                   0x333300
#define KEYNSP_CTRL_THREE              0x00D300
#define KEYNSP_PLUS                    0x2B1100
#define KEYNSP_CTRL_PLUS               0x00DF00
#define KEYNSP_ON                      0x000B00
#define KEYNSP_CTRL_ON                 0x00F000
#define KEYNSP_ZERO                    0x305000
#define KEYNSP_CTRL_ZERO               0x00D000
#define KEYNSP_POINT                   0x2E7000
#define KEYNSP_NEG                     0xB13000
#define KEYNSP_CTRL_NEG                0x00AE00
#define KEYNSP_ENTER                   0x0D1000
#define KEYNSP_CTRL_ENTER              0x00A600


#endif
