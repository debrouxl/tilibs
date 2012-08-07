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

static int warnings = 0;

#ifndef DISABLE_TI9X

#include <string.h>
#include "gettext.h"
#include "types92p.h"
#include "logging.h"

/*
  Is missing :
  - Y-Var (.92py)
  - Window Setup(.92pw)
  - Zoom (.92pz)
  - Table Setup (.92pt)
  - Lab Report (.92pr)
*/
const char *TI92p_CONST[TI92p_MAXTYPES + 1][4] = 
{
  {"EXPR", "9xe", "Expression", N_("Expression")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"LIST", "9xl", "List", N_("List")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"MAT", "9xm", "Matrix", N_("Matrix")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"DATA", "9xc", "Data", N_("Data")},
  {"TEXT", "9xt", "Text", N_("Text")},
  {"STR", "9xs", "String", N_("String")},
  {"GDB", "9xd", "GDB", N_("GDB")},
  {"FIG", "9xa", "Figure", N_("Figure")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"PIC", "9xi", "Picture", N_("Picture")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"PRGM", "9xp", "Program", N_("Program")},
  {"FUNC", "9xf", "Function", N_("Function")},
  {"MAC", "9xx", "Macro", N_("Macro")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"RDIR", "9x?", "Unknown", N_("Unknown")},
  {"LDIR", "9x?", "Unknown", N_("Unknown")},
  {"ZIP", "9xy", "Zipped", N_("Zipped")},
  {"BKUP", "9xg", "Backup", N_("Backup")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"DIR", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"ASM", "9xz", "Asm Program", N_("Asm Program")},
  {"IDLIST", "9xidl", "ID-LIST", N_("ID-LIST")},
  {"AMS", "9xu", "OS upgrade", N_("OS upgrade")},
  {"APPL", "9xk", "Application", N_("Application")},
  {"CERT", "9xq", "Certificate", N_("Certificate")},
  {"LOCKED", "9x?", "Unknown", N_("Unknown")},
  {"ARCHIV", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},
  {"", "9x?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti92p_byte2type(uint8_t data)
{
	if(warnings && (data >= TI92p_MAXTYPES)) tifiles_warning(_("types92p: unknown type (%02x).\n"), data);
	return (data < TI92p_MAXTYPES) ? TI92p_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti92p_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI92p_MAXTYPES; i++) 
	{
	    if (!strcmp(TI92p_CONST[i][0], s))
			break;
	}

	//if (i == TI92p_MAXTYPES)	tifiles_warning(_("ti92p_type2byte: unknown type."));
	return i;
}

// Return the file extension corresponding to the value
const char *ti92p_byte2fext(uint8_t data)
{
	if(warnings && (data >= TI92p_MAXTYPES)) tifiles_warning(_("types92p: unknown type (%02x).\n"), data);
	return (data < TI92p_MAXTYPES) ? TI92p_CONST[data][1] : "9x?";
}

// Return the value corresponding to the file extension
uint8_t ti92p_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI92p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI92p_CONST[i][1], s))
			break;
	}

	//if (i == TI92p_MAXTYPES) tifiles_warning( _("unknown type. It is a bug. Please report this information.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti92p_byte2desc(uint8_t data)
{
	if(warnings && (data >= TI92p_MAXTYPES)) tifiles_warning(_("types92p: unknown type (%02x).\n"), data);
	return (data < TI92p_MAXTYPES) ? TI92p_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti92p_byte2icon(uint8_t data)
{
	if(warnings && (data >= TI92p_MAXTYPES)) tifiles_warning(_("types92p: unknown type (%02x).\n"), data);
	return (data < TI92p_MAXTYPES) ? TI92p_CONST[data][3] : "Unknown";
}

#endif
