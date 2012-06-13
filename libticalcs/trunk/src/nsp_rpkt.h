/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.h 1179 2005-06-06 14:42:32Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 2007 Romain Liévin
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

#ifndef __NSP_RPKT__
#define __NSP_RPKT__

// Convenient structures

#define NSP_HEADER_SIZE 16
#define NSP_DATA_SIZE   254

typedef struct
{
    uint16_t  unused;
    uint16_t  src_addr;
    uint16_t  src_port;
    uint16_t  dst_addr;
    uint16_t  dst_port;
    uint16_t  data_sum;
    uint8_t   data_size;
    uint8_t   ack;
    uint8_t   seq;
    uint8_t   hdr_sum;

    uint8_t   data[NSP_DATA_SIZE];
} NSPRawPacket;

// Functions

int nsp_send(CalcHandle* cable, NSPRawPacket* pkt);
int nsp_recv(CalcHandle* cable, NSPRawPacket* pkt);

#endif
