/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32__
#include <windows.h>
#endif

#include "gettext.h"

#include "headers.h"
#include "calc_ver.h"
#include "calc_err.h"
#include "calc_def.h"
#include "tixx.h"
#include "type2str.h"
#include "printl.h"

/*****************/
/* Internal data */
/*****************/

TicableLinkCable *cable;
TicalcInfoUpdate *update;
TicalcType ticalcs_calc_type;
int lock = 0;

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticalcs_instance = 0;	// counts # of instances

/*
  This function should be the first one to call.
*/
TIEXPORT int TICALL ticalc_init()
{
	char locale_dir[65536];
#ifdef __WIN32__
	HANDLE hDll;
  	int i;
  	
  	// Get library path
  	hDll = GetModuleHandle("ticables.dll");
  	GetModuleFileName(hDll, locale_dir, 65535);
  	
  	for (i = strlen(locale_dir); i >= 0; i--) {
    		if (locale_dir[i] == '\\')
      			break;
  	}
  	
  	locale_dir[i] = '\0';
  	strcat(locale_dir, "\\locale");
#else
	strcpy(locale_dir, LOCALEDIR);
#endif

	if (ticalcs_instance)
		return (++ticalcs_instance);
	printl2(0, _("ticalcs library version %s\n"), LIBTICALCS_VERSION);

#if defined(ENABLE_NLS)
	// Init i18n support
	printl2(0, "setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	printl2(0, "bindtextdomain: <%s>\n", bindtextdomain(PACKAGE, LOCALEDIR));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	printl2(0, "textdomain: <%s>\n", textdomain(PACKAGE));
#endif

	// Check version
	if (strcmp(tifiles_get_version(), LIBCALCS_REQUIRES_LIBFILES_VERSION) < 0) {
    		printl2(0, _("Libtifiles: version mismatches. Library version >= <%s> is required.\n"),
	    		LIBCALCS_REQUIRES_LIBFILES_VERSION);
    		exit(-1);
	}	
	if (strcmp(ticable_get_version(), LIBCALCS_REQUIRES_LIBCABLES_VERSION) < 0) {
    		printl2(0, _("Libticables: version mismatches. Library version >= <%s> is required.\n"),
	    		LIBCALCS_REQUIRES_LIBCABLES_VERSION);
    		exit(-1);
	}

	// Init sub-libraries
  	tifiles_init();
  	ticable_init();

  	return (++ticalcs_instance);
}

/*
  This function should be called when the libticalcs library is
  no longer used.
*/
TIEXPORT int TICALL ticalc_exit()
{
	ticable_exit();
  	tifiles_exit();

  	return (--ticalcs_instance);
}

/***********/
/* Methods */
/***********/

/* Return the version number of the library */
TIEXPORT const char *TICALL ticalc_get_version()
{
  return LIBTICALCS_VERSION;
}

/* 
   Set up the update functions in order to be independant of the GUI
*/
TIEXPORT
    void TICALL ticalc_set_update(TicalcInfoUpdate * iu,
				  void (*start) (void),
				  void (*stop) (void),
				  void (*refresh) (void),
				  void (*pbar) (void),
				  void (*label) (void))
{
  iu->cancel = 0;
  iu->start = start;
  iu->stop = stop;
  iu->refresh = refresh;
  iu->pbar = pbar;
  iu->label = label;

  update = iu;
}


/* Set up link cable to use for calculator access */
TIEXPORT void TICALL ticalc_set_cable(TicableLinkCable * lc)
{
  cable = lc;
}

extern TicalcFncts *tcf;
int tixx_directorylist2(TNode ** vars, TNode ** apps, uint32_t * memory);

/*
  Set up the calculator functions according to the calculator type
*/
TIEXPORT void TICALL ticalc_set_calc(TicalcType type, TicalcFncts * calc)
{
  ticalcs_calc_type = type;
  tifiles_set_calc(ticalcs_calc_type);

#ifdef ENABLE_NLS
#if !defined(__WIN32__)
  bindtextdomain(PACKAGE, NULL);
  textdomain(PACKAGE);
#endif
#endif

  printl2(0, _("settings:\n"));
  printl2(0, _("  calc type: %s\n"),
	  tifiles_calctype_to_string(ticalcs_calc_type));

  tcf = calc;
  memset(calc, 0, sizeof(TicalcFncts));
  //bzero(calc, sizeof(TicalcFncts));
  calc->directorylist2 = tixx_directorylist2;

  switch (type) {
  case CALC_TI92:
    calc->supported_operations = ti92_supported_operations;
    calc->is_silent = !0;
    calc->has_folder = !0;
    calc->memory = MEMORY_USED;
    calc->is_flash = 0;

    calc->isready = ti92_isready;
    calc->send_key = ti92_send_key;
    calc->screendump = ti92_screendump;
    calc->recv_backup = ti92_recv_backup;
    calc->send_backup = ti92_send_backup;
    calc->directorylist = ti92_directorylist;
    calc->recv_var = ti92_recv_var;
    calc->send_var = ti92_send_var;
    calc->dump_rom = ti92_dump_rom;
    calc->send_flash = ti92_send_flash;
    calc->recv_flash = ti92_recv_flash;
    calc->get_idlist = ti92_get_idlist;
    break;

  case CALC_V200:
  case CALC_TI92P:
  case CALC_TI89:
  case CALC_TI89T:
    calc->supported_operations = ti89_supported_operations;
    calc->is_silent = !0;
    calc->has_folder = !0;
    calc->memory = MEMORY_USED;
    calc->is_flash = !0;

    calc->isready = ti89_isready;
    calc->send_key = ti89_send_key;
    calc->screendump = ti89_screendump;
    calc->recv_backup = ti89_recv_backup;
    calc->send_backup = ti89_send_backup;
    calc->directorylist = ti89_directorylist;
    calc->recv_var = ti89_recv_var;
    calc->send_var = ti89_send_var;
    calc->dump_rom = ti89_dump_rom;
    calc->send_flash = ti89_send_flash;
    calc->recv_flash = ti89_recv_flash;
    calc->get_idlist = ti89_get_idlist;
    calc->get_clock = ti89_get_clock;
    calc->set_clock = ti89_set_clock;
    break;

  case CALC_TI86:
    calc->supported_operations = ti86_supported_operations;
    calc->is_silent = !0;
    calc->has_folder = 0;
    calc->memory = MEMORY_FREE;
    calc->is_flash = 0;

    calc->isready = ti86_isready;
    calc->send_key = ti86_send_key;
    calc->screendump = ti86_screendump;
    calc->recv_backup = ti86_recv_backup;
    calc->send_backup = ti86_send_backup;
    calc->directorylist = ti86_directorylist;
    calc->recv_var = ti86_recv_var;
    calc->send_var = ti86_send_var;
    calc->dump_rom = ti86_dump_rom;
    calc->send_flash = ti86_send_flash;
    calc->recv_flash = ti86_recv_flash;
    calc->get_idlist = ti86_get_idlist;
    break;

  case CALC_TI85:
    calc->supported_operations = ti85_supported_operations;
    calc->is_silent = 0;
    calc->has_folder = 0;
    calc->memory = MEMORY_NONE;
    calc->is_flash = 0;

    calc->isready = ti85_isready;
    calc->send_key = ti85_send_key;
    calc->screendump = ti85_screendump;
    calc->recv_backup = ti85_recv_backup;
    calc->send_backup = ti85_send_backup;
    calc->directorylist = ti85_directorylist;
    calc->recv_var = ti85_recv_var;
    calc->send_var = ti85_send_var;
    calc->dump_rom = ti85_dump_rom;
    calc->send_flash = ti85_send_flash;
    calc->recv_flash = ti85_recv_flash;
    calc->get_idlist = ti85_get_idlist;
    break;

  case CALC_TI83:
    calc->supported_operations = ti83_supported_operations;
    calc->is_silent = !0;
    calc->has_folder = 0;
    calc->memory = MEMORY_FREE;
    calc->is_flash = 0;

    calc->isready = ti83_isready;
    calc->send_key = ti83_send_key;
    calc->screendump = ti83_screendump;
    calc->recv_backup = ti83_recv_backup;
    calc->send_backup = ti83_send_backup;
    calc->directorylist = ti83_directorylist;
    calc->recv_var = ti83_recv_var;
    calc->send_var = ti83_send_var;
    calc->dump_rom = ti83_dump_rom;
    calc->send_flash = ti83_send_flash;
    calc->recv_flash = ti83_recv_flash;
    calc->get_idlist = ti83_get_idlist;
    break;

  case CALC_TI82:
    calc->supported_operations = ti82_supported_operations;
    calc->is_silent = 0;
    calc->has_folder = 0;
    calc->memory = MEMORY_NONE;
    calc->is_flash = 0;

    calc->isready = ti82_isready;
    calc->send_key = ti82_send_key;
    calc->screendump = ti82_screendump;
    calc->recv_backup = ti82_recv_backup;
    calc->send_backup = ti82_send_backup;
    calc->directorylist = ti82_directorylist;
    calc->recv_var = ti82_recv_var;
    calc->send_var = ti82_send_var;
    calc->dump_rom = ti82_dump_rom;
    calc->send_flash = ti82_send_flash;
    calc->recv_flash = ti82_recv_flash;
    calc->get_idlist = ti82_get_idlist;
    break;

  case CALC_TI83P:
  case CALC_TI73:
    calc->supported_operations = ti73_supported_operations;
    calc->is_silent = !0;
    calc->has_folder = 0;
    calc->memory = MEMORY_FREE;
    calc->is_flash = !0;

    calc->isready = ti73_isready;
    calc->send_key = ti73_send_key;
    calc->screendump = ti73_screendump;
    calc->recv_backup = ti73_recv_backup;
    calc->send_backup = ti73_send_backup;
    calc->directorylist = ti73_directorylist;
    calc->recv_var = ti73_recv_var;
    calc->send_var = ti73_send_var;
    calc->dump_rom = ti73_dump_rom;
    calc->send_flash = ti73_send_flash;
    calc->recv_flash = ti73_recv_flash;
    calc->get_idlist = ti73_get_idlist;
    break;

  default:
    printl2(2, _("Function not implemented. This is a bug. Please report it."));
    printl2(2, _("Informations:\n"));
    printl2(2, _("Calc: %i\n"), type);
    printl2(2, _("Program halted before crashing...\n"));
    exit(-1);
    break;
  }
}

/*
  Set up the calculator functions according to the calculator type
*/
TIEXPORT int TICALL ticalc_get_calc(TicalcType * type)
{
  *type = ticalcs_calc_type;
  return 0;
}

TIEXPORT int TICALL ticalc_return_calc(void)
{
  return ticalcs_calc_type;
}


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
