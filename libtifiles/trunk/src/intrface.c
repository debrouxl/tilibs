/* Hey EMACS -*- linux-c -*- */
/*  libtifiles - TI File Format library
 *  Copyright (C) 2002-2003  Romain Lievin
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

#include "intl.h"

#include "export.h"
#include "file_ver.h"
#include "file_int.h"
#include "file_def.h"
#include "typesxx.h"
#include "trans.h"
#include "grp_ops.h"

/*****************/
/* Internal data */
/*****************/
int tifiles_instance = 0;	// counts # of instances
TicalcType tifiles_calc_type = CALC_NONE;	// current calc type (context)

TIFILES_PRINTF tifiles_printf = printf;

/****************/
/* Entry points */
/****************/

/*
  This function should be the first one to call.
  It tries to list available I/O functions (I/O resources).
 */
TIEXPORT int TICALL tifiles_init()
{
#ifdef __WIN32__
  HANDLE hDll;
  char LOCALEDIR[65536];
  int i;

  hDll = GetModuleHandle("ticables.dll");
  GetModuleFileName(hDll, LOCALEDIR, 65535);
  for (i = strlen(LOCALEDIR); i >= 0; i--) {
    if (LOCALEDIR[i] == '\\')
      break;
  }
  LOCALEDIR[i] = '\0';
  strcat(LOCALEDIR, "\\locale");
#endif

#if defined(ENABLE_NLS)
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  //fprintf(stdout, "%s: bindtextdomain to %s\n", PACKAGE, LOCALEDIR);
  //bind_textdomain_codeset(PACKAGE, "ISO-8859-15");
  textdomain(PACKAGE);
#endif

  if (tifiles_instance == 0) {
    fprintf(stdout, _("Libtifiles: version %s\n"), LIBTIFILES_VERSION);
  }

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
    fprintf(stderr,
	    _
	    ("Function not implemented. This is a bug. Please report it."));
    fprintf(stderr, _("Informations:\n"));
    fprintf(stderr, _("Calc: %i\n"), type);
    fprintf(stderr, _("Program halted before crashing...\n"));
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
  fprintf(stdout, _("Libtifiles settings...\n"));

  switch (tifiles_calc_type) {
  case CALC_V200:
    fprintf(stdout, _("  Calc type: %s\n"), "V200");
    break;
  case CALC_TI92P:
    fprintf(stdout, _("  Calc type: %s\n"), "TI92+");
    break;
  case CALC_TI92:
    fprintf(stdout, _("  Calc type: %s\n"), "TI92");
    break;
  case CALC_TI89:
    fprintf(stdout, _("  Calc type: %s\n"), "TI89");
    break;
  case CALC_TI86:
    fprintf(stdout, _("  Calc type: %s\n"), "TI86");
    break;
  case CALC_TI85:
    fprintf(stdout, _("  Calc type: %s\n"), "TI85");
    break;
  case CALC_TI83P:
    fprintf(stdout, _("  Calc type: %s\n"), "TI83+");
    break;
  case CALC_TI83:
    fprintf(stdout, _("  Calc type: %s\n"), "TI83");
    break;
  case CALC_TI82:
    fprintf(stdout, _("  Calc type: %s\n"), "TI82");
    break;
  case CALC_TI73:
    fprintf(stdout, _("  Calc type: %s\n"), "TI73");
    break;
  default:			// error !
    fprintf(stdout, _("Oops, there is a bug. Unknown calculator.\n"));
    break;
  }
}


TIEXPORT TIFILES_PRINTF tifiles_set_printf(TIFILES_PRINTF new_printf)
{
  TIFILES_PRINTF old_printf = tifiles_printf;

  fprintf(stderr, "printf = %p\n", printf);
  fprintf(stderr, "old_printf = %p\n", old_printf);
  fprintf(stderr, "new_printf = %p\n", new_printf);

  tifiles_printf = new_printf;

  return old_printf;
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
#if defined(__WIN32__)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,	// handle to DLL module
		    DWORD fdwReason,	// reason for calling function
		    LPVOID lpvReserved)	// reserved);
{
  /*#ifdef ENABLE_NLS
     char buffer[65536];
     HINSTANCE hDLL = hinstDLL;
     int i=0;

     GetModuleFileName(hinstDLL, buffer, 65535);
     for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
     buffer[i]='\0';
     strcat(buffer, "\\locale\\");

     bindtextdomain (PACKAGE, buffer);  
     //textdomain (PACKAGE);
     #endif */
  return TRUE;
}
#endif
