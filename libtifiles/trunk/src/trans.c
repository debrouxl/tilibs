/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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
  This unit is used to translate some varnames into a more readable name.
  Depends on the calculator type and the variable type.
  
  There are 2 steps:
  - varname detokenization for some calcs (73/82/83/83+) with TI-charset
  - TI-charset to user-defined charset (ASCII/Latin1/Unicode) conversion
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdints.h"

#include "gettext.h"
#include "typesxx.h"
#include "file_int.h"
#include "macros.h"
#include "printl.h"


extern int tifiles_calc_type;

/* 
   Variable name detokenization. This is needed for the following calcs:
   - 73/82/83/83+: binary-coded to TI-charset
   - 85/86: no operation
   - 89/92/92+: no operation
   See the 'TI Link Guide' for more informations...
*/


static char *ti8x_detokenize_varname(const char *varname, char *translate,
			      uint8_t vartype)
{
  int i;
  uint8_t tok1 = varname[0] & 0xff;
  uint8_t tok2 = varname[1] & 0xff;

  switch (vartype) 
    {
    case TI82_WDW:
      strcpy(translate, "Window");
      return translate;
      break;
    case TI82_ZSTO:
      strcpy(translate, "RclWin");
      return translate;
      break;
    case TI82_TAB:
      strcpy(translate, "TblSet");
      return translate;
      break;
    default:
      break;
    }
  
  switch (tok1) 
    {
    case 0x5C:			/* Matrix: [A] to [E]/[J] */
      switch(tok2)
	{
	case 0x00: sprintf(translate, "%cA]", '\xc1'); break;
	case 0x01: sprintf(translate, "%cB]", '\xc1'); break;
	case 0x02: sprintf(translate, "%cC]", '\xc1'); break;
	case 0x03: sprintf(translate, "%cD]", '\xc1'); break;
	case 0x04: sprintf(translate, "%cE]", '\xc1'); break;
	case 0x05: sprintf(translate, "%cF]", '\xc1'); break;
	case 0x06: sprintf(translate, "%cG]", '\xc1'); break;
	case 0x07: sprintf(translate, "%cH]", '\xc1'); break;
	case 0x08: sprintf(translate, "%cI]", '\xc1'); break;
	case 0x09: sprintf(translate, "%cJ]", '\xc1'); break;

	default:   sprintf(translate, "%c?]", '\xc1'); break;
	}
      break;
    case 0x5D:			/* List: L1 to L6/L0 */
      if(tifiles_calc_type == CALC_TI73) //TI73 != TI83 here
	sprintf(translate, "L%c", varname[1] + '\x80');	
      else 
	{// TI73 begins at L0, others at L1
	  switch(tok2)
	    {
	    case 0x00: sprintf(translate, "L%c", '\x81'); break;
	    case 0x01: sprintf(translate, "L%c", '\x82'); break;
	    case 0x02: sprintf(translate, "L%c", '\x83'); break;
	    case 0x03: sprintf(translate, "L%c", '\x84'); break;
	    case 0x04: sprintf(translate, "L%c", '\x85'); break;
	    case 0x05: sprintf(translate, "L%c", '\x86'); break;
	    case 0x06: sprintf(translate, "L%c", '\x87'); break;
	    case 0x07: sprintf(translate, "L%c", '\x88'); break;
	    case 0x08: sprintf(translate, "L%c", '\x89'); break;
	    case 0x09: sprintf(translate, "L%c", '\x80'); break;
	      
	    default: // named list
	      for (i = 0; i < 7; i++)
		translate[i] = varname[i + 1];
	      break;
	    }
	}
      break;	
    case 0x5E:			/* Equations: Y1 to Y0, ... */
      switch(tok2)
	{
	case 0x10: sprintf(translate, "Y%c", '\x81'); break;
	case 0x11: sprintf(translate, "Y%c", '\x82'); break;
	case 0x12: sprintf(translate, "Y%c", '\x83'); break;
	case 0x13: sprintf(translate, "Y%c", '\x84'); break;
	case 0x14: sprintf(translate, "Y%c", '\x85'); break;
	case 0x15: sprintf(translate, "Y%c", '\x86'); break;
	case 0x16: sprintf(translate, "Y%c", '\x87'); break;
	case 0x17: sprintf(translate, "Y%c", '\x88'); break;
	case 0x18: sprintf(translate, "Y%c", '\x89'); break;
	case 0x19: sprintf(translate, "Y%c", '\x80'); break;

	case 0x20: sprintf(translate, "X%ct", '\x81'); break;
	case 0x21: sprintf(translate, "Y%ct", '\x81'); break;
	case 0x22: sprintf(translate, "X%ct", '\x82'); break;
	case 0x23: sprintf(translate, "Y%ct", '\x82'); break;
	case 0x24: sprintf(translate, "X%ct", '\x83'); break;
	case 0x25: sprintf(translate, "Y%ct", '\x83'); break;
	case 0x26: sprintf(translate, "X%ct", '\x84'); break;
	case 0x27: sprintf(translate, "Y%ct", '\x84'); break;
	case 0x28: sprintf(translate, "X%ct", '\x85'); break;
	case 0x29: sprintf(translate, "Y%ct", '\x85'); break;
	case 0x2a: sprintf(translate, "X%ct", '\x86'); break;
	case 0x2b: sprintf(translate, "Y%ct", '\x86'); break;

	case 0x40: sprintf(translate, "r%c", '\x81'); break;
	case 0x41: sprintf(translate, "r%c", '\x82'); break;
	case 0x42: sprintf(translate, "r%c", '\x83'); break;
	case 0x43: sprintf(translate, "r%c", '\x84'); break;
	case 0x44: sprintf(translate, "r%c", '\x85'); break;
	case 0x45: sprintf(translate, "r%c", '\x86'); break;

	case 0x80: 
	  if(tifiles_calc_type == CALC_TI82)
	    sprintf(translate, "U%c", '\xd7'); 
	  else
	    sprintf(translate, "u");
	  break;
	case 0x81:
	  if(tifiles_calc_type == CALC_TI82)
	    sprintf(translate, "V%c", '\xd7'); 
	  else
	    sprintf(translate, "v");
	  break;
	case 0x82:
	  if(tifiles_calc_type == CALC_TI82)
	    sprintf(translate, "W%c", '\xd7'); 
	  else
	    sprintf(translate, "w");
	  break; 
	
	default: sprintf(translate, "_"); break;
    }
      break;
    case 0x60:			/* Pictures */
      if (tok2 != 0x09)
	sprintf(translate, "Pic%c", tok2 + '\x81');
      else
	sprintf(translate, "Pic%c", '\x80');
      break;
    case 0x61:			/* GDB */
      if (tok2 != 0x09)
	sprintf(translate, "GDB%c", tok2 + '\x81');
      else
	sprintf(translate, "GDB%c", '\x80');
      break;
    case 0x62:
      switch(tok2)
	{
	case 0x01: sprintf(translate, "ReqEq"); break;
	case 0x02: sprintf(translate, "n"); break;
	case 0x03: sprintf(translate, "%c", '\xcb'); break;
	case 0x04: sprintf(translate, "%c%c", '\xc6', 'x'); break;
	case 0x05: sprintf(translate, "%c%c%c", '\xc6', 'x', '\x12'); break;
	case 0x06: sprintf(translate, "%c%c", 'S', 'x'); break;
	case 0x07: sprintf(translate, "%c%c", '\xc7', 'x'); break;
	case 0x08: sprintf(translate, "minX"); break;
	case 0x09: sprintf(translate, "maxX"); break;
	case 0x0a: sprintf(translate, "minY"); break;
	case 0x0b: sprintf(translate, "maxY"); break;
	case 0x0c: sprintf(translate, "%c", '\xcc'); break;
	case 0x0d: sprintf(translate, "%c%c", '\xc6', 'y'); break;
	case 0x0e: sprintf(translate, "%c%c%c", '\xc6', 'y', '\x12'); break;
	case 0x0f: sprintf(translate, "%c%c", 'S', 'y'); break;
	case 0x10: sprintf(translate, "%c%c", '\xc7', 'y'); break;
	case 0x11: sprintf(translate, "%c%c%c", '\xc6', 'x', 'y'); break;
	case 0x12: sprintf(translate, "%c", 'r'); break; 
	case 0x13: sprintf(translate, "Med"); break;
	case 0x14: sprintf(translate, "%c%c", 'Q', '\x81'); break;
	case 0x15: sprintf(translate, "%c%c", 'Q', '\x83'); break;
	case 0x16: sprintf(translate, "a"); break;
	case 0x17: sprintf(translate, "b"); break;
	case 0x18: sprintf(translate, "c"); break;
	case 0x19: sprintf(translate, "d"); break;
	case 0x1a: sprintf(translate, "e"); break;
	case 0x1b: sprintf(translate, "%c%c", 'x', '\x81'); break;
	case 0x1c: sprintf(translate, "%c%c", 'x', '\x82'); break;
	case 0x1d: sprintf(translate, "%c%c", 'x', '\x83'); break;
	case 0x1e: sprintf(translate, "%c%c", 'y', '\x81'); break;
	case 0x1f: sprintf(translate, "%c%c", 'y', '\x82'); break;
	case 0x20: sprintf(translate, "%c%c", 'y', '\x83'); break;
	case 0x21: sprintf(translate, "%c", '\xd7'); break;
	case 0x22: sprintf(translate, "p"); break;
	case 0x23: sprintf(translate, "z"); break;
	case 0x24: sprintf(translate, "t"); break;
	case 0x25: sprintf(translate, "%c%c", '\xd9', '\x12'); break;
	case 0x26: sprintf(translate, "%c", '\xda'); break;
	case 0x27: sprintf(translate, "df"); break;
	case 0x28: sprintf(translate, "%c", '\xbc'); break;
	case 0x29: sprintf(translate, "%c%c", '\xbc', '\x81'); break;
	case 0x2a: sprintf(translate, "%c%c", '\xbc', '\x82'); break;
	case 0x2b: sprintf(translate, "%c%c", '\xcb', '\x81'); break;
	case 0x2c: sprintf(translate, "Sx%c", '\x81'); break;
	case 0x2d: sprintf(translate, "n%c", '\x81'); break;
	case 0x2e: sprintf(translate, "%c%c", '\xcb', '\x82'); break;
	case 0x2f: sprintf(translate, "Sx%c", '\x82'); break;
	case 0x30: sprintf(translate, "n%c", '\x82'); break;
	case 0x31: sprintf(translate, "Sxp"); break;
	case 0x32: sprintf(translate, "lower"); break;
	case 0x33: sprintf(translate, "upper"); break;
	case 0x34: sprintf(translate, "s"); break;
	case 0x35: sprintf(translate, "r%c", '\x12'); break;
	case 0x36: sprintf(translate, "R%c", '\x12'); break;
	case 0x37: sprintf(translate, "df"); break;
	case 0x38: sprintf(translate, "SS"); break;
	case 0x39: sprintf(translate, "MS"); break;
	case 0x3a: sprintf(translate, "df"); break;
	case 0x3b: sprintf(translate, "SS"); break;
	case 0x3c: sprintf(translate, "MS"); break;
	default: sprintf(translate, "_"); break;
	}
      break;
    case 0x63:
      switch(tok2)
	{
	case 0x00: sprintf(translate, "ZXscl"); break;
	case 0x01: sprintf(translate, "ZYscl"); break;
	case 0x02: sprintf(translate, "Xscl"); break;
	case 0x03: sprintf(translate, "Yscl"); break;
	case 0x04: sprintf(translate, "U%cStart", '\xd7'); break;
	case 0x05: sprintf(translate, "V%cStart", '\xd7'); break;
	case 0x06: sprintf(translate, "U%c-%c", '\xd7', '\x81'); break;
	case 0x07: sprintf(translate, "V%c-%c", '\xd7', '\x81'); break;
	case 0x08: sprintf(translate, "ZU%cStart", '\xd7'); break;
	case 0x09: sprintf(translate, "ZV%cStart", '\xd7'); break;
	case 0x0a: sprintf(translate, "Xmin"); break;
	case 0x0b: sprintf(translate, "Xmax"); break;
	case 0x0c: sprintf(translate, "Ymin"); break;
	case 0x0d: sprintf(translate, "Ymax"); break;
	case 0x0e: sprintf(translate, "Tmin"); break;
	case 0x0f: sprintf(translate, "Tmax"); break;
	case 0x10: sprintf(translate, "%cmin", '\x5b'); break;
	case 0x11: sprintf(translate, "%cmax", '\x5b'); break;
	case 0x12: sprintf(translate, "ZXmin"); break;
	case 0x13: sprintf(translate, "ZXmax"); break;
	case 0x14: sprintf(translate, "ZYmin"); break;
	case 0x15: sprintf(translate, "ZYmax"); break;
	case 0x16: sprintf(translate, "Z%cmin", '\x5b'); break;
	case 0x17: sprintf(translate, "Z%cmax", '\x5b'); break;
	case 0x18: sprintf(translate, "ZTmin"); break;
	case 0x19: sprintf(translate, "ZTmax"); break;
	case 0x1a: sprintf(translate, "TblMin"); break;
	case 0x1b: sprintf(translate, "%cMin", '\xd7'); break;
	case 0x1c: sprintf(translate, "Z%cMin", '\xd7'); break;
	case 0x1d: sprintf(translate, "%cMax", '\xd7'); break;
	case 0x1e: sprintf(translate, "Z%cMax", '\xd7'); break;
	case 0x1f: sprintf(translate, "%cStart", '\xd7'); break;
	case 0x20: sprintf(translate, "Z%cStart", '\xd7'); break;
	case 0x21: sprintf(translate, "%cTbl", '\xbe'); break;
	case 0x22: sprintf(translate, "Tstep"); break;
	case 0x23: sprintf(translate, "%cstep", '\x5b'); break;
	case 0x24: sprintf(translate, "ZTstep"); break;
	case 0x25: sprintf(translate, "Z%cstep", '\x5b'); break;
	case 0x26: sprintf(translate, "%cX", '\xbe'); break;
	case 0x27: sprintf(translate, "%cY", '\xbe'); break;
	case 0x28: sprintf(translate, "XFact"); break;
	case 0x29: sprintf(translate, "YFact"); break;
	case 0x2a: sprintf(translate, "TblInput"); break;
	case 0x2b: sprintf(translate, "N"); break;
	case 0x2c: sprintf(translate, "I%c", '\x25'); break;
	case 0x2d: sprintf(translate, "PV"); break;
	case 0x2e: sprintf(translate, "PMT"); break;
	case 0x2f: sprintf(translate, "FV"); break;
	case 0x30: sprintf(translate, "Xres"); break;
	case 0x31: sprintf(translate, "ZXres"); break;
	default: sprintf(translate, "_"); break;
	}
      break;
    case 0xAA:
      if (tok2 != 0x09)
	sprintf(translate, "Str%c", tok2 + '\x81');
      else
	sprintf(translate, "Str%c", '\x80');
      break;
    default:
      strcpy(translate, varname);
      break;
    }

    return translate;
}


