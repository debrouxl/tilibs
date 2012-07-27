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

;;; Table of supported OS and boot code versions
VersionTable:
	db 1,19, 1,01		; TI-83 Plus
	db 1,19, 1,00		; TI-83 Plus SE
	db 2,55, 1,03		; TI-84 Plus
	db 2,55, 1,03		; TI-84 Plus SE (odd versions)
	db 2,56, 1,03		; Nspire (even versions)

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
;; - Interrupts disabled
;; - Interrupt mode set to 1

Unlock:
	;; Check if current OS/boot version is known to be supported
	B_CALL _GetBaseVer
	ld c, a
	B_CALL _GetHWVer
	ld l, a
	add a, -4
	ret c
	inc a
	jr nz, Unlock_NotNspire
	bit 0, b
	jr nz, Unlock_NotNspire
	inc l
Unlock_NotNspire:
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, VersionTable
	add hl, de
	ld a, c
	call CheckVersion
	ret c
	inc hl
	B_CALL _GetBootVer
	call CheckVersion
	ret c

	di
	push	iy
	 ld	de, 7B09h
	 ld	a, e
	 ld	(arcInfo), a
	 out	(6), a
	 ld	a, (de)
	 ld	b, a
	 push	bc
	  push	de
	   call	SetFlashPageD
	   ld	a, e
	   B_CALL _WriteAByteSafe
	   ld hl, Unlock_Recover - 2
	   ld de, pagedBuf - 2
	   B_CALL _Mov18B
	   ld	(pagedBuf + 2), sp
	   call	Unlock_Main
	   pop	af
	  pop	bc
	 jr Unlock_Done

Unlock_Main:
	ld	a, (_GetVariableData + 2)
	ld	d, a
	ld	hl, ramCode
	ld	bc, 304
Unlock_Loop1:
	ld	a, 0EDh
	cpir
	ret	nz
	ld	a, (hl)
	sub	0A0h
	jr	nz, Unlock_Loop1
	ld	c, (hl)
	ld	b, a
	inc	hl
	ld	(iMathPtr5), hl
	ld	hl, (_GetVariableData)
	inc	a
Unlock_Data:
	ld	(pagedCount), a
	or	a
	call	nz, SetFlashPageD
Unlock_Loop2:
	ld	a, 3Ah
	cpir
	ret	nz
	ld	de, Unlock_Data + 1
Unlock_Loop3:
	ld	a,(de)
	cp	(hl)
	jr	nz, Unlock_Loop2
	inc	hl
	inc	de
	cp	0C4h
	jr	nz, Unlock_Loop3
	B_CALL	_LdHLind
	ld	iy, 56h - 25h
Unlock_Recover:
	jp	(hl)
	   ld	sp, 0
	   pop	hl
	  pop	af
	 ld	(hl), a
Unlock_WriteByte:
	 cp	(hl)
	 jr	nz, Unlock_WriteByte
	 ld	(hl), 0F0h
Unlock_Done:
	 pop	iy
	ret

SetFlashPageD:
	in a, (2)
	add a, a
	ld a, 1Fh
	jr nc, SetFlashPage_Mask
	in a, (21h)
	and 3
	ld a, 7Fh
	jr nz, SetFlashPage_Mask
	rra
SetFlashPage_Mask:
	and d
	out (6), a
	ret

CheckVersion:
	cp (hl)
	inc hl
	ccf
	ret nz
	ld a, (hl)
	cp b
	ret
