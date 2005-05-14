/* Hey EMACS -*- linux-c -*- */
/* $Id: link_xxx.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
TIEXPORT int TICALL ticables_cable_open(CableHandle* handle)
{
	const CableFncts *cable = handle->cable;

	TRYC(handle->cable->prepare(handle));
	
	TRYC(cable->open(handle));
	
	handle->open = 1;
	START_LOGGING();

	return 0;
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
TIEXPORT int TICALL ticables_cable_probe(CableHandle* handle, unsigned int* result)
{
	const CableFncts *cable = handle->cable;
	int already = handle->open;
	int ret;

	// Check if device is already opened
	if(!already && cable->need_open)
	    TRYC(ticables_cable_open(handle));

	// Do the check itself
	ret = cable->probe(handle);
	if(ret == ERR_PROBE_FAILED)
		*result = 0;
	else
		*result = !0;

	// If it was opened for this, close it
	if(!already && cable->need_open)
	    TRYC(ticables_cable_close(handle));

	return 0;
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
TIEXPORT int TICALL ticables_cable_close(CableHandle* handle)
{
	const CableFncts *cable = handle->cable;

	STOP_LOGGING();
	if(handle->open)
	{
	    cable->close(handle);
	    handle->open = 0;
	}

	return 0;
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
TIEXPORT int TICALL ticables_cable_send(CableHandle* handle, uint8_t *data, uint16_t len)
{
	const CableFncts *cable = handle->cable;
	int ret;

	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;
	if(!len)
		return 0;

	handle->busy = 1;
	handle->rate.count = len;
	TO_START(handle->rate.start);
	ret = cable->send(handle, data, len);
	TO_CURRENT(handle->rate.current);
	handle->busy = 0;

	return 0;
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
TIEXPORT int TICALL ticables_cable_recv(CableHandle* handle, uint8_t *data, uint16_t len)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;
	if(!len)
		return 0;

	handle->busy = 1;
	handle->rate.count = len;
	TO_START(handle->rate.start);
	ret = cable->recv(handle, data, len);
	TO_CURRENT(handle->rate.current);
	handle->busy = 0;

	return 0;
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
TIEXPORT int TICALL ticables_cable_check(CableHandle* handle, CableStatus *status)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->check(handle, (int *)status);
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
TIEXPORT int TICALL ticables_cable_set_d0(CableHandle* handle, int state)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->set_d0(handle, state);
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
TIEXPORT int TICALL ticables_cable_set_d1(CableHandle* handle, int state)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->set_d1(handle, state);
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
TIEXPORT int TICALL ticables_cable_get_d0(CableHandle* handle)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->get_d0(handle);
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
TIEXPORT int TICALL ticables_cable_get_d1(CableHandle* handle)
{
	const CableFncts *cable = handle->cable;
	int ret;
	
	if(!handle->open)
		return -1;
	if(handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->get_d1(handle);
	handle->busy = 0;

	return ret;
}

/**
 * ticables_cable_progress:
 * @handle: a previously allocated handle
 * @count: number of bytes transfered
 * @msec: time needed for the operation
 *
 * Returns informations needed to compute the transfer rate of the link cable.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT int TICALL ticables_cable_progress(CableHandle* handle, int *count, int *msec)
{
	*count = handle->rate.count;
	*msec = 1000 * TO_ELAPSED(handle->rate.start, handle->rate.current);

	return 0;
}
