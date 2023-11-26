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
#include "internal.h"
#include "error.h"
#include "logging.h"
#include "data_log.h"

/**
 * ticables_cable_open:
 * @handle: a previously allocated handle
 *
 * Attempt to open a connection on the cable with the parameters
 * given with #ticables_handle_new().
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticables_cable_open(CableHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_OPEN, /* retval */ 0);

	if (!ret)
	{
		if (cable->prepare)
		{
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_PREPARE);
			if (!ret)
			{
				ret = cable->prepare(handle);
			}
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_PREPARE);
		}

		if (!ret)
		{
			if (cable->open)
			{
				ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_OPEN);
				if (!ret)
				{
					ret = cable->open(handle);
				}
				ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_OPEN);
			}
			if (!ret)
			{
				handle->open = 1;
				START_LOGGING(handle);
			}
		}
	}

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_AFTER_OPEN, /* retval */ ret);

	return ret;
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
int TICALL ticables_cable_close(CableHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_CLOSE, /* retval */ 0);

	STOP_LOGGING(handle);
	if (!ret)
	{
		if (handle->open)
		{
			if (cable->close)
			{
				ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_CLOSE);
				if (!ret)
				{
					ret = cable->close(handle);
				}
				ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_CLOSE);
			}
			handle->open = 0;
			free(handle->device);
			handle->device = nullptr;
		}
	}

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_AFTER_CLOSE, /* retval */ ret);

	return ret;
}

