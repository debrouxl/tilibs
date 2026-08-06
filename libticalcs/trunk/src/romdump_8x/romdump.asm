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

;;.define equ .equ
;.define org .org
;.define rorg .org
;.define db .db
;.define dw .dw

.include "romdump.inc"

	;; Loading address, read by the linker script (ldscript)
	;; to place the code at the address where the calculator
	;; will load or execute it.
	.globl __load_addr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73
;;;

.ifdef TI73

;;; This program can be launched from the home screen.

.include "ti73defs.inc"

safe_mem = appBackUpScreen
fixed_exec = saveSScreen
fixed_size_limit = 768
__code_start:

	;; BASIC code to launch assembly stub
	.ascii "_00001005?"
	ld hl, (curPC)
	jp (hl)
start:	ld bc, 0x3FD5

	ld de, fixed_code - start
	add hl, de
	ld de, fixed_exec
	ld bc, fixed_code_size
	ldir

	res remoteKeyPress, (iy + remoteKeyFlag)

.endif ; TI73

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-82
;;;

.ifdef TI82

;;; This program is launched using a buffer overflow, which jumps
;;; directly to 'start', with HL equal to the address of 'start' and
;;; SP = (onSP).

.include "ti82defs.inc"

;#define NEED_DISPLAY_ROUTINES

safe_mem = saveSScreen
fixed_exec = (saveSScreen + 128)
fixed_size_limit = 640
__code_start:

start:
	    ld de, fixed_code - start
	    add hl, de
	    ld de, fixed_exec
	    ld bc, fixed_code_size
	    ldir
	    ld (fontTable), hl

	    ld (errSP), sp

	    ld de, Main - fixed_code - fixed_code_size
	    add hl, de
	    call CallHL

	    ld hl, (OPBase)
	    ld (OPS), hl
	    ld hl, (fpBase)
	    ld (FPS), hl

	    ;; look at error handler; if possible, tweak it so that we
	    ;; exit directly to the home screen
	    ld hl, 8
	    add hl, sp
	    ld e, (hl)
	    inc hl
	    ld d, (hl)
	    ld hl, 6
	    add hl, de
	    ld a, (hl)
	    inc hl
	    xor (hl)
	    inc hl
	    xor (hl)
	    cp 0x3E ^ cxError ^ 0x0CD
	    jr nz, exit_error
	    ld a, cxCmd
	    jp (hl)

exit_error:
	    ;; if we can't figure out how to exit to home screen, then
	    ;; simply throw an error
	    pop hl
	   pop hl
	  pop hl
	 pop af
	out (2), a
	ld a, 6
	ld (errNo), a
	ret

.endif ; TI82

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-83 / TI-82 STATS / TI-76.fr
;;;

.ifdef TI83

;;; This program can be launched using the command "Send(9prgmROMDUMP".

.include "ti83defs.inc"

safe_mem = saveSScreen
fixed_exec = saveSScreen + 128
fixed_size_limit = 640
__code_start:

	.ascii "CD2644"	; call _FindProgSym
	.ascii "211900"	; ld hl, 25
	.ascii "19"		; add hl, de
	.ascii "E9"		; jp (hl)
	.byte 0x0D4
	.ascii "?0000?"
start:
	ld de, fixed_code - start
	add hl, de
	ld de, fixed_exec
	ld bc, fixed_code_size
	ldir

.endif ; TI83

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-83 Plus / TI-84 Plus
;;;

.ifdef TI83P

;;; This program can be launched using the command "Asm(prgmROMDUMP".

.include "ti83pdefs.inc"

