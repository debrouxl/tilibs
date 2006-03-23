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
#define PT_LAST			0x04
#define PT_ACK			0x05

// Data Types (or opcodes)

#define DT_NONE			0x0000

/*********/
/* Types */
/*********/

/*
	Format:

	| packet header    | data (250 bytes max)								 |
	|				   |  or												 |
	| size		  | ty | size		 | code	 | data	(246 bytes)				 |
	|			  |    |			 |		 |								 |
	| 00 00 00 10 | 04 | 00 00 00 0A | 00 01 | 00 03 00 01 00 00 00 00 07 D0 |

	Examples:

	00 00 00 05 | 02 | 00 00 00 fa
	00 00 00 02 | 05 | e0 00
	HH HL LH LL | 03 | hdr, data or data
	HH HL LH LL | 04 | data
*/

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

/*
typedef struct
{
	PacketHdr	ph;

	union {
	DataHdr		dh;			// used for data with header (first block)
	uint8_t		data[250];	// used for pure data (no data header)
	};
} UsbPacket;
*/

typedef struct
{
	uint32_t	size;	// size of packet
	uint8_t		type;	// type of packet

	union {
	DataHdr		hdr;		// used for data with header (first block)
	uint8_t		data[250];	// used for pure data (no data header)
	};
} UsbPacket;

/*************/
/* Functions */
/*************/

// layer 0 (manage simple packets)

int dusb_send(CalcHandle* cable, UsbPacket* pkt);
int dusb_recv(CalcHandle* cable, UsbPacket* pkt);

// layer 1 (manage packet types)

int dusb_send_handshake(CalcHandle *h);
int dusb_recv_response (CalcHandle *h);
int dusb_send_data(CalcHandle *h, uint32_t  size, uint16_t  code, uint8_t *data);
int dusb_recv_data(CalcHandle *h, uint32_t *size, uint16_t *code, uint8_t *data);
int dusb_send_acknowledge(CalcHandle* h);
int dusb_recv_acknowledge(CalcHandle *h);

#endif