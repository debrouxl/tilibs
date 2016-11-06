/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
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
#include "tiopers.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

/*****************/
/* Internal data */
/*****************/


/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int tiopers_instance = 0;	// counts # of instances

/**
 * tiopers_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the instance count.
 **/
TIEXPORT5 int TICALL tiopers_library_init(void)
{
	char locale_dir[65536];

#ifdef __WIN32__
	HANDLE hDll;
	int i;

	hDll = GetModuleHandle("libtiopers-1.dll");
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

	if (tiopers_instance)
		return (++tiopers_instance);
	tiopers_info(_("tiopers library version %s"), LIBOPERS_VERSION);
	errno = 0;

#if defined(ENABLE_NLS)
	tiopers_info("setlocale: %s", setlocale(LC_ALL, ""));
	tiopers_info("bindtextdomain: %s", bindtextdomain(PACKAGE, locale_dir));
	bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
	tiopers_info("textdomain: %s", textdomain(NULL));
#endif

	return (++tiopers_instance);
}


/**
 * tiopers_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the instance count.
 **/
TIEXPORT5 int TICALL tiopers_library_exit(void)
{
	return (--tiopers_instance);
}

/***********/
/* Methods */
/***********/

/**
 * tiopers_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT5 const char *TICALL tiopers_version_get(void)
{
	return LIBOPERS_VERSION;
}

/**
 * tiopers_handle_new:
 * @model: a hand-held model
 *
 * Create a new handle associated with the given cable on the given port.
 * Must be freed with tiopers_handle_del when no longer needed.
 * Note: the handle is a pointer on an opaque structure and should not be modified.
 *
 * Return value: NULL if error, an handle otherwise.
 **/
TIEXPORT3 OperHandle* TICALL tiopers_handle_new(void)
{
	OperHandle *handle = (OperHandle *)g_malloc0(sizeof(OperHandle));

	return handle;
}

/**
 * tiopers_handle_del:
 * @handle: the handle
 *
 * Release the handle and free the associated resources.
 * If cable and calc have not been detached with #tiopers_cable_detach and #tiopers_calc_detach,
 * they will be detached.
 *
 * Return value: always 0.
 **/
TIEXPORT3 int TICALL tiopers_handle_del(OperHandle* handle)
{
	VALIDATE_HANDLE(handle);

	if (handle->calc_attached)
	{
		tiopers_calc_detach(handle);
	}

	if (handle->cable_attached)
	{
		tiopers_cable_detach(handle);
	}

	memset((void *)handle, 0, sizeof(*handle));
	g_free(handle);

	return 0;
}

/**
 * tiopers_handle_show:
 * @handle: the handle
 *
 * Show information stored in the handle.
 *
 * Return value: always 0.
 **/
TIEXPORT3 int TICALL tiopers_handle_show(OperHandle* handle)
{
	VALIDATE_HANDLE(handle);

	tiopers_info(_("Link calc handle details:"));
	tiopers_info(_("  cable model   : %s"), ticalcs_model_to_string(handle->options.cable_model));
	tiopers_info(_("  calc model    : %s"), ticalcs_model_to_string(handle->options.calc_model));
	ticables_handle_show(handle->cable);
	ticalcs_handle_show(handle->calc);

	return 0;
}

/**
 * tiopers_cable_attach:
 * @handle: the handle
 * @cable: a cable to use
 *
 * Attach the given cable for use with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL tiopers_cable_attach(OperHandle* handle, CableHandle* cable)
{
	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(cable);

	handle->options.cable_model = ticables_get_model(cable);
	handle->options.cable_port = ticables_get_port(cable);
	handle->cable = cable;
	handle->cable_attached = !0;

	return 0;
}

/**
 * tiopers_cable_detach:
 * @handle: the handle
 *
 * Detach the cable associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL tiopers_cable_detach(OperHandle* handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	handle->open = 0;

	handle->cable_attached = 0;
	handle->cable = NULL;

	return ret;
}

/**
 * tiopers_cable_get:
 * @handle: the handle
 *
 * Returns the cable associated with the hand-held, if any.
 *
 * Return value: the cable handle or NULL.
 **/
TIEXPORT3 CableHandle* TICALL tiopers_cable_get(OperHandle *handle)
{
	if (tiopers_validate_handle(handle))
	{
		return handle->cable;
	}

	return NULL;
}

/**
 * tiopers_calc_attach:
 * @handle: the handle
 * @calc: a calc to use
 *
 * Attach the given calc for use with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL tiopers_calc_attach(OperHandle* handle, CalcHandle* calc)
{
	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(calc);

	handle->options.calc_model = ticalcs_get_model(calc);
	handle->calc = calc;
	handle->calc_attached = !0;

	return 0;
}

/**
 * tiopers_calc_detach:
 * @handle: the handle
 *
 * Detach the calc associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL tiopers_calc_detach(OperHandle* handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	handle->open = 0;

	handle->calc_attached = 0;
	handle->calc = NULL;

	return ret;
}

/**
 * tiopers_calc_get:
 * @handle: the handle
 *
 * Returns the calc associated with the hand-held, if any.
 *
 * Return value: the calc handle or NULL.
 **/
TIEXPORT3 CalcHandle* TICALL tiopers_calc_get(OperHandle *handle)
{
	if (tiopers_validate_handle(handle))
	{
		return handle->calc;
	}

	return NULL;
}
