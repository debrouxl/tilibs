/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
#ifdef __LINUX__
#include <sys/utsname.h>
#endif

#ifdef _MSC_VER
# include "./win32/usb.h"
#elif defined(HAVE_LIBUSB)
# include <usb.h>
#elif defined(HAVE_LIBUSB_1_0)
# include <libusb.h>
#endif

#include "gettext.h"
#include "ticables.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "link_xxx.h"
#include "data_log.h"

/*****************/
/* Internal data */
/*****************/

static CableFncts const *const cables[] =
{
	&cable_nul,
#ifndef NO_CABLE_GRY
	&cable_gry,
#endif
#if !defined(NO_CABLE_BLK) && !defined(__MACOSX__)
	&cable_ser,
#endif
#if !defined(NO_CABLE_PAR) && (defined(HAVE_LINUX_PARPORT_H) || defined(__WIN32__))
	&cable_par,
#endif
#if !defined(NO_CABLE_SLV) && (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0) || defined(__WIN32__))
	&cable_slv,
#endif
#if !defined(NO_CABLE_SLV) && (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0) || defined(__WIN32__))
	&cable_raw,
#endif
#ifndef NO_CABLE_VTI
	&cable_vti,
#endif
#ifndef NO_CABLE_TIE
	&cable_tie,
#endif
	&cable_ilp,
	&cable_nul, // Dead cable
#if !defined(NO_CABLE_TCPC) && !defined(__WIN32__)
	&cable_tcpc,
#endif
#if !defined(NO_CABLE_TCPS) && !defined(__WIN32__)
	&cable_tcps,
#endif
	NULL
};

static const uint32_t supported_cables =
	  (1U << CABLE_NUL)
#ifndef NO_CABLE_GRY
	| (1U << CABLE_GRY)
#endif
#if !defined(NO_CABLE_BLK) && !defined(__MACOSX__)
	| (1U << CABLE_BLK)
#endif
#if !defined(NO_CABLE_PAR) && (defined(HAVE_LINUX_PARPORT_H) || defined(__WIN32__))
	| (1U << CABLE_PAR)
#endif
#if !defined(NO_CABLE_SLV) && (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0) || defined(__WIN32__))
	| (1U << CABLE_SLV)
#endif
#if !defined(NO_CABLE_SLV) && (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0) || defined(__WIN32__))
	| (1U << CABLE_USB)
#endif
#ifndef NO_CABLE_VTI
	| (1U << CABLE_VTI)
#endif
#ifndef NO_CABLE_TIE
	| (1U << CABLE_TIE)
#endif
	| (1U << CABLE_ILP)
	// | (1U << CABLE_DEV) Dead cable
#if !defined(NO_CABLE_TCPC) && !defined(__WIN32__)
	| (1U << CABLE_TCPC)
#endif
#if !defined(NO_CABLE_TCPS) && !defined(__WIN32__)
	| (1U << CABLE_TCPS)
#endif
;

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticables_instance = 0;	// counts # of instances
#ifdef HAVE_LIBUSB_1_0
int libusb_working = -1;
#endif

/**
 * ticables_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the instance count.
 **/
TIEXPORT1 int TICALL ticables_library_init(void)
{
	char locale_dir[65536];

#ifdef __WIN32__
	HANDLE hDll;
	int i;

	hDll = GetModuleHandle("libticables2-8.dll");
	GetModuleFileName(hDll, locale_dir, 65515);

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
#endif

	if (ticables_instance)
	{
		return (++ticables_instance);
	}
	ticables_info(_("ticables library version %s"), libticables2_VERSION);
	errno = 0;

#if defined(ENABLE_NLS)
	ticables_info("setlocale: %s", setlocale(LC_ALL, ""));
	ticables_info("bindtextdomain: %s", bindtextdomain(PACKAGE, locale_dir));
	bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
	ticables_info("textdomain: %s", textdomain(NULL));
#endif
#ifdef __LINUX__
	{
		struct utsname buf;

		uname(&buf);
		ticables_info("kernel: %s", buf.release);
	}
#endif
#if defined(HAVE_LIBUSB)
	// init libusb
	usb_init();
#elif defined(HAVE_LIBUSB_1_0)
	// init libusb, keeping track of success / failure status
	libusb_working = libusb_init(NULL);
	if (libusb_working == LIBUSB_SUCCESS) {
		libusb_set_debug(NULL, 3);
	}
#endif

	return (++ticables_instance);
}


/**
 * ticables_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the instance count.
 **/
