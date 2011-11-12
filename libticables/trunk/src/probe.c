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
#include <string.h>
#include <stdlib.h>

#include "gettext.h"
#include "logging.h"
#include "ticables.h"
#include "error.h"

#include "linux/detect.h"
#include "win32/detect.h"
#include "bsd/detect.h"

static void ticables_probing_show(int **array)
{
	CableModel model;

	for(model = CABLE_NUL; model < CABLE_MAX; model++)
	{
		ticables_info(_(" %i: %i %i %i %i"), model, array[model][1], array[model][2], array[model][3], array[model][4]);
	}
}

/**
 * ticables_probing_do:
 * @result: address of an array of integers to put the result.
 * @timeout: timeout to set during probing
 * @method: defines which link cables you want to search for.
 *
 * Returns cables which have been detected. All cables should be closed before !
 * The array is like a matrix which contains 5 columns (PORT_0 to PORT_4) and 
 * 7 lines (CABLE_GRY to CABLE_USB).
 * The array must be freed by #ticables_probing_finish when no longer used.
 *
 * Return value: 0 if successful, ERR_NO_CABLE if no cables found.
 **/
TIEXPORT1 int TICALL ticables_probing_do(int ***result, int timeout, ProbingMethod method)
{
	CablePort port;
	CableModel model;
	int **array;
	int found = 0;

	if (result == NULL)
	{
		ticables_critical("%s: result is NULL", __FUNCTION__);
		return ERR_PROBE_FAILED;
	}

	ticables_info(_("Link cable probing:"));

	array = (int **)calloc(CABLE_MAX + 1, sizeof(int *));
	for(model = CABLE_NUL; model <= CABLE_MAX; model++)
	    array[model] = (int *)calloc(5, sizeof(int));

	// look for USB devices (faster)
	if(method & PROBE_USB)
	{
		int *list, n, i;

		ticables_get_usb_devices(&list, &n);

		for(i = 0; i < n; i++)
		{
			port = i+1;

			if(list[i] == PID_TIGLUSB)
				array[CABLE_SLV][port] = !0;

			if(list[i])
				array[CABLE_USB][port] = !0;

			if(list[i])
				found = !0;
		}
	}

	if((method & PROBE_FIRST) && found)
	{
		*result = array;
		return found ? 0 : ERR_NO_CABLE;
	}

	// look for DBUS devices (slower)
	if(method & PROBE_DBUS)
	{
		for(model = CABLE_GRY; model <= CABLE_PAR; model++)
		{
			for(port = PORT_1; port <= PORT_4; port++)
			{
				CableHandle* handle;
				int err, ret;

				handle = ticables_handle_new(model, port);
				if(handle)
				{
					ticables_options_set_timeout(handle, timeout);
					err = ticables_cable_probe(handle, &ret);
					array[model][port] = (ret && !err) ? 1: 0;
					if(array[model][port]) found = !0;

					if(found && (method & PROBE_FIRST))
					{
						ticables_handle_del(handle);
						break;
					}
				}
				ticables_handle_del(handle);
			}
		}
	}

	*result = array;
	return found ? 0 : ERR_NO_CABLE;
}

/**
 * ticables_probing_finish:
 * @result: address of an array of integers. 
 *
 * Free the array created by #ticables_probing_do.
 *
 * Return value: always 0.
 **/
TIEXPORT1 int TICALL ticables_probing_finish(int ***result)
{
	int i;

	if (result != NULL && *result != NULL)
	{
		for(i = CABLE_GRY; i <= CABLE_TIE; i++)
		{
			free((*result)[i]);
			(*result)[i] = NULL;
		}

		free(*result);
		*result = NULL;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * ticables_is_usb_enabled:
 *
 * Checks whether USB support is available. Can be called at any time.
 *
 * Return value: !0 if available, 0 otherwise.
 **/
TIEXPORT1 int TICALL ticables_is_usb_enabled(void)
{
#if defined(__WIN32__)
	return !win32_check_libusb();
#elif defined(__LINUX__) && (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0))
	return !linux_check_libusb();
#elif (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0))
	return 1;
#else
	return 0;
#endif

	return 0;
}

extern int usb_probe_devices(int **list);

/**
 * ticables_get_usb_devices:
 * @array: address of a NULL-terminated allocated array of integers (PIDs).
 * @length: number of detected USB devices.
 *
 * Returns the list of detected USB PIDs. Note that list is in the 
 * same order as PORT#x.
 * The array must be freed when no longer used.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_get_usb_devices(int **list, int *len)
{
	if (list != NULL)
	{
#if defined(__WIN32__) || (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0))
		int i, *p;
		int ret = 0;

		ret = usb_probe_devices(list);
		if(ret)
		{
			*list = calloc(1, sizeof(int));
			if(len) *len = 0;
			return ret;
		}

		for(p = *list, i = 0; *p; p++, i++);
			//printf("%i: %04x\n", i, (*list)[i]);

		if(len) *len = i;
#else
		*list = calloc(1, sizeof(int));
		if(len) *len = 0;
#endif
	}
	else
	{
		ticables_critical("%s: list is NULL", __FUNCTION__);
		return -1;
	}

	return 0;
}
