/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (c) 1999-2006 Romain Lievin
 *  Copyright (c) 2001 Julien Blache (original author)
 *  Copyright (c) 2007 Romain Liévin (libusb-win32 support)
 *  Copyright (c) 2007 Kevin Kofler (libusb-win32 slv_check support)
 *
 *  Portions lifted from libusb (LGPL):
 *  Copyright (c) 2000-2003 Johannes Erdfelt <johannes@erdfelt.com>
 *  Modifications for libticables Copyright (C) 2005 Kevin Kofler
 *
 *  Portions lifted from libusb-win32 (LGPL):
 *  Copyright (c) 2002-2005 Stephan Meyer <ste_meyer@web.de>
 *  Copyright (c) 2000-2005 Johannes Erdfelt <johannes@erdfelt.com>
 *  Modifications for libticables Copyright (C) 2007 Kevin Kofler
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
   32/64 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
   This is transparent for the user because the libusb manages all these 
   things for us. Nethertheless, this fact has some consequences:

   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.

   - for reading, we have to read up to 32 bytes at a time (even if we need 
   only 1 byte) and to store them in a buffer for subsequent accesses. 
   In fact, if we try and get byte per byte, it will not work.

   - [ for writing, we don't store bytes in a buffer. It seems better to send
   data byte per byte (latency ?!). But, this make data-rate significantly 
   decrease (1KB/s instead of 5KB/s).
   Another way is to use partially buffered write operations: send consecutive
   blocks as a whole but partial block byte per byte. This is the best 
   compromise and it works fine ! ] => not needed any more

   - another particular effect (quirk): sometimes (usually when calc need to 
   reply and takes a while), a read call can returns with neither data 
   nor timeout. Simply retry a read call and it works fine. The best example 
   is to get IDLIST.

   - for checking, under Linux, we use a hack by Kevin Kofler (based on the
   libusb source code, and inspired by Romain's Windows driver). This is for
   Linux ONLY, not for other POSIX-like systems (it directly uses Linux kernel
   interfaces). Under Windows, we use the asynchronous API provided by the
   libusb-win32.
*/
                                     
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#ifndef __WIN32__
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#endif

#ifdef _MSC_VER
# include "../win32/usb.h"
#else
# include <usb.h>
#endif

/* --- */

#ifdef __WIN32__	// found in src/error.h of libusb-win32
# define ETIMEDOUT	116

# define usb_busses	usb_get_busses()

/* variables for slv_check and slv_bulk_read2 */
static int io_pending = 0;
static void *context = 0;
#endif

/* --- */

#ifdef __LINUX__
/* the libusb internal structure from usbi.h */
struct usb_dev_handle {
  int fd;

  struct usb_bus *bus;
  struct usb_device *device;

  int config;
  int interface;
  int altsetting;

  void *impl_info;
};

/* another pair of libusb internal structures from linux.h */
struct usb_iso_packet_desc {
	unsigned int length;
	unsigned int actual_length;
	unsigned int status;
};
struct usb_urb {
	unsigned char type;
	unsigned char endpoint;
	int status;
	unsigned int flags;
	void *buffer;
	int buffer_length;
	int actual_length;
	int start_frame;
	int number_of_packets;
	int error_count;
	unsigned int signr;
	void *usercontext;
	struct usb_iso_packet_desc iso_frame_desc[];
};

/* definitions from the libusb linux.h */
#define USB_URB_TYPE_BULK	3
#define IOCTL_USB_CONTROL	_IOWR('U', 0, struct usb_ctrltransfer)
#define IOCTL_USB_BULK		_IOWR('U', 2, struct usb_bulktransfer)
#define IOCTL_USB_RESETEP	_IOR('U', 3, unsigned int)
#define IOCTL_USB_SETINTF	_IOR('U', 4, struct usb_setinterface)
#define IOCTL_USB_SETCONFIG	_IOR('U', 5, unsigned int)
#define IOCTL_USB_GETDRIVER	_IOW('U', 8, struct usb_getdriver)
#define IOCTL_USB_SUBMITURB	_IOR('U', 10, struct usb_urb)
#define IOCTL_USB_DISCARDURB	_IO('U', 11)
#define IOCTL_USB_REAPURB	_IOW('U', 12, void *)
#define IOCTL_USB_REAPURBNDELAY	_IOW('U', 13, void *)
#define IOCTL_USB_CLAIMINTF	_IOR('U', 15, unsigned int)
#define IOCTL_USB_RELEASEINTF	_IOR('U', 16, unsigned int)
#define IOCTL_USB_CONNECTINFO	_IOW('U', 17, struct usb_connectinfo)
#define IOCTL_USB_IOCTL         _IOWR('U', 18, struct usb_ioctl)
#define IOCTL_USB_HUB_PORTINFO	_IOR('U', 19, struct usb_hub_portinfo)
#define IOCTL_USB_RESET		_IO('U', 20)
#define IOCTL_USB_CLEAR_HALT	_IOR('U', 21, unsigned int)
#define IOCTL_USB_DISCONNECT	_IO('U', 22)
#define IOCTL_USB_CONNECT	_IO('U', 23)

/* definitions to set the libusb error string from the libusb error.h */
typedef enum {
  USB_ERROR_TYPE_NONE = 0,
  USB_ERROR_TYPE_STRING,
  USB_ERROR_TYPE_ERRNO,
} usb_error_type_t;

extern char usb_error_str[1024];
extern int usb_error_errno;
extern usb_error_type_t usb_error_type;
extern int usb_debug;

#define USB_ERROR(x) \
	do { \
          usb_error_type = USB_ERROR_TYPE_ERRNO; \
          usb_error_errno = x; \
	  return x; \
	} while (0)

#define USB_ERROR_STR(x, format, args...) \
	do { \
	  usb_error_type = USB_ERROR_TYPE_STRING; \
	  snprintf(usb_error_str, sizeof(usb_error_str) - 1, format, ## args); \
          if (usb_debug >= 2) \
            fprintf(stderr, "USB error: %s\n", usb_error_str); \
	  return x; \
	} while (0)

/* variables for slv_check and slv_bulk_read2 */
static int io_pending = 0;
static struct usb_urb urb;
#endif

/* --- */

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#ifdef __WIN32__
#include "../win32/detect.h"
#else
#include "detect.h"
#endif
#include "../timeout.h"

/* Constants */

#define MAX_CABLES   4

#define VID_TIGLUSB  0x0451     /* Texas Instruments, Inc.            */

#define TIGL_BULK_IN    0x81    /* IN  endpoint */
#define TIGL_BULK_OUT   0x02    /* OUT endpoint */

#define to      (100 * h->timeout)        // in ms

/* Types */

// device infos
typedef struct
{
    uint16_t vid;
    uint16_t pid;
    char*    str;

    struct usb_device *dev;
} usb_infos;

// list of known devices
static usb_infos tigl_infos[] =
{
        {0x0451, 0xe001, "TI-GRAPH LINK USB", NULL},
		{0x0451, 0xe003, "TT-84 Plus Hand-Held", NULL},
        {0x0451, 0xe004, "TI-89 Titanium Hand-Held", NULL},
        {0x0451, 0xe008, "TI-84 Plus Silver Hand-Held", NULL},
		{0x0451, 0xe012, "TI-Nspire Hand-Held", NULL},
        { 0 }
};

// list of devices found 
static usb_infos tigl_devices[MAX_CABLES+1];

// internal structure for holding data
typedef struct
{
    struct usb_device *device;
    usb_dev_handle    *handle;

    int               nBytesRead;
    uint8_t           rBuf[64];
    uint8_t*          rBufPtr;
    
    int               max_ps;
} usb_struct;

// convenient macros
#define uDev       (((usb_struct *)(h->priv2))->device)
#define uHdl       (((usb_struct *)(h->priv2))->handle)
#define max_ps     (((usb_struct *)(h->priv2))->max_ps)
#define nBytesRead (((usb_struct *)(h->priv2))->nBytesRead)
#define rBuf       (((usb_struct *)(h->priv2))->rBuf)
#define rBufPtr    (((usb_struct *)(h->priv2))->rBufPtr)

/* Helpers (=driver API) */

static const char* tigl_get_product(struct usb_device *dev)
{
#if defined(__BSD__)
    /* The code below causes problems on FreeBSD (libusb bug?). */
    return "";
#else
    struct usb_dev_handle *han;
    int ret;
    static char string[64];

    if (dev->descriptor.iProduct)
    {
	han = usb_open(dev);
	ret = usb_get_string_simple(han, dev->descriptor.iProduct, 
				    string, sizeof(string));
	usb_close(han);
	if (ret > 0)
	    return string;
	else
	    return "";
    }

    return string;
#endif
}

static int tigl_find(void)
{
    struct usb_bus    *bus;
    struct usb_device *dev;
    int i, j;

    memset(tigl_devices, 0, sizeof(tigl_devices) / sizeof(usb_infos));
    j = 0;

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
			ticables_info(" found %s on #%i, version <%x.%02x>",
				      tigl_get_product(dev), j+1,
				      dev->descriptor.bcdDevice >> 8,
				      dev->descriptor.bcdDevice & 0xff);

			memcpy(&tigl_devices[j], &tigl_infos[i], 
			       sizeof(usb_infos));
			tigl_devices[j++].dev = dev;
		    }
		}
	    }
	}
    }

    return j;
}

