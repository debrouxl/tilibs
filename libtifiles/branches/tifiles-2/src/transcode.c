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
#include "tifiles.h"
#include "macros.h"
#include "logging.h"


/* 
   Variable name detokenization. This is needed for the following calcs:
   - 73/82/83/83+: binary-coded to TI-charset
   - 85/86: no operation
   - 89/92/92+: no operation
   See the 'TI Link Guide' for more informations...

  Vriable name charset conversion (ascii, latin1 and UTF8)
*/


static char *ti8x_detokenize_varname(TiCalcType model, const char *src, char *dst, uint8_t vartype)
{
  int i;
  uint8_t tok1 = src[0] & 0xff;
  uint8_t tok2 = src[1] & 0xff;

  switch (vartype) 
    {
    case TI82_WDW:
      strcpy(dst, "Window");
      return dst;
      break;
    case TI82_ZSTO:
      strcpy(dst, "RclWin");
      return dst;
      break;
    case TI82_TAB:
      strcpy(dst, "TblSet");
      return dst;
      break;
    default:
      break;
    }
  
  switch (tok1) 
    {
    case 0x5C:			/* Matrix: [A] to [E]/[J] */
      switch(tok2)
	{
	case 0x00: sprintf(dst, "%cA]", '\xc1'); break;
	case 0x01: sprintf(dst, "%cB]", '\xc1'); break;
	case 0x02: sprintf(dst, "%cC]", '\xc1'); break;
	case 0x03: sprintf(dst, "%cD]", '\xc1'); break;
	case 0x04: sprintf(dst, "%cE]", '\xc1'); break;
	case 0x05: sprintf(dst, "%cF]", '\xc1'); break;
	case 0x06: sprintf(dst, "%cG]", '\xc1'); break;
	case 0x07: sprintf(dst, "%cH]", '\xc1'); break;
	case 0x08: sprintf(dst, "%cI]", '\xc1'); break;
	case 0x09: sprintf(dst, "%cJ]", '\xc1'); break;

	default:   sprintf(dst, "%c?]", '\xc1'); break;
	}
      break;
    case 0x5D:			/* List: L1 to L6/L0 */
      if(model == CALC_TI73) //TI73 != TI83 here
	sprintf(dst, "L%c", src[1] + '\x80');	
      else 
	{// TI73 begins at L0, others at L1
	  switch(tok2)
	    {
	    case 0x00: sprintf(dst, "L%c", '\x81'); break;
	    case 0x01: sprintf(dst, "L%c", '\x82'); break;
	    case 0x02: sprintf(dst, "L%c", '\x83'); break;
	    case 0x03: sprintf(dst, "L%c", '\x84'); break;
	    case 0x04: sprintf(dst, "L%c", '\x85'); break;
	    case 0x05: sprintf(dst, "L%c", '\x86'); break;
	    case 0x06: sprintf(dst, "L%c", '\x87'); break;
	    case 0x07: sprintf(dst, "L%c", '\x88'); break;
	    case 0x08: sprintf(dst, "L%c", '\x89'); break;
	    case 0x09: sprintf(dst, "L%c", '\x80'); break;
	      
	    default: // named list
	      for (i = 0; i < 7; i++)
		dst[i] = src[i + 1];
	      break;
	    }
	}
      break;	
    case 0x5E:			/* Equations: Y1 to Y0, ... */
      switch(tok2)
	{
	case 0x10: sprintf(dst, "Y%c", '\x81'); break;
	case 0x11: sprintf(dst, "Y%c", '\x82'); break;
	case 0x12: sprintf(dst, "Y%c", '\x83'); break;
	case 0x13: sprintf(dst, "Y%c", '\x84'); break;
	case 0x14: sprintf(dst, "Y%c", '\x85'); break;
	case 0x15: sprintf(dst, "Y%c", '\x86'); break;
	case 0x16: sprintf(dst, "Y%c", '\x87'); break;
	case 0x17: sprintf(dst, "Y%c", '\x88'); break;
	case 0x18: sprintf(dst, "Y%c", '\x89'); break;
	case 0x19: sprintf(dst, "Y%c", '\x80'); break;

	case 0x20: sprintf(dst, "X%ct", '\x81'); break;
	case 0x21: sprintf(dst, "Y%ct", '\x81'); break;
	case 0x22: sprintf(dst, "X%ct", '\x82'); break;
	case 0x23: sprintf(dst, "Y%ct", '\x82'); break;
	case 0x24: sprintf(dst, "X%ct", '\x83'); break;
	case 0x25: sprintf(dst, "Y%ct", '\x83'); break;
	case 0x26: sprintf(dst, "X%ct", '\x84'); break;
	case 0x27: sprintf(dst, "Y%ct", '\x84'); break;
	case 0x28: sprintf(dst, "X%ct", '\x85'); break;
	case 0x29: sprintf(dst, "Y%ct", '\x85'); break;
	case 0x2a: sprintf(dst, "X%ct", '\x86'); break;
	case 0x2b: sprintf(dst, "Y%ct", '\x86'); break;

	case 0x40: sprintf(dst, "r%c", '\x81'); break;
	case 0x41: sprintf(dst, "r%c", '\x82'); break;
	case 0x42: sprintf(dst, "r%c", '\x83'); break;
	case 0x43: sprintf(dst, "r%c", '\x84'); break;
	case 0x44: sprintf(dst, "r%c", '\x85'); break;
	case 0x45: sprintf(dst, "r%c", '\x86'); break;

	case 0x80: 
	  if(model == CALC_TI82)
	    sprintf(dst, "U%c", '\xd7'); 
	  else
	    sprintf(dst, "u");
	  break;
	case 0x81:
	  if(model == CALC_TI82)
	    sprintf(dst, "V%c", '\xd7'); 
	  else
	    sprintf(dst, "v");
	  break;
	case 0x82:
	  if(model == CALC_TI82)
	    sprintf(dst, "W%c", '\xd7'); 
	  else
	    sprintf(dst, "w");
	  break; 
	
	default: sprintf(dst, "_"); break;
    }
      break;
    case 0x60:			/* Pictures */
      if (tok2 != 0x09)
	sprintf(dst, "Pic%c", tok2 + '\x81');
      else
	sprintf(dst, "Pic%c", '\x80');
      break;
    case 0x61:			/* GDB */
      if (tok2 != 0x09)
	sprintf(dst, "GDB%c", tok2 + '\x81');
      else
	sprintf(dst, "GDB%c", '\x80');
      break;
    case 0x62:
      switch(tok2)
	{
	case 0x01: sprintf(dst, "ReqEq"); break;
	case 0x02: sprintf(dst, "n"); break;
	case 0x03: sprintf(dst, "%c", '\xcb'); break;
	case 0x04: sprintf(dst, "%c%c", '\xc6', 'x'); break;
	case 0x05: sprintf(dst, "%c%c%c", '\xc6', 'x', '\x12'); break;
	case 0x06: sprintf(dst, "%c%c", 'S', 'x'); break;
	case 0x07: sprintf(dst, "%c%c", '\xc7', 'x'); break;
	case 0x08: sprintf(dst, "minX"); break;
	case 0x09: sprintf(dst, "maxX"); break;
	case 0x0a: sprintf(dst, "minY"); break;
	case 0x0b: sprintf(dst, "maxY"); break;
	case 0x0c: sprintf(dst, "%c", '\xcc'); break;
	case 0x0d: sprintf(dst, "%c%c", '\xc6', 'y'); break;
	case 0x0e: sprintf(dst, "%c%c%c", '\xc6', 'y', '\x12'); break;
	case 0x0f: sprintf(dst, "%c%c", 'S', 'y'); break;
	case 0x10: sprintf(dst, "%c%c", '\xc7', 'y'); break;
	case 0x11: sprintf(dst, "%c%c%c", '\xc6', 'x', 'y'); break;
	case 0x12: sprintf(dst, "%c", 'r'); break; 
	case 0x13: sprintf(dst, "Med"); break;
	case 0x14: sprintf(dst, "%c%c", 'Q', '\x81'); break;
	case 0x15: sprintf(dst, "%c%c", 'Q', '\x83'); break;
	case 0x16: sprintf(dst, "a"); break;
	case 0x17: sprintf(dst, "b"); break;
	case 0x18: sprintf(dst, "c"); break;
	case 0x19: sprintf(dst, "d"); break;
	case 0x1a: sprintf(dst, "e"); break;
	case 0x1b: sprintf(dst, "%c%c", 'x', '\x81'); break;
	case 0x1c: sprintf(dst, "%c%c", 'x', '\x82'); break;
	case 0x1d: sprintf(dst, "%c%c", 'x', '\x83'); break;
	case 0x1e: sprintf(dst, "%c%c", 'y', '\x81'); break;
	case 0x1f: sprintf(dst, "%c%c", 'y', '\x82'); break;
	case 0x20: sprintf(dst, "%c%c", 'y', '\x83'); break;
	case 0x21: sprintf(dst, "%c", '\xd7'); break;
	case 0x22: sprintf(dst, "p"); break;
	case 0x23: sprintf(dst, "z"); break;
	case 0x24: sprintf(dst, "t"); break;
	case 0x25: sprintf(dst, "%c%c", '\xd9', '\x12'); break;
	case 0x26: sprintf(dst, "%c", '\xda'); break;
	case 0x27: sprintf(dst, "df"); break;
	case 0x28: sprintf(dst, "%c", '\xbc'); break;
	case 0x29: sprintf(dst, "%c%c", '\xbc', '\x81'); break;
	case 0x2a: sprintf(dst, "%c%c", '\xbc', '\x82'); break;
	case 0x2b: sprintf(dst, "%c%c", '\xcb', '\x81'); break;
	case 0x2c: sprintf(dst, "Sx%c", '\x81'); break;
	case 0x2d: sprintf(dst, "n%c", '\x81'); break;
	case 0x2e: sprintf(dst, "%c%c", '\xcb', '\x82'); break;
	case 0x2f: sprintf(dst, "Sx%c", '\x82'); break;
	case 0x30: sprintf(dst, "n%c", '\x82'); break;
	case 0x31: sprintf(dst, "Sxp"); break;
	case 0x32: sprintf(dst, "lower"); break;
	case 0x33: sprintf(dst, "upper"); break;
	case 0x34: sprintf(dst, "s"); break;
	case 0x35: sprintf(dst, "r%c", '\x12'); break;
	case 0x36: sprintf(dst, "R%c", '\x12'); break;
	case 0x37: sprintf(dst, "df"); break;
	case 0x38: sprintf(dst, "SS"); break;
	case 0x39: sprintf(dst, "MS"); break;
	case 0x3a: sprintf(dst, "df"); break;
	case 0x3b: sprintf(dst, "SS"); break;
	case 0x3c: sprintf(dst, "MS"); break;
	default: sprintf(dst, "_"); break;
	}
      break;
    case 0x63:
      switch(tok2)
	{
	case 0x00: sprintf(dst, "ZXscl"); break;
	case 0x01: sprintf(dst, "ZYscl"); break;
	case 0x02: sprintf(dst, "Xscl"); break;
	case 0x03: sprintf(dst, "Yscl"); break;
	case 0x04: sprintf(dst, "U%cStart", '\xd7'); break;
	case 0x05: sprintf(dst, "V%cStart", '\xd7'); break;
	case 0x06: sprintf(dst, "U%c-%c", '\xd7', '\x81'); break;
	case 0x07: sprintf(dst, "V%c-%c", '\xd7', '\x81'); break;
	case 0x08: sprintf(dst, "ZU%cStart", '\xd7'); break;
	case 0x09: sprintf(dst, "ZV%cStart", '\xd7'); break;
	case 0x0a: sprintf(dst, "Xmin"); break;
	case 0x0b: sprintf(dst, "Xmax"); break;
	case 0x0c: sprintf(dst, "Ymin"); break;
	case 0x0d: sprintf(dst, "Ymax"); break;
	case 0x0e: sprintf(dst, "Tmin"); break;
	case 0x0f: sprintf(dst, "Tmax"); break;
	case 0x10: sprintf(dst, "%cmin", '\x5b'); break;
	case 0x11: sprintf(dst, "%cmax", '\x5b'); break;
	case 0x12: sprintf(dst, "ZXmin"); break;
	case 0x13: sprintf(dst, "ZXmax"); break;
	case 0x14: sprintf(dst, "ZYmin"); break;
	case 0x15: sprintf(dst, "ZYmax"); break;
	case 0x16: sprintf(dst, "Z%cmin", '\x5b'); break;
	case 0x17: sprintf(dst, "Z%cmax", '\x5b'); break;
	case 0x18: sprintf(dst, "ZTmin"); break;
	case 0x19: sprintf(dst, "ZTmax"); break;
	case 0x1a: sprintf(dst, "TblMin"); break;
	case 0x1b: sprintf(dst, "%cMin", '\xd7'); break;
	case 0x1c: sprintf(dst, "Z%cMin", '\xd7'); break;
	case 0x1d: sprintf(dst, "%cMax", '\xd7'); break;
	case 0x1e: sprintf(dst, "Z%cMax", '\xd7'); break;
	case 0x1f: sprintf(dst, "%cStart", '\xd7'); break;
	case 0x20: sprintf(dst, "Z%cStart", '\xd7'); break;
	case 0x21: sprintf(dst, "%cTbl", '\xbe'); break;
	case 0x22: sprintf(dst, "Tstep"); break;
	case 0x23: sprintf(dst, "%cstep", '\x5b'); break;
	case 0x24: sprintf(dst, "ZTstep"); break;
	case 0x25: sprintf(dst, "Z%cstep", '\x5b'); break;
	case 0x26: sprintf(dst, "%cX", '\xbe'); break;
	case 0x27: sprintf(dst, "%cY", '\xbe'); break;
	case 0x28: sprintf(dst, "XFact"); break;
	case 0x29: sprintf(dst, "YFact"); break;
	case 0x2a: sprintf(dst, "TblInput"); break;
	case 0x2b: sprintf(dst, "N"); break;
	case 0x2c: sprintf(dst, "I%c", '\x25'); break;
	case 0x2d: sprintf(dst, "PV"); break;
	case 0x2e: sprintf(dst, "PMT"); break;
	case 0x2f: sprintf(dst, "FV"); break;
	case 0x30: sprintf(dst, "Xres"); break;
	case 0x31: sprintf(dst, "ZXres"); break;
	default: sprintf(dst, "_"); break;
	}
      break;
    case 0xAA:
      if (tok2 != 0x09)
	sprintf(dst, "Str%c", tok2 + '\x81');
      else
	sprintf(dst, "Str%c", '\x80');
      break;
    default:
      strcpy(dst, src);
      break;
    }

    return dst;
}


