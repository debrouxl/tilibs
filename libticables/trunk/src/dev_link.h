/* Hey EMACS -*- linux-c -*- */
/*  libticables - link cable library, a part of the TiLP project
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

/* Linux tidev device driver support
 * Copyright (c) 2000 Leonard Stiles <ljs@uk2.net> */

#ifndef DEVLINK_H
#define DEVLINK_H

int dev_init(void);
int dev_open(void);
int dev_put(uint8_t data);
int dev_get(uint8_t * data);
int dev_probe(void);
int dev_close(void);
int dev_exit(void);
int dev_check(int *status);

int dev_set_red_wire(int b);
int dev_set_white_wire(int b);
int dev_get_red_wire();
int dev_get_white_wire();

int dev_supported();

#endif