static int tigl_enum(void)
{
    int ret = 0;
    
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
    
    /* find all TI products on all discovered busses/devices */
    ret = tigl_find();
    if(ret == 0)
    {
	ticables_warning(_("no devices found!\n"));
	return ERR_LIBUSB_OPEN;
    }

    return 0;
}    

static int tigl_open(int id, usb_dev_handle **udh)
{
    int ret; 

    TRYC(tigl_enum());

    if(tigl_devices[id].dev == NULL)
	return ERR_LIBUSB_OPEN;

    *udh = usb_open(tigl_devices[id].dev);
    if (*udh != NULL) 
    {
	/* only one configuration: #1 */
	ret = usb_set_configuration(*udh, 1);
        if (ret < 0)
        {
            //ticables_warning("usb_set_configuration (%s).\n",usb_strerror());
            //return ERR_LIBUSB_CONFIG;
        }

	/* configuration #1, interface #0 */
	ret = usb_claim_interface(*udh, 0);
	if (ret < 0) 
	{
	    ticables_warning("usb_claim_interface (%s).\n",
			     usb_strerror());
	    return ERR_LIBUSB_CLAIM;
	}

	return 0;
    } 
    else
	return ERR_LIBUSB_OPEN;

    return 0;
}

static int tigl_close(usb_dev_handle **udh)
{
    usb_release_interface(*udh, 0);
    usb_close(*udh);
    *udh = NULL;

    return 0;
}

