/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (c) 1999-2006 Romain Lievin
 *  Copyright (c) 2001 Julien Blache (original author)
 *  Copyright (c) 2007 Romain Lievin (libusb-win32 support)
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* TI-GRAPH LINK USB and direct USB cable support (libusb 0.1.x) */

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   
   This link cable use Bulk mode with packets. The max size of a packet is 
   32/64 bytes on most models (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
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

  Moreover,

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
#include <stdlib.h>
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

#include "../ticables.h"
#include "../internal.h"
#ifdef _MSC_VER
# include "../win32/usb.h"
#else
# include <usb.h>
#endif

/* --- */

#ifdef __WIN32__	// found in src/error.h of libusb-win32
#ifndef ETIMEDOUT
# define ETIMEDOUT	116
#endif

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
		{ \
			fprintf(stderr, "USB error: %s\n", usb_error_str); \
		} \
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
#if defined(__WIN32__)
#include "../win32/detect.h"
#elif defined(__MACOSX__)
#include "../macos/detect.h"
#elif defined(__BSD__)
#include "../bsd/detect.h"
#else
#include "detect.h"
#endif
#include "../timeout.h"
#include "../internal.h"

/* Constants */

#define MAX_CABLES   4

#define VID_TI  0x0451     /* Texas Instruments, Inc.            */

#define to      (100 * h->timeout)        // in ms

/* Types */

// device infos
typedef struct
{
	uint16_t    vid;
	uint16_t    pid;
	const char* str;

	struct usb_device *dev;
} usb_infos;

// list of known devices
static const usb_infos tigl_infos[] =
{
	{VID_TI, PID_TIGLUSB,       "TI-GRAPH LINK USB",           NULL},
	{VID_TI, PID_TI84P,         "TI-84 Plus Hand-Held",        NULL},
	{VID_TI, PID_TI89TM,        "TI-89 Titanium Hand-Held",    NULL},
	{VID_TI, PID_TI84P_SE,      "TI-84 Plus Silver Hand-Held", NULL},
	{VID_TI, PID_NSPIRE,        "TI-Nspire Hand-Held",         NULL},
	{VID_TI, PID_NSPIRE_CRADLE, "TI-Nspire Cradle",            NULL},
	{VID_TI, PID_NSPIRE_CXII,   "TI-Nspire CX II Hand-Held",   NULL},
	{0,      0,                 NULL,                          NULL}
};

// list of devices found 
static USBCableInfo tigl_devices[MAX_CABLES+1];
static int tigl_n_devices;

// internal structure for holding data
typedef struct
{
	struct usb_device *device;
	usb_dev_handle    *handle;

	USBCableInfo      cable_info;
	int               nBytesRead;
	uint8_t           rBuf[512];
	uint8_t*          rBufPtr;
	int               in_endpoint;
	int               out_endpoint;
	int               max_ps;
	int               was_max_size_packet;
} usb_struct;

// convenient macros
#define uDev                (((usb_struct *)(h->priv2))->device)
#define uHdl                (((usb_struct *)(h->priv2))->handle)
#define cable_info          (((usb_struct *)(h->priv2))->cable_info)
#define max_ps              (((usb_struct *)(h->priv2))->max_ps)
#define was_max_size_packet (((usb_struct *)(h->priv2))->was_max_size_packet)
#define nBytesRead          (((usb_struct *)(h->priv2))->nBytesRead)
#define rBuf                (((usb_struct *)(h->priv2))->rBuf)
#define rBufPtr             (((usb_struct *)(h->priv2))->rBufPtr)
#define uInEnd              (((usb_struct *)(h->priv2))->in_endpoint)
#define uOutEnd             (((usb_struct *)(h->priv2))->out_endpoint)

/* Helpers (=driver API) */

static void tigl_get_product(char * string, size_t maxlen, struct usb_device *dev)
{
/* The code below causes problems on FreeBSD (libusb bug?). */
#ifndef __BSD__
	struct usb_dev_handle *han;
	int ret;

	string[0] = 0;

	if (dev->descriptor.iProduct)
	{
		han = usb_open(dev);
		ret = usb_get_string_simple(han, dev->descriptor.iProduct, string, maxlen);
		usb_close(han);
	}
#endif
}

static int tigl_find(void)
{
	struct usb_bus    *bus;
	struct usb_device *dev;
	int i, j;

	memset(tigl_devices, 0, sizeof(tigl_devices));
	j = tigl_n_devices = 0;

	/* loop taken from testlibusb.c */
	for (bus = usb_busses; bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			if (dev->descriptor.idVendor == VID_TI)
			{
				for(i = 0; i < (int)(sizeof(tigl_infos) / sizeof(tigl_infos[0])); i++)
				{
					if (dev->descriptor.idProduct == tigl_infos[i].pid)
					{
						tigl_devices[j].vid = dev->descriptor.idVendor;
						tigl_devices[j].pid = dev->descriptor.idProduct;
						tigl_devices[j].version = dev->descriptor.bcdDevice;

						tigl_get_product(tigl_devices[j].product_str, sizeof(tigl_devices[j].product_str), dev);
						ticables_info(" found %s on #%i, version <%x.%02x>",
						tigl_devices[j].product_str, j+1,
						dev->descriptor.bcdDevice >> 8,
						dev->descriptor.bcdDevice & 0xff);

						tigl_devices[j++].dev = dev;
						tigl_n_devices = j;

						if (j >= MAX_CABLES)
						{
							return j;
						}
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
	if (ret == 0)
	{
		ticables_warning("%s", _("no devices found!\n"));
		return ERR_LIBUSB_OPEN;
	}

	return 0;
}

static int tigl_open(int id, usb_dev_handle **udh)
{
	int ret;

	ret = tigl_enum();
	if (ret)
	{
		return ret;
	}

	if (tigl_devices[id].dev == NULL)
	{
		return ERR_LIBUSB_OPEN;
	}

	*udh = usb_open((struct usb_device *)(tigl_devices[id].dev));
	if (*udh != NULL) 
	{
		/*
		 * Most models have a single configuration: #1.
		 * On the Nspire CX II, until NNSE support is implemented, use configuration #2.
		 */
		int configuration = tigl_devices[id].pid == PID_NSPIRE_CXII ? 2 : 1;
		ret = usb_set_configuration(*udh, configuration);
		if (ret < 0)
		{
			ticables_warning("usb_set_configuration (%s).\n", usb_strerror());
		}

		/* Interface #0 for the selected configuration. */
		ret = usb_claim_interface(*udh, 0);
		if (ret < 0) 
		{
			ticables_warning("usb_claim_interface (%s).\n", usb_strerror());
			return ERR_LIBUSB_CLAIM;
		}

		return 0;
	}
	else
	{
		return ERR_LIBUSB_OPEN;
	}

	return 0;
}

static int tigl_close(usb_dev_handle **udh)
{
	// NOTE: slv_close() has already checked for *udh != NULL .
	usb_release_interface(*udh, 0);
	usb_close(*udh);
	*udh = NULL;

	return 0;
}

static int tigl_reset(CableHandle *h)
{
	// Reset out pipe
	if (NULL != uHdl)
	{
		int ret = usb_clear_halt(uHdl, uOutEnd);
		if (ret < 0)
		{
			ticables_warning("usb_clear_halt of out pipe (%s).\n", usb_strerror());
		}

		// Reset in pipe
		ret = usb_clear_halt(uHdl, uInEnd);
		if (ret < 0)
		{
			ticables_warning("usb_clear_halt of in pipe (%s).\n", usb_strerror());
		}

		return 0;
	}
	else
	{
		return ERR_LIBUSB_RESET;
	}
}

/* API */
static int slv_prepare(CableHandle *h)
{
	int ret;
	char str[64];

#if defined(__WIN32__)
	ret = win32_check_libusb();
#elif defined(__MACOSX__)
	ret = macosx_check_libusb();
#elif defined(__BSD__)
	ret = bsd_check_libusb();
#else
	ret = linux_check_libusb();
#endif
	if (!ret)
	{
		if (h->port >= MAX_CABLES)
		{
			return ERR_ILLEGAL_ARG;
		}

		h->address = h->port-1;
		sprintf(str, "TiglUsb #%i", h->port);
		h->device = strdup(str);
		h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));
	}

	return ret;
}

static int slv_open(CableHandle *h)
{
	int ret;
	int i;
	struct usb_config_descriptor *config;
	struct usb_interface *interface_;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;    

	// open device
	ret = tigl_open(h->address, &uHdl);
	if (ret)
	{
		return ret;
	}
	cable_info = tigl_devices[h->address];
	uDev = (struct usb_device *)(tigl_devices[h->address].dev);
	uInEnd  = 0x81;
	uOutEnd = 0x02;

	// get max packet size
	config = &(uDev->config[0]);
	interface_ = &(config->interface[0]);
	interface = &(interface_->altsetting[0]);
	endpoint = &(interface->endpoint[0]);
	max_ps = endpoint->wMaxPacketSize;
	if (max_ps > sizeof(rBuf))
	{
		ticables_critical("Reducing max packet size to maximum supported by library, expect communication issues");
		max_ps = sizeof(rBuf);
	}
	// Enumerate endpoints.
	for (i = 0; i < interface->bNumEndpoints; i++)
	{
		if ((endpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_BULK)
		{
			if (endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)
			{
				if (endpoint->bEndpointAddress != 0x83) // Some Nspire OS use that seemingly bogus endpoint.
				{
					uInEnd = endpoint->bEndpointAddress;
					ticables_info("found bulk in endpoint 0x%02X\n", uInEnd);
				}
				else
				{
					ticables_info("XXX: swallowing bulk in endpoint 0x83, advertised by Nspire (CAS and non-CAS) 1.x but seemingly not working\n");
				}
			}
			else
			{
				uOutEnd = endpoint->bEndpointAddress;
				ticables_info("found bulk out endpoint 0x%02X\n", uOutEnd);
			}
		}
		endpoint++;
	}
	nBytesRead = 0;
	was_max_size_packet = 0;

	return 0;
}

static int slv_close(CableHandle *h)
{
	if (uHdl != NULL) 
	{
		tigl_close(&uHdl);
	}
	uDev = NULL; 

	free(h->priv2);
	h->priv2 = NULL;

	return 0;
}

static int slv_get_device_info(CableHandle *h, CableDeviceInfo *info)
{
	translate_usb_device_info(info, &cable_info);
	return 0;
}

static int slv_reset(CableHandle *h)
{
	int ret = 0;

#if !defined(__BSD__)
	/* Reset both endpoints (send an URB_FUNCTION_RESET_PIPE) */
	ret = tigl_reset(h);
	if (!ret)
	{
		/* NOTE: tigl_reset() has already checked for uHdl != NULL */
#ifdef __WIN32__
		/* Reset USB port (send an IOCTL_INTERNAL_USB_RESET_PORT) */
		ret = usb_reset_ex(uHdl, USB_RESET_TYPE_RESET_PORT);
#else
		ret = usb_reset(uHdl);
#endif

		if (ret < 0)
		{
			ticables_warning("usb_reset (%s).\n", usb_strerror());
			/* On Mac OS X, reenumeration isn't automatic, so let's not return here. */
#ifndef __MACOSX__
			ret = ERR_LIBUSB_RESET;
#else
			ret = 0;
#endif
		}

		if (!ret)
		{
			// lib-usb doc: after calling usb_reset, the device will need to re-enumerate
			// and therefore, requires you to find the new device and open a new handle.
			// The handle used to call usb_reset will no longer work.
#ifdef __WIN32__
			Sleep(500);
#else
			usleep(500000);
#endif
			ret = slv_close(h);
			if (!ret)
			{
				h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));
				ret = slv_open(h);
			}
		}
	}
#endif

	return ret;
}

// convenient function which send one or more bytes
static int send_block(CableHandle *h, uint8_t *data, int length)
{
	int ret;

	if (NULL == uHdl)
	{
		return ERR_WRITE_ERROR;
	}

	ret = usb_bulk_write(uHdl, uOutEnd, (char*)data, length, to);

	if (ret == -ETIMEDOUT) 
	{
		ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
		return ERR_WRITE_TIMEOUT;
	}
	else if (ret == -EPIPE) 
	{
		ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
		return ERR_WRITE_ERROR;
	}
	else if (ret < 0) 
	{
		ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
		return ERR_WRITE_ERROR;
	}

	// FIXME do Nspire CX II calculators also need this ?
	if ((tigl_devices[h->address].pid == PID_NSPIRE || tigl_devices[h->address].pid == PID_NSPIRE_CRADLE) && length % max_ps == 0)
	{
		ticables_info("XXX triggering an extra bulk write");
		ret = usb_bulk_write(uHdl, uOutEnd, (char*)data, 0, to);
		if (ret < 0)
		{
			ticables_warning("usb_bulk_write (%s).\n", usb_strerror());
			return ERR_WRITE_ERROR;
		}
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
		{
			requested = MAX_READ_WRITE;
		}

		if (io_pending)
		{
			io_pending = FALSE;
		}
		else
		{
			urb.type = USB_URB_TYPE_BULK;
			urb.endpoint = ep;
			urb.flags = 0;
			urb.buffer = bytes + bytesdone;
			urb.buffer_length = requested;
			urb.usercontext = (void *)(intptr_t)ep;
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
			{
				waiting = 0;
			}
		}

		/*
		 * If there was an error, that wasn't EAGAIN (no completion), then
		 * something happened during the reaping and we should return that
		 * error now
		 */
		if (ret < 0 && errno != EAGAIN)
		{
			USB_ERROR_STR(-errno, "error reaping URB: %s", strerror(errno));
		}

		bytesdone += urb.actual_length;
	} while (ret == 0 && bytesdone < size && urb.actual_length == requested);

	/* If the URB didn't complete in success or error, then let's unlink it */
	if (ret < 0) {
		int rc;

		if (!waiting)
		{
			rc = -ETIMEDOUT;
		}
		else
		{
			rc = urb.status;
		}

		ret = ioctl(dev->fd, IOCTL_USB_DISCARDURB, &urb);
		if (ret < 0 && errno != EINVAL && usb_debug >= 1)
		{
			fprintf(stderr, "error discarding URB: %s", strerror(errno));
		}

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
static int slv_bulk_read2(usb_dev_handle *dev, int ep, char *bytes, int size,
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
		ret = usb_bulk_setup_async(dev, &context, (unsigned char)ep);

		if (ret < 0)
		{
			return ret;
		}
	}

	do
	{
		requested = size > LIBUSB_MAX_READ_WRITE ? LIBUSB_MAX_READ_WRITE : size;

		if (io_pending)
		{
			io_pending = FALSE;
		}
		else
		{
			ret = usb_submit_async(context, bytes, requested);

			if (ret < 0)
			{
				transmitted = ret;
				break;
			}
		}

		ret = usb_reap_async(context, timeout);

		if (ret < 0)
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

	/* Read up to max_ps bytes and store them in a buffer for subsequent accesses */
	if (nBytesRead <= 0) 
	{
		TO_START(clk);
		do
		{
			// NOTE: slv_get() has already checked for uHdl != NULL .
#if defined(__LINUX__) || defined(__WIN32__)
			ret = slv_bulk_read2(uHdl, uInEnd, (char*)rBuf, max_ps, to);
#else
			ret = usb_bulk_read(uHdl, uInEnd, (char*)rBuf, max_ps, to);
#endif

			if (ret == max_ps)
			{
				was_max_size_packet = 1;
			}
			else
			{
				was_max_size_packet = 0;
			}

			if (TO_ELAPSED(clk, h->timeout))
			{
				nBytesRead = 0;
				return ERR_READ_TIMEOUT;
			}
/*
			if (ret == 0)
			{
				ticables_warning("\nweird, usb_bulk_read returns without any data & error; retrying...\n");
			}
*/
		}
		while(!ret);

		if (ret == -ETIMEDOUT) 
		{
			ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
			nBytesRead = 0;
			return ERR_READ_TIMEOUT;
		} 
		else if (ret == -EPIPE) 
		{
			ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
			nBytesRead = 0;
			return ERR_READ_ERROR;
		} 
		else if (ret < 0) 
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
	int i, ret;

	if (NULL == uHdl)
	{
		return ERR_READ_ERROR;
	}

	// we can't do that in any other way because slv_get_ can returns
	// 1, 2, ..., len bytes.
	for (i = 0; i < (int)len; i++)
	{
		ret = slv_get_(h, data+i);
		if (ret)
		{
			break;
		}
	}

	// FIXME do Nspire CX II calculators also need this ?
	if (!ret &&   ((tigl_devices[h->address].pid == PID_NSPIRE || tigl_devices[h->address].pid == PID_NSPIRE_CRADLE) && was_max_size_packet != 0 && nBytesRead == 0)
	           || (len == 0 && (   (tigl_devices[h->address].pid == PID_TI89TM   && was_max_size_packet != 0 && nBytesRead == 0)
			            || (tigl_devices[h->address].pid == PID_TI84P    && was_max_size_packet != 0 && nBytesRead == 0)
			            || (tigl_devices[h->address].pid == PID_TI84P_SE && was_max_size_packet != 0 && nBytesRead == 0)
			           )
	              )
	   )
	{
		ticables_info("XXX triggering an extra bulk read");
#if defined(__LINUX__) || defined(__WIN32__)
		ret = slv_bulk_read2(uHdl, uInEnd, (char*)rBuf, max_ps, to);
#else
		ret = usb_bulk_read(uHdl, uInEnd, (char*)rBuf, max_ps, to);
#endif
		if (ret < 0) {
			ticables_warning("usb_bulk_read (%s).\n", usb_strerror());
			return ERR_READ_ERROR;
		}
	}

	return ret;
}

static int slv_probe(CableHandle *h)
{
	int ret;
	int i;

	ret = tigl_enum();
	if (ret)
	{
		return ret;
	}

	for(i = 0; i < MAX_CABLES; i++)
	{
		if (tigl_devices[h->address].pid == PID_TIGLUSB)
		{
			return 0;
		}
	}

	return ERR_PROBE_FAILED;
}

static int raw_probe(CableHandle *h)
{
	int ret;
	int i;

	ret = tigl_enum();
	if (ret)
	{
		return ret;
	}

	for(i = 0; i < MAX_CABLES; i++)
	{
		if (tigl_devices[h->address].pid == PID_TI89TM ||
		    tigl_devices[h->address].pid == PID_TI84P ||
		    tigl_devices[h->address].pid == PID_TI84P_SE ||
		    tigl_devices[h->address].pid == PID_NSPIRE ||
		    tigl_devices[h->address].pid == PID_NSPIRE_CRADLE ||
		    tigl_devices[h->address].pid == PID_NSPIRE_CXII)
		{
			return 0;
		}
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

	if (nBytesRead > 0)
	{
		*status = !0;
		return 0;
	}

	if (NULL == uHdl)
	{
		return ERR_READ_ERROR;
	}

	if (!io_pending)
	{
		urb.type = USB_URB_TYPE_BULK;
		urb.endpoint = uInEnd;
		urb.flags = 0;
		urb.buffer = (char*)rBuf;
		urb.buffer_length = max_ps;
		urb.usercontext = (void *)(intptr_t)uInEnd;
		urb.signr = 0;
		urb.actual_length = 0;
		urb.number_of_packets = 0;

		ret = ioctl(uHdl->fd, IOCTL_USB_SUBMITURB, &urb);
		if (ret < 0)
		{
			return ERR_READ_ERROR;
		}
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

	if (nBytesRead > 0)
	{
		*status = !0;
		return 0;
	}

	if (!io_pending)
	{
		ret = usb_bulk_setup_async(uHdl, &context, uInEnd);
		if (ret < 0)
		{
			return ERR_READ_ERROR;
		}
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

extern const CableFncts cable_slv =
{
	CABLE_SLV,
	"SLV",
	N_("SilverLink"),
	N_("SilverLink (TI-GRAPH LINK USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &slv_probe, NULL,
	&slv_put, &slv_get, &slv_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&noop_set_device,
	&slv_get_device_info
};

extern const CableFncts cable_raw =
{
	CABLE_USB,
	"USB",
	N_("DirectLink"),
	N_("DirectLink (DIRECT USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &raw_probe, NULL,
	&slv_put, &slv_get, &slv_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&noop_set_device,
	&slv_get_device_info
};

//=======================

// returns list of detected devices
int usb_probe_device_info(const USBCableInfo **list, int *count)
{
	int ret;
	if (!(ret = tigl_enum()))
	{
		*list = tigl_devices;
		*count = tigl_n_devices;
	}
	else
	{
		*list = NULL;
		*count = 0;
	}
	return ret;
}
