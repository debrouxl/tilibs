/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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
   
   Note: this module is completely deprecated (replaced by a more convenient 
   module: printl). But, it can be used as sample code for overriding printl
   callback.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#ifdef __WIN32__
# include <windows.h>
#endif

#include "export.h"
#include "verbose.h"
#include "printl.h"

// for compatibility
TIEXPORT int TICALL DISPLAY(const char *format, ...)
{
	va_list ap;
	int ret;
	char buf[256];
		
	va_start(ap, format);
	ret = _vsnprintf(buf, 256, format, ap);
    	ret = printl1(0, "%s", buf);
    	va_end(ap);
	
	return ret;
}

// for compatibility
TIEXPORT int TICALL DISPLAY_ERROR(const char *format, ...)
{
	va_list ap;
	int ret;
	char buf[256];
		
	va_start(ap, format);
	ret = _vsnprintf(buf, 256, format, ap);
    	ret = printl1(2, "%s", buf);
    	va_end(ap);
	
	return ret;
}

TIEXPORT int TICALL ticable_DISPLAY_settings(TicableDisplay op)
{
	return 0;
}

TIEXPORT int TICALL ticable_verbose_settings(TicableDisplay op)
{
  	return 0;
}

TIEXPORT int TICALL ticable_verbose_set_file(const char *filename)
{
	return 0;
}

TIEXPORT int TICALL ticable_verbose_flush_file(void)
{
	return 0;
}

TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_stream(FILE * stream)
{
	return NULL;
}

TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_file(char *filename)
{
	 return NULL;
}

TIEXPORT int TICALL ticable_DISPLAY_close_file()
{
  	return 0;
}

#if 0

// Display output in console
#ifdef __WIN32__
static BOOL alloc_console_called = FALSE;
HANDLE hConsole;
#endif

// Default verbosity level (on for UNIXes, off for Windows)
#if defined(__LINUX__) || defined(__BSD__)
static int verbosity = 1;
#elif defined(__WIN32__) || defined(__MACOSX__)
static int verbosity = 0;
#endif


static FILE *flog = NULL;
static char *fname = NULL;


TIEXPORT int TICALL DISPLAY(const char *format, ...)
{
  	int ret = 0;
  	va_list ap;

  	if (verbosity) {
#if defined(__WIN32__)
                char buffer[128];
                int cnt;
                DWORD nWritten;

    	        if (alloc_console_called == FALSE) {
      		        AllocConsole();
      		        alloc_console_called = TRUE;
      		        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      		        //freopen("CONOUT$", "w", stdout);
    	        }

                va_start(ap, format);
                cnt = _vsnprintf(buffer, 128, format, ap);
                WriteConsole(hConsole, buffer, cnt, &nWritten, NULL);
                va_end(ap);
#else
                va_start(ap, format);
    	        ret = vfprintf(stdout, format, ap);
    	        va_end(ap);
#endif
  	}

        if (flog == NULL) {
    		flog = fopen(DISP_FILE, "wt");
        } else {
    		va_start(ap, format);
    		if (flog)
      			vfprintf(flog, format, ap);
    		va_end(ap);
  	}

  	return ret;
}


/* 
   This function is equivalent to 'DISPLAY_ERROR(...)' but 
   if the VERBOSE constant is defined.
*/
TIEXPORT int TICALL DISPLAY_ERROR(const char *format, ...)
{
        int ret = 0;
  	va_list ap;

  	if (verbosity) {
#if defined(__WIN32__)
                char buffer[128];
                int cnt;
                DWORD nWritten;

    		if (alloc_console_called == FALSE) {
      			AllocConsole();
      			alloc_console_called = TRUE;
      			hConsole = GetStdHandle(STD_ERROR_HANDLE);
      			//freopen("CONERR$", "w", stderr);
    		}

                va_start(ap, format);
                cnt = _vsnprintf(buffer, 128, format, ap);
                WriteConsole(hConsole, buffer, cnt, &nWritten, NULL);
                va_end(ap);
#endif
    		va_start(ap, format);
    		fprintf(stderr, "Error: ");
    		ret = vfprintf(stderr, format, ap);
    		va_end(ap);
  	}

  	if (flog == NULL)
    		flog = fopen(DISP_FILE, "wt");
  	else {
    		va_start(ap, format);
    		if (flog)
      			fprintf(flog, "Error: ");
    		if (flog)
      			vfprintf(flog, format, ap);
    		va_end(ap);
  	}

  	return ret;
}


/* 
   Set the verbosity level
*/
TIEXPORT int TICALL ticable_DISPLAY_settings(TicableDisplay op)
{
  	switch (op) {
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

TIEXPORT int TICALL ticable_verbose_settings(TicableDisplay op)
{
  	return ticable_DISPLAY_settings(op);
}

/*
	Change the log file
*/
TIEXPORT int TICALL ticable_verbose_set_file(const char *filename)
{
  	if (flog != NULL) {
    		fclose(flog);
    		free(fname);
	}

	fname = strdup(filename);
  	flog = fopen(filename, "wt");
  	if (flog != NULL) {
  		DISPLAY("flushing error (%s).\n", strerror(errno));
    		return -1;
	}

  	return 0;
}

/*
  Flush file
*/
TIEXPORT int TICALL ticable_verbose_flush_file(void)
{
	// fflush does not work under win32
	fflush(stdout);
	fflush(flog);

	return 0;
}

/************ Unused/Obsoleted *****************/


static FILE *old = NULL;	// old stream pointer
static FILE *f = NULL;


/* 
   This function is equivalent to 'fprintf(out, ...)'
   if the VERBOSE constant is defined.
   Default behaviour: out = stdout;
*/
TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_stream(FILE * stream)
{
//      old = out;
  //out = stream;
  return old;
}

TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_file(char *filename)
{
  f = fopen(filename, "wb");
  return f;
}

TIEXPORT int TICALL ticable_DISPLAY_close_file()
{
  return fclose(f);
}

#endif
