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
#include "types85.h"


const char *TI85_CONST[TI85_MAXTYPES][3] =
  {
    { "REAL",  "85n", "Real" },
    { "CPLX",  "85c", "Complex" },
    { "VECT",  "85v", "Vector" },
    { "CVECT", "85v", "Complex Vector" },
    { "LIST",  "85l", "List" },
    { "CLIST", "85l", "Complex List" },
    { "MAT",   "85m", "Matrix" },
    { "CMAT",  "85m", "Complex Matrix" },
    { "CONS",  "85k", "Constant" },
    { "CCONS", "85c", "Complex Constant" },
    { "EQU",   "85y", "Equation" },
    { "",      "85?", "Unknown" },
    { "STR",   "85s", "String" },
    { "GDB",   "85d", "GDB" },
    { "GDB",   "85d", "GDB" },
    { "GDB",   "85d", "GDB" },
    { "GDB",   "85d", "GDB" },
    { "PIC",   "85i", "Picture" },
    { "PRGM",  "85p", "Program" },
    { "RANGE", "85r", "Range" },
    { "SCRN",  "85?", "Screen" },
    { "DIR",   "85?", "Directory" },
    { "",      "85?", "Unknown" },
    { "WND",   "85w", "Window Setup" },
    { "POLAR", "85?", "Polar" },
    { "PARAM", "85?", "Parametric" },
    { "DIFEQ", "85?", "Diff Equ" },
    { "ZRCL",  "85z", "Zoom" },
    { "",      "85?", "Unknown" },
    { "BKUP",  "85b", "Backup" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "YVAR",  "85y", "Y-Var" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
    { "",      "85?", "Unknown" },
};

// Return the type corresponding to the value
const char *ti85_byte2type(uint8_t data)
{
  if(data > TI85_MAXTYPES) return NULL;
  return TI85_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti85_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI85_MAXTYPES; i++)
    {
      if(!strcmp(TI85_CONST[i][0], s)) break;
    }
  if(i>TI85_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}


// Return the file extension corresponding to the value
const char *ti85_byte2fext(uint8_t data)
{
  if(data > TI85_MAXTYPES) return NULL;
  return TI85_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti85_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI85_MAXTYPES; i++)
    {
      if(!strcasecmp(TI85_CONST[i][1], s)) break;
    }
  if(i > TI85_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti85_byte2desc(uint8_t data)
{
  if(data > TI85_MAXTYPES) return NULL;
  return TI85_CONST[data][2];
}
