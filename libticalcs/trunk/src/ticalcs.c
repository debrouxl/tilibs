/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
#include "internal.h"
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
#ifndef NO_TI89T
	&calc_89t,
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
#ifndef NO_TI84P_USB
	&calc_84p_usb,
#endif
#ifndef NO_TI89T_USB
	&calc_89t_usb,
#endif
#ifndef NO_NSPIRE
	&calc_nsp,
#endif
#ifndef NO_TI80
	&calc_80,
#endif
#ifndef NO_TI84PC
	&calc_84pcse,
#endif
#ifndef NO_TI84PC_USB
	&calc_84pcse_usb,
#endif
#ifndef NO_TI83PCE_USB
	&calc_83pce_usb,
#endif
#ifndef NO_TI84PCE_USB
	&calc_84pce_usb,
#endif
#ifndef NO_TI82A_USB
	&calc_82a_usb,
#endif
#ifndef NO_TI84PT_USB
	&calc_84pt_usb,
#endif
	NULL
};

static const uint32_t supported_calcs =
	  (1U << CALC_NONE)
#ifndef NO_TI73
	| (1U << CALC_TI73)
#endif
#ifndef NO_TI82
	| (1U << CALC_TI82)
#endif
#ifndef NO_TI83
	| (1U << CALC_TI83)
#endif
#ifndef NO_TI83P
	| (1U << CALC_TI83P)
#endif
#ifndef NO_TI84P
	| (1U << CALC_TI84P)
#endif
#ifndef NO_TI85
	| (1U << CALC_TI85)
#endif
#ifndef NO_TI86
	| (1U << CALC_TI86)
#endif
#ifndef NO_TI89
	| (1U << CALC_TI89)
#endif
#ifndef NO_TI89T
	| (1U << CALC_TI89T)
#endif
#ifndef NO_TI92
	| (1U << CALC_TI92)
#endif
#ifndef NO_TI92P
	| (1U << CALC_TI92P)
#endif
#ifndef NO_V200
	| (1U << CALC_V200)
#endif
#ifndef NO_TI84P_USB
	| (1U << CALC_TI84P_USB)
#endif
#ifndef NO_TI89T_USB
	| (1U << CALC_TI89T_USB)
#endif
#ifndef NO_NSPIRE
	| (1U << CALC_NSPIRE)
#endif
#ifndef NO_TI80
	| (1U << CALC_TI80)
#endif
#ifndef NO_TI84PC
	| (1U << CALC_TI84PC)
#endif
#ifndef NO_TI84PC_USB
	| (1U << CALC_TI84PC_USB)
#endif
#ifndef NO_TI83PCE_USB
	| (1U << CALC_TI83PCE_USB)
#endif
#ifndef NO_TI84PCE_USB
	| (1U << CALC_TI84PCE_USB)
#endif
#ifndef NO_TI82A_USB
	| (1U << CALC_TI82A_USB)
#endif
#ifndef NO_TI84PT_USB
	| (1U << CALC_TI84PT_USB)
#endif
;

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
	char locale_dir[65536 + 20];

#ifdef __WIN32__
	HANDLE hDll;
	int i;

	hDll = GetModuleHandle("libticalcs2-13.dll");
	GetModuleFileName(hDll, locale_dir, sizeof(locale_dir) - 21);

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
	strncpy(locale_dir, LOCALEDIR, sizeof(locale_dir) - 21);
	locale_dir[sizeof(locale_dir) - 21] = 0;
#endif

	if (ticalcs_instance)
	{
		return (++ticalcs_instance);
	}
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
TIEXPORT3 int TICALL ticalcs_library_exit(void)
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
 * ticalcs_supported_calcs:
 *
 * This function returns the calcs built into the current binary.
 *
 * Return value: an integer containing a binary OR of (1 << CALC_*) values,
 * where CALC_* values are defined in enum CalcModel.
 **/
TIEXPORT3 uint32_t TICALL ticalcs_supported_calcs (void)
{
	return supported_calcs;
}

/**
 * ticalcs_handle_new:
 * @model: a hand-held model
 *
 * Create a new handle associated with the given cable on the given port.
 * Must be freed with ticalcs_handle_del when no longer needed.
 * Note: the handle is a pointer on an opaque structure and should not be modified.
 *
 * Return value: NULL if error, an handle otherwise.
 **/
TIEXPORT3 CalcHandle* TICALL ticalcs_handle_new(CalcModel model)
{
	CalcHandle *handle = NULL;
	CalcFncts * calc = NULL;
	unsigned int i;

	for (i = 0; i < sizeof(calcs) / sizeof(calcs[0]) - 1; i++) // - 1 for the terminating NULL.
	{
		if (calcs[i]->model == (int const)model)
		{
			calc = (CalcFncts *)calcs[i];
			break;
		}
	}

	if (calc != NULL)
	{
		handle = (CalcHandle *)g_malloc0(sizeof(CalcHandle));
		if (handle != NULL)
		{
			handle->model = model;
			handle->calc = calc;
			handle->updat = (CalcUpdate *)&default_update;

			handle->priv.nsp_src_port = 0x8001;
			handle->priv.nsp_dst_port = 0x4003; // NSP_PORT_ADDR_REQUEST

			handle->buffer = (uint8_t *)g_malloc(65536 + 6);
			if (handle->buffer == NULL)
			{
				g_free(handle);
				handle = NULL;
			}
			handle->buffer2 = (uint8_t *)g_malloc(65536 + 6);
			if (handle->buffer2 == NULL)
			{
				g_free(handle->buffer);
				g_free(handle);
				handle = NULL;
			}
		}
	}

	return handle;
}

