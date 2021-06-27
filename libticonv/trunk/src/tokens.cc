/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - charset library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	This unit contains raw varname to TI-charset conversion routines.
 
	The detokenization is used to translate some raw varnames into TI-charset 
	encoded varnames. Tokenization is the reverse way.
	Many functions depends on the calculator model and the variable type ID.

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

static char *detokenize_vartype(CalcModel model, const char *src, unsigned char type)
{
	char *dst;

	switch(model)
	{
	case CALC_TI73:
		if (type == 0x0F)
		{
			return (dst = g_strdup("Window"));
		}
		if (type == 0x11)
		{
			return (dst = g_strdup("TblSet"));
		}
		break;
	case CALC_TI82:
		if (type == 0x0B)
		{
			return (dst = g_strdup("Window"));
		}
		if (type == 0x0C)
		{
			return (dst = g_strdup("RclWin"));
		}
		if (type == 0x0D)
		{
			return (dst = g_strdup("TblSet"));
		}
		break;
	case CALC_TI83:
	case CALC_TI83P:
	case CALC_TI84P:
	case CALC_TI84PC:
	case CALC_TI84P_USB:
	case CALC_TI84PC_USB:
	case CALC_TI83PCE_USB:
	case CALC_TI84PCE_USB:
	case CALC_TI82A_USB:
	case CALC_TI84PT_USB:
	case CALC_TI82AEP_USB:
		if (type == 0x0F)
		{
			return (dst = g_strdup("Window"));
		}
		if (type == 0x10)
		{
			return (dst = g_strdup("RclWin"));
		}
		if (type == 0x11)
		{
			return (dst = g_strdup("TblSet"));
		}
		break;
	case CALC_TI85:
	case CALC_TI86:
		if (type == 0x17)
		{
			return (dst = g_strdup("Func"));
		}
		if (type == 0x18)
		{
			return (dst = g_strdup("Pol"));
		}
		if (type == 0x19)
		{
			return (dst = g_strdup("Param"));
		}
		if (type == 0x1A)
		{
			return (dst = g_strdup("DifEq"));
		}
		if (type == 0x1B)
		{
			return (dst = g_strdup("ZRCL"));
		}
		break;
	default: 
		break;
	}

	return NULL;
}

// beware: raw varname is not always NUL-terminated
static char *detokenize_varname(CalcModel model, const char *src, unsigned char type)
{
	int i;
	unsigned int tok1 = src[0] & 0xff;
	unsigned int tok2 = src[1] & 0xff;
	char *dst;

	switch (tok1) 
	{
	case 0x3C:			/* Image: Image1 to Image0 */
		if (type == 0x1A)
		{
			if (tok2 != 0x09)
			{
				dst = g_strdup_printf("Image%d", tok2 + 1);
			}
			else
			{
				dst = g_strdup("Image0");
			}
		}
		else
		{
			dst = g_strndup(src, 8);
		}
		break;

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
		if (model == CALC_TI73)
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

			case 0x40: dst = g_strdup("IDList"); break;

			default: // named list (TI84+/USB)
				dst = (char *)g_malloc0(9);
				for (i = 0; i < 7; i++)
				{
					dst[i] = src[i + 1];
				}
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
			if (model == CALC_TI82)
			{
				dst = g_strdup_printf("U%c", '\xd7');
			}
			else if (model == CALC_TI73)
			{
				dst = g_strdup_printf("C\x81");
			}
			else
			{
				dst = g_strdup("u");
			}
			break;

		case 0x81:
			if (model == CALC_TI82)
			{
				dst = g_strdup_printf("V%c", '\xd7');
			}
			else if (model == CALC_TI73)
			{
				dst = g_strdup_printf("C\x82");
			}
			else
			{
				dst = g_strdup("v");
			}
			break;

		case 0x82:
			if (model == CALC_TI82)
			{
				dst = g_strdup_printf("W%c", '\xd7');
			}
			else if (model == CALC_TI73)
			{
				dst = g_strdup_printf("C\x83");
			}
			else
			{
				dst = g_strdup("w");
			}
			break;

		case 0x83:
			if (model == CALC_TI73)
			{
				dst = g_strdup_printf("C\x84");
			}
			else
			{
				dst = g_strdup("?");
			}
			break;

		default: dst = g_strdup("?"); break;
		}
		break;

	case 0x60:			/* Pictures */
		if (model == CALC_TI73)
		{
			dst = g_strdup_printf("Pic%d", tok2);
		}
		else if (tok2 != 0x09)
		{
			dst = g_strdup_printf("Pic%d", tok2 + 1);
		}
		else
		{
			dst = g_strdup("Pic0");
		}
		break;

	case 0x61:			/* GDB */
		if (model == CALC_TI73)
		{
			dst = g_strdup_printf("GDB%d", tok2);
		}
		else if (tok2 != 0x09)
		{
			dst = g_strdup_printf("GDB%d", tok2 + 1);
		}
		else
		{
			dst = g_strdup("GDB0");
		}
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
		default: dst = g_strdup("_"); break;
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
		default: dst = g_strdup("_"); break;
		}
		break;

	case 0xAA:
		if (model == CALC_TI73)
		{
			dst = g_strdup_printf("Str%d", tok2);
		}
		else if (tok2 != 0x09)
		{
			dst = g_strdup_printf("Str%d", tok2 + 1);
		}
		else
		{
			dst = g_strdup("Str0");
		}
		break;

	default:
		dst = g_strdup("12345678");
		strncpy(dst, src, 8);
		dst[8] = '\0';
		break;
	}

	return dst;
}

