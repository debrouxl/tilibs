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

;; DispHL:
;;
;; Display a 16-bit unsigned integer in decimal, padded on the left
;; with spaces.
;;
;; Input:
;; - HL = number
;; - (curRow) = cursor row
;; - (curCol) = cursor column
;;
;; Output:
;; - (curRow), (curCol) = updated
;;
;; Destroys:
;; - AF, BC, DE, HL

DispHL:
	ld b, 5
	ex de, hl
DispHL_PadLoop:
	ld a, ' '
	call PutC
	djnz DispHL_PadLoop
	ld hl, curCol
	ld a, (hl)
	push af
DispHL_Loop:
	 dec (hl)
	 ex de, hl
	 ld bc, 100Ah
	 xor a
DispHL_DivLoop:
	 add hl, hl
	 adc a, a
	 cp c
	 jr c, DispHL_DivNC
	 sub c
	 inc l
DispHL_DivNC:
	 djnz DispHL_DivLoop
	 ex de, hl
	 add a, '0'
	 call PutC
	 dec (hl)
	 ld a, d
	 or e
	 jr nz, DispHL_Loop
	 pop af
	ld (hl), a
	ret

;; PutS:
;;
;; Display a zero-terminated ASCII string.
;;
;; Input:
;; - HL = address of string
;; - (curRow) = cursor row
;; - (curCol) = cursor column
;;
;; Output:
;; - (curRow), (curCol) = updated
;;
;; Destroys:
;; - AF, HL

PutS:
	ld a, (hl)
	inc hl
	or a
	ret z
	call PutC
	jr PutS
