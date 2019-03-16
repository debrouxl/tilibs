/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (c) 1999-2006 Romain Lievin
 *  Copyright (c) 2001 Julien Blache (original author)
 *  Copyright (c) 2007 Romain Lievin (libusb-win32 support)
 *  Copyright (c) 2007, 2011 Kevin Kofler (slv_check support)
 *  Copyright (c) 2011 Jon Sturm (libusb-1.0 support)
 *  Copyright (c) 2011 Lionel Debroux (style fixes, corner case fixes)
 *
 *  Portions lifted from libusb (LGPL):
 *  Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 *  Copyright (C) 2001 Johannes Erdfelt <johannes@erdfelt.com>
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
 *  Foundation, Inc., 51 Franklin Street, Suite 500 Boston, MA 02110-1335 USA.
 */

/* TI-GRAPH LINK USB and direct USB cable support (libusb 1.0.x) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#if defined(__BSD__) || defined(__MACOSX__)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#ifdef __WIN32__
#ifndef _WINSOCKAPI_
#include <winsock2.h> /* struct timeval */
#endif
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#endif


#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "../internal.h"
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

/* Constants */

#define MAX_CABLES   4

#define VID_TI       0x0451     /* Texas Instruments, Inc.            */

#define to           (100 * h->timeout)        // in ms

/* Types */

// device infos
typedef struct
{
	uint16_t    vid;
	uint16_t    pid;
	const char* str;

	struct libusb_device *dev;
} usb_infos;

// list of known devices
static usb_infos tigl_infos[] =
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
	struct libusb_device *device;
	struct libusb_device_handle *handle;

	USBCableInfo cable_info;
	int      nBytesRead;
	uint8_t  rBuf[512];
	uint8_t* rBufPtr;
	int      in_endpoint;
	int      out_endpoint;
	int      max_ps;
	int      was_max_ps;
} usb_struct;

// variables for slv_check and slv_bulk_read
static int io_pending = 0;
static struct libusb_transfer *transfer;
static int completed = 0;

// convenient macros
#define uDev       (((usb_struct *)(h->priv2))->device)
#define uHdl       (((usb_struct *)(h->priv2))->handle)
#define cable_info (((usb_struct *)(h->priv2))->cable_info)
#define max_ps     (((usb_struct *)(h->priv2))->max_ps)
#define was_max_ps (((usb_struct *)(h->priv2))->was_max_ps)
#define nBytesRead (((usb_struct *)(h->priv2))->nBytesRead)
#define rBuf       (((usb_struct *)(h->priv2))->rBuf)
#define rBufPtr    (((usb_struct *)(h->priv2))->rBufPtr)
#define uInEnd     (((usb_struct *)(h->priv2))->in_endpoint)
#define uOutEnd    (((usb_struct *)(h->priv2))->out_endpoint)

#if !HAVE_LIBUSB10_STRERROR
#error Please use a version of libusb 1.0 which provides libusb_strerror() (>= 1.0.16).
#endif

static void tigl_get_product(char * string, size_t maxlen, struct libusb_device *dev)
{
	libusb_device_handle *han;
	libusb_error ret;
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);

	string[0] = 0;

	if (r < 0)
	{
		ticables_critical("failed to get device descriptor");
	}

	if (desc.iProduct)
	{
		if (!libusb_open(dev, &han))
		{
			ret = (libusb_error)libusb_get_string_descriptor_ascii(han, desc.iProduct, (unsigned char *) string, maxlen);
			libusb_close(han);
			if (ret <= 0)
			{
				ticables_warning("libusb_get_string_descriptor_ascii (%s).\n", libusb_strerror(ret));
			}
		}
		// else do nothing.
	}
}

