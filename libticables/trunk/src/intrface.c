/* Hey EMACS -*- linux-c -*- */
/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(__WIN32__)
# include <windows.h>
#endif

#if defined(__LINUX__)
# include <unistd.h>
# include <sys/types.h>
#endif

#include "intl.h"
#include "cabl_ver.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "links.h"
#include "verbose.h"
#include "export.h"
#include "probe.h"
#include "type2str.h"

#ifdef __LINUX__
#include "timodules.h"
#endif

/*****************/
/* Internal data */
/*****************/

int time_out = DFLT_TIMEOUT; // Timeout value for cables in 0.10 seconds
int delay = DFLT_DELAY;      // Time between 2 bits (home-made cables only)
int baud_rate = BR9600;      // Baud rate setting for serial port
int hfc = HFC_ON;            // Hardware flow control for fastAVRlink
#ifdef __MACOSX__
int port = OSX_USB_PORT;     // Use USB by default, handled through the IOKit
#else
int port = SERIAL_PORT_2;    // Use COM2 by default
#endif
int resources = IO_NONE;     // I/O methods detected
int method = IOM_AUTO;       // I/O method to use (automatic)

unsigned int io_address = 0; // I/O port base address
char io_device[256] = "";    // The character device (COMx, ttySx, ...)

const char *err_msg;         // The error message of the last error occured
int cable_type;              // Used for debug

char *os;		             // Operating System type
TicablePortInfo pinfo;	     // Informations on I/O ports
TicableDataRate tdr;         // Data rate during transfers


/****************/
/* Entry points */
/****************/

/*
  This function should be the first one to call.
  It tries to list available I/O functions (I/O resources).
 */
TIEXPORT int
TICALL ticable_init()
{
#ifdef __WIN32__
  HANDLE hDll;
  char LOCALEDIR[65536];
  int i;
  hDll = GetModuleHandle("ticables.dll");
  GetModuleFileName(hDll, LOCALEDIR, 65535);
  for (i = strlen(LOCALEDIR); i >= 0; i--) {
    if (LOCALEDIR[i] == '\\')
      break;
  }
  LOCALEDIR[i] = '\0';
  strcat(LOCALEDIR, "\\locale");
#endif

  errno = 0;

#if defined(ENABLE_NLS)
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  DISPLAY("%s: bindtextdomain to %s\n", PACKAGE, LOCALEDIR);
  //bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  textdomain(PACKAGE);
#endif
  DISPLAY(_("Libticables: version %s\n"), LIBTICABLES_VERSION);

  //ticable_detect_os(&os);
  //ticable_detect_port(&pinfo);
  probe_io_resources();

  return 0;
}


/*
  This function should be called when the libticables library is
  no longer used.
 */
TIEXPORT int
TICALL ticable_exit()
{
  return 0;
}


/***********/
/* Methods */
/***********/


TIEXPORT const char *TICALL ticable_get_version()
{
  return LIBTICABLES_VERSION;
}


TIEXPORT void
TICALL ticable_set_timeout(int timeout_v)
{
  time_out = timeout_v;
}


TIEXPORT int
TICALL ticable_get_timeout()
{
  return time_out;
}


TIEXPORT void
TICALL ticable_set_delay(int delay_v)
{
  delay = delay_v;
}


TIEXPORT int
TICALL ticable_get_delay()
{
  return delay;
}


TIEXPORT void
TICALL ticable_set_baudrate(int br)
{
  baud_rate = br;
}


TIEXPORT int
TICALL ticable_get_baudrate()
{
  return baud_rate;
}


TIEXPORT void
TICALL ticable_set_io_address(unsigned int io_addr)
{
  io_address = io_addr;
}


TIEXPORT unsigned int
TICALL ticable_get_io_address()
{
  return io_address;
}


TIEXPORT void
TICALL ticable_set_io_device(char *dev)
{
  strcpy(io_device, dev);
}


TIEXPORT char *TICALL ticable_get_io_device()
{
  return io_device;
}


TIEXPORT void
TICALL ticable_set_hfc(int action)
{
  hfc = action;
}


TIEXPORT int
TICALL ticable_get_hfc(void)
{
  return hfc;
}


TIEXPORT void
TICALL ticable_set_port(TicablePort p)
{
  port = p;
}


