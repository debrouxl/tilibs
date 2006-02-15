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
	by on-disk encoding. Depends on the calculator type.

	This is needed for all hand-helds.
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
	  (model == CALC_TI92) || (model == CALC_TI92P) || (model == CALC_V200));
}

static int tifiles_calc_is_ti8x(ConvModel model)
{
  return ((model == CALC_TI73) || (model == CALC_TI82) ||
	  (model == CALC_TI82) || (model == CALC_TI83) ||
	  (model == CALC_TI83P) || (model == CALC_TI84P) ||
	  (model == CALC_TI85) || (model == CALC_TI86));
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
	int is_utf8 = g_get_charset(NULL);
	const char *str;
	unsigned short *p;
	unsigned short utf16[16];	
	char *q;

	// detokenization to UTF-16
	ticonv_varname_to_utf16_s(model, src, utf16, -1);

	p = utf16;
	q = dst;
	*q = '\0';

	if(tifiles_calc_is_ti9x(model) && !is_utf8)
	{
		while(*p)
		{
			unsigned long msb = *p & 0xff00;

			if(!msb)
			{
				*q = *p & 0xff;
				*p++;
				*q++;
			}
			else
			{
				switch(*p)
				{
					case 0x03bc: str = "mu"; break;
					case 0x03b1: str = "alpha"; break;
					case 0x03b2: str = "beta"; break;
					case 0x0393: str = "GAMMA"; break;
					case 0x03b3: str = "gamma"; break;
					case 0x0394: str = "DELTA"; break;
					case 0x03b4: str = "delta"; break;
					case 0x03b5: str = "epsilon";break;
					case 0x03b6: str = "zeta"; break;
					case 0x03b8: str = "theta"; break;
					case 0x03bb: str = "lambda"; break;
					case 0x03be: str = "ksi"; break;
					case 0x03a0: str = "PI"; break;
					case 0x03c0: str = "pi"; break;
					case 0x03c1: str = "rho"; break;
					case 0x03a3: str = "SIGMA"; break; 
					case 0x03c3: str = "sigma"; break; 
					case 0x03c4: str = "tau"; break;
					case 0x03d5: str = "PHI"; break;
					case 0x03a8: str = "PSI"; break;
					case 0x03a9: str = "OMEGA"; break; 
					case 0x03c9: str = "omega"; break;
					default: break;
				}

				strcat(q, "_");
				strcat(q, str);
				strcat(q, "_");

				q += 1+strlen(str)+1;
				*p++;
			}
		}
		*q = '\0';
	}
	else if(tifiles_calc_is_ti8x(model) && !is_utf8)
	{
		while(*p)
		{
			unsigned long msb = *p & 0xff00;

			if(!msb)
			{
				*q = *p & 0xff;
				*p++;
				*q++;
			}
			else
			{
				if(*p >= 0x2080 && *p <= 0x2089)
				{
					*q++ = (*p++ - 0x2080) + '0';
				}
				else
				{
					switch(*p)
					{
						case 0x03bc: str = "mu"; break;
						case 0x03b1: str = "alpha"; break;
						case 0x03b2: str = "beta"; break;
						case 0x0393: str = "GAMMA"; break;
						case 0x03b3: str = "gamma"; break;
						case 0x0394: str = "DELTA"; break;
						case 0x03b4: str = "delta"; break;
						case 0x03b5: str = "epsilon";break;
						case 0x03b6: str = "zeta"; break;
						case 0x03b8: str = "theta"; break;
						case 0x03bb: str = "lambda"; break;
						case 0x03be: str = "ksi"; break;
						case 0x03a0: str = "PI"; break;
						case 0x03c0: str = "pi"; break;
						case 0x03c1: str = "rho"; break;
						case 0x03a3: str = "SIGMA"; break; 
						case 0x03c3: str = "sigma"; break; 
						case 0x03c4: str = "tau"; break;
						case 0x03d5: str = "PHI"; break;
						case 0x03a8: str = "PSI"; break;
						case 0x03a9: str = "OMEGA"; break; 
						case 0x03c9: str = "omega"; break;
						default: break;
					}

					strcat(q, "_");
					strcat(q, str);
					strcat(q, "_");

					q += 1+strlen(str)+1;
					*p++;
				}
			}
		}
		*q = '\0';
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
