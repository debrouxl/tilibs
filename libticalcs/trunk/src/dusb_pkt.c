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

#define BLK_SIZE	255		// USB packets have this max length
#define PH_SIZE		5		// packet header size
#define DH_SIZE		6		// data header size
#define DATA_SIZE	250		// max length of data

// lower layer: transmit packets formatted with an header

int dusb_send(CalcHandle* handle, UsbPacket* pkt)
{
	uint8_t buf[256]= { 0 };
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
	uint8_t buf[256];

	// Any packet has always an header of 5 bytes (size & type)
	TRYF(ticables_cable_recv(handle->cable, buf, 5));

	pkt->size = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
	pkt->type = buf[4];

	if(pkt->size > 250)
		return ERR_INVALID_PACKET;

	// Next, follows data
	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_recv(handle->cable, pkt->data, pkt->size));
	ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			
	if (handle->updat->cancel)
		return ERR_ABORT;

	return 0;
}

// upper layer: formats packets with type and split

int dusb_send_handshake(CalcHandle* h)
{
	UsbPacket pkt = { 0 };

	pkt.size = 4;
	pkt.type = PKT_HANDSHAKE;
	pkt.data[2] = 0x04;
	pkt.data[3] = 0x00;

	TRYF(dusb_send(h, &pkt));

	return 0;
}

int dusb_recv_response(CalcHandle *h)
{
	UsbPacket pkt = { 0 };

	TRYF(dusb_recv(h, &pkt));
	
	if(pkt.size != 4)
		return ERR_INVALID_PACKET;

	if(pkt.type != PKT_RESPONSE)
		return ERR_INVALID_PACKET;

	if(pkt.data[3] != 0xfa)
		return ERR_INVALID_PACKET;

	return 0;
}

int dusb_send_data(CalcHandle *h, uint32_t  size, uint16_t  code, uint8_t *data)
{
	UsbPacket pkt = { 0 };
	int i, r, q;

	if(size <= DATA_SIZE - DH_SIZE)
	{
		// we have a single packet which is the last one, too
		pkt.size = size + PH_SIZE;
		pkt.type = PKT_LAST;
		pkt.hdr.size = size;
		pkt.hdr.code = code;
		memcpy(&pkt.data[DH_SIZE], data, size);
	
		TRYF(dusb_send(h, &pkt));
	}
	else
	{
		// we have more than one packet: first packet have data header
		pkt.size = DATA_SIZE + PH_SIZE;
		pkt.type = PKT_DATA;
		pkt.hdr.size = size;
		pkt.hdr.code = code;
		memcpy(&pkt.data[DH_SIZE], data, DATA_SIZE);

		TRYF(dusb_send(h, &pkt));

		// other packets doesn't have data header but last one has a different type
		q = size / DATA_SIZE;
		r = size % DATA_SIZE;

		// send full chunks (no header)
		for(i = 1; i < q; i++)
		{
			pkt.size = DATA_SIZE + PH_SIZE;
			pkt.type = PKT_DATA;
			memcpy(pkt.data, data + i*DATA_SIZE, DATA_SIZE);

			TRYF(dusb_send(h, &pkt));
		}

		// send last chunk (type)
		{
			pkt.size = r + PH_SIZE;
			pkt.type = PKT_LAST;
			memcpy(pkt.data, data + i*DATA_SIZE, r);
			
			TRYF(dusb_send(h, &pkt));
		}
	}

	return 0;
}

int dusb_recv_data(CalcHandle *h, uint32_t *size, uint16_t *code, uint8_t *data)
{
	UsbPacket pkt = { 0 };
	int i;

	i = 0;
	do
	{
		TRYF(dusb_recv(h, &pkt));
		if(pkt.type != PKT_DATA && pkt.type != PKT_LAST)
			return ERR_INVALID_PACKET;

		if(!i)
		{
			// first packet has a data header
			*size = pkt.hdr.size;
			*code = pkt.hdr.code;
			memcpy(data, pkt.data, pkt.size - DH_SIZE);
		}
		else
		{
			// others have more data
			memcpy(data, pkt.data, pkt.size);
		}

		i++;

	} while(pkt.type != PKT_LAST);

	return 0;
}

int dusb_send_acknowledge(CalcHandle* h)
{
	UsbPacket pkt = { 0 };

	pkt.size = 2;
	pkt.type = PKT_ACK;
	pkt.data[0] = 0x00;
	pkt.data[1] = 0x00;

	TRYF(dusb_send(h, &pkt));

	return 0;
}

int dusb_recv_acknowledge(CalcHandle *h)
{
	UsbPacket pkt = { 0 };

	TRYF(dusb_recv(h, &pkt));
	
	pkt.size = pkt.size;
	if(pkt.size != 2)
		return ERR_INVALID_PACKET;

	if(pkt.type != PKT_ACK)
		return ERR_INVALID_PACKET;

	if(pkt.data[0] != 0x00 && pkt.data[1] != 0x00)
		return ERR_INVALID_PACKET;

	return 0;
}