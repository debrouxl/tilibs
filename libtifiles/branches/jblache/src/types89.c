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
#include "types89.h"


/*
  Is missing :
  - Y-Var (.89y)
  - Window Setup(.89w)
  - Zoom (.89z)
  - Table Setup (.89t)
  - Lab Report (89r)
*/
const char *TI89_CONST[TI89_MAXTYPES][3] =
  {
    { "EXPR",   "89e", "Expression" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "LIST",   "89l", "List" },
    { "",       "89?", "Unknown" },
    { "MAT",    "89m", "Matrix" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "DATA",   "89c", "Data" },
    { "TEXT",   "89t", "Text" },
    { "STR",    "89s", "String" },
    { "GDB",    "89d", "GDB"    },
    { "FIG",    "89a", "Figure"  },
    { "",       "89?", "Unknown" },
    { "PIC",    "89i", "Picture" },
    { "",       "89?", "Unknown" },
    { "PRGM",   "89p", "Program" },
    { "FUNC",   "89f", "Function" },
    { "MAC",    "89x", "Macro"   },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "CLOCK",  "89clk", "Unknown" },
    { "",       "89?", "Unknown" },
    { "RDIR",   "89?", "Unknown" },
    { "LDIR",   "89?", "Unknown" },
    { "ZIP",    "89?", "Zipped"  },
    { "BKUP",   "89g", "Backup"  },
    { "",       "89?", "Unknown" },
    { "DIR",    "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "ASM",    "89z", "Asm Program" },
    { "IDLIST", "89idl", "ID-LIST" },
    { "AMS",    "89u", "OS upgrade" },
    { "APPL",   "89k", "Application" },
    { "CERT",   "89q", "Certificate" },
    { "LOCKED", "89?", "Unknown" },
    { "ARCHIV", "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
    { "",       "89?", "Unknown" },
  };

// Return the type corresponding to the value
const char *ti89_byte2type(uint8_t data)
{
  if(data > TI89_MAXTYPES) return NULL;
  return TI89_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti89_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI89_MAXTYPES; i++)
    {
      if(!strcmp(TI89_CONST[i][0], s)) break;
    }
  if(i>TI89_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti89_byte2fext(uint8_t data)
{
  if(data > TI89_MAXTYPES) return NULL;
  return TI89_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti89_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI89_MAXTYPES; i++)
    {
      if(!strcasecmp(TI89_CONST[i][1], s)) break;
    }
  if(i > TI89_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti89_byte2desc(uint8_t data)
{
  if(data > TI89_MAXTYPES) return NULL;
  return TI89_CONST[data][2];
}
