/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.c 2077 2006-03-31 21:16:19Z roms $ */

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

/*
  This unit handles TI84+ commands with DirectLink.
*/

#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

#include "dusb_vpkt.h"
#include "cmd84p.h"

int ti84p_set_mode(CalcHandle *h)
{
	ModeSet mode = { 0 };
	VirtualPacket* vtl = vtl_pkt_new(sizeof(mode));

	mode.arg1 = 3;
	mode.arg2 = 1;
	mode.arg5 = 0x7d0;

	TRYF(dusb_buffer_size_request(h));
	TRYF(dusb_buffer_size_alloc(h));

	TRYF(dusb_send_data(h, vtl));

	return 0;
}