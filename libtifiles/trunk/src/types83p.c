/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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

/*
  Variable type ID and file extensions
*/

#include <stdio.h>
#include <string.h>
#include "intl3.h"

#include "export.h"
#include "types83p.h"

#ifdef __WIN32__
# define strcasecmp _stricmp
#endif


const char *TI83p_CONST[TI83p_MAXTYPES + 1][4] = {
  {"REAL", "8xn", "Real", N_("Real")},
  {"LIST", "8xl", "List", N_("List")},
  {"MAT", "8xm", "Matrix", N_("Matrix")},
  {"EQU", "8xe", "Equation", N_("Equation")},
  {"STR", "8xs", "String", N_("String")},
  {"PRGM", "8xp", "Program", N_("Program")},
  {"ASM", "8xz", "Asm Program", N_("Asm Program")},
  {"PIC", "8xi", "Picture", N_("Picture")},
  {"GDB", "8xg", "GDB", N_("GDB")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"WDW", "8xw", "Window", N_("Window")},
  {"CPLX", "8xc", "Complex", N_("Complex")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"WDW", "8xw", "Window Setup", N_("Window Setup")},
  {"ZSTO", "8xz", "Zoom", N_("Zoom")},
  {"TAB", "8xt", "Table Setup", N_("Table Setup")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"BKUP", "8xb", "Backup", N_("Backup")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"APPV", "8xv", "App Var", N_("App Var")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"GRP", "8x?", "Group Var", N_("Group Var")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"DIR", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"AMS", "8xu", "OS upgrade", N_("OS upgrade")},
  {"APPL", "8xk", "Application", N_("Application")},
  {"CERT", "8xq", "Certificate", N_("Certificate")},
  {"IDLIST", "8xidl", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},
  {"", "8x?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti83p_byte2type(uint8_t data)
{
  return (data < TI83p_MAXTYPES) ? TI83p_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti83p_type2byte(const char *s)
{
  int i;

  for (i = 0; i < TI83p_MAXTYPES; i++) {
    if (!strcmp(TI83p_CONST[i][0], s))
      break;
  }

  if (i == TI83p_MAXTYPES)
    printl(1, _("unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the file extension corresponding to the value
const char *ti83p_byte2fext(uint8_t data)
{
  return (data < TI83p_MAXTYPES) ? TI83p_CONST[data][1] : "8x?";
}

// Return the value corresponding to the file extension
uint8_t ti83p_fext2byte(const char *s)
{
  int i;

  for (i = 0; i < TI83p_MAXTYPES; i++) {
    if (!strcasecmp(TI83p_CONST[i][1], s))
      break;
  }

  if (i == TI83p_MAXTYPES)
    printl(1, _("unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the descriptive associated with the vartype
const char *ti83p_byte2desc(uint8_t data)
{
  return (data < TI83p_MAXTYPES) ? TI83p_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti83p_byte2icon(uint8_t data)
{
  return (data < TI83p_MAXTYPES) ? TI83p_CONST[data][3] : "Unknown";
}


