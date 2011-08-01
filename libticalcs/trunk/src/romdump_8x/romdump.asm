;;; -*- TI-Asm -*-

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73/82/83/83+/85/86 ROM Dumper
;;;
;;; Copyright (c) 2005 Benjamin Moody
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
;;; along with this program; if not, write to the Free Software Foundation,
;;; Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#include "header.asm"

#define CMD_PING 0AA55h
#define CMD_NOK 0
#define CMD_OK 1
#define CMD_EOT 2
#define CMD_SIZE 3
#define CMD_RETRY 4
#define CMD_REQ 5
#define CMD_DATA 6
#define CMD_REPEAT 7

BLOCK_SIZE = 1024
TIMEOUT = 65535

exitSP            = safeRAM	; 2  Saved stack pointer
exitROMPage       = safeRAM+2	; 1  Saved ROM page
errorCount        = safeRAM+3	; 2  Count of errors
blockCount        = safeRAM+5	; 2  Count of blocks transferred
sendPacketCmd     = safeRAM+7	; 2  Command to send
sendPacketLength  = safeRAM+9	; 2  Length of packet to send
sendPacketDataPtr = safeRAM+11	; 2  Pointer to data to send
sendPacketData    = safeRAM+13	; 4  Buffer for data to send
recPacketCmd      = safeRAM+17	; 2  Last command received
recPacketLength   = safeRAM+19	; 2  Length of last packet received
recPacketData     = safeRAM+21	; 64 Data received in last packet

;;; Initialization

#ifdef TI83P
	;; Set a system error handler on 83+
	ld hl,Error
	call APP_PUSH_ERRORH
#endif

	;; Save original SP and ROM page so that we can return there
	;; to exit or when an error occurs.
	GET_ROM_PAGE
	ld (exitROMPage),a
	ld (exitSP),sp

#ifdef TI82_VERSION_UNKNOWN
	call TI82LinkSetup
#endif

	ld hl,0
	ld (errorCount),hl
	ld (blockCount),hl

	romcall(_RunIndicOff)
	romcall(_ClrLCDFull)
	ld hl,0
	ld (curRow),hl
	ld hl,DumpingROMString
	romcall(_PutS)
	ld de,4
	ld (curRow),de
	romcall(_PutS)
	
	jr MainLoop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Main Loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Error:	ld hl,7
	ld (curRow),hl
	ld hl,ErrorString
	romcall(_PutS)
	ld hl,(errorCount)
	inc hl
	ld (errorCount),hl
	romcall(_DispHL)
#ifdef TI83P
	;; Error handler is removed when it's called
	ld hl,Error
	call APP_PUSH_ERRORH
#endif
MainLoop:
	ei
	halt
	GET_CSC
	cp EXIT_KEY
	jr z,Quit

	;; Check for link activity

#ifdef TI83P
	;; Check HW assist on 83+ SE/84+
	in a,(2)
	and 80h
	jr z,MainLoop_NoDBUS
	in a,(dbusStatusPort)
	and 18h
	jr nz,ReceivedLink
MainLoop_NoDBUS:
#endif

	;; Check lines
	in a,(bport)
	and D0D1_bits
	xor D0D1_bits
	jr z,MainLoop

ReceivedLink:
	call ReceivePacket
	jr nz,Error

	;; What command did we receive?
	ld de,(recPacketCmd)
	ld hl,CMD_PING
	or a
	sbc hl,de
	jr z,ReceivedPing

	ld a,d
	or a
	jr nz,ReceivedBadCommand

	ld a,e
	or a
	jr z,ReceivedBadCommand	; !OK
	dec a
	jr z,MainLoop		; OK
	dec a
	jr z,ReceivedEOT	; EOT
	dec a
	jr z,ReceivedSizeReq	; SIZE
	dec a
	jr z,ReceivedError	; RETRY
	dec a
	jr z,ReceivedBlockReq	; REQ
ReceivedBadCommand:
;	ld hl,CMD_RETRY
;	call SendShortPacket
	jr Error

