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

#ifndef __TICALCS_CMD82__
#define __TICALCS_CMD82__

int ti82_send_VAR(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
int ti82_send_CTS(CalcHandle*);
int ti82_send_XDP(CalcHandle*, int length, uint8_t * data);
int ti82_send_SKP(CalcHandle*, uint8_t rej_code);
int ti82_send_ACK(CalcHandle*);
int ti82_send_ERR(CalcHandle*);
int ti82_send_SCR(CalcHandle*);
int ti83_send_KEY(CalcHandle*, uint16_t scancode);
int ti82_send_KEY(CalcHandle*, uint16_t scancode);
int ti82_send_EOT(CalcHandle*);
int ti82_send_REQ(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
int ti82_send_RTS(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);

int ti82_recv_VAR(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);
int ti82_recv_CTS(CalcHandle*);
int ti82_recv_SKP(CalcHandle*, uint8_t * rej_code);
int ti82_recv_XDP(CalcHandle*, uint16_t * length, uint8_t * data);
int ti82_recv_ACK(CalcHandle*, uint16_t * status);
int ti82_recv_ERR(CalcHandle*, uint16_t * status);
int ti82_recv_RTS(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);

#endif
