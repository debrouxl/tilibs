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

#ifndef __TICALCS_CMD84P__
#define __TICALCS_CMD84P__

// Data Types (or opcodes)

#define TI84P_OPC_NONE		0x0000
#define TI84P_OPC_SCR		0x0007

int ti84p_send_handshake(CalcHandle *h);
int ti84p_recv_response (CalcHandle *h);
int ti84p_send_data(CalcHandle *h, uint32_t  size, uint16_t  code, uint8_t *data);
int ti84p_recv_data(CalcHandle *h, uint32_t *size, uint16_t *code, uint8_t *data);
int ti84p_send_acknowledge(CalcHandle* h);
int ti84p_recv_acknowledge(CalcHandle *h);

#endif