safe_mem = saveSScreen

	;; Header for Asm(
	.byte 0x0BB, 0x6D

	in a, (2)
	rlca
	and 1
	out (0x20), a

	res remoteKeyPress, (iy + remoteKeyFlag)

.endif ; TI83P

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-84 Plus C
;;;

.ifdef TI84PC

;;; This program can be launched using the command "Asm(prgmROMDUMP".

.include "ti84pcdefs.inc"

safe_mem = saveSScreen

	;; Header for Asm(
	.byte 0x0EF, 0x69

	ld a, 1
	out (0x20), a

	res appTextSave, (iy + appFlags)
	res remoteKeyPress, (iy + remoteKeyFlag)

.endif ; TI84PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-85
;;;

.ifdef TI85

;;; This program is launched using a buffer overflow, which jumps
;;; directly to 'start', with HL equal to the address of 'start' and
;;; SP = (onSP).

.include "ti85defs.inc"

;#define NEED_DISPLAY_ROUTINES

safe_mem = plotSScreen
fixed_exec = plotSScreen + 128
fixed_size_limit = 896
__code_start:

start:
	    ld de, fixed_code - start
	    add hl, de
	    ld de, fixed_exec
	    ld bc, fixed_code_size
	    ldir
	    ld (fontTable), hl

	    ; ld (errSP), sp

	    ld de, Main - fixed_code - fixed_code_size
	    add hl, de
	    call CallHL

	    ld hl, (OPBase)
	    ld (OPS), hl
	    ld hl, (fpBase)
	    ld (FPS), hl

	    ;; I don't know if there's any ROM-independent way to exit
	    ;; directly to home screen
	    pop hl
	   pop hl
	  pop hl
	 pop af
	out (5), a
	ld a, 6
	ld (errNo), a
	ret

.endif ; TI85

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-86
;;;

.ifdef TI86

.include "ti86defs.inc"

;;; This program can be launched using the command "Asm(ROMDump".

safe_mem = 0x9000

 	;; Header for Asm(
	.byte 0x8E, 0x28

	res remoteKeyPress, (iy + remoteKeyFlag)

.endif ; TI86

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Main program
;;;

.include "defs.inc"
.include "main.asm"

fixed_code:

.ifdef fixed_exec

.endif

CallHL:	jp (hl)

.ifdef TI73
 .include "unlock73.asm"
.endif

.ifdef TI83P
 .include "unlock83p.asm"
.endif

.include "packet.asm"

.ifdef USB
.if USB
 ;include "usb.asm"
.else
 .include "link.asm"
 .ifdef TI83P
  .include "link83p.asm"
 .else
  .ifdef TI84PC
   .include "link83p.asm"
  .else
   .include "linksw.asm"
  .endif
 .endif
.endif
.else
 .include "link.asm"
 .ifdef TI83P
  .include "link83p.asm"
 .else
  .ifdef TI84PC
   .include "link83p.asm"
  .else
   .include "linksw.asm"
  .endif
 .endif
.endif

.ifdef NEED_DISPLAY_ROUTINES
 .include "display.asm"
.ifdef TI82
  .include "display82.asm"
 .else
  .include "display85.asm"
 .endif
.endif

.include "data.asm"

.ifdef fixed_exec
fixed_code_size = . - fixed_code
.if fixed_code_size > fixed_size_limit
  .error "Fixed code section too large"
 .endif
.endif

.ifdef NEED_DISPLAY_ROUTINES
 .include "font.asm"
.endif

.ifdef TI83
	.byte 0x0D4
	.ascii "?0000?"
	.byte 0x0D4
.endif

;; Load address for the linker script: the address where the section
;; must be placed so that the fixed code, which is LDIRed to fixed_exec,
;; is assembled at fixed_exec (the section starts with the launcher,
;; hence the subtraction of the launcher length, fixed_code - __code_start).
.ifdef TI73
__load_addr = fixed_exec - (fixed_code - __code_start)
.endif
.ifdef TI82
__load_addr = fixed_exec - (fixed_code - __code_start)
.endif
.ifdef TI83
__load_addr = fixed_exec - (fixed_code - __code_start)
.endif
.ifdef TI85
__load_addr = fixed_exec - (fixed_code - __code_start)
.endif
.ifdef TI83P
__load_addr = 0x9D93
.endif
.ifdef TI84PC
__load_addr = 0x0A609
.endif
.ifdef TI86
__load_addr = 0x0D746
.endif
