/* Hey EMACS -*- macos-c -*- */
/* $Id: ioports.c 370 2004-03-22 18:47:32Z roms $ */

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
 * This unit manages direct low-level I/O operations depending on a
 * I/O method.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "intl.h"

#include "cabl_err.h"
#include "externs.h"
#include "printl.h"

/* Variables */

static int dev_fd;		// TTY handle for Linux ioctl calls (API)
static int tty_use = 0;

/* Function pointers */

int (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

/* I/O thru null code */

static int macos_null_read_io(unsigned int addr)
{
  return -1;
}

static void macos_null_write_io(unsigned int addr, int data)
{
  return;
}

/* I/O thru ioctl() calls */

static int macos_ioctl_read_io(unsigned int addr)
{
  unsigned int flags;

  if (ioctl(dev_fd, TIOCMGET, &flags) == -1) {
    printl(2, "linux_ioctl_read_io: ioctl failed !\n");
    return ERR_IOCTL;
  }

  return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
}

static void macos_ioctl_write_io(unsigned int address, int data)
{
  unsigned int flags = 0;

  flags |= (data & 2) ? TIOCM_RTS : 0;
  flags |= (data & 1) ? TIOCM_DTR : 0;
  if (ioctl(dev_fd, TIOCMSET, &flags) == -1) {
    printl(2, "linux_ioctl_write_io: ioctl failed !\n");
    return /*ERR_IOCTL */ ;
  }
}

/* Functions used for initializing the I/O routines */

int io_open(unsigned long from, unsigned long num)
{
    if (method & IOM_API) {
    struct termios termset;
    int flags = 0;

    if (tty_use)
      return 0;

    flags = O_RDWR | O_FSYNC;
    if ((dev_fd = open(io_device, flags)) == -1) {
      printl(2, "unable to open this serial port: %s\n", io_device);
      return ERR_OPEN_SER_DEV;
    }

    tcgetattr(dev_fd, &termset);
    cfmakeraw(&termset);

    io_rd = linux_ioctl_read_io;
    io_wr = linux_ioctl_write_io;

    tty_use++;

    return 0;
  } else
    return ERR_ROOT;

 /* keep that for now until I'm sure it Works (tm) */
  io_rd = null_read_io;
  io_wr = null_write_io;

  return 0;
}

int io_close(unsigned long from, unsigned long num)
{
  if (method & IOM_API) {
    if (tty_use) {
      close(dev_fd);
      tty_use--;
    }
  } else
    return -1;

  return 0;
}

