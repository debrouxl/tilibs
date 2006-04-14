/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 2006-2006 Romain Lievin and Kevin Kofler
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
	This unit contains raw varname to TI-charset conversion routines.
 
	The detokenization is used to translate some raw varnames into TI-charset 
	encoded varnames. Tokenization is used for TI84+ USB only.
	Depends on the calculator model.

	This is needed for the following calcs: 73/82/83/83+/84+.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"

//---

#define MAXCHARS	((8+1+8+1) * 4)

//---

// beware: raw varname is not always NUL-terminated
static char *detokenize_varname(CalcModel model, const char *src)
{
	int i;
	unsigned int tok1 = src[0] & 0xff;
	unsigned int tok2 = src[1] & 0xff;
	char *dst;

	switch (tok1) 
    {
    case 0x5C:			/* Matrix: [A] to [E] or [J] */
		switch(tok2)
		{
		case 0x00: dst = g_strdup_printf("%cA]", '\xc1'); break;
		case 0x01: dst = g_strdup_printf("%cB]", '\xc1'); break;
		case 0x02: dst = g_strdup_printf("%cC]", '\xc1'); break;
		case 0x03: dst = g_strdup_printf("%cD]", '\xc1'); break;
		case 0x04: dst = g_strdup_printf("%cE]", '\xc1'); break;
		case 0x05: dst = g_strdup_printf("%cF]", '\xc1'); break;
		case 0x06: dst = g_strdup_printf("%cG]", '\xc1'); break;
		case 0x07: dst = g_strdup_printf("%cH]", '\xc1'); break;
		case 0x08: dst = g_strdup_printf("%cI]", '\xc1'); break;
		case 0x09: dst = g_strdup_printf("%cJ]", '\xc1'); break;

		default:   dst = g_strdup_printf("%c?]", '\xc1'); break;
		}
		break;

    case 0x5D:			/* List: L1 to L6 or L1 to L0 */
		if(model == CALC_TI73)
		{
			// TI73 begins at L0
			dst = g_strdup_printf("L%c", src[1] + '\x80');	
		}
		else 
		{
			// TI8x begins at L1
			switch(tok2)
			{
			case 0x00: dst = g_strdup_printf("L%c", '\x81'); break;
			case 0x01: dst = g_strdup_printf("L%c", '\x82'); break;
			case 0x02: dst = g_strdup_printf("L%c", '\x83'); break;
			case 0x03: dst = g_strdup_printf("L%c", '\x84'); break;
			case 0x04: dst = g_strdup_printf("L%c", '\x85'); break;
			case 0x05: dst = g_strdup_printf("L%c", '\x86'); break;
			case 0x06: dst = g_strdup_printf("L%c", '\x87'); break;
			case 0x07: dst = g_strdup_printf("L%c", '\x88'); break;
			case 0x08: dst = g_strdup_printf("L%c", '\x89'); break;
			case 0x09: dst = g_strdup_printf("L%c", '\x80'); break;
			  
			default: // named list
			  dst = g_strdup_printf("1234567");
			  for (i = 0; i < 7; i++)
				dst[i] = src[i + 1];
			  break;
			}
		}
		break;
		
    case 0x5E:			/* Equations: Y1 to Y0, X1t, ... */
		switch(tok2)
		{
		case 0x10: dst = g_strdup_printf("Y%c", '\x81'); break;
		case 0x11: dst = g_strdup_printf("Y%c", '\x82'); break;
		case 0x12: dst = g_strdup_printf("Y%c", '\x83'); break;
		case 0x13: dst = g_strdup_printf("Y%c", '\x84'); break;
		case 0x14: dst = g_strdup_printf("Y%c", '\x85'); break;
		case 0x15: dst = g_strdup_printf("Y%c", '\x86'); break;
		case 0x16: dst = g_strdup_printf("Y%c", '\x87'); break;
		case 0x17: dst = g_strdup_printf("Y%c", '\x88'); break;
		case 0x18: dst = g_strdup_printf("Y%c", '\x89'); break;
		case 0x19: dst = g_strdup_printf("Y%c", '\x80'); break;

		case 0x20: dst = g_strdup_printf("X%c%c", '\x81', '\x0d'); break;
		case 0x21: dst = g_strdup_printf("Y%c%c", '\x81', '\x0d'); break;
		case 0x22: dst = g_strdup_printf("X%c%c", '\x82', '\x0d'); break;
		case 0x23: dst = g_strdup_printf("Y%c%c", '\x82', '\x0d'); break;
		case 0x24: dst = g_strdup_printf("X%c%c", '\x83', '\x0d'); break;
		case 0x25: dst = g_strdup_printf("Y%c%c", '\x83', '\x0d'); break;
		case 0x26: dst = g_strdup_printf("X%c%c", '\x84', '\x0d'); break;
		case 0x27: dst = g_strdup_printf("Y%c%c", '\x84', '\x0d'); break;
		case 0x28: dst = g_strdup_printf("X%c%c", '\x85', '\x0d'); break;
		case 0x29: dst = g_strdup_printf("Y%c%c", '\x85', '\x0d'); break;
		case 0x2a: dst = g_strdup_printf("X%c%c", '\x86', '\x0d'); break;
		case 0x2b: dst = g_strdup_printf("Y%c%c", '\x86', '\x0d'); break;

		case 0x40: dst = g_strdup_printf("r%c", '\x81'); break;
		case 0x41: dst = g_strdup_printf("r%c", '\x82'); break;
		case 0x42: dst = g_strdup_printf("r%c", '\x83'); break;
		case 0x43: dst = g_strdup_printf("r%c", '\x84'); break;
		case 0x44: dst = g_strdup_printf("r%c", '\x85'); break;
		case 0x45: dst = g_strdup_printf("r%c", '\x86'); break;

		case 0x80: 
		  if(model == CALC_TI82)
			dst = g_strdup_printf("U%c", '\xd7'); 
		  else
			dst = g_strdup_printf("u");
		  break;

		case 0x81:
		  if(model == CALC_TI82)
			dst = g_strdup_printf("V%c", '\xd7'); 
		  else
			dst = g_strdup_printf("v");
		  break;

		case 0x82:
		  if(model == CALC_TI82)
			dst = g_strdup_printf("W%c", '\xd7'); 
		  else
			dst = g_strdup_printf("w");
		  break; 
		
		default: dst = g_strdup_printf("?"); break;
		}
		break;

    case 0x60:			/* Pictures */
		if (tok2 != 0x09)
			dst = g_strdup_printf("Pic%c", tok2 + '\x81');
		else
			dst = g_strdup_printf("Pic%c", '\x80');
		break;

    case 0x61:			/* GDB */
		if (tok2 != 0x09)
			dst = g_strdup_printf("GDB%c", tok2 + '\x81');
		else
			dst = g_strdup_printf("GDB%c", '\x80');
		break;

    case 0x62:
		switch(tok2)
		{
		case 0x01: dst = g_strdup_printf("ReqEq"); break;
		case 0x02: dst = g_strdup_printf("n"); break;
		case 0x03: dst = g_strdup_printf("%c", '\xcb'); break;
		case 0x04: dst = g_strdup_printf("%c%c", '\xc6', 'x'); break;
		case 0x05: dst = g_strdup_printf("%c%c%c", '\xc6', 'x', '\x12'); break;
		case 0x06: dst = g_strdup_printf("%c%c", 'S', 'x'); break;
		case 0x07: dst = g_strdup_printf("%c%c", '\xc7', 'x'); break;
		case 0x08: dst = g_strdup_printf("minX"); break;
		case 0x09: dst = g_strdup_printf("maxX"); break;
		case 0x0a: dst = g_strdup_printf("minY"); break;
		case 0x0b: dst = g_strdup_printf("maxY"); break;
		case 0x0c: dst = g_strdup_printf("%c", '\xcc'); break;
		case 0x0d: dst = g_strdup_printf("%c%c", '\xc6', 'y'); break;
		case 0x0e: dst = g_strdup_printf("%c%c%c", '\xc6', 'y', '\x12'); break;
		case 0x0f: dst = g_strdup_printf("%c%c", 'S', 'y'); break;
		case 0x10: dst = g_strdup_printf("%c%c", '\xc7', 'y'); break;
		case 0x11: dst = g_strdup_printf("%c%c%c", '\xc6', 'x', 'y'); break;
		case 0x12: dst = g_strdup_printf("%c", 'r'); break; 
		case 0x13: dst = g_strdup_printf("Med"); break;
		case 0x14: dst = g_strdup_printf("%c%c", 'Q', '\x81'); break;
		case 0x15: dst = g_strdup_printf("%c%c", 'Q', '\x83'); break;
		case 0x16: dst = g_strdup_printf("a"); break;
		case 0x17: dst = g_strdup_printf("b"); break;
		case 0x18: dst = g_strdup_printf("c"); break;
		case 0x19: dst = g_strdup_printf("d"); break;
		case 0x1a: dst = g_strdup_printf("e"); break;
		case 0x1b: dst = g_strdup_printf("%c%c", 'x', '\x81'); break;
		case 0x1c: dst = g_strdup_printf("%c%c", 'x', '\x82'); break;
		case 0x1d: dst = g_strdup_printf("%c%c", 'x', '\x83'); break;
		case 0x1e: dst = g_strdup_printf("%c%c", 'y', '\x81'); break;
		case 0x1f: dst = g_strdup_printf("%c%c", 'y', '\x82'); break;
		case 0x20: dst = g_strdup_printf("%c%c", 'y', '\x83'); break;
		case 0x21: dst = g_strdup_printf("%c", '\xd7'); break;
		case 0x22: dst = g_strdup_printf("p"); break;
		case 0x23: dst = g_strdup_printf("z"); break;
		case 0x24: dst = g_strdup_printf("t"); break;
		case 0x25: dst = g_strdup_printf("%c%c", '\xd9', '\x12'); break;
		case 0x26: dst = g_strdup_printf("%c", '\xda'); break;
		case 0x27: dst = g_strdup_printf("df"); break;
		case 0x28: dst = g_strdup_printf("%c", '\xd8'); break;
		case 0x29: dst = g_strdup_printf("%c%c", '\xd8', '\x81'); break;
		case 0x2a: dst = g_strdup_printf("%c%c", '\xd8', '\x82'); break;
		case 0x2b: dst = g_strdup_printf("%c%c", '\xd8', '\x81'); break;
		case 0x2c: dst = g_strdup_printf("Sx%c", '\x81'); break;
		case 0x2d: dst = g_strdup_printf("n%c", '\x81'); break;
		case 0x2e: dst = g_strdup_printf("%c%c", '\xcb', '\x82'); break;
		case 0x2f: dst = g_strdup_printf("Sx%c", '\x82'); break;
		case 0x30: dst = g_strdup_printf("n%c", '\x82'); break;
		case 0x31: dst = g_strdup_printf("Sxp"); break;
		case 0x32: dst = g_strdup_printf("lower"); break;
		case 0x33: dst = g_strdup_printf("upper"); break;
		case 0x34: dst = g_strdup_printf("s"); break;
		case 0x35: dst = g_strdup_printf("r%c", '\x12'); break;
		case 0x36: dst = g_strdup_printf("R%c", '\x12'); break;
		case 0x37: dst = g_strdup_printf("df"); break;
		case 0x38: dst = g_strdup_printf("SS"); break;
		case 0x39: dst = g_strdup_printf("MS"); break;
		case 0x3a: dst = g_strdup_printf("df"); break;
		case 0x3b: dst = g_strdup_printf("SS"); break;
		case 0x3c: dst = g_strdup_printf("MS"); break;
		default: dst = g_strdup_printf("_"); break;
		}
		break;

    case 0x63:
		switch(tok2)
		{
		case 0x00: dst = g_strdup_printf("ZXscl"); break;
		case 0x01: dst = g_strdup_printf("ZYscl"); break;
		case 0x02: dst = g_strdup_printf("Xscl"); break;
		case 0x03: dst = g_strdup_printf("Yscl"); break;
		case 0x04: dst = g_strdup_printf("U%cStart", '\xd7'); break;
		case 0x05: dst = g_strdup_printf("V%cStart", '\xd7'); break;
		case 0x06: dst = g_strdup_printf("U%c-%c", '\xd7', '\x81'); break;
		case 0x07: dst = g_strdup_printf("V%c-%c", '\xd7', '\x81'); break;
		case 0x08: dst = g_strdup_printf("ZU%cStart", '\xd7'); break;
		case 0x09: dst = g_strdup_printf("ZV%cStart", '\xd7'); break;
		case 0x0a: dst = g_strdup_printf("Xmin"); break;
		case 0x0b: dst = g_strdup_printf("Xmax"); break;
		case 0x0c: dst = g_strdup_printf("Ymin"); break;
		case 0x0d: dst = g_strdup_printf("Ymax"); break;
		case 0x0e: dst = g_strdup_printf("Tmin"); break;
		case 0x0f: dst = g_strdup_printf("Tmax"); break;
		case 0x10: dst = g_strdup_printf("%cmin", '\x5b'); break;
		case 0x11: dst = g_strdup_printf("%cmax", '\x5b'); break;
		case 0x12: dst = g_strdup_printf("ZXmin"); break;
		case 0x13: dst = g_strdup_printf("ZXmax"); break;
		case 0x14: dst = g_strdup_printf("ZYmin"); break;
		case 0x15: dst = g_strdup_printf("ZYmax"); break;
		case 0x16: dst = g_strdup_printf("Z%cmin", '\x5b'); break;
		case 0x17: dst = g_strdup_printf("Z%cmax", '\x5b'); break;
		case 0x18: dst = g_strdup_printf("ZTmin"); break;
		case 0x19: dst = g_strdup_printf("ZTmax"); break;
		case 0x1a: dst = g_strdup_printf("TblMin"); break;
		case 0x1b: dst = g_strdup_printf("%cMin", '\xd7'); break;
		case 0x1c: dst = g_strdup_printf("Z%cMin", '\xd7'); break;
		case 0x1d: dst = g_strdup_printf("%cMax", '\xd7'); break;
		case 0x1e: dst = g_strdup_printf("Z%cMax", '\xd7'); break;
		case 0x1f: dst = g_strdup_printf("%cStart", '\xd7'); break;
		case 0x20: dst = g_strdup_printf("Z%cStart", '\xd7'); break;
		case 0x21: dst = g_strdup_printf("%cTbl", '\xbe'); break;
		case 0x22: dst = g_strdup_printf("Tstep"); break;
		case 0x23: dst = g_strdup_printf("%cstep", '\x5b'); break;
		case 0x24: dst = g_strdup_printf("ZTstep"); break;
		case 0x25: dst = g_strdup_printf("Z%cstep", '\x5b'); break;
		case 0x26: dst = g_strdup_printf("%cX", '\xbe'); break;
		case 0x27: dst = g_strdup_printf("%cY", '\xbe'); break;
		case 0x28: dst = g_strdup_printf("XFact"); break;
		case 0x29: dst = g_strdup_printf("YFact"); break;
		case 0x2a: dst = g_strdup_printf("TblInput"); break;
		case 0x2b: dst = g_strdup_printf("N"); break;
		case 0x2c: dst = g_strdup_printf("I%c", '\x25'); break;
		case 0x2d: dst = g_strdup_printf("PV"); break;
		case 0x2e: dst = g_strdup_printf("PMT"); break;
		case 0x2f: dst = g_strdup_printf("FV"); break;
		case 0x30: dst = g_strdup_printf("Xres"); break;
		case 0x31: dst = g_strdup_printf("ZXres"); break;
		default: dst = g_strdup_printf("_"); break;
		}
		break;

    case 0xAA:
		if (tok2 != 0x09)
			dst = g_strdup_printf("Str%c", tok2 + '\x81');
		else
			dst = g_strdup_printf("Str%c", '\x80');
		break;
    
	default:
		dst = g_strdup("12345678");
		strncpy(dst, src, 8);
		dst[8] = '\0';
		break;
    }

    return dst;
}

