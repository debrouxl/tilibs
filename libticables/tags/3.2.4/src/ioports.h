/*  IOPorts - I/O low-level port access routines for Linux, Windows9x,
 *		NT4/2000, DOS.
 *	A part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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
  This file redirects low-level I/O functions according to the architecture
  and/or the platform
 */

#ifndef IOPORTS_H
#define IOPORTS_H

extern int io_mode;

/* General function of this IOPort module */
extern int open_io(unsigned long from, unsigned long num);
extern int (*rd_io) (unsigned int addr);
extern int (*wr_io) (unsigned int addr, int data);
extern int close_io(unsigned long from, unsigned long num);


/* ASM functions */
#if defined(__WIN32__) || defined(__DOS__)
__inline int inp_ (unsigned short addr)
{ 
  int c;
  
  __asm 
    { 
      mov eax, 0
	mov dx, addr
	in al, dx
	mov c, eax
	}
  
  return c;
}

__inline void outp_ (unsigned short addr, short data)
{ 
  __asm 
    { 
      mov dx, addr
	mov ax, data
	out dx, al
	}
}
#endif // Win32

#ifdef __WIN32__
int open_com_port(char *comPort, PHANDLE hCom);
int close_com_port(PHANDLE hCom);
#endif

#endif