ReceivedEOT:
	ld hl,CMD_EOT
	call SendShortPacket
Quit:
	;; Restore original SP and ROM page
	ld sp,(exitSP)
	ld a,(exitROMPage)
	SET_ROM_PAGE
#ifdef TI83P
	;; Remove error handler
	ld hl,Error
	call APP_POP_ERRORH
	;; Set CPU back to 6 MHz
	xor a
	out (20h),a
#endif
#ifdef FLASH
	;; Re-protect Flash
	ld c,0
	call TryFlashWE
#endif
#ifdef TI86
	set 1,(iy+13)
	ld hl,safeRAM
	ld de,safeRAM+1
	ld (hl),' '
	ld bc,167
	ldir
#endif
	ld hl,0
	ld (curRow),hl
	ret


ReceivedPing:
	;; ping: reply OK
	ld hl,CMD_OK
	call SendShortPacket
	jr MainLoop

ReceivedError:
	;; error: re-send packet
	ld hl,(sendPacketCmd)
	call SendPacket
MainLoop1:
	jr MainLoop

ReceivedSizeReq:
	;; size: reply with 32 bit size of ROM
	ld hl,ROMSize
	ld (sendPacketDataPtr),hl
#ifdef TI83P
	;; Check HW version
	romcall(_GetHWVer)
	or a
	ld hl,512/64		; version 0 -> 83+, 512 kB
	jr z,ReceivedSizeReq_SetHW
	cp 2
	ld hl,1024/64		; version 2 -> 84+, 1024 kB
	jr z,ReceivedSizeReq_SetHW
	ld hl,2048/64		; version 1/3 -> 83+/84+ SE, 2048 kB
ReceivedSizeReq_SetHW:
	ld (ROMSize+2),hl
#endif
	ld hl,4
	ld (sendPacketLength),hl
	ld hl,CMD_SIZE
	call SendPacket

	ld hl,0402h
	ld (curRow),hl
	ld hl,KString
	romcall(_PutS)
#ifdef TI83P
	ld hl,(ROMSize+1)
	srl h
	srl h
	romcall(_DispHL)
	ld a,'K'
	romcall(_PutC)
#endif
MainLoop2:
	jr MainLoop1

ReceivedBlockReq:
	;; block request: reply with 1024-byte block
#ifdef FLASH
	;; Unlock read-protected Flash sectors if possible
	ld c,1
	call TryFlashWE
#endif

	ld hl,(recPacketData)
	ld a,(recPacketData+2)
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
	ld (sendPacketDataPtr),hl

	;; Check if the block is empty
	ld d,(hl)
	ld bc,BLOCK_SIZE
ReceivedBlockReq_Loop:
	ld a,b
	or c
	jr z,ReceivedBlockReq_Empty
	ld a,(hl)
	cp d
	jr nz,ReceivedBlockReq_NotEmpty
	inc hl
	dec bc
	jr ReceivedBlockReq_Loop

ReceivedBlockReq_Empty:
	;; Empty block, send a REPEAT packet
	ld e,d
	ld (sendPacketData+2),de
	ld hl,BLOCK_SIZE
	ld (sendPacketData),hl
	ld hl,sendPacketData
	ld (sendPacketDataPtr),hl
	ld hl,4
	ld de,CMD_REPEAT
	jr ReceivedBlockReq_SetPacket

ReceivedBlockReq_NotEmpty:
	ld hl,BLOCK_SIZE
	ld de,CMD_DATA
ReceivedBlockReq_SetPacket:
	ld (sendPacketLength),hl
	ex de,hl
	call SendPacket

	ld a,(exitROMPage)
	SET_ROM_PAGE

	ld hl,2
	ld (curRow),hl
	ld hl,(blockCount)
	inc hl
	ld (blockCount),hl
	romcall(_DispHL)
	ld a,'K'
	romcall(_PutC)

	jr MainLoop2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Memory access
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI82

GetCurrentROMPage:
	in a,(2)
	and 07h
	ret

SetCurrentROMPage:
	and 07h
	or 88h
	out (2),a
	ret

