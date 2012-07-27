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

linkPort          equ 0
LINK_STATUS_MASK  equ 0Ch
LINK_RESET        equ 0C0h
LINK_D0L          equ 0C1h
LINK_D1L          equ 0C2h

;;; RAM addresses

curRow       equ 800Ch
curCol       equ 800Dh
saveSScreen  equ 8265h

;;; Flags

apdFlags    equ 08h
apdRunning  equ 3

;;; System calls

_ClrScrnFull  equ 475Dh
_DispHL       equ 4709h
_GetCSC       equ 4014h
_PutC         equ 4705h
_PutS         equ 470Dh
_RunIndicOff  equ 4795h

;;; Macros

#define CLEAR_LCD      call _ClrScrnFull
#define GET_CSC        call _GetCSC
#define PUT_CHAR       call _PutC
#define PUT_DECIMAL    call _DispHL
#define PUT_STRING     call _PutS
#define RUN_INDIC_OFF  call _RunIndicOff

#define SET_ROM_PAGE  push af \ and 8 \ rlca \ or LINK_RESET \ out (linkPort), a \ pop af \ and 7 \ or 88h \ out (2), a

#define EXIT_KEY_STR db 0C1h, "MODE]"

;;; Miscellaneous

#define CALC_ROM_SIZE 256