static int tigl_reset(usb_dev_handle *udh)
{
	int ret;

  	// Reset out pipe
  	ret = usb_clear_halt(udh, TIGL_BULK_OUT);
  	if (ret < 0) 
	{
	    ticables_warning("usb_clear_halt (%s).\n", usb_strerror());
	    
	    ret = usb_resetep(udh, TIGL_BULK_OUT);
	    if (ret < 0) 
	    {
		ticables_warning("usb_resetep (%s).\n", usb_strerror());
		
		ret = usb_reset(udh);
		if (ret < 0) 
		{
		    ticables_warning("usb_reset (%s).\n", usb_strerror());
		    return ERR_LIBUSB_RESET;
		}
	    }
  	}
	
	// Reset in pipe
  	ret = usb_clear_halt(udh, TIGL_BULK_IN);
  	if (ret < 0) 
	{
	    ticables_warning("usb_clear_halt (%s).\n", usb_strerror());
	    
	    ret = usb_resetep(udh, TIGL_BULK_OUT);
	    if (ret < 0) 
	    {
		ticables_warning("usb_resetep (%s).\n", usb_strerror());
		
		ret = usb_reset(udh);
		if (ret < 0) 
		{
		    ticables_warning("usb_reset (%s).\n", usb_strerror());
		    return ERR_LIBUSB_RESET;
		}
	    }
  	}

	return 0;
}

