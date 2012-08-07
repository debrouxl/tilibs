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
#include "types73.h"
#include "logging.h"

/* is missing:
   { "VECT",  "73v", "Vector" },
   { "STR",   "73s", "String" },
    { "CONST", "73c", "Constant" },
*/

const char *TI73_CONST[TI73_MAXTYPES + 1][4] = 
{
  {"REAL", "73n", "Real", N_("Real")},
  {"LIST", "73l", "List", N_("List")},
  {"MAT", "73m", "Matrix", N_("Matrix")},
  {"EQU", "73e", "Equation", N_("Equation")},
  {"STR", "73s", "String", N_("String")},
  {"PRGM", "73p", "Program", N_("Program")},
  {"ASM", "73z", "Asm Program", N_("Asm Program")},
  {"PIC", "73i", "Picture", N_("Picture")},
  {"GDB", "73g", "GDB", N_("GDB")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"WDW", "73w", "Window Setup", N_("Window Setup")},
  {"CPLX", "73c", "Complex", N_("Complex")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"WINDW", "73w", "Window", N_("Window Setup")},
  {"ZSTO", "73z", "Zoom", N_("Zoom")},
  {"TABLE", "73t", "Table Setup", N_("Table Setup")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"BKUP", "73b", "Backup", N_("Backup")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"DIR", "73?", "Unknown", N_("Unknown")},
  {"AVAR", "73v", "App Var", N_("App Var")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"AMS", "73u", "OS upgrade", N_("OS upgrade")},
  {"APPL", "73k", "Application", N_("Application")},
  {"CERT", "73q", "Certificate", N_("Certificate")},
  {"IDLIST", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},
  {"", "73?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti73_byte2type(uint8_t data)
{
	//if(data >= TI73_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI73_MAXTYPES) ? TI73_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti73_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI73_MAXTYPES; i++) 
	{
		if (!strcmp(TI73_CONST[i][0], s))
			break;
	}

	//if (i == TI73_MAXTYPES) tifiles_warning(_("ti73_type2byte: unknown type.\n"));
	return i;
}

// Return the file extension corresponding to the value
const char *ti73_byte2fext(uint8_t data)
{
	//if(data >= TI73_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI73_MAXTYPES) ? TI73_CONST[data][1] : "73?";
}

// Return the value corresponding to the file extension
uint8_t ti73_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI73_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI73_CONST[i][1], s))
			break;
	}

	//if (i == TI73_MAXTYPES) tifiles_warning(_("ti73_fext2byte: unknown type.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti73_byte2desc(uint8_t data)
{
	//if(data >= TI73_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI73_MAXTYPES) ? TI73_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti73_byte2icon(uint8_t data)
{
	//if(data >= TI73_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI73_MAXTYPES) ? TI73_CONST[data][3] : "Unknown";
}

#endif
