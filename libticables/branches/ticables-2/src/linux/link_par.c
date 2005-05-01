/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* "Home-made parallel" link cable unit */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#ifdef HAVE_LINUX_TICABLE_H
# include <linux/ticable.h>
# include <sys/ioctl.h>
#endif

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "ioports.h"

//#define dev_fd      (int)(h->priv)
static int dev_fd = -1;

static struct cs {
  	uint8_t data;
  	int available;
} cs;

static int par_prepare(TiHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0x378; h->device = strdup("/dev/tipar0"); break;
	case PORT_2: h->address = 0x278; h->device = strdup("/dev/tipar1"); break;
	case PORT_3: h->address = 0x3bc; h->device = strdup("/dev/tipar2"); break;
	default: return ERR_ILLEGAL_ARG;
	}

	// check

	return 0;
}

static int par_open(TiHandle *h)
{
    int mask;
    
    /* Init some internal variables */
    cs.available = 0;
    cs.data = 0;
    
    /* Open the device */
    mask = O_RDWR | /*O_NONBLOCK | */ O_SYNC;
    if ((dev_fd = open(h->device, mask)) == -1) 
    {
	switch(errno) 
	{
	case ENODEV: ticables_warning(_("unable to open character device: %s.\n"), h->device); break;
	case EACCES: ticables_warning(_("unable to open character device: %s (wrong permissions).\n"), h->device); break;
	default: ticables_warning(_("unable to open character device: %s\n"), h->device); break;
	}
    	return ERR_OPEN;
    }
    
    /* Set timeout and inter-bit delay */
#if defined(HAVE_LINUX_TICABLE_H)
    if (ioctl(dev_fd, IOCTL_TIPAR_DELAY, delay) == -1) 
    {
	ticables_warning(_("failed to set delay.\n"));
	return ERR_IOCTL;
    }
    if (ioctl(dev_fd, IOCTL_TIPAR_TIMEOUT, h->timeout) == -1) {
	ticables_warning(_("failed to set timeout.\n"));
	return ERR_IOCTL;
    }
#endif

	return 0;
}

static int par_close(TiHandle *h)
{
    if (dev_fd) 
    {
	close(dev_fd);
	dev_fd = 0;
    }
    
    return 0;
}

static int par_reset(TiHandle *h)
{
	return 0;
}

static int par_put(TiHandle *h, uint8_t *data, uint16_t len)
{
    int err;

    err = write(dev_fd, (void *)data, len);
    if (err <= 0) 
    {
	if (errno == ETIMEDOUT)
	    return ERR_WRITE_TIMEOUT;
    	else
	    return ERR_WRITE_ERROR;
    }
    
    return 0;
}

static int par_get(TiHandle *h, uint8_t *data, uint16_t len)
{
    int err = 0;

    /* If the dev_check function was previously called, retrieve the uint8_t */
    if (cs.available) 
    {
	*data = cs.data;
	cs.available = 0;
	return 0;
    }
    
    err = read(dev_fd, (void *) data, 1);
    if (err <= 0) 
    {
	if (errno == ETIMEDOUT)
	    return ERR_READ_TIMEOUT;
	else
	    return ERR_READ_ERROR;
    }
    
    return 0;
}

static int par_probe(TiHandle *h)
{
	return 0;
}

static int par_check(TiHandle *h, int *status)
{
    int n = 0;
    
    /* Since the select function does not work, I do it myself ! */
    *status = STATUS_NONE;
    if (dev_fd) 
    {
	n = read(dev_fd, (void *) (&cs.data), 1);
	if (n > 0) 
	{
	    if (cs.available == 1)
		return ERR_BYTE_LOST;
	    
	    cs.available = 1;
	    *status = STATUS_RX;
	    return 0;
	}
	else 
	{
	    *status = STATUS_NONE;
	    return 0;
	}
    }
    
    return 0;
}

static int par_set_red_wire(TiHandle *h, int b)
{
    return 0;
}

static int par_set_white_wire(TiHandle *h, int b)
{
    return 0;
}

static int par_get_red_wire(TiHandle *h)
{
    return 1;
}

static int par_get_white_wire(TiHandle *h)
{
    return 1;
}

const TiCable cable_par = 
{
	CABLE_PAR,
	"PAR",
	N_("Parallel"),
	N_("Home-made parallel cable"),

	&par_prepare, &par_probe,
	&par_open, &par_close, &par_reset,
	&par_put, &par_get, &par_check,
	&par_set_red_wire, &par_set_white_wire,
	&par_get_red_wire, &par_get_white_wire,
};
