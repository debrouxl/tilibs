/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  Variable type ID and file extensions
*/

#ifndef DISABLE_TI8X

#include <string.h>
#include "gettext.h"
#include "types82.h"
#include "logging.h"

const char *TI82_CONST[TI82_MAXTYPES + 1][4] = 
{
  {"REAL", "82n", "Real", N_("Real")},
  {"LIST", "82l", "List", N_("List")},
  {"MAT", "82m", "Matrix", N_("Matrix")},
  {"YVAR", "82y", "Y-Var", N_("Y-Var")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"PRGM", "82p", "Program", N_("Program")},
  {"PPGM", "82p", "Protected Program", N_("Protected Program")},
  {"PIC", "82i", "Picture", N_("Picture")},
  {"GDB", "82d", "GDB", N_("GDB")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"WINDW", "82w", "Window Setup", N_("Window Setup")},
  {"ZSTO", "82z", "Zoom", N_("Zoom")},
  {"TABLE", "82t", "Table Setup", N_("Table Setup")},
  {"LCD", "82?", "LCD", N_("LCD")},
  {"BKUP", "82b", "Backup", N_("Backup")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},
  {"", "82?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti82_byte2type(uint8_t data)
{
	//if(data >= TI82_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI82_MAXTYPES) ? TI82_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti82_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI82_MAXTYPES; i++) 
	{
		if (!strcmp(TI82_CONST[i][0], s))
			break;
	}

	//if (i == TI82_MAXTYPES)		tifiles_warning(_("ti82_type2byte: unknown type.\n"));
	return i;
}

// Return the file extension corresponding to the value
const char *ti82_byte2fext(uint8_t data)
{
	//if(data >= TI82_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI82_MAXTYPES) ? TI82_CONST[data][1] : "82?";
}

// Return the value corresponding to the file extension
uint8_t ti82_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI82_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI82_CONST[i][1], s))
			break;
	}

	//if (i == TI82_MAXTYPES)	tifiles_warning(_("ti82_fext2byte: unknown type.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti82_byte2desc(uint8_t data)
{
	//if(data >= TI82_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI82_MAXTYPES) ? TI82_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti82_byte2icon(uint8_t data)
{
	//if(data >= TI82_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI82_MAXTYPES) ? TI82_CONST[data][3] : "Unknown";
}

#endif
