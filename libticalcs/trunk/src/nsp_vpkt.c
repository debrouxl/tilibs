/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.c 2077 2006-03-31 21:16:19Z roms $ */

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
	This unit manages virtual packets from/to NSPire (DirectLink).
	Virtual packets are fragmented into one or more raw packets.
*/

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#include "nsp_rpkt.h"
#include "nsp_vpkt.h"

// Type to string

static const ServiceName sid_types[] = 
{
	{ 0x00FE, "Reception Acknowledgment" },
	{ 0x00FF, "Reception Ack" },
	{ 0x4001, "Null" },
	{ 0x4002, "Echo" },
	{ 0x4003, "Device Address Request/Assignment" },
	{ 0x4020, "Device Information" },
	{ 0x4021, "Screen Capture" },
	{ 0x4024, "Screen Capture w/ RLE" },
	{ 0x4050, "Login" },
	{ 0x4060, "File Management" },
	{ 0x4080, "OS Installation" },
	{ 0x40DE, "Service Disconnect" },
	{ -1, NULL},
};

const char* nsp_sid2name(uint16_t id)
{
	const ServiceName *p;

	for(p = sid_types; p->name != NULL; p++)
		if(p->id == id)
			return p->name;

	return "unknown: not listed";
}

// Creation/Destruction/Garbage Collecting of packets

static GList *vtl_pkt_list = NULL;

VirtualPacket*  nsp_vtl_pkt_new_ex(uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port)
{
	VirtualPacket* vtl = g_malloc0(sizeof(VirtualPacket));

	vtl->src_addr = src_addr;
	vtl->src_port = src_port;
	vtl->dst_addr = dst_addr;
	vtl->dst_port = dst_port;
	vtl->size = size;
	vtl->data = g_malloc0(size+1);

	vtl_pkt_list = g_list_append(vtl_pkt_list, vtl);

	return vtl;
}

VirtualPacket*  nsp_vtl_pkt_new(void)
{
	return nsp_vtl_pkt_new_ex(0, 0, 0, 0, 0);
}

void			nsp_vtl_pkt_del(VirtualPacket* vtl)
{
	vtl_pkt_list = g_list_remove(vtl_pkt_list, vtl);

	g_free(vtl->data);
	g_free(vtl);
}

void			nsp_vtl_pkt_purge(void)
{
	g_list_foreach(vtl_pkt_list, (GFunc)nsp_vtl_pkt_del, NULL);
	g_list_free(vtl_pkt_list);
	vtl_pkt_list = NULL;
}

// Session Management

uint16_t	nsp_src_port = 0x8001;
uint16_t	nsp_dst_port = PORT_ADDR_REQUEST;

int nsp_session_open(CalcHandle *h, uint16_t port)
{
	nsp_src_port++;
	nsp_dst_port = port;

	ticalcs_info("  opening session from port #%04x to port #%04x:", nsp_src_port, nsp_dst_port);

	return 0;
}

int nsp_session_close(CalcHandle *h)
{
	ticalcs_info("  closed session from port #%04x to port #%04x:", nsp_src_port, nsp_dst_port);

	TRYF(nsp_disconnect(h));
	TRYF(nsp_recv_ack(h));

	nsp_dst_port = PORT_ADDR_REQUEST;

	return 0;
}

// Address Request/Assignment

int nsp_addr_request(CalcHandle *h)
{
	extern uint8_t nsp_seq_pc;
	RawPacket pkt = {0};

	// Reset connection so that device send an address request packet
	TRYC(h->cable->cable->reset(h->cable));
	nsp_seq_pc = 1;

	ticalcs_info("  device address request:");

	TRYF(nsp_recv(h, &pkt));
	
	if(pkt.src_port != PORT_ADDR_ASSIGN)
		return ERR_INVALID_PACKET;
	if(pkt.dst_port != PORT_ADDR_REQUEST)
		return ERR_INVALID_PACKET;

	return 0;
}

int nsp_addr_assign(CalcHandle *h, uint16_t addr)
{
	RawPacket pkt = {0};

	ticalcs_info("  assigning address %04x:", addr);

	pkt.data_size = 4;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = PORT_ADDR_ASSIGN;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = PORT_ADDR_ASSIGN;
	pkt.data[0] = MSB(addr);
	pkt.data[1] = LSB(addr);
	pkt.data[2] = 0xFF;
	pkt.data[3] = 0x00;
	TRYF(nsp_send(h, &pkt));

	return 0;
}

// Acknowledgement

