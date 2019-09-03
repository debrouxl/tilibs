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

memPort equ 5

;;; Link port

linkPort equ 7
LINK_STATUS_MASK equ 03h
LINK_RESET equ 0C0h
LINK_D0L equ 0D4h
LINK_D1L equ 0E8h

;;; RAM addresses

curRow equ 0C00Fh
curCol equ 0C010h

;;; Flags

apdFlags       equ 08h
apdRunning     equ 3

remoteKeyFlag  equ 15h
remoteKeyPress equ 7

;;; System calls

_clrScrn     equ 4A82h
_dispAHL     equ 4A33h
_get_key     equ 4068h
_putc        equ 4A2Bh
_puts        equ 4A37h
_runindicoff equ 4AB1h

;;; Macros

macro CLEAR_LCD
    call _clrScrn
endm
macro GET_CSC
    call _get_key
endm
macro PUT_CHAR
    call _putc
endm
macro PUT_DECIMAL
    xor a
    call _dispAHL
endm
macro PUT_STRING
    call _puts
endm
macro RUN_INDIC_OFF
    call _runindicoff
endm

macro SET_ROM_PAGE
    out (memPort), a
endm

macro EXIT_KEY_STR
    db "[EXIT]"
endm

;;; Miscellaneous

CALC_ROM_SIZE equ 256

