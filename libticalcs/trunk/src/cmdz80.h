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

#ifndef __TICALCS_CMDZ80__
#define __TICALCS_CMDZ80__

#ifdef __cplusplus
extern "C" {
#endif

/* TI-80 (not Z80-based, in fact), send functions */
TIEXPORT3 int TICALL ti80_send_SCR(CalcHandle*);

/* TI-80 (not Z80-based, in fact), receive functions */
TIEXPORT3 int TICALL ti80_recv_XDP(CalcHandle*, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti80_recv_ACK(CalcHandle*, uint16_t * status);


/* TI-73 family, send functions */
TIEXPORT3 int TICALL ti73_send_VAR(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
TIEXPORT3 int TICALL ti73_send_VAR2(CalcHandle*, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page);
TIEXPORT3 int TICALL ti73_send_CTS(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_XDP(CalcHandle*, uint16_t length, uint8_t * data);
TIEXPORT3 int TICALL ti73_send_SKP(CalcHandle*, uint8_t rej_code);
TIEXPORT3 int TICALL ti73_send_ACK(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_ERR(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_RDY(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_SCR(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_KEY(CalcHandle*, uint16_t scancode);
TIEXPORT3 int TICALL ti73_send_EOT(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_REQ(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
TIEXPORT3 int TICALL ti73_send_REQ2(CalcHandle*, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr);
TIEXPORT3 int TICALL ti73_send_RTS(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
TIEXPORT3 int TICALL ti73_send_VER(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_DEL(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
TIEXPORT3 int TICALL ti73_send_DUMP(CalcHandle*, uint16_t page);
TIEXPORT3 int TICALL ti73_send_EKE(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_DKE(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_ELD(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_DLD(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_GID(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_RID(CalcHandle*);
TIEXPORT3 int TICALL ti73_send_SID(CalcHandle*, uint8_t * data);

/* TI-73 family, receive functions */
TIEXPORT3 int TICALL ti73_recv_VAR(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);
TIEXPORT3 int TICALL ti73_recv_VAR2(CalcHandle*, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page);
TIEXPORT3 int TICALL ti73_recv_CTS(CalcHandle*, uint16_t length);
TIEXPORT3 int TICALL ti73_recv_SKP(CalcHandle*, uint8_t * rej_code);
TIEXPORT3 int TICALL ti73_recv_XDP(CalcHandle*, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti73_recv_SID(CalcHandle*, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti73_recv_ACK(CalcHandle*, uint16_t * status);
TIEXPORT3 int TICALL ti73_recv_RTS(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr);


/* TI-82, send functions */
TIEXPORT3 int TICALL ti82_send_VAR(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti82_send_CTS(CalcHandle*);
TIEXPORT3 int TICALL ti82_send_XDP(CalcHandle*, uint16_t length, uint8_t * data);
TIEXPORT3 int TICALL ti82_send_SKP(CalcHandle*, uint8_t rej_code);
TIEXPORT3 int TICALL ti82_send_ACK(CalcHandle*);
TIEXPORT3 int TICALL ti82_send_ERR(CalcHandle*);
TIEXPORT3 int TICALL ti82_send_SCR(CalcHandle*);
TIEXPORT3 int TICALL ti82_send_KEY(CalcHandle*, uint16_t scancode);
TIEXPORT3 int TICALL ti82_send_EOT(CalcHandle*);
TIEXPORT3 int TICALL ti82_send_REQ(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti82_send_RTS(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);

/* TI-82, receive functions */
TIEXPORT3 int TICALL ti82_recv_VAR(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);
TIEXPORT3 int TICALL ti82_recv_CTS(CalcHandle*);
TIEXPORT3 int TICALL ti82_recv_SKP(CalcHandle*, uint8_t * rej_code);
TIEXPORT3 int TICALL ti82_recv_XDP(CalcHandle*, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti82_recv_ACK(CalcHandle*, uint16_t * status);
TIEXPORT3 int TICALL ti82_recv_ERR(CalcHandle*, uint16_t * status);
TIEXPORT3 int TICALL ti82_recv_RTS(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);


/* TI-85 & TI-86, send functions */
TIEXPORT3 int TICALL ti85_send_VAR(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti85_send_CTS(CalcHandle*);
TIEXPORT3 int TICALL ti85_send_XDP(CalcHandle*, uint16_t length, uint8_t * data);
TIEXPORT3 int TICALL ti85_send_SKP(CalcHandle*, uint8_t rej_code);
TIEXPORT3 int TICALL ti85_send_ACK(CalcHandle*);
TIEXPORT3 int TICALL ti85_send_ERR(CalcHandle*);
TIEXPORT3 int TICALL ti85_send_SCR(CalcHandle*);
TIEXPORT3 int TICALL ti85_send_KEY(CalcHandle*, uint16_t scancode);
TIEXPORT3 int TICALL ti85_send_EOT(CalcHandle*);
TIEXPORT3 int TICALL ti85_send_REQ(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti85_send_RTS(CalcHandle*, uint16_t varsize, uint8_t vartype, const char *varname);

/* TI-85 & TI-86, receive functions */
TIEXPORT3 int TICALL ti85_recv_VAR(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);
TIEXPORT3 int TICALL ti85_recv_CTS(CalcHandle*);
TIEXPORT3 int TICALL ti85_recv_SKP(CalcHandle*, uint8_t * rej_code);
TIEXPORT3 int TICALL ti85_recv_XDP(CalcHandle*, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti85_recv_ACK(CalcHandle*, uint16_t * status);
TIEXPORT3 int TICALL ti85_recv_RTS(CalcHandle*, uint16_t * varsize, uint8_t * vartype, char *varname);

#ifdef __cplusplus
}
#endif

#endif
