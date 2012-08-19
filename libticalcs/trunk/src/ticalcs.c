/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#include <locale.h>

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
#ifndef NO_TI82
	&calc_82,
#endif
#ifndef NO_TI82S
	&calc_83/*&calc_82s*/,
#endif
#ifndef NO_TI83
	&calc_83,
#endif
#ifndef NO_TI83P
	&calc_83p,
#endif
#ifndef NO_TI84P
	&calc_84p,
#endif
#ifndef NO_TI85
	&calc_85,
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
#ifndef NO_TI84P_USB
	&calc_84p_usb,
#endif
#ifndef NO_TI89T_USB
	&calc_89t_usb,
#endif
#ifndef NO_SPIRE
	&calc_nsp,
#endif
#ifndef NO_TI80
	&calc_80,
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
TIEXPORT3 int TICALL ticalcs_library_init(void)
{
	char locale_dir[65536];

#ifdef __WIN32__
	HANDLE hDll;
	int i;

	hDll = GetModuleHandle("libticalcs2-11.dll");
	GetModuleFileName(hDll, locale_dir, 65535);

	for (i = strlen(locale_dir); i >= 0; i--)
	{
		if (locale_dir[i] == '\\')
		{
			break;
		}
	}
	locale_dir[i] = '\0';

#ifdef __MINGW32__
	strcat(locale_dir, "\\..\\share\\locale");
#else
	strcat(locale_dir, "\\locale");
#endif
#else
	strcpy(locale_dir, LOCALEDIR);
#endif

	if (ticalcs_instance)
		return (++ticalcs_instance);
	ticalcs_info(_("ticalcs library version %s"), LIBCALCS_VERSION);
	errno = 0;

#if defined(ENABLE_NLS)
	ticalcs_info("setlocale: %s", setlocale(LC_ALL, ""));
	ticalcs_info("bindtextdomain: %s", bindtextdomain(PACKAGE, locale_dir));
	bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
	ticalcs_info("textdomain: %s", textdomain(NULL));
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
TIEXPORT3 int
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
TIEXPORT3 const char *TICALL ticalcs_version_get(void)
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
TIEXPORT3 CalcHandle* TICALL ticalcs_handle_new(CalcModel model)
{
	CalcHandle *handle = (CalcHandle *)g_malloc0(sizeof(CalcHandle));
	if (handle != NULL)
	{
		int i;

		handle->model = model;

		for(i = 0; calcs[i]; i++)
			if(calcs[i]->model == (int const)model)
			{
				handle->calc = (CalcFncts *)calcs[i];
				break;
			}

		if(handle->calc == NULL)
		{
			g_free(handle);
			return NULL;
		}

		handle->updat = (CalcUpdate *)&default_update;

		handle->priv2 = (uint8_t *)g_malloc(65536 + 6);
		if(handle->priv2 == NULL)
		{
			g_free(handle);
			return NULL;
		}
	}

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
TIEXPORT3 int TICALL ticalcs_handle_del(CalcHandle* handle)
{
	if (handle != NULL)
	{
		if(handle->attached)
			ticalcs_cable_detach(handle);

		if(handle->priv2)
			g_free(handle->priv2);

		g_free(handle);
	}
	else
	{
		ticalcs_critical("ticalcs_handle_del(NULL)");
	}

	return 0;
}

/**
 * ticalcs_handle_show:
 * @handle: the handle
 *
 * Show information stored in the handle.
 *
 * Return value: always 0.
 **/
TIEXPORT3 int TICALL ticalcs_handle_show(CalcHandle* handle)
{
	if (handle != NULL)
	{
		ticalcs_info(_("Link calc handle details:"));
		ticalcs_info(_("  model   : %s"), ticalcs_model_to_string(handle->model));
	}
	else
	{
		ticalcs_critical("ticalcs_handle_show(NULL)");
	}

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
TIEXPORT3 int TICALL ticalcs_cable_attach(CalcHandle* handle, CableHandle* cable)
{
	if (handle != NULL)
	{
		handle->cable = cable;
		handle->attached = !0;

		TRYC(ticables_cable_open(cable));
		handle->open = !0;

		return 0;
	}
	else
	{
		ticalcs_critical("ticalcs_cable_attach(NULL)");
		return ERR_INVALID_HANDLE;
	}
}

/**
 * ticalcs_cable_detach:
 * @handle: the handle
 * @cable: a cable to use
 *
 * Close and detach the cable associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_cable_detach(CalcHandle* handle)
{
	if (handle != NULL)
	{
		TRYC(ticables_cable_close(handle->cable));
		handle->open = 0;

		handle->attached = 0;
		handle->cable = NULL;
		handle = NULL;

		return 0;
	}
	else
	{
		ticalcs_critical("ticalcs_handle_show(NULL)");
		return ERR_INVALID_HANDLE;
	}
}

/**
 * ticalcs_update_set:
 * @handle: the handle
 * @update: the callbacks to use
 *
 * Set the callbacks to use for the given handle.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_update_set(CalcHandle* handle, CalcUpdate* upd)
{
	if (handle != NULL)
	{
		handle->updat = upd;
		return 0;
	}
	else
	{
		ticalcs_critical("ticalcs_update_set: handle is NULL");
		return ERR_INVALID_HANDLE;
	}

}
