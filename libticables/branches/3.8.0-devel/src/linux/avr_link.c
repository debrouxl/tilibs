/* Hey EMACS -*- linux-c -*- */
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

/* "fastAVRlink" link cable unit (my own link cables) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include "intl1.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "timeout.h"
#include "printl.h"
#include "logging.h"

static char tty_dev[1024];
static int dev_fd = 0;
static struct termios termset;

int avr_close();

int avr_init()
{
	int br = BR9600;
	int flags = 0;
	
	strcpy(tty_dev, io_device);
	
#if defined(__MACOSX__)
	flags = O_RDWR | O_NDELAY;
#elif defined(__BSD__)
	flags = O_RDWR | O_FSYNC;
#elif defined(__LINUX__)
	flags = O_RDWR | O_SYNC;
#endif
	
	if ((dev_fd = open(io_device, flags)) == -1) {
		if(errno == EACCES)
			printl(2, _("libticables: unable to open this serial port: %s (wrong permissions).\n"), io_device);
		else
			printl(2, _("libticables: unable to open this serial port: %s\n"), io_device);
		return ERR_OPEN_SER_DEV;
	}
	
	/* Initialize it: 9600,8,N,1 */
	tcgetattr(dev_fd, &termset);
#ifdef HAVE_CFMAKERAW
	cfmakeraw(&termset);
	termset.c_iflag = 0;
	termset.c_oflag = 0;
	if (hfc == HFC_ON)
		termset.c_cflag = CS8 | CLOCAL | CREAD | CRTSCTS;
	else
		termset.c_cflag = CS8 | CLOCAL | CREAD;
	termset.c_lflag = 0;
#else
	termset.c_iflag = 0;
	termset.c_oflag = 0;
	if (hfc == HFC_ON)
		termset.c_cflag = CS8 | CLOCAL | CREAD | CRTSCTS;
	else
		termset.c_cflag = CS8 | CLOCAL | CREAD;
	termset.c_lflag = 0;
#endif
	
	if (baud_rate == 9600)
		br = B9600;
	else if (baud_rate == 19200)
		br = B19200;
	else if (baud_rate == 38400)
		br = B38400;
	else if (baud_rate == 57600)
		br = B57600;
	else
		br = B9600;
	
	cfsetispeed(&termset, br);
	cfsetospeed(&termset, br);
	
	START_LOGGING();
	
	return 0;
}

int avr_open()
{
	uint8_t unused[1024];
	int n;
	
	/* Flush the input */
	termset.c_cc[VMIN] = 0;
	termset.c_cc[VTIME] = 0;
	tcsetattr(dev_fd, TCSANOW, &termset);
	do {
		n = read(dev_fd, (void *) unused, 1024);
	} while ((n != 0) && (n != -1));
	
	/* and set/restore the timeout */
	termset.c_cc[VTIME] = time_out;
	tcsetattr(dev_fd, TCSANOW, &termset);
	
	tdr.count = 0;
	toSTART(tdr.start);
	
	return 0;
}

int avr_put(uint8_t data)
{
	int err;
	
	tdr.count++;
	LOG_DATA(data);
	
	err = write(dev_fd, (void *) (&data), 1);
	switch (err) {
	case -1:			//error
		avr_close();
		return ERR_WRITE_ERROR;
		break;
	case 0:			// timeout
		avr_close();
		return ERR_WRITE_TIMEOUT;
		break;
	}
	
	return 0;
}

int avr_get(uint8_t * data)
{
	int err;
	
	tcdrain(dev_fd);		// waits for all output written
	
	err = read(dev_fd, (void *) data, 1);
	switch (err) {
	case -1:			//error
		avr_close();
		return ERR_READ_ERROR;
		break;
	case 0:			        // timeout
		avr_close();
		return ERR_READ_TIMEOUT;
		break;
	}
	
	tdr.count++;
	LOG_DATA(*data);
	
	return 0;
}

int avr_probe()
{
	return 0;
}

int avr_close()
{
	return 0;
}

int avr_exit()
{
	STOP_LOGGING();
	close(dev_fd);
	return 0;
}

int avr_check(int *status)
{
	fd_set rdfs;
	struct timeval tv;
	int retval;
	
	*status = STATUS_NONE;
	
	FD_ZERO(&rdfs);
	FD_SET(dev_fd, &rdfs);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	retval = select(dev_fd + 1, &rdfs, NULL, NULL, &tv);
	switch (retval) {
	case -1:			//error
		return ERR_READ_ERROR;
	case 0:			        //no data
		return 0;
	default:			// data available
		*status = STATUS_RX;
		break;
	}
	
	return 0;
}

int avr_supported()
{
	return SUPPORT_ON;
}

int avr_register_cable(TicableLinkCable * lc, TicableMethod method)
{
  lc->init = avr_init;
  lc->open = avr_open;
  lc->put = avr_put;
  lc->get = avr_get;
  lc->close = avr_close;
  lc->exit = avr_exit;
  lc->probe = avr_probe;
  lc->check = avr_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
