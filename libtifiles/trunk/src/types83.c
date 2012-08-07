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
#include "types83.h"
#include "logging.h"

const char *TI83_CONST[TI83_MAXTYPES + 1][4] = 
{
  {"REAL", "83n", "Real", N_("Real")},
  {"LIST", "83l", "List", N_("List")},
  {"MAT", "83m", "Matrix", N_("Matrix")},
  {"YVAR", "83y", "Y-Var", N_("Y-Var")},
  {"STR", "83s", "String", N_("String")},
  {"PRGM", "83p", "Program", N_("Program")},
  {"ASM", "83p", "Asm Program", N_("Asm Program")},
  {"PIC", "83i", "Picture", N_("Picture")},
  {"GDB", "83d", "GDB", N_("GDB")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"CPLX", "83c", "Complex", N_("Complex")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"WINDW", "83w", "Window", N_("Window Setup")},
  {"ZSTO", "83z", "Zoom", N_("Zoom")},
  {"TABLE", "83t", "Table Setup", N_("Table Setup")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"BKUP", "83b", "Backup", N_("Backup")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"DIR", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti83_byte2type(uint8_t data)
{
	//if(data >= TI83_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83_MAXTYPES) ? TI83_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti83_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83_MAXTYPES; i++) 
	{
		if (!strcmp(TI83_CONST[i][0], s))
			break;
	}

	//if (i == TI83_MAXTYPES)	tifiles_warning(_("ti83_type2byteunknown type.\n"));
  return i;
}

// Return the file extension corresponding to the value
const char *ti83_byte2fext(uint8_t data)
{
	//if(data >= TI83_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83_MAXTYPES) ? TI83_CONST[data][1] : "83?";
}

// Return the value corresponding to the file extension
uint8_t ti83_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI83_CONST[i][1], s))
			break;
	}

	//if (i == TI83_MAXTYPES)	tifiles_warning( _("ti83_fext2byte: unknown type.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti83_byte2desc(uint8_t data)
{
	//if(data >= TI83_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83_MAXTYPES) ? TI83_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti83_byte2icon(uint8_t data)
{
	//if(data >= TI83_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83_MAXTYPES) ? TI83_CONST[data][3] : "Unknown";
}

#endif
