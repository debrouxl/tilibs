/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.h 2074 2006-03-31 08:36:06Z roms $ */

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

#ifndef __NSP_CMDS__
#define __NSP_CMDS__

// Device Information IDs
#define DI_VERSION	1
#define DI_MODEL	2
#define DI_FEXT		3

// Structures
// ...

// Command wrappers

int cmd_r_dev_addr_request(CalcHandle *h);
int cmd_s_dev_addr_assign(CalcHandle *h, uint16_t dev_addr);

int cmd_s_ack(CalcHandle *h, uint16_t  ack);
int cmd_r_ack(CalcHandle *h, uint16_t *ack);

int cmd_s_dev_infos(CalcHandle *h, uint8_t cmd);
int cmd_r_dev_infos(CalcHandle *h,  uint8_t *size, uint8_t **data);

#endif
