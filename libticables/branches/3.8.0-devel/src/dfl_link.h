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

#ifndef DEFAULT_LINK_H
#define DEFAULT_LINK_H

//#include <stdint.h>

int dfl_init();
int dfl_open();
int dfl_put(uint8_t data);
int dfl_get(uint8_t * data);
int dfl_probe();
int dfl_close();
int dfl_exit();
int dfl_check(int *status);

int dfl_set_red_wire(int b);
int dfl_set_white_wire(int b);
int dfl_get_red_wire();
int dfl_get_white_wire();

int dfl_supported();

int set_default_cable(TicableLinkCable * lc);

#endif
