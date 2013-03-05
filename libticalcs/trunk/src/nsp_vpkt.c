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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	This unit manages virtual packets from/to NSPire (DirectLink).
	Virtual packets are fragmented into one or more raw packets.

	Please note this unit does not fully implement the NSpire protocol. It assumes
	there is one Nspire which is not exposing services. This assumption allows to 
	work in a linear fashion although we need sometimes some nasty hacks (LOGIN for
	instance).

	A better unit should implement a kind of daemon listening on all ports and launching
	a thread for each connection attempt. This way is fully parallelized but need a state
	machine and so more (complex) code. Maybe later...
*/

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#include "nsp_vpkt.h"

// Type to string

static const NSPServiceName sid_types[] = 
{
	{ 0x00FE, "Reception Acknowledgment" },
	{ 0x00FF, "Reception Ack" },
	{ 0x4001, "Null" },
	{ 0x4002, "Echo" },
	{ 0x4003, "Device Address Request/Assignment" },
	{ 0x4020, "Device Information" },
	{ 0x4021, "Screen Capture" },
	{ 0x4024, "Screen Capture w/ RLE" },
	{ 0x4042, "Keypresses" },
	{ 0x4050, "Login" },
	{ 0x4060, "File Management" },
	{ 0x4080, "OS Installation" },
	{ 0x40DE, "Service Disconnect" },
	{ -1, NULL},
};

TIEXPORT3 const char* TICALL nsp_sid2name(uint16_t id)
{
	const NSPServiceName *p;

	for(p = sid_types; p->name != NULL; p++)
		if(p->id == id)
			return p->name;

	return "unknown: not listed";
}

// Creation/Destruction/Garbage Collecting of packets

static GList *vtl_pkt_list = NULL;

TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new_ex(uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port)
{
	NSPVirtualPacket* vtl = g_malloc0(sizeof(NSPVirtualPacket)); // aborts the program if it fails.

	vtl->src_addr = src_addr;
	vtl->src_port = src_port;
	vtl->dst_addr = dst_addr;
	vtl->dst_port = dst_port;
	vtl->size = size;
	vtl->data = g_malloc0(size+1); // aborts the program if it fails.

	vtl_pkt_list = g_list_append(vtl_pkt_list, vtl);

	return vtl;
}

TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new(void)
{
	return nsp_vtl_pkt_new_ex(0, 0, 0, 0, 0);
}

TIEXPORT3 void TICALL nsp_vtl_pkt_del(NSPVirtualPacket* vtl)
{
	if (vtl != NULL)
	{
		vtl_pkt_list = g_list_remove(vtl_pkt_list, vtl);

		g_free(vtl->data);
		g_free(vtl);
	}
	else
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
	}
}

void nsp_vtl_pkt_purge(void)
{
	g_list_foreach(vtl_pkt_list, (GFunc)nsp_vtl_pkt_del, NULL);
	g_list_free(vtl_pkt_list);
	vtl_pkt_list = NULL;
}

// Session Management

uint16_t	nsp_src_port = 0x8001;
uint16_t	nsp_dst_port = NSP_PORT_ADDR_REQUEST;

TIEXPORT3 int TICALL nsp_session_open(CalcHandle *h, uint16_t port)
{
	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	nsp_src_port++;
	nsp_dst_port = port;

	ticalcs_info("  opening session from port #%04x to port #%04x:", nsp_src_port, nsp_dst_port);

	return 0;
}

TIEXPORT3 int TICALL nsp_session_close(CalcHandle *h)
{
	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  closed session from port #%04x to port #%04x:", nsp_src_port, nsp_dst_port);

	TRYF(nsp_send_disconnect(h));
	TRYF(nsp_recv_ack(h));

	nsp_dst_port = NSP_PORT_ADDR_REQUEST;

	return 0;
}

// Address Request/Assignment

