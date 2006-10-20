/* Hey EMACS -*- linux-c -*- */
/* $Id: link_slv.c 2955 2006-09-24 17:11:11Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2006  Romain Lievin
 *  Copyright (C) 2001 Julien Blache (original author)
 *  Portions lifted from libusb (LGPL):
 *  Copyright (c) 2000-2003 Johannes Erdfelt <johannes@erdfelt.com>
 *  Modifications for libticables Copyright (C) 2005 Kevin Kofler
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

/* TI-GRAPH LINK USB and direct USB cable support (kernel: testing) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "../timeout.h"

#ifdef HAVE_LINUX_TICABLE_H
# include <linux/ticable.h>     //ioctl codes
# include <sys/ioctl.h>
# ifndef IOCTL_TIUSB_GET_MAXPS
#  define IOCTL_TIUSB_GET_MAXPS      _IOR('N', 0x23, int) /* max packet size */
# endif
# ifndef IOCTL_TIUSB_GET_DEVID
#  define IOCTL_TIUSB_GET_DEVID      _IOR('N', 0x24, int) /* get device type */
#endif
#endif

/* Constants */

#define MAX_CABLES   4

#define VID_TIGLUSB  0x0451     /* Texas Instruments, Inc.            */
#define PID_TIGLUSB  0xE001     /* TI-GRAPH LINK USB (SilverLink)     */
#define PID_TI89TM   0xE004     /* TI89 Titanium w/ embedded USB port */
#define PID_TI84P    0xE008     /* TI84+ w/ embedded USB port         */

typedef struct
{
    int               dev_fd;

    int               nBytesRead2;
    uint8_t           rBuf2[64];
    uint8_t*          rBufPtr2;

    int               max_ps2;
} usb_struct2;

// convenient macros
#define dev_fd      (((usb_struct2 *)(h->priv2))->dev_fd)
#define max_ps2     (((usb_struct2 *)(h->priv2))->max_ps2)
#define nBytesRead2 (((usb_struct2 *)(h->priv2))->nBytesRead2)
#define rBuf2       (((usb_struct2 *)(h->priv2))->rBuf2)
#define rBufPtr2    (((usb_struct2 *)(h->priv2))->rBufPtr2)

static int devlist[MAX_CABLES+1] = { 0 };

static int dev_prepare(CableHandle *h)
{
    int ret;
    char str[64];
    
    if(h->port >= MAX_CABLES)
	return ERR_ILLEGAL_ARG;
    
    h->address = h->port-1;
    sprintf(str, "/dev/tiusb%i", h->address);
    h->device = strdup(str);
    h->priv2 = (usb_struct2 *)calloc(1, sizeof(usb_struct2));

    // detect stuffs
    ret = check_for_tiusb(h->device);
    if(ret)
    {
	free(h->priv2);  h->priv2 = NULL;
	free(h->device); h->device = NULL;
	return ret;
    }

    return 0;
}

static int dev_open(CableHandle *h)
{
    int mask = O_RDWR | O_SYNC;

    if ((dev_fd = open(h->device, mask)) == -1) 
    {
	ticables_warning(_("unable to open this device: %s.\n"), h->device);
	return ERR_LIBUSB_OPEN;
    }

#ifdef HAVE_LINUX_TICABLE_H
    {
	int arg = 0;
	
       	if (ioctl(dev_fd, IOCTL_TIUSB_GET_MAXPS, &arg) == -1) 
	{
	    ticables_warning(_("unable to get max packet size (ioctl).\n"));
	    //return ERR_IOCTL;
	} else
	{
	    max_ps2 = arg;
	}
    }
#endif
#ifdef HAVE_LINUX_TICABLE_H    
    {
	int arg = h->timeout;
	
	if (ioctl(dev_fd, IOCTL_TIUSB_TIMEOUT, arg) == -1) 
	{
	    ticables_warning(_("unable to set timeout (ioctl).\n"));
	    //return ERR_IOCTL;
	}
    }
#endif
#ifdef HAVE_LINUX_TICABLE_H
    {
	int arg = 0;

	if (ioctl(dev_fd, IOCTL_TIUSB_RESET_PIPES, arg) == -1) 
	{
	    ticables_warning(_("unable to reset pipes (ioctl).\n"));
	    //return ERR_IOCTL;
	}
    }
#endif
  
  /* Clear buffers */
  nBytesRead2 = 0;

  return 0;
}

static int dev_close(CableHandle *h)
{
    if (dev_fd) 
    {
	close(dev_fd);
	dev_fd = 0;
    }

    free(h->priv2);
    h->priv2 = NULL;

    return 0;
}

static int dev_reset(CableHandle *h)
{
#ifdef HAVE_LINUX_TICABLE_H
    {
	int arg = 0;
	
	if (ioctl(dev_fd, IOCTL_TIUSB_RESET_PIPES, arg) == -1) 
	{
	    ticables_warning(_("unable to reset pipes (ioctl).\n"));
	    //return ERR_IOCTL;
	}
    }
#endif

    return 0;
}

