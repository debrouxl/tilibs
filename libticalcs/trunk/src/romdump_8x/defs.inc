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

defaultMemPage    equ safe_mem			; 1  Original ROM page
defaultLinkState  equ defaultMemPage + 1	; 1  Original link port value (83)
linkD0L	          equ defaultLinkState		; 1  Link port value for a 0 (82)
linkD1L           equ linkD0L + 1		; 1  Link port value for a 1 (82)

errorCount        equ linkD1L + 1		; 2  Count of errors
blockCount        equ errorCount + 2		; 2  Count of blocks transferred

sendPacketCmd     equ blockCount + 2		; 2  Command to send
sendPacketLength  equ sendPacketCmd + 2		; 2  Length of packet to send
sendPacketDataPtr equ sendPacketLength + 2	; 2  Pointer to data to send
sendPacketData    equ sendPacketDataPtr + 2	; 4  Buffer for data to send

packetChecksum    equ sendPacketData + 4	; 2  Checksum of packet
recPacketCmd      equ packetChecksum + 2	; 2  Last command received
recPacketLength   equ recPacketCmd + 2		; 2  Length of last packet received
recPacketData	  equ recPacketLength + 2	; 64 Data received in last packet
