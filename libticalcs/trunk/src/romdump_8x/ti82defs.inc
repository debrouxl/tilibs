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

memPort equ 2

;;; Link port

linkPort    equ 0
LINK_RESET  equ 0C0h

;;; RAM addresses

kbdScanCode  equ 8000h
curRow       equ 800Ch
curCol       equ 800Dh
errNo        equ 8219h
errSP        equ 821Ah
saveSScreen  equ 8228h
fpBase       equ 8D0Ch
FPS          equ 8D0Eh
OPBase       equ 8D10h
OPS          equ 8D12h

;;; Flags

apdFlags    equ 08h
apdRunning  equ 3

indicFlags  equ 12h
indicRun    equ 0

;;; Constants

cxCmd equ 40h
cxError equ 52h

;;; Macros

macro CLEAR_LCD
    call ClearLCD
endm
macro PUT_CHAR
    call PutC
endm
macro PUT_DECIMAL
    call DispHL
endm
macro PUT_STRING
    call PutS
endm

macro GET_CSC
    di
    ld hl, kbdScanCode
    ld a, (hl)
    ld (hl), 0
endm
macro RUN_INDIC_OFF
    res indicRun, (iy + indicFlags)
endm
macro SET_ROM_PAGE
    and 7
    or 88h
    out (memPort), a
endm

macro EXIT_KEY_STR
    db "[MODE]"
endm

;;; Miscellaneous

CALC_ROM_SIZE equ 128
