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

 .byte ' '
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000

 .byte '.'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00000000
 .byte %00001100
 .byte %00001100

 .byte ':'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00001100
 .byte %00001100
 .byte %00000000
 .byte %00001100
 .byte %00001100
 .byte %00000000

 .byte '['
;;;SPRITE 8x7x1
 .byte %00001100
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001100

 .byte ']'
;;;SPRITE 8x7x1
 .byte %00011000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00011000

 .byte '0'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00100010
 .byte %00100110
 .byte %00101010
 .byte %00110010
 .byte %00100010
 .byte %00011100

 .byte '1'
;;;SPRITE 8x7x1
 .byte %00001000
 .byte %00011000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00011100

 .byte '2'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00100010
 .byte %00000010
 .byte %00000100
 .byte %00001000
 .byte %00010000
 .byte %00111110

 .byte '3'
;;;SPRITE 8x7x1
 .byte %00111110
 .byte %00000100
 .byte %00001000
 .byte %00000100
 .byte %00000010
 .byte %00100010
 .byte %00011100

 .byte '4'
;;;SPRITE 8x7x1
 .byte %00000100
 .byte %00001100
 .byte %00010100
 .byte %00100100
 .byte %00111110
 .byte %00000100
 .byte %00000100

 .byte '5'
;;;SPRITE 8x7x1
 .byte %00111110
 .byte %00100000
 .byte %00111100
 .byte %00000010
 .byte %00000010
 .byte %00100010
 .byte %00011100

 .byte '6'
;;;SPRITE 8x7x1
 .byte %00001100
 .byte %00010000
 .byte %00100000
 .byte %00111100
 .byte %00100010
 .byte %00100010
 .byte %00011100

 .byte '7'
;;;SPRITE 8x7x1
 .byte %00111110
 .byte %00000010
 .byte %00000100
 .byte %00001000
 .byte %00010000
 .byte %00010000
 .byte %00010000

 .byte '8'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00100010
 .byte %00100010
 .byte %00011100
 .byte %00100010
 .byte %00100010
 .byte %00011100

 .byte '9'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00100010
 .byte %00100010
 .byte %00011110
 .byte %00000010
 .byte %00000100
 .byte %00011000

 .byte 'D'
;;;SPRITE 8x7x1
 .byte %00111100
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00111100

 .byte 'E'
;;;SPRITE 8x7x1
 .byte %00111110
 .byte %00100000
 .byte %00100000
 .byte %00111100
 .byte %00100000
 .byte %00100000
 .byte %00111110

 .byte 'I'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00011100

 .byte 'K'
;;;SPRITE 8x7x1
 .byte %00100010
 .byte %00100100
 .byte %00101000
 .byte %00110000
 .byte %00101000
 .byte %00100100
 .byte %00100010

 .byte 'M'
;;;SPRITE 8x7x1
 .byte %00100010
 .byte %00110110
 .byte %00101010
 .byte %00101010
 .byte %00100010
 .byte %00100010
 .byte %00100010

 .byte 'O'
;;;SPRITE 8x7x1
 .byte %00011100
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00011100

 .byte 'R'
;;;SPRITE 8x7x1
 .byte %00111100
 .byte %00100010
 .byte %00100010
 .byte %00111100
 .byte %00101000
 .byte %00100100
 .byte %00100010

 .byte 'T'
;;;SPRITE 8x7x1
 .byte %00111110
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000

 .byte 'X'
;;;SPRITE 8x7x1
 .byte %00100010
 .byte %00100010
 .byte %00010100
 .byte %00001000
 .byte %00010100
 .byte %00100010
 .byte %00100010

 .byte 'a'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00011100
 .byte %00000010
 .byte %00011110
 .byte %00100010
 .byte %00011110

 .byte 'c'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00011100
 .byte %00100000
 .byte %00100000
 .byte %00100010
 .byte %00011100

 .byte 'e'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00011100
 .byte %00100010
 .byte %00111110
 .byte %00100000
 .byte %00011100

 .byte 'f'
;;;SPRITE 8x7x1
 .byte %00001100
 .byte %00010010
 .byte %00010000
 .byte %00111000
 .byte %00010000
 .byte %00010000
 .byte %00010000

 .byte 'g'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00011110
 .byte %00100010
 .byte %00100010
 .byte %00011110
 .byte %00000010
 .byte %00011100

 .byte 'i'
;;;SPRITE 8x7x1
 .byte %00001000
 .byte %00000000
 .byte %00011000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00011100

 .byte 'l'
;;;SPRITE 8x7x1
 .byte %00011000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00001000
 .byte %00011100

 .byte 'm'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00110100
 .byte %00101010
 .byte %00101010
 .byte %00100010
 .byte %00100010

 .byte 'n'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00101100
 .byte %00110010
 .byte %00100010
 .byte %00100010
 .byte %00100010

 .byte 'o'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00011100
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00011100

 .byte 'p'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00111100
 .byte %00100010
 .byte %00111100
 .byte %00100000
 .byte %00100000

 .byte 'r'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00101100
 .byte %00110010
 .byte %00100000
 .byte %00100000
 .byte %00100000

 .byte 's'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00011100
 .byte %00100000
 .byte %00011100
 .byte %00000010
 .byte %00111100

 .byte 't'
;;;SPRITE 8x7x1
 .byte %00010000
 .byte %00010000
 .byte %00111000
 .byte %00010000
 .byte %00010000
 .byte %00010010
 .byte %00001100

 .byte 'u'
;;;SPRITE 8x7x1
 .byte %00000000
 .byte %00000000
 .byte %00100010
 .byte %00100010
 .byte %00100010
 .byte %00100110
 .byte %00011010
