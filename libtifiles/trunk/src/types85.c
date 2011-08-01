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
#include "types85.h"
#include "logging.h"

const char *TI85_CONST[TI85_MAXTYPES + 1][4] = 
{
  {"REAL", "85n", "Real", N_("Real")},
  {"CPLX", "85c", "Complex", N_("Complex")},
  {"VECT", "85v", "Vector", N_("Vector")},
  {"CVECT", "85v", "Complex Vector", N_("Complex Vector")},
  {"LIST", "85l", "List", N_("List")},
  {"CLIST", "85l", "Complex List", N_("Complex List")},
  {"MAT", "85m", "Matrix", N_("Matrix")},
  {"CMAT", "85m", "Complex Matrix", N_("Complex Matrix")},
  {"CONS", "85k", "Constant", N_("Constant")},
  {"CCONS", "85c", "Complex Constant", N_("Complex Constant")},
  {"EQU", "85y", "Equation", N_("Equation")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"STR", "85s", "String", N_("String")},
  {"GDB", "85d", "GDB", N_("GDB")},
  {"GDB", "85d", "GDB", N_("GDB")},
  {"GDB", "85d", "GDB", N_("GDB")},
  {"GDB", "85d", "GDB", N_("GDB")},
  {"PIC", "85i", "Picture", N_("Picture")},
  {"PRGM", "85p", "Program", N_("Program")},
  {"RANGE", "85r", "Range", N_("Range")},
  {"SCRN", "85?", "Screen", N_("Screen")},
  {"DIR", "85?", "Directory", N_("Directory")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"WND", "85w", "Window Setup", N_("Window Setup")},
  {"POLAR", "85?", "Polar", N_("Polar")},
  {"PARAM", "85?", "Parametric", N_("Parametric")},
  {"DIFEQ", "85?", "Diff Equ", N_("Diff Equ")},
  {"ZRCL", "85z", "Zoom", N_("Zoom")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"BKUP", "85b", "Backup", N_("Backup")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"YVAR", "85y", "Y-Var", N_("Y-Var")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},
  {"", "85?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti85_byte2type(uint8_t data)
{
	//if(data >= TI85_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI85_MAXTYPES) ? TI85_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti85_type2byte(const char *s)
{
  int i;

  for (i = 0; i < TI85_MAXTYPES; i++) {
    if (!strcmp(TI85_CONST[i][0], s))
      break;
  }

  //if (i == TI85_MAXTYPES)  tifiles_warning(_("ti85_type2byte: unknown type."));
  return i;
}


// Return the file extension corresponding to the value
const char *ti85_byte2fext(uint8_t data)
{
	//if(data >= TI85_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI85_MAXTYPES) ? TI85_CONST[data][1] : "85?";
}

// Return the value corresponding to the file extension
uint8_t ti85_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI85_MAXTYPES; i++) {
		if (!g_ascii_strcasecmp(TI85_CONST[i][1], s))
			break;
	}

	//if (i == TI85_MAXTYPES)tifiles_warning(_("ti85_fext2byte: unknown type."));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti85_byte2desc(uint8_t data)
{
	//if(data >= TI85_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI85_MAXTYPES) ? TI85_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti85_byte2icon(uint8_t data)
{
	//if(data >= TI85_MAXTYPES)	tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI85_MAXTYPES) ? TI85_CONST[data][3] : "Unknown";
}

#endif