/**
 * ticonv_varname_detokenize:
 * @model: hand-held model
 * @src: binary string to detokenize
 *
 * This function translates a binary variable name (as used on TI8x) into a human readable one.
 *
 * Return value: a newly allocated string. Must be freed using ticonv_varname_free() when no longer used.
 **/
char* TICALL ticonv_varname_detokenize(CalcModel model, const char *src, unsigned char type)
{
	char *dst;

	if (src == NULL)
	{
		return g_strdup("________");
	}

	switch (model) 
	{
	case CALC_TI73:
	case CALC_TI82:
	case CALC_TI83:
	case CALC_TI83P:
	case CALC_TI84P:
	case CALC_TI84PC:
		dst = detokenize_vartype(model, src, type);
		if (dst)
		{
			return dst;
		}
		return detokenize_varname(model, src, type);
	case CALC_TI85:
	case CALC_TI86:
		dst = detokenize_vartype(model, src, type);
		if (dst)
		{
			return dst;
		}
	case CALC_TI89:
	case CALC_TI89T:
	case CALC_TI92:
	case CALC_TI92P:
	case CALC_V200:
		return g_strdup(src);
	case CALC_TI84P_USB:
	case CALC_TI84PC_USB:
	case CALC_TI83PCE_USB:
	case CALC_TI84PCE_USB:
	case CALC_TI82A_USB:
	case CALC_TI84PT_USB:
	case CALC_TI82AEP_USB:
	case CALC_TI89T_USB:
		return g_strdup(src);
	case CALC_NSPIRE:
	case CALC_NSPIRE_CRADLE:
	case CALC_NSPIRE_CLICKPAD:
	case CALC_NSPIRE_CLICKPAD_CAS:
	case CALC_NSPIRE_TOUCHPAD:
	case CALC_NSPIRE_TOUCHPAD_CAS:
	case CALC_NSPIRE_CX:
	case CALC_NSPIRE_CX_CAS:
	case CALC_NSPIRE_CMC:
	case CALC_NSPIRE_CMC_CAS:
	case CALC_NSPIRE_CXII:
	case CALC_NSPIRE_CXII_CAS:
	case CALC_NSPIRE_CXIIT:
	case CALC_NSPIRE_CXIIT_CAS:
		return g_strdup(src);
	default:
		return g_strdup("________");
	}
}

//---

static int shift(int v)
{
	return (v == 0) ? 9 : v-1;
}

//FIXME: does not work with named list because we should pass the vartype, too
/**
 * ticonv_varname_tokenize:
 * @model: hand-held model
 * @src: binary string to tokenize
 *
 * This function tries and translates a human-readable variable name into a binary name (as used on TI8x).
 *
 * Return value: a newly allocated string. Must be freed using ticonv_varname_free() when no longer used.
 **/
