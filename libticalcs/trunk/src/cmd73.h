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

#ifndef __TICALCS_CMD73__
#define __TICALCS_CMD73__

int ti73_send_VAR_h(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
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
int ti73_send_REQ_h(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
int ti73_send_REQ2_h(CalcHandle*, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr);
int ti73_send_RTS_h(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
int ti73_send_VER_h(CalcHandle*);
int ti73_send_DEL_h(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);

int ti73_recv_VAR_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);
int ti73_recv_VAR2_h(CalcHandle*, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page);
int ti73_recv_CTS_h(CalcHandle*, uint16_t length);
int ti73_recv_SKP_h(CalcHandle*, uint8_t * rej_code);
int ti73_recv_XDP_h(CalcHandle*, uint16_t * length, uint8_t * data);
int ti73_recv_ACK_h(CalcHandle*, uint16_t * status);
int ti73_recv_RTS_h(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);

#endif