#endif

#ifdef TI83

GetCurrentROMPage:
	in a,(2)
	and 07h
	push bc
	 ld b,a
	 in a,(0)
	 and 10h
	 rrca
	 or b
	 pop bc
	ret

SetCurrentROMPage:
	push af
	 and 07h
	 or 88h
	 out (2),a
	 pop af
	and 08h
	rlca
	or 0C0h
	out (0),a
	ret

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; FOLLOWING CODE IS RELOCATED ON TI-85
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RelocStart:

#ifdef TI85
RELOC_OFF = relocMem - RelocStart
#else
RELOC_OFF = 0
#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Packet communication
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ReceivePacket = $+RELOC_OFF
	;; Return NZ on protocol error

	;; Command
	call GetWord
	ld (recPacketCmd),hl
	;; Length
	ex de,hl
	call GetWord
	ld (recPacketLength),hl
	;; Check for valid length
	ld a,h
	or a
	jr nz,_ReceivePacket_TooLong
	ld a,l
	cp 65
	jr nc,_ReceivePacket_TooLong
	;; Checksum of bytes received so far
	push hl
	 ld c,h
	 ld h,0
	 ld b,h
	 add hl,bc
	 ld c,d
	 add hl,bc
	 ld c,e
	 add hl,bc
	 pop bc
	ld de,recPacketData
_ReceivePacket_Loop:
	;; BC = bytes remaining
	;; HL = checksum
	ld a,b
	or c
	jr z,_ReceivePacket_Done
	call GetByte
	call AddToChecksum
	ld (de),a
	inc de
	dec bc
	jr _ReceivePacket_Loop
_ReceivePacket_Done:
	;; Checksum
	push hl
	 call GetWord
	 pop de
	or a
	sbc hl,de
	ret z
_ReceivePacket_Retry:
;	ld hl,CMD_RETRY
;	call SendShortPacket
;	or 1
	ret
_ReceivePacket_TooLong:
	call SetError
	jr _ReceivePacket_Retry


SendShortPacket = $+RELOC_OFF
	;; Send packet with length = 0
	push hl
	 ld hl,0
	 ld (sendPacketLength),hl
	 pop hl
SendPacket = $+RELOC_OFF
	;; HL = command
	ld (sendPacketCmd),hl
	push hl
	 call SendWord
	 ld hl,(sendPacketLength)
	 call SendWord
	 ld c,h
	 ld h,0
	 ld b,h
	 add hl,bc
	 pop de
	ld c,d
	add hl,bc
	ld c,e
	add hl,bc
	ld de,(sendPacketDataPtr)
	ld bc,(sendPacketLength)
_SendPacket_Loop:
	;; BC = bytes remaining
	;; HL = checksum
	ld a,b
	or c
	jr z,_SendPacket_Done
	ld a,(de)
	call AddToChecksum
	call SendByte
	inc de
	dec bc
	jr _SendPacket_Loop
_SendPacket_Done:
	;; Send checksum
SendWord = $+RELOC_OFF
	ld a,l
	call SendByte
	ld a,h
	jp SendByte

GetWord = $+RELOC_OFF
	call GetByte
	ld l,a
	call GetByte
	ld h,a
	ret

AddToChecksum = $+RELOC_OFF
	push bc
	 ld c,a
	 ld b,0
	 add hl,bc
	 pop bc
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Low level
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Pull link lines low briefly to flag an error
SetError = $+RELOC_OFF
#ifdef TI83P
	in a,(2)
	and 80h
	out (dbusEnablePort),a
	ld a,D0LD1L
	out (bport),a
	ei
	halt
	halt
	di
	xor a
	out (bport),a
	out (dbusEnablePort),a
	ret
#else
#ifdef TI83
	GET_ROM_PAGE
	push af
#endif
TI82_FixD0LD1L_1 = $+1+RELOC_OFF
	 ld a,D0LD1L
	 out (bport),a
	 ei
	 halt
	 halt
	 di
	 ld a,D0HD1H
	 out (bport),a
