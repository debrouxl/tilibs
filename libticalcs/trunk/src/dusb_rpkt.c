/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.c 1404 2005-07-20 20:39:39Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

/*
	This unit manages raw packets from/to D-USB (DirectLink).
*/

#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dusb_rpkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

TIEXPORT3 int TICALL dusb_send(CalcHandle* handle, DUSBRawPacket* pkt)
{
	uint8_t buf[1023 + 5];
	uint32_t size;
	int ret;

	VALIDATE_HANDLE(handle)
	VALIDATE_NONNULL(pkt)

	memset(buf, 0, sizeof(buf));
	size = pkt->size;

	if (size > 1023)
	{
		size = 1023;
	}

	buf[0] = MSB(MSW(size));
	buf[1] = LSB(MSW(size));
	buf[2] = MSB(LSW(size));
	buf[3] = LSB(LSW(size));
	buf[4] = pkt->type;
	memcpy(buf+5, pkt->data, size);

	//printf("dusb_send: pkt->size=%d\n", pkt->size);
	ticables_progress_reset(handle->cable);
	ret = ticables_cable_send(handle->cable, buf, size + 5);
	if (!ret)
	{
		if (size >= 128)
		{
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
		}

		if (handle->updat->cancel)
		{
			ret = ERR_ABORT;
		}
	}

	return ret;
}

TIEXPORT3 int TICALL dusb_recv(CalcHandle* handle, DUSBRawPacket* pkt)
{
	uint8_t buf[5];
	int ret;

	VALIDATE_HANDLE(handle)
	VALIDATE_NONNULL(pkt)

	// Any packet has always an header of 5 bytes (size & type)
	ticables_progress_reset(handle->cable);
	ret = ticables_cable_recv(handle->cable, buf, 5);
	while (!ret)
	{

		pkt->size = buf[3] | (((uint32_t)buf[2]) << 8) | (((uint32_t)buf[1]) << 16) | (((uint32_t)buf[0]) << 24);
		pkt->type = buf[4];

		if (   (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB)
		    && pkt->size > 250)
		{
			ticalcs_warning("Raw packet is unexpectedly large: %u bytes", pkt->size);
		}
		else if (   (handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB)
		         && pkt->size > 1018)
		{
			ticalcs_warning("Raw packet is unexpectedly large: %u bytes", pkt->size);
		}
		else if (handle->model == CALC_TI89T_USB)
		{
			// Fall through.
		}
		// else do nothing for now.

		if (pkt->size > sizeof(pkt->data))
		{
			ticalcs_critical("Raw packet is too large: %u bytes", pkt->size);
			ret = ERR_INVALID_PACKET;
			break;
		}

		//printf("dusb_send: pkt->size=%d\n", pkt->size);
		// Next, follows data
		ret = ticables_cable_recv(handle->cable, pkt->data, pkt->size);
		if (!ret)
		{
			if (pkt->size >= 128)
			{
				ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			}

			if (handle->updat->cancel)
			{
				ret = ERR_ABORT;
			}
		}
		break;
	}

	return ret;
}