/* API */

static int slv_prepare(CableHandle *h)
{
	char str[64];

#ifdef __WIN32__
	TRYC(win32_check_libusb());
#else
	TRYC(linux_check_libusb());
#endif

	if(h->port >= MAX_CABLES)
	    return ERR_ILLEGAL_ARG;

	h->address = h->port-1;
	sprintf(str, "TiglUsb #%i", h->port);
	h->device = strdup(str);
	h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));

	return 0;
}

static int slv_open(CableHandle *h)
{
    struct usb_config_descriptor *config;
    struct usb_interface *interface_;
    struct usb_interface_descriptor *interface;
    struct usb_endpoint_descriptor *endpoint;    

    // open device
    TRYC(tigl_open(h->address, &uHdl));
    uDev = tigl_devices[h->address].dev;    

    // get max packet size
    config = &(uDev->config[0]);
    interface_ = &(config->interface[0]);
    interface = &(interface_->altsetting[0]);
    endpoint = &(interface->endpoint[0]);
    max_ps = endpoint->wMaxPacketSize;
    //printf("max_ps = %i\n", max_ps);
    nBytesRead = 0;
    
#if !defined(__BSD__)
    /* Reset both endpoints */
    TRYC(tigl_reset(uHdl));
#endif

    return 0;
}

static int slv_close(CableHandle *h)
{
    if (uHdl != NULL) 
	tigl_close(&uHdl);
    uDev = NULL; 

    free(h->priv2);
    h->priv2 = NULL;
    
    return 0;
}

static int slv_reset(CableHandle *h)
{

    return 0;
}

// convenient function which send one or more bytes
static int send_block(CableHandle *h, uint8_t *data, int length)
{
    int ret;
    
    ret = usb_bulk_write(uHdl, TIGL_BULK_OUT, (char*)data, length, to);
    
    if(ret == -ETIMEDOUT) 
    {
	ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
	return ERR_WRITE_TIMEOUT;
    } 
    else if(ret == -EPIPE) 
    {
	ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
	return ERR_WRITE_ERROR;
    } 
    else if(ret < 0) 
    {
	ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
	return ERR_WRITE_ERROR;
    }

    return 0;
}

static int slv_put(CableHandle* h, uint8_t *data, uint32_t len)
{
    return send_block(h, data, len);
}

