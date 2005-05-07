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
 * This unit manages direct low-level and user-landI/O operations.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_LINUX_PARPORT_H
#include <linux/parport.h>
#include <linux/ppdev.h>
#endif

#include "gettext.h"
#include "error.h"
#include "logging.h"

#ifdef HAVE_LINUX_PARPORT_H

int par_io_open(const char *dev_name, int *dev_fd)
{
    int mode;

    // Open device
    *dev_fd = open(dev_name, O_RDWR);
    if (*dev_fd == -1) 
    {
	ticables_warning("unable to open parallel device '%s'.", dev_name);
	return ERR_PPT_OPEN;
    }

    // Claim access
    if (ioctl(*dev_fd, PPCLAIM) == -1) 
    {
	ticables_warning(_("ioctl failed on parallel device: can't claim parport."));
	return ERR_PPT_IOCTL;
    }

    // and exclusive access !
    if (ioctl(*dev_fd, PPEXCL) == -1)
    {
        ticables_warning(_("ioctl failed on parallel device: can't claim exclusive access."));
        return ERR_PPT_IOCTL;
    }

    // Change transfer mode
    //mode = PARPORT_MODE_PCSPP;
    mode = IEEE1284_MODE_COMPAT;
    if (ioctl(*dev_fd, PPSETMODE, &mode) == -1)
    {
	ticables_warning(_("ioctl failed on parallel device: can't change transfer mode."));
	return ERR_PPT_IOCTL;
    }

    return 0;
}

int par_io_close(int dev_fd)
{
    // release access
    if (ioctl(dev_fd, PPRELEASE) == -1)
    {
        ticables_warning(_("ioctl failed on parallel device: can't release parport."));
        return ERR_PPT_IOCTL;
    }

    // and close
    return close(dev_fd);
}

int par_io_rd(int dev_fd, uint8_t *data)
{
    if (ioctl(dev_fd, PPRSTATUS, data) == -1)
    {
        ticables_warning(_("ioctl failed on parallel device: can't read status lines."));
        return ERR_PPT_IOCTL;
    }
    
    return 0;
}

int par_io_wr(int dev_fd, uint8_t data)
{
    if(write(dev_fd, &data, 1) < 1)
    {
	ticables_warning(_("write failed on parallel device: can't write value."));
	return ERR_WRITE_ERROR;
    }

    return 0;
}

#endif

// ---

#ifdef HAVE_TERMIOS_H

int ser_io_open(const char *dev_name, int *dev_fd)
{
    *dev_fd = open(dev_name, O_RDWR | O_SYNC);
    if (*dev_fd == -1) 
    {
	ticables_warning("unable to open serial device '%s'", dev_name);
	return ERR_TTY_OPEN;
    }

    return 0;
}

int ser_io_close(int dev_fd)
{
    return close(dev_fd);
}

int ser_io_rd(int dev_fd)
{
    unsigned int flags;

    if (ioctl(dev_fd, TIOCMGET, &flags) == -1) 
    {
	ticables_warning(_("ioctl failed on serial device."));
	return ERR_TTY_IOCTL;
    }

    return (flags & TIOCM_CTS ? 0x10 : 0) | (flags & TIOCM_DSR ? 0x20 : 0);
}

int ser_io_wr(int dev_fd, uint8_t data)
{
    unsigned int flags = 0;

    flags |= (data & 2) ? TIOCM_RTS : 0;
    flags |= (data & 1) ? TIOCM_DTR : 0;
    
    if (ioctl(dev_fd, TIOCMSET, &flags) == -1) 
    {
	ticables_warning(_("ioctl failed on serial device."));
        return ERR_TTY_IOCTL;
    }
    
    return 0;
}

#endif