#ifdef TI83
	pop af
	SET_ROM_PAGE
#endif
	ret
#endif

;;; Get a byte (may throw error)
GetByte = $+RELOC_OFF
#ifdef TI83P
	push hl
	 push de
	  push bc
	   romcall(_RecAByteIO)
	   pop bc
	  pop de
	 pop hl
	ret
#else
	di
	push bc
	 push hl
#ifdef TI83
	  ;; Save bit 4 (which controls the ROM page)
	  in a,(bport)
	  and 10h
	  push af
#endif
	   ld bc,0800h		; B = Count of bits remaining
				; C = Bits we have stored so far
_GetByteLoop:
	   ld hl,TIMEOUT
_GetByteWaitLoop1:
	   ;; Wait for one of the lines to go low
	   ld a,h
	   or l
	   jr z,_LinkError
	   dec hl
	   in a,(bport)
	   and D0D1_bits
	   jr z,_LinkError	; both lines low, this is an error
	   cp D0D1_bits
	   jr z,_GetByteWaitLoop1

	   ;; check which line went low
	   cp 2*(D0D1_bits/3)
	   ;; D0D1_bits  =  ...11...
	   ;; line 1 low -> ...01...  (carry set)
	   ;; line 0 low -> ...10...  (zero set)
TI82_FixD0HD1L_1 = $+1+RELOC_OFF
	   ld a,D0HD1L		; line 0 pulled low, we set line 1 low
	   jr z,_GetByteZeroBit
TI82_FixD0LD1H_1 = $+1+RELOC_OFF
	   ld a,D0LD1H		; line 1 pulled low, we set line 0 low
_GetByteZeroBit:
	   rr c			; rotate in a 1 bit if line 1 was low
	   out (bport),a
_GetByteWaitLoop2:
	   ;; Now both lines have been pulled low; wait for one to return to high
	   ld a,h
	   or l
	   jr z,_LinkError
	   dec hl
	   in a,(bport)
	   and D0D1_bits
	   jr z,_GetByteWaitLoop2
	   ;; bring our line high again and continue
	   ld a,D0HD1H
	   out (bport),a
	   djnz _GetByteLoop
#ifdef TI83
	   pop af
	  or D0HD1H
	  out (bport),a
#endif
	  ld a,c
	  pop hl
	 pop bc
	ret

_LinkError:
	ld sp,(exitSP)
	ld a,(exitROMPage)
	SET_ROM_PAGE
#ifdef TI85
	zjump(Error)
#else
	jp Error
#endif
#endif

;;; Send a byte (may throw error)
SendByte = $+RELOC_OFF
#ifdef TI83P
	push hl
	 push de
	  push bc
	   push af
	    romcall(_SendAByte)
	    pop af
	   pop bc
	  pop de
	 pop hl
	ret
#else
	di
	push bc
	 push hl
	  push af
	   ld c,a
#ifdef TI83
	   in a,(bport)
	   and 10h
	   push af
#endif
	    ld b,8
_SendByteLoop:
	    ld hl,TIMEOUT
_SendByteWaitLoop1:
	    ;; Wait for both lines to go high
	    ld a,h
	    or l
	    jr z,_LinkError
	    dec hl
	    in a,(bport)
	    and D0D1_bits
	    cp D0D1_bits
	    jr nz,_SendByteWaitLoop1
	    ;; rotate out a bit
	    rr c
TI82_FixD0LD1H_2 = $+1+RELOC_OFF
	    ld a,D0LD1H		; zero bit, pull line 0 low
	    jr nc,_SendByteZeroBit
TI82_FixD0HD1L_2 = $+1+RELOC_OFF
	    ld a,D0HD1L		; one bit, pull line 1 low
_SendByteZeroBit:
	    out (bport),a
_SendByteWaitLoop2:
	    ;; Wait for other device to pull the other line low
	    ld a,h
	    or l
	    jr z,_LinkError
	    dec hl
	    in a,(bport)
	    and D0D1_bits
	    jr nz,_SendByteWaitLoop2
	    ;; Bring our line high again and continue
	    ld a,D0HD1H
	    out (bport),a
	    djnz _SendByteLoop

