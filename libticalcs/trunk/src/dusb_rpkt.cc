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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	This unit manages raw packets from/to D-USB (DirectLink).
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dusb_rpkt.h"
#include "dusb_vpkt.h"
#include "logging.h"
#include "error.h"

typedef struct
{
	uint8_t     id;
	uint8_t     data_hdr;
	uint8_t     data;
	const char* name;
} DUSBRawPacketInfo;

static const DUSBRawPacketInfo dusbrawpackets[] =
{
	{ DUSB_RPKT_BUF_SIZE_REQ, 0, 4, "Buffer Size Request" },
	{ DUSB_RPKT_BUF_SIZE_ALLOC, 0, 4, "Buffer Size Allocation" },
	{ DUSB_RPKT_VIRT_DATA, 1, 6, "Virtual Packet Data with Continuation" },
	{ DUSB_RPKT_VIRT_DATA_LAST, 1, 6, "Virtual Packet Data Final" },
	{ DUSB_RPKT_VIRT_DATA_ACK, 0, 2, "Virtual Packet Data Acknowledgement" }
};

const char* TICALL dusb_rpkt_type2name(uint8_t id)
{
	for (unsigned int i = 0; i < sizeof(dusbrawpackets) / sizeof(dusbrawpackets[0]); i++)
	{
		if (id == dusbrawpackets[i].id)
		{
			return dusbrawpackets[i].name;
		}
	}

	return "";
}

