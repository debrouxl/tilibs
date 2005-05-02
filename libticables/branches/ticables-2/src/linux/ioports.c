/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 * This unit manages direct low-level I/O operations depending on a
 * I/O method.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#if defined(__I386__) && defined(HAVE_SYS_PERM_H)
#include <sys/perm.h>
#endif
#if defined(__I386__) && defined(HAVE_SYS_IO_H)
#include <sys/io.h>
#elif defined(__I386__) && defined(HAVE_ASM_IO_H)
#include <asm/io.h>
#endif

#include "gettext.h"
#include "error.h"

/* Function pointers */

int (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

/* I/O thru assembly code */

static int linux_asm_read_io(unsigned int addr)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H)
	return inb(addr);
#else
	return 0;
#endif
}

static void linux_asm_write_io(unsigned int addr, int data)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H)
	outb(data, addr);
#endif
}


/* I/O thru ioctl() calls */
#if 0
static int linux_ioctl_read_io(unsigned int addr)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags;

  	if (ioctl(dev_fd, TIOCMGET, &flags) == -1) {
    		printl1(2, _("ioctl failed in linux_ioctl_read_io !\n"));
    		return ERR_IOCTL;
  	}

  	return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
#endif
}

static void linux_ioctl_write_io(unsigned int address, int data)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags = 0;

  	flags |= (data & 2) ? TIOCM_RTS : 0;
  	flags |= (data & 1) ? TIOCM_DTR : 0;
  	if (ioctl(dev_fd, TIOCMSET, &flags) == -1) {
    		printl1(2, _("ioctl failed in linux_ioctl_write_io !\n"));
    		//return ERR_IOCTL;
  	}
#endif
}
#endif

/* Functions used for initializing the I/O routines */

int io_open(unsigned long from)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H)
    		io_rd = linux_asm_read_io;
    		io_wr = linux_asm_write_io;

		return (ioperm(from, 3, 1) ? ERR_ROOT : 0);
#else
		return ERR_ROOT;
#endif

	return 0;
}

int io_close(unsigned long from)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H)
    		return (ioperm(from, 3, 0) ? ERR_ROOT : 0);
#else
		return 0;
#endif
	return 0;
}
