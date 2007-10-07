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

#ifndef __NSP_RPKT__
#define __NSP_RPKT__

// Convenient structures

#define PH_SIZE		16

typedef struct
{
	uint16_t	unused;
	uint16_t	src_addr;
	uint16_t	src_id;
	uint16_t	dst_addr;
	uint16_t	dst_id;
	uint16_t	data_sum;
	uint8_t		data_size;
	uint8_t		ack;
	uint8_t		seq;
	uint8_t		hdr_sum;

	uint8_t		data[254];
} RawPacket;

// Functions

RawPacket*  raw_pkt_new(uint32_t size);
void		raw_pkt_del(RawPacket* pkt);

int nsp_send(CalcHandle* cable, RawPacket* pkt);
int nsp_recv(CalcHandle* cable, RawPacket* pkt);

#endif
