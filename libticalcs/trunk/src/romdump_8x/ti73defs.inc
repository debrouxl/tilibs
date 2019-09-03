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

memPort equ 6

;;; Link port

linkPort          equ 0
LINK_STATUS_MASK  equ 3
LINK_RESET        equ 0
LINK_D0L          equ 1
LINK_D1L          equ 2

;;; RAM addresses

curRow           equ 83E2h
curCol           equ 83E3h
appBackUpScreen  equ 8613h
saveSScreen      equ 8A52h
curPC            equ 9A5Dh

;;; Flags

apdFlags       equ 08h
apdRunning     equ 3

remoteKeyFlag  equ 1Fh
remoteKeyPress equ 5

;;; System calls

_ClrScrnFull  equ 44ADh
_DispHL       equ 446Eh
_FlashToRAM2  equ 8054h
_GetBaseVer   equ 4B8Eh
_GetCSC       equ 400Fh
_Mov18B       equ 46FCh
_PutC         equ 446Bh
_PutS         equ 4471h
_RunIndicOff  equ 44D7h

;;; Macros

macro B_CALL, arg
    db 0EFh
    dw arg
endm

macro CLEAR_LCD
    B_CALL _ClrScrnFull
endm
macro GET_CSC
    B_CALL _GetCSC
endm
macro PUT_CHAR
    B_CALL _PutC
endm
macro PUT_DECIMAL
    B_CALL _DispHL
endm
macro PUT_STRING
    B_CALL _PutS
endm
macro RUN_INDIC_OFF
    B_CALL _RunIndicOff
endm
macro SET_ROM_PAGE
    out (memPort), a
endm

macro EXIT_KEY_STR
    db 0C1h, "MODE]"
endm

;;; Miscellaneous

CALC_ROM_SIZE equ 512
