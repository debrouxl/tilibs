/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.h 1179 2005-06-06 14:42:32Z roms $ */

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

#ifndef __DUSB_RPKT__
#define __DUSB_RPKT__

#define MAX_RAW_SIZE	1023

// Raw packet types

#define RPKT_BUF_SIZE_REQ		1
#define RPKT_BUF_SIZE_ALLOC		2

#define RPKT_VIRT_DATA			3
#define RPKT_VIRT_DATA_LAST		4
#define RPKT_VIRT_DATA_ACK		5

// Convenient structures

typedef struct
{
	uint32_t	size;	// raw packet size
	uint8_t		type;	// raw packet type
} RawPktHdr;

typedef struct
{
	uint32_t	size;	// raw packet size
	uint8_t		type;	// raw packet type

	uint8_t		data[MAX_RAW_SIZE];	// raw packet data (should be allocated)
} RawPacket;

#define PH_SIZE		(4+1)

// Functions

RawPacket*  raw_pkt_new(uint32_t size);
void		raw_pkt_del(RawPacket* pkt);

int dusb_send(CalcHandle* cable, RawPacket* pkt);
int dusb_recv(CalcHandle* cable, RawPacket* pkt);

#endif