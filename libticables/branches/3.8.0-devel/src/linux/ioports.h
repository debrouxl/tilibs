/* Hey EMACS -*- linux-c -*- */
/* $Id: ioports.h 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
 *  Copyright (c) 2002, Kevin Kofler for the __MINGW32__ & __GNUC__ extensions.
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

/*
  This file redirects low-level I/O functions according to the architecture
  and/or the platform
 */

#ifndef __IOPORTS_H__
#define __IOPORTS_H__

/* I/O abstraction */
int io_open(unsigned long from, unsigned long num);
extern int (*io_rd) (unsigned int addr);
extern void (*io_wr) (unsigned int addr, int data);
int io_close(unsigned long from, unsigned long num);


#endif
