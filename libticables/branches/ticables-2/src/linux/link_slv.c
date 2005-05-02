/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (C) 2001 Julien Blache (original author)
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

/* TI-GRAPH LINK USB and direct USB cable support (lib-usb) */

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   
   This link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
   This is transparent for the user because the libusb manages all these 
   things for us. Nethertheless, this fact has some consequences:

   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.

   - for reading, we have to read up to 32 bytes at a time (even if we need 
   only 1 byte) and to store them in a buffer for subsequent accesses. 
   In fact, if we try and get byte per byte, it will not work.

   - for writing, we don't store bytes in a buffer. It seems better to send
   data byte per byte (latency ?!). But, this make data-rate significantly 
   decrease (1KB/s instead of 5KB/s).
   Another way is to use partially buffered write operations: send consecutive
   blocks as a whole but partial block byte per byte. This is the best 
   compromise and it works fine !

   - another particular effect (quirk): sometimes (usually when calc need to 
   reply and takes a while), a read call can returns with neither data 
   nor timeout. Simply retry a read call and it works fine. The best example 
   is to get IDLIST.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <usb.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "ioports.h"

/* Constants */

#define MAX_CABLES   4

#define VID_TIGLUSB  0x0451     /* Texas Instruments, Inc.            */
#define PID_TIGLUSB  0xE001     /* TI-GRAPH LINK USB (SilverLink)     */
#define PID_TI89TM   0xE004     /* TI89 Titanium w/ embedded USB port */
#define PID_TI84P    0xE008     /* TI84+ w/ embedded USB port         */

#define TIGL_BULK_IN    0x81
#define TIGL_BULK_OUT   0x02

#define to      (100 * h->timeout)        // in ms

/* Types */

typedef struct
{
    uint16_t vid;
    uint16_t pid;
    char*    str;

    struct usb_device *dev;
} usb_infos;

static usb_infos tigl_infos[] =
{
        {0x0451, 0xe001, "SilverLink", NULL},
        {0x0451, 0xe004, "TI89 Titanium", NULL},
        {0x0451, 0xe008, "TI84 Plus", NULL},
        {}
};

static usb_infos tigl_devices[MAX_CABLES];// = { 0 }; 
static int ndevices = 0;

typedef struct
{
    struct usb_device *tigl_dev;
    usb_dev_handle    *tigl_han;
    
    int     max_ps;
    
    int     nBytesWrite;
    uint8_t *wBuf;
    
    int     nBytesRead;
    int8_t *rBuf;
} usb_struct;

#define tigl_dev (((usb_struct *)(h->priv2))->tigl_dev)
#define tigl_han (((usb_struct *)(h->priv2))->tigl_han)
#define max_ps   (((usb_struct *)(h->priv2))->max_ps)
#define nBytesWrite (((usb_struct *)(h->priv2))->nBytesWrite)
#define wBuf        (((usb_struct *)(h->priv2))->wBuf)
#define nBytesRead  (((usb_struct *)(h->priv2))->nBytesRead)
#define rBuf        (((usb_struct *)(h->priv2))->rBuf)

/* Helpers */

static void find_tigl_devices(void)
{
    struct usb_bus    *bus;
    struct usb_device *dev;
    int i;
    
    ndevices = 0;
    /* loop taken from testlibusb.c */
    for (bus = usb_busses; bus; bus = bus->next)
    {
	for (dev = bus->devices; dev; dev = dev->next)
	{
	    if ((dev->descriptor.idVendor == VID_TIGLUSB))
	    {
		for(i = 0; i < sizeof(tigl_infos) / sizeof(usb_infos); i++)
		{
		    if(dev->descriptor.idProduct == tigl_infos[i].pid)
		    {
			/* keep track of the TIGL device */
			ticables_info(_("found <%s>."), tigl_infos[i].str);
			memcpy(&tigl_devices[ndevices], &tigl_infos[i], 
			       sizeof(usb_infos));
			tigl_devices[ndevices++].dev = dev;
		    }
		}
	    }
	}
    }
}

static int enumerate_tigl_devices(void)
{
    int ret = 0;
    
    /* init the libusb */
    usb_init();
    
    /* find all usb busses on the system */
    ret = usb_find_busses();
    if (ret < 0)
    {
	ticables_warning(_("usb_find_busses (%s).\n"), usb_strerror());
	return ERR_LIBUSB_OPEN;
    }
    
    /* find all usb devices on all discovered busses */
    ret = usb_find_devices();
    if (ret < 0)
    {
	ticables_warning(_("usb_find_devices (%s).\n"), usb_strerror());
	return ERR_LIBUSB_OPEN;
    }
    
    /* iterate through the busses/devices */
    find_tigl_devices();

    return 0;
}    

