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
#include "types83.h"


const char *TI83_CONST[TI83_MAXTYPES][3] =
  {
    { "REAL",  "83n", "Real" },
    { "LIST",  "83l", "List" },
    { "MAT",   "83m", "Matrix" },
    { "YVAR",  "83y", "Y-Var" },
    { "STR",   "83s", "String" },
    { "PRGM",  "83p", "Program" },
    { "ASM",   "83p", "Asm Program" },
    { "PIC",   "83i", "Picture" },
    { "GDB",   "83d", "GDB" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "CPLX",  "83c", "Complex" },
    { "ZSTO",  "83z", "Zoom" },
    { "TAB",   "83t", "Table Setup" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "BKUP",  "83b", "Backup" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "DIR",   "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
    { "",      "83?", "Unknown" },
};

// Return the type corresponding to the value
const char *ti83_byte2type(uint8_t data)
{
  if(data > TI83_MAXTYPES) return NULL;
  if(data==TI83_DIR) return "DIR";
  return TI83_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti83_type2byte(const char *s)
{
  int i;
  
  for(i=0; i<TI83_MAXTYPES; i++)
    {
      if(!strcmp(TI83_CONST[i][0], s)) break;
    }
  if(i > TI83_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti83_byte2fext(uint8_t data)
{
  if(data > TI83_MAXTYPES) return NULL;
  return TI83_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti83_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI83_MAXTYPES; i++)
    {
      if(!strcasecmp(TI83_CONST[i][1], s)) break;
    }
  if(i>TI83_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti83_byte2desc(uint8_t data)
{
  if(data > TI83_MAXTYPES) return NULL;
  return TI83_CONST[data][2];
}
