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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __NSP_VPKT__
#define __NSP_VPKT__

// Convenients structures

typedef struct
{
	uint16_t	src_addr;
	uint16_t	src_id;
	uint16_t	dst_addr;
	uint16_t	dst_id;

	uint32_t	size;
	uint8_t		*data;
} VirtualPacket;

typedef struct
{
	uint16_t	id;
	const char *name;
} ServiceName;

// Constants

#define SID_PAK_ACK1	0x00FE
#define SID_PAK_ACK2	0x00FF
#define SID_NULL		0x4001
#define SID_ECHO		0x4002
#define SID_ADDR_REQ	0x4003
#define SID_ADDR_ASSIGN	0x4003
#define SID_DEV_INFO	0x4020
#define SID_SCREENSHOT	0x4021
#define SID_SCREEN_RLE	0x4024
#define SID_LOGIN		0x4050
#define SID_FILE_MGMT	0x4060
#define SID_OS_INSTALL	0x4080
#define SID_DISCONNECT	0x40DE

// Functions

VirtualPacket*  nsp_vtl_pkt_new(uint32_t size, uint16_t src_addr, uint16_t src_id, uint16_t dst_addr, uint16_t dst_id);
void			nsp_vtl_pkt_del(VirtualPacket* pkt);
void			nsp_vtl_pkt_purge(void);

int nsp_send_data(CalcHandle* h, VirtualPacket* pkt);
int nsp_recv_data(CalcHandle* h, VirtualPacket* pkt);

const char* nsp_sid2name(uint16_t id);

#endif