/**
 * ticalcs_handle_del:
 * @handle: the handle
 *
 * Release the handle and free the associated resources.
 * If cable has not been detached with #ticalcs_cable_detach,
 * it will be detached.
 *
 * Return value: always 0.
 **/
TIEXPORT3 int TICALL ticalcs_handle_del(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	if (handle->attached)
	{
		ticalcs_cable_detach(handle);
	}

	if (handle->buffer2)
	{
		g_free(handle->buffer2);
	}

	if (handle->buffer)
	{
		g_free(handle->buffer);
	}

	memset((void *)handle, 0, sizeof(*handle));
	g_free(handle);

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
	VALIDATE_HANDLE(handle);

	ticalcs_info(_("Link calc handle details:"));
	ticalcs_info(_("  model   : %s"), ticalcs_model_to_string(handle->model));

	return 0;
}

/**
 * ticalcs_get_model:
 * @handle: the handle
 *
 * Return the handle's calculator model.
 *
 * Return value: an integer containing the calc handle's calculator model.
 **/
TIEXPORT3 CalcModel TICALL ticalcs_get_model(CalcHandle *handle)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return CALC_NONE;
	}

	return handle->model;
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
	int ret;

	VALIDATE_HANDLE(handle);

	handle->cable = cable;
	handle->attached = !0;

	ret = ticables_cable_open(cable);
	if (!ret)
	{
		handle->open = !0;
	}

	return ret;
}

/**
 * ticalcs_cable_detach:
 * @handle: the handle
 *
 * Close and detach the cable associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_cable_detach(CalcHandle* handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ret = ticables_cable_close(handle->cable);
	if (!ret)
	{
		handle->open = 0;

		handle->attached = 0;
		handle->cable = NULL;
	}

	return ret;
}

/**
 * ticalcs_cable_get:
 * @handle: the handle
 *
 * Returns the cable associated with the hand-held.
 *
 * Return value: the cable handle or NULL.
 **/
TIEXPORT3 CableHandle* TICALL ticalcs_cable_get(CalcHandle *handle)
{
	if (ticalcs_validate_handle(handle))
	{
		return handle->cable;
	}

	return NULL;
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
	VALIDATE_HANDLE(handle);
	VALIDATE_CALCUPDATE(upd);

	handle->updat = upd;
	return 0;
}

/**
 * ticalcs_update_get:
 * @handle: the handle
 *
 * Get the update callbacks used for the given handle.
 *
 * Return value: the update struct pointer or NULL.
 **/
TIEXPORT3 CalcUpdate* TICALL ticalcs_update_get(CalcHandle *handle)
{
	if (ticalcs_validate_handle(handle))
	{
		return handle->updat;
	}

	return NULL;
}

/**
 * ticalcs_model_supports_dbus:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the protocol dubbed DBUS by libticalcs.
 * That is, the standard protocol used by TI-Z80 and TI-68k calculators over the legacy I/O.
 *
 * Return value: nonzero if the calculator supports the DBUS protocol, zero if it doesn't.
 */
TIEXPORT3 int TICALL ticalcs_model_supports_dbus(CalcModel model)
{
	return (   /*model <  CALC_MAX
	        &&*/ ( model == CALC_TI73
	            || model == CALC_TI82
	            || model == CALC_TI83
	            || model == CALC_TI83P
	            || model == CALC_TI84P
	            || model == CALC_TI85
	            || model == CALC_TI86
	            || model == CALC_TI89
	            || model == CALC_TI89T
	            || model == CALC_TI92
	            || model == CALC_TI92P
	            || model == CALC_V200
	            || model == CALC_TI80
	            || model == CALC_TI84PC));
}

/**
 * ticalcs_model_supports_dusb:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the protocol dubbed DUSB by libticalcs.
 * That is, the standard protocol used by TI-Z80, TI-eZ80 and TI-68k calculators over the USB port.
 *
 * Return value: nonzero if the calculator supports the DUSB protocol, zero if it doesn't.
 */
TIEXPORT3 int TICALL ticalcs_model_supports_dusb(CalcModel model)
{
	return (   /*model <  CALC_MAX
	        &&*/ ( model == CALC_TI84P_USB
	            || model == CALC_TI89T_USB
	            || model == CALC_TI84PC_USB
	            || model == CALC_TI83PCE_USB
	            || model == CALC_TI84PCE_USB
	            || model == CALC_TI82A_USB
	            || model == CALC_TI84PT_USB));
}

/**
 * ticalcs_model_supports_nsp:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the protocol dubbed NSP by libticalcs.
 * That is, the standard protocol used by TI-Nspire calculators over the USB port.
 *
 * Return value: nonzero if the calculator supports the NSP protocol, zero if it doesn't.
 */
TIEXPORT3 int TICALL ticalcs_model_supports_nsp(CalcModel model)
{
	return (   /*model <  CALC_MAX
	        &&*/ ( model == CALC_NSPIRE));
}
