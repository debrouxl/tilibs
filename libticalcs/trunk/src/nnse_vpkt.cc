/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2019  Lionel Debroux
 *  Copyright (C) 2019  Fabian Vogt
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

#include "nnse_vpkt.h"

// Creation/Destruction/Garbage Collecting of packets

TIEXPORT3 NNSEVirtualPacket* TICALL nnse_vtl_pkt_new(CalcHandle * handle)
{
	return nnse_vtl_pkt_new_ex(handle, 0, 0, 0, 0, NULL);
}

TIEXPORT3 NNSEVirtualPacket* TICALL nnse_vtl_pkt_new_ex(CalcHandle * handle, uint32_t size, uint8_t service, uint8_t src_addr, uint8_t dst_addr, uint8_t * data)
{
	NNSEVirtualPacket* vtl = NULL;

	if (ticalcs_validate_handle(handle))
	{
		vtl = (NNSEVirtualPacket *)g_malloc0(sizeof(*vtl));

		if (NULL != vtl)
		{
			//GList * vtl_pkt_list;

			nnse_vtl_pkt_fill(vtl, size, service, src_addr, dst_addr, data);

			//vtl_pkt_list = g_list_append((GList *)(handle->priv.nnse_vtl_pkt_list), vtl);
			//handle->priv.nnse_vtl_pkt_list = (void *)vtl_pkt_list;
		}
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return vtl;
}

TIEXPORT3 void TICALL nnse_vtl_pkt_fill(NNSEVirtualPacket* vtl, uint32_t size, uint8_t service, uint8_t src_addr, uint8_t dst_addr, uint8_t * data)
{
	if (vtl != NULL)
	{
		vtl->service = service;
		vtl->src_addr = src_addr;
		vtl->dst_addr = dst_addr;
		vtl->size = size;
		vtl->data = data;
	}
	else
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
	}
}

TIEXPORT3 void TICALL nnse_vtl_pkt_del(CalcHandle *handle, NNSEVirtualPacket* vtl)
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

	//vtl_pkt_list = g_list_remove((GList *)(handle->priv.nnse_vtl_pkt_list), vtl);
	//handle->priv.nnse_vtl_pkt_list = (void *)vtl_pkt_list;

	g_free(vtl->data);
	g_free(vtl);
}

TIEXPORT3 void * TICALL nnse_vtl_pkt_alloc_data(size_t size)
{
	return g_malloc0(size + 1);
}

TIEXPORT3 NNSEVirtualPacket * TICALL nnse_vtl_pkt_realloc_data(NNSEVirtualPacket* vtl, size_t size)
{
	if (vtl != NULL)
	{
		if (size + 1 > size)
		{
			uint8_t * data = (uint8_t *)g_realloc(vtl->data, size + 1);
			if (NULL != data)
			{
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
		else
		{
			return NULL;
		}
	}

	return vtl;
}

TIEXPORT3 void TICALL nnse_vtl_pkt_free_data(void * data)
{
	return g_free(data);
}

// Session Management



// Address Request/Assignment

TIEXPORT3 int TICALL nnse_addr_request(CalcHandle *handle)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}

TIEXPORT3 int TICALL nnse_addr_assign(CalcHandle *handle, uint8_t addr)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}

// Acknowledgement

TIEXPORT3 int TICALL nnse_send_ack(CalcHandle* handle)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}

TIEXPORT3 int TICALL nnse_recv_ack(CalcHandle *handle)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}

// Fragmenting of packets

TIEXPORT3 int TICALL nnse_send_data(CalcHandle *handle, NNSEVirtualPacket *vtl)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}

// Note: data field may be re-allocated.
TIEXPORT3 int TICALL nnse_recv_data(CalcHandle* handle, NNSEVirtualPacket* vtl)
{
	int ret;
	ret = ERR_UNSUPPORTED;
	return ret;
}
