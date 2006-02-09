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

#define PT_HANDSHAKE	0x01
#define PT_RESPONSE		0x02
#define PT_DATA			0x03
#define PT_DATA_LAST	0x04
#define PT_ACK			0x05

// Data Types (or opcodes)

#define DT_NONE			0x0000

/*********/
/* Types */
/*********/

/*
	Format:

    |				   |		(250 bytes max)								 |
	| packet header    | data hdr (1st pkt)  | data	(244 bytes max)			 |
	| size		  | ty | size		 | code	 |								 |
	|			  |    |			 |		 |								 |
	| 00 00 00 10 | 04 | 00 00 00 0A | 00 01 | 00 03 00 01 00 00 00 00 07 D0 |

	Examples:

	00 00 00 05 | 02 | 00 00 00 fa
	00 00 00 02 | 05 | e0 00
	LL LL LL LL | 03 | hdr, data
	LL LL LL LL | 04 | data
*/

typedef struct
{
	uint8_t		type;
	uint16_t	size;
	uint8_t*	data;
} Packet;

typedef struct
{
	uint32_t	size;	// size information
	uint16_t	code;	// opcode
	uint8_t		data[244];
} DataHdr;

typedef struct
{
	uint32_t	size;	// length of data
	uint8_t		type;	// packet type

	union
	{
		uint8_t	d[250];	// used for pure data (no header)
		DataHdr	h;		// used for data with header
	} data;

} PacketHdr;

typedef struct
{
	uint32_t	size;
	uint8_t		type;
	uint8_t		data[250];
} UsbPacket;

/*************/
/* Functions */
/*************/

// layer 0 (manage raw packets)

int send_dusb(CalcHandle* cable, UsbPacket* pkt);
int recv_dusb(CalcHandle* cable, UsbPacket* pkt);

// layer 1 (split into packets)

///...

#endif