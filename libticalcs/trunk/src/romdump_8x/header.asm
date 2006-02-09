;;; -*- TI-Asm -*-

#define DESCRIPTION "ROM Dump"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-73 (Mallard)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI73
#define FLASH

#define romcall(x) rst 28h \ .dw x
_ClrLCDFull  = 44A7h
_RunIndicOff = 44d7h
_PutC        = 446Bh
_PutS        = 4471h
_DispHL      = 446Eh
#define GET_CSC romcall(400Fh)

curRow  = 83E2h
safeRAM = 8613h 		; appBackUpScreen

#define GET_ROM_PAGE in a,(6)
#define SET_ROM_PAGE out (6),a

bport = 0
D0D1_bits = 00000011b
D0HD1H    = 00000000b
D0LD1H    = 00000001b
D0HD1L    = 00000010b
D0LD1L    = 00000011b

EXIT_KEY = 37h
#define EXIT_KEY_STR "\301MODE]"

	.org 9C37h
	.db 0D9h,0
	.db "Duck"
	.dw ProgStart
	.db DESCRIPTION,0
ProgStart:

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-82 (SNG)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI82
#define TI82_VERSION_UNKNOWN

#define romcall(x) call 8D74h \ .dw x
_ClrLCDFull  = 389Ah
_RunIndicOff = 3918h
_PutC        = 39D2h
_PutS        = 38FAh
_DispHL      = 387Ch
#define GET_CSC romcall(01D4h)

curRow  = 800Ch
safeRAM = 8228h			; saveSScreen

#define GET_ROM_PAGE call GetCurrentROMPage
#define SET_ROM_PAGE call SetCurrentROMPage

bport = 0
D0D1_bits = 00000011b
D0HD1H    = 11000000b
;;; other control values depend on hw version
#ifdef TI82_VERSION_UNKNOWN
D0LD1H = 0DEh
D0HD1L = 0ADh
D0LD1L = 82h
#endif
#ifdef TI82_VERSION_19_0
D0LD1H = 11010100b
D0HD1L = 11101000b
D0LD1L = 11111100b
#endif
#ifdef TI82_VERSION_19_006
D0LD1H = 11000001b
D0HD1L = 11000010b
D0LD1L = 11000011b
#endif

EXIT_KEY = 37h
#define EXIT_KEY_STR "\301MODE]"

	.org 941Eh
	.db "\07313p941343\004\\K\02019+34?"    ; e13+941343->a(19,34
	.db "r\004\\K\02019+3?"                 ; Ans->a(19,3
	.db "\3401+1+*?"                        ; Output(1,1,"
	.db "\331?"                             ; Stop
	.db DESCRIPTION,0

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-83 (Squish / Ion)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI83

#define romcall(x) call x
_ClrLCDFull  = 4755h
_RunIndicOff = 4795h
_PutC        = 4705h
_PutS        = 470Dh
_DispHL      = 4709h
#define GET_CSC romcall(4014h)

curRow  = 800Ch
safeRAM = 8265h			; saveSScreen

#define GET_ROM_PAGE call GetCurrentROMPage
#define SET_ROM_PAGE call SetCurrentROMPage

bport = 0
D0D1_bits = 00001100b
D0HD1H    = 11000000b
D0LD1H    = 11000001b
D0HD1L    = 11000010b
D0LD1L    = 11000011b

EXIT_KEY = 37h
#define EXIT_KEY_STR "\301MODE]"

	.org 9327h
	xor a
	jr nc,ProgStart
	.db DESCRIPTION,0
ProgStart:

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-83+
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI83P
#define FLASH

#define romcall(x) rst 28h \ .dw x
_ClrLCDFull  = 4540h
_RunIndicOff = 4570h
_PutC        = 4504h
_PutS        = 450Ah
_DispHL      = 4507h
_SendAByte   = 4EE5h
_RecAByteIO  = 4F03h
_GetHWVer    = 80BAh
#define GET_CSC romcall(4018h)

APP_PUSH_ERRORH = 59h
APP_POP_ERRORH = 5Ch

curRow  = 844Bh
safeRAM = 9872h			; appBackUpScreen

#define GET_ROM_PAGE in a,(6)
#define SET_ROM_PAGE out (6),a

bport = 0
D0D1_bits = 00000011b
D0HD1H    = 00000000b
D0LD1H    = 00000001b
D0HD1L    = 00000010b
D0LD1L    = 00000011b

dbusEnablePort = 8
dbusStatusPort = 9

EXIT_KEY = 37h
#define EXIT_KEY_STR "\301MODE]"

	.org 9D93h
	.db 0BBh,6Dh
	xor a
	jr nc,ProgStart
	.db DESCRIPTION,0
ProgStart:
	in a,(2)
	and 80h
	rlca
	out (20h),a

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-85
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI85

#define romcall(x) call 8C09h \ .db x
_ClrLCDFull  = 08h
_RunIndicOff = 0Ch
_PutC        = 00h
_PutS        = 0Bh
_DispHL      = 09h
#define GET_CSC call 01BEh
#define zjump(x) call 8C27h \ .dw x-Description

curRow      = 800Ch
safeRAM     = 80DFh		; textShadow
relocMem    = 8641h		; plotSScreen
programAddr = 8C3Ch

#define GET_ROM_PAGE in a,(5)
#define SET_ROM_PAGE out (5),a

bport = 7
D0D1_bits = 00000011b
D0HD1H    = 11000000b
D0LD1H    = 11010100b
D0HD1L    = 11101000b
D0LD1L    = 11111100b

EXIT_KEY = 37h
#define EXIT_KEY_STR "[EXIT]"

	.org 0
	.db 0,0FDh
	.db DescriptionLength
Description:
	.db DESCRIPTION
DescriptionLength = $-Description
	.db 0

	;; Move non-PI code to safe RAM
	ld hl,(programAddr)
	ld de,RelocStart-Description
	add hl,de
	ld de,relocMem
	ld bc,RelocSize
	ldir
	set 0,(iy+3)
	res 1,(iy+13)

#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TI-86
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef TI86

#define romcall(x) call x
_ClrLCDFull  = 4A7Eh
_RunIndicOff = 4AB1h
_PutC        = 4A2Bh
_PutS        = 4A37h
_DispAHL     = 4A33h
#define GET_CSC romcall(5371h)

curRow  = 0C00Fh
safeRAM = 0C0F9h		; textShadow

#define GET_ROM_PAGE in a,(5)
#define SET_ROM_PAGE out (5),a

bport = 7
D0D1_bits = 00000011b
D0HD1H    = 11000000b
D0LD1H    = 11010100b
D0HD1L    = 11101000b
D0LD1L    = 11111100b

EXIT_KEY = 37h
#define EXIT_KEY_STR "[EXIT]"

	.org 0D746h
	.db 8Eh,28h
	nop
	jp ProgStart
	.dw 0
	.dw Description
Description:
	.db DESCRIPTION,0
ProgStart:
	res 1,(iy+13)

#endif


