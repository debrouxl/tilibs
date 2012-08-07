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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	This unit manages raw packets from/to NSpire thru DirectLink cable.
	Documentation & credits can be found at <http://hackspire.unsads.com/USB_Protocol>.
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#define VPKT_DBG	1	// 1 = verbose, 2 = more verbose

// CRC implementation from O. Armand (ExtendeD)
static uint16_t compute_crc(uint8_t *data, uint32_t size)
{
	uint16_t acc = 0;
	uint32_t i;

	if(size == 0)
		return 0;

	for(i = 0; i < size; i++)
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
#if (VPKT_DBG == 1)
	char str[64];
	uint32_t i;

	str[0] = 0;
	if (size <= 12)
	{
		str[0] = ' '; str[1] = ' '; str[2] = ' '; str[3] = ' ';

		for (i = 0; i < size; i++)
		{
			sprintf(&str[3*i+4], "%02X ", data[i]);
		}
	}
	else
	{
		sprintf(str, "    %02X %02X %02X %02X %02X ..... %02X %02X %02X %02X %02X",
		             data[0], data[1], data[2], data[3], data[4],
		             data[size-5], data[size-4], data[size-3], data[size-2], data[size-1]);
	}
	ticalcs_info(str);
#endif
#if (VPKT_DBG == 2)
	char *str = (char *)g_malloc(3*size + 8 + 10);
	uint32_t i, j, k;
	int step = 12;

	for(k = 0; k < 4; k++)
	{
		str[k] = ' ';
	}

	for (i = j = 0; i < size; i++, j++)
	{
		if(i && !(i % step))
		{
			ticalcs_info(str);
			j = 0;
		}

		sprintf(&str[3*j+4], "%02X ", data[i]);
	}
	ticalcs_info(str);

	g_free(str);
#endif
  return 0;
}

uint8_t		nsp_seq_ti;
uint8_t		nsp_seq_pc;
uint8_t		nsp_seq;

TIEXPORT3 int TICALL nsp_send(CalcHandle* handle, NSPRawPacket* pkt)
{
	uint8_t buf[sizeof(NSPRawPacket)] = { 0 };
	uint32_t size;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (pkt == NULL)
	{
		ticalcs_critical("%s: pkt is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	size = pkt->data_size + NSP_HEADER_SIZE;
	pkt->data_sum = compute_crc(pkt->data, pkt->data_size);

	if(pkt->src_port == 0x00fe || pkt->src_port == 0x00ff || pkt->src_port == 0x00d3)
	{
		pkt->ack = 0x0a;
		pkt->seq = nsp_seq;
	}
	else
	{
		if(!nsp_seq_pc) nsp_seq_pc++;
		pkt->seq = nsp_seq_pc;
	}

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
	buf[15] = pkt->hdr_sum = tifiles_checksum(buf, NSP_HEADER_SIZE-1) & 0xff;

	memcpy(buf + NSP_HEADER_SIZE, pkt->data, pkt->data_size);

	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_send(handle->cable, buf, size));
	if(size >= 128)
		ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

	if (handle->updat->cancel)
		return ERR_ABORT;

	return 0;
}

TIEXPORT3 int TICALL nsp_recv(CalcHandle* handle, NSPRawPacket* pkt)
{
	uint8_t buf[NSP_HEADER_SIZE];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (pkt == NULL)
	{
		ticalcs_critical("%s: pkt is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	ticables_progress_reset(handle->cable);
	TRYF(ticables_cable_recv(handle->cable, buf, NSP_HEADER_SIZE));

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

	if(pkt->src_port == 0x00fe || pkt->src_port == 0x00ff || pkt->src_port == 0x00d3)
		nsp_seq_pc++;
	else
		nsp_seq = pkt->seq;

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