int TICALL dusb_send(CalcHandle* handle, DUSBRawPacket* pkt)
{
	uint8_t buf[sizeof(pkt->data) + 5];
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(pkt);

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_DUSB_RPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_rpkt(&event, /* size */ pkt->size, /* type */ pkt->type, /* data */ pkt->data);
	int ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		memset(buf, 0, sizeof(buf));
		uint32_t size = pkt->size;

		if (size > sizeof(pkt->data))
		{
			size = sizeof(pkt->data);
		}

		buf[0] = MSB(MSW(size));
		buf[1] = LSB(MSW(size));
		buf[2] = MSB(LSW(size));
		buf[3] = LSB(LSW(size));
		buf[4] = pkt->type;
		memcpy(buf + 5, pkt->data, size);

		//printf("dusb_send: pkt->size=%d\n", pkt->size);
		ticables_progress_reset(handle->cable);
		ret = ticables_cable_send(handle->cable, buf, size + 5);
		if (!ret)
		{
			if (size >= 128)
			{
				ticables_progress_get(handle->cable, nullptr, nullptr, &handle->updat->rate);
			}

			if (handle->updat->cancel)
			{
				ret = ERR_ABORT;
			}
		}
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_DUSB_RPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_rpkt(&event, /* size */ pkt->size, /* type */ pkt->type, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL dusb_recv(CalcHandle* handle, DUSBRawPacket* pkt)
{
	uint8_t buf[5];
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(pkt);

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_DUSB_RPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_rpkt(&event, /* size */ 0, /* type */ 0, /* data */ pkt->data);
	int ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		// Any packet has always an header of 5 bytes (size & type)
		ticables_progress_reset(handle->cable);
		ret = ticables_cable_recv(handle->cable, buf, 5);
		while (!ret)
		{

			pkt->size = buf[3] | (((uint32_t)buf[2]) << 8) | (((uint32_t)buf[1]) << 16) | (((uint32_t)buf[0]) << 24);
			pkt->type = buf[4];

			if (   (handle->model == CALC_TI84P_USB || handle->model == CALC_TI84PC_USB || handle->model == CALC_TI82A_USB || handle->model == CALC_TI84PT_USB)
			    && pkt->size > 250)
			{
				ticalcs_warning("Raw packet is unexpectedly large: %u bytes", pkt->size);
			}
			else if (   (handle->model == CALC_TI83PCE_USB || handle->model == CALC_TI84PCE_USB || handle->model == CALC_TI82AEP_USB)
				 && pkt->size > 1018)
			{
				ticalcs_warning("Raw packet is unexpectedly large: %u bytes", pkt->size);
			}
			else if (handle->model == CALC_TI89T_USB)
			{
				// Fall through.
			}
			// else do nothing for now.

			if (pkt->size > sizeof(pkt->data))
			{
				ticalcs_critical("Raw packet is too large: %u bytes", pkt->size);
				ret = ERR_INVALID_PACKET;
				break;
			}

			//printf("dusb_send: pkt->size=%d\n", pkt->size);
			// Next, follows data
			ret = ticables_cable_recv(handle->cable, pkt->data, pkt->size);
			if (!ret)
			{
				if (pkt->size >= 128)
				{
					ticables_progress_get(handle->cable, nullptr, nullptr, &handle->updat->rate);
				}

				if (handle->updat->cancel)
				{
					ret = ERR_ABORT;
				}
			}
			break;
		}
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_DUSB_RPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dusb_rpkt(&event, /* size */ pkt->size, /* type */ pkt->type, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

static const char* ep_way(uint8_t ep)
{
	if (ep == 0x01)
	{
		return "TI>PC";
	}
	else if (ep == 0x02)
	{
		return "PC>TI";
	}
	else
	{
		return "XX>XX";
	}
}

int TICALL dusb_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len, uint8_t ep, uint8_t * first)
{
	int ret = 0;

	VALIDATE_NONNULL(f);
	VALIDATE_NONNULL(data);
	VALIDATE_NONNULL(first);

	if (len < 5 || len > sizeof(((DUSBRawPacket *)nullptr)->data))
	{
		ticalcs_critical("Length %lu (%lX) is too small or too large for a valid DUSB raw packet", (unsigned long)len, (unsigned long)len);
		return ERR_INVALID_PACKET;
	}

	uint32_t raw_size = (((uint32_t)(data[0])) << 24) | (((uint32_t)(data[1])) << 16) | (((uint32_t)(data[2])) << 8) | ((uint32_t)(data[3]));
	const uint8_t raw_type = data[4];

	fprintf(f, "%08lX (%02X)\t\t\t\t\t\t\t| %s: %s\n", (unsigned long)raw_size, (unsigned int)raw_type, ep_way(ep), dusb_rpkt_type2name(raw_type));

	if (raw_size > sizeof(((DUSBRawPacket *)nullptr)->data) - 5)
	{
		ticalcs_critical("Raw size %lu (%lX) is too large for a valid DUSB raw packet", (unsigned long)raw_size, (unsigned long)raw_size);
		return ERR_INVALID_PACKET;
	}

	if (raw_type < 1 || raw_type > 5)
	{
		ticalcs_critical("Raw type unknown in DUSB raw packet");
		return ERR_INVALID_PACKET;
	}

	if (len < 5U + dusbrawpackets[raw_type - 1].data)
	{
		ticalcs_critical("Length %ld is too small for a valid data part in DUSB raw packet of type %u", (unsigned long)len, raw_type);
		return ERR_INVALID_PACKET;
	}

	switch (raw_type)
	{
		case DUSB_RPKT_BUF_SIZE_REQ:
		case DUSB_RPKT_BUF_SIZE_ALLOC:
		{
			const uint32_t tmp = (((uint32_t)(data[5])) << 24) | (((uint32_t)(data[6])) << 16) | (((uint32_t)(data[7])) << 8) | ((uint32_t)(data[8]));
			fprintf(f, "\t[%08lX]\n", (unsigned long)tmp);
			if (len != 5U + 4)
			{
				fputs("(unexpected size for a packet of that type)\n", f);
			}
		}
		break;

		case DUSB_RPKT_VIRT_DATA:
		case DUSB_RPKT_VIRT_DATA_LAST:
		{
			if (*first)
			{
				const uint32_t vtl_size = (((uint32_t)(data[5])) << 24) | (((uint32_t)(data[6])) << 16) | (((uint32_t)(data[7])) << 8) | ((uint32_t)(data[8]));
				const uint16_t vtl_type = (((uint16_t)(data[9])) << 8) | ((uint16_t)(data[10]));
				fprintf(f, "\t%08lX {%04X}\t\t\t\t\t\t| CMD: %s\n", (unsigned long)vtl_size, vtl_type, dusb_vpkt_type2name(vtl_type));

				if (vtl_size != raw_size - 6)
				{
					fputs("(unexpected size for a packet of that type)\n", f);
				}

				if (!vtl_size)
				{
					fputs("(no data to dissect)\n", f);
				}
				else
				{
					data += 11;
					len -= 11;
					if (len < vtl_size)
					{
						fputs("(packet truncated: insufficient given length)\n", f);
					}
					else if (len > vtl_size)
					{
						fputs("(given length larger than length in packet)\n", f);
					}
					fprintf(f, "\t\t");
					for (uint32_t i = 0; i < len;)
					{
						fprintf(f, "%02X ", *data++);
						if (!(++i & 15))
						{
							fprintf(f, "\n\t\t");
						}
					}
					data -= vtl_size;
					fputc('\n', f);
					ret = dusb_dissect_cmd_data(model, f, data, len, vtl_size, vtl_type);
					if (ret)
					{
						fputs("(inner data dissection had a problem with the packet)\n", f);
					}
				}
			}
			else
			{
				fprintf(f, "\t%02X %02X %02X ", data[5], data[6], data[7]);
				data += 8;
				len -= 8;
				if (len < raw_size - 3)
				{
					fputs("(packet truncated: insufficient given length)\n", f);
					raw_size = len;
				}
				else if (len > raw_size - 3)
				{
					fputs("(given length larger than length in packet)\n", f);
				}
				for (uint32_t i = 0; i < len;)
				{
					fprintf(f, "%02X ", *data++);
					if (!(++i & 15))
					{
						fprintf(f, "\n\t\t");
					}
				}
				fputc('\n', f);
			}
			*first = (raw_type == DUSB_RPKT_VIRT_DATA) ? 0 : 1;
		}
		break;


		case DUSB_RPKT_VIRT_DATA_ACK:
		{
			const uint16_t tmp = (((uint16_t)(data[5])) << 8) | ((uint16_t)(data[6]));
			fprintf(f, "\t[%04X]\n", tmp);
			if (len != 5 + 2)
			{
				fputs("(unexpected size for a packet of that type)\n", f);
			}
		}
		break;
	}

	return ret;
}
