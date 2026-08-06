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

memPort = 5

;;; Link port

linkPort = 7
LINK_STATUS_MASK = 0x03
LINK_RESET = 0x0C0
LINK_D0L = 0x0D4
LINK_D1L = 0x0E8

;;; RAM addresses

curRow = 0x0C00F
curCol = 0x0C010

;;; Flags

apdFlags = 0x08
apdRunning = 3

remoteKeyFlag = 0x15
remoteKeyPress = 7

;;; System calls

_clrScrn = 0x4A82
_dispAHL = 0x4A33
_get_key = 0x4068
_putc = 0x4A2B
_puts = 0x4A37
_runindicoff = 0x4AB1

;;; Macros

.macro CLEAR_LCD
    call _clrScrn
.endm
.macro GET_CSC
    call _get_key
.endm
.macro PUT_CHAR
    call _putc
.endm
.macro PUT_DECIMAL
    xor a
    call _dispAHL
.endm
.macro PUT_STRING
    call _puts
.endm
.macro RUN_INDIC_OFF
    call _runindicoff
.endm

.macro SET_ROM_PAGE
    out (memPort), a
.endm

.macro EXIT_KEY_STR
    .ascii "[EXIT]"
    .endm
;;; Miscellaneous

CALC_ROM_SIZE = 256

