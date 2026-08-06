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

ramCode = 0x8100
arcInfo = 0x83EE
pagedCount = 0x9834
pagedBuf = 0x983A
curRow = 0x844B
curCol = 0x844C
iMathPtr5 = 0x84DB
saveSScreen = 0x86EC

;;; Flags

apdFlags = 0x08
apdRunning = 3

remoteKeyFlag = 0x1F
remoteKeyPress = 5

;;; System calls

APP_PUSH_ERRORH = 0x59
APP_POP_ERRORH = 0x5C

_ClrScrnFull = 0x4546
_DispHL = 0x4507
_GetBaseVer = 0x4C6F
_GetBootVer = 0x80B7
_GetHWVer = 0x80BA
_GetCSC = 0x4018
_GetVariableData = 0x5092
_LdHLind = 0x4009
_Mov18B = 0x47DA
_PutC = 0x4504
_PutS = 0x450A
_RecAByteIO = 0x4F03
_RunIndicOff = 0x4570
_SendAByte = 0x4EE5
_WriteAByteSafe = 0x80C6

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

CALC_ROM_SIZE = 0
