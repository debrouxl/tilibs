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

#ifndef SERLINK_H
#define SERLINK_H

#include "typedefs.h"

/* I/O mode (Linux & Win32) */
int ser_init_port();
int ser_open_port();
int ser_put(byte data);
int ser_get(byte *data);
int ser_probe_port();
int ser_close_port();
int ser_term_port();
int ser_check_port(int *status);

int ser_set_red_wire(int b);
int ser_set_white_wire(int b);
int ser_get_red_wire();
int ser_get_white_wire();

int ser_supported();

/* DCB mode (Win32 only) */
int ser_init_port2();
int ser_open_port2();
int ser_put2(byte data);
int ser_get2(byte *data);
int ser_probe_port2();
int ser_close_port2();
int ser_term_port2();
int ser_check_port2(int *status);

int ser_set_red_wire2(int b);
int ser_set_white_wire2(int b);
int ser_get_red_wire2();
int ser_get_white_wire2();

int ser_supported2();

#endif

