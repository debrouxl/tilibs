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

#ifndef AVRLINK_H
#define AVRLINK_H

//#include <stdint.h>

int avr_init();
int avr_open();
int avr_put(uint8_t data);
int avr_get(uint8_t *data);
int avr_probe();
int avr_close();
int avr_exit();
int avr_check(int *status);

int avr_set_red_wire(int b);
int avr_set_white_wire(int b);
int avr_get_red_wire();
int avr_get_white_wire();

int avr_supported();

#endif
