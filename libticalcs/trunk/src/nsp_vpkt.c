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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "nsp_vpkt.h"

// Creation/Destruction/Garbage Collecting of packets

TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new(CalcHandle * handle)
{
	return nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, NULL);
}

TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new_ex(CalcHandle * handle, uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint8_t cmd, uint8_t * data)
{
	NSPVirtualPacket* vtl = NULL;

	if (ticalcs_validate_handle(handle))
	{
		//GList * vtl_pkt_list;

		vtl = g_malloc0(sizeof(NSPVirtualPacket)); // aborts the program if it fails.

		nsp_vtl_pkt_fill(vtl, size, src_addr, src_port, dst_addr, dst_port, cmd, data); // aborts the program if it fails.

		//vtl_pkt_list = g_list_append((GList *)(handle->priv.nsp_vtl_pkt_list), vtl);
		//handle->priv.nsp_vtl_pkt_list = (void *)vtl_pkt_list;
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return vtl;
}

TIEXPORT3 void TICALL nsp_vtl_pkt_fill(NSPVirtualPacket* vtl, uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint8_t cmd, uint8_t * data)
{
	if (vtl != NULL)
	{
		vtl->src_addr = src_addr;
		vtl->src_port = src_port;
		vtl->dst_addr = dst_addr;
		vtl->dst_port = dst_port;
		vtl->cmd = cmd;
		vtl->size = size;
		vtl->data = data;
	}
	else
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
	}
}

TIEXPORT3 void TICALL nsp_vtl_pkt_del(CalcHandle *handle, NSPVirtualPacket* vtl)
{
	//GList *vtl_pkt_list;

	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	if (vtl == NULL)
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
		return;
	}

	//vtl_pkt_list = g_list_remove((GList *)(handle->priv.nsp_vtl_pkt_list), vtl);
	//handle->priv.nsp_vtl_pkt_list = (void *)vtl_pkt_list;

	g_free(vtl->data);
	g_free(vtl);
}

TIEXPORT3 void * TICALL nsp_vtl_pkt_alloc_data(size_t size)
{
	return g_malloc0(size + 1);
}

TIEXPORT3 NSPVirtualPacket * TICALL nsp_vtl_pkt_realloc_data(NSPVirtualPacket* vtl, size_t size)
{
	if (vtl != NULL)
	{
		if (size + 1 > size)
		{
			uint8_t * data = g_realloc(vtl->data, size + 1);
			if (size > vtl->size)
			{
				// The previous time, vtl->size + 1 bytes were allocated and initialized.
				// This time, we've allocated size + 1 bytes, so we need to initialize size - vtl->size extra bytes.
				memset(data + vtl->size + 1, 0x00, size - vtl->size);
			}
			vtl->data = data;
		}
		else
		{
			return NULL;
		}
	}

	return vtl;
}

TIEXPORT3 void TICALL nsp_vtl_pkt_free_data(void * data)
{
	return g_free(data);
}

// Session Management

TIEXPORT3 int TICALL nsp_session_open(CalcHandle *handle, uint16_t port)
{
	VALIDATE_HANDLE(handle);

	handle->priv.nsp_src_port++;
	handle->priv.nsp_dst_port = port;

	ticalcs_info("  opening session from port #%04x to port #%04x:", handle->priv.nsp_src_port, handle->priv.nsp_dst_port);

	return 0;
}

TIEXPORT3 int TICALL nsp_session_close(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  closed session from port #%04x to port #%04x:", handle->priv.nsp_src_port, handle->priv.nsp_dst_port);

	ret = nsp_send_disconnect(handle);
	if (!ret)
	{
		ret = nsp_recv_ack(handle);
		if (!ret)
		{
			handle->priv.nsp_dst_port = NSP_PORT_ADDR_REQUEST;
		}
	}

	return ret;
}

// Address Request/Assignment

TIEXPORT3 int TICALL nsp_addr_request(CalcHandle *handle)
{
	NSPRawPacket pkt;
	int ret;

	VALIDATE_HANDLE(handle);

	memset(&pkt, 0, sizeof(pkt));

	// Reset connection so that device send an address request packet
	ret = handle->cable->cable->reset(handle->cable);
	if (!ret)
	{
		handle->priv.nsp_seq_pc = 1;

		ticalcs_info("  device address request:");

		ret = nsp_recv(handle, &pkt);
		if (!ret)
		{
			if (   pkt.src_port != NSP_PORT_ADDR_ASSIGN
			    || pkt.dst_port != NSP_PORT_ADDR_REQUEST)
			{
				ret = ERR_INVALID_PACKET;
			}
		}
	}

	return ret;
}