TIEXPORT int
TICALL ticable_get_port(void)
{
  return port;
}


TIEXPORT void
TICALL ticable_set_method(int m)
{
  method = m;
}


TIEXPORT int
TICALL ticable_get_method(void)
{
  return method;
}

TIEXPORT int
TICALL ticable_get_datarate(TicableDataRate ** ptdr)
{
  *ptdr = &tdr;
  return tdr.count;
}

static int convert_port_into_device(void);

TIEXPORT int
TICALL ticable_set_param2(TicableLinkParam lp)
{
  time_out = lp.timeout;
  delay = lp.delay;
  baud_rate = lp.baud_rate;
  hfc = lp.hfc;
  port = lp.port;
  method = lp.method;

  if ((port == USER_PORT) || (port == OSX_SERIAL_PORT))	// force args
  {
    io_address = lp.io_addr;
    strcpy(io_device, lp.device);
  }

  return 0;
}


TIEXPORT int
TICALL ticable_set_param(const TicableLinkParam * lp)
{
  return ticable_set_param2(*lp);
}


TIEXPORT int
TICALL ticable_get_param(TicableLinkParam * lp)
{
  lp->timeout = time_out;
  lp->delay = delay;
  lp->baud_rate = baud_rate;
  lp->hfc = hfc;

  lp->io_addr = io_address;
  strcpy(lp->device, io_device);

  lp->port = port;
  lp->method = method;

  return 0;
}


TIEXPORT int
TICALL ticable_get_default_param(TicableLinkParam * lp)
{
  lp->calc_type = 2;		// CALC_TI92
#ifdef __MACOSX__
  lp->link_type = LINK_SLV;	// USB by default on Mac OS X
#else
  lp->link_type = LINK_TGL;	// does not change this (WinNT)
#endif
  lp->timeout = DFLT_TIMEOUT;
  lp->delay = DFLT_DELAY;
  lp->baud_rate = BR9600;
  lp->io_addr = AUTO_ADDR;
  strcpy(lp->device, AUTO_NAME);
  lp->hfc = HFC_ON;

#ifdef __MACOSX__
  lp->port = OSX_USB_PORT;
#else
  lp->port = SERIAL_PORT_2;
#endif
  lp->method = IOM_AUTO;

  return 0;
}


extern TicableLinkCable *tcl;
static void print_informations();

