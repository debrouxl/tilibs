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
#include <stdlib.h>

#include "ticables.h"
#include "data_log.h"
#include "error.h"
#include "logging.h"

/**
 * ticables_cable_open:
 * @handle: a previously allocated handle
 *
 * Attempt to open a connection on the cable with the parameters
 * given with #ticables_handle_new().
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_open(CableHandle* handle)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;

		TRYC(handle->cable->prepare(handle));

		TRYC((cable->open)(handle));

		handle->open = 1;
		START_LOGGING(handle);
		return 0;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_reset:
 * @handle: a previously allocated handle
 *
 * Reset link cable status (flush buffers, set ready).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_reset(CableHandle* handle)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		if(cable->reset)
			ret = cable->reset(handle);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_probe:
 * @handle: a previously allocated handle
 * @result: cable found (!0) or not (0)
 *
 * Attempt to probe if a cable is present. Open device if not opened.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_probe(CableHandle* handle, int* result)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int opened = handle->open;
		int ret;

		// Check if device is already opened
		if(!opened && cable->need_open)
		{
			TRYC(ticables_cable_open(handle));
		}
		else if(!opened && !cable->need_open)
		{
			TRYC(handle->cable->prepare(handle));
		}

		// Do the check itself
		ret = cable->probe(handle);
		if (result != NULL)
		{
			*result = !ret;
		}
		else
		{
			ticables_critical("%s: result is NULL", __FUNCTION__);
		}

		// If it was opened for this, close it
		if(!opened && cable->need_open)
		{
			TRYC(ticables_cable_close(handle));
		}
		else if(!opened && !cable->need_open)
		{
			free(handle->device); handle->device = NULL;
			free(handle->priv2); handle->priv2 = NULL;
		}

		handle = NULL;
		return 0;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_close:
 * @handle: a previously allocated handle
 *
 * Attempt to close a connection on the cable with the parameters
 * given with #ticables_handle_new().
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_close(CableHandle* handle)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;

		STOP_LOGGING(handle);
		if(handle->open)
		{
			cable->close(handle);
			handle->open = 0;
			free(handle->device);
			handle->device = NULL;
		}
		return 0;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_send:
 * @handle: a previously allocated handle
 * @data: buffer with data to send
 * @len: length of buffer
 *
 * Send %len bytes of the %data buffer from PC to hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_send(CableHandle* handle, uint8_t *data, uint32_t len)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;
		if(!len)
		{
			// Complain loudly, but don't return 0 immediately in such a case.
			// Indeed, the DUSB file transfer code of the 84+(SE) & 89T wants writes
			// of length 0 after writes of length 64, otherwise transfer hangs...
			//
			// The workaround for these models used to be done purely in libticables,
			// but doing so broke ROM dumping... so the problem is probably not
			// intrinsic to the USB controller and low-level USB stack.
			ticables_critical("ticables_cable_send: len = 0\n");
		}

		handle->busy = 1;
		handle->rate.count += len;
		if (data != NULL)
		{
			ret = cable->send(handle, data, len);
			LOG_N_DATA(handle, LOG_OUT, data, len);
		}
		else
		{
			ticables_critical("%s: data is NULL", __FUNCTION__);
		}
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_put:
 * @handle: a previously allocated handle
 * @data: data to send
 *
 * Send one byte from PC to hand-held.
 * Convenient function implemented for compatibility.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_put(CableHandle* handle, uint8_t data)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		handle->rate.count += 1;
		ret = cable->send(handle, &data, 1);
		LOG_1_DATA(handle, LOG_OUT, data);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_recv:
 * @handle: a previously allocated handle
 * @data: buffer where data can be placed
 * @len: number of bytes requested
 *
 * Attempt to receive %len bytes from hand-held to PC.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_recv(CableHandle* handle, uint8_t *data, uint32_t len)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;
		if(!len)
		{
			// See ticables_cable_send above.
			ticables_critical("ticables_cable_recv: len = 0\n");
		}

		handle->busy = 1;
		handle->rate.count += len;
		if (data != NULL)
		{
			ret = cable->recv(handle, data, len);
			LOG_N_DATA(handle, LOG_IN, data, len);
		}
		else
		{
			ticables_critical("%s: data is NULL", __FUNCTION__);
		}
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_get:
 * @handle: a previously allocated handle
 * @data: data to receive
 *
 * Receive one byte from hand-held to PC.
 * Convenient function implemented for compatibility.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_get(CableHandle* handle, uint8_t *data)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;
		
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		handle->rate.count += 1;
		if (data != NULL)
		{
			ret = cable->recv(handle, data, 1);
			LOG_1_DATA(handle, LOG_IN, *data);
		}
		else
		{
			ticables_critical("%s: data is NULL", __FUNCTION__);
		}
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return -1;
	}
}

/**
 * ticables_cable_check:
 * @handle: a previously allocated handle
 * @status: status is placed here
 *
 * Check for link cable status
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_check(CableHandle* handle, CableStatus *status)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		if (status != NULL)
		{
			ret = cable->check(handle, (int *)status);
		}
		else
		{
			ticables_critical("%s: status is NULL", __FUNCTION__);
		}
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_set_raw:
 * @handle: a previously allocated handle
 * @state: bit mask of lines to pull low
 *
 * Set the raw cable state (if the hardware supports raw access.)  An
 * input of 3 is the default state; 2 means to pull the D0 ("red")
 * line low; 1 means to pull the D1 ("white") line low; 0 means both
 * (never used in the TI link protocol.)
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_set_raw(CableHandle* handle, int state)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;

		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;
		if(!handle->cable->set_raw)
			return ERR_RAW_IO_UNSUPPORTED;

		handle->busy = 1;
		ret = cable->set_raw(handle, state);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_get_raw:
 * @handle: a previously allocated handle
 * @state: pointer to variable to store current state
 *
 * Get the raw cable state (if the hardware supports raw access.)  A
 * bit set in the output means the line is high (1 for the D0 or "red"
 * line, 2 for the D1 or "white" line.)
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_get_raw(CableHandle* handle, int *state)
{
	if (handle != NULL)
	{
		const CableFncts *cable;
		int ret = 0;

		if(state == NULL)
		{
			ticables_critical("%s: state is NULL", __FUNCTION__);
			return ERR_ILLEGAL_ARG;
		}

		cable = handle->cable;
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;
		if(!handle->cable->get_raw)
			return ERR_RAW_IO_UNSUPPORTED;

		handle->busy = 1;
		ret = cable->get_raw(handle, state);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_set_d0:
 * @handle: a previously allocated handle
 * @state: logical state (0 or 1) of D0 wire.
 *
 * Set the electrical state of the D0 wire (if possible).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_set_d0(CableHandle* handle, int state)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;
		
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		ret = cable->set_d0(handle, state);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_set_d1:
 * @handle: a previously allocated handle
 * @state: logical state (0 or 1) of D1 wire.
 *
 * Set the electrical state of the D1 wire (if possible).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT1 int TICALL ticables_cable_set_d1(CableHandle* handle, int state)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;
		
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		ret = cable->set_d1(handle, state);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_get_d0:
 * @handle: a previously allocated handle
 *
 * Get the electrical state of the D0 wire (if possible).
 *
 * Return value: 0 or 1.
 **/
