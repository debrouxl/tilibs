/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifndef __TICABLES_EXPORT__
#define __TICABLES_EXPORT__

/*
 * Defines this if you want use this library with Visual Basic.
 * VB & Delphi also requires the __stdcall calling convention
 */
//#define VB

/*
 *  BCC32 v5.x (or C++Builder)
 *  (C) 2001 Thomas Wolf (two@chello.at)
 */
#if defined(__WIN32__)
# ifdef __BORLANDC__				// Borland
#  if __BORLANDC__ >= 0x0500
#   define TIEXPORT
#   define TICALL  __stdcall
#  else
#   define TICALL
#   define TICALL
#  endif
/*
 * MSVC 5.0 mini
 */
# elif defined(_MSC_VER) && !defined(VB)	// Microsoft
#  ifdef TICABLES_EXPORTS
#   define TIEXPORT __declspec(dllexport)
#   define TICALL
#  else
#   define TIEXPORT __declspec(dllexport)       //__declspec(dllimport)
#   define TICALL
#  endif
# elif defined(VB)
#  define TIEXPORT
#  define TICALL  __stdcall			// VB
# endif

#elif defined(__LINUX__)			// GNU
# define TIEXPORT extern
# define TICALL
#else
# define TIEXPORT				// default
# define TICALL
#endif

// Note: VB requires __sdtcall but __stdcall make entry points disappear -> 
// .def file; MSVC uses _cdecl by default (__declspec)

#endif
