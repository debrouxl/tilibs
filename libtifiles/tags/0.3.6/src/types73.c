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
#include "types73.h"

/* is missing:
   { "VECT",  "73v", "Vector" },
   { "STR",   "73s", "String" },
    { "CONST", "73c", "Constant" },
*/

const char *TI73_CONST[TI73_MAXTYPES][3] =
  {
    { "REAL",  "73n", "Real" },
    { "LIST",  "73l", "List" },
    { "MAT",   "73m", "Matrix" },
    { "EQU",   "73e", "Equation" },
    { "STR",   "73s", "String" },
    { "PRGM",  "73p", "Program" },
    { "ASM",   "73z", "Asm Program" },
    { "PIC",   "73i", "Picture" },
    { "GDB",   "73g", "GDB" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "WDW",   "73w", "Window Setup" },
    { "CPLX",  "73c", "Complex" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "WDW",   "73w", "Window" },
    { "ZSTO",  "73z", "Zoom" },
    { "TAB",   "73t", "Table Setup" },
    { "",      "73?", "Unknown" },
    { "BKUP",  "73b", "Backup" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "DIR",   "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "AMS",   "73u", "OS upgrade" },
    { "APPL",  "73?", "Application" },
    { "CERT",  "73?", "Certificate" },
    { "IDLIST","73idl", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
    { "",      "73?", "Unknown" },
  };

// Return the type corresponding to the value
const char *ti73_byte2type(uint8_t data)
{
  if(data > TI73_MAXTYPES) return NULL;
  if(data==TI73_DIR) return "DIR";
  return TI73_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti73_type2byte(const char *s)
{
  int i;
  
  for(i=0; i<TI73_MAXTYPES; i++)
    {
      if(!strcmp(TI73_CONST[i][0], s)) break;
    }
  if(i > TI73_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti73_byte2fext(uint8_t data)
{
  if(data > TI73_MAXTYPES) return NULL;
  return TI73_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti73_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI73_MAXTYPES; i++)
    {
      if(!strcasecmp(TI73_CONST[i][1], s)) break;
    }
  if(i>TI73_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti73_byte2desc(uint8_t data)
{
  if(data > TI73_MAXTYPES) return NULL;
  return TI73_CONST[data][2];
}
