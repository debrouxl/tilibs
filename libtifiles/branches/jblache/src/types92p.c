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
#include "types92p.h"


/*
  Is missing :
  - Y-Var (.92py)
  - Window Setup(.92pw)
  - Zoom (.92pz)
  - Table Setup (.92pt)
  - Lab Report (.92pr)
*/
const char *TI92p_CONST[TI92p_MAXTYPES][3] =
  {
    { "EXPR",   "9xe", "Expression" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "LIST",   "9xl", "List" },
    { "",       "9x?", "Unknown" },
    { "MAT",    "9xm", "Matrix" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "DATA",   "9xc", "Data" },
    { "TEXT",   "9xt", "Text" },
    { "STR",    "9xs", "String" },
    { "GDB",    "9xd", "GDB"    },
    { "FIG",    "9xa", "Figure"  },
    { "",       "9x?", "Unknown" },
    { "PIC",    "9xi", "Picture" },
    { "",       "9x?", "Unknown" },
    { "PRGM",   "9xp", "Program" },
    { "FUNC",   "9xf", "Function" },
    { "MAC",    "9xx", "Macro"   },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "RDIR",   "9x?", "Unknown" },
    { "LDIR",   "9x?", "Unknown" },
    { "ZIP",    "9x?", "Zipped"  },
    { "BKUP",   "9xg", "Backup"  },
    { "",       "9x?", "Unknown" },
    { "DIR",    "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "ASM",    "9xz", "Asm Program" },
    { "IDLIST", "9xidl", "ID-LIST" },
    { "AMS",    "9xu", "OS upgrade" },
    { "APPL",   "9xk", "Application" },
    { "CERT",   "9xq", "Certificate" },
    { "LOCKED", "9x?", "Unknown" },
    { "ARCHIV", "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
    { "",       "9x?", "Unknown" },
  };

// Return the type corresponding to the value
const char *ti92p_byte2type(uint8_t data)
{
  if(data > TI92p_MAXTYPES) return NULL;
  return TI92p_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t ti92p_type2byte(const char *s)
{
  int i;

  for(i=0; i<TI92p_MAXTYPES; i++)
    {
      if(!strcmp(TI92p_CONST[i][0], s)) break;
    }
  if(i>TI92p_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti92p_byte2fext(uint8_t data)
{
  if(data > TI92p_MAXTYPES) return NULL;
  return TI92p_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t ti92p_fext2byte(const char *s)
{
  int i;

  for(i=0; i<TI92p_MAXTYPES; i++)
    {
      if(!strcasecmp(TI92p_CONST[i][1], s)) break;
    }
  if(i > TI92p_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *ti92p_byte2desc(uint8_t data)
{
  if(data > TI92p_MAXTYPES) return NULL;
  return TI92p_CONST[data][2];
}