TIEXPORT3 int TICALL nsp_addr_assign(CalcHandle *handle, uint16_t addr)
{
	NSPRawPacket pkt;

	VALIDATE_HANDLE(handle);

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

	return nsp_send(handle, &pkt);
}

// Acknowledgement

TIEXPORT3 int TICALL nsp_send_ack(CalcHandle* handle)
{
	NSPRawPacket pkt;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending ack:");

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_PKT_ACK2;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = handle->priv.nsp_dst_port;
	pkt.data[0] = MSB(handle->priv.nsp_src_port);
	pkt.data[1] = LSB(handle->priv.nsp_src_port);

	return nsp_send(handle, &pkt);
}

TIEXPORT3 int TICALL nsp_send_nack(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	return nsp_send_nack_ex(handle, handle->priv.nsp_dst_port);
}

TIEXPORT3 int TICALL nsp_send_nack_ex(CalcHandle* handle, uint16_t port)
{
	NSPRawPacket pkt;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending nAck:");

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_PKT_NACK;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = port;
	pkt.data[0] = MSB(NSP_PORT_LOGIN);
	pkt.data[1] = LSB(NSP_PORT_LOGIN);

	return nsp_send(handle, &pkt);
}

TIEXPORT3 int TICALL nsp_recv_ack(CalcHandle *handle)
{
	NSPRawPacket pkt;
	uint16_t addr;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  receiving ack:");

	memset(&pkt, 0, sizeof(pkt));

	ret = nsp_recv(handle, &pkt);
	if (!ret)
	{
		if (pkt.src_port != NSP_PORT_PKT_ACK2)
		{
			ticalcs_info("XXX weird src_port\n");
			ret = ERR_INVALID_PACKET;
		}
		if (pkt.dst_port != handle->priv.nsp_src_port)
		{
			ticalcs_info("XXX weird .dst_port\n");
			ret = ERR_INVALID_PACKET;
		}

		if (pkt.data_size >= 2)
		{
			addr = (((uint16_t)pkt.data[0]) << 8) | pkt.data[1];
			if (addr != handle->priv.nsp_dst_port)
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

		if (pkt.ack != 0x0A)
		{
			ticalcs_info("XXX weird .ack\n");
			ret = ERR_INVALID_PACKET;
		}
	}

	return ret;
}

// Service Disconnection

TIEXPORT3 int TICALL nsp_send_disconnect(CalcHandle *handle)
{
	NSPRawPacket pkt;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  disconnecting from service #%04x:", handle->priv.nsp_dst_port);

	memset(&pkt, 0, sizeof(pkt));
	pkt.data_size = 2;
	pkt.src_addr = NSP_SRC_ADDR;
	pkt.src_port = NSP_PORT_DISCONNECT;
	pkt.dst_addr = NSP_DEV_ADDR;
	pkt.dst_port = handle->priv.nsp_dst_port;
	pkt.data[0] = MSB(handle->priv.nsp_src_port);
	pkt.data[1] = LSB(handle->priv.nsp_src_port);

	return nsp_send(handle, &pkt);
}

TIEXPORT3 int TICALL nsp_recv_disconnect(CalcHandle *handle)
{
	NSPRawPacket pkt;
	uint16_t addr;
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  receiving disconnect:");

	memset(&pkt, 0, sizeof(pkt));

	ret = nsp_recv(handle, &pkt);
	if (!ret)
	{

		if (pkt.src_port != NSP_PORT_DISCONNECT)
		{
			ret = ERR_INVALID_PACKET;
		}
		else
		{
			// nasty hacks
			handle->priv.nsp_dst_port = (((uint16_t)pkt.data[0]) << 8) | pkt.data[1];
			addr = pkt.dst_port;

			ticalcs_info("  sending ack:");

			pkt.unused = 0;
			pkt.data_size = 2;
			pkt.src_addr = NSP_SRC_ADDR;
			pkt.src_port = NSP_PORT_PKT_ACK2;
			pkt.dst_addr = NSP_DEV_ADDR;
			pkt.dst_port = handle->priv.nsp_dst_port;
			pkt.data_sum = 0;
			pkt.ack = 0;
			pkt.seq = 0;
			pkt.hdr_sum = 0;
			pkt.data[0] = MSB(addr);
			pkt.data[1] = LSB(addr);
			ret = nsp_send(handle, &pkt);
		}
	}

	return ret;
}

// Fragmenting of packets

TIEXPORT3 int TICALL nsp_send_data(CalcHandle *handle, NSPVirtualPacket *vtl)
{
	NSPRawPacket raw;
	int i, r, q;
	long offset = 0;
	int ret = 0;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vtl);
	if (vtl->size && !vtl->data)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_NSP_VPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nsp_vpkt(&event, vtl->src_addr, vtl->src_port, vtl->dst_addr, vtl->dst_port, vtl->cmd, vtl->size, vtl->data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		memset(&raw, 0, sizeof(raw));
		raw.src_addr = vtl->src_addr;
		raw.src_port = vtl->src_port;
		raw.dst_addr = vtl->dst_addr;
		raw.dst_port = vtl->dst_port;

		q = (vtl->size - offset) / (NSP_DATA_SIZE-1);
		r = (vtl->size - offset) % (NSP_DATA_SIZE-1);

		for (i = 1; i <= q; i++)
		{
			raw.data_size = NSP_DATA_SIZE;
			raw.data[0] = vtl->cmd;
			memcpy(raw.data + 1, vtl->data + offset, NSP_DATA_SIZE-1);
			offset += NSP_DATA_SIZE-1;

			ret = nsp_send(handle, &raw);
			if (ret)
			{
				break;
			}

			if (raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
			{
				ret = nsp_recv_ack(handle);
				if (ret)
				{
					break;
				}
			}

			handle->updat->max1 = vtl->size;
			handle->updat->cnt1 += NSP_DATA_SIZE;
			handle->updat->pbar();
		}

		if (!ret)
		{
			if (r || !vtl->size)
			{
				raw.data_size = r + 1;
				raw.data[0] = vtl->cmd;
				if (vtl->data)
				{
					memcpy(raw.data + 1, vtl->data + offset, r);
				}
				offset += r;

				ret = nsp_send(handle, &raw);
				if (!ret)
				{
					if (raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
					{
						ret = nsp_recv_ack(handle);
					}
				}
			}
		}
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_NSP_VPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nsp_vpkt(&event, vtl->src_addr, vtl->src_port, vtl->dst_addr, vtl->dst_port, vtl->cmd, vtl->size, vtl->data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

// Note: data field may be re-allocated.
TIEXPORT3 int TICALL nsp_recv_data(CalcHandle* handle, NSPVirtualPacket* vtl)
{
	NSPRawPacket raw;
	long offset = 0;
	uint32_t size;
	int ret = 0;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vtl);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_NSP_VPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nsp_vpkt(&event, /* src_addr */ 0, /* src_port */ 0, /* dst_addr */ 0, /* dst_port */ 0, /* cmd */ 0, /* size */ 0, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		memset(&raw, 0, sizeof(raw));

		size = vtl->size;
		vtl->size = 0;
		vtl->data = g_malloc(NSP_DATA_SIZE);

		if (vtl->data)
		{
			for (;;)
			{
				ret = nsp_recv(handle, &raw);
				if (ret)
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

					handle->updat->max1 = size ? size : vtl->size;
					handle->updat->cnt1 += NSP_DATA_SIZE;
					handle->updat->pbar();
				}

				if (raw.dst_port == NSP_PORT_LOGIN)
				{
					ret = nsp_send_nack_ex(handle, raw.src_port);
					if (ret)
					{
						break;
					}
				}
				else if (raw.src_port != NSP_PORT_ADDR_ASSIGN && raw.dst_port != NSP_PORT_ADDR_REQUEST)
				{
					ret = nsp_send_ack(handle);
					if (ret)
					{
						break;
					}
				}

				if (raw.data_size < NSP_DATA_SIZE)
				{
					break;
				}
				if (size && vtl->size == size)
				{
					break;
				}
			}
		}

		vtl->src_addr = raw.src_addr;
		vtl->src_port = raw.src_port;
		vtl->dst_addr = raw.dst_addr;
		vtl->dst_port = raw.dst_port;
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_NSP_VPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nsp_vpkt(&event, vtl->src_addr, vtl->src_port, vtl->dst_addr, vtl->dst_port, vtl->cmd, vtl->size, vtl->data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}