int nsp_send_ack(CalcHandle* h)
{
	RawPacket pkt = {0};

	ticalcs_info("  sending ack:");

	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = PORT_PKT_ACK2;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(nsp_src_port);
	pkt.data[1] = LSB(nsp_src_port);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

int nsp_send_nack(CalcHandle* h)
{
	RawPacket pkt = {0};

	ticalcs_info("  sending nAck:");

	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = PORT_PKT_NACK;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(PORT_LOGIN);
	pkt.data[1] = LSB(PORT_LOGIN);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

int nsp_send_nack_ex(CalcHandle* h, uint16_t port)
{
	RawPacket pkt = {0};

	ticalcs_info("  sending nAck:");

	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = PORT_PKT_NACK;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = port;
	pkt.data[0] = MSB(PORT_LOGIN);
	pkt.data[1] = LSB(PORT_LOGIN);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

int nsp_recv_ack(CalcHandle *h)
{
	RawPacket pkt = {0};
	uint16_t addr;

	ticalcs_info("  receiving ack:");

	TRYF(nsp_recv(h, &pkt));
	
	if(pkt.src_port != PORT_PKT_ACK2)
		return ERR_INVALID_PACKET;
	if(pkt.dst_port != nsp_src_port)
		return ERR_INVALID_PACKET;

	addr = (pkt.data[0] << 8) | pkt.data[1];
	if(addr != nsp_dst_port)
		return ERR_INVALID_PACKET;

	if(pkt.ack != 0x0A)
		return ERR_INVALID_PACKET;

	return 0;
}

// Service Disconnection

int nsp_disconnect(CalcHandle *h)
{
	RawPacket pkt = {0};

	ticalcs_info("  disconnecting from service #%04x:", nsp_dst_port);

	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = PORT_DISCONNECT;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(nsp_src_port);
	pkt.data[1] = LSB(nsp_src_port);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

// Fragmenting of packets

int nsp_send_data(CalcHandle *h, VirtualPacket *vtl)
{
	RawPacket raw = {0};
	int i, r, q;
	long offset = 0;

	raw.src_addr = vtl->src_addr;
	raw.src_port = vtl->src_port;
	raw.dst_addr = vtl->dst_addr;
	raw.dst_port = vtl->dst_port;

	q = (vtl->size - offset) / (DATA_SIZE-1);
	r = (vtl->size - offset) % (DATA_SIZE-1);

	for(i = 1; i <= q; i++)
	{
		raw.data_size = DATA_SIZE;
		raw.data[0] = vtl->cmd;
		memcpy(raw.data + 1, vtl->data + offset, DATA_SIZE-1);
		offset += DATA_SIZE-1;

		TRYF(nsp_send(h, &raw));
		
		if(raw.src_port != PORT_ADDR_ASSIGN && raw.dst_port != PORT_ADDR_REQUEST)
			TRYF(nsp_recv_ack(h));

		h->updat->max1 = vtl->size;
		h->updat->cnt1 += DATA_SIZE;
		h->updat->pbar();
	}

	if(r || !vtl->size)
	{
		raw.data_size = r + 1;
		raw.data[0] = vtl->cmd;
		memcpy(raw.data + 1, vtl->data + offset, r);
		offset += r;
		
		TRYF(nsp_send(h, &raw));

		if(raw.src_port != PORT_ADDR_ASSIGN && raw.dst_port != PORT_ADDR_REQUEST)
			TRYF(nsp_recv_ack(h));
	}	

	return 0;
}

// Note: data field may be re-allocated.
int nsp_recv_data(CalcHandle* h, VirtualPacket* vtl)
{
	RawPacket raw = {0};
	long offset = 0;
	uint32_t size = vtl->size;

	vtl->size = 0;
	vtl->data = malloc(DATA_SIZE);

	do
	{
		TRYF(nsp_recv(h, &raw));
		vtl->cmd = raw.data[0];
		vtl->size += raw.data_size-1;

		vtl->data = realloc(vtl->data, vtl->size);
		memcpy(vtl->data + offset, &(raw.data[1]), raw.data_size-1);
		offset += raw.data_size-1;

		h->updat->max1 = size ? size : vtl->size;
		h->updat->cnt1 += DATA_SIZE;
		h->updat->pbar();

		if(raw.dst_port == PORT_LOGIN)
		{ TRYF(nsp_send_nack_ex(h, raw.src_port)); }
		else if(raw.src_port != PORT_ADDR_ASSIGN && raw.dst_port != PORT_ADDR_REQUEST)
		{ TRYF(nsp_send_ack(h)); }

	} while(raw.data_size >= DATA_SIZE);

	vtl->src_addr = raw.src_addr;
	vtl->src_port = raw.src_port;
	vtl->dst_addr = raw.dst_addr;
	vtl->dst_port = raw.dst_port;

	return 0;
}
