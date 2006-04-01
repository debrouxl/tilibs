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

#ifndef __TICALCS_CMD84P__
#define __TICALCS_CMD84P__

typedef struct
{
	uint16_t	arg1;
	uint16_t	arg2;
	uint16_t	arg3;
	uint16_t	arg4;
	uint16_t	arg5;
} ModeSet;

int ti84p_set_mode(CalcHandle *h);

int ti84p_os_begin(CalcHandle *h);
int ti84p_os_data(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint8_t* data);

int ti84p_param_req(CalcHandle *h);


#endif
