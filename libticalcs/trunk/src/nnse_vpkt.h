/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2019  Lionel Debroux
 *  Copyright (C) 2019  Fabian Vogt
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

#ifndef __NNSE_VPKT__
#define __NNSE_VPKT__

#ifdef __cplusplus
extern "C" {
#endif

// Constants

#define NNSE_ADDR_CALC  0x01
#define NNSE_ADDR_ME    0xFE
#define NNSE_ADDR_ALL   0xFF

// Functions

TIEXPORT3 NNSEVirtualPacket* TICALL nnse_vtl_pkt_new(CalcHandle *handle);
TIEXPORT3 NNSEVirtualPacket* TICALL nnse_vtl_pkt_new_ex(CalcHandle *handle, uint32_t size, uint8_t service, uint8_t src_addr, uint8_t dst_addr, uint8_t * data);
TIEXPORT3 void TICALL nnse_vtl_pkt_fill(NNSEVirtualPacket* vtl, uint32_t size, uint8_t service, uint8_t src_addr, uint8_t dst_addr, uint8_t * data);
TIEXPORT3 void TICALL nnse_vtl_pkt_del(CalcHandle *handle, NNSEVirtualPacket* vtl);
TIEXPORT3 void * TICALL nnse_vtl_pkt_alloc_data(size_t size);
TIEXPORT3 NNSEVirtualPacket * TICALL nnse_vtl_pkt_realloc_data(NNSEVirtualPacket* vtl, size_t size);
TIEXPORT3 void TICALL nnse_vtl_pkt_free_data(void * data);

TIEXPORT3 int TICALL nnse_addr_request(CalcHandle *handle);
TIEXPORT3 int TICALL nnse_addr_assign(CalcHandle *handle, uint8_t dev_addr);

TIEXPORT3 int TICALL nnse_send_ack(CalcHandle *handle);
TIEXPORT3 int TICALL nnse_recv_ack(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_send_data(CalcHandle *handle, NNSEVirtualPacket* vtl);
TIEXPORT3 int TICALL nnse_recv_data(CalcHandle *handle, NNSEVirtualPacket* vtl);

#ifdef __cplusplus
}
#endif

#endif
