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

BIT_TIMEOUT equ 65535

if ! defined TI82

macro LD_A_LINK_D0L
    ld a, LINK_D0L
endm
macro LD_A_LINK_D1L
    ld a, LINK_D1L
endm
macro AND_LINK_STATUS_MASK
    and LINK_STATUS_MASK
endm

else ; TI82

macro LD_A_LINK_D0L
    ld a, (linkD0L)
endm
macro LD_A_LINK_D1L
    ld a, (linkD1L)
endm
macro AND_LINK_STATUS_MASK
    call AndLinkStatusMask
endm

LINK_STATUS_MASK equ 0

;; InitializeLink:
;;
;; Set the link port status mask and control values based on the
;; hardware version.  Note that the LINK_RESET value is C0h for both
;; hardware versions.
;;
;; Destroys:
;; - Interrupts disabled
;; - AF, DE, HL

InitializeLink:
	di
	ld a, LINK_RESET
	out (linkPort), a

	ld bc, 0FE03h
	ld hl, 0E8D4h
	in a, (linkPort)
	and 0Ch
	jr z, InitializeLink_Old
	ld bc, 0F80Ch
	ld hl, 0C2C1h
InitializeLink_Old:
	ld a, c
	ld (AndLinkStatusMask + 1), a
	ld a, b
	ld (LinkGetByte_CheckStatus + 1), a
	ld (linkD0L), hl
	ret

endif ; TI82

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
AndLinkStatusMask:
	and LINK_STATUS_MASK
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
;; - ROM bank number [TI83]

LinkGetByte:
	di
	ld a, LINK_RESET
	out (linkPort), a

	ld b, 8
LinkGetByte_Loop:
	ld d, 0FFh
	call LinkWaitEither
	ret c

LinkGetByte_CheckStatus:
	add a, -(LINK_STATUS_MASK & 0AAh)
	;; NC, NZ if "D0" bit set
	;; C, Z if "D1" bit set
	;; C, NZ if both bits set

	LD_A_LINK_D1L
	jr nc, LinkGetByte_0	; ~status < D1 -> zero bit
	ret nz			; ~status > D1 -> error
	LD_A_LINK_D0L		; ~status = D1 -> one bit
LinkGetByte_0:
	out (linkPort), a
	rr c
	inc d			; -> D = 0
	call LinkWaitEither
	ret c

	ld a, LINK_RESET
	out (linkPort), a
	djnz LinkGetByte_Loop
	ret

;; LinkWaitEither:
;;
;; Wait until link port state does not equal D.
;;
;; Input:
;; - A = link port control value
;; - D = existing status value
;;
;; Output:
;; - Carry flag set if timeout reached
;; - A = input bits xor D
;;
;; Destroys:
;; - AF, HL

LinkWaitEither:
	ld hl, BIT_TIMEOUT
LinkWaitEither_Loop:
	dec hl
	in a, (linkPort)
	xor d
	AND_LINK_STATUS_MASK
	ret nz
	ld a, h
	or l
	jr nz, LinkWaitEither_Loop
	scf
	ret

;; LinkPutByte:
;;
;; Write a byte to the link port.
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
;; - ROM bank number [TI83]

LinkPutByte:
	di
	ld b, 8
LinkPutByte_Loop:
	call LinkSetWaitBothHigh
	ret c
	rr c
	LD_A_LINK_D0L
	jr nc, LinkPutByte_0
	LD_A_LINK_D1L
LinkPutByte_0:
	inc d			; -> D = 0
	call LinkSetWaitBoth
	ret c
	djnz LinkPutByte_Loop
LinkSetWaitBothHigh:
	ld a, LINK_RESET
	ld d, 0FFh
	;; fall through

;; LinkSetWaitBoth:
;;
;; Write a value to link port, then wait until line state equals D.
;;
;; Input:
;; - A = link port control value
;; - D = desired status value
;;
;; Output:
;; - Carry flag set if timeout reached
;;
;; Destroys:
;; - AF, HL

LinkSetWaitBoth:
	out (linkPort), a
	ld hl, BIT_TIMEOUT
LinkSetWaitBoth_Loop:
	dec hl
	in a, (linkPort)
	xor d
	AND_LINK_STATUS_MASK
	ret z
	ld a, h
	or l
	jr nz, LinkSetWaitBoth_Loop
	scf
	ret