TIEXPORT1 int TICALL ticables_library_exit(void)
{
#if defined(HAVE_LIBUSB)
	// No exit function for libusb 0.1.x.
#elif defined(HAVE_LIBUSB_1_0)
	// Must not call libusb_exit() if libusb_init() failed, or call it multiple times.
	if (ticables_instance == 1 && libusb_working == LIBUSB_SUCCESS) {
		libusb_exit(NULL);
	}
#endif
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
TIEXPORT1 const char *TICALL ticables_version_get(void)
{
	return libticables2_VERSION;
}

/**
 * ticables_supported_cables:
 *
 * This function returns the cables built into the current binary.
 *
 * Return value: an integer containing a binary OR of (1 << CABLE_*) values,
 * where CABLE_* values are defined in enum CableModel.
 **/
TIEXPORT1 uint32_t TICALL ticables_supported_cables(void)
{
	return supported_cables;
}

/**
 * ticables_max_ports:
 *
 * This function returns the maximum number of ports supported for any given cable.
 *
 * Return value: the PORT_MAX value against which the library was built.
 **/
TIEXPORT1 uint32_t TICALL ticables_max_ports(void)
{
	return PORT_MAX;
}

static int default_event_hook(CableHandle * handle, uint32_t event_count, const CableEventData * event, void * user_pointer)
{
	(void)user_pointer;
	const char * cablestr = ticables_model_to_string(ticables_get_model(handle));
	const char * portstr = ticables_port_to_string(ticables_get_port(handle));
	if (getenv("TICABLES_EVENT_DEBUG") != NULL)
	{
		ticables_info("Event #%u type %d for cable %s port %s", event_count, event->type, cablestr, portstr);
	}
	switch (event->type)
	{
		case CABLE_EVENT_TYPE_BEFORE_SEND: break;
		case CABLE_EVENT_TYPE_AFTER_SEND:
		{
			LOG_N_DATA(handle, LOG_OUT, event->data.data.data, event->data.data.len);
			break;
		}
		case CABLE_EVENT_TYPE_BEFORE_RECV: break;
		case CABLE_EVENT_TYPE_AFTER_RECV:
		{
			LOG_N_DATA(handle, LOG_IN, event->data.data.data, event->data.data.len);
			break;
		}
		default: break;
	}
	return event->retval;
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
TIEXPORT1 CableHandle* TICALL ticables_handle_new(CableModel model, CablePort port)
{
	CableHandle *handle = (CableHandle *)calloc(1, sizeof(CableHandle));
	int i;

	handle->model = model;
	handle->port = port;

	handle->delay = DFLT_DELAY;
	handle->timeout = DFLT_TIMEOUT;

	for (i = 0; cables[i] != NULL; i++)
	{
		if (cables[i]->model == (const int)model)
		{
			handle->cable = (CableFncts *)cables[i];
			break;
		}
	}

	if (handle->cable == NULL)
	{
		free(handle);
		handle = NULL;
	}

	if (handle != NULL)
	{
		handle->event_hook = default_event_hook;
		//handle->event_count = 0;
	}

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
TIEXPORT1 int TICALL ticables_handle_del(CableHandle* handle)
{
	VALIDATE_HANDLE(handle);

	free(handle->priv2);
	handle->priv2 = NULL;

	free(handle->device);
	handle->device = NULL;

	memset((void *)handle, 0, sizeof(*handle));
	free(handle);

	return 0;
}

/**
 * ticables_options_set_timeout:
 * @handle: the handle
 * @timeout: timeout value in tenth of seconds
 *
 * Set timeout for any cable.
 *
 * Return value: the previous timeout.
 **/
TIEXPORT1 unsigned int TICALL ticables_options_set_timeout(CableHandle* handle, unsigned int timeout)
{
	if (handle != NULL)
	{
		const CableFncts *cable;
		unsigned int old_timeout = handle->timeout;

		handle->timeout = timeout;
		cable = handle->cable;

		if(!handle->open)
		{
			return -1;
		}
		if(handle->busy)
		{
			return ERR_BUSY;
		}

		handle->busy = 1;
		if(cable->timeout)
		{
			cable->timeout(handle);
		}
		handle->busy = 0;

		return old_timeout;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_options_set_delay:
 * @handle: the handle
 * @delay: delay in micro-seconds
 *
 * Set inter-bit delay for parallel or BlackLink cable.
 *
 * Return value: the previous delay.
 **/
TIEXPORT1 unsigned int TICALL ticables_options_set_delay(CableHandle* handle, unsigned int delay)
{
	if (handle != NULL)
	{
		unsigned int old_delay = handle->delay;
		handle->delay = delay;
		return old_delay;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_get_model:
 * @handle: the handle
 *
 * Retrieve link cable model.
 *
 * Return value: the previous #CableModel value.
 **/
TIEXPORT1 CableModel TICALL ticables_get_model(CableHandle* handle)
{
	if (handle != NULL)
	{
		return handle->model;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_get_port:
 * @handle: the handle
 *
 * Retrieve link port.
 *
 * Return value: a #CablePort value.
 **/
TIEXPORT1 CablePort TICALL ticables_get_port(CableHandle* handle)
{
	if (handle != NULL)
	{
		return handle->port;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_get_device:
 * @handle: the handle
 *
 * Retrieve device port.
 *
 * Return value: a char pointer.
 **/
TIEXPORT1 const char * TICALL ticables_get_device(CableHandle* handle)
{
	if (handle != NULL)
	{
		return handle->device;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_get_timeout:
 * @handle: the handle
 *
 * Retrieve timeout.
 *
 * Return value: an unsigned integer;
 **/
TIEXPORT1 unsigned int TICALL ticables_get_timeout(CableHandle *handle)
{
	if (handle != NULL)
	{
		return handle->timeout;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_get_delay:
 * @handle: the handle
 *
 * Retrieve timeout.
 *
 * Return value: an unsigned integer;
 **/
TIEXPORT1 unsigned int TICALL ticables_get_delay(CableHandle *handle)
{
	if (handle != NULL)
	{
		return handle->delay;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return 0;
	}
}

/**
 * ticables_handle_show:
 * @handle: the handle
 *
 * Show information stored in the handle.
 *
 * Return value: always 0.
 **/
TIEXPORT1 int TICALL ticables_handle_show(CableHandle* handle)
{
	if (handle != NULL)
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
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}