TIEXPORT1 int TICALL ticables_cable_get_d0(CableHandle* handle)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;
		
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		ret = cable->get_d0(handle);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_get_d1:
 * @handle: a previously allocated handle
 *
 * Get the electrical state of the D1 wire (if possible).
 *
 * Return value: 0 or 1.
 **/
TIEXPORT1 int TICALL ticables_cable_get_d1(CableHandle* handle)
{
	if (handle != NULL)
	{
		const CableFncts *cable = handle->cable;
		int ret = 0;
		
		if(!handle->open)
			return ERR_NOT_OPEN;
		if(handle->busy)
			return ERR_BUSY;

		handle->busy = 1;
		ret = cable->get_d1(handle);
		handle->busy = 0;

		return ret;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_progress_reset:
 * @handle: a previously allocated handle
 *
 * Reset byte counter and timer used for computing data rate.
 *
 * Return value: always 0.
 **/
TIEXPORT1 int TICALL ticables_progress_reset(CableHandle* handle)
{
	if (handle != NULL)
	{
		handle->rate.count = 0;
		TO_START(handle->rate.start);
		return 0;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return ERR_NO_CABLE;
	}
}

/**
 * ticables_cable_progress_get:
 * @handle: a previously allocated handle
 * @count: number of bytes transfered
 * @msec: time needed for the operation
 * @rate: data rate
 *
 * Returns information needed to compute the transfer rate of the link cable.
 *
 * Return value: always 0.
 **/
TIEXPORT1 int TICALL ticables_progress_get(CableHandle* handle, int* count, int* msec, float* rate)
{
	if (handle != NULL)
	{
		TO_START(handle->rate.current);

		if(count)
			*count = handle->rate.count;

		if(msec)
			*msec = handle->rate.current - handle->rate.start;

		if(rate)
		{
			if(handle->rate.current > handle->rate.start)
				*rate = (float)handle->rate.count / ((float)(handle->rate.current - handle->rate.start));
		}

#if 0
		if(handle->rate.current > handle->rate.start)
			printf("<%u %u %u %u %f\n", 
				handle->rate.count, handle->rate.start, handle->rate.current,
				handle->rate.current - handle->rate.start, (float)handle->rate.count / ((float)(handle->rate.current - handle->rate.start)));
#endif

		return 0;
	}
	else
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return -1;
	}
}