#ifdef __LINUX__
#define MAX_READ_WRITE  (16 * 1024)
static int slv_bulk_read2(usb_dev_handle *dev, int ep, char *bytes, int size,
	int timeout)
{
	// This is a variant of usb_bulk_read in libusb, edited to take the
	// io_pending variable set in slv_check into account.
	unsigned int bytesdone = 0, requested;
	struct timeval tv, tv_ref, tv_now;
	void *context;
	int ret, waiting;

	/*
	 * FIXME: The use of the URB interface is incorrect here if there are
	 * multiple callers at the same time. We assume we're the only caller
	 * and if we get completions from another caller, this code will fail
	 * in interesting ways.
	 */

	/*
	 * Get actual time, and add the timeout value. The result is the absolute
	 * time where we have to quit waiting for an message.
	 */
	gettimeofday(&tv_ref, NULL);
	tv_ref.tv_sec = tv_ref.tv_sec + timeout / 1000;
	tv_ref.tv_usec = tv_ref.tv_usec + (timeout % 1000) * 1000;

	if (tv_ref.tv_usec > 1000000) {
		tv_ref.tv_usec -= 1000000;
		tv_ref.tv_sec++;
	}

	do {
		fd_set writefds;

		requested = size - bytesdone;
		if (requested > MAX_READ_WRITE)
			requested = MAX_READ_WRITE;

		if (io_pending)
			io_pending = FALSE;
		else
		{
			urb.type = USB_URB_TYPE_BULK;
			urb.endpoint = ep;
			urb.flags = 0;
			urb.buffer = bytes + bytesdone;
			urb.buffer_length = requested;
			urb.usercontext = (void *)ep;
			urb.signr = 0;
			urb.actual_length = 0;
			urb.number_of_packets = 0;	/* don't do isochronous yet */

			ret = ioctl(dev->fd, IOCTL_USB_SUBMITURB, &urb);
			if (ret < 0) {
				USB_ERROR_STR(-errno, "error submitting URB: %s", strerror(errno));
				return ret;
			}
		}

		FD_ZERO(&writefds);
		FD_SET(dev->fd, &writefds);

		waiting = 1;
		while (((ret = ioctl(dev->fd, IOCTL_USB_REAPURBNDELAY, &context)) == -1) && waiting) {
			tv.tv_sec = 0;
			tv.tv_usec = 1000; // 1 msec
			select(dev->fd + 1, NULL, &writefds, NULL, &tv); //sub second wait

			/* compare with actual time, as the select timeout is not that precise */
			gettimeofday(&tv_now, NULL);

			if ((tv_now.tv_sec > tv_ref.tv_sec) ||
			    ((tv_now.tv_sec == tv_ref.tv_sec) && (tv_now.tv_usec >= tv_ref.tv_usec)))
				waiting = 0;
		}

		/*
		 * If there was an error, that wasn't EAGAIN (no completion), then
		 * something happened during the reaping and we should return that
		 * error now
		 */
		if (ret < 0 && errno != EAGAIN)
			USB_ERROR_STR(-errno, "error reaping URB: %s", strerror(errno));

		bytesdone += urb.actual_length;
	} while (ret == 0 && bytesdone < size && urb.actual_length == requested);

	/* If the URB didn't complete in success or error, then let's unlink it */
	if (ret < 0) {
		int rc;

		if (!waiting)
			rc = -ETIMEDOUT;
		else
			rc = urb.status;

		ret = ioctl(dev->fd, IOCTL_USB_DISCARDURB, &urb);
		if (ret < 0 && errno != EINVAL && usb_debug >= 1)
			fprintf(stderr, "error discarding URB: %s", strerror(errno));

		/*
		 * When the URB is unlinked, it gets moved to the completed list and
		 * then we need to reap it or else the next time we call this function,
		 * we'll get the previous completion and exit early
		 */
		ioctl(dev->fd, IOCTL_USB_REAPURB, &context);

		return rc;
	}

	return bytesdone;
}
#endif

#ifdef _WIN32
#define LIBUSB_MAX_READ_WRITE 0x10000
int slv_bulk_read2(usb_dev_handle *dev, int ep, char *bytes, int size,
                   int timeout)
{
  // This is a variant of usb_bulk_read in libusb-win32, edited to take the
  // io_pending variable set in slv_check into account and to use the public
  // async API instead of the private one.

  int transmitted = 0;
  int ret;
  int requested;

  if (!io_pending)
    {
      ret = usb_bulk_setup_async(dev, &context, ep);

      if(ret < 0)
        {
          return ret;
        }
    }

  do {
    requested = size > LIBUSB_MAX_READ_WRITE ? LIBUSB_MAX_READ_WRITE : size;

    if (io_pending)
      io_pending = FALSE;
    else
      {
        ret = usb_submit_async(context, bytes, requested);

        if(ret < 0)
          {
            transmitted = ret;
            break;
          }
      }

    ret = usb_reap_async(context, timeout);

    if(ret < 0)
      {
        transmitted = ret;
        break;
      }

    transmitted += ret;
    bytes += ret;
    size -= ret;
  } while(size > 0 && ret == requested);
  
  usb_free_async(&context);

  return transmitted;
}
#endif

