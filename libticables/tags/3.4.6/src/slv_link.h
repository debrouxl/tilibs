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

#ifndef UGLLINK_H
#define UGLLINK_H

//#include <stdint.h>

// USB through kernel module or device driver
int ugl_init();
int ugl_open();
int ugl_put(uint8_t data);
int ugl_get(uint8_t *data);
int ugl_probe();
int ugl_close();
int ugl_exit();
int ugl_check(int *status);

int ugl_set_red_wire(int b);
int ugl_set_white_wire(int b);
int ugl_get_red_wire();
int ugl_get_white_wire();

int ugl_supported();

// USB through LIBUSB
int ugl_init2();
int ugl_open2();
int ugl_put2(uint8_t data);
int ugl_get2(uint8_t *data);
int ugl_probe2();
int ugl_close2();
int ugl_exit2();
int ugl_check2(int *status);

int ugl_set_red_wire2(int b);
int ugl_set_white_wire2(int b);
int ugl_get_red_wire2();
int ugl_get_white_wire2();

int ugl_supported2();

#endif



