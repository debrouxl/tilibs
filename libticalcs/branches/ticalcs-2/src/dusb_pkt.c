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

#define BLK_SIZE	255	// USB packets have this max length

int send_dusb(CalcHandle* handle, UsbPacket* pkt)
{
	int i, r, q;
	uint8_t* buf = NULL;

	ticables_progress_reset(handle->cable);

	q = 1;	//(length + 6) / BLK_SIZE;
	r = 1;	//(length + 6) % BLK_SIZE;

	handle->updat->max1 = 1;	//length + 6;
	handle->updat->cnt1 = 0;

	// send full chunks
	for(i = 0; i < q; i++)
	{
		TRYF(ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], BLK_SIZE));
		ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

		handle->updat->cnt1 += BLK_SIZE;
		handle->updat->pbar();

		if (handle->updat->cancel)
			return ERR_ABORT;
	}

	// send last chunk
	{
		TRYF(ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], (uint16_t)r));
		ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

		handle->updat->cnt1 += 1;
		handle->updat->pbar();
	}

	return 0;
}

int recv_dusb(CalcHandle* handle, UsbPacket* pkt)
{
	return 0;
}