static int slv_get_(CableHandle *h, uint8_t *data)
{
    int ret = 0;
    tiTIME clk;

    /* Read up to 32/64 bytes and store them in a buffer for 
       subsequent accesses */
    if (nBytesRead <= 0) 
    {
	TO_START(clk);
	do 
	{
#if defined(__LINUX__) || defined(__WIN32__)
	    ret = slv_bulk_read2(uHdl, TIGL_BULK_IN, (char*)rBuf, 
				max_ps, to);
#else
	    ret = usb_bulk_read(uHdl, TIGL_BULK_IN, (char*)rBuf, 
				max_ps, to);
#endif

	    if (TO_ELAPSED(clk, h->timeout))
	    {
		nBytesRead = 0;
		return ERR_READ_TIMEOUT;
	    }
/*
	    if (ret == 0)
		ticables_warning("\nweird, usb_bulk_read returns without any data & error; retrying...\n");
*/
	}
	while(!ret);
	
	if(ret == -ETIMEDOUT) 
	{
	    ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
	    nBytesRead = 0;
	    return ERR_READ_TIMEOUT;
	} 
	else if(ret == -EPIPE) 
	{
	    ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
	    nBytesRead = 0;
	    return ERR_READ_ERROR;
	} 
	else if(ret < 0) 
	{
	    ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
	    nBytesRead = 0;
	    return ERR_READ_ERROR;
	}

	nBytesRead = ret;
	rBufPtr = rBuf;
    }

    *data = *rBufPtr++;
    nBytesRead--;

    return 0;
}

static int slv_get(CableHandle* h, uint8_t *data, uint32_t len)
{
    int i;

    // we can't do that in any other way because slv_get_ can returns
    // 1, 2, ..., len bytes.
    for(i = 0; i < (int)len; i++)
	TRYC(slv_get_(h, data+i));
    
    return 0;
}

static int slv_probe(CableHandle *h)
{
    int i;
    
    TRYC(tigl_enum());

    for(i = 0; i < MAX_CABLES; i++)
    {
	if(tigl_devices[h->address].pid == PID_TIGLUSB)
	    return 0;
    }
    
    return ERR_PROBE_FAILED;
}

static int usb_probe(CableHandle *h)
{
    int i;

    TRYC(tigl_enum());

    for(i = 0; i < MAX_CABLES; i++)
    {
		if(tigl_devices[h->address].pid == PID_TIGLUSB   ||
			tigl_devices[h->address].pid == PID_TI89TM   ||
			tigl_devices[h->address].pid == PID_TI84P    ||
			tigl_devices[h->address].pid == PID_TI84P_SE ||
			tigl_devices[h->address].pid == PID_NSPIRE) 
	    return 0;
    }    

    return ERR_PROBE_FAILED;
}

