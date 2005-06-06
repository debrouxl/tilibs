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

#ifndef __TICALCS_CMD73__
#define __TICALCS_CMD73__

int ti73_send_VAR_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname, uint8_t varattr);
int ti73_send_VAR2_h(CalcHandle*, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page);
int ti73_send_CTS_h(CalcHandle*);
int ti73_send_XDP_h(CalcHandle*, int length, uint8_t * data);
int ti73_send_FLSH_h(CalcHandle*);
int ti73_send_SKP_h(CalcHandle*, uint8_t rej_code);
int ti73_send_ACK_h(CalcHandle*);
int ti73_send_ERR_h(CalcHandle*);
int ti73_send_RDY_h(CalcHandle*);
int ti73_send_SCR_h(CalcHandle*);
int ti73_send_KEY_h(CalcHandle*, uint16_t scancode);
int ti73_send_EOT_h(CalcHandle*);
int ti73_send_REQ_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname, uint8_t varattr);
int ti73_send_REQ2_h(CalcHandle*, uint16_t appsize, uint8_t apptype, char *appname, uint8_t appattr);
int ti73_send_RTS_h(CalcHandle*, uint16_t varsize, uint8_t vartype, char *varname, uint8_t varattr);

int ti73_recv_VAR_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);
int ti73_recv_VAR2_h(CalcHandle*, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page);
int ti73_recv_CTS_h(CalcHandle*, uint16_t length);
int ti73_recv_SKP_h(CalcHandle*, uint8_t * rej_code);
int ti73_recv_XDP_h(CalcHandle*, uint16_t * length, uint8_t * data);
int ti73_recv_ACK_h(CalcHandle*, uint16_t * status);
int ti73_recv_RTS_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);

// ---

#define ti73_send_VAR(a,b,c,d)		ti73_send_VAR_h(handle,a,b,c,d)
#define ti73_send_VAR2(a,b,c,d,e)	ti73_send_VAR2_h(handle,a,b,c,d,e)
#define ti73_send_CTS()				ti73_send_CTS_h(handle)
#define ti73_send_XDP(a,b)			ti73_send_XDP_h(handle,a,b)
#define ti73_send_FLSH()			ti73_send_FLSH_h(handle)
#define ti73_send_SKP(a)			ti73_send_SKP_h(handle,a)
#define ti73_send_ACK()				ti73_send_ACK_h(handle)
#define ti73_send_ERR()				ti73_send_ERR_h(handle)
#define ti73_send_RDY()				ti73_send_RDY_h(handle)
#define ti73_send_SCR()				ti73_send_SCR_h(handle)
#define ti73_send_KEY(a)			ti73_send_KEY_h(handle,a)
#define ti73_send_EOT()				ti73_send_EOT_h(handle)
#define ti73_send_REQ(a,b,c,d)		ti73_send_REQ_h(handle,a,b,c,d)
#define ti73_send_REQ2(a,b,c,d)		ti73_send_REQ2_h(handle,a,b,c,d)
#define ti73_send_RTS(a,b,c,d)		ti73_send_RTS_h(handle,a,b,c,d)

#define ti73_recv_VAR(a,b,c,d)		ti73_recv_VAR_h(handle,a,b,c,d)
#define ti73_recv_VAR2(a,b,c,d,e)	ti73_recv_VAR2_h(handle,a,b,c,d,e)
#define ti73_recv_CTS(a)			ti73_recv_CTS_h(handle,a)
#define ti73_recv_SKP(a)			ti73_recv_SKP_h(handle,a)
#define ti73_recv_XDP(a,b)			ti73_recv_XDP_h(handle,a,b)
#define ti73_recv_ACK(a)			ti73_recv_ACK_h(handle,a)
#define ti73_recv_RTS(a,b,c,d,e)	ti73_recv_RTS_h(handle,a,b,c,d,e)

#endif
