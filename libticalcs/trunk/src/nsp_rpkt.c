/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.c 1404 2005-07-20 20:39:39Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 2007 Romain Liévin
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
	This unit manages raw packets from/to NSpire thru DirectLink cable.
	Documentation & credits can be found at <http://hackspire.unsads.com/USB_Protocol>.
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "nsp_rpkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#define VPKT_DBG	1	// 1 = verbose, 2 = more verbose

// CRC implementation from O. Armand (ExtendeD)
static uint16_t compute_crc(uint8_t *data, uint32_t size)
{
	uint16_t acc = 0;
	int i;

	if(size == 0)
		return 0;

	for(i = 0; i < (int)size; i++)
	{
		uint16_t first, second, third;

		first = (data[i] << 8) | (acc >> 8); 
		acc &= 0xff;
		second = (((acc & 0x0f) << 4) ^ acc) << 8;
		third = second >> 5;
		acc = third >> 7;
		acc = (acc ^ first ^ second ^ third);
	}

	return acc;
}

static int hexdump(uint8_t *data, uint32_t size)
{
#if (VPKT_DBG == 2)
	char *str = (char *)g_malloc(3*size + 8 + 10);
	int i, j;
	int step = 12;
  
	for(i = 0; i < 4; i++)
		str[i] = ' ';

	for (i = j = 0; i < (int)size; i++, j++)
	{
		sprintf(&str[3*j+4], "%02X ", data[i]);
		if(!(i % step) && i)
		{
			ticalcs_info(str);
			j = 0;
		}
	}
	
	g_free(str);
#endif
  return 0;
}

uint8_t		nsp_seq = 1;
/*
void nsp_inc_seq(void)
{
	nsp_seq++;
}
*/

int nsp_send(CalcHandle* handle, RawPacket* pkt)
{
	uint8_t buf[sizeof(RawPacket)] = { 0 };
	uint32_t size = pkt->data_size + HEADER_SIZE;
	
	pkt->data_sum = compute_crc(pkt->data, pkt->data_size);

	if(!nsp_seq) nsp_seq++;
	pkt->seq = nsp_seq;

	ticalcs_info("   %04x:%04x->%04x:%04x AK=%02x SQ=%02x HC=%02x DC=%04x (%i bytes)", 
			pkt->src_addr, pkt->src_port, pkt->dst_addr, pkt->dst_port, 
			pkt->ack, pkt->seq, pkt->hdr_sum, pkt->data_sum, pkt->data_size);
	if(pkt->data_size)
		hexdump(pkt->data, pkt->data_size);
	
	buf[0] = 0x54;
	buf[1] = 0xFD;
	buf[2] = MSB(pkt->src_addr);
	buf[3] = LSB(pkt->src_addr);
	buf[4] = MSB(pkt->src_port);
	buf[5] = LSB(pkt->src_port);
	buf[6] = MSB(pkt->dst_addr);
	buf[7] = LSB(pkt->dst_addr);
	buf[8] = MSB(pkt->dst_port);
	buf[9] = LSB(pkt->dst_port);
	buf[10] = MSB(pkt->data_sum);
	buf[11] = LSB(pkt->data_sum);
	buf[12] = pkt->data_size;
	buf[13] = pkt->ack;
	buf[14] = pkt->seq;
	buf[15] = pkt->hdr_sum = tifiles_checksum(buf, HEADER_SIZE-1) & 0xff;

	memcpy(buf + HEADER_SIZE, pkt->data, pkt->data_size);

	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_send(handle->cable, buf, size));
	if(size >= 128)
		ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

	if (handle->updat->cancel)
		return ERR_ABORT;

	return 0;
}

int nsp_recv(CalcHandle* handle, RawPacket* pkt)
{
	uint8_t buf[HEADER_SIZE];

	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_recv(handle->cable, buf, HEADER_SIZE));

	pkt->unused		= (buf[0] << 8) | buf[1];
	pkt->src_addr	= (buf[2] << 8) | buf[3];
	pkt->src_port	= (buf[4] << 8) | buf[5];
	pkt->dst_addr	= (buf[6] << 8) | buf[7];
	pkt->dst_port	= (buf[8] << 8) | buf[9];
	pkt->data_sum	= (buf[10] << 8) | buf[11];
	pkt->data_size	= buf[12];
	pkt->ack		= buf[13];
	pkt->seq		= buf[14];
	pkt->hdr_sum	= buf[15];

	// Next, follows data
	if(pkt->data_size)
	{
		TRYF(ticables_cable_recv(handle->cable, pkt->data, pkt->data_size));
		if(pkt->data_size >= 128)
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
	}
			
	if (handle->updat->cancel)
		return ERR_ABORT;

	ticalcs_info("   %04x:%04x->%04x:%04x AK=%02x SQ=%02x HC=%02x DC=%04x (%i bytes)", 
			pkt->src_addr, pkt->src_port, pkt->dst_addr, pkt->dst_port, 
			pkt->ack, pkt->seq, pkt->hdr_sum, pkt->data_sum, pkt->data_size);
	if(pkt->data_size)
		hexdump(pkt->data, pkt->data_size);

	return 0;
}

