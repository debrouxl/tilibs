/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifndef TIGLINK_H
#define TIGLINK_H

#include "typedefs.h"

int tig_init();
int tig_open();
int tig_put(byte data);
int tig_get(byte *data);
int tig_probe();
int tig_close();
int tig_exit();
int tig_check(int *status);

int tig_set_red_wire(int b);
int tig_set_white_wire(int b);
int tig_get_red_wire();
int tig_get_white_wire();

int tig_supported();

#endif



