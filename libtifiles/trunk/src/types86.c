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
#include "types86.h"
#include "logging.h"

const char *TI86_CONST[TI86_MAXTYPES + 1][4] = 
{
  {"REAL", "86n", "Real", N_("Real")},
  {"CPLX", "86c", "Complex", N_("Complex")},
  {"VECT", "86v", "Vector", N_("Vector")},
  {"CVECT", "86v", "Complex Vector", N_("Complex Vector")},
  {"LIST", "86l", "List", N_("List")},
  {"CLIST", "86l", "Complex List", N_("Complex List")},
  {"MAT", "86m", "Matrix", N_("Matrix")},
  {"CMAT", "86m", "Complex Matrix", N_("Complex Matrix")},
  {"CONS", "86k", "Constant", N_("Constant")},
  {"CCONS", "86k", "Complex Constant", N_("Complex Constant")},
  {"EQU", "86y", "Equation", N_("Equation")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"STR", "86s", "String", N_("String")},
  {"GDB", "86d", "GDB", N_("GDB")},
  {"GDB", "86d", "GDB", N_("GDB")},
  {"GDB", "86d", "GDB", N_("GDB")},
  {"GDB", "86d", "GDB", N_("GDB")},
  {"PIC", "86i", "Picture", N_("Picture")},
  {"PRGM", "86p", "Program", N_("Program")},
  {"RANGE", "86r", "Range", N_("Range")},
  {"SCRN", "86?", "Screen", N_("Screen")},
  {"DIR", "86?", "Directory", N_("Directory")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"WIND", "86w", "Func", N_("Window Setup")},
  {"WIND", "86w", "Pol", N_("Window Setup")},
  {"WIND", "86w", "Param", N_("Window Setup")},
  {"WIND", "86w", "DifEq", N_("Window Setup")},
  {"WIND", "86w", "ZRCL", N_("Window Setup")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"BKUP", "86b", "Backup", N_("Backup")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},
  {"", "86?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti86_byte2type(uint8_t data)
{
	//if(data >= TI86_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI86_MAXTYPES) ? TI86_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti86_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI86_MAXTYPES; i++) 
	{
		if (!strcmp(TI86_CONST[i][0], s))
			break;
	}

	//if (i == TI86_MAXTYPES) tifiles_warning(_("ti86_type2byte: unknown type."));
  return i;
}

// Return the file extension corresponding to the value
const char *ti86_byte2fext(uint8_t data)
{
	//if(data >= TI86_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI86_MAXTYPES) ? TI86_CONST[data][1] : "86?";
}

// Return the value corresponding to the file extension
uint8_t ti86_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI86_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI86_CONST[i][1], s))
			break;
	}

	//if (i == TI86_MAXTYPES) tifiles_warning(_("ti86_fext2byte: unknown type."));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti86_byte2desc(uint8_t data)
{
	//if(data >= TI86_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI86_MAXTYPES) ? TI86_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti86_byte2icon(uint8_t data)
{
	//if(data >= TI86_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI86_MAXTYPES) ? TI86_CONST[data][3] : "Unknown";
}

#endif