static int dev_put(CableHandle* h, uint8_t *data, uint32_t len)
{
    int ret;
    int bytesdone = 0;

    do
      {
	ret = write(dev_fd, (void *)(data + bytesdone), len - bytesdone);
  
	if(ret == -ETIMEDOUT) 
	  {
	    ticables_warning("write (%s).\n", usb_strerror());
	    return ERR_WRITE_TIMEOUT;
	  } 
	else if(ret == -EPIPE) 
	  {
	    ticables_warning("write (%s).\n", usb_strerror());
	    return ERR_WRITE_ERROR;
	  } 
	else if(ret < 0) 
	  {
	    ticables_warning("write (%s).\n", usb_strerror());
	    return ERR_WRITE_ERROR;
	  }
	
	bytesdone += ret;
      } while(bytesdone < len);

    return 0;
}

static int dev_get_(CableHandle *h, uint8_t *data)
{
    int ret = 0;
    
    /* Read up to 32/64 bytes and store them in a buffer for 
       subsequent accesses */
    if (nBytesRead2 <= 0) 
    {
	do 
	{
	    ret = read(dev_fd, (void *) rBuf2, max_ps2);
	    if (ret == 0)
	      ticables_warning("\nweird, read returns without any data & erro\r; retrying...\n");

	}
	while(!ret);
	
	if(ret == -ETIMEDOUT) 
	{
	    ticables_warning("read (%s).\n", usb_strerror());
	    nBytesRead2 = 0;
	    return ERR_READ_TIMEOUT;
	} 
	else if(ret == -EPIPE) 
	{
	    ticables_warning("read (%s).\n", usb_strerror());
	    nBytesRead2 = 0;
	    return ERR_READ_ERROR;
	} 
	else if(ret < 0) 
	{
	    ticables_warning("read (%s).\n", usb_strerror());
	    nBytesRead2 = 0;
	    return ERR_READ_ERROR;
	}

	nBytesRead2 = ret;
	rBufPtr2 = rBuf2;
    }

    *data = *rBufPtr2++;
    nBytesRead2--;

    return 0;
}

static int dev_get(CableHandle* h, uint8_t *data, uint32_t len)
{
    int i;

    // we can't do that in any other way because dev_get_ can returns
    // 1, 2, ..., len bytes.
    for(i = 0; i < len; i++)
	TRYC(dev_get_(h, data+i));
    
    return 0;
}

static int dev_enum(void)
{
    int i;

    memset(devlist, 0, sizeof(devlist));
    for(i = 0; i < MAX_CABLES; i++)
    {
        int fd = -1;
        int arg = 0;
        int mask = O_RDWR | O_NONBLOCK | O_SYNC;
	char devname[64];

	sprintf(devname, "/dev/tiusb%i", i);
        if((fd = open(devname, mask)) == -1)
            return ERR_PROBE_FAILED;

        if(ioctl(fd, IOCTL_TIUSB_GET_DEVID, &arg) == -1)
            return ERR_PROBE_FAILED;
	devlist[i] = arg;

        close(fd);
    }    

    return 0;
}

static int dev_probe(CableHandle *h)
{    
    int i;

    TRYC(dev_enum());

    for(i = 0; i < MAX_CABLES; i++)
    {
        if(devlist[i] == PID_TIGLUSB || devlist[i] == PID_TI89TM ||
	   devlist[i] == PID_TI84P)
	    return 0;
    }

    return ERR_PROBE_FAILED;
}

static int dev_check(CableHandle *h, int *status)
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
    case 0:			//no data
	return 0;
    default:			// data available
	*status = STATUS_RX;
    	break;
    }
    
    return 0;
}

static int dev_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

static int dev_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

static int dev_get_red_wire(CableHandle *h)
{
	return 1;
}

static int dev_get_white_wire(CableHandle *h)
{
	return 1;
}

const CableFncts cable_dev =
{
	CABLE_DEV,
	"USB-DEV",
	N_("Direct Link"),
	N_("Direct Link cable"),
	0,
	&dev_prepare,
	&dev_open, &dev_close, &dev_reset, &dev_probe, NULL,
	&dev_put, &dev_get, &dev_check,
	&dev_set_red_wire, &dev_set_white_wire,
	&dev_get_red_wire, &dev_get_white_wire,
};

//=======================

// returns number of devices and list of PIDs (dynamically allocated)
TIEXPORT1 int TICALL usb_probe_devices2(int **list)
{
    int i;

    TRYC(dev_enum());

    *list = (int *)calloc(MAX_CABLES+1, sizeof(int));
    for(i = 0; i < MAX_CABLES; i++)
        (*list)[i] = devlist[i];

    return 0;
}
