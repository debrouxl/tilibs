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
#include "types83p.h"


const char *TI83p_CONST[TI83p_MAXTYPES][3] =
  {
    { "REAL",  "8xn", "Real" },
    { "LIST",  "8xl", "List" },
    { "MAT",   "8xm", "Matrix" },
    { "EQU",   "8xe", "Equation" },
    { "STR",   "8xs", "String" },
    { "PRGM",  "8xp", "Program" },
    { "ASM",   "8xz", "Asm Program" },
    { "PIC",   "8xi", "Picture" },
    { "GDB",   "8xg", "GDB" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "WDW",   "8xw", "Window" },
    { "CPLX",  "8xc", "Complex" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "WDW",   "8xw", "Window Setup" },
    { "ZSTO",  "8xz", "Zoom" },
    { "TAB",   "8xt", "Table Setup" },
    { "",      "8x?", "Unknown" },
    { "BKUP",  "8xb", "Backup" },
    { "",      "8x?", "Unknown" },
    { "APPV",  "8xv", "App Var" },
    { "",      "8x?", "Unknown" },
    { "GRP",   "8x?", "Group Var" },
    { "",      "8x?", "Unknown" },
    { "DIR",   "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "AMS",   "8xu", "OS upgrade" },
    { "APPL",  "8xk", "Application" },
    { "CERT",  "8xq", "Certificate" },
    { "IDLIST","8xidl", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
    { "",      "8x?", "Unknown" },
  };

// Return the type corresponding to the value
const char *ti83p_byte2type(uint8_t data)
{
  if(data > TI83p_MAXTYPES) return NULL;
  if(data==TI83p_DIR) return "DIR";
  return TI83p_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti83p_type2byte(const char *s)
{
  int i;
  
  for(i=0; i<TI83p_MAXTYPES; i++)
    {
      if(!strcmp(TI83p_CONST[i][0], s)) break;
    }
  if(i >= TI83p_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti83p_byte2fext(uint8_t data)
{
  if(data > TI83p_MAXTYPES) return NULL;
  return TI83p_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti83p_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI83p_MAXTYPES; i++)
    {
      if(!strcasecmp(TI83p_CONST[i][1], s)) 
	break;
    }
  if(i>=TI83p_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti83p_byte2desc(uint8_t data)
{
  if(data > TI83p_MAXTYPES) return NULL;
  return TI83p_CONST[data][2];
}