static int slv_check(CableHandle *h, int *status)
{
#if defined(__LINUX__)
    // This really should be in libusb, but alas it isn't, so their code was
    // adapted by Kevin Kofler for use here. It's required to get TiEmu 3 to
    // work with the SilverLink.

	void *context;
	int ret;

	if(nBytesRead > 0)
        {
	    *status = !0;
	    return 0;
        }

	if (!io_pending)
	{
		urb.type = USB_URB_TYPE_BULK;
		urb.endpoint = TIGL_BULK_IN;
		urb.flags = 0;
		urb.buffer = (char*)rBuf;
		urb.buffer_length = max_ps;
		urb.usercontext = (void *)TIGL_BULK_IN;
		urb.signr = 0;
		urb.actual_length = 0;
		urb.number_of_packets = 0;

		ret = ioctl(uHdl->fd, IOCTL_USB_SUBMITURB, &urb);
		if (ret < 0)
			return ERR_READ_ERROR;
		io_pending = TRUE;
	}

	ret = ioctl(uHdl->fd, IOCTL_USB_REAPURBNDELAY, &context);
	if (ret < 0 && errno != EAGAIN)
	{
		// Error, unlink URB and return failure.
		ioctl(uHdl->fd, IOCTL_USB_DISCARDURB, &urb);

		/*
		 * When the URB is unlinked, it gets moved to the completed list and
		 * then we need to reap it or else the next time we call this function,
		 * we'll get the previous completion and exit early
		 */
		ioctl(uHdl->fd, IOCTL_USB_REAPURB, &context);

		io_pending = FALSE;
		return ERR_READ_ERROR;
	}

	if (ret >= 0)
	{
		io_pending = FALSE;
		if (urb.actual_length > 0)
		{
			nBytesRead = urb.actual_length;
			rBufPtr = rBuf;
			*status = STATUS_RX; // data available
		}
	}
	return 0;
#elif defined(__WIN32__)
	int ret;

	if(nBytesRead > 0)
        {
	    *status = !0;
	    return 0;
        }

	if (!io_pending)
	{
		ret = usb_bulk_setup_async(uHdl, &context, TIGL_BULK_IN);
		if (ret < 0)
			return ERR_READ_ERROR;
		ret = usb_submit_async(context, (char*)rBuf, max_ps);
		if (ret < 0)
		{
			usb_free_async(&context);
			return ERR_READ_ERROR;
		}
		io_pending = TRUE;
	}

	ret = usb_reap_async_nocancel(context, 0);
	if (ret < 0 && ret != -ETIMEDOUT)
	{
		// Error, unlink URB and return failure.
		usb_cancel_async(context);
		usb_free_async(&context);
		io_pending = FALSE;
		return ERR_READ_ERROR;
	}

	if (ret >= 0)
	{
		usb_free_async(&context);
		io_pending = FALSE;
		if (ret > 0)
		{
			nBytesRead = ret;
			rBufPtr = rBuf;
			*status = STATUS_RX; // data available
		}
	}
	return 0;
#else
	// no way to check yet
	return 0;
#endif
}

static int slv_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

static int slv_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

static int slv_get_red_wire(CableHandle *h)
{
	return 1;
}

static int slv_get_white_wire(CableHandle *h)
{
	return 1;
}

const CableFncts cable_slv =
{
	CABLE_SLV,
	"SLV",
	N_("SilverLink"),
	N_("SilverLink (TI-GRAPH LINK USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &slv_probe, NULL,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
};

const CableFncts cable_usb =
{
	CABLE_USB,
	"USB",
	N_("UsbLink"),
	N_("UsbLink cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &usb_probe, NULL,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
};

//=======================

TIEXPORT1 int TICALL usb_probe_devices1(int **list);
#ifdef HAVE_LINUX_TICABLE_H
TIEXPORT1 int TICALL usb_probe_devices2(int **list);
#endif

TIEXPORT1 int TICALL usb_probe_devices(int **list)
{
#ifdef HAVE_LINUX_TICABLE_H
  int fd;

  fd = open("/dev/tiusb0", O_NONBLOCK);
  close (fd);
  //printf("fd = %i, errno = %i\n", fd, errno);
  //perror(strerror(errno));

  if((fd == -1) && (errno != EBUSY))
    return usb_probe_devices1(list);
  else
    return usb_probe_devices2(list);
#else
  return usb_probe_devices1(list);
#endif
}

// returns number of devices and list of PIDs (dynamically allocated)
TIEXPORT1 int TICALL usb_probe_devices1(int **list)
{
	int i;

    TRYC(tigl_enum());

    *list = (int *)calloc(MAX_CABLES+1, sizeof(int));
    for(i = 0; i < MAX_CABLES; i++)
        (*list)[i] = tigl_devices[i].pid;

    return 0;
}
