/* Hey EMACS -*- linux-c -*- */
/*  ti_link - link program for TI calculators
 *  Copyright (C) 1999-2003  Romain Lievin
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

#ifndef PARLINK_H
#define PARLINK_H

int par_init();
int par_open();
int par_put(uint8_t data);
int par_get(uint8_t * data);
int par_probe();
int par_close();
int par_exit();
int par_check(int *status);

int par_set_red_wire(int b);
int par_set_white_wire(int b);
int par_get_red_wire();
int par_get_white_wire();

int par_supported();

#endif