TIEXPORT char* TICALL ticonv_varname_detokenize(CalcModel model, const char *src)
{
	switch (model) 
	{
	case CALC_TI73:
	case CALC_TI82:
	case CALC_TI83:
	case CALC_TI83P:
	case CALC_TI84P:
		return detokenize_varname(model, src);
	case CALC_TI85:
	case CALC_TI86:
	case CALC_TI89:
	case CALC_TI89T:
	case CALC_TI92:
	case CALC_TI92P:
	case CALC_V200:
		return g_strdup(src);
	case CALC_TI84P_USB:
	case CALC_TI89T_USB:
		return g_strdup(src);
	default:
		return g_strdup("________");
  }
}

//---

static int shift(int v)
{
	if(v == 0)
		return 9;
	else 
		return v-1; 
}

TIEXPORT char* TICALL ticonv_varname_tokenize(CalcModel model, const char *src_)
{
	const unsigned char *src = src_;

	if(src[0] == '[' && src[2] == ']' && strlen(src) == 3)
	{
		// matrices
		return g_strdup_printf("%c%c", 0x5C, src[1] - 'A');
	}
	else if(src[0] == 'L' && (src[1] >= 128 && src[1] <= 137) && strlen(src) == 2)
	{
		// lists
		return g_strdup_printf("%c%c", 0x5D, shift(src[1] - 0x80));
	} 
	else if(src[0] == 'Y' && (src[1] >= 128 && src[1] <= 137) && src[2] == 0x13 && strlen(src) == 2)
	{
		// cart. equations
		return g_strdup_printf("%c%c", 0x5E, 0x10 + shift(src[1] - 0x80));
	}
	else if(src[0] == 'X' && (src[1] >= 128 && src[1] <= 133) && src[2] == 0x13 && strlen(src) == 3)
	{
		// parametric equations
		return g_strdup_printf("%c%c", 0x5E, 0x20 + 2*(src[1] - 0x81)+0);
	}
	else if(src[0] == 'Y' && (src[1] >= 128 && src[1] <= 133) && src[2] == 0x13 && strlen(src) == 3)
	{
		// parametric equations
		return g_strdup_printf("%c%c", 0x5E, 0x20 + 2*(src[1] - 0x81)+1);
	}
	else if(src[0] == 'r' && (src[1] >= 128 && src[1] <= 133) && strlen(src) == 2)
	{
		// polar equations
		return g_strdup_printf("%c%c", 0x5E, 0x40 + (src[1] - 0x81));
	}
	else if(src[0] == 2 && strlen(src) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x80);
	}
	else if(src[0] == 3 && strlen(src) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x81);
	}	
	else if(src[0] == 4 && strlen(src) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x82);
	}
	else if(src[0] == 'P' && src[1] == 'i' && src[2] == 'c' && src[3] >= '0' && src[4] <= '9' && strlen(src) == 4)
	{
		// pictures
		return g_strdup_printf("%c%c", 0x60, shift(src[1] - 0x80));
	}
	else if(src[0] == 'G' && src[1] == 'D' && src[2] == 'B' && src[3] >= '0' && src[4] <= '9' && strlen(src) == 4)
	{
		// pictures
		return g_strdup_printf("%c%c", 0x61, shift(src[1] - 0x80));
	}
	else if(src[0] == 'S' && src[1] == 't' && src[2] == 'r' && src[3] >= '0' && src[4] <= '9' && strlen(src) == 4)
	{
		// pictures
		return g_strdup_printf("%c%c", 0xAA, shift(src[1] - 0x80));
	}

	return g_strdup("");
}