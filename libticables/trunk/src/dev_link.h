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

/* Linux tidev device driver support
 * Copyright (c) 2000 Leonard Stiles <ljs@uk2.net> */

#ifndef DEVLINK_H
#define DEVLINK_H

#include "typedefs.h"

int dev_init_port(unsigned int unused_uint, char *unused_pchar);
int dev_open_port(void);
int dev_put(byte data);
int dev_get(byte *data);
int dev_probe_port(void);
int dev_close_port(void);
int dev_term_port(void);
int dev_check_port(int *status);

int dev_set_red_wire(int b);
int dev_set_white_wire(int b);
int dev_get_red_wire();
int dev_get_white_wire();

int dev_supported();

#endif /* DEVLINK_H */

