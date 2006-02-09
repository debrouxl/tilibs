/* Hey EMACS -*- win32-c -*- */
/* $Id: links.h 397 2004-03-29 17:21:12Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifndef __LINKS_H__
#define __LINKS_H__

#include "../cabl_def.h"

int avr_register_cable(CableLinkCable * lc);

int dev_register_cable(CableLinkCable * lc);

int nul_register_cable(CableLinkCable * lc);

int par_register_cable(CableLinkCable * lc);

int ser_register_cable_1(CableLinkCable * lc);
int ser_register_cable_2(CableLinkCable * lc);

int slv_register_cable_1(CableLinkCable * lc);
int slv_register_cable_2(CableLinkCable * lc);

int tie_register_cable(CableLinkCable * lc);

int tig_register_cable(CableLinkCable * lc);

int vti_register_cable(CableLinkCable * lc);

int vtl_register_cable(CableLinkCable * lc);

#endif
