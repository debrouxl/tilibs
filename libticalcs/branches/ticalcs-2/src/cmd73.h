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

#ifndef __CMDS_73__
#define __CMDS_73__


int ti73_send_VAR(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr);
int ti73_send_VAR2(uint32_t length, uint8_t type, uint8_t flag,
		   uint16_t offset, uint16_t page);
int ti73_send_CTS(void);
int ti73_send_XDP(int length, uint8_t * data);
int ti73_send_FLSH(void);
int ti73_send_SKIP(uint8_t rej_code);
int ti73_send_ACK(void);
int ti73_send_ERR(void);
int ti73_send_RDY(void);
int ti73_send_SCR(void);
int ti73_send_KEY(uint16_t scancode);
int ti73_send_EOT(void);
int ti73_send_REQ(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr);
int ti73_send_REQ2(uint16_t appsize, uint8_t apptype, char *appname,
		   uint8_t appattr);
int ti73_send_RTS(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr);

int ti73_recv_VAR(uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr);
int ti73_recv_VAR2(uint16_t * length, uint8_t * type, char *name,
		   uint16_t * offset, uint16_t * page);
int ti73_recv_CTS(uint16_t length);
int ti73_recv_SKIP(uint8_t * rej_code);
int ti73_recv_XDP(uint16_t * length, uint8_t * data);
int ti73_recv_ACK(uint16_t * status);
int ti73_recv_RTS(uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr);

#endif
