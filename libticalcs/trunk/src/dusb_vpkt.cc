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
	This unit manages virtual packets from/to D-USB (DirectLink).
	Virtual packets are fragmented into one or more raw packets.
	The size of packet can be negotiated on both sides.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "dusb_rpkt.h"
#include "dusb_vpkt.h"

#define VPKT_DBG	1	// 1 = verbose, 2 = more verbose

// Type to string

typedef struct
{
	uint16_t   id;
	const char *name;
} DUSBVtlPktInfo;

static const DUSBVtlPktInfo vpkt_types[] =
{
	{ 0x0000, "" },
	{ DUSB_VPKT_PING, "Ping / Set Mode" },
	{ DUSB_VPKT_OS_BEGIN, "Begin OS Transfer" },
	{ DUSB_VPKT_OS_ACK, "Acknowledgement of OS Transfer" },
	{ DUSB_VPKT_OS_HEADER, "OS Header" },
	{ DUSB_VPKT_OS_DATA, "OS Data" },
	{ DUSB_VPKT_EOT_ACK, "Acknowledgement of EOT" },
	{ DUSB_VPKT_PARM_REQ, "Parameter Request" },
	{ DUSB_VPKT_PARM_DATA, "Parameter Data" },
	{ DUSB_VPKT_DIR_REQ, "Request Directory Listing" },
	{ DUSB_VPKT_VAR_HDR, "Variable Header" },
	{ DUSB_VPKT_RTS, "Request to Send" },
	{ DUSB_VPKT_VAR_REQ, "Request Variable" },
	{ DUSB_VPKT_VAR_CNTS, "Variable Contents" },
	{ DUSB_VPKT_PARM_SET, "Parameter Set" },
	{ 0x000F, "" },
	{ DUSB_VPKT_MODIF_VAR, "Modify Variable" },
	{ DUSB_VPKT_EXECUTE, "Remote Control" },
	{ DUSB_VPKT_MODE_SET, "Acknowledgement of Mode Setting" },
	{ DUSB_VPKT_DATA_ACK, "Acknowledgement of Data" },
	{ DUSB_VPKT_DELAY_ACK, "Delay Acknowledgment" },
	{ DUSB_VPKT_EOT, "End of Transmission" },
	{ DUSB_VPKT_ERROR, "Error" },
	{ 0xFFFF, NULL}
};

const char* TICALL dusb_vpkt_type2name(uint16_t id)
{
	const DUSBVtlPktInfo *p;

	for (p = vpkt_types; p->name != NULL; p++)
	{
		if (p->id == id)
		{
			return p->name;
		}
	}

	return "unknown: not listed";
}

// Creation/Destruction/Garbage Collecting of packets

DUSBVirtualPacket* TICALL dusb_vtl_pkt_new_ex(CalcHandle * handle, uint32_t size, uint16_t type, uint8_t * data)
{
	DUSBVirtualPacket* vtl = NULL;

	// RFS: it's alright if data is nullptr at this stage.

	if (ticalcs_validate_handle(handle))
	{
		vtl = (DUSBVirtualPacket *)g_malloc0(sizeof(DUSBVirtualPacket));

		if (NULL != vtl)
		{
			//GList * vtl_pkt_list;

			vtl->size = size;
			vtl->type = type;
			vtl->data = data;

			//vtl_pkt_list = g_list_append((GList *)(handle->priv.dusb_vtl_pkt_list), vtl);
			//handle->priv.dusb_vtl_pkt_list = (void *)vtl_pkt_list;
		}
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return vtl;
}

DUSBVirtualPacket* TICALL dusb_vtl_pkt_new(CalcHandle * handle)
{
	return dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);
}

void TICALL dusb_vtl_pkt_fill(DUSBVirtualPacket* vtl, uint32_t size, uint16_t type, uint8_t * data)
{
	if (vtl != NULL)
	{
		vtl->size = size;
		vtl->type = type;
		vtl->data = data;
	}
	else
	{
		ticalcs_critical("%s: vtl is NULL", __FUNCTION__);
	}
}

void TICALL dusb_vtl_pkt_del(CalcHandle * handle, DUSBVirtualPacket* vtl)
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

	//vtl_pkt_list = g_list_remove((GList *)(handle->priv.dusb_vtl_pkt_list), vtl);
	//handle->priv.dusb_vtl_pkt_list = (void *)vtl_pkt_list;

	g_free(vtl->data);
	g_free(vtl);
}

