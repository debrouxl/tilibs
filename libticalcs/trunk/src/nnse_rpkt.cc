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
	This unit manages raw packets from/to Nspire CX II thru DirectLink cable.
	Documentation & credits can be found at <http://hackspire.unsads.com/USB_Protocol>.
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "nnse_rpkt.h"
#include "logging.h"
#include "error.h"

#define VPKT_DBG	1	// 1 = verbose, 2 = more verbose

typedef struct
{
	uint8_t		id;
	const char*	name;
} NNSEServiceId;

typedef struct
{
	uint8_t		id;
	const char*	name;
} NNSEAddress;

static const NNSEAddress nnseaddrs[] =
{
	{ 0x01, "CALC" },
	{ 0xFE, "ME" },
	{ 0xFF, "ALL" },
};

static const NNSEServiceId nnsesids[] =
{
	{ 0x01, "Address Request" },
	{ 0x02, "Time" },
	{ 0x03, "Echo" },
	{ 0x04, "Stream" },
	{ 0x05, "Transmit" },
	{ 0x06, "Loopback" },
	{ 0x07, "Stats" },
	{ 0x08, "Unknown" },
	{ 0x80, "ACK Flag" },
};

TIEXPORT3 const char* TICALL nnse_addr2name(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(nnseaddrs) / sizeof(nnseaddrs[0]); i++)
	{
		if (id == nnseaddrs[i].id || id == nnseaddrs[i].id + 0x80)
		{
			return nnseaddrs[i].name;
		}
	}

	return "";
}

TIEXPORT3 const char* TICALL nnse_sid2name(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(nnsesids) / sizeof(nnsesids[0]); i++)
	{
		if (id == nnsesids[i].id || id == nnseaddrs[i].id + 0x80)
		{
			return nnsesids[i].name;
		}
	}

	return "";
}

// UDP checksum implementation
static uint16_t compute_checksum(const uint8_t *data, uint32_t size)
{
	uint32_t acc = 0;

	if (size > 0)
	{
		for (uint32_t i = 0; i < size - 1; i += 2)
		{
			uint16_t cur = (((uint16_t)data[i]) << 8) | data[i + 1];
			acc += cur;
		}
		if (size & 1)
		{
			acc += (((uint16_t)data[size - 1]) << 8);
		}

		acc = (acc & 0xFFFF) + (acc >> 16);
		acc = ~acc;
	}

	return (uint16_t)acc;
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
	ticalcs_info("%s", str);
#endif
#if (VPKT_DBG == 2)
	char *str = (char *)g_malloc(3*size + 8 + 10);
	uint32_t i, j, k;
	int step = 12;

	for (k = 0; k < 4; k++)
	{
		str[k] = ' ';
	}

	for (i = j = 0; i < size; i++, j++)
	{
		if (i && !(i % step))
		{
			ticalcs_info(str);
			j = 0;
		}

		sprintf(&str[3*j+4], "%02X ", data[i]);
	}
	ticalcs_info("%s", str);

	g_free(str);
#endif
	return 0;
}