TIEXPORT uint8_t TICALL *tixx_detokenize_varname(const char *varname, 
						 char *translate,
						 uint8_t vartype,
						 TicalcType calc_type)
{
  switch (calc_type) {
  case CALC_TI73:
  case CALC_TI82:
  case CALC_TI83:
  case CALC_TI83P:
  case CALC_TI84P:
    return ti8x_detokenize_varname(varname, translate, vartype);
    break;
  case CALC_TI85:
  case CALC_TI86:
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI92:
  case CALC_TI92P:
  case CALC_V200:
    return strcpy(translate, varname);
    break;
  default:
    return strcpy(translate, "________");
    break;
  }
}


/* 
   Charset transcoding. Required for all calcs.
   See the 'TI Link Guide' for TI-charsets...
   A char from the TI-charset can be converted into:
   - ASCII charset,
   - ISO8859-1 charset (aka Latin1),
   - UTF-8 charset (Unicode).
   See: www.unicode.org/charts & www.czyborra.com/utf
*/

/* TI82 */

static char transcode_from_ti82_charset_to_ascii(const char c)
{
  switch((uint8_t)c)
    {
    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';
    case 0xd8: return 'n'; // eta, u, v, w: diff between 82 and 83
    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static char transcode_from_ti82_charset_to_latin1(const char c)
{
  switch((uint8_t)c)
    {
    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';

    case 0x8a: return (char)192+1;
    case 0x8b: return (char)192+0;
    case 0x8c: return (char)192+2;
    case 0x8d: return (char)192+4;
    case 0x8e: return (char)224+1;
    case 0x8f: return (char)224+0;
    case 0x90: return (char)224+2;
    case 0x91: return (char)224+4;
    case 0x92: return (char)200+1;
    case 0x93: return (char)200+0;
    case 0x94: return (char)200+2;
    case 0x95: return (char)200+4;
    case 0x96: return (char)231+1;
    case 0x97: return (char)231+0;
    case 0x98: return (char)231+2;
    case 0x99: return (char)231+4;
    case 0x9a: return (char)204+1;
    case 0x9b: return (char)204+0;
    case 0x9c: return (char)204+2;
    case 0x9d: return (char)204+3;
    case 0x9e: return (char)236+1;
    case 0x9f: return (char)236+0;
    case 0xa0: return (char)236+2;
    case 0xa1: return (char)236+3;
    case 0xa2: return (char)210+1;
    case 0xa3: return (char)210+0;
    case 0xa4: return (char)210+2;
    case 0xa5: return (char)210+4;
    case 0xa6: return (char)242+1;
    case 0xa7: return (char)242+0;
    case 0xa8: return (char)242+2;
    case 0xa9: return (char)242+4;
    case 0xaa: return (char)217+1;
    case 0xab: return (char)217+0;
    case 0xac: return (char)217+2;
    case 0xad: return (char)217+3;
    case 0xae: return (char)249+1;
    case 0xaf: return (char)249+0;
    case 0xb0: return (char)249+2;
    case 0xb1: return (char)249+3;
    case 0xb2: return (char)199;
    case 0xb3: return (char)231;
    case 0xb4: return (char)209;
    case 0xb5: return (char)241;

    case 0xd8: return 'n'; // eta, u, v, w: diff between 82 and 83
    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static uint16_t ti82_charset[256] = { 
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_', 
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_', 
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  ' ',   '!',   '\"',  '#',   '_',   '%',   '&',   '\'', 
  '(',   ')',   '*',   '+',   ',',   '-',   '.',   '/',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7', 
  '8',   '9',   ':',   ';',   '<',   '=',   '>',   '?',
  '@',   'A',   'B',   'C',   'D',   'E',   'F',   'G',
  'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',
  'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W',
  'X',   'Y',   'Z',   0x3b8, '\\',  ']',   '^',   '_',
  '`',   'a',   'b',   'c',   'd',   'e',   'f',   'g',
  'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',
  'p',   'q',   'r',   's',   't',   'u',   'v',   'w',
  'x',   'y',   'z',   '{',   '|',   '}',   '~',   '=',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7',
  '8',   '9',   192+1, 192+0, 192+2, 192+4, 224+1, 224+0,
  224+2, 224+4, 200+1, 200+0, 200+2, 200+4, 231+1, 231+0,
  231+2, 231+4, 204+1, 204+0, 204+2, 204+3, 236+1, 236+0,
  236+2, 236+3, 210+1, 210+0, 210+2, 210+4, 242+1, 242+0,
  242+2, 242+4, 217+1, 217+0, 217+2, 217+3, 249+1, 249+0,
  249+2, 249+3, 199,   231,   209,   204,   '\'',  '`',
  0x0a8, 0x0bf, 0x0a1, 0x3b1, 0x3b2, 0x3b3, 0x394, 0x3b4,
  0x3b5, '[',   0x3bb, 0x3bc, 0x3c0, 0x3c1, 0x3a3, 0x3c3,
  0x3c4, 0x3d5, 0x3a9, 'x',   'y',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   0x3b7,
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
};

static uint16_t transcode_from_ti82_charset_to_utf8(const char c)
{
  return ti82_charset[(int)c];
}

/* TI83/83+/73 (very similar to TI82 but different) */

static char transcode_from_ti83_charset_to_ascii(const char c)
{
  switch((uint8_t)c)
    {
    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';

    case 0x01: return 'n'; // eta, u, v, w: diff between 82 and 83
    case 0x02: return 'u';
    case 0x03: return 'v'; 
    case 0x04: return 'w';
    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static char transcode_from_ti83_charset_to_latin1(const char c)
{
  switch((uint8_t)c)
    {
    case 0x01: return 'n'; // eta, u, v, w: diff between 82 and 83
    case 0x02: return 'u';
    case 0x03: return 'v'; 
    case 0x04: return 'w';

    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';

    case 0x8a: return (char)192+1;
    case 0x8b: return (char)192+0;
    case 0x8c: return (char)192+2;
    case 0x8d: return (char)192+4;
    case 0x8e: return (char)224+1;
    case 0x8f: return (char)224+0;
    case 0x90: return (char)224+2;
    case 0x91: return (char)224+4;
    case 0x92: return (char)200+1;
    case 0x93: return (char)200+0;
    case 0x94: return (char)200+2;
    case 0x95: return (char)200+4;
    case 0x96: return (char)231+1;
    case 0x97: return (char)231+0;
    case 0x98: return (char)231+2;
    case 0x99: return (char)231+4;
    case 0x9a: return (char)204+1;
    case 0x9b: return (char)204+0;
    case 0x9c: return (char)204+2;
    case 0x9d: return (char)204+3;
    case 0x9e: return (char)236+1;
    case 0x9f: return (char)236+0;
    case 0xa0: return (char)236+2;
    case 0xa1: return (char)236+3;
    case 0xa2: return (char)210+1;
    case 0xa3: return (char)210+0;
    case 0xa4: return (char)210+2;
    case 0xa5: return (char)210+4;
    case 0xa6: return (char)242+1;
    case 0xa7: return (char)242+0;
    case 0xa8: return (char)242+2;
    case 0xa9: return (char)242+4;
    case 0xaa: return (char)217+1;
    case 0xab: return (char)217+0;
    case 0xac: return (char)217+2;
    case 0xad: return (char)217+3;
    case 0xae: return (char)249+1;
    case 0xaf: return (char)249+0;
    case 0xb0: return (char)249+2;
    case 0xb1: return (char)249+3;
    case 0xb2: return (char)199;
    case 0xb3: return (char)231;
    case 0xb4: return (char)209;
    case 0xb5: return (char)241;

    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static uint16_t ti83_charset[256] = { 
  0x3b7, 'u',   'v',   'w',   '_',   '_',   '_',   '_', 
  '_',   'X',   '_',   '_',   '_',   '_',   '_',   'F',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_', 
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  ' ',   '!',   '\"',  '#',   '_',   '%',   '&',   '\'', 
  '(',   ')',   '*',   '+',   ',',   '-',   '.',   '/',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7', 
  '8',   '9',   ':',   ';',   '<',   '=',   '>',   '?',
  '@',   'A',   'B',   'C',   'D',   'E',   'F',   'G',
  'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',
  'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W',
  'X',   'Y',   'Z',   0x3b8, '\\',  ']',   '^',   '_',
  '`',   'a',   'b',   'c',   'd',   'e',   'f',   'g',
  'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',
  'p',   'q',   'r',   's',   't',   'u',   'v',   'w',
  'x',   'y',   'z',   '{',   '|',   '}',   '~',   '=',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7',
  '8',   '9',   192+1, 192+0, 192+2, 192+4, 224+1, 224+0,
  224+2, 224+4, 200+1, 200+0, 200+2, 200+4, 231+1, 231+0,
  231+2, 231+4, 204+1, 204+0, 204+2, 204+3, 236+1, 236+0,
  236+2, 236+3, 210+1, 210+0, 210+2, 210+4, 242+1, 242+0,
  242+2, 242+4, 217+1, 217+0, 217+2, 217+3, 249+1, 249+0,
  249+2, 249+3, 199,   231,   209,   204,   '\'',  '`',
  0x0a8, 0x0bf, 0x0a1, 0x3b1, 0x3b2, 0x3b3, 0x394, 0x3b4,
  0x3b5, '[',   0x3bb, 0x3bc, 0x3c0, 0x3c1, 0x3a3, 0x3c3,
  0x3c4, 0x3d5, 0x3a9, 'x',   'y',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   0x3b7,
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
};

static uint16_t transcode_from_ti83_charset_to_utf8(const char c)
{
  return ti83_charset[(int)c];
}

/* TI85/86 */

static char transcode_from_ti85_charset_to_ascii(const char c)
{
  switch((uint8_t)c)
    {
    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';
    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static char transcode_from_ti85_charset_to_latin1(const char c)
{
  switch((uint8_t)c)
    {
    case 0x80: return '0';
    case 0x81: return '1';
    case 0x82: return '2';
    case 0x83: return '3';
    case 0x84: return '4';
    case 0x85: return '5';
    case 0x86: return '6';
    case 0x87: return '7';
    case 0x88: return '8';
    case 0x89: return '9';

    case 0x8a: return (char)192+1;
    case 0x8b: return (char)192+0;
    case 0x8c: return (char)192+2;
    case 0x8d: return (char)192+4;
    case 0x8e: return (char)224+1;
    case 0x8f: return (char)224+0;
    case 0x90: return (char)224+2;
    case 0x91: return (char)224+4;
    case 0x92: return (char)200+1;
    case 0x93: return (char)200+0;
    case 0x94: return (char)200+2;
    case 0x95: return (char)200+4;
    case 0x96: return (char)231+1;
    case 0x97: return (char)231+0;
    case 0x98: return (char)231+2;
    case 0x99: return (char)231+4;
    case 0x9a: return (char)204+1;
    case 0x9b: return (char)204+0;
    case 0x9c: return (char)204+2;
    case 0x9d: return (char)204+3;
    case 0x9e: return (char)236+1;
    case 0x9f: return (char)236+0;
    case 0xa0: return (char)236+2;
    case 0xa1: return (char)236+3;
    case 0xa2: return (char)210+1;
    case 0xa3: return (char)210+0;
    case 0xa4: return (char)210+2;
    case 0xa5: return (char)210+4;
    case 0xa6: return (char)242+1;
    case 0xa7: return (char)242+0;
    case 0xa8: return (char)242+2;
    case 0xa9: return (char)242+4;
    case 0xaa: return (char)217+1;
    case 0xab: return (char)217+0;
    case 0xac: return (char)217+2;
    case 0xad: return (char)217+3;
    case 0xae: return (char)249+1;
    case 0xaf: return (char)249+0;
    case 0xb0: return (char)249+2;
    case 0xb1: return (char)249+3;
    case 0xb2: return (char)199;
    case 0xb3: return (char)231;
    case 0xb4: return (char)209;
    case 0xb5: return (char)241;

    case 0xd8: return 'n'; // eta, u, v, w: diff between 82 and 83
    default:
      return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
    }
}

static uint16_t ti85_charset[256] = { 
  0x3b7, 'u',   'v',   'w',   '_',   '_',   '_',   '_', 
  '_',   'X',   '_',   '_',   '_',   '_',   '_',   'F',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_', 
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  ' ',   '!',   '\"',  '#',   '_',   '%',   '&',   '\'', 
  '(',   ')',   '*',   '+',   ',',   '-',   '.',   '/',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7', 
  '8',   '9',   ':',   ';',   '<',   '=',   '>',   '?',
  '@',   'A',   'B',   'C',   'D',   'E',   'F',   'G',
  'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',
  'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W',
  'X',   'Y',   'Z',   '[',   '\\',  ']',   '^',   '_',
  '`',   'a',   'b',   'c',   'd',   'e',   'f',   'g',
  'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',
  'p',   'q',   'r',   's',   't',   'u',   'v',   'w',
  'x',   'y',   'z',   '{',   '|',   '}',   '~',   '=',
  '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7',
  '8',   '9',   192+1, 192+0, 192+2, 192+4, 224+1, 224+0,
  224+2, 224+4, 200+1, 200+0, 200+2, 200+4, 231+1, 231+0,
  231+2, 231+4, 204+1, 204+0, 204+2, 204+3, 236+1, 236+0,
  236+2, 236+3, 210+1, 210+0, 210+2, 210+4, 242+1, 242+0,
  242+2, 242+4, 217+1, 217+0, 217+2, 217+3, 249+1, 249+0,
  249+2, 249+3, 199,   231,   209,   204,   '\'',  '`',
  0x0a8, 0x0bf, 0x0a1, 0x3b1, 0x3b2, 0x3b3, 0x394, 0x3b4,
  0x3b5, 0x3b8, 0x3bb, 0x3bc, 0x3c0, 0x3c1, 0x3a3, 0x3c3,
  0x3c4, 0x3d5, 0x3a9, 'x',   'y',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   0x3b7,
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
  '_',   '_',   '_',   '_',   '_',   '_',   '_',   '_',
};

static uint16_t transcode_from_ti85_charset_to_utf8(const char c)
{
  return ti85_charset[(int)c];
}

/* TI89/92/92+/V200 */

static char transcode_from_ti9x_charset_to_ascii(const char c)
{
  return (((uint8_t)c >= 0x20) && ((uint8_t)c < 0x80)) ? c : '_';
}

static char transcode_from_ti9x_charset_to_latin1(const char c)
{
  return (((uint8_t)c >= 0x80) && ((uint8_t)c < 0xA0)) ? '_' : c;
}

static uint16_t transcode_from_ti9x_charset_to_utf8(const char c)
{
  uint16_t wchar;

  if((uint8_t)c < 0x80)       // ASCII part
    wchar = c;
  else if((uint8_t)c >= 0xA0) // ISO8859-1 part
    switch((uint8_t)c) 
      {
      case 0xb5: wchar = 0x03bc; break; // mu
      default:   wchar = c & 0xff; break;
      }
  else 
    {                         // greek characters
      switch((uint8_t)c) 
	{
	case 0x80: wchar = 0x03b1; break; // alpha
	case 0x81: wchar = 0x03b2; break; // beta
	case 0x82: wchar = 0x0393; break; // gamma (capital)
	case 0x83: wchar = 0x03b3; break; // gamma
	case 0x84: wchar = 0x0394; break; // delta (capital)
	case 0x85: wchar = 0x03b4; break; // delta
	case 0x86: wchar = 0x03b5; break; // epsilon
	case 0x87: wchar = 0x03b6; break; // dzeta
	case 0x88: wchar = 0x03b8; break; // theta
	case 0x89: wchar = 0x03bb; break; // lambda
	case 0x8a: wchar = 0x03be; break; // ksi
	case 0x8b: wchar = 0x03a0; break; // pi (capital)
	case 0x8c: wchar = 0x03c0; break; // pi
	case 0x8d: wchar = 0x03c1; break; // rho
	case 0x8e: wchar = 0x03a3; break; // sigma (capital)
	case 0x8f: wchar = 0x03c3; break; // sigma
	case 0x90: wchar = 0x03c4; break; // tau
	case 0x91: wchar = 0x03d5; break; // phi (capital)
	case 0x92: wchar = 0x03a8; break; // psi (capital)
	case 0x93: wchar = 0x03a9; break; // omega (capital)
	case 0x94: wchar = 0x03c9; break; // omega
	default:   wchar = '_';
	}
    }
  
  return wchar;
}

/* Generic transcoding */

typedef char     (*TRANSCODE_TO_ASCII)   (const char c);
typedef char     (*TRANSCODE_TO_LATIN1)  (const char c);
typedef uint16_t (*TRANSCODE_TO_UNICODE) (const char c);

extern int tifiles_calc_type;

/*
  Convert string to pure ASCII.
  Note: src & dst will have the same length.
*/
TIEXPORT char* TICALL tifiles_transcode_to_ascii(char* dst, const char *src)
{
  char *dest = dst;
  TRANSCODE_TO_ASCII f = NULL;

  switch(tifiles_calc_type)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: f = transcode_from_ti83_charset_to_ascii; 
      break;
    case CALC_TI82:  f = transcode_from_ti82_charset_to_ascii; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  f = transcode_from_ti85_charset_to_ascii; 
      break;
    case CALC_TI89:
    case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  f = transcode_from_ti9x_charset_to_ascii; 
      break;
	default:
		printl3(2, _("libtifiles error: unknown calc type. Program halted before crashing !\n"));
		exit(-1);
    }

  while(*src)
    *dest++ = f(*src++);
  *dest = '\0';

  return dest;
}

/*
  Convert string to the ISO8859-1 charset (aka Latin1).
  Note: src & dst will have the same length.
*/
TIEXPORT char* TICALL tifiles_transcode_to_latin1(char* dst, const char *src)
{
  char *dest = dst;
  TRANSCODE_TO_LATIN1 f = NULL;

  switch(tifiles_calc_type)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: f = transcode_from_ti83_charset_to_latin1; 
      break;
    case CALC_TI82:  f = transcode_from_ti82_charset_to_latin1; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  f = transcode_from_ti85_charset_to_latin1; 
      break;
    case CALC_TI89:
    case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  f = transcode_from_ti9x_charset_to_latin1; 
      break;
	default:
	  printl3(2, "libtifiles error: unknown calc type. Program halted before crashing !\n");
		exit(-1);
	break;
    }

  while(*src)
    *dst++ = f(*src++);
  *dst = '\0';

  return dest;
}

/*
  Convert string to the UTF-8 charset (Unicode).
  See: www.unicode.org/charts & www.czyborra.com/utf
  Note: dst may be up to twice the length of src.
*/
TIEXPORT char* TICALL tifiles_transcode_to_utf8(char* dst, const char *src)
{
  char *dest = dst;
  uint16_t wchar;
  TRANSCODE_TO_UNICODE f = NULL;

  switch(tifiles_calc_type)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: 
	case CALC_TI84P: f = transcode_from_ti83_charset_to_utf8; 
      break;
    case CALC_TI82:  f = transcode_from_ti82_charset_to_utf8; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  f = transcode_from_ti85_charset_to_utf8; 
      break;
    case CALC_TI89:
	case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  f = transcode_from_ti9x_charset_to_utf8; 
      break;
	default:
	  printl3(2, "libtifiles error: unknown calc type. Program halted before crashing !\n");
		exit(-1);
	break;
    }

  while(*src) {
    wchar = f(*src++);

    // write our wide-char
    if ((uint16_t)wchar < 0x80)
      *dst++ = (char)wchar;
    else if ((uint16_t)wchar < 0x0800) {
      *dst++ = (0xC0 | (wchar >> 6)) & 0xff;
      *dst++ = (0x80 | (wchar & 0x3f)) & 0xff;
    }
  }
  *dst = '\0';

  return dest;
}


/*
  Set/Get encoding methods
*/


// set to ISO8859-1 for compatibility with previous releases
static TifileEncoding tifiles_encoding = ENCODING_LATIN1;

TIEXPORT void TICALL tifiles_translate_set_encoding(TifileEncoding encoding)
{
  tifiles_encoding = encoding;
}

TIEXPORT TifileEncoding TICALL tifiles_translate_get_encoding(void)
{
  return tifiles_encoding;
}


/* 
   Variable name translation: detokenization + charset transcoding.
*/

char *tixx_translate_varname(const char *varname, char *translate,
			     uint8_t vartype, TicalcType calc_type)
{
  char detokenized[18];
  char *src = detokenized;
  char dst[2*18];

  tixx_detokenize_varname(varname, detokenized, vartype, calc_type);

  switch(tifiles_encoding)
  {
  case ENCODING_ASCII:   tifiles_transcode_to_ascii(dst, src); break;
  case ENCODING_LATIN1:  tifiles_transcode_to_latin1(dst, src); break;
  case ENCODING_UNICODE: tifiles_transcode_to_utf8(dst, src); break;
  }

  strcpy(translate, dst);

  return translate;
}

TIEXPORT char *TICALL tifiles_translate_varname(const char *varname,
						char *translate,
						uint8_t vartype)
{
	return tixx_translate_varname(varname, translate, vartype, 
				tifiles_calc_type);
}

TIEXPORT char *TICALL tifiles_translate_varname_static(const char *varname,
						       uint8_t vartype)
{
  static char trans[18];
  
  return tifiles_translate_varname(varname, trans, vartype);
}

/* obsolete, replaced by the func below */
TIEXPORT char *TICALL tifiles_translate_varname2(const char *varname,
                                                 uint8_t vartype)
{
  return tifiles_translate_varname_static(varname, vartype);
}
