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

#ifndef DISABLE_TI9X

#include <string.h>
#include "gettext.h"
#include "types92.h"
#include "logging.h"

/*
  Is missing:
  Y-Var (.89y)
  Window Setup(.89w)
  Zoom (.89z)
  Table Setup (.89t)
  Lab Report (92r)
*/
const char *TI92_CONST[TI92_MAXTYPES + 1][4] = 
{
  {"EXPR", "92e", "Expression", N_("Expression")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"LIST", "92l", "List", N_("List")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"MAT", "92m", "Matrix", N_("Matrix")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"DATA", "92c", "Data", N_("Data")},
  {"TEXT", "92t", "Text", N_("Text")},
  {"STR", "92s", "String", N_("String")},
  {"GDB", "92d", "GDB", N_("GDB")},
  {"FIG", "92a", "Figure", N_("Figure")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"PIC", "92i", "Picture", N_("Picture")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"PRGM", "92p", "Program", N_("Program")},
  {"FUNC", "92f", "Function", N_("Function")},
  {"MAC", "92x", "Macro", N_("Macro")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"ZIP", "92?", "Zipped", N_("Zipped")},
  {"BKUP", "92b", "Backup", N_("Backup")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"DIR", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"ASM", "92z", "Asm Program", N_("Asm Program")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},
  {"", "92?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti92_byte2type(uint8_t data)
{
	//if(data >= TI92_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI92_MAXTYPES) ? TI92_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti92_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI92_MAXTYPES; i++) 
	{
		if (!strcmp(TI92_CONST[i][0], s))
			break;
	}

	//if (i == TI92_MAXTYPES) tifiles_warning( _("ti92_type2byte: unknown type."));
	return i;
}

// Return the file extension corresponding to the value
const char *ti92_byte2fext(uint8_t data)
{
	//if(data >= TI92_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI92_MAXTYPES) ? TI92_CONST[data][1] : "92?";
}

// Return the value corresponding to the file extension
uint8_t ti92_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI92_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI92_CONST[i][1], s))
			break;
	}

	//if (i == TI92_MAXTYPES)	tifiles_warning(_("ti92_fext2byte: unknown type."));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti92_byte2desc(uint8_t data)
{
	//if(data >= TI92_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI92_MAXTYPES) ? TI92_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti92_byte2icon(uint8_t data)
{
	//if(data >= TI92_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI92_MAXTYPES) ? TI92_CONST[data][3] : "Unknown";
}

#endif
