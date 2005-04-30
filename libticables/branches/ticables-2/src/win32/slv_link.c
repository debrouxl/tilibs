/* Hey EMACS -*- win32-c -*- */
/* $Id: slv_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* TI-GRAPH LINK USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../gettext.h"
#include "../logging.h"
#include "../ticables.h"
#include "detect.h"
#include "../error.h"

/* 
   This part talk with the USB device driver through the TiglUsb library.
   This module need TiglUsb.h (interface) but does not need TiglUsb.lib (linkage).
*/

#include <stdio.h>
#include <windows.h>

#include "tiglusb.h"

#define MIN_VERSION "3.0"

static HINSTANCE hDLL = NULL;	// DLL handle on TiglUsb.dll

TIGLUSB_VERSION dynTiglUsbVersion = NULL;	// Functions pointers for dynamic loading

TIGLUSB_OPEN dynTiglUsbOpen = NULL;
TIGLUSB_CLOSE dynTiglUsbClose = NULL;

TIGLUSB_FLUSH dynTiglUsbFlush = NULL;
TIGLUSB_RESET dynTiglUsbReset = NULL;

TIGLUSB_CHECK dynTiglUsbCheck = NULL;
TIGLUSB_READ dynTiglUsbRead = NULL;
TIGLUSB_WRITE dynTiglUsbWrite = NULL;

TIGLUSB_SETTIMEOUT dynTiglUsbSetTimeout = NULL;
TIGLUSB_GETTIMEOUT dynTiglUsbGetTimeout = NULL;

#define hCom	(HANDLE)(h->priv)
#define BUFFER_SIZE 1024

static int slv_prepare(TiHandle *h)
{
	h->address = 0;
	h->device = NULL;

	// detect driver
	if(!win32_detect_tiglusb())
		return -1;

	return 0;
}