TIEXPORT int
TICALL ticable_set_cable(int typ, TicableLinkCable * lc)
{
  int type = typ;
  cable_type = type;

  set_default_cable(lc);

  // compile informations (I/O resources & OS platform) in order to 
  // determine the best I/O method to use.
  if (method & IOM_AUTO)
    method &= ~(IOM_ASM | IOM_API | IOM_DRV);

  method &= ~IOM_OK;

  if ((type == LINK_TGL) && (resources & IO_API))
    method |= IOM_API | IOM_OK;
  else if ((type == LINK_TGL) && (resources & IO_OSX))
    method |= IOM_API | IOM_OK;

  if ((type == LINK_AVR) && (resources & IO_API))
    method |= IOM_API | IOM_OK;

  if ((type == LINK_SER) && (resources & IO_TISER))
    method |= IOM_DRV | IOM_OK;
  else if ((type == LINK_SER) && (resources & IO_ASM))
    method |= IOM_ASM | IOM_OK;
  else if ((type == LINK_SER) && (resources & IO_DLL))
    method |= IOM_DRV | IOM_OK;
  else if ((type == LINK_SER) && (resources & IO_API))
    method |= IOM_API | IOM_OK;

  if ((type == LINK_PAR) && (resources & IO_TIPAR))
    method |= IOM_DRV | IOM_OK;
  else if ((type == LINK_PAR) && (resources & IO_ASM))
    method |= IOM_ASM | IOM_OK;
  else if ((type == LINK_PAR) && (resources & IO_DLL))
    method |= IOM_DRV | IOM_OK;

  if ((type == LINK_SLV) && (resources & IO_TIUSB))
    method |= IOM_DRV | IOM_OK;
  else if ((type == LINK_SLV) && (resources & IO_LIBUSB))
    method |= IOM_API | IOM_OK;
  else if ((type == LINK_SLV) && (resources & IO_OSX))
    method |= IOM_OK;

  if ((type == LINK_TIE) || (type == LINK_VTI)) {
    method |= IOM_API | IOM_OK;
  }

  if (!(method & IOM_OK)) {
    DISPLAY_ERROR("unable to find an I/O method.\n");
    return ERR_NO_RESOURCES;
  }
  // fill device and io_addr fields
  convert_port_into_device();
  print_informations();

  // set the link cable
  if (((resources & IO_LINUX) && !(method & IOM_DRV)) || (resources & IO_WIN32) || (resources & IO_OSX) || (resources & IO_BSD)) {	// no kernel driver (tipar/tiser/tiusb)
    switch (type) {
#if !defined(__MACOSX__)
    case LINK_PAR:		// IOM_ASM, IOM_DRV&Win32
      if ((port != PARALLEL_PORT_1) &&
	  (port != PARALLEL_PORT_2) &&
	  (port != PARALLEL_PORT_3) && (port != USER_PORT))
	return ERR_INVALID_PORT;

      lc->init = par_init;
      lc->open = par_open;
      lc->put = par_put;
      lc->get = par_get;
      lc->close = par_close;
      lc->exit = par_exit;
      lc->probe = par_probe;
      lc->check = par_check;

      lc->set_red_wire = par_set_red_wire;
      lc->set_white_wire = par_set_white_wire;
      lc->get_red_wire = par_get_red_wire;
      lc->get_white_wire = par_get_white_wire;
      break;

    case LINK_SER:		// IOM_ASM, IOM_API, IOM_DRV&Win32
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

      if ((method & IOM_ASM) || (method & IOM_DRV)) {
	// serial routines in IOM_ASM/DLL mode
	lc->init = ser_init;
	lc->open = ser_open;
	lc->put = ser_put;
	lc->get = ser_get;
	lc->close = ser_close;
	lc->exit = ser_exit;
	lc->probe = ser_probe;
	lc->check = ser_check;

	lc->set_red_wire = ser_set_red_wire;
	lc->set_white_wire = ser_set_white_wire;
	lc->get_red_wire = ser_get_red_wire;
	lc->get_white_wire = ser_get_white_wire;
      } else if (method & IOM_API) {
	// serial routines in IOM_API mode
	lc->init = ser_init2;
	lc->open = ser_open2;
	lc->put = ser_put2;
	lc->get = ser_get2;
	lc->close = ser_close2;
	lc->exit = ser_exit2;
	lc->probe = ser_probe2;
	lc->check = ser_check2;

	lc->set_red_wire = ser_set_red_wire2;
	lc->set_white_wire = ser_set_white_wire2;
	lc->get_red_wire = ser_get_red_wire2;
	lc->get_white_wire = ser_get_white_wire2;
      } else
	set_default_cable(lc);
      break;

    case LINK_AVR:		// IOM_API
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

      lc->init = avr_init;
      lc->open = avr_open;
      lc->put = avr_put;
      lc->get = avr_get;
      lc->close = avr_close;
      lc->exit = avr_exit;
      lc->probe = avr_probe;
      lc->check = avr_check;
      break;

    case LINK_VTL:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;

      lc->init = vtl_init;
      lc->open = vtl_open;
      lc->put = vtl_put;
      lc->get = vtl_get;
      lc->close = vtl_close;
      lc->exit = vtl_exit;
      lc->probe = vtl_probe;
      lc->check = vtl_check;
      break;

    case LINK_TIE:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;

      lc->init = tie_init;
      lc->open = tie_open;
      lc->put = tie_put;
      lc->get = tie_get;
      lc->close = tie_close;
      lc->exit = tie_exit;
      lc->probe = tie_probe;
      lc->check = tie_check;
      break;

#endif				/* !__MACOSX__ */

    case LINK_TGL:		// IOM_API
#ifndef __MACOSX__
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
#else
      if (port != OSX_SERIAL_PORT)
#endif
	return ERR_INVALID_PORT;

      lc->init = tig_init;
      lc->open = tig_open;
      lc->put = tig_put;
      lc->get = tig_get;
      lc->close = tig_close;
      lc->exit = tig_exit;
      lc->probe = tig_probe;
      lc->check = tig_check;
      break;

    case LINK_VTI:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;

      lc->init = vti_init;
      lc->open = vti_open;
      lc->put = vti_put;
      lc->get = vti_get;
      lc->close = vti_close;
      lc->exit = vti_exit;
      lc->probe = vti_probe;
      lc->check = vti_check;
      break;

    case LINK_SLV:		// IOM_LIBUSB (Linux) or IOM_TIUSB (Win32) or IOM_MAC (IOKit)
#ifndef __MACOSX__
      if ((port != USB_PORT_1) &&
	  (port != USB_PORT_2) &&
	  (port != USB_PORT_3) &&
	  (port != USB_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;
#else
      if (port != OSX_USB_PORT)
	return ERR_INVALID_PORT;
#endif

#ifdef __WIN32__		// IOM_TIUSB
      lc->init = slv_init;
      lc->open = slv_open;
      lc->put = slv_put;
      lc->get = slv_get;
      lc->close = slv_close;
      lc->exit = slv_exit;
      lc->probe = slv_probe;
      lc->check = slv_check;
#else				// IOM_TIUSB
      lc->init = slv_init2;
      lc->open = slv_open2;
      lc->put = slv_put2;
      lc->get = slv_get2;
      lc->close = slv_close2;
      lc->exit = slv_exit2;
      lc->probe = slv_probe2;
      lc->check = slv_check2;
#endif
      break;

    default:			// error !
      DISPLAY(_
	      ("Function not implemented. This is a bug. Please report it."));
      DISPLAY(_("Informations:\n"));
      DISPLAY(_("Cable: %i\n"), type);
      DISPLAY(_("Program halted before crashing...\n"));
      exit(-1);
      break;
    }
  } else {			// kernel driver
#if defined(__LINUX__)
    switch (type) {
    case LINK_PAR:		// IOM_TIPAR
    case LINK_SER:		// IOM_TISER
      if ((port != PARALLEL_PORT_1) &&
	  (port != PARALLEL_PORT_2) &&
	  (port != PARALLEL_PORT_3) &&
	  (port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

      lc->init = dev_init;
      lc->open = dev_open;
      lc->put = dev_put;
      lc->get = dev_get;
      lc->close = dev_close;
      lc->exit = dev_exit;
      lc->probe = dev_probe;
      lc->check = dev_check;
      break;

    case LINK_SLV:		// IOM_TIUSB
      if ((port != USB_PORT_1) &&
	  (port != USB_PORT_2) &&
	  (port != USB_PORT_3) &&
	  (port != USB_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

      lc->init = slv_init;
      lc->open = slv_open;
      lc->put = slv_put;
      lc->get = slv_get;
      lc->close = slv_close;
      lc->exit = slv_exit;
      lc->probe = slv_probe;
      lc->check = slv_check;
      break;

    default:			// error !
      DISPLAY(_
	      ("Function not implemented. This is a bug. Please report it."));
      DISPLAY(_("Informations:\n"));
      DISPLAY(_("Cable: %i\n"), type);
      DISPLAY(_("Program halted before crashing...\n"));
      exit(-1);
      break;
    }
#endif
  }
  tcl = lc;			// for the error function

  return 0;
}


static void print_informations(void)
{
  DISPLAY(_("Libticables: current settings...\n"));

  DISPLAY(_("  Link cable: %s\n"),
	  ticable_cabletype_to_string(cable_type));
  DISPLAY(_("  Port: %s\n"), ticable_port_to_string(port));
  DISPLAY(_("  Method: %s\n"), ticable_method_to_string(method));
  DISPLAY(_("  Timeout value: %i\n"), time_out);
  DISPLAY(_("  Delay value: %i\n"), delay);
  DISPLAY(_("  Baud-rate: %i\n"), baud_rate);
  DISPLAY(_("  Hardware flow control: %s\n"), hfc ? _("on") : _("off"));
  DISPLAY(_("  I/O address: 0x%03x\n"), io_address);
  DISPLAY(_("  Device name: %s\n"), io_device);
}


TIEXPORT int
TICALL ticable_get_support(int type)
{
  int support = SUPPORT_OFF;
  int test = 1;

#if defined(__LINUX__)
  test = !(method & IOM_DRV);
#elif defined(__WIN32__)
  test = 1;
#endif
  if (test) {
    switch (type) {
    case LINK_PAR:
      support = par_supported();
      break;
    case LINK_SER:
      support = ser_supported();
      break;
    case LINK_TGL:
      support = tig_supported();
      break;
    case LINK_AVR:
      support = avr_supported();
      break;
    case LINK_VTL:
      support = vtl_supported();
      break;
    case LINK_TIE:
      support = tie_supported();
      break;
    case LINK_VTI:
      support = vti_supported();
      break;
    case LINK_SLV:
      support = slv_supported();
      break;
    }
  } else
    support |= dev_supported();

  return support;
}


/*********************/
/* Utility functions */
/*********************/

extern const char *search_for_tipar_node(int minor);

static int convert_port_into_device(void)
{
  switch (port) {
  case USER_PORT:
    break;

#if !defined(__MACOSX__)
  case PARALLEL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tipar_node(0));
    else {
      io_address = PP1_ADDR;
      strcpy(io_device, PP1_NAME);
    }
    break;

  case PARALLEL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tipar_node(1));
    else {
      io_address = PP2_ADDR;
      strcpy(io_device, PP2_NAME);
    }
    break;

  case PARALLEL_PORT_3:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tipar_node(2));
    else {
      io_address = PP3_ADDR;
      strcpy(io_device, PP3_NAME);
    }
    break;

  case SERIAL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tiser_node(0));
    else {
      io_address = SP1_ADDR;
      strcpy(io_device, SP1_NAME);
    }
    break;

  case SERIAL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tiser_node(1));
    else {
      io_address = SP2_ADDR;
      strcpy(io_device, SP2_NAME);
    }
    break;

  case SERIAL_PORT_3:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tiser_node(2));
    else {
      io_address = SP3_ADDR;
      strcpy(io_device, SP3_NAME);
    }
    break;

  case SERIAL_PORT_4:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, search_for_tiser_node(3));
    else {
      io_address = SP4_ADDR;
      strcpy(io_device, SP4_NAME);
    }
    break;

  case USB_PORT_1:
#ifdef __LINUX__
    strcpy(io_device, search_for_tiusb_node(0));
#else
    strcpy(io_device, "none");
#endif
    break;

  case USB_PORT_2:
#ifdef __LINUX__
    strcpy(io_device, search_for_tiusb_node(1));
#else
    strcpy(io_device, "none");
#endif
    break;

  case USB_PORT_3:
#ifdef __LINUX__
    strcpy(io_device, search_for_tiusb_node(2));
#else
    strcpy(io_device, "none");
#endif
    break;

  case USB_PORT_4:
#ifdef __LINUX__
    strcpy(io_device, search_for_tiusb_node(3));
#else
    strcpy(io_device, "none");
#endif
    break;
#else
  case OSX_USB_PORT:
    strcpy(io_device, "");
    break;

  case OSX_SERIAL_PORT:
    break;
#endif				/* !__MACOSX__ */
  case VIRTUAL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, TIDEV_V0);
    else {
      io_address = VLINK0;
      strcpy(io_device, "");
    }
    break;

  case VIRTUAL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, TIDEV_V1);
    else {
      io_address = VLINK1;
      strcpy(io_device, "");
    }
    break;

  default:
    DISPLAY(_("libticables error: port value is illegal (%i) !\n"), port);
    DISPLAY(_("Exiting...\n"));
    //exit(1);
    break;
  }

  return 0;
}


/* Initialize i18n support under Win32 platforms */
/*
#if defined(__WIN32__)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,	// handle to DLL module
		    DWORD fdwReason,	// reason for calling function
		    LPVOID lpvReserved)	// reserved);
{
#ifdef ENABLE_NLS
		char buffer[65536];
		HINSTANCE hDLL = hinstDLL;
		int i=0;

		GetModuleFileName(hinstDLL, buffer, 65535);
		for(i=strlen(buffer); i>=0; i--) { 
			if(buffer[i]=='\\') break; 
		}
		buffer[i]='\0';
		strcat(buffer, "\\locale\\");

		setlocale(LC_ALL, "");
		bindtextdomain(PACKAGE, buffer);
        DISPLAY("%s: bindtextdomain to %s\n", PACKAGE, buffer);
		//bind_textdomain_codeset(PACKAGE, "ISO-8859-15");
		textdomain(PACKAGE);
#endif

		return TRUE;
}
#endif
*/
