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

#ifndef __TICALCS_DUSB__
#define __TICALCS_DUSB__

/*************/
/* Constants */
/*************/

// Packet Types

#define PKT_HANDSHAKE	0x01
#define PKT_RESPONSE	0x02
#define PKT_DATA		0x03
#define PKT_LAST		0x04
#define PKT_ACK			0x05

/*********/
/* Types */
/*********/

typedef struct
{
	uint32_t	size;	// size of packet
	uint8_t		type;	// type of packet
} PacketHdr;


typedef struct
{
	uint32_t	size;	// size of data
	uint16_t	code;	// opcode
} DataHdr;

typedef struct
{
	uint32_t	size;	// size of packet
	uint8_t		type;	// type of packet

	union
	{
		DataHdr		hdr;		// used for data with header (first block)
		uint8_t		data[1023];	// used for pure data (no data header)
	};
} UsbPacket;

/*************/
/* Functions */
/*************/

int dusb_send(CalcHandle* cable, UsbPacket* pkt);
int dusb_recv(CalcHandle* cable, UsbPacket* pkt);

#endif