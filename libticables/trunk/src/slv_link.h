/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

#ifndef UGLLINK_H
#define UGLLINK_H

//#include <stdint.h>

// USB through kernel module or device driver
int slv_init();
int slv_open();
int slv_put(uint8_t data);
int slv_get(uint8_t * data);
int slv_probe();
int slv_close();
int slv_exit();
int slv_check(int *status);

int slv_set_red_wire(int b);
int slv_set_white_wire(int b);
int slv_get_red_wire();
int slv_get_white_wire();

int slv_supported();

// USB through LIBUSB
int slv_init2();
int slv_open2();
int slv_put2(uint8_t data);
int slv_get2(uint8_t * data);
int slv_probe2();
int slv_close2();
int slv_exit2();
int slv_check2(int *status);

int slv_set_red_wire2(int b);
int slv_set_white_wire2(int b);
int slv_get_red_wire2();
int slv_get_white_wire2();

int slv_supported2();

#endif