/**
 * ticables_cable_reset:
 * @handle: a previously allocated handle
 *
 * Reset link cable status (flush buffers, set ready).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticables_cable_reset(CableHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_RESET, /* retval */ 0);

	if (!ret)
	{
		handle->busy = 1;
		if (cable->reset)
		{
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_RESET);
			if (!ret)
			{
				ret = cable->reset(handle);
			}
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_RESET);
		}
		handle->busy = 0;
	}

	ret = ticables_event_send_simple(handle, /* type */ CABLE_EVENT_TYPE_AFTER_RESET, /* retval */ ret);

	return ret;
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
int TICALL ticables_cable_probe(CableHandle* handle, int* result)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	const int opened = handle->open;

	// Check if device is already opened
	if (!opened && cable->need_open)
	{
		ret = ticables_cable_open(handle);
	}
	else if (!opened && !cable->need_open)
	{
		if (cable->prepare)
		{
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_PREPARE);
			if (!ret)
			{
				ret = cable->prepare(handle);
			}
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_PREPARE);
		}
	}

	if (!ret)
	{
		// Do the check itself
		if (cable->probe)
		{
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_PROBE);
			if (!ret)
			{
				ret = cable->probe(handle);
			}
			ret = ticables_event_send_simple_generic(handle, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_PROBE);
			if (result != nullptr)
			{
				*result = !ret;
			}
			else
			{
				ticables_critical("%s: result is NULL", __FUNCTION__);
			}

			// If it was opened for this, close it
			if (!opened && cable->need_open)
			{
				ret = ticables_cable_close(handle);
			}
			else if (!opened && !cable->need_open)
			{
				free(handle->device); handle->device = nullptr;
				free(handle->priv2); handle->priv2 = nullptr;
			}
		}
	}

	return ret;
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
int TICALL ticables_cable_send(CableHandle* handle, uint8_t *data, uint32_t len)
{
	int ret = 0;
	CableEventData event;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	if (!len)
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

	ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_SEND, /* retval */ 0, /* operation */ CABLE_FNCT_LAST);
	ticables_event_fill_data(&event, /* data */ data, /* len */ len);
	ret = ticables_event_send(handle, &event);

	if (!ret)
	{
		handle->busy = 1;
		if (data != nullptr)
		{
			handle->rate.count += len;
			if (cable->send)
			{
				ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_SEND);
				ticables_event_fill_data(&event, /* data */ data, /* len */ len);
				ret = ticables_event_send(handle, &event);
				if (!ret)
				{
					ret = cable->send(handle, data, len);
				}
				ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_SEND);
				ticables_event_fill_data(&event, /* data */ data, /* len */ len);
				ret = ticables_event_send(handle, &event);
			}
		}
		else
		{
			ticables_critical("%s: data is NULL", __FUNCTION__);
		}
		handle->busy = 0;
	}

	ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_SEND, /* retval */ ret, /* operation */ CABLE_FNCT_LAST);
	ticables_event_fill_data(&event, /* data */ data, /* len */ len);
	ret = ticables_event_send(handle, &event);

	return ret;
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
int TICALL ticables_cable_recv(CableHandle* handle, uint8_t *data, uint32_t len)
{
	int ret = 0;
	CableEventData event;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	if (!len)
	{
		// See ticables_cable_send above.
		ticables_critical("ticables_cable_recv: len = 0\n");
	}

	ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_RECV, /* retval */ 0, /* operation */ CABLE_FNCT_LAST);
	ticables_event_fill_data(&event, /* data */ data, /* len */ len);
	ret = ticables_event_send(handle, &event);
	if (!ret)
	{
		handle->busy = 1;
		if (data != nullptr)
		{
			handle->rate.count += len;
			if (cable->recv)
			{
				ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_RECV);
				ticables_event_fill_data(&event, /* data */ data, /* len */ len);
				ret = ticables_event_send(handle, &event);
				if (!ret)
				{
					ret = cable->recv(handle, data, len);
				}
				ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_RECV);
				ticables_event_fill_data(&event, /* data */ data, /* len */ len);
				ret = ticables_event_send(handle, &event);
			}
		}
		else
		{
			ticables_critical("%s: data is NULL", __FUNCTION__);
		}
		handle->busy = 0;
	}

	ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_RECV, /* retval */ ret, /* operation */ CABLE_FNCT_LAST);
	ticables_event_fill_data(&event, /* data */ data, /* len */ len);
	ret = ticables_event_send(handle, &event);

	return ret;
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
int TICALL ticables_cable_check(CableHandle* handle, CableStatus *status)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (status != nullptr)
	{
		if (cable->check)
		{
			CableEventData event;
			ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_CHECK);
			event.data.intval = 0;
			ret = ticables_event_send(handle, &event);
			if (!ret)
			{
				ret = cable->check(handle, (int *)status);
			}
			ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_CHECK);
			event.data.intval = *status;
			ret = ticables_event_send(handle, &event);
		}
	}
	else
	{
		ticables_critical("%s: status is NULL", __FUNCTION__);
	}
	handle->busy = 0;

	return ret;
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
int TICALL ticables_cable_set_d0(CableHandle* handle, int state)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->set_d0)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_SET_D0);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->set_d0(handle, state);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_SET_D0);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
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
int TICALL ticables_cable_set_d1(CableHandle* handle, int state)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->set_d1)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_SET_D1);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->set_d1(handle, state);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_SET_D1);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_get_d0:
 * @handle: a previously allocated handle
 *
 * Get the electrical state of the D0 wire (if possible).
 *
 * Return value: 0 or 1.
 **/
int TICALL ticables_cable_get_d0(CableHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->get_d0)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_GET_D0);
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->get_d0(handle);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_GET_D0);
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_get_d1:
 * @handle: a previously allocated handle
 *
 * Get the electrical state of the D1 wire (if possible).
 *
 * Return value: 0 or 1.
 **/
int TICALL ticables_cable_get_d1(CableHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->get_d1)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_GET_D1);
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->get_d1(handle);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_GET_D1);
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
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
int TICALL ticables_cable_set_raw(CableHandle* handle, int state)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);
	if (!cable->set_raw)
	{
		return ERR_RAW_IO_UNSUPPORTED;
	}

	handle->busy = 1;
	if (cable->set_raw)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_SET_RAW);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->set_raw(handle, state);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_SET_RAW);
		event.data.intval = state;
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
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
int TICALL ticables_cable_get_raw(CableHandle* handle, int *state)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(state);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);
	if (!cable->get_raw)
	{
		return ERR_RAW_IO_UNSUPPORTED;
	}

	handle->busy = 1;
	if (cable->get_raw)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_GET_RAW);
		event.data.intval = 0;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->get_raw(handle, state);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_GET_RAW);
		event.data.intval = *state;
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_set_device:
 * @handle: a previously allocated handle
 * @device: new device information
 *
 * Sets cable device if that makes sense.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticables_cable_set_device(CableHandle* handle, const char * device)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(device);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->set_device)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_SET_DEVICE);
		event.data.cptrval = (const void *)device;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = cable->set_device(handle, device);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_SET_DEVICE);
		event.data.cptrval = (const void *)device;
		ret = ticables_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_get_device_info:
 * @handle: a previously allocated handle
 * @info: pointer to structure to store device info
 *
 * Get the type of device on the other end of the cable, if this can
 * be determined.
 */