void * TICALL dusb_vtl_pkt_alloc_data(CalcHandle * handle, size_t size)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return nullptr;
	}

	return g_malloc0(size + DUSB_DH_SIZE);
}

DUSBVirtualPacket * TICALL dusb_vtl_pkt_realloc_data(CalcHandle * handle, DUSBVirtualPacket * vtl, size_t size)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return nullptr;
	}

	if (vtl != NULL)
	{
		if (size + DUSB_DH_SIZE > size)
		{
			uint8_t * data = (uint8_t *)g_realloc(vtl->data, size + DUSB_DH_SIZE);
			if (size > vtl->size)
			{
				// The previous time, vtl->size + DUSB_DH_SIZE bytes were allocated and initialized.
				// This time, we've allocated size + DUSB_DH_SIZE bytes, so we need to initialize size - vtl->size extra bytes.
				memset(data + vtl->size + DUSB_DH_SIZE, 0x00, size - vtl->size);
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

void TICALL dusb_vtl_pkt_free_data(CalcHandle * handle, void * data)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	return g_free(data);
}

// Raw packets

int TICALL dusb_send_buf_size_request(CalcHandle* handle, uint32_t size)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle);

	// Single call to dusb_send(), no need to take handle->busy.

	if (size > sizeof(raw.data) + 1)
	{
		ticalcs_warning("Clamping dubious large DUSB buffer size request");
		size = sizeof(raw.data) + 1;
	}

	memset(&raw, 0, sizeof(raw));
	raw.size = 4;
	raw.type = DUSB_RPKT_BUF_SIZE_REQ;
	raw.data[0] = (size >> 24) & 0xFF;
	raw.data[1] = (size >> 16) & 0xFF;
	raw.data[2] = (size >>  8) & 0xFF;
	raw.data[3] = (size      ) & 0xFF;

	ret = dusb_send(handle, &raw);
	if (!ret)
	{
		ticalcs_info("  PC->TI: Buffer Size Request (%i bytes)", size);
	}

	return ret;
}

int TICALL dusb_recv_buf_size_alloc(CalcHandle* handle, uint32_t *size)
{
	DUSBRawPacket raw;
	uint32_t tmp;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	// Single call to dusb_recv(), no need to take handle->busy.

	memset(&raw, 0, sizeof(raw));

	do
	{
		ret = dusb_recv(handle, &raw);

		if (ret)
		{
			break;
		}

		if (raw.size != 4 || raw.type != DUSB_RPKT_BUF_SIZE_ALLOC)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}

		tmp = (((uint32_t)raw.data[0]) << 24) | (((uint32_t)raw.data[1]) << 16) | (((uint32_t)raw.data[2]) << 8) | (((uint32_t)raw.data[3]) << 0);
		if (tmp > sizeof(raw.data))
		{
			ticalcs_critical("Clamping overly large buffer size allocation to %u bytes", (unsigned int)sizeof(raw.data));
			tmp = sizeof(raw.data);
		}
		if (   (handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB)
		    && tmp > 1018)
		{
			ticalcs_info("The 83PCE/84+CE allocate more than they support. Clamping buffer size to 1018");
			tmp = 1018;
		}
		if (size)
		{
			*size = tmp;
		}
		ticalcs_info("  TI->PC: Buffer Size Allocation (%i bytes)", tmp);

		handle->priv.dusb_rpkt_maxlen = tmp;
	} while(0);

	return ret;
}

int TICALL dusb_recv_buf_size_request(CalcHandle* handle, uint32_t *size)
{
	DUSBRawPacket raw;
	uint32_t tmp;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	// Single call to dusb_recv(), no need to take handle->busy.

	memset(&raw, 0, sizeof(raw));

	do
	{
		ret = dusb_recv(handle, &raw);
		if (ret)
		{
			break;
		}

		if (raw.size != 4 || raw.type != DUSB_RPKT_BUF_SIZE_REQ)
		{
			ret = ERR_INVALID_PACKET;
		}

		tmp = (((uint32_t)raw.data[0]) << 24) | (((uint32_t)raw.data[1]) << 16) | (((uint32_t)raw.data[2]) << 8) | (((uint32_t)raw.data[3]) << 0);
		if (size)
		{
			*size = tmp;
		}
		ticalcs_info("  TI->PC: Buffer Size Request (%i bytes)", tmp);
	} while(0);

	return ret;
}