TIEXPORT3 int TICALL nsp_addr_request(CalcHandle *h)
{
	extern uint8_t nsp_seq_pc;
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	memset(&pkt, 0, sizeof(pkt));

	// Reset connection so that device send an address request packet
	TRYC(h->cable->cable->reset(h->cable));
	nsp_seq_pc = 1;

	ticalcs_info("  device address request:");

	TRYF(nsp_recv(h, &pkt));
	
	if(pkt.src_port != NSP_PORT_ADDR_ASSIGN)
		return ERR_INVALID_PACKET;
	if(pkt.dst_port != NSP_PORT_ADDR_REQUEST)
		return ERR_INVALID_PACKET;

	return 0;
}

TIEXPORT3 int TICALL nsp_addr_assign(CalcHandle *h, uint16_t addr)
{
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  assigning address %04x:", addr);

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 4;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_ADDR_ASSIGN;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = NSP_PORT_ADDR_ASSIGN;
	pkt.data[0] = MSB(addr);
	pkt.data[1] = LSB(addr);
	pkt.data[2] = 0xFF;
	pkt.data[3] = 0x00;
	TRYF(nsp_send(h, &pkt));

	return 0;
}

// Acknowledgement

TIEXPORT3 int TICALL nsp_send_ack(CalcHandle* h)
{
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  sending ack:");

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_PKT_ACK2;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(nsp_src_port);
	pkt.data[1] = LSB(nsp_src_port);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

TIEXPORT3 int TICALL nsp_send_nack(CalcHandle* h)
{
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  sending nAck:");

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_PKT_NACK;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(NSP_PORT_LOGIN);
	pkt.data[1] = LSB(NSP_PORT_LOGIN);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

TIEXPORT3 int TICALL nsp_send_nack_ex(CalcHandle* h, uint16_t port)
{
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  sending nAck:");

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_PKT_NACK;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = port;
	pkt.data[0] = MSB(NSP_PORT_LOGIN);
	pkt.data[1] = LSB(NSP_PORT_LOGIN);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

TIEXPORT3 int TICALL nsp_recv_ack(CalcHandle *h)
{
	NSPRawPacket pkt;
	uint16_t addr;
	int ret = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  receiving ack:");

	memset(&pkt, 0, sizeof(pkt));

	TRYF(nsp_recv(h, &pkt));

	if(pkt.src_port != NSP_PORT_PKT_ACK2)
	{
		ticalcs_info("XXX weird src_port\n");
		ret = ERR_INVALID_PACKET;
	}
	if(pkt.dst_port != nsp_src_port)
	{
		ticalcs_info("XXX weird .dst_port\n");
		ret = ERR_INVALID_PACKET;
	}

	if (pkt.data_size >= 2)
	{
		addr = (pkt.data[0] << 8) | pkt.data[1];
		if(addr != nsp_dst_port)
		{
			ticalcs_info("XXX weird addr\n");
			ret = ERR_INVALID_PACKET;
		}
	}
	else
	{
		ticalcs_info("XXX weird addr\n");
		ret = ERR_INVALID_PACKET;
	}

	if(pkt.ack != 0x0A)
	{
		ticalcs_info("XXX weird .ack\n");
		ret = ERR_INVALID_PACKET;
	}

	return ret;
}

// Service Disconnection

TIEXPORT3 int TICALL nsp_send_disconnect(CalcHandle *h)
{
	NSPRawPacket pkt;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  disconnecting from service #%04x:", nsp_dst_port);

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_DISCONNECT;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = nsp_dst_port;
	pkt.data[0] = MSB(nsp_src_port);
	pkt.data[1] = LSB(nsp_src_port);
	TRYF(nsp_send(h, &pkt));

	return 0;
}

TIEXPORT3 int TICALL nsp_recv_disconnect(CalcHandle *h)
{
	NSPRawPacket pkt;
	uint16_t addr;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  receiving disconnect:");

	memset(&pkt, 0, sizeof(pkt));

	TRYF(nsp_recv(h, &pkt));

	if(pkt.src_port != NSP_PORT_DISCONNECT)
		return ERR_INVALID_PACKET;

	// nasty hack
	nsp_dst_port = (pkt.data[0] << 8) | pkt.data[1];
	addr = pkt.dst_port;

	// nasty hack
	{
		ticalcs_info("  sending ack:");

		pkt.unused = 0;
		pkt.data_size = 2;
		pkt.src_addr = NSP_SRC_ADDR;
		pkt.src_port = NSP_PORT_PKT_ACK2;
		pkt.dst_addr = NSP_DEV_ADDR;
		pkt.dst_port = nsp_dst_port;
		pkt.data_sum = 0;
		pkt.ack = 0;
		pkt.seq = 0;
		pkt.hdr_sum = 0;
		pkt.data[0] = MSB(addr);
		pkt.data[1] = LSB(addr);
		TRYF(nsp_send(h, &pkt));
	}

	return 0;
}

// Fragmenting of packets

TIEXPORT3 int TICALL nsp_send_data(CalcHandle *h, NSPVirtualPacket *vtl)
{
	NSPRawPacket raw;
	int i, r, q;
	long offset = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vtl == NULL)
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	memset(&raw, 0, sizeof(raw));
	raw.src_addr = vtl->src_addr;
	raw.src_port = vtl->src_port;
	raw.dst_addr = vtl->dst_addr;
	raw.dst_port = vtl->dst_port;

	q = (vtl->size - offset) / (NSP_DATA_SIZE-1);
	r = (vtl->size - offset) % (NSP_DATA_SIZE-1);

	for(i = 1; i <= q; i++)
	{
		raw.data_size = NSP_DATA_SIZE;
		raw.data[0] = vtl->cmd;
		memcpy(raw.data + 1, vtl->data + offset, NSP_DATA_SIZE-1);
		offset += NSP_DATA_SIZE-1;

		TRYF(nsp_send(h, &raw));
		
		if(raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
			TRYF(nsp_recv_ack(h));

		h->updat->max1 = vtl->size;
		h->updat->cnt1 += NSP_DATA_SIZE;
		h->updat->pbar();
	}

	if(r || !vtl->size)
	{
		raw.data_size = r + 1;
		raw.data[0] = vtl->cmd;
		memcpy(raw.data + 1, vtl->data + offset, r);
		offset += r;
		
		TRYF(nsp_send(h, &raw));

		if(raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
			TRYF(nsp_recv_ack(h));
	}	

	return 0;
}

// Note: data field may be re-allocated.
TIEXPORT3 int TICALL nsp_recv_data(CalcHandle* h, NSPVirtualPacket* vtl)
{
	NSPRawPacket raw;
	long offset = 0;
	uint32_t size;
	int err = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vtl == NULL)
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	memset(&raw, 0, sizeof(raw));

	size = vtl->size;
	vtl->size = 0;
	vtl->data = g_malloc(NSP_DATA_SIZE);

	if (vtl->data)
	{
		for(;;)
		{
			err = nsp_recv(h, &raw);
			if (err)
			{
				break;
			}
			if (raw.data_size > 0)
			{
				vtl->cmd = raw.data[0];
				vtl->size += raw.data_size-1;

				vtl->data = g_realloc(vtl->data, vtl->size);
				memcpy(vtl->data + offset, &(raw.data[1]), raw.data_size-1);
				offset += raw.data_size-1;

				h->updat->max1 = size ? size : vtl->size;
				h->updat->cnt1 += NSP_DATA_SIZE;
				h->updat->pbar();
			}

			if(raw.dst_port == NSP_PORT_LOGIN)
			{
				err = nsp_send_nack_ex(h, raw.src_port);
				if (err)
				{
					break;
				}
			}
			else if(raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
			{
				err = nsp_send_ack(h);
				if (err)
				{
					break;
				}
			}

			if (raw.data_size < NSP_DATA_SIZE)
				break;
			if (size && vtl->size == size)
				break;
		}
	}

	vtl->src_addr = raw.src_addr;
	vtl->src_port = raw.src_port;
	vtl->dst_addr = raw.dst_addr;
	vtl->dst_port = raw.dst_port;

	return err;
}
