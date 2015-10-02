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
  This unit contains the interface of the libticonv library.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"
#include "charset.h"

/***********/
/* Methods */
/***********/

/**
 * ticonv_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a constant string.
 **/
TIEXPORT4 const char *TICALL ticonv_version_get(void)
{
	return LIBCONV_VERSION;
}

/**
 * ticonv_utf16_strlen:
 * @str: null terminated UTF-16 string
 *
 * UTF-16 version of strlen (same as wcslen if wchar_t is UTF-16, but portable).
 *
 * Return value: number of characters. Surrogate pairs are counted as 2 characters each.
 **/
TIEXPORT4 size_t TICALL ticonv_utf16_strlen(const unsigned short *str)
{
	size_t l = 0;
	if (str != NULL)
	{
		const unsigned short *p = str;
		while (*(p++))
		{
			l++;
		}
	}
	else
	{
		g_critical("%s(NULL)", __FUNCTION__);
	}
	return l;
}

/**
 * ticonv_utf8_to_utf16:
 * @src: null terminated UTF-8 string
 *
 * UTF-8 to UTF-16 conversion.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT4 unsigned short* ticonv_utf8_to_utf16(const char *str)
{
	gunichar2* dst;
	const gchar* src = str;
	GError *error = NULL;

	if (str == NULL)
	{
		g_critical("%s: str is NULL", __FUNCTION__);
		return NULL;
	}

	dst = g_utf8_to_utf16(src, -1, NULL, NULL, &error);
	if (error)
	{
		g_critical("%s", error->message);
	}

	return dst;
}

/**
 * ticonv_utf16_free:
 * @str: previously allocated UTF-16 string to be freed.
 *
 * This function frees an UTF-16 string previously allocated by e.g. ticonv_utf8_to_utf16().
 **/
TIEXPORT4 void TICALL ticonv_utf16_free(unsigned short *str)
{
	g_free((void *)str);
}

/**
 * ticonv_utf16_to_utf8:
 * @src: null terminated UTF-16 string
 *
 * UTF-16 to UTF-8 conversion.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* ticonv_utf16_to_utf8(const unsigned short *str)
{
	const gunichar2* src = str;
	gchar* dst;
	GError *error = NULL;

	if (str == NULL)
	{
		g_critical("%s: str is NULL", __FUNCTION__);
		return NULL;
	}

	dst = g_utf16_to_utf8(src, -1, NULL, NULL, &error);
	if (error)
	{
		g_critical("%s", error->message);
	}

	return dst;
}

/**
 * ticonv_utf8_free:
 * @str: previously allocated UTF-8 string to be freed.
 *
 * This function frees an UTF-8 string previously allocated by e.g. ticonv_utf16_to_utf8().
 **/
TIEXPORT4 void TICALL ticonv_utf8_free(char *str)
{
	g_free((void *)str);
}

/**
 * ticonv_charset_utf16_to_ti_s:
 * @model: calculator model
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI charset conversion.
 *
 * Return value: the %ti string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_charset_utf16_to_ti_s(CalcModel model, const unsigned short *utf16, char *ti)
{
	if (utf16 != NULL && ti != NULL)
	{
		switch (model)
		{
			case CALC_TI73: return ticonv_utf16_to_ti73(utf16, ti); break;
			case CALC_TI80: return ticonv_utf16_to_ti80(utf16, ti); break;
			case CALC_TI82: return ticonv_utf16_to_ti82(utf16, ti); break;
			case CALC_TI83: return ticonv_utf16_to_ti83(utf16, ti); break;
			case CALC_TI83P:
			case CALC_TI84P:
			case CALC_TI84PC: return ticonv_utf16_to_ti83p(utf16, ti); break;
			case CALC_TI85: return ticonv_utf16_to_ti85(utf16, ti); break;
			case CALC_TI86: return ticonv_utf16_to_ti86(utf16, ti); break;
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI92:
			case CALC_TI92P:
			case CALC_V200: return ticonv_utf16_to_ti9x(utf16, ti); break;
			case CALC_TI84P_USB:
			case CALC_TI84PC_USB:
			case CALC_TI83PCE_USB:
			case CALC_TI84PCE_USB:
			case CALC_TI82A_USB: return ticonv_utf16_to_ti84pusb(utf16, ti); break;
			case CALC_TI89T_USB: return ticonv_utf16_to_ti89tusb(utf16, ti); break;
			case CALC_NSPIRE:
			{
				char *tmp = ticonv_utf16_to_utf8(utf16);
				strcpy(ti, tmp);
				ticonv_utf8_free(tmp);
				return ti;
			}
			break;
			default:
			{
				ti[0] = 0;
				return ti;
			}
			break;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_charset_utf16_to_ti:
 * @model: calculator model
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI charset conversion.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_charset_utf16_to_ti(CalcModel model, const unsigned short *utf16)
{
	if (utf16 != NULL)
	{
		char * ti = g_malloc0(4*ticonv_utf16_strlen(utf16) + 1);	// upper bound
		char * out = ticonv_charset_utf16_to_ti_s(model, utf16, ti);
		if (out != NULL)
		{
			return out;
		}
		else
		{
			g_free(ti);
			return NULL;
		}
	}
	else
	{
		g_critical("%s: utf16 is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_ti_free:
 * @str: previously allocated TI string to be freed.
 *
 * This function frees a string in TI format previously allocated by e.g. ticonv_charset_utf16_to_ti().
 **/
