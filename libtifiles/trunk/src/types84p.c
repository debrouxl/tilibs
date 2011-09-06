/* Hey EMACS -*- linux-c -*- */
/* $Id: types84p.c 908 2005-03-29 19:16:56Z roms $ */

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
#include "types84p.h"
#include "logging.h"

const char *TI84p_CONST[TI84p_MAXTYPES + 1][4] = 
{
  {"REAL", "8Xn", "Real", N_("Real")},
  {"LIST", "8Xl", "List", N_("List")},
  {"MAT", "8Xm", "Matrix", N_("Matrix")},
  {"EQU", "8Xe", "Equation", N_("Equation")},
  {"STR", "8Xs", "String", N_("String")},
  {"PRGM", "8Xp", "Program", N_("Program")},
  {"PPRGM", "8Xp", "Program", N_("Program")},
  {"PIC", "8Xi", "Picture", N_("Picture")},
  {"GDB", "8Xg", "GDB", N_("GDB")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"WINDW", "8Xw", "Window Setup", N_("Window Setup")},
  {"CPLX", "8Xc", "Complex", N_("Complex")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"WINDW", "8Xw", "Window Setup", N_("Window Setup")},
  {"ZSTO", "8Xz", "Zoom", N_("Zoom")},
  {"TABLE", "8Xt", "Table Setup", N_("Table Setup")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"BKUP", "8Xb", "Backup", N_("Backup")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"APPV", "8Xv", "App Var", N_("App Var")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"GRP", "8Xo", "Group Var", N_("Group Var")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"DIR", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"AMS", "8Xu", "OS upgrade", N_("OS upgrade")},
  {"APPL", "8Xk", "Application", N_("Application")},
  {"CERT", "8Xq", "Certificate", N_("Certificate")},
  {"IDLIST", "8Xidl", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},
  {"", "8X?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti84p_byte2type(uint8_t data)
{
	//if(data >= TI84p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI84p_MAXTYPES) ? TI84p_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti84p_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI84p_MAXTYPES; i++) 
	{
		if (!strcmp(TI84p_CONST[i][0], s))
			break;
	}

	//if (i == TI84p_MAXTYPES) tifiles_warning(_("ti84p_byte2type: unknown type.\n"));
	return i;
}

// Return the file extension corresponding to the value
const char *ti84p_byte2fext(uint8_t data)
{
	//if(data >= TI84p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI84p_MAXTYPES) ? TI84p_CONST[data][1] : "8X?";
}

// Return the value corresponding to the file extension
uint8_t ti84p_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI84p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI84p_CONST[i][1], s))
			break;
	}

	//if (i == TI84p_MAXTYPES) tifiles_warning(("ti84p_fext2byte: unknown type.\n"));
	return i;
}

// Return the descriptive associated with the vartype
const char *ti84p_byte2desc(uint8_t data)
{
	//if(data >= TI84p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI84p_MAXTYPES) ? TI84p_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti84p_byte2icon(uint8_t data)
{
	//if(data >= TI84p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI84p_MAXTYPES) ? TI84p_CONST[data][3] : "Unknown";
}

#endif
