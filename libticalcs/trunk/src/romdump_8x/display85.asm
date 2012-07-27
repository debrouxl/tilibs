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
	ld hl, 0FC00h
	ld (hl), l
	ld de, 0FC01h
	ld bc, 03FFh
	ldir
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

	   ld bc, curRow
	   ld a, (bc)
	   rrca
	   ld e, a
	   or 0FCh
	   ld d, a

	   inc bc
	   ld a, (bc)
	   inc a
	   ld (bc), a
	   dec a
	   ld b, a
	   add a, a
	   add a, b
	   rrca
	   rrca
	   xor e
	   and 1Fh
	   xor e
	   ld e, a

	   ld b, 7
PutC_Loop:
	   ld a, (curCol)
	   and 3
	   jr z, PutC_0
	   dec a
	   jr z, PutC_1
	   dec a
	   ld a, (hl)
	   rrca
	   rrca
	   jr z, PutC_2

	   ld c, a
	   inc e
	   ld a, (de)
	   xor c
	   and 03Fh
	   xor c
	   ld (de), a
	   dec e
	   ld a, (de)
	   xor c
	   and 0F0h
	   xor c
	   jr PutC_Next

PutC_0:
	   ld a, (de)
	   and 0C0h
	   or (hl)
	   jr PutC_Next

PutC_1:
	   ld a, (hl)
	   rlca
	   rlca
	   ld c, a
	   ld a, (de)
	   and 3
	   or c
	   jr PutC_Next

PutC_2:
	   rrca
	   rrca
	   ld c, a
	   inc e
	   ld a, (de)
	   xor c
	   and 0Fh
	   xor c
	   ld (de), a
	   dec e
	   ld a, (de)
	   xor c
	   and 0FCh
	   xor c
PutC_Next:
	   ld (de), a
	   inc hl
	   ld a, e
	   add a, 16
	   ld e, a
	   djnz PutC_Loop
	   pop bc
	  pop de
	 pop hl
	ret
