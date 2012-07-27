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
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b

 db '.'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b
 db 00000000b
 db 00001100b
 db 00001100b

 db ':'
;;;SPRITE 8x7x1
 db 00000000b
 db 00001100b
 db 00001100b
 db 00000000b
 db 00001100b
 db 00001100b
 db 00000000b

 db '['
;;;SPRITE 8x7x1
 db 00001100b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001100b

 db ']'
;;;SPRITE 8x7x1
 db 00011000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00011000b

 db '0'
;;;SPRITE 8x7x1
 db 00011100b
 db 00100010b
 db 00100110b
 db 00101010b
 db 00110010b
 db 00100010b
 db 00011100b

 db '1'
;;;SPRITE 8x7x1
 db 00001000b
 db 00011000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00011100b

 db '2'
;;;SPRITE 8x7x1
 db 00011100b
 db 00100010b
 db 00000010b
 db 00000100b
 db 00001000b
 db 00010000b
 db 00111110b

 db '3'
;;;SPRITE 8x7x1
 db 00111110b
 db 00000100b
 db 00001000b
 db 00000100b
 db 00000010b
 db 00100010b
 db 00011100b

 db '4'
;;;SPRITE 8x7x1
 db 00000100b
 db 00001100b
 db 00010100b
 db 00100100b
 db 00111110b
 db 00000100b
 db 00000100b

 db '5'
;;;SPRITE 8x7x1
 db 00111110b
 db 00100000b
 db 00111100b
 db 00000010b
 db 00000010b
 db 00100010b
 db 00011100b

 db '6'
;;;SPRITE 8x7x1
 db 00001100b
 db 00010000b
 db 00100000b
 db 00111100b
 db 00100010b
 db 00100010b
 db 00011100b

 db '7'
;;;SPRITE 8x7x1
 db 00111110b
 db 00000010b
 db 00000100b
 db 00001000b
 db 00010000b
 db 00010000b
 db 00010000b

 db '8'
;;;SPRITE 8x7x1
 db 00011100b
 db 00100010b
 db 00100010b
 db 00011100b
 db 00100010b
 db 00100010b
 db 00011100b

 db '9'
;;;SPRITE 8x7x1
 db 00011100b
 db 00100010b
 db 00100010b
 db 00011110b
 db 00000010b
 db 00000100b
 db 00011000b

 db 'D'
;;;SPRITE 8x7x1
 db 00111100b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00111100b

 db 'E'
;;;SPRITE 8x7x1
 db 00111110b
 db 00100000b
 db 00100000b
 db 00111100b
 db 00100000b
 db 00100000b
 db 00111110b

 db 'I'
;;;SPRITE 8x7x1
 db 00011100b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00011100b

 db 'K'
;;;SPRITE 8x7x1
 db 00100010b
 db 00100100b
 db 00101000b
 db 00110000b
 db 00101000b
 db 00100100b
 db 00100010b

 db 'M'
;;;SPRITE 8x7x1
 db 00100010b
 db 00110110b
 db 00101010b
 db 00101010b
 db 00100010b
 db 00100010b
 db 00100010b

 db 'O'
;;;SPRITE 8x7x1
 db 00011100b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00011100b

 db 'R'
;;;SPRITE 8x7x1
 db 00111100b
 db 00100010b
 db 00100010b
 db 00111100b
 db 00101000b
 db 00100100b
 db 00100010b

 db 'T'
;;;SPRITE 8x7x1
 db 00111110b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b

 db 'X'
;;;SPRITE 8x7x1
 db 00100010b
 db 00100010b
 db 00010100b
 db 00001000b
 db 00010100b
 db 00100010b
 db 00100010b

 db 'a'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00011100b
 db 00000010b
 db 00011110b
 db 00100010b
 db 00011110b

 db 'c'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00011100b
 db 00100000b
 db 00100000b
 db 00100010b
 db 00011100b

 db 'e'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00011100b
 db 00100010b
 db 00111110b
 db 00100000b
 db 00011100b

 db 'f'
;;;SPRITE 8x7x1
 db 00001100b
 db 00010010b
 db 00010000b
 db 00111000b
 db 00010000b
 db 00010000b
 db 00010000b

 db 'g'
;;;SPRITE 8x7x1
 db 00000000b
 db 00011110b
 db 00100010b
 db 00100010b
 db 00011110b
 db 00000010b
 db 00011100b

 db 'i'
;;;SPRITE 8x7x1
 db 00001000b
 db 00000000b
 db 00011000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00011100b

 db 'l'
;;;SPRITE 8x7x1
 db 00011000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00001000b
 db 00011100b

 db 'm'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00110100b
 db 00101010b
 db 00101010b
 db 00100010b
 db 00100010b

 db 'n'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00101100b
 db 00110010b
 db 00100010b
 db 00100010b
 db 00100010b

 db 'o'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00011100b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00011100b

 db 'p'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00111100b
 db 00100010b
 db 00111100b
 db 00100000b
 db 00100000b

 db 'r'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00101100b
 db 00110010b
 db 00100000b
 db 00100000b
 db 00100000b

 db 's'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00011100b
 db 00100000b
 db 00011100b
 db 00000010b
 db 00111100b

 db 't'
;;;SPRITE 8x7x1
 db 00010000b
 db 00010000b
 db 00111000b
 db 00010000b
 db 00010000b
 db 00010010b
 db 00001100b

 db 'u'
;;;SPRITE 8x7x1
 db 00000000b
 db 00000000b
 db 00100010b
 db 00100010b
 db 00100010b
 db 00100110b
 db 00011010b
