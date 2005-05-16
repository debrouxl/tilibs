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

#ifndef __TICALCS_CMD89__
#define __TICALCS_CMD89__


int ti85_send_VAR(uint16_t varsize, uint8_t vartype, char *varname);
int ti85_send_CTS(void);
int ti85_send_XDP(int length, uint8_t * data);
int ti85_send_SKIP(uint8_t rej_code);
int ti85_send_ACK(void);
int ti85_send_ERR(void);
int ti85_send_SCR(void);
int ti85_send_KEY(uint16_t scancode);
int ti85_send_EOT(void);
int ti85_send_REQ(uint16_t varsize, uint8_t vartype, char *varname);
int ti85_send_RTS(uint16_t varsize, uint8_t vartype, char *varname);

int ti85_recv_VAR(uint16_t * varsize, uint8_t * vartype, char *varname);
int ti85_recv_CTS(void);
int ti85_recv_SKIP(uint8_t * rej_code);
int ti85_recv_XDP(uint16_t * length, uint8_t * data);
int ti85_recv_ACK(uint16_t * status);
int ti85_recv_RTS(uint16_t * varsize, uint8_t * vartype, char *varname);


#endif
