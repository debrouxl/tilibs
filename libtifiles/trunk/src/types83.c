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
#include "types83.h"

#ifdef __WIN32__
# define strcasecmp _stricmp
#endif


const char *TI83_CONST[TI83_MAXTYPES + 1][4] = {
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
  {"ZSTO", "83z", "Zoom", N_("Zoom")},
  {"TAB", "83t", "Table Setup", N_("Table Setup")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
  {"", "83?", "Unknown", N_("Unknown")},
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
  return (data < TI83_MAXTYPES) ? TI83_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti83_type2byte(const char *s)
{
  int i;

  for (i = 0; i < TI83_MAXTYPES; i++) {
    if (!strcmp(TI83_CONST[i][0], s))
      break;
  }

  if (i == TI83_MAXTYPES)
    printf
	(_
	 ("Warning: unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the file extension corresponding to the value
const char *ti83_byte2fext(uint8_t data)
{
  return (data < TI83_MAXTYPES) ? TI83_CONST[data][1] : "83?";
}

// Return the value corresponding to the file extension
uint8_t ti83_fext2byte(const char *s)
{
  int i;

  for (i = 0; i < TI83_MAXTYPES; i++) {
    if (!strcasecmp(TI83_CONST[i][1], s))
      break;
  }

  if (i == TI83_MAXTYPES)
    printf
	(_
	 ("Warning: unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the descriptive associated with the vartype
const char *ti83_byte2desc(uint8_t data)
{
  return (data < TI83_MAXTYPES) ? TI83_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti83_byte2icon(uint8_t data)
{
  return (data < TI83_MAXTYPES) ? TI83_CONST[data][3] : "Unknown";
}


