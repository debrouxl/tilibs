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

DumpingROMString:	db "Dumping ROM...", 0
			EXIT_KEY_STR
			db " to cancel", 0

ErrorString:		db "Errors:", 0

KString:		db "0K of "

 if CALC_ROM_SIZE = 4096
			db "4096K"
ROMSize:		dw 0000h, 0040h
 endif

 if CALC_ROM_SIZE = 512
			db "512K"
ROMSize:		dw 0000h, 0008h
 endif

 if CALC_ROM_SIZE = 128
			db "128K"
ROMSize:		dw 0000h, 0002h
 endif

 if CALC_ROM_SIZE = 256
			db "256K"
ROMSize:		dw 0000h, 0004h
 endif

 if CALC_ROM_SIZE = 0
ROMSize:		dw 0000h, 0008h
 endif