static int slv_open(TiHandle *h)
{
	int ret;

	// Create an handle on library and retrieve symbols
	hDLL = LoadLibrary("TIGLUSB.DLL");
	if (hDLL == NULL) {
		ticables_warning(_("TiglUsb library not found. Have you installed the TiglUsb driver ?"));
		return ERR_OPEN_USB_DEV;
	}

	dynTiglUsbVersion = (TIGLUSB_VERSION) GetProcAddress(hDLL, "TiglUsbVersion");
	if (!dynTiglUsbVersion || (strcmp(dynTiglUsbVersion(), MIN_VERSION) < 0)) {
	    char buffer[256];
		sprintf(buffer, _("TiglUsb.dll: version %s mini needed, got version %s.\nPlease download the latest release on <http://ti-lpg.org/prj_usb>."),
			MIN_VERSION, dynTiglUsbVersion());
		ticables_warning(buffer);
		MessageBox(NULL, buffer, "Error in SilverLink support", MB_OK);
		FreeLibrary(hDLL);
		return ERR_TIGLUSB_VERSION;
	}
	ticables_info(_("using TiglUsb.dll version %s"), dynTiglUsbVersion());

	dynTiglUsbOpen = (TIGLUSB_OPEN) GetProcAddress(hDLL, "TiglUsbOpen");
	if (!dynTiglUsbOpen) {
		ticables_warning(_("Unable to load TiglUsbOpen symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbClose = (TIGLUSB_CLOSE) GetProcAddress(hDLL, "TiglUsbClose");
	if (!dynTiglUsbClose) {
		ticables_warning(_("Unable to load TiglUsbClose symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbFlush = (TIGLUSB_FLUSH) GetProcAddress(hDLL, "TiglUsbFlush");
	if (!dynTiglUsbOpen) {
	    ticables_warning(_("Unable to load TiglUsbFlush symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbReset = (TIGLUSB_RESET) GetProcAddress(hDLL, "TiglUsbReset");
	if (!dynTiglUsbOpen) {
	    ticables_warning(_("Unable to load TiglUsbFlush symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbCheck = (TIGLUSB_CHECK) GetProcAddress(hDLL, "TiglUsbCheck");
	if (!dynTiglUsbCheck) {
		ticables_warning(_("Unable to load TiglUsbCheck symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbRead = (TIGLUSB_READ) GetProcAddress(hDLL, "TiglUsbRead");
	if (!dynTiglUsbRead) {
		ticables_warning(_("Unable to load TiglUsbRead symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbWrite = (TIGLUSB_WRITE) GetProcAddress(hDLL, "TiglUsbWrite");
	if (!dynTiglUsbWrite) {
	    ticables_warning(_("Unable to load TiglUsbWrite symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbSetTimeout = (TIGLUSB_SETTIMEOUT) GetProcAddress(hDLL, "TiglUsbSetTimeout");
	if (!dynTiglUsbSetTimeout) {
		ticables_warning(_("Unable to load TiglUsbSetTimeout symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbGetTimeout = (TIGLUSB_GETTIMEOUT) GetProcAddress(hDLL, "TiglUsbGetTimeout");
	if (!dynTiglUsbSetTimeout) {
		ticables_warning(_("Unable to load TiglUsbSetTimeout symbol."));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}
  
	ret = dynTiglUsbOpen();
	switch (ret) {
		case TIGLERR_DEV_OPEN_FAILED: return ERR_OPEN_USB_DEV;
		case TIGLERR_DEV_ALREADY_OPEN: return ERR_OPEN_USB_DEV;
		default: break;
	}

	dynTiglUsbSetTimeout(h->timeout);

	return 0;
}

static int slv_close(TiHandle *h)
{
	int ret;

    ret = dynTiglUsbClose();

    /* Free library handle */
    if (hDLL != NULL)
        FreeLibrary(hDLL);
    hDLL = NULL;

	return 0;
}

static int slv_reset(TiHandle *h)
{
	int ret;

	/*
	ret = dynTiglUsbFlush();

    switch (ret) {
    case TIGLERR_WRITE_TIMEOUT:
        return ERR_WRITE_TIMEOUT;
    case TIGLERR_WRITE_ERROR:
        return ERR_WRITE_ERROR;
    default:
        break;
    }
	*/

    ret = dynTiglUsbReset();
    if(ret == TIGLERR_RESET_FAILED)
        return ERR_TIGLUSB_RESET;

	return 0;
}

static int slv_put(TiHandle *h, uint8_t data)
{
	int ret;

    ret = dynTiglUsbWrite(data);

    switch (ret) 
	{
    case TIGLERR_WRITE_TIMEOUT:
        return ERR_WRITE_TIMEOUT;
    case TIGLERR_WRITE_ERROR:
        return ERR_WRITE_ERROR;
    default:
        break;
    }

	return 0;
}

static int slv_get(TiHandle *h, uint8_t *data)
{
	int ret;

    ret = dynTiglUsbRead(data);

    switch (ret) 
	{
    case TIGLERR_READ_TIMEOUT:
        return ERR_READ_TIMEOUT;
    case TIGLERR_READ_ERROR:
        return ERR_READ_ERROR;
    default:
        break;
    }

	return 0;
}

static int slv_probe(TiHandle *h)
{
	return 0;
}

static int slv_check(TiHandle *h, int *status)
{
	int ret = dynTiglUsbCheck(status);

    switch (ret) 
	{
    case TIGLERR_READ_TIMEOUT:
        return ERR_READ_TIMEOUT;
    case TIGLERR_READ_ERROR:
        return ERR_READ_ERROR;
    default:
        break;
    }

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

	&slv_prepare, &slv_probe,
	&slv_open, &slv_close, &slv_reset,
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

	&slv_prepare, &slv_probe,
	&slv_open, &slv_close, &slv_reset,
	&slv_put, &slv_get, &slv_check,
	&slv_set_red_wire, &slv_set_white_wire,
	&slv_get_red_wire, &slv_get_white_wire,
};
