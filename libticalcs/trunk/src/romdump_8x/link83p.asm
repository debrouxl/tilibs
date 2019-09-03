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

;; CheckLinkActivity:
;;
;; Check whether we are receiving data from the link port.
;;
;; Output:
;; - Zero flag clear (NZ) if we are receiving data
;;
;; Destroys:
;; - AF
;; - DE, HL

CheckLinkActivity:
	in a, (linkPort)
	cpl
	and 3
	ret nz
 if ! defined TI84PC
	in a, (2)
	and 80h
	ret z
 endif
	in a, (9)
	and 18h
	ret

;; LinkGetByte:
;;
;; Read a byte from the link port.
;;
;; Output:
;; - Carry flag set if there was an error
;; - C = byte value
;;
;; Destroys:
;; - Interrupts disabled
;; - AF, B, DE, HL

LinkGetByte:
	ld hl, LinkIOError
	call APP_PUSH_ERRORH
	 B_CALL _RecAByteIO
	 ld d, a
	 jr LinkIODone

;; LinkPutByte:
;;
;; Read a byte from the link port.
;;
;; Input:
;; - A = C = byte value to write
;;
;; Output:
;; - Carry flag set if there was an error
;;
;; Destroys:
;; - Interrupts disabled
;; - AF, B, DE, HL

LinkPutByte:
	ld hl, LinkIOError
	call APP_PUSH_ERRORH
	 B_CALL _SendAByte
LinkIODone:
	 call APP_POP_ERRORH
	ld c, d
	or a
	ret

LinkIOError:
	scf
	ret
