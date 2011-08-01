/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

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

#ifndef __WIN32_IOPORTS__
#define __WIN32_IOPORTS__

#include <stdio.h>
#include <windows.h>

int io_open(unsigned long from);
int io_close(unsigned long from);

int  (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

int win32_comport_open(char *device, PHANDLE hCom);
int win32_comport_close(PHANDLE hCom);

#endif
