/* Hey EMACS -*- linux-c -*- */
/* $Id: linux_detect.h 994 2005-04-30 09:09:49Z roms $ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __LINUX_IOPORTS__
#define __LINUX_IOPORTS__

#include <stdio.h>

int par_io_open(const char *device, int *dev_fd);
int par_io_close(int dev_fd);

int par_io_rd(int dev_fd);
int par_io_wr(int dev_fd, uint8_t data);

// ---

int ser_io_open(const char *device, int *dev_fd);
int ser_io_close(int dev_fd);

int ser_io_rd(int dev_fd);
int ser_io_wr(int dev_fd, uint8_t data);

#endif
