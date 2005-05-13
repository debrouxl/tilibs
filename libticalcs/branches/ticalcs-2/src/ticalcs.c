/* Hey EMACS -*- linux-c -*- */
/* $Id: ticables.c 1045 2005-05-13 06:26:21Z roms $ */

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

#include "gettext.h"
#include "ticables.h"
#include "logging.h"
#include "error.h"
#include "link_xxx.h"
#include "data_log.h"

/*****************/
/* Internal data */
/*****************/

static TiCable const *const cables[] = 
{
	&cable_nul,
#ifndef NO_CABLE_GRY
	&cable_gry,
#endif
#ifndef NO_CABLE_BLK
	&cable_ser,
#endif
#ifndef NO_CABLE_PAR
	&cable_par,
#endif
#if !defined(NO_CABLE_SLV) && defined(HAVE_LIBSUB)
	&cable_slv,
#endif
#if !defined(NO_CABLE_SLV) && defined(HAVE_LIBSUB)
	&cable_raw,
#endif
#ifndef NO_CABLE_VTI
	&cable_vti,
#endif
#ifndef NO_CABLE_TIE
	&cable_tie,
#endif
	NULL
};

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticables_instance = 0;	// counts # of instances

/**
 * tifiles_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the instance count.
 **/
TIEXPORT int TICALL ticables_library_init(void)
{
    char locale_dir[65536];
	
#ifdef __WIN32__
  	HANDLE hDll;
  	int i;
  	
  	hDll = GetModuleHandle("ticables.dll");
  	GetModuleFileName(hDll, locale_dir, 65535);
  	
  	for (i = strlen(locale_dir); i >= 0; i--) {
    	if (locale_dir[i] == '\\')
      		break;
  	}
  	
  	locale_dir[i] = '\0';
  	strcat(locale_dir, "\\locale");
#else
	strcpy(locale_dir, LOCALEDIR);
#endif

	if (ticables_instance)
		return (++ticables_instance);
	ticables_info(_("ticables library version %s"), LIBTICABLES_VERSION);
  	errno = 0;

#if defined(ENABLE_NLS)
  	ticables_info("setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	ticables_info("bindtextdomain: <%s>", bindtextdomain(PACKAGE, locale_dir));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	ticables_info("textdomain: <%s>", textdomain(PACKAGE));
#endif

  	return (++ticables_instance);
}


/**
 * tifiles_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the instance count.
 **/
TIEXPORT int
TICALL ticables_library_exit(void)
{
  	return (--ticables_instance);
}

/***********/
/* Methods */
/***********/

/**
 * ticables_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT const char *TICALL ticables_version_get(void)
{
	return LIBTICABLES_VERSION;
}

/**
 * ticables_handle_new:
 * @model: a cable model
 * @port: the generic port on which cable is attached.
 *
 * Create a new handle associated with the given cable on the given port.
 * Must be freed with ticables_handle_del when no longer needed.
 * Note: the handle is a pointer on an opaque structure and should not be modified.
 *
 * Return value: NULL if error, an handle otherwise.
 **/
TIEXPORT CableHandle* TICALL ticables_handle_new(TiCableModel model, TiCablePort port)
{
	CableHandle *handle = (CableHandle *)calloc(1, sizeof(CableHandle));
	int i;

	handle->model = model;
	handle->port = port;

	handle->delay = DFLT_DELAY;
	handle->timeout = DFLT_TIMEOUT;

	for(i = 0; cables[i]; i++)
		if(cables[i]->model == model)
		{
			handle->cable = cables[i];
			break;
		}
	
	if(handle->cable == NULL)
		return NULL;

	return handle;
}

/**
 * ticables_handle_del:
 * @handle: the handle
 *
 * Release the cable and free the associated resources.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticables_handle_del(CableHandle* handle)
{
    if(handle->priv2)
	free(handle->priv2);
    if(handle->device)
	free(handle->device);
    if(handle)
	free(handle);
    handle = NULL;
    
    return 0;
}

/**
 * ticables_options_set_timeout:
 * @handle: the handle
 * @timeout: timeout value in tenth of seconds
 *
 * Set timeout for any cable.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticables_options_set_timeout(CableHandle* handle, int timeout)
{
	return handle->timeout = timeout;
}
	
/**
 * ticables_options_set_delay:
 * @handle: the handle
 * @delay: delay in micro-seconds
 *
 * Set inter-bit delay for parallel or BlackLink cable.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticables_options_set_delay(CableHandle* handle, int delay)
{
	return handle->delay = delay;
}

/**
 * ticables_get_model:
 * @handle: the handle
 *
 * Retrieve link cable model.
 *
 * Return value: a #TiCableModel value.
 **/
TIEXPORT TiCableModel TICALL ticables_get_model(CableHandle* handle)
{
	return handle->model;
}

/**
 * ticables_get_port:
 * @handle: the handle
 *
 * Retrieve link port.
 *
 * Return value: a #TiCablePort value.
 **/
TIEXPORT TiCablePort  TICALL ticables_get_port(CableHandle* handle)
{
	return handle->port;
}

TIEXPORT int TICALL ticables_handle_show(CableHandle* handle)
{
	ticables_info(_("Link cable handle details:"));
	ticables_info(_("  model   : %s"), ticables_model_to_string(handle->model));
	ticables_info(_("  port    : %s"), ticables_port_to_string(handle->port));
	ticables_info(_("  timeout : %2.1fs"), (float)handle->timeout / 10);
	ticables_info(_("  delay   : %i us"), handle->delay);
	if(handle->device)
	{
		ticables_info(_("  device  : %s"), handle->device);
		ticables_info(_("  address : 0x%03x"), handle->address);
	}

	return 0;
}
