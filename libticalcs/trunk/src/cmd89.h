/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#ifndef __CMDS_89__
#define __CMDS_89__


int ti89_send_VAR(uint32_t varsize, uint8_t vartype, char *varname);
int ti89_send_CTS(void);
int ti89_send_XDP(int length, uint8_t * data);
int ti89_send_SKIP(uint8_t rej_code);
int ti89_send_ACK(void);
int ti89_send_ERR(void);
int ti89_send_RDY(void);
int ti89_send_SCR(void);
int ti89_send_CONT(void);
int ti89_send_KEY(uint16_t scancode);
int ti89_send_EOT(void);
int ti89_send_REQ(uint32_t varsize, uint8_t vartype, char *varname);
int ti89_send_RTS(uint32_t varsize, uint8_t vartype, char *varname);

int ti89_recv_VAR(uint32_t * varsize, uint8_t * vartype, char *varname);
int ti89_recv_CTS(void);
int ti89_recv_SKIP(uint8_t * rej_code);
int ti89_recv_XDP(uint32_t * length, uint8_t * data);
int ti89_recv_ACK(uint16_t * status);
int ti89_recv_CONT(void);
int ti89_recv_EOT(void);
int ti89_recv_RTS(uint32_t * varsize, uint8_t * vartype, char *varname);


#endif
