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
#include "typesv2.h"


/*
  Is missing :
  - Y-Var (.v2y)
  - Window Setup(.v2w)
  - Zoom (.v2z)
  - Table Setup (.v2t)
  - Lab Report (v2r)
*/
const char *V200_CONST[V200_MAXTYPES][3] =
  {
    { "EXPR",   "v2e", "Expression" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "LIST",   "v2l", "List   " },
    { "",       "v2?", "Unknown" },
    { "MAT",    "v2m", "Matrix " },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "DATA",   "v2c", "Data   " },
    { "TEXT",   "v2t", "Text   " },
    { "STR",    "v2s", "String " },
    { "GDB",    "v2d", "GDB "    },
    { "FIG",    "v2a", "Figure"  },
    { "",       "v2?", "Unknown" },
    { "PIC",    "v2i", "Picture" },
    { "",       "v2?", "Unknown" },
    { "PRGM",   "v2p", "Program" },
    { "FUNC",   "v2f", "Function" },
    { "MAC",    "v2x", "Macro"   },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "RDIR",   "v2?", "Unknown" },
    { "LDIR",   "v2?", "Unknown" },
    { "ZIP",    "v2?", "Zipped"  },
    { "BKUP",   "v2g", "Backup"  },
    { "",       "v2?", "Unknown" },
    { "DIR",    "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "ASM",    "v2z", "Asm Prog" },
    { "IDLIST", "v2idl", "ID-LIST" },
    { "AMS",    "v2u", "OS upgrade" },
    { "APPL",   "v2k", "Application" },
    { "CERT",   "v2q", "Certificate" },
    { "LOCKED", "v2?", "Unknown" },
    { "ARCHIV", "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
    { "",       "v2?", "Unknown" },
  };

// Return the type corresponding to the value
const char *v200_byte2type(uint8_t data)
{
  if(data > V200_MAXTYPES) return NULL;
  return V200_CONST[data][0];
}

// Return the value corresponding to the type
uint8_t v200_type2byte(const char *s)
{
  int i;

  for(i=0; i<V200_MAXTYPES; i++)
    {
      if(!strcmp(V200_CONST[i][0], s)) break;
    }
  if(i>V200_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *v200_byte2fext(uint8_t data)
{
  if(data > V200_MAXTYPES) return NULL;
  return V200_CONST[data][1];
}

// Return the value corresponding to the file extension
uint8_t v200_fext2byte(const char *s)
{
  int i;

  for(i=0; i<V200_MAXTYPES; i++)
    {
      if(!strcasecmp(V200_CONST[i][1], s)) break;
    }
  if(i > V200_MAXTYPES)
    {
      printf("Warning: Unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the descriptive associated with the vartype
const char *v200_byte2desc(uint8_t data)
{
  if(data > V200_MAXTYPES) return NULL;
  return V200_CONST[data][2];
}
