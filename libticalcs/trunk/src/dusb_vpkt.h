/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.h 2074 2006-03-31 08:36:06Z roms $ */

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

#ifndef __DUSB_VPKT__
#define __DUSB_VPKT__

// Convenients structures

#define DH_SIZE		(4+2)	// size + type

typedef struct
{
	uint32_t	size;		// virtual packet size
	uint16_t	type;		// virtual packet type

	uint8_t		*data;		// virtual packet data
} VirtualPacket;

typedef struct
{
	uint16_t	id;
	const char *name;
} VtlPktName;

// Virtual packet types

#define VPKT_PING		0x0001
#define VPKT_OS_BEGIN	0x0002
#define VPKT_OS_ACK		0x0003
#define VPKT_OS_HEADER	0x0004
#define VPKT_OS_DATA	0x0005
#define VPKT_EOT_ACK	0x0006
#define VPKT_PARM_REQ	0x0007
#define VPKT_PARM_DATA	0x0008
#define VPKT_DIR_REQ	0x0009
#define VPKT_VAR_HDR	0x000A
#define VPKT_RTS		0x000B
#define VPKT_VAR_REQ	0x000C
#define VPKT_VAR_CNTS	0x000D
#define VPKT_PARM_SET	0x000E
#define VPKT_DEL_VAR	0x0010
#define VPKT_EXECUTE	0x0011
#define VPKT_MODE_SET	0x0012

#define VPKT_DATA_ACK	0xAA00
#define VPKT_DELAY_ACK	0xBB00
#define VPKT_EOT		0xDD00
#define VPKT_ERROR		0xEE00

// Functions

VirtualPacket*  dusb_vtl_pkt_new(uint32_t size, uint16_t type);
void			dusb_vtl_pkt_del(VirtualPacket* pkt);
void			dusb_vtl_pkt_purge(void);

int dusb_send_buf_size_request(CalcHandle* h, uint32_t size);
int dusb_recv_buf_size_alloc(CalcHandle* h, uint32_t *size);

int dusb_recv_buf_size_request(CalcHandle* h, uint32_t *size);
int dusb_send_buf_size_alloc(CalcHandle* h, uint32_t size);

int dusb_send_data(CalcHandle* h, VirtualPacket* pkt);
int dusb_recv_data(CalcHandle* h, VirtualPacket* pkt);

int dusb_send_acknowledge(CalcHandle* h);
int dusb_recv_acknowledge(CalcHandle *h);

const char* dusb_vpkt_type2name(uint16_t id);

uint32_t dusb_get_buf_size(void);

#endif