_SendByteWaitLoop3:
	    ;; Wait for both lines to go high again
	    in a,(bport)
	    and D0D1_bits
	    cp D0D1_bits
	    jr nz,_SendByteWaitLoop3

#ifdef TI83
	    pop af
	   or D0HD1H
	   out (bport),a
#endif
	   pop af
	  pop hl
	 pop bc
	or a
	ret

#endif


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Platform-dependent junk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI82_VERSION_UNKNOWN

;;; Fix link output values

;;; calculators with ROM 19.0 (and all earlier known versions)
;;; D0LD1H = 11010100b
;;; D0HD1L = 11101000b
;;; D0LD1L = 11111100b

;;; calculators with ROM 19.006
;;; D0LD1H = 11000001b
;;; D0HD1L = 11000010b
;;; D0LD1L = 11000011b

TI82LinkSetup:
	ld a,(29D0h)		; check for version 19.006
	cp 18h
	ld bc,1100000111000010b	; B = D0LD1H, C = D0HD1L
	jr z,TI82LinkSetup_19_006
	ld bc,1101010011101000b
TI82LinkSetup_19_006:
	ld a,b
	ld (TI82_FixD0LD1H_1),a
	ld (TI82_FixD0LD1H_2),a
	ld a,c
	ld (TI82_FixD0HD1L_1),a
	ld (TI82_FixD0HD1L_2),a
	or b
	ld (TI82_FixD0LD1L_1),a
	ret
#endif

#ifdef FLASH

;;; Flash write-enabling for 73/83+

;;; This code will look for a Flash WE routine at 7FF0 on the last
;;; page of the OS (page 1D/3D/7D.)  The code must be exactly as
;;; follows:

FlashWECode:
	nop
	nop
	im 1
	di
	out (14h),a
	ret

;;; The corresponding line of Intel hex:
;;; :207FE000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000ED56F3D314C9FFFFFFFFFFFFFFFFB3

TryFlashWE:
	in a,(6)
	push af
#ifdef TI73
	 ld a,1Dh
#else
	 romcall(_GetHWVer)
	 ld b,1Dh
	 or a
	 jr z,TryFlashWE_Page
	 ld b,3Dh
	 cp 2
	 jr z,TryFlashWE_Page
	 ld b,7Dh
TryFlashWE_Page:
	 ld a,b
#endif
	 out (6),a
	 ld hl,FlashWECode
	 ld de,7FF0h
	 ld b,8
TryFlashWE_Loop:
	 ld a,(de)
	 cp (hl)
	 jr nz,TryFlashWE_NoPatch
	 inc hl
	 inc de
	 djnz TryFlashWE_Loop
	 ;; The patch is installed... let's use it!
	 ld a,c
	 di
	 call 7FF0h
	 ei
TryFlashWE_NoPatch:
	 pop af
	out (6),a
	ret

#endif

;;; DispHL for TI-86

#ifdef TI86
_DispHL:
	xor a
	jp _DispAHL
#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Data
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DumpingROMString = $+RELOC_OFF
	.db "Dumping ROM...",0
	.db EXIT_KEY_STR
	.db " to cancel",0

ErrorString = $+RELOC_OFF
	.db "Errors:",0

KString = $+RELOC_OFF
	.db "0K of "

#ifdef TI73
	.db "512K"
ROMSize:
	.dw 0000h,0008h
#endif
#ifdef TI82
	.db "128K"
ROMSize:
	.dw 0000h,0002h
#endif
#ifdef TI83
	.db "256K"
ROMSize:
	.dw 0000h,0004h
#endif
#ifdef TI83P
ROMSize:
	.dw 0000h,1234h
#endif
#ifdef TI85
	.db "128K"
ROMSize = $+RELOC_OFF
	.dw 0000h,0002h
#endif
#ifdef TI86
	.db "256K"
ROMSize:
	.dw 0000h,0004h
#endif

RelocSize = $-RelocStart

.end
END
