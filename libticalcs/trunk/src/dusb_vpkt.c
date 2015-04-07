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

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#include "dusb_rpkt.h"
#include "dusb_vpkt.h"

#define VPKT_DBG	1	// 1 = verbose, 2 = more verbose

// Pseudo-Constants

static unsigned int DATA_SIZE = 250;	// max length of data in raw packet

// Type to string

static const DUSBVtlPktName vpkt_types[] = 
{
	{ 0x0000, ""},
	{ 0x0001, "Ping / Set Mode"},
	{ 0x0002, "Begin OS Transfer"},
	{ 0x0003, "Acknowledgement of OS Transfer"},
	{ 0x0004, "OS Header"},
	{ 0x0005, "OS Data"},
	{ 0x0006, "Acknowledgement of EOT"},
	{ 0x0007, "Parameter Request"},
	{ 0x0008, "Parameter Data"},
	{ 0x0009, "Request Directory Listing"},
	{ 0x000A, "Variable Header"},
	{ 0x000B, "Request to Send"},
	{ 0x000C, "Request Variable"},
	{ 0x000D, "Variable Contents"},
	{ 0x000E, "Parameter Set"},
	{ 0x000F, ""},
	{ 0x0010, "Modify Variable"},
	{ 0x0011, "Remote Control"},
	{ 0x0012, "Acknowledgement of Mode Setting"},
	{ 0xAA00, "Acknowledgement of Data"},
	{ 0xBB00, "Delay Acknowledgment"},
	{ 0xDD00, "End of Transmission"},
	{ 0xEE00, "Error"},
	{ -1, NULL},
};

TIEXPORT3 const char* TICALL dusb_vpkt_type2name(uint16_t id)
{
	const DUSBVtlPktName *p;

	for (p = vpkt_types; p->name != NULL; p++)
	{
		if (p->id == id)
		{
			return p->name;
		}
	}

	return "unknown: not listed";
}

// Buffer allocation

static GList *vtl_pkt_list = NULL;

TIEXPORT3 DUSBVirtualPacket* TICALL dusb_vtl_pkt_new(uint32_t size, uint16_t type)
{
	DUSBVirtualPacket* vtl = g_malloc0(sizeof(DUSBVirtualPacket)); // aborts the program if it fails.

	vtl->size = size;
	vtl->type = type;
	vtl->data = g_malloc0(size + DUSB_DH_SIZE); // aborts the program if it fails.

	vtl_pkt_list = g_list_append(vtl_pkt_list, vtl);

	return vtl;
}

TIEXPORT3 void TICALL dusb_vtl_pkt_del(DUSBVirtualPacket* vtl)
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

void dusb_vtl_pkt_purge(void)
{
	g_list_foreach(vtl_pkt_list, (GFunc)dusb_vtl_pkt_del, NULL);
	g_list_free(vtl_pkt_list);
	vtl_pkt_list = NULL;
}

// Raw packets

TIEXPORT3 int TICALL dusb_send_buf_size_request(CalcHandle* handle, uint32_t size)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle)

	memset(&raw, 0, sizeof(raw));
	raw.size = 4;
	raw.type = DUSB_RPKT_BUF_SIZE_REQ;
	raw.data[2] = MSB(size);
	raw.data[3] = LSB(size);

	ret = dusb_send(handle, &raw);
	if (!ret)
	{
		ticalcs_info("  PC->TI: Buffer Size Request (%i bytes)", size);
	}

	return ret;
}

TIEXPORT3 int TICALL dusb_recv_buf_size_alloc(CalcHandle* handle, uint32_t *size)
{
	DUSBRawPacket raw;
	uint32_t tmp;
	int ret = 0;

	VALIDATE_HANDLE(handle)

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
			ticalcs_critical("Clamping overly large buffer size allocation to %d bytes", sizeof(raw.data));
			tmp = sizeof(raw.data);
		}
		if (size)
		{
			*size = tmp;
		}
		ticalcs_info("  TI->PC: Buffer Size Allocation (%i bytes)", tmp);

		DATA_SIZE = tmp;
	} while(0);

	return ret;
}

TIEXPORT3 int TICALL dusb_recv_buf_size_request(CalcHandle* handle, uint32_t *size)
{
	DUSBRawPacket raw;
	uint32_t tmp;
	int ret = 0;

	VALIDATE_HANDLE(handle)

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

TIEXPORT3 int TICALL dusb_send_buf_size_alloc(CalcHandle* handle, uint32_t size)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle)

	memset(&raw, 0, sizeof(raw));
	raw.size = 4;
	raw.type = DUSB_RPKT_BUF_SIZE_ALLOC;
	raw.data[2] = MSB(size);
	raw.data[3] = LSB(size);

	ret = dusb_send(handle, &raw);
	if (!ret)
	{
		ticalcs_info("  PC->TI: Buffer Size Allocation (%i bytes)", size);
	}

	DATA_SIZE = size;

	return ret;
}

TIEXPORT3 uint32_t TICALL dusb_get_buf_size(void)
{
	return DATA_SIZE;
}

