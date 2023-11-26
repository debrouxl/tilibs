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
	This unit contains UTF-16 to GFE conversion routines.
  
	This is used to translate some varnames into a filename supported
	by on-disk encoding (GLib filename in fact). 
	Depends on the calculator model.

	This is needed for all hand-helds.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"

/**
 * ticonv_utf16_to_gfe:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert from UTF-16
 *
 * This function converts a varname into a valid filename (depends on locale).
 * Example: 'foobar' => foobar, 'alpha' => _alpha_.
 * 
 * Greeks characters need conversion if the locale is not UTF-8 (Windows for sure, Linux
 * if locale is different of UTF-8) because greek characters are often missed or mis-converted
 * when converting to locale.
 *
 * Return value: %dst as a newly allocated string or NULL if error.
 **/
char* TICALL ticonv_utf16_to_gfe(CalcModel model, const unsigned short *src)
{
	const int is_utf8 = ticonv_environment_is_utf8();
	const char *str;
	unsigned short *utf16_src;
	unsigned short *utf16_dst;
	char *dst;

	if (src == nullptr)
	{
		return nullptr;
	}

	// detokenization to UTF-16
	unsigned short* p = utf16_src = (unsigned short*)src;
	unsigned short* q = utf16_dst = (unsigned short*)g_malloc0(18 * ticonv_utf16_strlen(utf16_src) + 2);

	// conversion from UTF-16 to UTF-16
	if (ticonv_model_is_ti68k(model) && !is_utf8)
	{
		while (*p)
		{
			const unsigned long msb = *p & 0xff00;

			if (!msb)
			{
				*q++ = *p++ & 0xff;
			}
			else
			{
				glong ir, iw;

				switch (*p)
				{
					case 0x03bc: str = "_mu_"; break;
					case 0x03b1: str = "_alpha_"; break;
					case 0x03b2: str = "_beta_"; break;
					case 0x0393: str = "_GAMMA_"; break;
					case 0x03b3: str = "_gamma_"; break;
					case 0x0394: str = "_DELTA_"; break;
					case 0x03b4: str = "_delta_"; break;
					case 0x03b5: str = "_epsilon_";break;
					case 0x03b6: str = "_zeta_"; break;
					case 0x03b8: str = "_theta_"; break;
					case 0x03bb: str = "_lambda_"; break;
					case 0x03be: str = "_ksi_"; break;
					case 0x03a0: str = "_PI_"; break;
					case 0x03c0: str = "_pi_"; break;
					case 0x03c1: str = "_rho_"; break;
					case 0x03a3: str = "_SIGMA_"; break; 
					case 0x03c3: str = "_sigma_"; break; 
					case 0x03c4: str = "_tau_"; break;
					case 0x03d5: str = "_PHI_"; break;
					case 0x03a8: str = "_PSI_"; break;
					case 0x03a9: str = "_OMEGA_"; break; 
					case 0x03c9: str = "_omega_"; break;
					default: str = ""; break;
				}

				gunichar2* str2 = g_utf8_to_utf16(str, -1, &ir, &iw, nullptr);
				memcpy(q, str2, (iw+1) * sizeof(unsigned short));
				g_free(str2);

				q += iw;
				p++;
			}
		}
		*q = '\0';
	}
	else if ((ticonv_model_is_tiz80(model) || ticonv_model_is_tiez80(model)) && !is_utf8)
	{
		while (*p)
		{
			const unsigned long msb = *p & 0xff00;

			if (!msb)
			{
				*q++ = *p++ & 0xff;
			}
			else
			{
				if (*p >= 0x2080 && *p <= 0x2089)
				{
					*q++ = (*p++ - 0x2080) + '0';
				}
				else
				{
					glong ir, iw;

					switch(*p)
					{
						case 0x03bc: str = "_mu_"; break;
						case 0x03b1: str = "_alpha_"; break;
						case 0x03b2: str = "_beta_"; break;
						case 0x0393: str = "_GAMMA_"; break;
						case 0x03b3: str = "_gamma_"; break;
						case 0x0394: str = "_DELTA_"; break;
						case 0x03b4: str = "_delta_"; break;
						case 0x03b5: str = "_epsilon_";break;
						case 0x03b6: str = "_zeta_"; break;
						case 0x03b8: str = "_theta_"; break;
						case 0x03bb: str = "_lambda_"; break;
						case 0x03be: str = "_ksi_"; break;
						case 0x03a0: str = "_PI_"; break;
						case 0x03c0: str = "_pi_"; break;
						case 0x03c1: str = "_rho_"; break;
						case 0x03a3: str = "_SIGMA_"; break;
						case 0x03c3: str = "_sigma_"; break;
						case 0x03c4: str = "_tau_"; break;
						case 0x03d5: str = "_PHI_"; break;
						case 0x03a8: str = "_PSI_"; break;
						case 0x03a9: str = "_OMEGA_"; break;
						case 0x03c9: str = "_omega_"; break;
						default: str = ""; break;
					}

					gunichar2* str2 = g_utf8_to_utf16(str, -1, &ir, &iw, nullptr);
					memcpy(q, str2, (iw+1) * sizeof(gunichar2));
					g_free(str2);

					q += iw;
					p++;
				}
			}
		}
		*q = '\0';
	}
	else
	{
		while (*p)
		{
#ifdef __WIN32__
			if(*p >= 0x2080 && *p <= 0x2089)
			{
				*q++ = (*p++ - 0x2080) + '0';
			}
			else
#endif
			{
				*q++ = *p++;
			}
		}
		*q = '\0';
	}

	// '/' is not allowed in filenames
	for (q = utf16_dst; *q; q++)
	{
		if (*q == '/')
		{
			*q = '_';
		}
	}

	// UTF-16 to UTF-8 to GFE encoding
	{
		gchar* utf8 = g_utf16_to_utf8(utf16_dst, -1, nullptr, nullptr, nullptr);
		g_free(utf16_dst);

		dst = g_filename_from_utf8(utf8, -1, nullptr, nullptr, nullptr);
		g_free(utf8);
	}

	return dst;
}

