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

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticonv_instance = 0;	// counts # of instances

/**
 * ticonv_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the handle count.
 **/
TIEXPORT int TICALL ticonv_library_init()
{
	if (ticonv_instance)
		return (++ticonv_instance);
	printf("ticonv library version %s", LIBCONV_VERSION);

  	return (++ticonv_instance);
}

/**
 * ticonv_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the handle count.
 **/
TIEXPORT int TICALL ticonv_library_exit()
{
  	return (--ticonv_instance);
}

/***********/
/* Methods */
/***********/

/**
 * ticonv_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT const char *TICALL ticonv_version_get(void)
{
	return LIBCONV_VERSION;
}

/**
 * ticonv_utf8_to_utf16:
 * @src: null terminated UTF-8 string
 *
 * UTF-8 to UTF-16 conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT unsigned short* ticonv_utf8_to_utf16(const char* str)
{
	gunichar2*  dst;
	const gchar* src = str;
	
	dst = g_utf8_to_utf16(str, -1, NULL, NULL, NULL);

	return dst;
}

/**
 * ticonv_utf16_to_utf8:
 * @src: null terminated UTF-16 string
 *
 * UTF-16 to UTF-8 conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT const char*	   ticonv_utf16_to_utf8(const unsigned short* str)
{
	const gunichar2*  src = str;
	gchar* dst;

	dst = g_utf16_to_utf8(src, -1, NULL, NULL, NULL);

	return dst;
}

/**
 * ticonv_charset_utf16_to_ti:
 * @model: calculator model
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI charset conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT char*		   TICALL ticonv_charset_utf16_to_ti(ConvModel model, const unsigned short *utf16, char *ti)
{
	/*
	switch(model)
	{
		case CALC_TI82: return ticonv_utf16_to_ti82(utf16, ti); break;
		case CALC_TI83: return ticonv_utf16_to_ti83(utf16, ti); break;
		case CALC_TI85: return ticonv_utf16_to_ti85(utf16, ti); break;
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI92:
		case CALC_TI92P:
		case CALC_V200: return ticonv_utf16_to_ti9x(utf16, ti); break;
		default: return "???";	// to do...
	}
	*/

	return g_strdup("");
}

/**
 * ticonv_charset_ti_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI charset to UTF-16 conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT unsigned short* TICALL ticonv_charset_ti_to_utf16(ConvModel model, const char *ti, unsigned short *utf16)
{
	switch(model)
	{
		case CALC_TI82: return ticonv_ti82_to_utf16(ti, utf16); break;
		case CALC_TI83: return ticonv_ti83_to_utf16(ti, utf16); break;
		case CALC_TI85: return ticonv_ti85_to_utf16(ti, utf16); break;
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI92:
		case CALC_TI92P:
		case CALC_V200: return ticonv_ti9x_to_utf16(ti, utf16); break;
		default: return 0x0000;	// to do...
	}

	return ticonv_utf8_to_utf16("");
}