int TICALL dusb_send_buf_size_alloc(CalcHandle* handle, uint32_t size)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle);

	// Single call to dusb_send(), no need to take handle->busy.

	if (size > sizeof(raw.data) + 1)
	{
		ticalcs_warning("Clamping dubious large DUSB buffer size request");
		size = sizeof(raw.data) + 1;
	}

	memset(&raw, 0, sizeof(raw));
	raw.size = 4;
	raw.type = DUSB_RPKT_BUF_SIZE_ALLOC;
	raw.data[0] = (size >> 24) & 0xFF;
	raw.data[1] = (size >> 16) & 0xFF;
	raw.data[2] = (size >>  8) & 0xFF;
	raw.data[3] = (size      ) & 0xFF;

	ret = dusb_send(handle, &raw);
	if (!ret)
	{
		ticalcs_info("  PC->TI: Buffer Size Allocation (%i bytes)", size);
	}

	handle->priv.dusb_rpkt_maxlen = size;

	return ret;
}

int TICALL dusb_get_buf_size(CalcHandle* handle, uint32_t *size)
{
	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(size);

	*size = handle->priv.dusb_rpkt_maxlen;

	return 0;
}

int TICALL dusb_set_buf_size(CalcHandle* handle, uint32_t size)
{
	VALIDATE_HANDLE(handle);

	if (size > DUSB_DATA_SIZE + 1)
	{
		ticalcs_warning("Clamping dubious large DUSB buffer size");
		size = DUSB_DATA_SIZE + 1;
	}

	handle->priv.dusb_rpkt_maxlen = size;

	return 0;
}

int TICALL dusb_send_acknowledge(CalcHandle* handle)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle);

	// Single call to dusb_send(), no need to take handle->busy.

	memset(&raw, 0, sizeof(raw));
	raw.size = 2;
	raw.type = DUSB_RPKT_VIRT_DATA_ACK;
	raw.data[0] = 0xE0;
	raw.data[1] = 0x00;

	ret = dusb_send(handle, &raw);
	if (!ret)
	{
#if (VPKT_DBG == 2)
		ticalcs_info("  PC->TI: Virtual Packet Data Acknowledgement");
#endif
	}

	return ret;
}

int TICALL dusb_recv_acknowledge(CalcHandle *handle)
{
	DUSBRawPacket raw;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	memset(&raw, 0, sizeof(raw));

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	do
	{
		ret = dusb_recv(handle, &raw);
		if (ret)
		{
			break;
		}
#if (VPKT_DBG == 2)
		ticalcs_info("  TI->PC: Virtual Packet Data Acknowledgement");
#endif

		if (raw.size != 2 && raw.size != 4)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}

		if (raw.type == DUSB_RPKT_BUF_SIZE_REQ)
		{
			uint32_t size;

			if (raw.size != 4)
			{
				ret = ERR_INVALID_PACKET;
				break;
			}

			size = (((uint32_t)raw.data[0]) << 24) | (((uint32_t)raw.data[1]) << 16) | (((uint32_t)raw.data[2]) << 8) | (((uint32_t)raw.data[3]) << 0);
			ticalcs_info("  TI->PC: Buffer Size Request (%i bytes)", size);

			ret = dusb_send_buf_size_alloc(handle, size);
			if (ret)
			{
				break;
			}

			ret = dusb_recv(handle, &raw);
			if (ret)
			{
				break;
			}
		}

		if (raw.type != DUSB_RPKT_VIRT_DATA_ACK)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}

		if (raw.data[0] != 0xE0 && raw.data[1] != 0x00)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}
	} while(0);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

