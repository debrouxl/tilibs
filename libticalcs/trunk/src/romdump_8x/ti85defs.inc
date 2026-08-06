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

kbdScanCode = 0x8000
curRow = 0x800C
curCol = 0x800D
errNo = 0x8337
errSP = 0x8338
plotSScreen = 0x8641
fpBase = 0x8BDF
FPS = 0x8BE1
OPBase = 0x8BE3
OPS = 0x8BE5

;;; Flags

apdFlags = 0x08
apdRunning = 3

indicFlags = 0x12
indicRun = 0

;;; Macros

.macro CLEAR_LCD
    call ClearLCD
.endm
.macro PUT_CHAR
    call PutC
.endm
.macro PUT_DECIMAL
    call DispHL
.endm
.macro PUT_STRING
    call PutS
.endm

.macro GET_CSC
    di
    ld hl, kbdScanCode
    ld a, (hl)
    ld (hl), 0
.endm
.macro RUN_INDIC_OFF
    res indicRun, (iy + indicFlags)
.endm
.macro SET_ROM_PAGE
    out (memPort), a
.endm

.macro EXIT_KEY_STR
    .ascii "[EXIT]"
    .endm
;;; Miscellaneous

CALC_ROM_SIZE = 128
