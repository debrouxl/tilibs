/* Hey EMACS -*- linux-c -*- */
/* $Id: ticonv.c 1721 2006-01-21 13:58:14Z roms $ */

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
	This unit contains detokenization routines.
  
	This is used to translate some varnames into a more readable name.
	Depends on the calculator type and the variable type.

	This is needed for the following calcs: 73/82/83/83+/84+ only.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"

static char *detokenize_varname(CalcModel model, const char *src, char *dst, unsigned int vartype)
{
	int i;
	unsigned int tok1 = src[0] & 0xff;
	unsigned int tok2 = src[1] & 0xff;

	switch (vartype) 
    {
    case 0x0B:	// TI82_WDW
		strcpy(dst, "Window");
		return dst;
    case 0x0C:	// TI82_ZSTO
		strcpy(dst, "RclWin");
		return dst;
    case 0x0D:	// TI82_TAB
		strcpy(dst, "TblSet");
		return dst;
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

		case 0x20: sprintf(dst, "X%c%c", '\x81', '\x0d'); break;
		case 0x21: sprintf(dst, "Y%c%c", '\x81', '\x0d'); break;
		case 0x22: sprintf(dst, "X%c%c", '\x82', '\x0d'); break;
		case 0x23: sprintf(dst, "Y%c%c", '\x82', '\x0d'); break;
		case 0x24: sprintf(dst, "X%c%c", '\x83', '\x0d'); break;
		case 0x25: sprintf(dst, "Y%c%c", '\x83', '\x0d'); break;
		case 0x26: sprintf(dst, "X%c%c", '\x84', '\x0d'); break;
		case 0x27: sprintf(dst, "Y%c%c", '\x84', '\x0d'); break;
		case 0x28: sprintf(dst, "X%c%c", '\x85', '\x0d'); break;
		case 0x29: sprintf(dst, "Y%c%c", '\x85', '\x0d'); break;
		case 0x2a: sprintf(dst, "X%c%c", '\x86', '\x0d'); break;
		case 0x2b: sprintf(dst, "Y%c%c", '\x86', '\x0d'); break;

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
		
		default: sprintf(dst, "?"); break;
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
		case 0x28: sprintf(dst, "%c", '\xd8'); break;
		case 0x29: sprintf(dst, "%c%c", '\xd8', '\x81'); break;
		case 0x2a: sprintf(dst, "%c%c", '\xd8', '\x82'); break;
		case 0x2b: sprintf(dst, "%c%c", '\xd8', '\x81'); break;
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
		strncpy(dst, src, 8);
		dst[8] = '\0';
		break;
    }

    return dst;
}

static char* ticonv_varname_detokenize_s(CalcModel model, const char *src, char *dst, unsigned int vartype)
{
	switch (model) 
	{
	case CALC_TI73:
	case CALC_TI82:
	case CALC_TI83:
	case CALC_TI83P:
	case CALC_TI84P:
		return detokenize_varname(model, src, dst, vartype);
	case CALC_TI85:
	case CALC_TI86:
	case CALC_TI89:
	case CALC_TI89T:
	case CALC_TI92:
	case CALC_TI92P:
	case CALC_V200:
		return strncpy(dst, src, 9);
	default:
		return strcpy(dst, "________");
  }

	return dst;
}

static char* ticonv_varname_detokenize(CalcModel model, const char *src, unsigned int vartype)
{
	static char dst[17];
	return g_strdup(ticonv_varname_detokenize_s(model, src, dst, vartype));
}

/**
 * ticonv_varname_to_utf16_s:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate.
 * @dst: a buffer where to placed the result (18 chars max).
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 * 
 * Static version.
 *
 * Return value: the %dst string.
 **/
TIEXPORT unsigned short* TICALL ticonv_varname_to_utf16_s(CalcModel model, const char *src, unsigned short *dst, unsigned int vartype)
{
	char tmp[32];

	ticonv_varname_detokenize_s(model, src, tmp, vartype);
	ticonv_charset_ti_to_utf16_s(model, tmp, dst);

	return dst;
}

/**
 * ticonv_varname_to_utf16:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate.
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 *
 * Dynamic version.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT unsigned short* TICALL ticonv_varname_to_utf16(CalcModel model, const char *src, unsigned int vartype)
{
	char *tmp;
	unsigned short *utf16;

	tmp = ticonv_varname_detokenize(model, src, vartype);
	utf16 = ticonv_charset_ti_to_utf16(model, tmp);

	g_free(tmp);
	return utf16;
}

/**
 * ticonv_varname_to_utf8_s:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate.
 * @dst: a buffer where to placed the result (18 chars max).
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 *
 * Static version.
 *
 * Return value: the %dst string.
 **/
TIEXPORT char* TICALL ticonv_varname_to_utf8_s(CalcModel model, const char *src, char *dst, unsigned int vartype)
{
	unsigned short tmp[32];
	gchar *utf8;

	ticonv_varname_to_utf16_s(model, src, tmp, vartype);
	utf8 = ticonv_utf16_to_utf8(tmp);

	strcpy(dst, utf8);
	g_free(utf8);

	return dst;
}

/**
 * ticonv_varname_to_utf8:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate.
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 *
 * Dynamic version.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT char* TICALL ticonv_varname_to_utf8(CalcModel model, const char *src, unsigned int vartype)
{
	unsigned short *utf16;
	gchar *utf8;

	utf16 = ticonv_varname_to_utf16(model, src, vartype);
	utf8 = ticonv_utf16_to_utf8(utf16);

	g_free(utf16);
	return utf8;
}