static int tigl_find(void)
{
	// discover devices
	libusb_device **list;
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	ssize_t i = 0;
	int j = 0;
	int k;

	if (cnt <= 0)
	{
		return 0;
	}

	memset(tigl_devices, 0, sizeof(tigl_devices));
	tigl_n_devices = 0;

	for (i = 0; i < cnt; i++)
	{
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(device, &desc);
		if (r < 0)
		{
			fprintf(stderr, "failed to get device descriptor");
			return r;
		}
		if (desc.idVendor == VID_TI)
		{
			for(k = 0; k < (int)(sizeof(tigl_infos) / sizeof(tigl_infos[0])); k++)
			{
				if (desc.idProduct == tigl_infos[k].pid)
				{
					tigl_devices[j].vid = desc.idVendor;
					tigl_devices[j].pid = desc.idProduct;
					tigl_devices[j].version = desc.bcdDevice;
					tigl_get_product(tigl_devices[j].product_str, sizeof(tigl_devices[j].product_str), device);
					ticables_info(_(" found %s on #%i, version <%x.%02x>\n"),
						      tigl_devices[j].product_str, j+1,
						      desc.bcdDevice >> 8,
						      desc.bcdDevice & 0xff);

					tigl_devices[j++].dev = device;
					tigl_n_devices = j;

					if (j >= MAX_CABLES)
					{
						return j;
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

	/* find all TI products on all ports */
	ret = tigl_find();
	if (ret == 0)
	{
		ticables_warning("%s", _("no devices found!\n"));
		return ERR_LIBUSB_OPEN;
	}

	return 0;
}

static int tigl_open(int id, libusb_device_handle ** udh)
{
	int ret;

	tigl_enum();

	if (tigl_devices[id].dev == NULL)
	{
		return ERR_LIBUSB_OPEN;
	}

	if (!libusb_open((libusb_device *)(tigl_devices[id].dev), udh))
	{
		/*
		 * Most models have a single configuration: #1.
		 * On the Nspire CX II, until NNSE support is implemented, use configuration #2.
		 */
		int configuration = tigl_devices[id].pid == PID_NSPIRE_CXII ? 2 : 1;
		ret = libusb_set_configuration(*udh, configuration);
		if (ret)
		{
			ticables_warning("libusb_set_configuration (%s).\n", libusb_strerror((libusb_error)ret));
		}

		/* Interface #0 for the selected configuration. */
		ret = libusb_claim_interface(*udh, 0);
		if (ret)
		{
			ticables_warning("libusb_claim_interface (%s).\n", libusb_strerror((libusb_error)ret));
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

static int tigl_close(libusb_device_handle **udh)
{
	// cancel any pending transfers to prevent a segfault in libusb
	if (io_pending)
	{
		io_pending = FALSE;
		if (!completed)
		{
			libusb_cancel_transfer(transfer);
			while (!completed)
			{
				if (libusb_handle_events(NULL) < 0)
				{
					break;
				}
			}
		}
		libusb_free_transfer(transfer);
	}

	// NOTE: slv_close() has already checked for *udh != NULL .
	libusb_release_interface(*udh, 0);
	libusb_close(*udh);
	*udh = NULL;

	return 0;
}

static int tigl_reset(CableHandle *h)
{
	// Reset out pipe
	if (NULL != uHdl)
	{
		int ret = libusb_clear_halt(uHdl, uOutEnd);
		if (ret)
		{
			ticables_warning("libusb_clear_halt (%s).\n", libusb_strerror((libusb_error)ret));
		}

		// Reset in pipe
		ret = libusb_clear_halt(uHdl, uInEnd);
		if (ret)
		{
			ticables_warning("libusb_clear_halt (%s).\n", libusb_strerror((libusb_error)ret));
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
	struct libusb_config_descriptor *config;
	const struct libusb_interface *interface_;
	const struct libusb_interface_descriptor *interface;
	const struct libusb_endpoint_descriptor *endpoint;

	// open device
	ret = tigl_open(h->address, &uHdl);
	if (ret)
	{
		return ret;
	}

	cable_info = tigl_devices[h->address];
	uDev = (libusb_device *)(tigl_devices[h->address].dev);
	uInEnd  = 0x81;
	uOutEnd = 0x02;

	// get max packet size
	libusb_get_active_config_descriptor(uDev, &config);
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
		endpoint = &(interface->endpoint[i]);
		if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_BULK) == LIBUSB_TRANSFER_TYPE_BULK)
		{
			if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN)
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
	}
	nBytesRead = 0;
	was_max_ps = 0;

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
	int ret;

	/* Reset both endpoints (send an URB_FUNCTION_RESET_PIPE) */
	ret = tigl_reset(h);
	if (!ret)
	{
		/* Reset USB port (send an IOCTL_INTERNAL_USB_RESET_PORT) */
		/* NOTE: tigl_reset() has already checked for uHdl != NULL */
		ret = libusb_reset_device(uHdl);
		if (ret != 0)
		{
			ticables_warning("libusb_device_reset (%s).\n", libusb_strerror((libusb_error)ret));
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

	return ret;
}

// convenient function which send one or more bytes
static int send_block(CableHandle *h, uint8_t *data, int length)
{
	int ret, tmp;

	if (NULL == uHdl)
	{
		return ERR_WRITE_ERROR;
	}

	ret = libusb_bulk_transfer(uHdl, uOutEnd, (unsigned char*)data, length, &tmp, to);

	if (ret == LIBUSB_ERROR_TIMEOUT)
	{
		ticables_warning("libusb_bulk_transfer (%s).\n", libusb_strerror((libusb_error)ret));
		return ERR_WRITE_TIMEOUT;
	}
	else if (ret < 0)
	{
		ticables_warning("libusb_bulk_transfer (%s).\n", libusb_strerror((libusb_error)ret));
		return ERR_WRITE_ERROR;
	}

	// FIXME do Nspire CX II calculators also need this ?
	if ((tigl_devices[h->address].pid == PID_NSPIRE || tigl_devices[h->address].pid == PID_NSPIRE_CRADLE) && length % max_ps == 0)
	{
		ticables_info("XXX triggering an extra bulk write");
		ret = libusb_bulk_transfer(uHdl, uOutEnd, (unsigned char*)data, 0, &tmp, to);

		if (ret == LIBUSB_ERROR_TIMEOUT)
		{
			ticables_warning("libusb_bulk_transfer (%s).\n", libusb_strerror((libusb_error)ret));
			return ERR_WRITE_TIMEOUT;
		}
		else if (ret < 0)
		{
			ticables_warning("libusb_bulk_transfer (%s).\n", libusb_strerror((libusb_error)ret));
			return ERR_WRITE_ERROR;
		}
	}

	return 0;
}

static int slv_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	return send_block(h, data, len);
}

static void LIBUSB_CALL bulk_transfer_cb(struct libusb_transfer *transfer2)
{
	// This comes from libusb.
	int *completed2 = (int *)(transfer2->user_data);
	*completed2 = 1;
	/* caller interprets results and frees transfer */
}

static int slv_bulk_read(struct libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *buffer, int length,
	int *transferred, unsigned int timeout)
{
	// This is a variant of libusb_bulk_transfer in libusb, edited to take
	// the io_pending variable set in slv_check into account.
	int r;

	if (io_pending)
	{
		io_pending = FALSE;
	}
	else
	{
		completed = 0;
		transfer = libusb_alloc_transfer(0);
		if (!transfer)
		{
			return LIBUSB_ERROR_NO_MEM;
		}

		libusb_fill_bulk_transfer(transfer, dev_handle, endpoint,
					  buffer, length, bulk_transfer_cb,
					  &completed, timeout);

		r = libusb_submit_transfer(transfer);
		if (r < 0)
		{
			libusb_free_transfer(transfer);
			return r;
		}
	}

	while (!completed)
	{
		r = libusb_handle_events(NULL);
		if (r < 0)
		{
			if (r == LIBUSB_ERROR_INTERRUPTED)
			{
				continue;
			}
			libusb_cancel_transfer(transfer);
			while (!completed)
			{
				if (libusb_handle_events(NULL) < 0)
				{
					break;
				}
			}
			libusb_free_transfer(transfer);
			return r;
		}
	}

	*transferred = transfer->actual_length;
	switch (transfer->status)
	{
		case LIBUSB_TRANSFER_COMPLETED:
			r = 0;
			break;
		case LIBUSB_TRANSFER_TIMED_OUT:
			r = LIBUSB_ERROR_TIMEOUT;
			break;
		case LIBUSB_TRANSFER_STALL:
			r = LIBUSB_ERROR_PIPE;
			break;
		case LIBUSB_TRANSFER_OVERFLOW:
			r = LIBUSB_ERROR_OVERFLOW;
			break;
		case LIBUSB_TRANSFER_NO_DEVICE:
			r = LIBUSB_ERROR_NO_DEVICE;
			break;
		default:
			ticables_warning("slv_bulk_read: unrecognized status code %d", transfer->status);
			r = LIBUSB_ERROR_OTHER;
	}

	libusb_free_transfer(transfer);
	return r;
}

static int slv_get_(CableHandle *h, uint8_t *data)
{
	int ret = 0;
	int len = 0;
	tiTIME clk;

	/* Read up to max_ps bytes and store them in a buffer for subsequent accesses */
	if (nBytesRead <= 0)
	{
		TO_START(clk);
		do
		{
			// NOTE: slv_get() has already checked for uHdl != NULL .
			ret = slv_bulk_read(uHdl, uInEnd, (unsigned char*)rBuf, max_ps, &len, to);
		}
		while(!len && !ret);

		if (len == max_ps)
		{
			was_max_ps = 1;
		}
		else
		{
			was_max_ps = 0;
		}

		if (ret == LIBUSB_ERROR_TIMEOUT)
		{
			ticables_warning("slv_bulk_read (%s).\n", libusb_strerror((libusb_error)ret));
			nBytesRead = 0;
			return ERR_READ_TIMEOUT;
		}
		else if (ret != 0)
		{
			ticables_warning("slv_bulk_read (%s).\n", libusb_strerror((libusb_error)ret));
			nBytesRead = 0;
			return ERR_READ_ERROR;
		}

		nBytesRead = len;
		rBufPtr = rBuf;
	}

	*data = *rBufPtr++;
	nBytesRead--;

	return 0;
}

static int slv_get(CableHandle* h, uint8_t *data, uint32_t len)
{
	int i=0;
	int ret = 0;
	int tmp;

	if (NULL == uHdl)
	{
		return ERR_READ_ERROR;
	}

	/* we can't do that in any other way because slv_get_ can returns
	 * 1, 2, ..., len bytes.
	 *
	 * XXX But we know how much was actually recived can't we just try
	 * again if its less than we expected rather than this mess, whatever
	 * the point of it was?
	 */
	for(i = 0; i < (int)len; i++)
	{
		ret = slv_get_(h, data+i);
		if (ret != 0)
		{
			break;
		}
	}

	if (!ret && was_max_ps != 0 && nBytesRead == 0)
	{
		// FIXME do Nspire CX II calculators also need this ?
		if (   (tigl_devices[h->address].pid == PID_NSPIRE || tigl_devices[h->address].pid == PID_NSPIRE_CRADLE)
		    || len == 0 && (   tigl_devices[h->address].pid == PID_TI89TM
		                    || tigl_devices[h->address].pid == PID_TI84P
		                    || tigl_devices[h->address].pid == PID_TI84P_SE
		                   )
		   )
		{
			ticables_info("XXX triggering an extra bulk read");
			ret = slv_bulk_read(uHdl, uInEnd, (unsigned char*)data, max_ps, &tmp, to);

			if (ret == LIBUSB_ERROR_TIMEOUT)
			{
				ticables_warning("slv_bulk_read (%s).\n", libusb_strerror((libusb_error)ret));
				nBytesRead = 0;
				return ERR_READ_TIMEOUT;
			}
			else if (ret != 0)
			{
				ticables_warning("slv_bulk_read (%s).\n", libusb_strerror((libusb_error)ret));
				nBytesRead = 0;
				return ERR_READ_ERROR;
			}
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

	for (i = 0; i < MAX_CABLES; i++)
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
	// This really should be in libusb, but alas it isn't, so their code was
	// adapted by Kevin Kofler for use here. It's required to get TiEmu 3 to
	// work with the SilverLink.

	int r;
	struct timeval tv;

	if (nBytesRead > 0)
	{
		*status = TRUE;
		return 0;
	}

	if (NULL == uHdl)
	{
		return ERR_READ_ERROR;
	}

	if (!io_pending)
	{
		completed = 0;
		transfer = libusb_alloc_transfer(0);
		if (!transfer)
		{
			return ERR_READ_ERROR;
		}

		libusb_fill_bulk_transfer(transfer, uHdl, uInEnd, rBuf,
					  max_ps, bulk_transfer_cb,
					  &completed, to);
		transfer->type = LIBUSB_TRANSFER_TYPE_BULK;

		r = libusb_submit_transfer(transfer);
		if (r < 0)
		{
			libusb_free_transfer(transfer);
			return ERR_READ_ERROR;
		}

		io_pending = TRUE;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	r = libusb_handle_events_timeout(NULL, &tv);
	if (r < 0)
	{
		if (r == LIBUSB_ERROR_INTERRUPTED)
		{
			return 0;
		}
		libusb_cancel_transfer(transfer);
		while (!completed)
		{
			if (libusb_handle_events(NULL) < 0)
			{
				break;
			}
		}
		libusb_free_transfer(transfer);
		io_pending = FALSE;
		return ERR_READ_ERROR;
	}

	if (completed && transfer->status != LIBUSB_TRANSFER_COMPLETED
	    && transfer->status != LIBUSB_TRANSFER_TIMED_OUT)
	{
		libusb_free_transfer(transfer);
		io_pending = FALSE;
		return ERR_READ_ERROR;
	}

	if (transfer->actual_length > 0)
	{
		nBytesRead = transfer->actual_length;
		rBufPtr = rBuf;
		*status = STATUS_RX; // data available
	}
	if (completed)
	{
		io_pending = FALSE;
		libusb_free_transfer(transfer);
	}
	return 0;
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
