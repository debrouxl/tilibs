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
#include "types86.h"


const char *TI86_CONST[TI86_MAXTYPES][3] =
  {
    { "REAL",  "86n", "Real" },
    { "CPLX",  "86c", "Complex" },
    { "VECT",  "86v", "Vector" },
    { "CVECT", "86v", "Complex Vector" },
    { "LIST",  "86l", "List" },
    { "CLIST", "86l", "Complex List" },
    { "MAT",   "86m", "Matrix" },
    { "CMAT",  "86m", "Complex Matrix" },
    { "CONS",  "86k", "Constant" },
    { "CCONS", "86k", "Complex Constant" },
    { "EQU",   "86y", "Equation" },
    { "",      "86?", "Unknown" },
    { "STR",   "86s", "String" },
    { "GDB",   "86d", "GDB" },
    { "GDB",   "86d", "GDB" },
    { "GDB",   "86d", "GDB" },
    { "GDB",   "86d", "GDB" },
    { "PIC",   "86i", "Picture" },
    { "PRGM",  "86p", "Program" },
    { "RANGE", "86r", "Range" },
    { "SCRN",  "86?", "Screen" },
    { "DIR",   "86?", "Directory" },
    { "",      "86?", "Unknown" },
    { "YVAR",  "86y", "Y-Var" },
    { "POLAR", "86?", "Polar" },
    { "PARAM", "86?", "Parametric" },
    { "DIFEQ", "86?", "Diff Equ" },
    { "ZRCL",  "86z", "Zoom" },
    { "",      "86?", "Unknown" },
    { "BKUP",  "86b", "Backup" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
    { "",      "86?", "Unknown" },
};

// Return the type corresponding to the value
const char *ti86_byte2type(uint8_t data)
{
  if(data > TI86_MAXTYPES) return NULL;
  return TI86_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti86_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI86_MAXTYPES; i++)
    {
      if(!strcmp(TI86_CONST[i][0], s)) break;
    }
  if(i>TI86_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti86_byte2fext(uint8_t data)
{
  if(data > TI86_MAXTYPES) return NULL;
  return TI86_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti86_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI86_MAXTYPES; i++)
    {
      if(!strcasecmp(TI86_CONST[i][1], s)) break;
    }
  if(i > TI86_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti86_byte2desc(uint8_t data)
{
  if(data > TI86_MAXTYPES) return NULL;
  return TI86_CONST[data][2];
}
