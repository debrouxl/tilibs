;;; -*- TI-Asm -*-

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73/82/83/83+/84+/85/86 ROM Dumper
;;;
;;; Copyright (c) 2012-2013 Benjamin Moody
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
memExtPort = 0x0E

;;; Link port

linkPort = 0
LINK_STATUS_MASK = 3
LINK_RESET = 0
LINK_D0L = 1
LINK_D1L = 2

;;; RAM addresses

saveSScreen = 0x8798
curRow = 0x8459

;;; Flags

apdFlags = 0x08
apdRunning = 3

appFlags = 0x0D
appTextSave = 1

remoteKeyFlag = 0x1F
remoteKeyPress = 5

;;; System calls

APP_PUSH_ERRORH = 0x59
APP_POP_ERRORH = 0x5C

_ClrScrnFull = 0x453D
_DispHL = 0x44FE
_GetCSC = 0x4018
_PutC = 0x44FB
_PutS = 0x4501
_RecAByteIO = 0x4EE8
_RunIndicOff = 0x4567
_SendAByte = 0x4ECA

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
    rlca
    out (memExtPort), a
    srl a
    out (memPort), a
.endm

.macro EXIT_KEY_STR
    .byte 0x0C1
    .ascii "MODE]"
    .endm
;;; Miscellaneous

CALC_ROM_SIZE = 4096