int TICALL ticables_cable_get_device_info(CableHandle *handle, CableDeviceInfo *info)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(info);

	const CableFncts * cable = handle->cable;
	VALIDATE_CABLEFNCTS(cable);

	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	handle->busy = 1;
	if (cable->get_device_info)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ CABLE_FNCT_GET_DEVICE_INFO);
		event.data.ptrval = (void *)info;
		ret = ticables_event_send(handle, &event);
		if (!ret)
		{
			ret = (cable->get_device_info)(handle, info);
		}
		ticables_event_fill_header(handle, &event, /* type */ CABLE_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ CABLE_FNCT_GET_DEVICE_INFO);
		event.data.ptrval = (void *)info;
		ret = ticables_event_send(handle, &event);
	}
	else
	{
		info->family = CABLE_FAMILY_DBUS;
		info->variant = CABLE_VARIANT_UNKNOWN;
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_progress_reset:
 * @handle: a previously allocated handle
 *
 * Reset byte counter and timer used for computing data rate.
 *
 * Return value: always 0.
 **/
int TICALL ticables_progress_reset(CableHandle* handle)
{
	VALIDATE_HANDLE(handle);

	handle->rate.count = 0;
	TO_START(handle->rate.start);

	return 0;
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
int TICALL ticables_progress_get(CableHandle* handle, int* count, int* msec, float* rate)
{
	VALIDATE_HANDLE(handle);

	TO_START(handle->rate.current);

	if (count)
	{
		*count = handle->rate.count;
	}

	if (msec)
	{
		*msec = handle->rate.current - handle->rate.start;
	}

	if (rate)
	{
		if (handle->rate.current > handle->rate.start)
			*rate = (float)handle->rate.count / ((float)(handle->rate.current - handle->rate.start));
	}

#if 0
	if (handle->rate.current > handle->rate.start)
		printf("<%u %u %u %u %f\n",
			handle->rate.count, handle->rate.start, handle->rate.current,
			handle->rate.current - handle->rate.start, (float)handle->rate.count / ((float)(handle->rate.current - handle->rate.start)));
#endif

	return 0;
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
int TICALL ticables_cable_put(CableHandle* handle, uint8_t data)
{
	return ticables_cable_send(handle, &data, 1);
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
int TICALL ticables_cable_get(CableHandle* handle, uint8_t *data)
{
	return ticables_cable_recv(handle, data, 1);
}

/**
 * ticables_cable_get_pre_send_hook:
 *
 * Get the current pre send hook function pointer.
 *
 * Return value: a function pointer.
 */
ticables_pre_send_hook_type TICALL ticables_cable_get_pre_send_hook(CableHandle *handle)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_set_pre_send_hook:
 * @hook: new pre send hook
 *
 * Set the current pre send hook function pointer.
 *
 * Return value: the previous pre send hook, so that the caller can use it to chain hooks.
 */
ticables_pre_send_hook_type TICALL ticables_cable_set_pre_send_hook(CableHandle *handle, ticables_pre_send_hook_type hook)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_get_post_send_hook:
 *
 * Get the current post send hook function pointer.
 *
 * Return value: a function pointer.
 */
ticables_post_send_hook_type TICALL ticables_cable_get_post_send_hook(CableHandle *handle)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_set_post_send_hook:
 * @hook: new post send hook
 *
 * Set the current post send hook function pointer.
 *
 * Return value: the previous post send hook, so that the caller can use it to chain hooks.
 */
ticables_post_send_hook_type TICALL ticables_cable_set_post_send_hook(CableHandle *handle, ticables_post_send_hook_type hook)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_get_pre_recv_hook:
 *
 * Get the current pre recv hook function pointer.
 *
 * Return value: a function pointer.
 */
ticables_pre_recv_hook_type TICALL ticables_cable_get_pre_recv_hook(CableHandle *handle)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_set_pre_recv_hook:
 * @hook: new pre recv hook
 *
 * Set the current pre recv hook function pointer.
 *
 * Return value: the previous pre recv hook, so that the caller can use it to chain hooks.
 */
ticables_pre_recv_hook_type TICALL ticables_cable_set_pre_recv_hook(CableHandle *handle, ticables_pre_recv_hook_type hook)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_get_post_recv_hook:
 *
 * Get the current post recv hook function pointer.
 *
 * Return value: a function pointer.
 */
ticables_post_recv_hook_type TICALL ticables_cable_get_post_recv_hook(CableHandle *handle)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_set_post_recv_hook:
 * @hook: new post recv hook
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
ticables_post_recv_hook_type TICALL ticables_cable_set_post_recv_hook(CableHandle *handle, ticables_post_recv_hook_type hook)
{
	ticables_critical("%s: deprecated function, does nothing anymore", __FUNCTION__);

	return nullptr;
}

/**
 * ticables_cable_get_event_hook:
 *
 * Get the current event hook function pointer.
 *
 * Return value: a function pointer.
 */
ticables_event_hook_type TICALL ticables_cable_get_event_hook(CableHandle *handle)
{
	if (!ticables_validate_handle(handle))
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	return handle->event_hook;
}

/**
 * ticables_cable_set_post_recv_hook:
 * @hook: new post recv hook
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
ticables_event_hook_type TICALL ticables_cable_set_event_hook(CableHandle *handle, ticables_event_hook_type hook)
{
	if (!ticables_validate_handle(handle))
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	const ticables_event_hook_type old_hook = handle->event_hook;
	handle->event_hook = hook;

	return old_hook;
}

/**
 * ticables_cable_get_event_user_pointer:
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
void * ticables_cable_get_event_user_pointer(CableHandle *handle)
{
	if (!ticables_validate_handle(handle))
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	return handle->user_pointer;
}

/**
 * ticables_cable_set_event_user_pointer:
 * @user_pointer: new user pointer
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
void * ticables_cable_set_event_user_pointer(CableHandle *handle, void * user_pointer)
{
	if (!ticables_validate_handle(handle))
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	void* old_pointer = handle->user_pointer;
	handle->user_pointer = user_pointer;

	return old_pointer;
}

/**
 * ticables_cable_get_event_count:
 *
 * Get the current event count since the handle was initialized.
 *
 * Return value: an unsigned integer.
 */
uint32_t TICALL ticables_cable_get_event_count(CableHandle *handle)
{
	if (!ticables_validate_handle(handle))
	{
		ticables_critical("%s: handle is NULL", __FUNCTION__);
		return 0;
	}

	return handle->event_count;
}

/**
 * ticables_cable_fire_user_event:
 * @handle: a previously allocated handle.
 * @type: event type.
 * @user_data: user-specified data.
 * @user_len: user-specified length.
 *
 * Fire a user-specified event to the registered event hook function, if any.
 *
 * Return value: 0 if successful, an error code otherwise.
 */
int TICALL ticables_cable_fire_user_event(CableHandle *handle, CableEventType type, int retval, void * user_data, uint32_t user_len)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	if (handle->event_hook && type >= CABLE_EVENT_TYPE_USER)
	{
		CableEventData event;
		ticables_event_fill_header(handle, &event, /* type */ type, /* retval */ retval, /* operation */ CABLE_FNCT_LAST);
		event.data.user_data.data = (uint8_t *)user_data;
		event.data.user_data.len = user_len;
		handle->event_count++;
		ret = handle->event_hook(handle, handle->event_count, &event, handle->user_pointer);
	}

	return ret;
}
