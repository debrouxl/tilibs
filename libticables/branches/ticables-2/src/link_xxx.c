/* Hey EMACS -*- linux-c -*- */
/* $Id: ticables.c 990 2005-04-29 14:14:02Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#ifndef TRY
# define TRY(x) { int aaaa_; if((aaaa_ = (x))) return aaaa_; }	//new !
#endif

TIEXPORT int TICALL ticables_cable_open(TiHandle* handle)
{
	TiCable *cable = handle->cable;

	TRY(cable->open(handle));
	handle->open = 1;
	START_LOGGING();

	return 0;
}

TIEXPORT int TICALL ticables_cable_close(TiHandle* handle)
{
	TiCable *cable = handle->cable;

	STOP_LOGGING();
	cable->close(handle);
	handle->open = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_send(TiHandle* handle, uint8_t *data, uint16_t len)
{
	TiCable *cable = handle->cable;
	int ret, i;
	
	if (handle->open == 1 && handle->busy == 0 && len > 0)
		return ERR_BUSY;

	handle->busy = 1;
	handle->rate.count = len;
	toSTART(handle->rate.start);
	for(i = 0; i < len; i++)
	{
		LOG_DATA(data[i]);
		ret = cable->send(handle, data[i]);
		if(ret)
		{
			handle->busy = 0;
			return ret;
		}
	}	
	toCURRENT(handle->rate.current);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_recv(TiHandle* handle, uint8_t *data, uint16_t len)
{
	TiCable *cable = handle->cable;
	int ret, i;
	
	if (handle->open == 1 && handle->busy == 0 && len > 0)
		return ERR_BUSY;

	handle->busy = 1;
	handle->rate.count = len;
	toSTART(handle->rate.start);
	for(i = 0; i < len; i++)
	{
		ret = cable->recv(handle, &data[i]);
		LOG_DATA(data[i]);
		if(ret)
		{
			handle->busy = 0;
			return ret;
		}
	}
	toCURRENT(handle->rate.current);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_check(TiHandle* handle, TiCableStatus *status)
{
	TiCable *cable = handle->cable;
	int ret;
	
	if (handle->open == 1 && handle->busy)
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->check(handle, status);
	handle->busy = 0;

	return ret;
}

TIEXPORT int TICALL ticables_cable_set_d0(TiHandle* handle, int state)
{
	TiCable *cable = handle->cable;
	int ret;
	
	if (handle->open == 1 && handle->busy == 0 )
		return ERR_BUSY;

	handle->busy = 1;
	ret = cable->set_d0(handle, state);
	handle->busy = 0;

	return ret;
}

TIEXPORT int TICALL ticables_cable_set_d1(TiHandle* handle, int state)
{
	TiCable *cable = handle->cable;
	int ret;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	ret = cable->set_d1(handle, state);
	handle->busy = 0;

	return ret;
}

TIEXPORT int TICALL ticables_cable_get_d0(TiHandle* handle)
{
	TiCable *cable = handle->cable;
	int ret;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	ret = cable->get_d0(handle);
	handle->busy = 0;

	return ret;
}

TIEXPORT int TICALL ticables_cable_get_d1(TiHandle* handle)
{
	TiCable *cable = handle->cable;
	int ret;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	ret = cable->get_d1(handle);
	handle->busy = 0;

	return ret;
}

TIEXPORT int TICALL ticables_cable_progress(TiHandle* handle, int *count, int *msec)
{
	*count = handle->rate.count;
	*msec = 1000 * toELAPSED(handle->rate.start, handle->rate.current);

	return 0;
}
