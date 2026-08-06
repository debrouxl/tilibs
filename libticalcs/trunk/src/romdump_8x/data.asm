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

DumpingROMString:	.asciz "Dumping ROM..."
			EXIT_KEY_STR
			.asciz " to cancel"

ErrorString:		.asciz "Errors:"

KString:		.ascii "0K of "

.if CALC_ROM_SIZE == 4096
			.ascii "4096K"
ROMSize:		.word 0x0000, 0x0040
 .endif

.if CALC_ROM_SIZE == 512
			.ascii "512K"
ROMSize:		.word 0x0000, 0x0008
 .endif

.if CALC_ROM_SIZE == 128
			.ascii "128K"
ROMSize:		.word 0x0000, 0x0002
 .endif

.if CALC_ROM_SIZE == 256
			.ascii "256K"
ROMSize:		.word 0x0000, 0x0004
 .endif

.if CALC_ROM_SIZE == 0
ROMSize:		.word 0x0000, 0x0008
 .endif
