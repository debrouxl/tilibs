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

memPort = 2

;;; Link port

linkPort = 0
LINK_STATUS_MASK = 0x0C
LINK_RESET = 0x0C0
LINK_D0L = 0x0C1
LINK_D1L = 0x0C2

;;; RAM addresses

curRow = 0x800C
curCol = 0x800D
saveSScreen = 0x8265

;;; Flags

apdFlags = 0x08
apdRunning = 3

;;; System calls

_ClrScrnFull = 0x475D
_DispHL = 0x4709
_GetCSC = 0x4014
_PutC = 0x4705
_PutS = 0x470D
_RunIndicOff = 0x4795

;;; Macros

.macro CLEAR_LCD
    call _ClrScrnFull
.endm
.macro GET_CSC
    call _GetCSC
.endm
.macro PUT_CHAR
    call _PutC
.endm
.macro PUT_DECIMAL
    call _DispHL
.endm
.macro PUT_STRING
    call _PutS
.endm
.macro RUN_INDIC_OFF
    call _RunIndicOff
.endm

.macro SET_ROM_PAGE
    push af
    and 8
    rlca
    or LINK_RESET
    out (linkPort), a
    pop af
    and 7
    or 0x88
    out (2), a
.endm

.macro EXIT_KEY_STR
    .byte 0x0C1
    .ascii "MODE]"
    .endm
;;; Miscellaneous

CALC_ROM_SIZE = 256
