/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2009  Romain Lievin
 *  Copyright (C) 2013-2016  Lionel Debroux
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
#include <unistd.h>
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

#ifdef __WIN32__
#include <windows.h>
#define PAUSE(x) Sleep(x)
#else
#define PAUSE(x) usleep(1000*(x))
#endif

/**
 * tiopers_device_probe_usb:
 *
 * This function probes USB devices and returns the first device found.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT5 int TICALL tiopers_device_probe_usb(CableModel* cable_model, CablePort *port, CalcModel* calc_model)
{
	int err;
	int ret = -1;
	int **cables = NULL;
	CableHandle* handle;
	CableModel cable;

	// search for all USB cables (faster)
	tiopers_info("Searching for link cables...");
	err = ticables_probing_do(&cables, 5, PROBE_USB | PROBE_FIRST);
	if(err)
	{
		ticables_probing_finish(&cables);
		return -1;
	}

	for (cable = CABLE_USB; cable >= CABLE_SLV; cable--)
	{
		*port = ticables_probing_found(cables[cable]);
		if (*port)
		{
			tiopers_info("Searching for handhelds on %s", ticables_model_to_string(cable));
			handle = ticables_handle_new(CABLE_USB, *port);
			ticables_options_set_timeout(handle, 10);

			ret = ticables_cable_open(handle);
			if (ret)
			{
				ticables_handle_del(handle);
				continue;
			}

			ret = ticalcs_probe_usb_calc(handle, calc_model);
			ticables_cable_close(handle);
			ticables_handle_del(handle);
			if (!ret)
			{
				*cable_model = cable;
				ret = 0;
				break;
			}
		}
	}

	ticables_probing_finish(&cables);
	return ret;
}

/**
 * tiopers_device_probe_all:
 *
 * This function probes all cables and devices.
 * The current device is closed during scanning because some link cables
 * (serial & parallel) can't be probed while open because already locked.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT5 int TICALL tiopers_device_probe_all(OperHandle *handle, int ***result)
{
	int ret;

	// Close device
	ret = tiopers_device_close(handle);
	if (!ret)
	{
		int **array;
		CableModel cable;
		CablePort port;
		CalcModel calc;
		int found = 0;

		// Search for cables
		tiopers_info("Searching for link cables...");
		ticables_probing_do(&array, 5, PROBE_ALL);
		*result = array;

		ticables_probing_show(array);

		// Is there at least 1 cable ?
		for (cable = CABLE_GRY; cable <= CABLE_MAX; cable++)
		{
			for (port = PORT_1; port <= PORT_4; port++)
			{
				if (array[cable][port])
				{
					found = 1;
				}
			}
		}
		if(!found)
		{
			return -1;
		}

		// search for devices on all cables
		for (cable = CABLE_GRY; cable <= CABLE_USB; cable++)
		{
			for (port = PORT_1; port <= PORT_4; port++)
			{
				if (!array[cable][port])
				{
					continue;
				}

				tiopers_info("Searching for handhelds on %i:%i...", cable, port);

				ret = ticalcs_probe(cable, port, &calc, !0);
				if (ret)
				{
					array[cable][port] = CALC_NONE;
					continue;
				}
				array[cable][port] = calc;

				tiopers_info("Found: %s on %s:%s",
					ticalcs_model_to_string(calc),
					ticables_model_to_string(cable),
					ticables_port_to_string(port));
			}
		}

		// show list
		ticables_probing_show(array);

		// Re-open device
		ret = tiopers_device_open(handle);
	}

	return ret;
}

//TODO use OperHandle.open instead.
static int lk_open = 0; /* keep status to avoid multiple error messages */

TIEXPORT5 int TICALL tiopers_device_open(OperHandle *handle)
{
	int ret = 0;
	const char *calc;
	const char *cable;
	CableHandle *cable_handle;
	CalcHandle *calc_handle;

	VALIDATE_HANDLE(handle);

	// TODO error checking.
	// close cable before opening a new one
	if (lk_open)
	{
		tiopers_device_close(handle);
	}

	cable = ticables_model_to_string(handle->options.cable_model);
	calc = ticalcs_model_to_string(handle->options.calc_model);
	tiopers_info("Opening cable %s on port #%d to communicate with calculator %s", cable, handle->options.cable_port, calc);
	if (handle->options.calc_model == CALC_NONE)
	{
		tiopers_warning("Attempting to communicate with calculator \"none\" !");
	}

	cable_handle = ticables_handle_new(handle->options.cable_model, handle->options.cable_port);
	if (cable_handle == NULL)
	{
		// TODO leave this in caller.
		//gif->msg_box1("Error", "Can't set cable");
	}
	else
	{
		ticables_options_set_timeout(cable_handle, handle->options.cable_timeout);
		ticables_options_set_delay(cable_handle, handle->options.cable_delay);

		calc_handle = ticalcs_handle_new(handle->options.calc_model);
		if (calc_handle == NULL)
		{
			// TODO leave this in caller.
			//gif->msg_box1("Error", "Can't set calc");
		}
		else
		{
			ret = ticalcs_cable_attach(calc_handle, cable_handle);
			//tilp_err(ret);

			if (handle->options.cable_model != CABLE_USB)
			{
				// BlackLink & ParallelLink need a reset before use
				ret = ticables_cable_reset(cable_handle);
				PAUSE(2000);
				// TODO refactor this.
				//tilp_err(ret);
			}
		}

		// Initialize callbacks with default functions
		//tilp_update_set_default();
		//TODO leave this in caller.
		//tilp_update_set_gtk();
	}

	// TODO store cable_handle, calc_handle.

	lk_open = ret ? 0 : 1;
	return ret;
}

TIEXPORT5 int TICALL tiopers_device_close(OperHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	// close cable unless already closed
	if (!lk_open)
	{
		return 0;
	}

	// detach cable (made by handle_del, too)
	if (handle->calc == NULL)
	{
		return 0;
	}
	ret = ticalcs_cable_detach(handle->calc);
	// TODO refactor this.
	//tilp_err(ret);

	// remove calc & cable
	ticalcs_handle_del(handle->calc); handle->calc = NULL;
	ticables_handle_del(handle->cable); handle->cable = NULL;

	lk_open = 0;
	return ret;
}

static int tiopers_device_err(int err)
{
	char *s = NULL;

	if (!err)
	{
		return 0;
	}

	tiopers_info("tiopers_device_err caught error %i\n", err);

	err = ticables_error_get(err, &s);
	if (err) 
	{
		g_free(s);
		err = ticalcs_error_get(err, &s);
		if (err)
		{
			g_free(s);
			err = tiopers_error_get(err, &s);
			if (err)
			{
				g_free(s);
			}
		}
	}

	if (s)
	{
		// TODO leave this in caller.
		/*char *utf;
		gsize bw;
		utf = g_locale_to_utf8(s, -1, NULL, &bw, NULL);
		gif->msg_box1(_("Error"), utf);*/
	}

	return err;
}

/*
  Note: SilverLink is still NACK'ed after error. This did not appear with 
  TILP 1 because it always closed/opened the device before any transfer.
  It seems that an error (HALT condition) cannot be cleared by a simple
  slv_reset. We need to reopen the device. Why? I don't know!

  No call to tilp_err in this function to avoid recursivity with tilp_err which
  may call tilp_device_reset.
*/
TIEXPORT5 int TICALL tiopers_device_reset(OperHandle *handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	if (lk_open)
	{
		ret = ticables_cable_reset(handle->cable);
		tiopers_device_err(ret);
		PAUSE(1000);
	}

	return ret;
}
