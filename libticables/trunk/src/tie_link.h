/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef TIELINK_H
#define TIELINK_H

#include "typedefs.h"

int tie_init_port();
int tie_open_port();
int tie_put(byte data);
int tie_get(byte *data);
int tie_probe_port();
int tie_close_port();
int tie_term_port();
int tie_check_port(int *status);

int tie_set_red_wire(int b);
int tie_set_white_wire(int b);
int tie_get_red_wire();
int tie_get_white_wire();

int tie_supported();

#endif
