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
*/

typedef struct
{
	uint32_t	size;
	uint16_t	code;
	uint8_t		data[250];
} DataHdr;

typedef struct
{
	uint32_t	size;
	uint8_t		type;
	DataHdr*	data;	// NULL if no data
} PacketHdr;

typedef PacketHdr	UsbPacket;

/*************/
/* Functions */
/*************/

int send_dusb(CalcHandle* cable, UsbPacket* pkt);
int recv_dusb(CalcHandle* cable, UsbPacket* pkt);

#endif