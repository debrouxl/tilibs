;;; -*- TI-Asm -*-

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73/82/83/83+/84+/85/86 ROM Dumper
;;;
;;; Copyright (c) 2012 Benjamin Moody
;;;
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Memory mapping

memPort = 6

;;; Link port

linkPort = 0
LINK_STATUS_MASK = 3
LINK_RESET = 0
LINK_D0L = 1
LINK_D1L = 2

;;; RAM addresses

curRow = 0x83E2
curCol = 0x83E3
appBackUpScreen = 0x8613
saveSScreen = 0x8A52
curPC = 0x9A5D

;;; Flags

apdFlags = 0x08
apdRunning = 3

remoteKeyFlag = 0x1F
remoteKeyPress = 5

;;; System calls

_ClrScrnFull = 0x44AD
_DispHL = 0x446E
_FlashToRAM2 = 0x8054
_GetBaseVer = 0x4B8E
_GetCSC = 0x400F
_Mov18B = 0x46FC
_PutC = 0x446B
_PutS = 0x4471
_RunIndicOff = 0x44D7

;;; Macros

.macro B_CALL arg
    .byte 0x0EF
    .word \arg
.endm

.macro CLEAR_LCD
    B_CALL _ClrScrnFull
.endm
.macro GET_CSC
    B_CALL _GetCSC
.endm
.macro PUT_CHAR
    B_CALL _PutC
.endm
.macro PUT_DECIMAL
    B_CALL _DispHL
.endm
.macro PUT_STRING
    B_CALL _PutS
.endm
.macro RUN_INDIC_OFF
    B_CALL _RunIndicOff
.endm
.macro SET_ROM_PAGE
    out (memPort), a
.endm

.macro EXIT_KEY_STR
    .byte 0x0C1
    .ascii "MODE]"
    .endm
;;; Miscellaneous

CALC_ROM_SIZE = 512
