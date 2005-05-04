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

/* "Grey TIGraphLink" link cable unit */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

#define dev_fd      ((int)(h->priv))
#define termset     ((struct termios *)(h->priv2))

static int gry_prepare(TiHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0x3f8; h->device = strdup("/dev/ttyS0"); break;
	case PORT_2: h->address = 0x2f8; h->device = strdup("/dev/ttyS1"); break;
	case PORT_3: h->address = 0x3e8; h->device = strdup("/dev/ttyS2"); break;
	case PORT_4: h->address = 0x3e8; h->device = strdup("/dev/ttyS3"); break;
	default: return ERR_ILLEGAL_ARG;
	}
	h->priv2 = (struct termios *)calloc(1, sizeof(struct termios));

	TRYC(check_for_tty(h->device));

	return 0;
}

static int gry_open(TiHandle *h)
{
    int flags = 0;
    
#if defined(__MACOSX__)
    flags = O_RDWR | O_NDELAY;
#elif defined(__BSD__)
    flags = O_RDWR | O_FSYNC;
#elif defined(__LINUX__)
    flags = O_RDWR | O_SYNC;
#endif
    
    if ((dev_fd = open(h->device, flags)) == -1) 
    {
	if(errno == EACCES)
	    ticables_warning(_("unable to open this serial port: %s (wrong permissions).\n"), h->device);
	else
	    ticables_warning(_("unable to open this serial port: %s\n"), h->device);
	return ERR_GRY_OPEN;
    }
    
    // Initialize it: 9600,8,N,1
    tcgetattr(dev_fd, termset);
#ifdef HAVE_CFMAKERAW
    cfmakeraw(termset);
#else
    termset->c_iflag = 0;
    termset->c_oflag = 0;
    termset->c_cflag = CS8 | CLOCAL | CREAD;
    termset->c_lflag = 0;
#endif

    cfsetispeed(termset, B9600);
    cfsetospeed(termset, B9600);

    return 0;
}

static int gry_close(TiHandle *h)
{
    close(dev_fd);
    free(h->priv2);
    h->priv2 = NULL;

    return 0;
}

static int gry_reset(TiHandle *h)
{
    uint8_t unused[1024];
    int n;

    /* Flush the input */
    termset->c_cc[VMIN] = 0;
    termset->c_cc[VTIME] = 0;
    tcsetattr(dev_fd, TCSANOW, termset);
    do
    {
	n = read(dev_fd, (void *) unused, 1024);
    } while ((n != 0) && (n != -1));

    /* and set/restore the timeout */
    termset->c_cc[VTIME] = h->timeout;
    tcsetattr(dev_fd, TCSANOW, termset);
    
    return 0;
}

static int gry_put(TiHandle* h, uint8_t *data, uint16_t len)
{
    int err;

    err = write(dev_fd, (void *)data, len);
    switch (err) 
    {
    case -1:		//error
	return ERR_WRITE_ERROR;
    	break;
    case 0:			// timeout
	return ERR_WRITE_TIMEOUT;
    	break;
    }
    
    return 0;
}

static int gry_get(TiHandle* h, uint8_t *data, uint16_t len)
{
    int err;
    
    tcdrain(dev_fd);	// waits for all output written
    
    err = read(dev_fd, (void *)data, len);
    switch (err) 
    {
    case -1:		//error
	return ERR_READ_ERROR;
    	break;
    case 0:		// timeout
	return ERR_READ_TIMEOUT;
    	break;
    }
    
    return 0;
}

// Migrate these functions into ioports.c
static int dcb_read_io(TiHandle *h)
{
#ifdef HAVE_TERMIOS_H	
    unsigned int flags;
    
    if (ioctl(dev_fd, TIOCMGET, &flags) == -1)
	return ERR_GRY_IOCTL;
    
    return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
#endif
}

static int dcb_write_io(TiHandle *h, int data)
{
#ifdef HAVE_TERMIOS_H
    unsigned int flags = 0;
    
    flags |= (data & 2) ? TIOCM_RTS : 0;
    flags |= (data & 1) ? TIOCM_DTR : 0;
    if (ioctl(dev_fd, TIOCMSET, &flags) == -1)
	return ERR_GRY_IOCTL;
    
    return 0;
#endif
}

static int gry_probe(TiHandle *h)
{
    int i;
    int seq_in[] =  { 3, 2, 0, 1, 3 };
    int seq_out[] = { 2, 0, 0, 2, 2 };

    for (i = 0; i < 5; i++) 
    {
	dcb_write_io(h, seq_in[i]);
	usleep(1000);
	//printf("%i : %i\n", seq[i], dcb_read_io() & 0x3);

	if ((dcb_read_io(h) & 0x3) != seq_out[i]) 
	{
	    dcb_write_io(h, 3);
	    return ERR_PROBE_FAILED;
	}

    }

    return 0;
}

static int gry_check(TiHandle *h, int *status)
{
    fd_set rdfs;
    struct timeval tv;
    int retval;
    
    if (dev_fd < 0)
	return ERR_READ_ERROR;
    
    *status = STATUS_NONE;
    
    FD_ZERO(&rdfs);
    FD_SET(dev_fd, &rdfs);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    retval = select(dev_fd + 1, &rdfs, NULL, NULL, &tv);
    switch (retval) {
    case -1:			//error
	return ERR_READ_ERROR;
    case 0:			//no data
	return 0;
    default:			// data available
	*status = STATUS_RX;
    	break;
    }
    
    return 0;
}

static int gry_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

static int gry_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

static int gry_get_red_wire(TiHandle *h)
{
	return 1;
}

static int gry_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_gry = 
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),
	!0,
	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe,
	&gry_put, &gry_get, &gry_check,
	&gry_set_red_wire, &gry_set_white_wire,
	&gry_get_red_wire, &gry_get_white_wire,
};
