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
#include "gettext.h"

#include "export.h"
#include "types89.h"
#include "printl.h"

#ifdef __WIN32__
# define strcasecmp _stricmp
#endif


/*
  Is missing :
  - Y-Var (.89y)
  - Window Setup(.89w)
  - Zoom (.89z)
  - Table Setup (.89t)
  - Lab Report (89r)
*/
const char *TI89_CONST[TI89_MAXTYPES + 1][4] = {
  {"EXPR", "89e", "Expression", N_("Expression")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"LIST", "89l", "List", N_("List")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"MAT", "89m", "Matrix", N_("Matrix")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"DATA", "89c", "Data", N_("Data")},
  {"TEXT", "89t", "Text", N_("Text")},
  {"STR", "89s", "String", N_("String")},
  {"GDB", "89d", "GDB", N_("GDB")},
  {"FIG", "89a", "Figure", N_("Figure")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"PIC", "89i", "Picture", N_("Picture")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"PRGM", "89p", "Program", N_("Program")},
  {"FUNC", "89f", "Function", N_("Function")},
  {"MAC", "89x", "Macro", N_("Macro")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"CLOCK", "89clk", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"RDIR", "89?", "Unknown", N_("Unknown")},
  {"LDIR", "89?", "Unknown", N_("Unknown")},
  {"STDY", "89y", "Zipped", N_("Zipped")},
  {"BKUP", "89g", "Backup", N_("Backup")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"DIR", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"ASM", "89z", "Asm Program", N_("Asm Program")},
  {"IDLIST", "89idl", "ID-LIST", N_("ID-LIST")},
  {"AMS", "89u", "OS upgrade", N_("OS upgrade")},
  {"APPL", "89k", "Application", N_("Application")},
  {"CERT", "89q", "Certificate", N_("Certificate")},
  {"LOCKED", "89?", "Unknown", N_("Unknown")},
  {"ARCHIV", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},
  {"", "89?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti89_byte2type(uint8_t data)
{
  return (data < TI89_MAXTYPES) ? TI89_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t ti89_type2byte(const char *s)
{
  int i;

  for (i = 0; i < TI89_MAXTYPES; i++) {
    if (!strcmp(TI89_CONST[i][0], s))
      break;
  }

  if (i == TI89_MAXTYPES)
    printl3(1, _("unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the file extension corresponding to the value
const char *ti89_byte2fext(uint8_t data)
{
  return (data < TI89_MAXTYPES) ? TI89_CONST[data][1] : "89?";
}

// Return the value corresponding to the file extension
uint8_t ti89_fext2byte(const char *s)
{
  int i;

  for (i = 0; i < TI89_MAXTYPES; i++) {
    if (!strcasecmp(TI89_CONST[i][1], s))
      break;
  }

  if (i == TI89_MAXTYPES)
    printl3(1, _("unknown type. It is a bug. Please report this information.\n"));

  return i;
}

// Return the descriptive associated with the vartype
const char *ti89_byte2desc(uint8_t data)
{
  return (data < TI89_MAXTYPES) ? TI89_CONST[data][3] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *ti89_byte2icon(uint8_t data)
{
  return (data < TI89_MAXTYPES) ? TI89_CONST[data][2] : "Unknown";
}
