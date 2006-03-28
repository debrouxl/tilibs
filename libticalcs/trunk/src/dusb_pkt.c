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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	This unit manages packets from/to D-USB (DirectLink).
*/

#include <string.h>

#include "ticalcs.h"
#include "dusb_pkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#define BUF_SIZE	1024

int dusb_send(CalcHandle* handle, UsbPacket* pkt)
{
	uint8_t buf[BUF_SIZE]= { 0 };
	uint32_t size = pkt->size + 5;

	buf[0] = MSB(MSW(pkt->size));
	buf[1] = LSB(MSW(pkt->size));
	buf[2] = MSB(LSW(pkt->size));
	buf[3] = LSB(LSW(pkt->size));
	buf[4] = pkt->type;
	memcpy(buf+5, pkt->data, pkt->size);

	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_send(handle->cable, buf, size));
	ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

	if (handle->updat->cancel)
		return ERR_ABORT;

	return 0;
}

int dusb_recv(CalcHandle* handle, UsbPacket* pkt)
{
	uint8_t buf[BUF_SIZE];

	// Any packet has always an header of 5 bytes (size & type)
	TRYF(ticables_cable_recv(handle->cable, buf, 5));

	pkt->size = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
	pkt->type = buf[4];

	if(handle->model == CALC_TI84P_USB && pkt->size > 250)
		return ERR_INVALID_PACKET;
	if(handle->model == CALC_TI89T_USB && pkt->size > 1023)
		return ERR_INVALID_PACKET;

	// Next, follows data
	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_recv(handle->cable, pkt->data, pkt->size));
	ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			
	if (handle->updat->cancel)
		return ERR_ABORT;

	return 0;
}
