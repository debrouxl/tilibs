/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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


// /!\ NOTE: for this file, backwards compatibility will not necessarily be maintained as strongly as it is for ticalcs.h !

#ifndef __NSP_VPKT__
#define __NSP_VPKT__

#ifdef __cplusplus
extern "C" {
#endif

// Constants

#define NSP_SRC_ADDR            0x6400
#define NSP_DEV_ADDR            0x6401

#define NSP_PORT_PKT_NACK       0x00D3
#define NSP_PORT_PKT_ACK1       0x00FE
#define NSP_PORT_PKT_ACK2       0x00FF

#define NSP_PORT_NULL           0x4001
#define NSP_PORT_ECHO           0x4002
#define NSP_PORT_ADDR_REQUEST   0x4003
#define NSP_PORT_ADDR_ASSIGN    0x4003
#define NSP_PORT_DEV_INFOS      0x4020
#define NSP_PORT_SCREENSHOT     0x4021
#define NSP_PORT_EVENT          0x4022
#define NSP_PORT_SHUTDOWN       0x4023
#define NSP_PORT_SCREEN_RLE     0x4024
#define NSP_PORT_ACTIVITY       0x4041
#define NSP_PORT_KEYPRESSES     0x4042
#define NSP_PORT_RPC            0x4043
#define NSP_PORT_LOGIN          0x4050
#define NSP_PORT_MESSAGES       0x4051
#define NSP_PORT_HUB_CONNECTION 0x4054
#define NSP_PORT_FILE_MGMT      0x4060
#define NSP_PORT_TIROBOT        0x4070
#define NSP_PORT_OS_INSTALL     0x4080
#define NSP_PORT_REMOTE_MGMT    0x4090
#define NSP_PORT_DISCONNECT     0x40DE
#define NSP_PORT_EXTECHO        0x5000

// Functions

TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new(CalcHandle *handle);
TIEXPORT3 NSPVirtualPacket* TICALL nsp_vtl_pkt_new_ex(CalcHandle *handle, uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint8_t cmd, uint8_t * data);
TIEXPORT3 void TICALL nsp_vtl_pkt_fill(NSPVirtualPacket* vtl, uint32_t size, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint8_t cmd, uint8_t * data);
TIEXPORT3 void TICALL nsp_vtl_pkt_del(CalcHandle *handle, NSPVirtualPacket* vtl);
TIEXPORT3 void * TICALL nsp_vtl_pkt_alloc_data(CalcHandle * handle, size_t size);
TIEXPORT3 NSPVirtualPacket * TICALL nsp_vtl_pkt_realloc_data(CalcHandle * handle, NSPVirtualPacket* vtl, size_t size);
TIEXPORT3 void TICALL nsp_vtl_pkt_free_data(CalcHandle * handle, void * data);

TIEXPORT3 int TICALL nsp_session_open(CalcHandle *handle, uint16_t port);
TIEXPORT3 int TICALL nsp_session_close(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_addr_request(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_addr_assign(CalcHandle *handle, uint16_t dev_addr);

TIEXPORT3 int TICALL nsp_send_ack(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_recv_ack(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_send_nack(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_send_nack_ex(CalcHandle *handle, uint16_t port);

TIEXPORT3 int TICALL nsp_send_data(CalcHandle *handle, NSPVirtualPacket* vtl);
TIEXPORT3 int TICALL nsp_recv_data(CalcHandle *handle, NSPVirtualPacket* vtl);

TIEXPORT3 int TICALL nsp_send_disconnect(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_recv_disconnect(CalcHandle *handle);

#ifdef __cplusplus
}
#endif

#endif
