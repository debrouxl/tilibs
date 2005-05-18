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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TICALCS_CMD85__
#define __TICALCS_CMD85__

int ti85_send_VAR_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname);
int ti85_send_CTS_h(CalcHandle*);
int ti85_send_XDP_h(CalcHandle*, int length, uint8_t * data);
int ti85_send_SKP_h(CalcHandle*, uint8_t rej_code);
int ti85_send_ACK_h(CalcHandle*);
int ti85_send_ERR_h(CalcHandle*);
int ti85_send_SCR_h(CalcHandle*);
int ti85_send_KEY_h(CalcHandle*, uint16_t scancode);
int ti85_send_EOT_h(CalcHandle*);
int ti85_send_REQ_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname);
int ti85_send_RTS_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname);

int ti85_recv_VAR_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);
int ti85_recv_CTS_h(CalcHandle*);
int ti85_recv_SKP_h(CalcHandle*, uint8_t * rej_code);
int ti85_recv_XDP_h(CalcHandle*, uint16_t * length, uint8_t * data);
int ti85_recv_ACK_h(CalcHandle*, uint16_t * status);
int ti85_recv_RTS_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);

// ---

#define ti85_send_VAR(a,b,c)		ti85_send_VAR_h(handle, a, b, c)
#define ti85_send_CTS()				ti85_send_CTS_h(handle)
#define ti85_send_XDP(a,b,c)		ti85_send_XDP_h(handle, a, b, c)
#define ti85_send_SKP(a,b)			ti85_send_SKP_h(handle, a, b)
#define ti85_send_ACK()				ti85_send_ACK_h(handle)	
#define ti85_send_ERR()				ti85_send_ERR_h(handle)			
#define ti85_send_SCR()				ti85_send_SCR_h(handle)			
#define ti85_send_KEY(a)			ti85_send_KEY_h(handle, a)
#define ti85_send_EOT()				ti85_send_EOT_h(handle)	
#define ti85_send_REQ(a,b,c)		ti85_send_REQ_h(handle, a, b, c)
#define ti85_send_RTS(a,b,c)		ti85_send_RTS_h(handle, a, b, c)

#define ti85_recv_VAR(a,b,c)		ti85_recv_VAR_h(handle, a, b, c)
#define ti85_recv_CTS()				ti85_recv_CTS_h(handle)	
#define ti85_recv_SKP(a,b)			ti85_recv_SKP_h(handle, a, b)	
#define ti85_recv_XDP(a,b)			ti85_recv_XDP_h(handle, a, b)	
#define ti85_recv_ACK(a)			ti85_recv_ACK_h(handle, a)
#define ti85_recv_RTS(a,b,c)		ti85_recv_RTS_h(handle, a, b, c)

#endif
