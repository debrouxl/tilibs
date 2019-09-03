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

;; LinkSendBC:
;;
;; Send a sequence of bytes to the link port.
;;
;; Input:
;; - DE = address of bytes to send
;; - BC = number of bytes to send
;; - HL = checksum so far
;;
;; Output:
;; - Carry flag set if there was an error
;; - HL = updated checksum
;;
;; Destroys:
;; - AF, BC, DE

LinkSendBC:
	ld a, b
	or c
	ret z
	dec bc
	push bc
 if defined TI83
	 in a, (linkPort)
	 and 10h
	 or LINK_RESET
	 push af
 endif
	  ld a, (de)
	  inc de
	  ld c, a
	  ld b, 0
	  add hl, bc
	  push de
	   push hl
	    call LinkPutByte
	    pop hl
	   pop de
 if defined TI83
	  pop bc
	 ld c, linkPort
	 out (c), b
 endif
	 pop bc
	ret c
	jr LinkSendBC

;; LinkReceiveB:
;;
;; Read a sequence of bytes from the link port.
;;
;; Input:
;; - B = number of bytes to read
;; - DE = address of buffer to store data
;; - HL = checksum so far
;;
;; Output:
;; - Carry flag set if there was an error
;; - HL = updated checksum
;;
;; Destroys:
;; - AF, BC, DE

LinkReceiveB:
 if defined TI83
	in a, (linkPort)
	and 10h
	or LINK_RESET
	ld c, a
 endif
LinkReceiveB_Loop:
	push bc
	 push hl
	  push de
	   call LinkGetByte
	   pop de
	  pop hl
	 jr c, LinkReceiveB_Error
	 ld b, 0
	 add hl, bc
	 ld a, c
	 ld (de), a
	 inc de
	 pop bc
	djnz LinkReceiveB_Loop
	or a
	push bc
LinkReceiveB_Error:
	 pop bc
 if defined TI83
	ld a, c
	out (linkPort), a
 endif
	ret

;; LinkError:
;;
;; Reset the link port and wait for ~2 seconds to ensure that the
;; PC sees an error condition.
;;
;; Destroys:
;; - B

LinkError:
	ld b, 255
	push af
 if defined CALC_LINK_ASSIST
	 in a, (2)
	 and 80h
	 out (8), a
 endif
 if defined TI83
	 in a, (linkPort)
	 and 10h
	 or LINK_RESET
 else
	 ld a, LINK_RESET
 endif
	 out (linkPort), a
LinkDelay_Loop:
	 ei
	 halt
	 halt
	 djnz LinkDelay_Loop
 if defined CALC_LINK_ASSIST
	 xor a
	 out (8), a
 endif
	 pop af
	ret
