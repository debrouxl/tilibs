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

/* Allow to display or not some informations depending on the
	verbosity level.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#ifdef __WIN32__
# include <windows.h>
#endif

#include "export.h"
#include "verbose.h"

// Not all systems have the vprintf function
#ifndef HAVE_VPRINTF
# undef VERBOSE
#else
# define VERBOSE
#endif

//
#if defined(__WIN32__) 
# define VERBOSE
  static int alloc_console_called = FALSE;
  HANDLE hConsole;
#elif defined(__LINUX__)
# define VERBOSE
#endif

// Default verbosity
#if defined(__LINUX__) // || defined(__WIN32__) && defined(_DEBUG)
  static int verbosity = 1;
#elif defined(__WIN32__) || defined(__MACOSX__)
  static int verbosity = 0;
#endif

/* 
   This function is equivalent to 'fprintf(stdout, ...)' but 
   if the VERBOSE constant is defined.
*/
DLLEXPORT int DLLEXPORT2 DISPLAY(const char *format, ...)
{
  int ret = 0;
#ifdef VERBOSE
  va_list ap;
#endif

  if(verbosity)
    {
      // Under Win32, we redirect stdout to the console
#if defined(__WIN32__)				
      if (!alloc_console_called)
	{
	  hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
	  
	  if (hConsole == INVALID_HANDLE_VALUE)
	    {
	      AllocConsole ();
	      alloc_console_called = TRUE;
	      freopen ("CONOUT$", "w", stdout);
	    }
	}
#endif
#ifdef VERBOSE
      va_start(ap, format);
      ret=vfprintf(stdout, format, ap);
      va_end(ap);
#endif
    }

  return ret;
}


/* 
   Set the verbosity degree
*/
DLLEXPORT int DLLEXPORT2 ticable_DISPLAY_settings(int op)
{
  switch(op)
    {
    case DSP_OFF:
      verbosity = 0;
      break;
    case DSP_ON:
      verbosity = 1;
      break;
    case DSP_CLOSE:
#ifdef __WIN32__
      FreeConsole();
#endif
      break;
    default:
      break;
    }
  
  return 0;
}
