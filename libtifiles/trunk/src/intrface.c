/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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
  This unit contains the interface of the libtifiles library.
*/

#include <stdio.h>
#include <stdlib.h>

#include "intl3.h"

#include "export.h"
#include "file_ver.h"
#include "file_int.h"
#include "file_def.h"
#include "typesxx.h"
#include "trans.h"
#include "grp_ops.h"
#include "printl.h"

/*****************/
/* Internal data */
/*****************/

TicalcType tifiles_calc_type = CALC_NONE;	// current calc type (context)

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int tifiles_instance = 0;	// counts # of instances

/*
  This function should be the first one to call (or the
  second one after tifiles_set_print).
  It tries to list available I/O functions (I/O resources).
 */
TIEXPORT int TICALL tifiles_init()
{
#ifdef __WIN32__
  	HANDLE hDll;
  	char LOCALEDIR[65536];
  	int i;
  	
  	hDll = GetModuleHandle("tifiles.dll");
  	GetModuleFileName(hDll, LOCALEDIR, 65535);
  	for (i = strlen(LOCALEDIR); i >= 0; i--) {
    		if (LOCALEDIR[i] == '\\')
      			break;
  	}
  	
  	LOCALEDIR[i] = '\0';
  	strcat(LOCALEDIR, "\\locale");
#endif

	if (tifiles_instance)
		return (++tifiles_instance);
	printl3(0, _("tifiles library version %s\n"), LIBTIFILES_VERSION);

#if defined(ENABLE_NLS)
	printl3(0, "setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	printl3(0, "bindtextdomain: <%s>\n", bindtextdomain(PACKAGE, LOCALEDIR));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	printl3(0, "textdomain: <%s>\n", textdomain(PACKAGE));
#endif

  	return (++tifiles_instance);
}

/*
  This function should be called when the libticables library is
  no longer used.
 */
TIEXPORT int TICALL tifiles_exit()
{
  	return (--tifiles_instance);
}


/***********/
/* Methods */
/***********/

TIFILES_MSGBOX tifiles_msgbox = NULL;
TIFILES_CHOOSE tifiles_choose = NULL;

TIEXPORT const char *TICALL tifiles_get_version()
{
  return LIBTIFILES_VERSION;
}

static void print_informations();

TIEXPORT void TICALL tifiles_set_calc(TicalcType type)
{
  tifiles_calc_type = type;

  print_informations();

  switch (type) {
  case CALC_TI73:
    break;
  case CALC_TI82:
    break;
  case CALC_TI83:
    break;
  case CALC_TI83P:
    break;
  case CALC_TI85:
    break;
  case CALC_TI86:
    break;
  case CALC_TI89:
    break;
  case CALC_TI92:
    break;
  case CALC_TI92P:
    break;
  case CALC_V200:
    break;
  default:
    printl3(2, _("Function not implemented. There is a bug. Please report it."));
    printl3(2, _("Informations:\n"));
    printl3(2, _("Calc: %i\n"), type);
    printl3(2, _("Program halted before crashing...\n"));
    abort();
    break;
  }
}

TIEXPORT TicalcType TICALL tifiles_get_calc(void)
{
  return tifiles_calc_type;
}

static void print_informations(void)
{
  printl3(0, _("settings:\n"));
  printl3(0, _("  calc type: %s\n"), 
  	tifiles_calctype_to_string(tifiles_calc_type));
}

/* deprecated */
TIEXPORT TIFILES_PRINTF tifiles_set_printf(TIFILES_PRINTF new_printf)
{
#ifdef __WIN32__
#pragma warning( push )
#pragma warning( disable : 4550 )
        return printf;
#pragma warning( pop ) 
#else
	return printf;
#endif
}

#ifdef __WIN32__
TIEXPORT void *TICALL tifiles_calloc(size_t nmemb, size_t size)
{
  return calloc(nmemb, size);
}

TIEXPORT void *TICALL tifiles_malloc(size_t size)
{
  return malloc(size);
}

TIEXPORT void TICALL tifiles_free(void *ptr)
{
  free(ptr);
}

TIEXPORT void *TICALL tifiles_realloc(void *ptr, size_t size)
{
  return realloc(ptr, size);
}
#endif


/* Initialize i18n support under Win32 platforms */
/*
#if defined(__WIN32__)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,	// handle to DLL module
		    DWORD fdwReason,	// reason for calling function
		    LPVOID lpvReserved)	// reserved);
{
#ifdef ENABLE_NLS
     char buffer[65536];
     HINSTANCE hDLL = hinstDLL;
     int i=0;

     GetModuleFileName(hinstDLL, buffer, 65535);
     for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
     buffer[i]='\0';
     strcat(buffer, "\\locale\\");

     bindtextdomain (PACKAGE, buffer);  
     //textdomain (PACKAGE);
#endif 
  return TRUE;
}
#endif
*/
