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

;; CheckBlockUniform:
;;
;; Check whether a data block consists of a single repeated byte
;; value.
;;
;; Input:
;; - HL = address of block
;;
;; Output:
;; - A = byte value at start of block
;; - Zero flag set (Z) if block is uniform
;;
;; Destroys:
;; - F, BC, HL

CheckBlockUniform:
	ld a, (hl)
	ld bc, BLOCK_SIZE
CheckBlockUniform_Loop:
	cpi
	ret po
	jr z, CheckBlockUniform_Loop
	ret

;; ReceivePacket:
;;
;; Receive a packet using the TiLP ROM protocol.
;;
;; Output:
;; - Carry flag set if there was an error
;; - (recPacketCmd) = command
;;
;; Destroys:
;; - Interrupts disabled
;; - AF, BC, DE, HL

ReceivePacket:
	ld hl, 0
	ld de, recPacketCmd
	ld b, 4
	call LinkReceiveB
	ret c

	ld a, (recPacketLength + 1)
	add a, -1
	ret c
	ld a, (recPacketLength)
	or a
	jr z, ReceivePacket_0
	ld b, a
	add a, -65
	ret c

	ld de, recPacketData
	call LinkReceiveB
	ret c

ReceivePacket_0:
	push hl
	 ld de, packetChecksum
	 ld b, 2
	 call LinkReceiveB
	 pop de
	ret c
	ld hl, (packetChecksum)
	sbc hl, de
	ret z
	scf
	ret


SendShortPacket:
	push hl
	 ld hl, 0
	 ld (sendPacketLength), hl
	 pop hl
SendPacket:
	ld (sendPacketCmd), hl
	ld hl, 0
	ld de, sendPacketCmd
	ld bc, 4
	call LinkSendBC
	ret c

	ld de, (sendPacketDataPtr)
	ld bc, (sendPacketLength)
	call LinkSendBC
	ret c

	ld (packetChecksum), hl
	ld de, packetChecksum
	ld bc, 2
	jr LinkSendBC

