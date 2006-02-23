/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

/* 
   Encapsulate the ISO-C99 'stdint.h' header for platforms which haven't it
*/

#ifndef __TICONV_STDINT__
#define __TICONV_STDINT__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# if defined(__WIN32__)
#  include <windows.h>
typedef BYTE uint8_t;
typedef WORD uint16_t;
typedef DWORD uint32_t;
#pragma warning( push )
#pragma warning( disable : 4142 )
typedef unsigned char int8_t;
typedef unsigned short int16_t;
typedef unsigned long int32_t;
#pragma warning( pop )
# elif defined(__BSD__)
#  include <inttypes.h>
# else
#  include <inttypes.h>
# endif				/* __WIN32__, __BSD__ */

#endif				/* HAVE_STDINT_H */

#endif				/* __TIFILES_STDINT__ */