TIEXPORT char* TICALL tixx_detokenize_varname(TiCalcType model, 
												 const char *src, 
												 char *dst,
												 uint8_t vartype)
{
  switch (model) 
  {
  case CALC_TI73:
  case CALC_TI82:
  case CALC_TI83:
  case CALC_TI83P:
  case CALC_TI84P:
    return ti8x_detokenize_varname(model, src, dst, vartype);
    break;
  case CALC_TI85:
  case CALC_TI86:
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI92:
  case CALC_TI92P:
  case CALC_V200:
    return strcpy(dst, src);
    break;
  default:
    return strcpy(dst, "________");
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

/*
  Convert string to pure ASCII.
  Note: src & dst will have the same length.
*/
TIEXPORT char* TICALL tifiles_transcode_to_ascii(TiCalcType model, char* dst, const char *src)
{
  char *dest = dst;
  TRANSCODE_TO_ASCII f = NULL;

  switch(model)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: 
	case CALC_TI84P: 
		f = transcode_from_ti83_charset_to_ascii; 
      break;
    case CALC_TI82:  
		f = transcode_from_ti82_charset_to_ascii; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  
		f = transcode_from_ti85_charset_to_ascii; 
      break;
    case CALC_TI89:
    case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  
		f = transcode_from_ti9x_charset_to_ascii; 
      break;
	default:
		tifiles_warning("tifiles_transcode_to_ascii: invalid calc type");
		break;
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
TIEXPORT char* TICALL tifiles_transcode_to_latin1(TiCalcType model, char* dst, const char *src)
{
  char *dest = dst;
  TRANSCODE_TO_LATIN1 f = NULL;

  switch(model)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: 
	case CALC_TI84P:
		f = transcode_from_ti83_charset_to_latin1; 
      break;
    case CALC_TI82:  
		f = transcode_from_ti82_charset_to_latin1; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  
		f = transcode_from_ti85_charset_to_latin1; 
      break;
    case CALC_TI89:
    case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  
		f = transcode_from_ti9x_charset_to_latin1; 
      break;
	default:
	  tifiles_warning("tifiles_transcode_to_latin1: invalid calc type");
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
TIEXPORT char* TICALL tifiles_transcode_to_utf8(TiCalcType model, char* dst, const char *src)
{
  char *dest = dst;
  uint16_t wchar;
  TRANSCODE_TO_UNICODE f = NULL;

  switch(model)
    {
    case CALC_TI73:
    case CALC_TI83:
    case CALC_TI83P: 
	case CALC_TI84P: 
		f = transcode_from_ti83_charset_to_utf8; 
      break;
    case CALC_TI82:  
		f = transcode_from_ti82_charset_to_utf8; 
      break;
    case CALC_TI85:  
    case CALC_TI86:  
		f = transcode_from_ti85_charset_to_utf8; 
      break;
    case CALC_TI89:
	case CALC_TI89T:
    case CALC_TI92:
    case CALC_TI92P:
    case CALC_V200:  
		f = transcode_from_ti9x_charset_to_utf8; 
      break;
	default:
	  tifiles_warning("tifiles_transcode_to_utf8: invalid calc type");
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
static TiFileEncoding tifiles_encoding = ENCODING_LATIN1;

TIEXPORT void TICALL tifiles_translate_set_encoding(TiFileEncoding encoding)
{
  tifiles_encoding = encoding;
}

TIEXPORT TiFileEncoding TICALL tifiles_translate_get_encoding(void)
{
  return tifiles_encoding;
}


/* 
   Variable name translation: detokenization + charset transcoding.
*/

char *tixx_translate_varname(TiCalcType model, char *dst, const char *src, uint8_t vartype)
{
  char detokenized[18];

  tixx_detokenize_varname(model, src, detokenized, vartype);

  switch(tifiles_encoding)
  {
  case ENCODING_ASCII:   tifiles_transcode_to_ascii(model, dst, detokenized); break;
  case ENCODING_LATIN1:  tifiles_transcode_to_latin1(model, dst, detokenized); break;
  case ENCODING_UNICODE: tifiles_transcode_to_utf8(model, dst, detokenized); break;
  }

  return dst;
}

TIEXPORT char *TICALL tifiles_translate_varname(TiCalcType model, char *dst, const char *src, uint8_t vartype)
{
	return tixx_translate_varname(model, dst, src, vartype);
}

TIEXPORT char *TICALL tifiles_translate_varname_static(TiCalcType model, const char *src, uint8_t vartype)
{
  static char trans[18];
  
  return tifiles_translate_varname(model, trans, src, vartype);
}
