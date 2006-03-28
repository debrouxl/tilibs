/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
  This unit handles Titanium commands with DirectLink.
*/

#include <string.h>

#include "ticalcs.h"
#include "dusb_pkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#define BLK_SIZE	1023	// USB packets have this max length
#define PH_SIZE		5		// packet header size
#define DH_SIZE		6		// data header size
#define DATA_SIZE	1018	// max length of data (BLK_SIZE - PH_SIZE)

// upper layer: formats packets with type and split

int ti89t_send_handshake(CalcHandle* h)
{
	UsbPacket pkt = { 0 };

	pkt.size = 4;
	pkt.type = PKT_HANDSHAKE;
	pkt.data[2] = 0x04;
	pkt.data[3] = 0x00;

	TRYF(dusb_send(h, &pkt));

	return 0;
}

int ti89t_recv_response(CalcHandle *h)
{
	UsbPacket pkt = { 0 };

	TRYF(dusb_recv(h, &pkt));
	
	if(pkt.size != 4)
		return ERR_INVALID_PACKET;

	if(pkt.type != PKT_RESPONSE)
		return ERR_INVALID_PACKET;

	if(pkt.data[2] != 0x03 && pkt.data[3] != 0xff)
		return ERR_INVALID_PACKET;

	return 0;
}

int ti89t_send_acknowledge(CalcHandle* h)
{
	UsbPacket pkt = { 0 };

	pkt.size = 2;
	pkt.type = PKT_ACK;
	pkt.data[0] = 0x00;
	pkt.data[1] = 0x00;

	TRYF(dusb_send(h, &pkt));

	return 0;
}

int ti89t_recv_acknowledge(CalcHandle *h)
{
	UsbPacket pkt = { 0 };

	TRYF(dusb_recv(h, &pkt));
	
	pkt.size = pkt.size;
	if(pkt.size != 2)
		return ERR_INVALID_PACKET;

	if(pkt.type != PKT_ACK)
		return ERR_INVALID_PACKET;

	if(pkt.data[0] != 0xe0 && pkt.data[1] != 0x00)
		return ERR_INVALID_PACKET;

	return 0;
}

int ti89t_send_data(CalcHandle *h, uint32_t  size, uint16_t  code, uint8_t *data)
{
	UsbPacket pkt = { 0 };
	int i, r, q;

	if(size <= DATA_SIZE - DH_SIZE)
	{
		// we have a single packet which is the last one, too
		pkt.size = size + DH_SIZE;
		pkt.type = PKT_LAST;
		pkt.hdr.size = GUINT32_TO_BE(size);
		pkt.hdr.code = GUINT16_TO_BE(code);
		memcpy(&pkt.data[DH_SIZE], data, size);
	
		TRYF(dusb_send(h, &pkt));
		TRYF(ti89t_recv_acknowledge(h));
	}
	else
	{
		// we have more than one packet: first packet have data header
		pkt.size = DATA_SIZE;
		pkt.type = PKT_DATA;
		pkt.hdr.size = GUINT32_TO_BE(size);
		pkt.hdr.code = GUINT16_TO_BE(code);
		memcpy(&pkt.data[DH_SIZE], data, DATA_SIZE);

		TRYF(dusb_send(h, &pkt));
		TRYF(ti89t_recv_acknowledge(h));

		// other packets doesn't have data header but last one has a different type
		q = size / DATA_SIZE;
		r = size % DATA_SIZE;

		// send full chunks (no header)
		for(i = 1; i < q; i++)
		{
			pkt.size = DATA_SIZE;
			pkt.type = PKT_DATA;
			memcpy(pkt.data, data + i*DATA_SIZE, DATA_SIZE);

			TRYF(dusb_send(h, &pkt));
			TRYF(ti89t_recv_acknowledge(h));
		}

		// send last chunk (type)
		{
			pkt.size = r;
			pkt.type = PKT_LAST;
			memcpy(pkt.data, data + i*DATA_SIZE, r);
			
			TRYF(dusb_send(h, &pkt));
			TRYF(ti89t_recv_acknowledge(h));
		}
	}

	return 0;
}

int ti89t_recv_data(CalcHandle *h, uint32_t *size, uint16_t *code, uint8_t *data)
{
	UsbPacket pkt = { 0 };
	int i;

	i = 0;
	do
	{
		TRYF(dusb_recv(h, &pkt));
		if(pkt.type != PKT_DATA && pkt.type != PKT_LAST)
			return ERR_INVALID_PACKET;
		TRYF(ti89t_send_acknowledge(h));

		if(!i)
		{
			// first packet has a data header
			*size = GUINT32_FROM_BE(pkt.hdr.size);
			*code = GUINT16_FROM_BE(pkt.hdr.code);
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
