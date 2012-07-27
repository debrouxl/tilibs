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

MAX_OS_VERSION equ 0191h	; last supported version: 1.91 (BCD)

;; Unlock:
;;
;; Write-enable Flash.
;;
;; Output:
;; - Carry flag set if unlocking failed
;;
;; Destroys:
;; - AF, BC, DE, HL
;; - Port 6
;; - appBackUpScreen
;; - Interrupts disabled
;; - Interrupt mode set to 1

Unlock:
	;; Check if current OS version is known to be supported
	B_CALL _GetBaseVer
	ld c, b
	ld b, a
	ld hl, MAX_OS_VERSION
	or a
	sbc hl, bc
	ret c

	di
	ld a, 1Ch
	out (6), a
	ld hl, 4000h
	ld b, h
	ld c, l
Unlock_FindTargetLoop:
	ld a, 0F5h
	cpir
	scf
	ret nz
	push hl
	 push bc
	  ld de, Unlock_Data
	  ld b, 12
Unlock_CheckTargetLoop:
	  ld a, (de)
	  cp (hl)
	  jr nz, Unlock_CheckTargetDone
	  inc hl
	  inc de
	  djnz Unlock_CheckTargetLoop
	  inc hl
	  inc hl
	  inc hl
	  add a, a
	  ld b, 4
	  jr nz, Unlock_CheckTargetLoop
Unlock_CheckTargetDone:
	  pop bc
	 pop hl
	jr nz, Unlock_FindTargetLoop
	ld (appBackUpScreen), sp
	dec hl
	push hl			;SETRETURN
	ld hl, Unlock_Cleanup
	ld (appBackUpScreen + 2), hl
	ld hl, appBackUpScreen + 2
	push hl
	 ld de, appBackUpScreen + 4
	 B_CALL _Mov18B
	 ld hl, -16
	 add hl, sp
	 ex de, hl
	 pop hl
	ld bc, 20
	ret
Unlock_Cleanup:
	ld sp, (appBackUpScreen)
	xor a
	ret

Unlock_Data:
    ld a, 1
    nop
    di
    nop
    nop
    im 1
    di
    out (14h), a
    di
    ;; ...
    pop af
    B_CALL _FlashToRAM2
