;;; -*- TI-Asm -*-
#include "ti83plus.inc"
#include "extraequ.inc"

page = appBackUpScreen
addr = appBackUpScreen+1
count = appBackUpScreen+3
maxcount = appBackUpScreen+5

        .org $9d93
        .db $bb,$6d
        xor a
        jr nc,Start
        .db "ROM Dump",0

Start:        in a,(6)
        push af

        ld c,1                        ; If user has a patched OS, we can
        call FlashOut                ; also dump the certificate :)

        B_CALL _ClrLCDFull
        B_CALL _HomeUp

        in a,(2)
        and $80
        jr z,SkipSEInit
        rlca
        out ($20),a
#ifdef SE_USE_DBUS
        xor a
#else
        ld a,$80
#endif
        out (8),a
SkipSEInit:
        xor a
        ld (page),a
        ld hl,$4000
        ld (addr),hl
        ld hl,0
        ld (count),hl

        in a,(2)
        and $80
        jr z,TI83p_BE
        in a,($21)
        and 1
        jr z,TI84p_BE
TI84p_SE:
TI83p_SE:
        ld hl,2048
        jr SetHW
TI84p_BE:
        ld hl,1024
        jr SetHW
TI83p_BE:
        ld hl,512
SetHW:
        ld (maxcount),hl

Loop:        ld hl,3
        ld (curRow),hl
        ld hl,(count)
        push hl
        B_CALL _DispHL
        ld hl,Str1
        B_CALL _PutS
        ld hl,(maxcount)
        push hl
        B_CALL _DispHL
        ld a,'K'
        B_CALL _PutC

        pop de
        pop hl
        or a
        sbc hl,de
        jr nc,Done

        ld a,(page)
        out (6),a
        ld de,(addr)
        ld bc,4
        ld hl,0
SendBytesLoop:
        push bc
        ld a,(de)
        inc de
        ld c,a
        ld b,0
        add hl,bc
        call Send
        pop bc
        djnz SendBytesLoop
        dec c
        jr nz,SendBytesLoop

        ld a,h
        call Send
        jr c,DispLinkError
        ld a,l
        call Send
        jr c,DispLinkError

        bit 7,d
        jr z,NoPageFlip
        res 7,d
        set 6,d
        ld a,(page)
        inc a
        ld (page),a
NoPageFlip:
        ld (addr),de

        ld hl,(count)
        inc hl
        ld (count),hl

        call Get
        jr c,DispLinkError
        cp $DA
        jr z,Loop
;        ld l,a
;        ld h,0
;        B_CALL _DispHL
        
        B_CALL _HomeUp
        ld hl,BadReceiveStr
        jr Error
DispLinkError:
        B_CALL _HomeUp
        ld hl,LinkErrorStr
Error:
        in a,(4)
        and 8
        jr z,Error
        B_CALL _PutS
        res onInterrupt,(iy+onFlags)
Quit:        ld a,$80
        out (8),a
        xor a
        out (0),a
        ld c,0
        call FlashOut
        pop af
        out (6),a
        ret

Done:        B_CALL _ClrScrnFull
        B_CALL _HomeUp
        jr Quit

;;; Send - byte in A; preserve HL, DE; carry on error
Send:
#ifdef SE_USE_DBUS
        push af
         in a,(2)
         and $80
         jr nz,SendSE
         pop af
#endif
        ld c,a
        ld b,8
SendLoop:
        in a,(4)
        and 8
        jr z,LinkError
        in a,(0)
        or $fc
        inc a
        jr nz,SendLoop
        rr c
        adc a,1
        out (0),a
SendWaitLoop:
        in a,(4)
        and 8
        jr z,LinkError
        in a,(0)
        and 3
        jr nz,SendWaitLoop
        out (0),a
        djnz SendLoop
        or a
        ret

#ifdef SE_USE_DBUS
;;; Status bits on port 9:
;;;  Bit 7: apparently unused
;;;  Bit 6: set if the DBUS detected an error
;;;  Bit 5: set if the DBUS is ready
;;;  Bit 4: set if the DBUS has received and stored a byte since the
;;;            last input from port A
;;;  Bit 3: set if the DBUS is currently in the process of receiving
;;;  Bit 2: apparently unused
;;;  Bit 1: apparently unused
;;;  Bit 0: function unknown

SendSE:         ;; Note that any error condition will not be caught until the
         ;; next time around...
         in a,(4)
         and 8
         jr z,LinkErrorPop
         in a,(9)
         bit 5,a
         jr nz,SendSE_OK
         and $58
         jr z,SendSE
         jr LinkError
SendSE_OK:
         pop af
        out ($D),a
        or a
        ret
LinkErrorPop:
         pop af
#endif
LinkError:
;        ld hl,6
;        ld (curRow),hl
;        ld h,0
;        in a,(9)
;        ld l,a
;        B_CALL _DispHL
        scf
        ret


;;; Get - byte returned in A; carry on error
Get:
#ifdef SE_USE_DBUS
        in a,(2)
        and $80
        jr nz,GetSE
#endif
        ld c,0
        ld b,8
GetLoop:
        in a,(4)
        and 8
        jr z,LinkError
        in a,(0)
        and 3
        jr z,LinkError
        cp 3
        jr z,GetLoop
        out (0),a
        rra
        rr c
GetWaitLoop:
        in a,(4)
        and 8
        jr z,LinkError
        in a,(0)
        and 3
        jr z,GetWaitLoop
        xor a
        out (0),a
        djnz GetLoop
        ld a,c
        ret

#ifdef SE_USE_DBUS
GetSE:        in a,(4)
        and 8
        jr z,LinkError
        in a,(9)
        bit 6,a
        jr nz,LinkError
        and $11
        jr z,GetSE
        in a,($A)
        ret
#endif

FlashOut:
        ;; C = 1 to write-enable, 0 to disable
        in a,(2)
        and $80
        ld a,$1d
        jr z,FlashOutPage
        in a,($21)
        and 1
        ld a,$3d
        jr z,FlashOutPage
        ld a,$7d
FlashOutPage:
        out (6),a
        ld hl,UnlockCode
        ld de,$7FF0
        ld b,8
FlashOutCompareLoop:
        ld a,(de)
        cp (hl)
        ret nz
        inc hl
        inc de
        djnz FlashOutCompareLoop
        ;; The patch is installed... let's use it!
        ld a,c
        di
        call $7FF0
        ei
        ret

UnlockCode:
        nop
        nop
        im 1
        di
        out ($14),a
        ret

Str1:        .db "K of",0

LinkErrorStr:
        .db "Link error",0

BadReceiveStr:
        .db "Checksum error",0

.end