/* Hey EMACS -*- win32-c -*- */
/* $Id: slv_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* TI-GRAPH LINK USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "printl.h"
#include "logging.h"
#include "externs.h"
#include "timeout.h"

/* 
   This part talk with the USB device driver through the TiglUsb library.
   This module need TiglUsb.h (interface) but does not need TiglUsb.lib (linkage).
*/

#include <stdio.h>
#include <windows.h>

#include "tiglusb.h"

#define MIN_VERSION "3.0"

static HINSTANCE hDLL = NULL;	// DLL handle on TiglUsb.dll
static dllOk = FALSE;		// Dll loading is OK

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

int slv_init()
{
	int ret;

	// Create an handle on library and retrieve symbols
	hDLL = LoadLibrary("TIGLUSB.DLL");
	if (hDLL == NULL) {
		printl1(2, _("TiglUsb library not found. Have you installed the TiglUsb driver ?\n"));
		return ERR_OPEN_USB_DEV;
	}

	dynTiglUsbVersion = (TIGLUSB_VERSION) GetProcAddress(hDLL, "TiglUsbVersion");
	if (!dynTiglUsbVersion || (strcmp(dynTiglUsbVersion(), MIN_VERSION) < 0)) {
	    char buffer[256];
		sprintf(buffer, _("TiglUsb.dll: version %s mini needed, got version %s.\nPlease download the latest release on <http://ti-lpg.org/prj_usb>."),
			MIN_VERSION, dynTiglUsbVersion());
		printl1(2, buffer);
		MessageBox(NULL, buffer, "Error in SilverLink support", MB_OK);
		FreeLibrary(hDLL);
		return ERR_TIGLUSB_VERSION;
	}
	printl1(0, _("using TiglUsb.dll version %s\n"), dynTiglUsbVersion());

	dynTiglUsbOpen = (TIGLUSB_OPEN) GetProcAddress(hDLL, "TiglUsbOpen");
	if (!dynTiglUsbOpen) {
		printl1(2, _("Unable to load TiglUsbOpen symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbClose = (TIGLUSB_CLOSE) GetProcAddress(hDLL, "TiglUsbClose");
	if (!dynTiglUsbClose) {
		printl1(2, _("Unable to load TiglUsbClose symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbFlush = (TIGLUSB_FLUSH) GetProcAddress(hDLL, "TiglUsbFlush");
	if (!dynTiglUsbOpen) {
	    printl1(2, _("Unable to load TiglUsbFlush symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbReset = (TIGLUSB_RESET) GetProcAddress(hDLL, "TiglUsbReset");
	if (!dynTiglUsbOpen) {
	    printl1(2, _("Unable to load TiglUsbFlush symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbCheck = (TIGLUSB_CHECK) GetProcAddress(hDLL, "TiglUsbCheck");
	if (!dynTiglUsbCheck) {
		printl1(2, _("Unable to load TiglUsbCheck symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbRead = (TIGLUSB_READ) GetProcAddress(hDLL, "TiglUsbRead");
	if (!dynTiglUsbRead) {
		printl1(2, _("Unable to load TiglUsbRead symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbWrite = (TIGLUSB_WRITE) GetProcAddress(hDLL, "TiglUsbWrite");
	if (!dynTiglUsbWrite) {
	    printl1(2, _("Unable to load TiglUsbWrite symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

	dynTiglUsbSetTimeout = (TIGLUSB_SETTIMEOUT) GetProcAddress(hDLL, "TiglUsbSetTimeout");
	if (!dynTiglUsbSetTimeout) {
		printl1(2, _("Unable to load TiglUsbSetTimeout symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

    dynTiglUsbGetTimeout = (TIGLUSB_GETTIMEOUT) GetProcAddress(hDLL, "TiglUsbGetTimeout");
	if (!dynTiglUsbSetTimeout) {
		printl1(2, _("Unable to load TiglUsbSetTimeout symbol.\n"));
		FreeLibrary(hDLL);
		return ERR_FREELIBRARY;
	}

  
	// DLL loading succeeded !
	dllOk = TRUE;

	ret = dynTiglUsbOpen();
	switch (ret) {
		case TIGLERR_DEV_OPEN_FAILED: return ERR_OPEN_USB_DEV;
		case TIGLERR_DEV_ALREADY_OPEN: return ERR_OPEN_USB_DEV;
		default: break;
	}

	dynTiglUsbSetTimeout(time_out);

	START_LOGGING();

	return 0;
}

int slv_exit()
{
    int ret;

    STOP_LOGGING();

    ret = dynTiglUsbClose();

    /* Free library handle */
    if (hDLL != NULL)
        FreeLibrary(hDLL);
    hDLL = NULL;

    dllOk = FALSE;

    return 0;
}

int slv_open()
{
    int ret;

    if (!hDLL)
        ERR_TIGLUSB_VERSION;

    dynTiglUsbSetTimeout(time_out);

    ret = dynTiglUsbReset();
    if(ret == TIGLERR_RESET_FAILED)
        return ERR_TIGLUSB_RESET;

    tdr.count = 0;
    toSTART(tdr.start);

    return 0;
}

int slv_close()
{
    int ret;

    if (!hDLL)
        ERR_TIGLUSB_VERSION;

    ret = dynTiglUsbFlush();

    switch (ret) {
    case TIGLERR_WRITE_TIMEOUT:
        return ERR_WRITE_TIMEOUT;
    case TIGLERR_WRITE_ERROR:
        return ERR_WRITE_ERROR;
    default:
        break;
    }

    return 0;
}

int slv_put(uint8_t data)
{
    int ret;

    tdr.count++;
    LOG_DATA(data);

    ret = dynTiglUsbWrite(data);

    switch (ret) {
    case TIGLERR_WRITE_TIMEOUT:
        return ERR_WRITE_TIMEOUT;
    case TIGLERR_WRITE_ERROR:
        return ERR_WRITE_ERROR;
    default:
        break;
    }

    return 0;
}

int slv_get(uint8_t * data)
{
    int ret;

    ret = dynTiglUsbRead(data);

    switch (ret) {
    case TIGLERR_READ_TIMEOUT:
        return ERR_READ_TIMEOUT;
    case TIGLERR_READ_ERROR:
        return ERR_READ_ERROR;
    default:
        break;
    }

    tdr.count++;
    LOG_DATA(*data);

    return 0;
}

int slv_check(int *status)
{
    int ret = dynTiglUsbCheck(status);

    switch (ret) {
    case TIGLERR_READ_TIMEOUT:
        return ERR_READ_TIMEOUT;
    case TIGLERR_READ_ERROR:
        return ERR_READ_ERROR;
    default:
        break;
    }

    return 0;
}

int slv_probe()
{
  /*
     HANDLE hDev = dynTiglUsbOpen();

     if(hDev == INVALID_HANDLE_VALUE)
     {
     return ERR_PROBE_FAILED;
     }
     else
     {
     CloseHandle(hDev);
     return 0;
     }
   */

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire(int b)
{
  return 0;
}

int slv_set_white_wire(int b)
{
  return 0;
}

int slv_get_red_wire()
{
  return 0;
}

int slv_get_white_wire()
{
  return 0;
}

int slv_supported()
{
  return SUPPORT_ON;
}

int slv_reset()
{
	dynTiglUsbFlush();
	return 0;
}

int slv_register_cable_1(TicableLinkCable * lc)
{
  lc->init = slv_init;
  lc->open = slv_open;
  lc->put = slv_put;
  lc->get = slv_get;
  lc->close = slv_close;
  lc->exit = slv_exit;
  lc->probe = slv_probe;
  lc->check = slv_check;
  lc->reset = slv_reset;

  return 0;
}
