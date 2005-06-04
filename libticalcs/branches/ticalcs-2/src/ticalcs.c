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
#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "calc_xx.h"

/*****************/
/* Internal data */
/*****************/

extern const CalcUpdate default_update;

static CalcFncts const *const calcs[] = 
{
	&calc_00,
#ifndef NO_TI73
	&calc_73,
#endif
#ifndef NO_TI83
	&calc_83,
#endif
#ifndef NO_TI83P2
	&calc_83p,
#endif
#ifndef NO_TI84P
	&calc_84p,
#endif
#ifndef NO_TI86
	&calc_86,
#endif
#ifndef NO_TI89
	&calc_89,
#endif
#ifndef NO_TI92
	&calc_92,
#endif
#ifndef NO_TI92P
	&calc_92p,
#endif
#ifndef NO_V200
	&calc_v2,
#endif
#ifndef NO_TI89T
	&calc_89t,
#endif
	NULL
};

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticalcs_instance = 0;	// counts # of instances

/**
 * ticalcs_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the instance count.
 **/
TIEXPORT int TICALL ticalcs_library_init(void)
{
    char locale_dir[65536];
	
#ifdef __WIN32__
  	HANDLE hDll;
  	int i;
  	
  	hDll = GetModuleHandle("ticalcs2.dll");
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

	if (ticalcs_instance)
		return (++ticalcs_instance);
	ticalcs_info(_("ticalcs library version %s"), LIBCALCS_VERSION);
  	errno = 0;

#if defined(ENABLE_NLS)
  	ticalcs_info("setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	ticalcs_info("bindtextdomain: <%s>", bindtextdomain(PACKAGE, locale_dir));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	ticalcs_info("textdomain: <%s>", textdomain(PACKAGE));
#endif

  	return (++ticalcs_instance);
}


/**
 * ticalcs_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the instance count.
 **/
TIEXPORT int
TICALL ticalcs_library_exit(void)
{
  	return (--ticalcs_instance);
}

/***********/
/* Methods */
/***********/

/**
 * ticalcs_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT const char *TICALL ticalcs_version_get(void)
{
	return LIBCALCS_VERSION;
}

/**
 * ticalcs_handle_new:
 * @model: a hand-held model
 *
 * Create a new handle associated with the given cable on the given port.
 * Must be freed with ticables_handle_del when no longer needed.
 * Note: the handle is a pointer on an opaque structure and should not be modified.
 *
 * Return value: NULL if error, an handle otherwise.
 **/
TIEXPORT CalcHandle* TICALL ticalcs_handle_new(CalcModel model)
{
	CalcHandle *handle = (CalcHandle *)calloc(1, sizeof(CalcHandle));
	int i;

	handle->model = model;

	for(i = 0; calcs[i]; i++)
		if(calcs[i]->model == model)
		{
			handle->calc = (CalcFncts *)calcs[i];
			break;
		}
	
	if(handle->calc == NULL)
		return NULL;

	handle->updat = (CalcUpdate *)&default_update;

	handle->priv2 = (uint8_t *)malloc(65536 + 4);
	if(handle->priv2 == NULL)
		return NULL;

	return handle;
}

/**
 * ticalcs_handle_del:
 * @handle: the handle
 *
 * Release the cable and free the associated resources.
 * If cable has not been detached with #ticalcs_cable_detach,
 * it will be detached.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticalcs_handle_del(CalcHandle* handle)
{
	if(handle->attached)
		ticalcs_cable_detach(handle);

    if(handle->priv2)
	free(handle->priv2);

    if(handle)
	free(handle);
    handle = NULL;

    return 0;
}

/**
 * ticalcs_handle_show:
 * @handle: the handle
 *
 * Show informations stored in the handle.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticalcs_handle_show(CalcHandle* handle)
{
	ticalcs_info(_("Link calc handle details:"));
	ticalcs_info(_("  model   : %s"), ticalcs_model_to_string(handle->model));

	return 0;
}

/**
 * ticalcs_cable_attach:
 * @handle: the handle
 * @cable: a cable to use
 *
 * Attach and open the given cable for use with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT int TICALL ticalcs_cable_attach(CalcHandle* handle, CableHandle* cable)
{
	handle->cable = cable;
	handle->attached = !0;

	TRYC(ticables_cable_open(cable));
	handle->open = !0;

	return 0;
}

/**
 * ticalcs_cable_attach:
 * @handle: the handle
 * @cable: a cable to use
 *
 * Close and detach the cable associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT int TICALL ticalcs_cable_detach(CalcHandle* handle)
{
	TRYC(ticables_cable_close(handle->cable));
	handle->open = 0;
	
	handle->attached = 0;
	handle->cable = NULL;

	return 0;
}

/**
 * ticalcs_update_set:
 * @handle: the handle
 * @update: the callbacks to use
 *
 * Set the callbacks to use for the given handle.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticalcs_update_set(CalcHandle* handle, CalcUpdate* upd)
{
	handle->updat = upd;

	return 0;
}