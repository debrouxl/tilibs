/*  libtifiles - TI File Format library
 *  Copyright (C) 2002  Romain Lievin
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

#include "export.h"
#include "types82.h"


const char *TI82_CONST[TI82_MAXTYPES][3] =
  {
    { "REAL",  "82n", "Real" },
    { "LIST",  "82l", "List" },
    { "MAT",   "82m", "Matrix" },
    { "YVAR",  "82y", "Y-Var" },
    { "",      "82?", "Unknown" },
    { "PRGM",  "82p", "Program" },
    { "PPGM",  "82p", "Protected Program" },
    { "PIC",   "82i", "Picture" },
    { "GDB",   "82d", "GDB" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "WDW",   "82w", "Window Setup" },
    { "ZSTO",  "82z", "Zoom" },
    { "TAB",   "82t", "Table Setup" },
    { "LCD",   "82?", "LCD" },
    { "BKUP",  "82b", "Backup" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
    { "",      "82?", "Unknown" },
};

// Return the type corresponding to the value
const char *ti82_byte2type(uint8_t data)
{
  if(data > TI82_MAXTYPES) return NULL;
  return TI82_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti82_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI82_MAXTYPES; i++)
    {
      if(!strcmp(TI82_CONST[i][0], s)) break;
    }
  if(i>TI82_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti82_byte2fext(uint8_t data)
{
  if(data > TI82_MAXTYPES) return NULL;
  return TI82_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti82_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI82_MAXTYPES; i++)
    {
      if(!strcasecmp(TI82_CONST[i][1], s)) break;
    }
  if(i > TI82_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti82_byte2desc(uint8_t data)
{
  if(data > TI82_MAXTYPES) return NULL;
  return TI82_CONST[data][2];
}
