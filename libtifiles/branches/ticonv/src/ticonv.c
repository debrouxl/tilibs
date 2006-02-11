/* Hey EMACS -*- linux-c -*- */
/* $Id: ticonv.c 1721 2006-01-21 13:58:14Z roms $ */

/*  libticonv - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
#ifdef __WIN32__
#include <windows.h>
#endif

#include "ticonv.h"

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
