/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (c) 1999-2006 Romain Lievin
 *  Copyright (c) 2001 Julien Blache (original author)
 *  Copyright (c) 2007 Romain Liévin (libusb-win32 support)
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
#include <libusb-1.0/libusb.h>

#ifdef __WIN32__
#include <winsock2.h> /* struct timeval */
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
#if defined(__WIN32__)
#include "../win32/detect.h"
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
	{VID_TI, PID_TIGLUSB,  "TI-GRAPH LINK USB",           NULL},
	{VID_TI, PID_TI84P,    "TI-84 Plus Hand-Held",        NULL},
	{VID_TI, PID_TI89TM,   "TI-89 Titanium Hand-Held",    NULL},
	{VID_TI, PID_TI84P_SE, "TI-84 Plus Silver Hand-Held", NULL},
	{VID_TI, PID_NSPIRE,   "TI-Nspire Hand-Held",         NULL},
	{0,      0,            NULL,                          NULL}
};

// list of devices found
static usb_infos tigl_devices[MAX_CABLES+1];

// internal structure for holding data
typedef struct
{
	struct libusb_device *device;
	struct libusb_device_handle *handle;

	int      nBytesRead;
	uint8_t  rBuf[64];
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
#define max_ps     (((usb_struct *)(h->priv2))->max_ps)
#define was_max_ps (((usb_struct *)(h->priv2))->was_max_ps)
#define nBytesRead (((usb_struct *)(h->priv2))->nBytesRead)
#define rBuf       (((usb_struct *)(h->priv2))->rBuf)
#define rBufPtr    (((usb_struct *)(h->priv2))->rBufPtr)
#define uInEnd     (((usb_struct *)(h->priv2))->in_endpoint)
#define uOutEnd    (((usb_struct *)(h->priv2))->out_endpoint)

#if HAVE_LIBUSB10_STRERROR
const char* tigl_strerror(enum libusb_error errcode)
{
	return libusb_strerror(errorcode);
}
#else
/*
 * Taken from libusb git, will be included in later releases of
 * libusb-1.0 but as most distros will not be shipping that for a while
 * this will have to do. ~ Jon 2011/02/08
 */
const char* tigl_strerror(enum libusb_error errcode)
{
	switch (errcode)
	{
		case LIBUSB_SUCCESS:
			return "Success";
		case LIBUSB_ERROR_IO:
			return "Input/output error";
		case LIBUSB_ERROR_INVALID_PARAM:
			return "Invalid parameter";
		case LIBUSB_ERROR_ACCESS:
			return "Access denied (insufficient permissions)";
		case LIBUSB_ERROR_NO_DEVICE:
			return "No such device (it may have been disconnected)";
		case LIBUSB_ERROR_NOT_FOUND:
			return "Entity not found";
		case LIBUSB_ERROR_BUSY:
			return "Resource busy";
		case LIBUSB_ERROR_TIMEOUT:
			return "Operation timed out";
		case LIBUSB_ERROR_OVERFLOW:
			return "Overflow";
		case LIBUSB_ERROR_PIPE:
			return "Pipe error";
		case LIBUSB_ERROR_INTERRUPTED:
			return "System call interrupted (perhaps due to signal)";
		case LIBUSB_ERROR_NO_MEM:
			return "Insufficient memory";
		case LIBUSB_ERROR_NOT_SUPPORTED:
			return "Operation not supported or unimplemented on this platform";
		case LIBUSB_ERROR_OTHER:
			return "Other error";
	}
	return "Unknown error";
}
#endif

static void tigl_get_product(unsigned char * string, size_t maxlen, struct libusb_device *dev)
{
	libusb_device_handle *han;
	int ret;
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);

	string[0] = 0;

	if (r < 0)
	{
		ticables_error("failed to get device descriptor");
	}

	if (desc.iProduct)
	{
		if (!libusb_open(dev, &han))
		{
			ret = libusb_get_string_descriptor_ascii(han, desc.iProduct, string, maxlen);
			libusb_close(han);
			if (ret <= 0)
			{
				ticables_warning("libusb_get_string_descriptor_ascii (%s).\n", tigl_strerror(ret));
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
				if(desc.idProduct == tigl_infos[k].pid)
				{
					unsigned char string[64+1];
					tigl_get_product(string, sizeof(string) - 1, device);
					ticables_info(_(" found %s on #%i, version <%x.%02x>\n"),
						      string, j+1,
						      desc.bcdDevice >> 8,
						      desc.bcdDevice & 0xff);

					memcpy(&tigl_devices[j], &tigl_infos[k], sizeof(usb_infos));
					tigl_devices[j++].dev = device;
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
		ticables_warning(_(" no devices found!\n"));
		return ERR_LIBUSB_OPEN;
	}

	return 0;
}

static int tigl_open(int id, libusb_device_handle ** udh)
{
	int ret;

	tigl_enum();

	if(tigl_devices[id].dev == NULL)
	{
		return ERR_LIBUSB_OPEN;
	}

	if (!libusb_open(tigl_devices[id].dev, udh))
	{
		/* only one configuration: #1 */
		ret = libusb_set_configuration(*udh, 1);
		if (ret)
		{
			ticables_warning("libusb_set_configuration (%s).\n", tigl_strerror(ret));
		}

		/* configuration #1, interface #0 */
		ret = libusb_claim_interface(*udh, 0);
		if (ret)
		{
			ticables_warning("libusb_claim_interface (%s).\n", tigl_strerror(ret));
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

	libusb_release_interface(*udh, 0);
	libusb_close(*udh);
	*udh = NULL;

	return 0;
}

static int tigl_reset(CableHandle *h)
{
	int ret;

	// Reset out pipe
	ret = libusb_clear_halt(uHdl, uOutEnd);
	if (ret)
	{
		ticables_warning("libusb_clear_halt (%s).\n", tigl_strerror(ret));
	}

	// Reset in pipe
	ret = libusb_clear_halt(uHdl, uInEnd);
	if (ret)
	{
		ticables_warning("libusb_clear_halt (%s).\n", tigl_strerror(ret));
	}

	return 0;
}

/* API */

static int slv_prepare(CableHandle *h)
{
	char str[64];

#if defined(__WIN32__)
	TRYC(win32_check_libusb());
#elif defined(__BSD__)
	TRYC(bsd_check_libusb());
#else
	TRYC(linux_check_libusb());
#endif

	if(h->port >= MAX_CABLES)
	{
		return ERR_ILLEGAL_ARG;
	}

	h->address = h->port-1;
	sprintf(str, "TiglUsb #%i", h->port);
	h->device = strdup(str);
	h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));

	return 0;
}

static int slv_open(CableHandle *h)
{
	int i;
	struct libusb_config_descriptor *config;
	const struct libusb_interface *interface_;
	const struct libusb_interface_descriptor *interface;
	const struct libusb_endpoint_descriptor *endpoint;

	// open device
	TRYC(tigl_open(h->address, &uHdl));
	uDev = tigl_devices[h->address].dev;
	uInEnd  = 0x81;
	uOutEnd = 0x02;

	// get max packet size
	libusb_get_active_config_descriptor(uDev, &config);
	interface_ = &(config->interface[0]);
	interface = &(interface_->altsetting[0]);
	endpoint = &(interface->endpoint[0]);
	max_ps = endpoint->wMaxPacketSize;

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

static int slv_reset(CableHandle *h)
{
	int ret = 0;

	/* Reset both endpoints (send an URB_FUNCTION_RESET_PIPE) */
	TRYC(tigl_reset(h));

	/* Reset USB port (send an IOCTL_INTERNAL_USB_RESET_PORT) */
	ret = libusb_reset_device(uHdl);
	if (ret != 0)
	{
		ticables_warning("libusb_device_reset (%s).\n", tigl_strerror(ret));
		return ERR_LIBUSB_RESET;
	}
	else
	{
		// lib-usb doc: after calling usb_reset, the device will need to re-enumerate
		// and thusly, requires you to find the new device and open a new handle. The
		// handle used to call usb_reset will no longer work.
#ifdef __WIN32__
		Sleep(500);
#else
		usleep(500000);
#endif
		TRYC(slv_close(h));

		h->priv2 = (usb_struct *)calloc(1, sizeof(usb_struct));
		TRYC(slv_open(h));
	}

	return 0;
}

// convenient function which send one or more bytes
static int send_block(CableHandle *h, uint8_t *data, int length)
{
	int ret, tmp;

	ret = libusb_bulk_transfer(uHdl, uOutEnd, (unsigned char*)data, length, &tmp, to);

	if (ret == LIBUSB_ERROR_TIMEOUT)
	{
		ticables_warning("libusb_bulk_transfer (%s).\n", tigl_strerror(ret));
		return ERR_WRITE_TIMEOUT;
	}
	else if (ret < 0)
	{
		ticables_warning("libusb_bulk_transfer (%s).\n", tigl_strerror(ret));
		return ERR_WRITE_ERROR;
	}

	if (tigl_devices[h->address].pid == PID_NSPIRE && length % max_ps == 0)
	{
		ticables_info("XXX triggering an extra bulk write");
		ret = libusb_bulk_transfer(uHdl, uOutEnd, (unsigned char*)data, 0, &tmp, to);

		if (ret == LIBUSB_ERROR_TIMEOUT)
		{
			ticables_warning("libusb_bulk_transfer (%s).\n", tigl_strerror(ret));
			return ERR_WRITE_TIMEOUT;
		}
		else if (ret < 0)
		{
			ticables_warning("libusb_bulk_transfer (%s).\n", tigl_strerror(ret));
			return ERR_WRITE_ERROR;
		}
	}


	return 0;
}

static int slv_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	return send_block(h, data, len);
}

static void bulk_transfer_cb(struct libusb_transfer *transfer2)
{
	// This comes from libusb.
	int *completed2 = transfer2->user_data;
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

	/* Read up to 32/64 bytes and store them in a buffer for subsequent accesses */
	if (nBytesRead <= 0)
	{
		TO_START(clk);
		do
		{
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
			ticables_warning("slv_bulk_read (%s).\n", tigl_strerror(ret));
			nBytesRead = 0;
			return ERR_READ_TIMEOUT;
		}
		else if (ret != 0)
		{
			ticables_warning("slv_bulk_read (%s).\n", tigl_strerror(ret));
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
		if (   tigl_devices[h->address].pid == PID_NSPIRE
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
				ticables_warning("slv_bulk_read (%s).\n", tigl_strerror(ret));
				nBytesRead = 0;
				return ERR_READ_TIMEOUT;
			}
			else if (ret != 0)
			{
				ticables_warning("slv_bulk_read (%s).\n", tigl_strerror(ret));
				nBytesRead = 0;
				return ERR_READ_ERROR;
			}
		}
	}

	return ret;
}

static int slv_probe(CableHandle *h)
{
	int i;

	TRYC(tigl_enum());

	for(i = 0; i < MAX_CABLES; i++)
	{
		if(tigl_devices[h->address].pid == PID_TIGLUSB)
		{
			return 0;
		}
	}

	return ERR_PROBE_FAILED;
}

static int raw_probe(CableHandle *h)
{
	int i;

	TRYC(tigl_enum());

	for(i = 0; i < MAX_CABLES; i++)
	{
		if (tigl_devices[h->address].pid == PID_TI89TM ||
		    tigl_devices[h->address].pid == PID_TI84P ||
		    tigl_devices[h->address].pid == PID_TI84P_SE ||
		    tigl_devices[h->address].pid == PID_NSPIRE)
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

	if(nBytesRead > 0)
	{
		*status = TRUE;
		return 0;
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

static int slv_set_device(CableHandle *h, const char * device)
{
	return 0;
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
	NULL, NULL,
	&slv_set_device
};

const CableFncts cable_raw =
{
	CABLE_USB,
	"USB",
	N_("DirectLink"),
	N_("DirectLink (DIRECT USB) cable"),
	0,
	&slv_prepare,
	&slv_open, &slv_close, &slv_reset, &raw_probe, NULL,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
	NULL, NULL,
	&slv_set_device
};

//=======================

TIEXPORT1 int TICALL usb_probe_devices1(int **list);

TIEXPORT1 int TICALL usb_probe_devices(int **list)
{
	return usb_probe_devices1(list);
}

// returns number of devices and list of PIDs (dynamically allocated)
TIEXPORT1 int TICALL usb_probe_devices1(int **list)
{
	int i;

	TRYC(tigl_enum());

	*list = (int *)calloc(MAX_CABLES+1, sizeof(int));
	for(i = 0; i < MAX_CABLES; i++)
	{
		(*list)[i] = tigl_devices[i].pid;
	}

	return 0;
}
