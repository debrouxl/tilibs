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

include "romdump.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-73
;;;

if defined TI73

;;; This program can be launched from the home screen.

include "ti73defs.inc"

safe_mem         equ appBackUpScreen
fixed_exec       equ saveSScreen
fixed_size_limit equ 768

	;; BASIC code to launch assembly stub
	db "_00001005?"
	ld hl, (curPC)
	jp (hl)
start:	ld bc, 3FD5h

	ld de, fixed_code - start
	add hl, de
	ld de, fixed_exec
	ld bc, fixed_code_size
	ldir

	res remoteKeyPress, (iy + remoteKeyFlag)

endif ; TI73

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-82
;;;

if defined TI82

;;; This program is launched using a buffer overflow, which jumps
;;; directly to 'start', with HL equal to the address of 'start' and
;;; SP = (onSP).

include "ti82defs.inc"

;#define NEED_DISPLAY_ROUTINES

safe_mem         equ saveSScreen
fixed_exec       equ (saveSScreen + 128)
fixed_size_limit equ 640

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
	    cp 3Eh xor cxError xor 0CDh
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

endif ; TI82

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-83 / TI-82 STATS / TI-76.fr
;;;

if defined TI83

;;; This program can be launched using the command "Send(9prgmROMDUMP".

include "ti83defs.inc"

safe_mem         equ saveSScreen
fixed_exec       equ saveSScreen + 128
fixed_size_limit equ 640

	db "CD2644"	; call _FindProgSym
	db "211900"	; ld hl, 25
	db "19"		; add hl, de
	db "E9"		; jp (hl)
	db 0D4h
	db "?0000?"

start:
	ld de, fixed_code - start
	add hl, de
	ld de, fixed_exec
	ld bc, fixed_code_size
	ldir

endif ; TI83

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-83 Plus / TI-84 Plus
;;;

if defined TI83P

;;; This program can be launched using the command "Asm(prgmROMDUMP".

include "ti83pdefs.inc"

safe_mem equ saveSScreen

	;; Header for Asm(
	org 9d93h
	db 0BBh, 6Dh

	in a, (2)
	rlca
	and 1
	out (20h), a

	res remoteKeyPress, (iy + remoteKeyFlag)

endif ; TI83P

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-84 Plus C
;;;

if defined TI84PC

;;; This program can be launched using the command "Asm(prgmROMDUMP".

include "ti84pcdefs.inc"

safe_mem equ saveSScreen

	;; Header for Asm(
	org 0A609h
	db 0EFh, 69h

	ld a, 1
	out (20h), a

	res appTextSave, (iy + appFlags)
	res remoteKeyPress, (iy + remoteKeyFlag)

endif ; TI84PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-85
;;;

if defined TI85

;;; This program is launched using a buffer overflow, which jumps
;;; directly to 'start', with HL equal to the address of 'start' and
;;; SP = (onSP).

include "ti85defs.inc"

;#define NEED_DISPLAY_ROUTINES

safe_mem         equ plotSScreen
fixed_exec       equ plotSScreen + 128
fixed_size_limit equ 896

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

endif ; TI85

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; TI-86
;;;

if defined TI86

include "ti86defs.inc"

;;; This program can be launched using the command "Asm(ROMDump".

safe_mem equ 9000h

 	;; Header for Asm(
	org 0D746h
	db 8Eh, 28h

	res remoteKeyPress, (iy + remoteKeyFlag)

endif ; TI86

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Main program
;;;

include "defs.inc"
include "main.asm"

fixed_code:

if defined fixed_exec
 org fixed_exec
endif

CallHL:	jp (hl)

if defined TI73
 include "unlock73.asm"
endif

if defined TI83P
 include "unlock83p.asm"
endif

include "packet.asm"

if defined USB && USB != 0
 ;include "usb.asm"
else
 include "link.asm"
 if defined TI83P
  include "link83p.asm"
 else
  if defined TI84PC
   include "link83p.asm"
  else
   include "linksw.asm"
  endif
 endif
endif

if defined NEED_DISPLAY_ROUTINES
 include "display.asm"
 if defined TI82
  include "display82.asm"
 else
  include "display85.asm"
 endif
endif

include "data.asm"

if defined fixed_exec
fixed_code_size equ $ - fixed_exec
 if (fixed_code_size > fixed_size_limit)
  error "Fixed code section too large"
 endif
endif

if defined NEED_DISPLAY_ROUTINES
 include "font.asm"
endif

if defined TI83
	db 0D4h, "?0000?", 0D4h
endif