TIEXPORT3 int TICALL dusb_send_acknowledge(CalcHandle* handle)
{
	DUSBRawPacket raw;
	int ret;

	VALIDATE_HANDLE(handle)

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

TIEXPORT3 int TICALL dusb_recv_acknowledge(CalcHandle *handle)
{
	DUSBRawPacket raw;
	int ret = 0;

	VALIDATE_HANDLE(handle)

	memset(&raw, 0, sizeof(raw));
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
	else if (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB)
	{
		// A 244-byte (program) variable doesn't require this workaround, but bigger (program) variables do.
		if (raw->type == DUSB_RPKT_VIRT_DATA_LAST && vtl->size > 244 && (vtl->size % 250) == 244)
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

TIEXPORT3 int TICALL dusb_send_data(CalcHandle *handle, DUSBVirtualPacket *vtl)
{
	DUSBRawPacket raw;
	int i, r, q;
	long offset;
	int ret;

	VALIDATE_HANDLE(handle)
	VALIDATE_NONNULL(vtl)

	memset(&raw, 0, sizeof(raw));

	do
	{
		if (vtl->size <= DATA_SIZE - DUSB_DH_SIZE)
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
			memcpy(&raw.data[DUSB_DH_SIZE], vtl->data, vtl->size);

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
			raw.size = DATA_SIZE;
			raw.type = DUSB_RPKT_VIRT_DATA;

			raw.data[0] = MSB(MSW(vtl->size));
			raw.data[1] = LSB(MSW(vtl->size));
			raw.data[2] = MSB(LSW(vtl->size));
			raw.data[3] = LSB(LSW(vtl->size));
			raw.data[4] = MSB(vtl->type);
			raw.data[5] = LSB(vtl->type);
			memcpy(&raw.data[DUSB_DH_SIZE], vtl->data, DATA_SIZE - DUSB_DH_SIZE);
			offset = DATA_SIZE - DUSB_DH_SIZE;

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
			q = (vtl->size - offset) / DATA_SIZE;
			r = (vtl->size - offset) % DATA_SIZE;

			// send full chunks (no header)
			for (i = 1; i <= q; i++)
			{
				raw.size = DATA_SIZE;
				raw.type = DUSB_RPKT_VIRT_DATA;
				memcpy(raw.data, vtl->data + offset, DATA_SIZE);
				offset += DATA_SIZE;

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
				handle->updat->cnt1 += DATA_SIZE;
				handle->updat->pbar();
			}

			// send last chunk (type)
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
			if (handle->model != CALC_TI84P_USB && handle->model != CALC_TI84PC_USB && handle->model != CALC_TI82A_USB)
			{
				workaround_send(handle, &raw, vtl);
			}
			ret = dusb_recv_acknowledge(handle);
			if (ret)
			{
				break;
			}
		}
	} while(0);
end:

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
	else if (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB)
	{
		if (((raw->size + 5) % 64) == 0)
		{
			ticalcs_info("XXX triggering an extra bulk read\n\tvtl->size=%d\traw->size=%d", vtl->size, raw->size);
			ticables_cable_recv(handle->cable, buf, 0);
		}
	}
	else
	{
		ticalcs_warning("XXX unhandled model in workaround_recv");
	}
}

// beware: data field may be re-allocated in size !
TIEXPORT3 int TICALL dusb_recv_data_varsize(CalcHandle* handle, DUSBVirtualPacket* vtl, uint32_t* declared_size, uint32_t est_size)
{
	DUSBRawPacket raw;
	int i = 0;
	unsigned long alloc_size;
	int ret;

	VALIDATE_HANDLE(handle)
	VALIDATE_NONNULL(vtl)
	VALIDATE_NONNULL(declared_size)

	memset(&raw, 0, sizeof(raw));

	do
	{
		ret = dusb_recv(handle, &raw);
		if (ret)
		{
			break;
		}

		if (raw.type != DUSB_RPKT_VIRT_DATA && raw.type != DUSB_RPKT_VIRT_DATA_LAST)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}

		if (!i++)
		{
			// first packet has a data header
			if (raw.size < DUSB_DH_SIZE)
			{
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
			vtl->data = g_realloc(vtl->data, alloc_size);
			memcpy(vtl->data, &raw.data[DUSB_DH_SIZE], raw.size - DUSB_DH_SIZE);
			vtl->size = raw.size - DUSB_DH_SIZE;
#if (VPKT_DBG == 2)
			ticalcs_info("  TI->PC: %s\n\t\t(size = %08x, type = %s)", 
				raw.type == DUSB_RPKT_VIRT_DATA_LAST ? "Virtual Packet Data Final" : "Virtual Packet Data with Continuation",
				*declared_size, dusb_vpkt_type2name(vtl->type));
#elif (VPKT_DBG == 1)
			ticalcs_info("  TI->PC: %s", dusb_vpkt_type2name(vtl->type));
#endif
			if(vtl->type == 0xEE00)
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
				vtl->data = g_realloc(vtl->data, alloc_size);
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

			handle->updat->cnt1 = vtl->size;
			handle->updat->pbar();
		}

		workaround_recv(handle, &raw, vtl);

		ret = dusb_send_acknowledge(handle);
		if (ret)
		{
			break;
		}

	} while (raw.type != DUSB_RPKT_VIRT_DATA_LAST);

	//printf("dusb_recv_data: rpkt.size=%d\n", raw.size);

	return ret;
}

TIEXPORT3 int TICALL dusb_recv_data(CalcHandle* handle, DUSBVirtualPacket* vtl)
{
	uint32_t declared_size;
	int ret;

	VALIDATE_HANDLE(handle)
	VALIDATE_NONNULL(vtl)

	ret = dusb_recv_data_varsize(handle, vtl, &declared_size, 0);
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
