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

;; ClearLCD:
;;
;; Clear the LCD.
;;
;; Destroys:
;; - AF, BC, DE, HL

ClearLCD:
	ld a, 20h
ClearLCD_Loop:
	ld c, a
	call LCDCommand
	ld b, 64
ClearLCD_ColumnLoop:
	call LCDBusy
	xor a
	out (11h), a
	djnz ClearLCD_ColumnLoop
	ld a, c
	inc a
	cp 2Ch
	jr c, ClearLCD_Loop
	ret

;; PutC:
;;
;; Display an ASCII character.
;;
;; Input:
;; - A = character
;; - (curRow) = cursor row
;; - (curCol) = cursor column
;;
;; Output:
;; - (curRow), (curCol) = updated
;;
;; Destroys:
;; - AF

PutC:
	push hl
	 push de
	  push bc
fontTable equ $ + 1
	   ld hl, 0
	   ld bc, 8
PutC_FindBitmapLoop:
	   cp (hl)
	   jr z, PutC_FoundBitmap
	   add hl, bc
	   jr nc, PutC_FindBitmapLoop
PutC_FoundBitmap:
	   inc hl

	   xor a
	   call LCDCommand

	   ld de, curCol
	   ld a, (de)
	   inc a
	   ld (de), a
	   dec a
	   or 20h
	   call LCDCommand

	   dec de
	   ld a, (de)
	   add a, a
	   add a, a
	   add a, a
	   or 80h
	   call LCDCommand

	   ld b, 7
PutC_Loop:
	   call LCDBusy
	   ld a, (hl)
	   out (11h), a
	   inc hl
	   djnz PutC_Loop

	   pop bc
	  pop de
	 pop hl
	ld a, 1

;; LCDCommand:
;;
;; Send a command to the LCD driver.
;;
;; Input:
;; - A = command
;;
;; Destroys:
;; - None

LCDCommand:
	push af
	 call LCDBusy
	 pop af
	out (10h), a
	ret

;; LCDBusy:
;;
;; Wait for the LCD driver to be ready to accept a command.
;;
;; Destroys:
;; - AF

LCDBusy:
	in a, (10h)
	add a, a
	jr c, LCDBusy
	ret