/**
 * ticonv_gfe_free:
 * @src: previously allocated gfe string to be freed.
 *
 * This function frees a gfe previously allocated by ticonv_utf16_to_gfe().
 **/
void TICALL ticonv_gfe_free(char *src)
{
	g_free(src);
}

/**
 * ticonv_gfe_to_zfe:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert from GLib filename encoding
 *
 * This function converts a locale dependent filename into a 
 * valid ZIP filename.
 * Example: 'foobar' => foobar, 'alpha' => _alpha_.
 *
 * Return value: %dst as a newly allocated string.
 **/
char* TICALL ticonv_gfe_to_zfe(CalcModel model, const char *src_)
{
	char *src;
	char *dst;

	if (src_ == nullptr)
	{
		return g_strdup("");
	}

	// This conversion is needed and works only if the filename charset
	// is UTF-8. Otherwise, the equivalent conversion is done in
	// ticonv_utf16_to_gfe.
	if (!ticonv_environment_has_utf8_filenames()) return g_strdup(src_);

	const char* p = src = (char*)src_;
	char* q = dst = (char*)g_malloc0(18 * strlen(src) + 1);

	while(*p)
	{
		if((*p & 0xFF) == 0xCE)
		{
			const char *str;

			p++;
			switch(*p & 0xff)
			{
				case 0xbc: str = "_mu_"; break;
				case 0xb1: str = "_alpha_"; break;
				case 0xb2: str = "_beta_"; break;
				case 0x93: str = "_GAMMA_"; break;
				case 0xb3: str = "_gamma_"; break;
				case 0x94: str = "_DELTA_"; break;
				case 0xb4: str = "_delta_"; break;
				case 0xb5: str = "_epsilon_";break;
				case 0xb6: str = "_zeta_"; break;
				case 0xb8: str = "_theta_"; break;
				case 0xbb: str = "_lambda_"; break;
				case 0xbe: str = "_ksi_"; break;
				case 0xa0: str = "_PI_"; break;
				case 0xc0: str = "_pi_"; break;
				case 0xc1: str = "_rho_"; break;
				case 0xa3: str = "_SIGMA_"; break; 
				case 0xc3: str = "_sigma_"; break; 
				case 0xc4: str = "_tau_"; break;
				case 0xd5: str = "_PHI_"; break;
				case 0xa8: str = "_PSI_"; break;
				case 0xa9: str = "_OMEGA_"; break; 
				case 0xc9: str = "_omega_"; break;
				default: str = ""; break;
			}

			memcpy(q, str, strlen(str) + 1);

			q += strlen(str);
			p++;
		}
		else
		{
			*q++ = *p++;
		}
	}
	*q = '\0';

	return dst;
}

/**
 * ticonv_gfe_free:
 * @src: previously allocated zfe string to be freed.
 *
 * This function frees a zfe previously allocated by ticonv_gfe_to_zfe().
 **/
void TICALL ticonv_zfe_free(char *src)
{
	g_free(src);
}

/**
 * ticonv_environment_is_utf8:
 *
 * This function returns whether the current locale uses an UTF-8 charset.
 *
 * Return value: TRUE when the current locale uses an UTF-8 charset, FALSE otherwise.
 **/
int TICALL ticonv_environment_is_utf8(void)
{
#ifdef __WIN32__
	return G_WIN32_HAVE_WIDECHAR_API();
#else
	return g_get_charset(nullptr);
#endif
}

/**
 * ticonv_environment_has_utf8_filenames:
 *
 * This function returns whether the filename charset is UTF-8.
 *
 * Return value: TRUE when the filename charset is UTF-8, FALSE otherwise.
 **/
int TICALL ticonv_environment_has_utf8_filenames(void)
{
	return g_get_filename_charsets(nullptr);
}
