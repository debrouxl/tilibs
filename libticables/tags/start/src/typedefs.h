/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define MAXCHARS 256  // Size max for strings

#include <sys/types.h>

#ifndef TILP_TYPES
#define TILP_TYPES
/*************************/
/* Some type definitions */
/*************************/
#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char  byte;      /* One byte */
#endif // !_BYTE_DEFINED

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short word;      /* Two bytes */
#endif // !_WORD_DEFINED

/* Under linux, sizeof(int)=4 but under Win32, sizeof(int)=2 -> longint */
#if defined(__WIN32__) || defined(__WIN16__)
  typedef unsigned long longword;
  typedef unsigned int uint;
#else //if defined(__LINUX__)
# ifndef longword
   typedef unsigned int longword;
# endif
#endif
#endif //TILP_TYPES

#endif




