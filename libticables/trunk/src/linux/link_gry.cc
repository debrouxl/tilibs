/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#include "../internal.h"
#include "detect.h"
#include "../bsd/detect.h"
#include "../macos/detect.h"

#define dev_fd      (GPOINTER_TO_INT(h->priv))
#define termset     ((struct termios *)(h->priv2))

#if defined(__BSD__)
#if defined(__NetBSD__)
#define DEVNAME "dty0"
#else /* FreeBSD */
#define DEVNAME "cuad"
#endif
#else
#define DEVNAME "ttyS"
#endif

static int gry_prepare(CableHandle *h)
{
	const char * device;
	int ret;

	switch (h->port)
	{
		case PORT_1: h->address = 0x3f8; device = "/dev/" DEVNAME "0"; break;
		case PORT_2: h->address = 0x2f8; device = "/dev/" DEVNAME "1"; break;
		case PORT_3: h->address = 0x3e8; device = "/dev/" DEVNAME "2"; break;
		case PORT_4: h->address = 0x3e8; device = "/dev/" DEVNAME "3"; break;
		default: return ERR_ILLEGAL_ARG;
	}

	if (h->device == NULL)
	{
		h->device = strdup(device);
	}
	h->priv2 = (struct termios *)calloc(1, sizeof(struct termios));

#if defined(__MACOSX__)
	ret = macosx_check_tty(h->device);
#elif defined(__BSD__)
	ret = bsd_check_tty(h->device);
#else
	ret = linux_check_tty(h->device);
#endif
	if (ret)
	{
		free(h->device); h->device = NULL;
		free(h->priv2);  h->priv2 = NULL;
		return ret;
	}

	return 0;
}

static int gry_open(CableHandle *h)
{
	int flags = 0;

#if defined(__MACOSX__)
	flags = O_RDWR | O_NDELAY;
#elif defined(__BSD__)
	flags = O_RDWR | O_FSYNC;
#elif defined(__LINUX__)
	flags = O_RDWR | O_SYNC;
#endif

	//dev_fd = (int)open(h->device, flags);
	h->priv = GINT_TO_POINTER(open(h->device, flags));
	if (dev_fd == -1) 
	{
		if (errno == EACCES)
		{
			ticables_warning(_("unable to open this serial port: %s (wrong permissions).\n"), h->device);
		}
		else
		{
			ticables_warning(_("unable to open this serial port: %s\n"), h->device);
		}
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
	termset->c_cc[VMIN] = 0;
	termset->c_cc[VTIME] = h->timeout;

	cfsetispeed(termset, B9600);
	cfsetospeed(termset, B9600);
	tcsetattr(dev_fd, TCSANOW, termset);

	if (tcflush(dev_fd, TCIOFLUSH) == -1)
	{
		return ERR_FLUSH_ERROR;
	}

	return 0;
}

static int gry_close(CableHandle *h)
{
	close(dev_fd);
	free(h->priv2);
	h->priv2 = NULL;

	return 0;
}

static int gry_reset(CableHandle *h)
{
	if (tcflush(dev_fd, TCIOFLUSH) == -1)
	{
		return ERR_FLUSH_ERROR;
	}

	return 0;
}

static int gry_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	ssize_t ret;

	ret = write(dev_fd, (void *)data, len);
	switch (ret) 
	{
	case -1:		//error
		return ERR_WRITE_ERROR;
	case 0:		// timeout
		return ERR_WRITE_TIMEOUT;
	}

	return 0;
}

static int gry_get(CableHandle* h, uint8_t *data, uint32_t len)
{
	ssize_t ret;
	ssize_t i;

	tcdrain(dev_fd);	// waits for all output written

	// Doesn't work as expected by the manpage. Use a 'for' loop instead.
	//termset->c_cc[VMIN] = len;
	//tcsetattr(dev_fd, TCSANOW, termset);

	for(i = 0; i < len; )
	{
		ret = read(dev_fd, (void *)(data+i), len - i);
		switch (ret) 
		{
		case -1:		//error
			return ERR_READ_ERROR;
		case 0:		// timeout
			return ERR_READ_TIMEOUT;
		}

		i += ret;
	}

	return 0;
}

// Migrate these functions into ioports.c
static int dcb_read_io(CableHandle *h)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags;

	if (ioctl(dev_fd, TIOCMGET, &flags) == -1)
	{
		return ERR_GRY_IOCTL;
	}

	return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
#endif
}

static int dcb_write_io(CableHandle *h, int data)
{
#ifdef HAVE_TERMIOS_H
	unsigned int flags = 0;

	flags |= (data & 2) ? TIOCM_RTS : 0;
	flags |= (data & 1) ? TIOCM_DTR : 0;
	if (ioctl(dev_fd, TIOCMSET, &flags) == -1)
	{
		return ERR_GRY_IOCTL;
	}

	return 0;
#endif
}

static int gry_probe(CableHandle *h)
{
	int i;
	int seq_in[] =  { 3, 2, 0, 1, 3 };
	int seq_out[] = { 2, 0, 0, 2, 2 };

	for (i = 0; i < 5; i++) 
	{
		dcb_write_io(h, seq_in[i]);
		usleep(1000000);
		//printf("%i : %i\n", seq[i], dcb_read_io() & 0x3);

		if ((dcb_read_io(h) & 0x3) != seq_out[i]) 
		{
			dcb_write_io(h, 3);
			return ERR_PROBE_FAILED;
		}
	}

	return 0;
}

static int gry_check(CableHandle *h, int *status)
{
	fd_set rdfs;
	struct timeval tv;
	int retval;

	if (dev_fd < 0)
	{
		return ERR_READ_ERROR;
	}

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

static int gry_timeout(CableHandle *h)
{
	termset->c_cc[VTIME] = h->timeout;
	tcsetattr(dev_fd, TCSANOW, termset);

	return 0;
}

static int gry_set_device(CableHandle *h, const char * device)
{
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			free(h->device);
			h->device = device2;
		}
		else
		{
			ticables_warning(_("unable to set device %s.\n"), device);
		}
		return 0;
	}
	return ERR_ILLEGAL_ARG;
}

extern const CableFncts cable_gry = 
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),
	!0,
	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe, &gry_timeout,
	&gry_put, &gry_get, &gry_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&gry_set_device,
	NULL
};
