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

/* 
   Allow to display or not some informations depending on the
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

// Not all UNIXes have the vprintf function
#ifndef HAVE_VPRINTF
# undef VERBOSE
#else
# define VERBOSE
#endif

// Display informations in a console
#if defined(__WIN32__) 
# define VERBOSE
  static int alloc_console_called = FALSE;
  HANDLE hConsole;
#elif defined(__LINUX__)
# define VERBOSE
#endif

// Default verbosity
#if defined(__LINUX__) || defined(__WIN32__) && defined(_DEBUG)
  static int verbosity = 1;
#elif defined(__WIN32__) || defined(__MACOSX__)
  static int verbosity = 0;
#endif

// Store in a file what is displayed in the console
#define LOG_FILE "console.log"
static FILE *flog = NULL;

/* 
   This function is equivalent to 'fprintf(stdout, ...)' but 
   if the VERBOSE constant is defined.
*/
TIEXPORT int TICALL DISPLAY(const char *format, ...)
{
  int ret = 0;
  va_list ap;

  if(flog == NULL)
  {
	  flog = fopen(LOG_FILE, "wt");
  }

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
	  if(flog) vfprintf(flog, format, ap);
      va_end(ap);
#endif
    }

  return ret;
}

/* 
   This function is equivalent to 'fprintf(stderr, ...)' but 
   if the VERBOSE constant is defined.
*/
TIEXPORT int TICALL dERROR(const char *format, ...)
{
  int ret = 0;
  va_list ap;

  if(flog == NULL)
  {
    flog = fopen(LOG_FILE, "wt");
  }

  if(verbosity)
    {
      // Under Win32, we redirect stderr to the console
#if defined(__WIN32__)				
      if (!alloc_console_called)
	{
	  hConsole = GetStdHandle (STD_ERROR_HANDLE);
	  
	  if (hConsole == INVALID_HANDLE_VALUE)
	    {
	      AllocConsole ();
	      alloc_console_called = TRUE;
	      freopen ("CONERR$", "w", stderr);
	    }
	}
#endif
#ifdef VERBOSE
      va_start(ap, format);
      fprintf(stderr, "libticables error: ");
      if(flog) fprintf(flog, "libticables error: ");
      ret=vfprintf(stderr, format, ap);
      if(flog) vfprintf(flog, format, ap);
      va_end(ap);
#endif
    }

  return ret;
}


/* 
   Set the verbosity level
*/
TIEXPORT int TICALL ticable_DISPLAY_settings(int op)
{
  switch(op)
    {
    case DSP_OFF:
#if defined(__WIN32__) && defined(_DEBUG)
		verbosity = 1;
#else
      verbosity = 0;
#endif
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
