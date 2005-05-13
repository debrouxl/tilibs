/* Hey EMACS -*- linux-c -*- */
/* $Id: ticables.c 1038 2005-05-09 12:05:08Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(__WIN32__)
#include <windows.h>
#endif

//#include "vbapi.h"

#include "gettext.h"
#include "ticables.h"
#include "logging.h"
#include "error.h"
#include "link_xxx.h"
#include "data_log.h"

#if 0// defined(__WIN32__)

TiCblHandle* handle_list[MAX_DESCRIPTORS] = { 0 };

#define CD_DEREF(cd)	(handle_list[cd])

static int find_free_spot(void)
{
	TiCblHandle **p;
	int i;

	for(p = handle_list, i = 0; *p == NULL && i < MAX_DESCRIPTORS; p++, i++);

	if(i == MAX_DESCRIPTORS)
		return -1;

	return i;
}


//http://support.microsoft.com/default.aspx?scid=kb%3Ben-us%3B118643
/**
 * ticables_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
HLSTR __stdcall ticables_vb_version_get(void)
{
	return LIBTICABLES_VERSION;
}

/**
 * ticables_descriptor_new:
 * @model: a cable model
 * @port: the generic port on which cable is attached.
 *
 * Create a new descriptor associated with the given cable on the given port.
 * Must be freed with ticables_descriptor_del when no longer needed.
 * Note: the cable descriptor is like a file descriptor: this is an access to your cable.
 *		 This function exists for bindings which does not support function pointers (like VB).
 *		 Prefer #ticables_handle_new rather than this one.
 *
 * Return value: -1 if error, a valid descriptor otherwise.
 **/
int __stdcall ticables_vb_handle_new(TiCableModel model, TiCablePort port)
{
	TiCblHandle *handle = ticables_handle_new(model, port);
	int cd;

	cd = find_free_spot();
	if(cd == -1)
		return -1;

	CD_DEREF(cd) = handle;

	return 0;
}

/**
 * ticables_descriptor_del:
 * @cd: a cable descriptor
 *
 * Release the cable and free the associated resources.
 *
 * Return value: -1 if descriptor is invalid, 0 otherwise.
 **/
int __stdcall ticables_vb_handle_del(int cd)
{
	if(cd < 0 || cd >= MAX_DESCRIPTORS)
		return -1;
	
	ticables_handle_del(CD_DEREF(cd));
    
    return 0;
}

#endif