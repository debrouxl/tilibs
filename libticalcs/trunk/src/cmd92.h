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

#ifndef __TICALCS_CMD92__
#define __TICALCS_CMD92__

int ti92_send_VAR_h(CalcHandle*, uint32_t varsize, uint8_t vartype, const char *varname);
int ti92_send_CTS_h(CalcHandle*);
int ti92_send_XDP_h(CalcHandle*, int length, uint8_t * data);
int ti92_send_SKP_h(CalcHandle*, uint8_t rej_code);
int ti92_send_ACK_h(CalcHandle*);
int ti92_send_ERR_h(CalcHandle*);
int ti92_send_RDY_h(CalcHandle*);
int ti92_send_SCR_h(CalcHandle*);
int ti92_send_CNT_h(CalcHandle*);
int ti92_send_KEY_h(CalcHandle*, uint16_t scancode);
int ti92_send_EOT_h(CalcHandle*);
int ti92_send_REQ_h(CalcHandle*, uint32_t varsize, uint8_t vartype, const char *varname);
int ti92_send_RTS_h(CalcHandle*, uint32_t varsize, uint8_t vartype, const char *varname);

int ti92_recv_VAR_h(CalcHandle*, uint32_t * varsize, uint8_t * vartype, char *varname);
int ti92_recv_CTS_h(CalcHandle*);
int ti92_recv_SKP_h(CalcHandle*, uint8_t * rej_code);
int ti92_recv_XDP_h(CalcHandle*, uint32_t * length, uint8_t * data);
int ti92_recv_ACK_h(CalcHandle*, uint16_t * status);
int ti92_recv_CNT_h(CalcHandle*);
int ti92_recv_EOT_h(CalcHandle*);
int ti92_recv_RTS_h(CalcHandle*, uint32_t * varsize, uint8_t * vartype, char *varname);

#endif

