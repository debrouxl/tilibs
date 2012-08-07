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
#include "typesv2.h"
#include "logging.h"

/*
  Is missing :
  - Y-Var (.v2y)
  - Window Setup(.v2w)
  - Zoom (.v2z)
  - Table Setup (.v2t)
  - Lab Report (v2r)
*/
const char *V200_CONST[V200_MAXTYPES + 1][4] = {
  {"EXPR", "v2e", "Expression", N_("Expression")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"LIST", "v2l", "List", N_("List")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"MAT", "v2m", "Matrix", N_("Matrix")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"DATA", "v2c", "Data", N_("Data")},
  {"TEXT", "v2t", "Text", N_("Text")},
  {"STR", "v2s", "String", N_("String")},
  {"GDB", "v2d", "GDB", N_("GDB")},
  {"FIG", "v2a", "Figure", N_("Figure")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"PIC", "v2i", "Picture", N_("Picture")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"PRGM", "v2p", "Program", N_("Program")},
  {"FUNC", "v2f", "Function", N_("Function")},
  {"MAC", "v2x", "Macro", N_("Macro")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"RDIR", "v2?", "Unknown"},
  {"LDIR", "v2?", "Unknown"},
  {"ZIP", "v2y", "Zipped", N_("Zipped")},
  {"BKUP", "v2g", "Backup", N_("Backup")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"DIR", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"ASM", "v2z", "Asm Prog", N_("Asm Program")},
  {"IDLIST", "v2idl", "ID-LIST", N_("ID-LIST")},
  {"AMS", "v2u", "OS upgrade", N_("OS upgrade")},
  {"APPL", "v2k", "Application", N_("Application")},
  {"CERT", "v2q", "Certificate", N_("Certificate")},
  {"LOCKED", "v2?", "Unknown"},
  {"ARCHIV", "v2?", "Unknown"},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},
  {"", "v2?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *v200_byte2type(uint8_t data)
{
	//if(data >= V200_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < V200_MAXTYPES) ? V200_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t v200_type2byte(const char *s)
{
	int i;

	for (i = 0; i < V200_MAXTYPES; i++) 
	{
		if (!strcmp(V200_CONST[i][0], s))
			break;
	}

	//if (i == V200_MAXTYPES) tifiles_warning( _("v200_type2byte: unknown type."));
	return i;
}

// Return the file extension corresponding to the value
const char *v200_byte2fext(uint8_t data)
{
	//if(data >= V200_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < V200_MAXTYPES) ? V200_CONST[data][1] : "v2?";
}

// Return the value corresponding to the file extension
uint8_t v200_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < V200_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(V200_CONST[i][1], s))
			break;
	}

	//if (i == V200_MAXTYPES)	tifiles_warning( _("v200_fext2byte: unknown type.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *v200_byte2desc(uint8_t data)
{
	//if(data >= V200_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < V200_MAXTYPES) ? V200_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *v200_byte2icon(uint8_t data)
{
	//if(data >= V200_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < V200_MAXTYPES) ? V200_CONST[data][3] : "Unknown";
}

#endif
