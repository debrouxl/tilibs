/*  IOPorts - I/O low-level port access routines for Linux, Windows9x,
 *		NT4/2000, DOS.
 *	A part of the TiLP project
 *  Copyright (C) 1999, 2000  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributelabeld in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  This file provide inter-compatability between the different I/O functions
  according to th platform type.
  We have:
  - inb/outb under Linux
  - ouportb/inportb under DOS (Borland CONIO)
  - _inp/_outp under Windows9x (MS conio.h)
 */

#ifndef IOCOMPAT_H
#define IOCOMPAT_H

#include "ioports.h"

#if defined(__WIN16__)
# define INLINE //inline
#else
# define INLINE __inline
#endif

/* Linux ioperm function */
#if defined(__WIN32__) || defined(__WIN16__) || defined(__DOS__)
INLINE int ioperm_ (unsigned long from, unsigned long num, int turn_on)
{
  return 0;
}

INLINE int inb(unsigned addr)
{
  return rd_io(addr);
}

INLINE int outb(unsigned port, unsigned data)
{
  return wr_io(port, data);
}

#endif // not UNIX

/* Windows9x -> Linux compatibility */
#if defined(__LINUX__)
INLINE int inp_ (unsigned short addr)
{
  return rd_io(addr);
}

INLINE void outp_ (unsigned short addr, short data)
{
  wr_io(addr, data);
}
#endif // __LINUX__

/* DOS -> Linux compatibility */
#if !defined(__DOS__) && !defined(__WIN16__)
INLINE unsigned inportb (unsigned portid)
{
  return rd_io(portid);
}

INLINE void outportb (unsigned portid, unsigned char data)
{
  wr_io(portid, data);
}
#endif // __LINUX__

#endif
