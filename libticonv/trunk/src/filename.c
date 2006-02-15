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
	This unit contains varname to filename conversion routines.
  
	This is used to translate some varnames into a filename supported
	by on-disk encoding.
	Depends on the calculator type.

	This is needed for the following calcs: TI9x only.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"

static int tifiles_calc_is_ti9x(ConvModel model)
{
  return ((model == CALC_TI89) || (model == CALC_TI89T) ||
	  (model == CALC_TI92) || (model == CALC_TI92P) || (model == CALC_V200) ||
	  (model == CALC_TI89T_USB));
}

static int tifiles_calc_is_ti8x(ConvModel model)
{
  return ((model == CALC_TI73) || (model == CALC_TI82) ||
	  (model == CALC_TI82) || (model == CALC_TI83) ||
	  (model == CALC_TI83P) || (model == CALC_TI84P) ||
	  (model == CALC_TI85) || (model == CALC_TI86) ||
	  (model == CALC_TI84P_USB)|| (model == CALC_TI89T_USB));
}

/**
 * ticonv_varname_to_filename_s:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (transcoded name as UTF-16).
 * @dst: a buffer to place result of transcoding (64 bytes max).
 *
 * This function converts a varname into a valid filename (depends on locale).
 * Example: 'foobar' => foobar, 'alpha' => _alpha_.
 * 
 * Greeks characters need conversion if the locale is not UTF-8 (Windows for sure, Linux
 * if locale is different of UTF-8) because greek characters are often missed or mis-converted
 * when converting to locale.
 *
 * Return value: %dst.
 **/
TIEXPORT char* TICALL ticonv_varname_to_filename_s(ConvModel model, const char *src, char *dst)
{
	int i;
	int is_utf8 = g_get_charset(NULL);
	const char *str;
	char *p;

	p = dst;
	*p = '\0';

	if(tifiles_calc_is_ti9x(model) && !is_utf8)
	{
		for(i = 0; i < (int)strlen(src);)
		{
			unsigned char schar = (unsigned char)src[i];
			unsigned int  wchar = (((unsigned char)src[i]) << 8) | ((unsigned char)src[i+1]);

			if(schar < 0x80)		// ASCII
				dst[i++] = src[i];
			else if(wchar < 0xc3c0)	// Latin-1
			{
				*p++ = src[i++];
				*p++ = src[i++];
			}
			else if(wchar >= 0xC3C0/*schar >= 0xC0*/)
			{
				switch(wchar)
				{
					case 0x0ebc: str = "mu"; break;
					case 0x0eb1: str = "alpha"; break;
					case 0x0eb2: str = "beta"; break;
					case 0x0e93: str = "GAMMA"; break;
					case 0x0eb3: str = "gamma"; break;
					case 0x0e94: str = "DELTA"; break;
					case 0x0eb4: str = "delta"; break;
					case 0x0eb5: str = "epsilon";break;
					case 0x0eb6: str = "zeta"; break;
					case 0x0eb8: str = "theta"; break;
					case 0x0ebb: str = "lambda"; break;
					case 0x0ebe: str = "ksi"; break;
					case 0x0ea0: str = "PI"; break;
					case 0x0ec0: str = "pi"; break;
					case 0x0ec1: str = "rho"; break;
					case 0x0ea3: str = "SIGMA"; break; 
					case 0x0ec3: str = "sigma"; break; 
					case 0x0ec4: str = "tau"; break;
					case 0x0ed5: str = "PHI"; break;
					case 0x0ea8: str = "PSI"; break;
					case 0x0ea9: str = "OMEGA"; break; 
					case 0x0ec9: str = "omega"; break;
					default: break;
				}

				strcat(p, "_");
				strcat(p, str);
				strcat(p, "_");

				p += 1+strlen(str)+1;
				i++; i++;
			}
		}
		*p = '\0';
	}
	else if(tifiles_calc_is_ti8x(model))
	{
		// already managed by detokenize
		strncpy(dst, src, 17);
	}
	else
		strncpy(dst, src, 17);

	return dst;
}

  
/**
 * ticonv_varname_to_filename:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 * @dst: a buffer to place result of transcoding (18 bytes max).
 *
 * This function converts a varname into a valid filename (depends on locale).
 * Example: 'foobar' => foobar, 'alpha' => _alpha_.
 * 
 * Greeks characters need conversion if the locale is not UTF-8 (Windows for sure, Linux
 * if locale is different of UTF-8) because greek characters are often missed or mis-converted
 * when converting to locale.
 *
 * Return value: %dst as a newly allocated string.
 **/ 
TIEXPORT char* TICALL ticonv_varname_to_filename(ConvModel model, const char *src)
{
	static char dst[256];
	return g_strdup(ticonv_varname_to_filename_s(model, src, dst));
}