// Work around TI's OS behaviour: extra bulk write of 0 size required after the last raw packet in a transfer,
// when some conditions are met.
static void workaround_send(CalcHandle *handle, DUSBRawPacket *raw, DUSBVirtualPacket *vtl)
{
	uint8_t buf[64];

	ticalcs_info("workaround_send: vtl->size=%d\traw->size=%d", vtl->size, raw->size);

	if (handle->model == CALC_TI89T_USB)
	{
		// A 1076-byte (string) variable doesn't require this workaround, but bigger (string) variables do.
		if (vtl->size > 1076 && ((raw->size + 5) % 64) == 0)
		{
			ticalcs_info("XXX triggering an extra bulk write\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_send(handle->cable, buf, 0);
		}
	}
	else if (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB || handle->model == CALC_TI84PT_USB)
	{
		// A 244-byte (program) variable doesn't require this workaround, but bigger (program) variables do.
		if (raw->type == DUSB_RPKT_VIRT_DATA_LAST && vtl->size > 244 && (vtl->size % 250) == 244)
		{
			ticalcs_info("XXX triggering an extra bulk write\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_send(handle->cable, buf, 0);
		}
	}
	else if (handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB)
	{
		if (raw->type == DUSB_RPKT_VIRT_DATA_LAST && ((((raw->size + 5) % 64) == 0)/* || (vtl->size > 1018 && ((vtl->size + 6) % 1018) == 0)*/))
		{
			ticalcs_info("XXX triggering an extra bulk write\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_send(handle->cable, buf, 0);
		}
	}
	else
	{
		ticalcs_warning("XXX unhandled model in workaround_send");
	}
}

// Fragmentation of packets

int TICALL dusb_send_data(CalcHandle *handle, DUSBVirtualPacket *vtl)
{
	DUSBRawPacket raw;
	int i, r, q;
	long offset;
	int ret = 0;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vtl);
	if (vtl->size && !vtl->data)
	{
		return ERR_INVALID_PARAMETER;
	}

	memset(&raw, 0, sizeof(raw));

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_DUSB_VPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_vpkt(&event, /* size */ vtl->size, /* type */ vtl->type, /* data */ vtl->data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		do
		{
			if (vtl->size <= handle->priv.dusb_rpkt_maxlen - DUSB_DH_SIZE)
			{
				// we have a single packet which is the last one, too
				raw.size = vtl->size + DUSB_DH_SIZE;
				raw.type = DUSB_RPKT_VIRT_DATA_LAST;

				raw.data[0] = MSB(MSW(vtl->size));
				raw.data[1] = LSB(MSW(vtl->size));
				raw.data[2] = MSB(LSW(vtl->size));
				raw.data[3] = LSB(LSW(vtl->size));
				raw.data[4] = MSB(vtl->type);
				raw.data[5] = LSB(vtl->type);
				if (vtl->data)
				{
					memcpy(&raw.data[DUSB_DH_SIZE], vtl->data, vtl->size);
				}

				ret = dusb_send(handle, &raw);
				if (ret)
				{
					break;
				}
#if (VPKT_DBG == 2)
				ticalcs_info("  PC->TI: Virtual Packet Data Final\n\t\t(size = %08x, type = %s)", vtl->size, dusb_vpkt_type2name(vtl->type));
#elif (VPKT_DBG == 1)
				ticalcs_info("  PC->TI: %s", dusb_vpkt_type2name(vtl->type));
#endif
				workaround_send(handle, &raw, vtl);
				ret = dusb_recv_acknowledge(handle);
				if (ret)
				{
					break;
				}
			}
			else
			{
				// we have more than one packet: first packet has data header
				raw.size = handle->priv.dusb_rpkt_maxlen;
				raw.type = DUSB_RPKT_VIRT_DATA;

				raw.data[0] = MSB(MSW(vtl->size));
				raw.data[1] = LSB(MSW(vtl->size));
				raw.data[2] = MSB(LSW(vtl->size));
				raw.data[3] = LSB(LSW(vtl->size));
				raw.data[4] = MSB(vtl->type);
				raw.data[5] = LSB(vtl->type);
				memcpy(&raw.data[DUSB_DH_SIZE], vtl->data, handle->priv.dusb_rpkt_maxlen - DUSB_DH_SIZE);
				offset = handle->priv.dusb_rpkt_maxlen - DUSB_DH_SIZE;

				ret = dusb_send(handle, &raw);
				if (ret)
				{
					break;
				}
#if (VPKT_DBG == 2)
				ticalcs_info("  PC->TI: Virtual Packet Data with Continuation\n\t\t(size = %08x, type = %s)", vtl->size, dusb_vpkt_type2name(vtl->type));
#elif (VPKT_DBG == 1)
				ticalcs_info("  PC->TI: %s", dusb_vpkt_type2name(vtl->type));
#endif
				ret = dusb_recv_acknowledge(handle);
				if (ret)
				{
					break;
				}

				// other packets doesn't have data header but last one has a different type
				q = (vtl->size - offset) / handle->priv.dusb_rpkt_maxlen;
				r = (vtl->size - offset) % handle->priv.dusb_rpkt_maxlen;

				// send full chunks (no header)
				for (i = 1; i <= q; i++)
				{
					raw.size = handle->priv.dusb_rpkt_maxlen;
					raw.type = (i != q || r != 0) ? DUSB_RPKT_VIRT_DATA : DUSB_RPKT_VIRT_DATA_LAST;
					memcpy(raw.data, vtl->data + offset, handle->priv.dusb_rpkt_maxlen);
					offset += handle->priv.dusb_rpkt_maxlen;

					ret = dusb_send(handle, &raw);
					if (ret)
					{
						goto end;
					}
#if (VPKT_DBG == 2)
					ticalcs_info("  PC->TI: Virtual Packet Data with Continuation");
#endif
					ret = dusb_recv_acknowledge(handle);
					if (ret)
					{
						goto end;
					}

					handle->updat->max1 = vtl->size;
					handle->updat->cnt1 += handle->priv.dusb_rpkt_maxlen;
					handle->updat->pbar();
				}

				// send last chunk (type)
				if (r != 0)
				{
					raw.size = r;
					raw.type = DUSB_RPKT_VIRT_DATA_LAST;
					memcpy(raw.data, vtl->data + offset, r);
					offset += r;

					ret = dusb_send(handle, &raw);
					if (ret)
					{
						break;
					}

#if (VPKT_DBG == 2)
					ticalcs_info("  PC->TI: Virtual Packet Data Final");
#endif
					// XXX is that workaround necessary on 83PCE/84+CE/84+CE-T ?
					if (handle->model != CALC_TI84P_USB && handle->model != CALC_TI84PC_USB && handle->model != CALC_TI82A_USB && handle->model != CALC_TI84PT_USB)
					{
						workaround_send(handle, &raw, vtl);
					}
					ret = dusb_recv_acknowledge(handle);
					if (ret)
					{
						break;
					}
				}
				else
				{
					ticalcs_info("XXX r = 0");
				}
			}
		} while(0);
	}
end:

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_DUSB_VPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_vpkt(&event, /* size */ vtl->size, /* type */ vtl->type, /* data */ vtl->data);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

// Work around TI's OS behaviour: extra bulk read of 0 size required after the last raw packet in a transfer,
// when some conditions are met.
static void workaround_recv(CalcHandle *handle, DUSBRawPacket * raw, DUSBVirtualPacket * vtl)
{
	uint8_t buf[64];

	ticalcs_info("workaround_recv: vtl->size=%d\traw->size=%d", vtl->size, raw->size);

	if (handle->model == CALC_TI89T_USB)
	{
		if ((raw->size % 64) == 0)
		{
			ticalcs_info("XXX triggering an extra bulk read\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_recv(handle->cable, buf, 0);
		}
	}
	else if (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB || handle->model == CALC_TI84PT_USB)
	{
		if (((raw->size + 5) % 64) == 0)
		{
			ticalcs_info("XXX triggering an extra bulk read\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_recv(handle->cable, buf, 0);
		}
	}
	else if (handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB)
	{
		// These models don't seem to need receive workarounds.
	}
	else
	{
		ticalcs_warning("XXX unhandled model in workaround_recv");
	}
}

// beware: data field may be re-allocated in size !
int TICALL dusb_recv_data_varsize(CalcHandle* handle, DUSBVirtualPacket* vtl, uint32_t* declared_size, uint32_t est_size)
{
	DUSBRawPacket raw;
	int i = 0;
	unsigned long alloc_size;
	int ret = 0;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vtl);
	VALIDATE_NONNULL(declared_size);

	memset(&raw, 0, sizeof(raw));

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_DUSB_VPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_vpkt(&event, /* size */ 0, /* type */ 0, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		do
		{
			ret = dusb_recv(handle, &raw);
			if (ret)
			{
				break;
			}

			if (raw.type != DUSB_RPKT_VIRT_DATA && raw.type != DUSB_RPKT_VIRT_DATA_LAST)
			{
				ticalcs_critical("Unexpected raw packet type");
				ret = ERR_INVALID_PACKET;
				break;
			}

			if (!i++)
			{
				// first packet has a data header
				if (raw.size < DUSB_DH_SIZE)
				{
					ticalcs_critical("First raw packet is too small");
					ret = ERR_INVALID_PACKET;
					break;
				}

				if (raw.size > sizeof(raw.data))
				{
					ticalcs_critical("Raw packet is too large: %u bytes", raw.size);
					ret = ERR_INVALID_PACKET;
					break;
				}

				*declared_size = (((uint32_t)raw.data[0]) << 24) | (((uint32_t)raw.data[1]) << 16) | (((uint32_t)raw.data[2]) << 8) | (((uint32_t)raw.data[3]) << 0);
				alloc_size = (*declared_size > 10000 ? 10000 : *declared_size);

				if (alloc_size < raw.size - DUSB_DH_SIZE)
				{
					alloc_size = raw.size - DUSB_DH_SIZE;
				}

				vtl->type = (((uint16_t)raw.data[4]) << 8) | (raw.data[5] << 0);
				vtl->data = (uint8_t *)g_realloc(vtl->data, alloc_size);
				if (vtl->data != NULL)
				{
					memcpy(vtl->data, &raw.data[DUSB_DH_SIZE], raw.size - DUSB_DH_SIZE);
				}
				vtl->size = raw.size - DUSB_DH_SIZE;
#if (VPKT_DBG == 2)
				ticalcs_info("  TI->PC: %s\n\t\t(size = %08x, type = %s)",
					raw.type == DUSB_RPKT_VIRT_DATA_LAST ? "Virtual Packet Data Final" : "Virtual Packet Data with Continuation",
					*declared_size, dusb_vpkt_type2name(vtl->type));
#elif (VPKT_DBG == 1)
				ticalcs_info("  TI->PC: %s", dusb_vpkt_type2name(vtl->type));
#endif
				if (vtl->data != NULL && vtl->type == 0xEE00)
				{
					ticalcs_info("    Error Code : %04x\n", (((int)vtl->data[0]) << 8) | vtl->data[1]);
				}
			}
			else
			{
				// others have more data

				if (vtl->size + raw.size > alloc_size)
				{
					if (vtl->size + raw.size <= est_size)
					{
						alloc_size = est_size;
					}
					else
					{
						alloc_size = (vtl->size + raw.size) * 2;
					}
					vtl->data = (uint8_t *)g_realloc(vtl->data, alloc_size);
				}

				memcpy(vtl->data + vtl->size, raw.data, raw.size);
				vtl->size += raw.size;
#if (VPKT_DBG == 2)
				ticalcs_info("  TI->PC: %s", raw.type == DUSB_RPKT_VIRT_DATA_LAST ? "Virtual Packet Data Final" : "Virtual Packet Data with Continuation");
#endif

				if (raw.type == DUSB_RPKT_VIRT_DATA_LAST)
				{
					handle->updat->max1 = vtl->size;
				}
				else if (vtl->size < *declared_size)
				{
					handle->updat->max1 = *declared_size;
				}
				else if (vtl->size < est_size)
				{
					handle->updat->max1 = est_size;
				}
				else
				{
					handle->updat->max1 = vtl->size + raw.size;
				}
			}

			workaround_recv(handle, &raw, vtl);

			ret = dusb_send_acknowledge(handle);
			if (ret)
			{
				break;
			}

		} while (raw.type != DUSB_RPKT_VIRT_DATA_LAST);
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_DUSB_VPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_vpkt(&event, /* size */ vtl->size, /* type */ vtl->type, /* data */ vtl->data);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL dusb_recv_data(CalcHandle* handle, DUSBVirtualPacket* vtl)
{
	uint32_t declared_size;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vtl);

	// dusb_recv_data_varsize() takes care of handle->busy.

	ret = dusb_recv_data_varsize(handle, vtl, &declared_size, 0);
	// TODO MAYBE reimplement the following block as a temporary event hook ?
	if (!ret)
	{
		if (declared_size != vtl->size)
		{
			ticalcs_warning("invalid packet (declared size = %d, actual size = %d)", declared_size, vtl->size);
			ret = ERR_INVALID_PACKET;
		}
	}

	return ret;
}
