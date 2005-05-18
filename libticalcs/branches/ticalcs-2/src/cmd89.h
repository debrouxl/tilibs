/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (CalcHandle*, C) 1999-2005  Romain Liévin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (CalcHandle*, at your option) any later version.
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

#ifndef __TICALCS_CMD89__
#define __TICALCS_CMD89__

int ti89_send_VAR_h(CalcHandle*, uint32_t varsize, uint8_t vartype, char *varname);
int ti89_send_CTS_h(CalcHandle*);
int ti89_send_XDP_h(CalcHandle*, int length, uint8_t * data);
int ti89_send_SKP_h(CalcHandle*, uint8_t rej_code);
int ti89_send_ACK_h(CalcHandle*);
int ti89_send_ERR_h(CalcHandle*);
int ti89_send_RDY_h(CalcHandle*);
int ti89_send_SCR_h(CalcHandle*);
int ti89_send_CNT_h(CalcHandle*);
int ti89_send_KEY_h(CalcHandle*, uint16_t scancode);
int ti89_send_EOT_h(CalcHandle*);
int ti89_send_REQ_h(CalcHandle*, uint32_t varsize, uint8_t vartype, char *varname);
int ti89_send_RTS_h(CalcHandle*, uint32_t varsize, uint8_t vartype, char *varname);
int ti89_send_RTS2_h(CalcHandle*, uint32_t varsize, uint8_t vartype, char *varname);	// titanium

int ti89_recv_VAR_h(CalcHandle*, uint32_t * varsize, uint8_t * vartype, char *varname);
int ti89_recv_CTS_h(CalcHandle*);
int ti89_recv_SKP_h(CalcHandle*, uint8_t * rej_code);
int ti89_recv_XDP_h(CalcHandle*, uint32_t * length, uint8_t * data);
int ti89_recv_ACK_h(CalcHandle*, uint16_t * status);
int ti89_recv_CNT_h(CalcHandle*);
int ti89_recv_EOT_h(CalcHandle*);
int ti89_recv_RTS_h(CalcHandle*, uint32_t * varsize, uint8_t * vartype, char *varname);

// ---

#define ti89_send_VAR(a,b,c)		ti89_send_VAR_h(handle, a, b, c)
#define ti89_send_CTS				ti89_send_CTS_h(handle)
#define ti89_send_XDP(a,b)			ti89_send_XDP_h(handle, a, b)
#define ti89_send_SKP(a)			ti89_send_SKP_h(handle, a)
#define ti89_send_ACK				ti89_send_ACK_h(handle)
#define ti89_send_ERR				ti89_send_ERR_h(handle)
#define ti89_send_RDY()				ti89_send_RDY_h(handle)
#define ti89_send_SCR				ti89_send_SCR_h(handle)
#define ti89_send_CNT				ti89_send_CNT(handle)
#define ti89_send_KEY(a)			ti89_send_KEY_h(handle, a)
#define ti89_send_EOT				ti89_send_EOT_h(handle)
#define ti89_send_REQ(a,b,c)		ti89_send_REQ_h(handle, a, b, c)
#define ti89_send_RTS(a,b,c)		ti89_send_RTS_h(handle, a, b, c)
#define ti89_send_RTS2(a,b,c)		ti89_send_RTS2_h(handle, a, b, c)

#define ti89_recv_VAR(a,b,c)		ti89_recv_VAR_h((handle, a, b, c)		
#define ti89_recv_CTS				ti89_recv_CTS_h(handle)
#define ti89_recv_SKP(a)			ti89_recv_SKP_h(handle, a)
#define ti89_recv_XDP(a,b)			ti89_recv_XDP_h(handle, a , b)
#define ti89_recv_ACK(a)			ti89_recv_ACK_h(handle, a)
#define ti89_recv_CNT				ti89_recv_CNT_h(handle)
#define ti89_recv_EOT				ti89_recv_EOT_h(handle)
#define ti89_recv_RTS(a,b,c)		ti89_recv_RTS_h(handle, a, b, c)

#endif
