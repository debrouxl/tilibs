/*  ti_link - link program for TI calculators
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

#ifndef __COMPAT__
#define __COMPAT__

/* Parallel port addresses */
#define LPT3 0x3BC
#define LPT1 0x378
#define LPT2 0x278

/* Serial port addresses */
#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

/* Serial port devices */
#if defined(__LINUX__)
# define TTY0 "/dev/ttyS0"
# define TTY1 "/dev/ttyS1"
# define TTY2 "/dev/ttyS2"
# define TTY3 "/dev/ttyS3"
#elif defined(__WIN32__)
# define TTY0 "COM1"
# define TTY1 "COM2"
# define TTY2 "COM3"
# define TTY3 "COM4"
#elif defined(__SOL__)
# define TTY0 "/dev/cua/a"
# define TTY1 "/dev/cua/b"
# define TTY2 "/dev/cua/a"
# define TTY3 "/dev/cua/b"
#else
# define TTY0 "/dev/ttyS0"
# define TTY1 "/dev/ttyS1"
# define TTY2 "/dev/ttyS2"
# define TTY3 "/dev/ttyS3"
#endif

#endif