TIEXPORT4 void TICALL ticonv_ti_free(char *str)
{
	g_free((void *)str);
}

/**
 * ticonv_charset_ti_to_utf16_s:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI charset to UTF-16 conversion.
 * The %utf16 destination buffer will hold the result. It must be big enough.
 *
 * Return value: the %utf16 string or NULL if error.
 **/
TIEXPORT4 unsigned short* TICALL ticonv_charset_ti_to_utf16_s(CalcModel model, const char *ti, unsigned short *utf16)
{
	if (ti != NULL && utf16 != NULL)
	{
		switch (model)
		{
			case CALC_TI73: return ticonv_ti73_to_utf16(ti, utf16); break;
			case CALC_TI80: return ticonv_ti80_to_utf16(ti, utf16); break;
			case CALC_TI82: return ticonv_ti82_to_utf16(ti, utf16); break;
			case CALC_TI83: return ticonv_ti83_to_utf16(ti, utf16); break;
			case CALC_TI83P:
			case CALC_TI84P:
			case CALC_TI84PC: return ticonv_ti83p_to_utf16(ti, utf16); break;
			case CALC_TI85: return ticonv_ti85_to_utf16(ti, utf16); break;
			case CALC_TI86: return ticonv_ti86_to_utf16(ti, utf16); break;
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI92:
			case CALC_TI92P:
			case CALC_V200: return ticonv_ti9x_to_utf16(ti, utf16); break;
			case CALC_TI84P_USB:
			case CALC_TI84PC_USB:
			case CALC_TI83PCE_USB:
			case CALC_TI84PCE_USB:
			case CALC_TI82A_USB: return ticonv_ti84pusb_to_utf16(ti, utf16); break;
			case CALC_TI89T_USB: return ticonv_ti89tusb_to_utf16(ti, utf16); break;
			case CALC_NSPIRE:
			{
				unsigned short *tmp = ticonv_utf8_to_utf16(ti);
				memcpy(utf16, tmp, 2*ticonv_utf16_strlen(tmp));
				ticonv_utf16_free(tmp);
				return utf16;
			}
			break;
			default: utf16[0] = 0; return utf16;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_charset_ti_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI charset to UTF-16 conversion.
 *
 * Return value: a newly allocated string or NULL if error.
 **/
TIEXPORT4 unsigned short* TICALL ticonv_charset_ti_to_utf16(CalcModel model, const char *ti)
{
	if (ti != NULL)
	{
		unsigned short * utf16 = g_malloc0(4 * strlen(ti) + 2);	// upper bound
		unsigned short * out = ticonv_charset_ti_to_utf16_s(model, ti, utf16);
		if (out != NULL)
		{
			return out;
		}
		else
		{
			g_free(utf16);
			return NULL;
		}

	}
	else
	{
		g_critical("%s: ti is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_utf16_s:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate (17 chars max).
 * @dst: a buffer where to placed the result (big enough).
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 *
 * Static version.
 *
 * Return value: the %dst string or NULL if error.
 **/
TIEXPORT4 unsigned short* TICALL ticonv_varname_to_utf16_s(CalcModel model, const char *src, unsigned short *dst, unsigned char type)
{
	if (src != NULL && dst != NULL)
	{
		char * tmp = ticonv_varname_detokenize(model, src, type);
		unsigned short * utf16 = ticonv_charset_ti_to_utf16_s(model, tmp, dst);
		g_free(tmp);
		return utf16;
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_utf16:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate (17 chars max).
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
TIEXPORT4 unsigned short* TICALL ticonv_varname_to_utf16(CalcModel model, const char *src, unsigned char type)
{
	if (src != NULL)
	{
		char * tmp = ticonv_varname_detokenize(model, src, type);
		unsigned short * utf16 = ticonv_charset_ti_to_utf16(model, tmp);
		g_free(tmp);
		return utf16;
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
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
TIEXPORT4 char* TICALL ticonv_varname_to_utf8(CalcModel model, const char *src, unsigned char type)
{
	if (src != NULL)
	{
		unsigned short * utf16 = ticonv_varname_to_utf16(model, src, type);
		gchar * utf8 = ticonv_utf16_to_utf8(utf16);
		g_free(utf16);
		return utf8;
	}
	else
	{
		g_critical("%s: src is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_utf8_s:
 * @model: a calculator model.
 * @src: a name of variable to detokenize and translate (17 chars max).
 * @dst: a buffer where to placed the result (big enough).
 * @vartype: the type of variable.
 *
 * Some calculators (like TI73/82/83/83+/84+) does not return the real name of the 
 * variable (like L1) but uses a special encoded way. This functions expands the name 
 * and converts it to UTF-16.
 *
 * Static version.
 *
 * Return value: the %dst string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_to_utf8_s(CalcModel model, const char *src, char *dst, unsigned char type)
{
	if (src != NULL && dst != NULL)
	{
		char * tmp = ticonv_varname_to_utf8(model, src, type);
		if (tmp != NULL)
		{
			strcpy(dst, tmp);
			g_free(tmp);
			return dst;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_filename:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 *
 * This function converts a varname into a valid filename (depends on locale).
 * Example: 'foobar' => foobar, 'alpha' => _alpha_/alpha.
 * 
 * Greek characters need conversion if the locale is not UTF-8 (Windows for sure, Linux
 * if locale is different of UTF-8) because greek characters are often missed or mis-converted
 * when converting to locale.
 *
 * Return value: %dst as a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_to_filename(CalcModel model, const char *src, unsigned char type)
{
	if (src != NULL)
	{
		unsigned short * utf16 = ticonv_varname_to_utf16(model, src, type);
		char * gfe = ticonv_utf16_to_gfe(model, utf16);
		g_free(utf16);
		return gfe;
	}
	else
	{
		g_critical("%s: src is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_filename_s:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 * @dst: a buffer to place result in the GLib filename encoding (64 bytes max).
 *
 * This function converts a varname into a valid filename (depends on locale).
 * Example: 'foobar' => foobar, 'alpha' => _alpha_/alpha.
 *
 * Greek characters need conversion if the locale is not UTF-8 (Windows for sure, Linux
 * if locale is different of UTF-8) because greek characters are often missed or mis-converted
 * when converting to locale.
 *
 * Return value: %dst or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_to_filename_s(CalcModel model, const char *src, char *dst, unsigned char type)
{
	if (src != NULL && dst != NULL)
	{
		char * tmp = ticonv_varname_to_filename(model, src, type);
		if (tmp != NULL)
		{
			strcpy(dst, tmp);
			g_free(tmp);
			return dst;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_to_tifile:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 *
 * This function converts a raw varname into a TI file varname.
 * Needed because USB hand-helds use TI-UTF-8 while TI files are still encoded in
 * raw varname encoding.
 *
 * Return value: %dst as a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_to_tifile(CalcModel model, const char *src, unsigned char type)
{
	unsigned short *utf16;
	char *ti;
	char *dst;

	if (src == NULL)
	{
		g_critical("%s: src is NULL", __FUNCTION__);
		return NULL;
	}

	// Do TI-UTF-8 -> UTF-16,UTF-16 -> TI-8x/9x charset
	if (model == CALC_TI84P_USB || model == CALC_TI84PC_USB || model == CALC_TI83PCE_USB || model == CALC_TI84PCE_USB || model == CALC_TI82A_USB)
	{
		utf16 = ticonv_charset_ti_to_utf16(CALC_TI84P_USB, src);

		ti = ticonv_charset_utf16_to_ti(CALC_TI84P, utf16);
		ticonv_utf16_free(utf16);

		dst = ticonv_varname_tokenize(CALC_TI84P, ti, type);
		ticonv_ti_free(ti);
	}
	else if (model == CALC_TI89T_USB)
	{
		utf16 = ticonv_charset_ti_to_utf16(CALC_TI89T_USB, src);

		ti = ticonv_charset_utf16_to_ti(CALC_TI89T, utf16);
		ticonv_utf16_free(utf16);

		dst = ti;
	}
	else
	{
		dst = g_strdup(src);
	}

	return dst;
}

/**
 * ticonv_varname_to_tifile_s:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 * @dst: the location where to place the result (big enough).
 *
 * This function converts a raw varname into a TI file varname.
 * Needed because USB hand-helds use TI-UTF-8 while TI files are still encoded in
 * raw varname encoding.
 *
 * Return value: %dst or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_to_tifile_s(CalcModel model, const char *src, char *dst, unsigned char type)
{
	if (src != NULL && dst != NULL)
	{
		char * tmp = ticonv_varname_to_tifile(model, src, type);
		if (tmp)
		{
			strcpy(dst, tmp);
			g_free(tmp);
			return dst;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_varname_from_tifile:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 *
 * This function converts a raw varname into a TI file varname.
 * Needed because USB hand-helds use TI-UTF-8 while TI files are still encoded in
 * raw varname encoding.
 *
 * Return value: %dst as a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_from_tifile(CalcModel model, const char *src, unsigned char type)
{
	unsigned short *utf16;
	char *ti;
	char *dst;

	if (src == NULL)
	{
		g_critical("%s: src is NULL", __FUNCTION__);
		return NULL;
	}

	if (model == CALC_TI84P_USB || model == CALC_TI84PC_USB || model == CALC_TI83PCE_USB || model == CALC_TI84PCE_USB || model == CALC_TI82A_USB)
	{
		ti = ticonv_varname_detokenize(CALC_TI84P, src, type);

		utf16 = ticonv_charset_ti_to_utf16(CALC_TI84P, ti);
		g_free(ti);

		dst = ticonv_charset_utf16_to_ti(CALC_TI84P_USB, utf16);
		g_free(utf16);
	}
	else if (model == CALC_TI89T_USB)
	{
		utf16 = ticonv_charset_ti_to_utf16(CALC_TI89T, src);

		dst = ticonv_charset_utf16_to_ti(CALC_TI89T_USB, utf16);
		g_free(utf16);
	}
	else
	{
		dst = g_strdup(src);
	}

	return dst;
}

/**
 * ticonv_varname_from_tifile_s:
 * @model: a calculator model taken in #CalcModel.
 * @src: the name of variable to convert (raw/binary name).
 * @dst: the location where to place the result (big enough).
 *
 * This function converts a raw varname into a TI file varname.
 * Needed because USB hand-helds use TI-UTF-8 while TI files are still encoded in
 * raw varname encoding.
 *
 * Return value: %dst as a newly allocated string or NULL if error.
 **/
TIEXPORT4 char* TICALL ticonv_varname_from_tifile_s(CalcModel model, const char *src, char *dst, unsigned char type)
{
	if (src != NULL && dst != NULL)
	{
		char * tmp = ticonv_varname_from_tifile(model, src, type);
		if (tmp != NULL)
		{
			strcpy(dst, tmp);
			g_free(tmp);
			return dst;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		g_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}
}

/**
 * ticonv_model_uses_utf8:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model uses UTF-8.
 *
 * Return value: nonzero if the calculator uses UTF-8, zero if it doesn't.
 */
TIEXPORT4 int TICALL ticonv_model_uses_utf8(CalcModel model)
{
	// In 2015, the blacklist condition about twice longer than the equivalent whitelist condition,
	// but less likely to get future models (most of which should have exclusively direct USB connectivity) wrong.
	return (   model != CALC_NONE
	        && model != CALC_TI73
	        && model != CALC_TI82
	        && model != CALC_TI83
	        && model != CALC_TI83P
	        && model != CALC_TI84P
	        && model != CALC_TI85
	        && model != CALC_TI86
	        && model != CALC_TI89
	        && model != CALC_TI89T
	        && model != CALC_TI92
	        && model != CALC_TI92P
	        && model != CALC_V200
	        && model != CALC_TI80
	        && model != CALC_TI84PC);
}
