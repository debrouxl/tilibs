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
#include "types92.h"


/*
  Is missing:
  Y-Var (.89y)
  Window Setup(.89w)
  Zoom (.89z)
  Table Setup (.89t)
  Lab Report (92r)
*/
const char *TI92_CONST[TI92_MAXTYPES][3] =
  {
    { "EXPR",   "92e", "Expression" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "LIST",   "92l", "List" },
    { "",       "92?", "Unknown" },
    { "MAT",    "92m", "Matrix" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "DATA",   "92c", "Data" },
    { "TEXT",   "92t", "Text" },
    { "STR",    "92s", "String" },
    { "GDB",    "92d", "GDB"    },
    { "FIG",    "92a", "Figure"  },
    { "",       "92?", "Unknown" },
    { "PIC",    "92i", "Picture" },
    { "",       "92?", "Unknown" },
    { "PRGM",   "92p", "Program" },
    { "FUNC",   "92f", "Function" },
    { "MAC",    "92x", "Macro"   },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "ZIP",    "92?", "Zipped"  },
    { "BKUP",   "92b", "Backup"  },
    { "",       "92?", "Unknown" },
    { "DIR",    "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "ASM",    "92z", "Asm Program" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
    { "",       "92?", "Unknown" },
  };

// Return the type corresponding to the value
const char *ti92_byte2type(uint8_t data)
{
  if(data > TI92_MAXTYPES) return NULL;
  return TI92_CONST[data][0];
}

// Return the value corresponding to the type
/*TIEXPORT*/
uint8_t ti92_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI92_MAXTYPES; i++)
    {
      if(!strcmp(TI92_CONST[i][0], s)) break;
    }
  if(i>TI92_MAXTYPES)
    {
      fprintf(stdout, "Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
/*TIEXPORT*/
const char *ti92_byte2fext(uint8_t data)
{
  if(data > TI92_MAXTYPES) return NULL;
  return TI92_CONST[data][1];
}

// Return the value corresponding to the file extension
/*TIEXPORT*/
uint8_t ti92_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI92_MAXTYPES; i++)
    {
      if(!strcasecmp(TI92_CONST[i][1], s)) break;
    }
  if(i > TI92_MAXTYPES)
    {
      fprintf(stdout, "Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti92_byte2desc(uint8_t data)
{
  if(data > TI92_MAXTYPES) return NULL;
  return TI92_CONST[data][2];
}
