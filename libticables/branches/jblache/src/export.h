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

#ifndef EXPORT_H
#define EXPORT_H
/*
 * For exporting function in a Windows DLL
 */
#if defined(__WIN32__) // MSVC 5.0 mini
# define DLLEXPORT __declspec(dllexport)
# define DLLIMPORT __declspec(dllimport)
# define DLLEXPORT2
#elif defined(__LINUX__)
# define DLLEXPORT
# define DLLIMPORT
# define DLLEXPORT2
#elif defined(__WIN16__) // Borland 4.5 mini
# define DLLEXPORT
# define DLLIMPORT
# define DLLEXPORT2 __export // int far __export myfunc
#else
# define DLLEXPORT
# define DLLIMPORT
# define DLLEXPORT2
#endif

/*
 *  BCC32 v5.x (or C++Builder)
 *  (C) 2001 Thomas Wolf (two@chello.at)
 */
#ifdef __WIN32__
# ifdef __BORLANDC__
#  if __BORLANDC__ >= 0x0500
#   define TIEXPORT __stdcall
#  else
#   define TIEXPORT
#  endif
# else
#  define TIEXPORT
# endif // __BORLANDC__
#else
# define TIEXPORT
#endif // __WIN32__

#endif