TIEXPORT3 int TICALL nnse_send(CalcHandle* handle, NNSERawPacket* pkt)
{
	int ret;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(pkt);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_NNSE_RPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nnse_rpkt(&event, /* unused1 */ pkt->unused1, /* service */ pkt->service, /* src_addr */ pkt->src_addr, /* dst_addr */ pkt->dst_addr,
	                             /* unknown2 */ pkt->unknown2, /* req_ack */ pkt->req_ack, /* size */ pkt->size, /* seq */ pkt->seq, /* csum */ pkt->csum, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		uint8_t buf[sizeof(NNSERawPacket)] = { 0 };
		uint32_t size = pkt->size + NNSE_HEADER_SIZE;
		uint32_t csum;

		csum = compute_checksum((uint8_t*)pkt, NNSE_HEADER_SIZE - 2);
		csum += compute_checksum(pkt->data, pkt->size);
		if (csum > 0xFFFF)
		{
			csum = (csum & 0xFFFF) + (csum >> 16);
		}
		pkt->csum = (uint16_t)csum;

		pkt->seq = handle->priv.nnse_seq_pc;
		handle->priv.nnse_seq_pc++;

			ticalcs_info("   %02X: %02X->%02X RA=%02X SQ=%04X CK=%04X U12=%02X,%02X (%u bytes)\n",
			             pkt->service, pkt->src_addr, pkt->dst_addr, pkt->req_ack,
			             pkt->seq, pkt->csum, pkt->unused1, pkt->unknown2, pkt->size);
		if (pkt->size)
		{
			hexdump(pkt->data, pkt->size);
		}

		buf[0] = pkt->unused1;
		buf[1] = pkt->service;
		buf[2] = pkt->src_addr;
		buf[3] = pkt->dst_addr;
		buf[4] = pkt->unknown2;
		buf[5] = pkt->req_ack;
		buf[6] = MSB(pkt->size);
		buf[7] = LSB(pkt->size);
		buf[8] = MSB(pkt->seq);
		buf[9] = LSB(pkt->seq);
		buf[10] = MSB(pkt->csum);
		buf[11] = LSB(pkt->csum);

		memcpy(buf + NNSE_HEADER_SIZE, pkt->data, pkt->size);

		ticables_progress_reset(handle->cable);
		ret = ticables_cable_send(handle->cable, buf, size);
		if (!ret)
		{
			if (size >= 128)
			{
				ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			}

			if (handle->updat->cancel)
			{
				ret = ERR_ABORT;
			}
		}
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_NNSE_RPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nnse_rpkt(&event, /* unused1 */ pkt->unused1, /* service */ pkt->service, /* src_addr */ pkt->src_addr, /* dst_addr */ pkt->dst_addr,
	                             /* unknown2 */ pkt->unknown2, /* req_ack */ pkt->req_ack, /* size */ pkt->size, /* seq */ pkt->seq, /* csum */ pkt->csum, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

TIEXPORT3 int TICALL nnse_recv(CalcHandle* handle, NNSERawPacket* pkt)
{
	int ret;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(pkt);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_NNSE_RPKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nnse_rpkt(&event, /* unused1 */ 0, /* service */ 0, /* src_addr */ 0, /* dst_addr */ 0, /* unknown2 */ 0, /* req_ack */ 0,
	                            /* size */ 0, /* seq */ 0, /* csum */ 0, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		uint8_t buf[NNSE_HEADER_SIZE];
		ticables_progress_reset(handle->cable);
		ret = ticables_cable_recv(handle->cable, buf, NNSE_HEADER_SIZE);
		while (!ret)
		{
			pkt->unused1   = buf[0];
			pkt->service   = buf[1];
			pkt->src_addr  = buf[2];
			pkt->dst_addr  = buf[3];
			pkt->unknown2  = buf[4];
			pkt->req_ack   = buf[5];
			pkt->size      = (((uint16_t)buf[6]) << 8) | buf[7];
			pkt->seq       = (((uint16_t)buf[8]) << 8) | buf[9];
			pkt->csum      = (((uint16_t)buf[10]) << 8) | buf[11];

			// TODO take seq / ACK into account correctly.
			handle->priv.nnse_seq = pkt->seq;

			// Next, follows data
			if (pkt->size)
			{
				ret = ticables_cable_recv(handle->cable, pkt->data, pkt->size);
				if (ret)
				{
					break;
				}

				if (pkt->size >= 256)
				{
					ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
				}
			}

			if (handle->updat->cancel)
			{
				ret = ERR_ABORT;
				break;
			}

			ticalcs_info("   %02X: %02X->%02X RA=%02X SQ=%04X CK=%04X U12=%02X,%02X (%u bytes)\n",
			             pkt->service, pkt->src_addr, pkt->dst_addr, pkt->req_ack,
			             pkt->seq, pkt->csum, pkt->unused1, pkt->unknown2, pkt->size);
			if (pkt->size)
			{
				hexdump(pkt->data, pkt->size);
			}

			break;
		}
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_NNSE_RPKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_nnse_rpkt(&event, /* unused1 */ pkt->unused1, /* service */ pkt->service, /* src_addr */ pkt->src_addr, /* dst_addr */ pkt->dst_addr,
	                             /* unknown2 */ pkt->unknown2, /* req_ack */ pkt->req_ack, /* size */ pkt->size, /* seq */ pkt->seq, /* csum */ pkt->csum, /* data */ pkt->data);
	ret = ticalcs_event_send(handle, &event);

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

TIEXPORT3 int TICALL nnse_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len, uint8_t ep)
{
	int ret = 0;
	uint8_t unused1;
	uint8_t service;
	uint8_t src_addr;
	uint8_t dst_addr;
	uint8_t unknown2;
	uint8_t req_ack;
	uint16_t size;
	uint16_t seq;
	uint16_t csum;
	uint32_t i;
	uint32_t computed_csum;

	VALIDATE_NONNULL(f);
	VALIDATE_NONNULL(data);

	if (len < NNSE_HEADER_SIZE || len > NNSE_DATA_SIZE)
	{
		ticalcs_critical("Length %lu (%lX) is too small or too large for a valid NNSE raw packet", (unsigned long)len, (unsigned long)len);
		return ERR_INVALID_PACKET;
	}

	unused1   = data[0];
	service   = data[1];
	src_addr  = data[2];
	dst_addr  = data[3];
	unknown2  = data[4];
	req_ack   = data[5];
	size      = (((uint16_t)data[6]) << 8) | data[7];
	seq       = (((uint16_t)data[8]) << 8) | data[9];
	csum      = (((uint16_t)data[10]) << 8) | data[11];

	fprintf(f, "%08lX\t| %s: srv (%02X - %s): %02X - %s -> %02X - %s\n", (unsigned long)len, ep_way(ep),
	           service, nnse_sid2name(service), src_addr, nnse_addr2name(src_addr),
	           dst_addr, nnse_addr2name(dst_addr));
	fprintf(f, "\t  unused1=%02X unknown2=%02X req_ack=%02X seq=%04X checksum=%04X (%u bytes)\n",
	           unused1, unknown2, req_ack, seq, csum, size);

	if (len != (uint32_t)size)
	{
		ticalcs_critical("Data size %u (%X) is incoherent with given NNSE raw packet length %lu (%lX)", size, size, (unsigned long)len, (unsigned long)len);
		return ERR_INVALID_PACKET;
	}

	computed_csum = compute_checksum(data, NNSE_HEADER_SIZE - 2);
	computed_csum += compute_checksum(data + NNSE_HEADER_SIZE, len - NNSE_HEADER_SIZE);
	if (computed_csum > 0xFFFF)
	{
		computed_csum = (computed_csum & 0xFFFF) + (computed_csum >> 16);
	}

	fprintf(f, "\t  computed_csum=%04X\n", computed_csum);
	if (computed_csum != csum)
	{
		fprintf(f, "\t  (NOTE: data checksum does not match header)\n");
	}

	data += NNSE_HEADER_SIZE;
	len -= NNSE_HEADER_SIZE;
	fprintf(f, "\t\t");
	for (i = 0; i < len;)
	{
		fprintf(f, "%02X ", *data++);
		if (!(++i & 15))
		{
			fprintf(f, "\n\t\t");
		}
	}
	fputc('\n', f);

	return ret;
}
