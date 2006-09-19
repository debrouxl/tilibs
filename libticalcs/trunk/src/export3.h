/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#ifndef __TICALCS2_EXPORT__
#define __TICALCS2_EXPORT__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Choose one of these calling conventions (override compiler settings)
 */
//#define FORCE_STDCALL
//#define FORCE_C_CALL
//#define FORCE_FASTCALL
//#define FORCE_NONE

/*
 * Defines one of the previous definitions for forcing a calling convention.
 * VB & Delphi users will enable FORCE_STDCALL.
 */
#if defined(__WIN32__)
# if defined(FORCE_STDCALL)
#  define TICALL    __stdcall

# elif defined(FORCE_C_CALL)
#  define TICALL    __cdecl

# elif defined(FORCE_FASTCALL)
#  define TICALL    __fastcall

# else
#  define TICALL
# endif
#else
# define TICALL
#endif

#if defined(HAVE_FVISIBILITY)// GCC 4.0 has introduced the -fvisibility flag (similar to declspec)
# define TIEXPORT __attribute__ ((visibility("default")))

#elif defined(__WIN32__)
# if defined(__BORLANDC__)		// BCC32 v5.x (or C++Builder)
#  if __BORLANDC__ >= 0x0500	// (c) 2001 Thomas Wolf (two@chello.at)
#   define TIEXPORT
#  else
#   define TIEXPORT
#  endif

# elif defined(_MSC_VER)		// MSVC 5.0 mini
#  if defined(DLL_EXPORT) || defined(TICABLES_EXPORTS) || defined(TIFILES_EXPORTS) || defined(TICALCS_EXPORTS) || defined(TICONV_EXPORTS)
#   define TIEXPORT __declspec(dllexport)
#  else
#   define TIEXPORT __declspec(dllimport)
#  endif

# elif defined(__MINGW32__)		// MinGW - GCC for Windows, (c) 2002 Kevin Kofler
#  if defined(DLL_EXPORT)		// defined by the configure script
#   define TIEXPORT __declspec(dllexport)
#  else
#   define TIEXPORT extern
#  endif
# endif

#elif defined(__LINUX__) || defined(__BSD__)	// GNU
# define TIEXPORT extern

#else
# define TIEXPORT				// default
#endif

#ifdef __cplusplus
}
#endif

// Note: VB requires __sdtcall but __stdcall make entry points disappear -> 
// .def file; MSVC uses _cdecl by default (__declspec)

#ifndef TILIBS_DEPRECATED
# ifdef __GNUC__
#  if (__GNUC__>3) || (__GNUC__==3 && __GNUC_MINOR__>=3)
#   define TILIBS_DEPRECATED __attribute__((deprecated))
#  else /* not GCC >= 3.3 */
#   define TILIBS_DEPRECATED
#  endif /* GCC >= 3.3 */
# else /* not __GNUC__ */
#  ifdef _MSC_VER
#   if _MSC_VER >= 1300
#    define TILIBS_DEPRECATED __declspec((deprecated))
#   else /* not _MSC_VER >= 1300 */
#    define TILIBS_DEPRECATED
#   endif /* _MSC_VER >= 1300 */
#  else /* not _MSC_VER */
#   define TILIBS_DEPRECATED
#  endif /* _MSC_VER */
# endif /* __GNUC__ */
#endif /* TILIBS_DEPRECATED */

#endif
