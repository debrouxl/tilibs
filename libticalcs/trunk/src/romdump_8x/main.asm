;;; -*- TI-Asm -*-

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73/82/83/83+/84+/85/86 ROM Dumper
;;;
;;; Copyright (c) 2012-2013 Benjamin Moody
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

Main:
	res apdRunning, (iy + apdFlags)
	in a, (memPort)
	ld (defaultMemPage), a
 if defined TI83
	in a, (linkPort)
	and 10h
	or LINK_RESET
	ld (defaultLinkState), a
 endif
 if defined TI84PC
	in a, (memExtPort)
	ld (defaultMemExt), a
 endif

 if defined TI82
	call InitializeLink
 endif

 if defined CALC_USB
	call InitializeUSB
 endif

 if defined CALC_FLASH
	call Unlock
	ld a, (defaultMemPage)
	out (memPort), a
 endif

 if defined TI83P
	in a, (2)
	add a, a
	jr nc, Main_83PlusBE
	ld a, 1
	out (20h), a
	in a, (21h)
	and 3
	ld a, 10h
	jr z, Main_84PlusBE
	add a, a
Main_84PlusBE:
	ld (ROMSize + 2), a
Main_83PlusBE:
 endif

	ld hl, 0
	ld (errorCount), hl
	ld (blockCount), hl

	RUN_INDIC_OFF
	CLEAR_LCD

	ld hl, 0
	ld (curRow), hl

	ld hl, DumpingROMString
	PUT_STRING

	ld de, 4
	ld (curRow), de
	PUT_STRING

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Main Loop
;;;

MainLoop:
	ei
	halt
	GET_CSC
	cp 37h
	jr z, Quit

	call CheckLinkActivity
	jr z, MainLoop

	call ReceivePacket
	jr c, Main_Error

	;; What command did we receive?
	ld de, (recPacketCmd)
	ld hl, CMD_PING
	or a
	sbc hl, de
	jr z, Received_Ping

	ld a, d
	or a
	jr nz, Main_Error

	ld b, e
	djnz Received_2
	;; 0001: OK, no response needed
	jr MainLoop

Received_Ping:
	;; AA55: reply OK
	ld hl, CMD_OK
	call SendShortPacket
	jr MainLoop

Received_2:
	djnz Received_3
	;; 0002: End of Transmission
	ld hl, CMD_EOT
	call SendShortPacket
Quit:
	CLEAR_LCD
	ld hl, 0
	ld (curRow), hl
	ret

Received_3:
	djnz Received_4
	;; 0003: Request ROM size
	ld hl, ROMSize
	ld (sendPacketDataPtr), hl
	ld hl, 4
	ld (sendPacketLength), hl
	ld hl, CMD_SIZE
	call SendPacket

	ld hl, 0402h
	ld (curRow), hl
	ld hl, KString
	PUT_STRING
 if defined TI83P
	ld hl, (ROMSize + 1)
	srl h
	srl h
	PUT_DECIMAL
	ld a, 'K'
	PUT_CHAR
 endif
MainLoop_:
	jr MainLoop

Received_4:
	djnz Received_5
	;; 0004: deprecated
Main_Error:
	ld hl, 7
	ld (curRow), hl
	ld hl, ErrorString
	PUT_STRING
	ld hl, (errorCount)
	inc hl
	ld (errorCount), hl
	PUT_DECIMAL
 if ! defined USB
	call LinkError
 endif
MainLoop__:
	jr MainLoop_

Received_5:
	djnz Main_Error
	;; 0005: Request data block

	ld hl, (recPacketData)
	ld a, (recPacketData + 2)
	;; A = addr >> 14
	sla h
	rla
	sla h
	rla
	SET_ROM_PAGE
	;; HL = (addr & 3FFFh) | 4000h
	scf
	rr h
	srl h
	ld (sendPacketDataPtr), hl

	;; Check if the block is empty
	call CheckBlockUniform
	jr nz, BlockRequest_NotEmpty

	;; Empty block, send a REPEAT packet
	ld h, a
	ld l, a
	ld (sendPacketData + 2), hl
	ld hl, BLOCK_SIZE
	ld (sendPacketData), hl
	ld hl, sendPacketData
	ld (sendPacketDataPtr), hl
	ld hl, 4
	ld de, CMD_REPEAT
	jr BlockRequest_SetPacket

BlockRequest_NotEmpty:
	ld hl, BLOCK_SIZE
	ld de, CMD_DATA
BlockRequest_SetPacket:
	ld (sendPacketLength), hl
	ex de, hl
	call SendPacket

	;; Restore original memory page
 if defined TI83
	ld a, (defaultLinkState)
	out (linkPort), a
 endif
 if defined TI84PC
	ld a,(defaultMemExt)
	out (memExtPort), a
 endif
	ld a, (defaultMemPage)
	out (memPort), a

	jr c, Main_Error

	ld hl, 2
	ld (curRow), hl
	ld hl, (blockCount)
	inc hl
	ld (blockCount), hl
	PUT_DECIMAL

	ld a, 'K'
	PUT_CHAR

	jr MainLoop__