char* TICALL ticonv_varname_tokenize(CalcModel model, const char *src_, unsigned char type)
{
	const unsigned char *src = (const unsigned char *)src_;

	if (src == NULL)
	{
		return NULL;
	}

	switch(model)
	{
		case CALC_TI73:
			if (!strcmp("Window", src_) || type == 0x0F)
			{
				return g_strdup("");
			}
			if (!strcmp("TblSet", src_) || type == 0x11)
			{
				return g_strdup("");
			}
		break;
		case CALC_TI82:
			if (!strcmp("Window", src_) || type == 0x0B)
			{
				return g_strdup("");
			}
			if (!strcmp("RclWin", src_) || type == 0x0C)
			{
				return g_strdup("");
			}
			if (!strcmp("TblSet", src_) || type == 0x0D)
			{
				return g_strdup("");
			}
		break;
		case CALC_TI83:
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84PC:
			if (!strcmp("Window", src_) || type == 0x0F)
			{
				return g_strdup("");
			}
			if (!strcmp("RclWin", src_) || type == 0x10)
			{
				return g_strdup("");
			}
			if (!strcmp("TblSet", src_) || type == 0x11)
			{
				return g_strdup("");
			}
		break;
		case CALC_TI85:
		case CALC_TI86:
			if (!strcmp("Func", src_)  || type == 0x17)
			{
				return g_strdup("");
			}
			if (!strcmp("Pol", src_)   || type == 0x18)
			{
				return g_strdup("");
			}
			if (!strcmp("Param", src_) || type == 0x19)
			{
				return g_strdup("");
			}
			if (!strcmp("DifEq", src_) || type == 0x1A)
			{
				return g_strdup("");
			}
			if (!strcmp("ZRCL", src_)  || type == 0x1B)
			{
				return g_strdup("");
			}
		break;
		default:
		break;
	}

	if (type == 0x01 && (model == CALC_TI83P || model == CALC_TI84P || model == CALC_TI84PC))
	{
		// Named Lists
		char *str = (char *)g_malloc0(9);

		str[0] = 0x5D;
		strncpy(str+1, src_, 7);
		str[8] = '\0';

		return str;
	}

	if (src[0] == '[' && src[2] == ']' && strlen(src_) == 3)
	{
		// matrices
		return g_strdup_printf("%c%c", 0x5C, src[1] - 'A');
	}
	else if (src[0] == 'L' && (src[1] >= 128 && src[1] <= 137) && strlen(src_) == 2)
	{
		// lists
		return g_strdup_printf("%c%c", 0x5D, (model == CALC_TI73 ? src[1] - 0x80 : shift(src[1] - 0x80)));
	} 
	else if (src[0] == 'Y' && (src[1] >= 128 && src[1] <= 137) && strlen(src_) == 2)
	{
		// cartesian equations
		return g_strdup_printf("%c%c", 0x5E, 0x10 + shift(src[1] - 0x80));
	}
	else if (src[0] == 'X' && (src[1] >= 128 && src[1] <= 133) && strlen(src_) == 3)
	{
		// parametric equations
		return g_strdup_printf("%c%c", 0x5E, 0x20 + 2*(src[1] - 0x81)+0);
	}
	else if (src[0] == 'Y' && (src[1] >= 128 && src[1] <= 133) && strlen(src_) == 3)
	{
		// parametric equations
		return g_strdup_printf("%c%c", 0x5E, 0x20 + 2*(src[1] - 0x81)+1);
	}
	else if (src[0] == 'r' && (src[1] >= 128 && src[1] <= 133) && strlen(src_) == 2)
	{
		// polar equations
		return g_strdup_printf("%c%c", 0x5E, 0x40 + (src[1] - 0x81));
	}
	else if (model == CALC_TI73 && src[0] == 'C' && (src[1] >= 128 && src[1] <= 131) && strlen(src_) == 2)
	{
		// constant equations
		return g_strdup_printf("%c%c", 0x5E, 0x80 + shift(src[1] - 0x80));
	}
	else if (src[0] == 2 && strlen(src_) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x80);
	}
	else if (src[0] == 3 && strlen(src_) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x81);
	}	
	else if (src[0] == 4 && strlen(src_) == 1)
	{
		return g_strdup_printf("%c%c", 0x5E, 0x82);
	}
	else if (src[0] == 'P' && src[1] == 'i' && src[2] == 'c' && src[3] >= '0' && src[3] <= '9' && strlen(src_) == 4)
	{
		// pictures
		return g_strdup_printf("%c%c", 0x60, (model == CALC_TI73 ? src[3] - '0' : shift(src[3] - '0')));
	}
	else if (src[0] == 'G' && src[1] == 'D' && src[2] == 'B' && src[3] >= '0' && src[3] <= '9' && strlen(src_) == 4)
	{
		// gdb
		return g_strdup_printf("%c%c", 0x61, (model == CALC_TI73 ? src[3] - '0' : shift(src[3] - '0')));
	}
	else if (src[0] == 'S' && src[1] == 't' && src[2] == 'r' && src[3] >= '0' && src[3] <= '9' && strlen(src_) == 4)
	{
		// strings
		return g_strdup_printf("%c%c", 0xAA, (model == CALC_TI73 ? src[3] - '0' : shift(src[3] - '0')));
	}
	else if (type == 0x1A && !strncmp((const char*) src, "Image", 5) && src[5] >= '0' && src[5] <= '9')
	{
		// images
		return g_strdup_printf("%c%c", 0x3C, shift(src[5] - '0'));
	}

	return g_strdup(src_);
}

/**
 * ticonv_varname_strdup:
 * @varname: string to be duplicated.
 *
 * This function is mostly meant for internal use: duplicating a non-tokenized string with the same allocation function ticonv_varname_tokenize(),
 * so that it can be passed to ticonv_varname_free().
 **/
TIEXPORT4 char* TICALL ticonv_varname_strdup(char * varname)
{
	if (NULL == varname)
	{
		return NULL;
	}
	return g_strdup(varname);
}

/**
 * ticonv_varname_free:
 * @varname: previously allocated varname string to be freed.
 *
 * This function frees a varname previously allocated by ticonv_varname_detokenize() or ticonv_varname_tokenize().
 **/
void TICALL ticonv_varname_free(char * varname)
{
	g_free(varname);
}
