/* Hey EMACS -*- bsd-c -*- */
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

/*
 * This unit manages direct low-level I/O operations depending on a
 * I/O method. Very close to the Linux i/o module.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <machine/sysarch.h>
#include <machine/cpufunc.h>

#include "intl1.h"

#include "cabl_err.h"
#include "externs.h"
#include "printl.h"

/* Variables */

static int dev_fd;	// TTY handle for bsd ioctl calls (API)
static int tty_use = 0;	// open TTY just once

/* Function pointers */

int (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

/* I/O thru assembly code */

static int bsd_asm_read_io(unsigned int addr)
{
	return inb(addr);
}

static void bsd_asm_write_io(unsigned int addr, int data)
{
	outb(addr, data);
}


/* I/O thru ioctl() calls */

static int bsd_ioctl_read_io(unsigned int addr)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags;

  	if (ioctl(dev_fd, TIOCMGET, &flags) == -1) {
    		printl1(2, _("ioctl failed in bsd_ioctl_read_io !\n"));
    		return ERR_IOCTL;
  	}

  	return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
#endif
}

static void bsd_ioctl_write_io(unsigned int address, int data)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags = 0;

  	flags |= (data & 2) ? TIOCM_RTS : 0;
  	flags |= (data & 1) ? TIOCM_DTR : 0;
  	if (ioctl(dev_fd, TIOCMSET, &flags) == -1) {
    		printl1(2, _("ioctl failed in bsd_ioctl_write_io !\n"));
    		//return ERR_IOCTL;
  	}
#endif
}

/* Functions used for initializing the I/O routines */

int io_open(unsigned long from, unsigned long num)
{
#if defined(__I386__)
	if (method & IOM_ASM) {
    		io_rd = bsd_asm_read_io;
    		io_wr = bsd_asm_write_io;

		return (i386_set_ioperm(from, num, 1) ? ERR_ROOT : 0);
	}
	else if (method & IOM_IOCTL) {
#else
	if (method & IOM_IOCTL) {
#endif
		struct termios termset;

    		if (tty_use)
      			return 0;

		if ((dev_fd = open(io_device, O_RDWR | O_FSYNC)) == -1) {
      			if(errno == EACCES)
      				printl1(2, _("unable to open this serial port: %s (wrong permissions).\n"), io_device);
      			else
      				printl1(2, _("unable to open this serial port: %s\n"), io_device);
      			return ERR_OPEN_SER_DEV;
    		}

    		tcgetattr(dev_fd, &termset);
    		cfmakeraw(&termset);

    		io_rd = bsd_ioctl_read_io;
    		io_wr = bsd_ioctl_write_io;

    		tty_use++;
	} else {
		printl1(2, "bad argument (invalid method).\n");
                return ERR_ILLEGAL_ARG;
	}

	return 0;
}

int io_close(unsigned long from, unsigned long num)
{
#if defined(__I386__)
	if (method & IOM_ASM) {
    		return (i386_set_ioperm(from, num, 0) ? ERR_ROOT : 0);
    	}
    	else if (method & IOM_IOCTL) {
#else
    	if (method & IOM_IOCTL) {
#endif
    		if (tty_use) {
      			close(dev_fd);
      			tty_use--;
    		}
	} else {
                printl1(2, "bad argument (invalid method).\n");
                return ERR_ILLEGAL_ARG;
        }
	
	return 0;
}