int open_tigl_device(int id, usb_dev_handle **udh)
{
    int ret; 

    ret = enumerate_tigl_devices();
    if(ret) return ret;

    if(tigl_devices[id].dev == NULL)
	return ERR_ILLEGAL_ARG;

    *udh = usb_open(tigl_devices[id].dev);
    if (*udh != NULL) 
    {
	/* interface 0, configuration 1 */
	ret = usb_claim_interface(*udh, 0);
	if (ret < 0) 
	{
	    ticables_warning("usb_claim_interface (%s).\n",
			     usb_strerror());
	    return ERR_LIBUSB_INIT;
	}
	
	ret = usb_set_configuration(*udh, 1);
	if (ret < 0) 
	{
	    ticables_warning("usb_set_configuration (%s).\n",
			     usb_strerror());
	    return ERR_LIBUSB_INIT;
	}
	return 0;
    } 
    else
	return ERR_LIBUSB_OPEN;

    return 0;
}

static int reset_pipes(usb_struct *s)
{
#if 0
	int ret;

	printf("resetting pipes... ");

  	// Reset out pipe
  	ret = usb_clear_halt(tigl_han, TIGL_BULK_OUT);
  	if (ret < 0) {
    		ticables_warning("usb_clear_halt (%s).\n", usb_strerror());

    		ret = usb_resetep(tigl_han, TIGL_BULK_OUT);
    		if (ret < 0) {
      			ticables_warning("usb_resetep (%s).\n", usb_strerror());

      			ret = usb_reset(tigl_han);
      			if (ret < 0) {
				ticables_warning("usb_reset (%s).\n", usb_strerror());
				return ERR_LIBUSB_RESET;
      			}
    		}
  	}

	// Reset in pipe
  	ret = usb_clear_halt(tigl_han, TIGL_BULK_IN);
  	if (ret < 0) {
    		ticables_warning("usb_clear_halt (%s).\n", usb_strerror());

	    	ret = usb_resetep(tigl_han, TIGL_BULK_OUT);
	    	if (ret < 0) {
	      		ticables_warning("usb_resetep (%s).\n", usb_strerror());

	      		ret = usb_reset(tigl_han);
	      		if (ret < 0) {
				ticables_warning("usb_reset (%s).\n", usb_strerror());
				return ERR_LIBUSB_RESET;
	      		}
	    	}
  	}

	printf("done !\n");
#endif
	return 0;
}

/* API */

static int slv_prepare(TiHandle *h)
{
	char str[64];

	if(h->port >= MAX_CABLES)
	    return ERR_ILLEGAL_ARG;

	h->address = h->port;
	sprintf(str, "TiglUsb #%i", h->port);
	h->device = strdup(str);
	h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));

	//if(!check_for_libusb())
	//  return -1;

	return 0;
}

static int slv_open(TiHandle *h)
{
    int ret;

    // open device
    tigl_dev = tigl_devices[h->address-1].dev;
    ret = open_tigl_device(h->address, &tigl_han);
    if(ret)
	return ret;
    
// get max packet size
    max_ps = 32;

// allocate buffers
    wBuf = (uint8_t *)malloc(max_ps * sizeof(uint8_t));
    rBuf = (uint8_t *)malloc(max_ps * sizeof(uint8_t));
    if((wBuf == NULL) || (rBuf == NULL))
    {
	free(wBuf);
	free(rBuf);
	return ERR_OPEN_USB_DEV;
    }

#if 0 //!defined(__BSD__)
 {
     /* Reset both endpoints */
     int ret = reset_pipes();
     if(ret) return ret;
 }
#endif

    return 0;
}

static int slv_close(TiHandle *h)
{
    free(wBuf); wBuf = NULL;
    free(rBuf); rBuf = NULL;

    tigl_dev = NULL;

    if (tigl_han != NULL) 
    {
	usb_release_interface(tigl_han, 0);
	usb_close(tigl_han);
	tigl_han = NULL;
    }

    free(h->priv2);

    return 0;
}

static int slv_reset(TiHandle *h)
{
  return 0;
}

static int slv_put(TiHandle* h, uint8_t *data, uint16_t len)
{
  return 0;
}

static int slv_get(TiHandle* h, uint8_t *data, uint16_t len)
{
  return 0;
}

static int slv_probe(TiHandle *h)
{
    enumerate_tigl_devices();

    if(tigl_devices[h->address-1].pid == PID_TIGLUSB)
	return 0;

    return ERR_PROBE_FAILED;
}

static int raw_probe(TiHandle *h)
{
    enumerate_tigl_devices();

    if(tigl_devices[h->address-1].pid == PID_TI89TM ||
       tigl_devices[h->address-1].pid == PID_TI84P)
        return 0;
    
    return ERR_PROBE_FAILED;
}

static int slv_check(TiHandle *h, int *status)
{
  	return 0;
}

static int slv_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

static int slv_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

static int slv_get_red_wire(TiHandle *h)
{
	return 1;
}

static int slv_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_slv =
{
	CABLE_SLV,
	"SLV",
	N_("SilverLink"),
	N_("SilverLink (TI-GRAPH LINK USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &slv_probe,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
};

const TiCable cable_raw =
{
	CABLE_USB,
	"USB",
	N_("DirectLink"),
	N_("DirectLink (direct USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &raw_probe,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
};
