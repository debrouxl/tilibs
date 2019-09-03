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

 db ' '
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00000000
 db %00000000
 db %00000000
 db %00000000
 db %00000000

 db '.'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00000000
 db %00000000
 db %00000000
 db %00001100
 db %00001100

 db ':'
;;;SPRITE 8x7x1
 db %00000000
 db %00001100
 db %00001100
 db %00000000
 db %00001100
 db %00001100
 db %00000000

 db '['
;;;SPRITE 8x7x1
 db %00001100
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001100

 db ']'
;;;SPRITE 8x7x1
 db %00011000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00011000

 db '0'
;;;SPRITE 8x7x1
 db %00011100
 db %00100010
 db %00100110
 db %00101010
 db %00110010
 db %00100010
 db %00011100

 db '1'
;;;SPRITE 8x7x1
 db %00001000
 db %00011000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00011100

 db '2'
;;;SPRITE 8x7x1
 db %00011100
 db %00100010
 db %00000010
 db %00000100
 db %00001000
 db %00010000
 db %00111110

 db '3'
;;;SPRITE 8x7x1
 db %00111110
 db %00000100
 db %00001000
 db %00000100
 db %00000010
 db %00100010
 db %00011100

 db '4'
;;;SPRITE 8x7x1
 db %00000100
 db %00001100
 db %00010100
 db %00100100
 db %00111110
 db %00000100
 db %00000100

 db '5'
;;;SPRITE 8x7x1
 db %00111110
 db %00100000
 db %00111100
 db %00000010
 db %00000010
 db %00100010
 db %00011100

 db '6'
;;;SPRITE 8x7x1
 db %00001100
 db %00010000
 db %00100000
 db %00111100
 db %00100010
 db %00100010
 db %00011100

 db '7'
;;;SPRITE 8x7x1
 db %00111110
 db %00000010
 db %00000100
 db %00001000
 db %00010000
 db %00010000
 db %00010000

 db '8'
;;;SPRITE 8x7x1
 db %00011100
 db %00100010
 db %00100010
 db %00011100
 db %00100010
 db %00100010
 db %00011100

 db '9'
;;;SPRITE 8x7x1
 db %00011100
 db %00100010
 db %00100010
 db %00011110
 db %00000010
 db %00000100
 db %00011000

 db 'D'
;;;SPRITE 8x7x1
 db %00111100
 db %00100010
 db %00100010
 db %00100010
 db %00100010
 db %00100010
 db %00111100

 db 'E'
;;;SPRITE 8x7x1
 db %00111110
 db %00100000
 db %00100000
 db %00111100
 db %00100000
 db %00100000
 db %00111110

 db 'I'
;;;SPRITE 8x7x1
 db %00011100
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00011100

 db 'K'
;;;SPRITE 8x7x1
 db %00100010
 db %00100100
 db %00101000
 db %00110000
 db %00101000
 db %00100100
 db %00100010

 db 'M'
;;;SPRITE 8x7x1
 db %00100010
 db %00110110
 db %00101010
 db %00101010
 db %00100010
 db %00100010
 db %00100010

 db 'O'
;;;SPRITE 8x7x1
 db %00011100
 db %00100010
 db %00100010
 db %00100010
 db %00100010
 db %00100010
 db %00011100

 db 'R'
;;;SPRITE 8x7x1
 db %00111100
 db %00100010
 db %00100010
 db %00111100
 db %00101000
 db %00100100
 db %00100010

 db 'T'
;;;SPRITE 8x7x1
 db %00111110
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000

 db 'X'
;;;SPRITE 8x7x1
 db %00100010
 db %00100010
 db %00010100
 db %00001000
 db %00010100
 db %00100010
 db %00100010

 db 'a'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00011100
 db %00000010
 db %00011110
 db %00100010
 db %00011110

 db 'c'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00011100
 db %00100000
 db %00100000
 db %00100010
 db %00011100

 db 'e'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00011100
 db %00100010
 db %00111110
 db %00100000
 db %00011100

 db 'f'
;;;SPRITE 8x7x1
 db %00001100
 db %00010010
 db %00010000
 db %00111000
 db %00010000
 db %00010000
 db %00010000

 db 'g'
;;;SPRITE 8x7x1
 db %00000000
 db %00011110
 db %00100010
 db %00100010
 db %00011110
 db %00000010
 db %00011100

 db 'i'
;;;SPRITE 8x7x1
 db %00001000
 db %00000000
 db %00011000
 db %00001000
 db %00001000
 db %00001000
 db %00011100

 db 'l'
;;;SPRITE 8x7x1
 db %00011000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00001000
 db %00011100

 db 'm'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00110100
 db %00101010
 db %00101010
 db %00100010
 db %00100010

 db 'n'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00101100
 db %00110010
 db %00100010
 db %00100010
 db %00100010

 db 'o'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00011100
 db %00100010
 db %00100010
 db %00100010
 db %00011100

 db 'p'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00111100
 db %00100010
 db %00111100
 db %00100000
 db %00100000

 db 'r'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00101100
 db %00110010
 db %00100000
 db %00100000
 db %00100000

 db 's'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00011100
 db %00100000
 db %00011100
 db %00000010
 db %00111100

 db 't'
;;;SPRITE 8x7x1
 db %00010000
 db %00010000
 db %00111000
 db %00010000
 db %00010000
 db %00010010
 db %00001100

 db 'u'
;;;SPRITE 8x7x1
 db %00000000
 db %00000000
 db %00100010
 db %00100010
 db %00100010
 db %00100110
 db %00011010
