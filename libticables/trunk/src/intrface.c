/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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
#if defined(__WIN32__)
# include <windows.h>
#endif
#ifdef __LINUX__
# include <unistd.h>
# include <sys/types.h>
#endif
#include "str.h"

#include "intl.h"
#include "cabl_ver.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "links.h"
#include "verbose.h"
#include "default.h"
#include "export.h"
#include "probe.h"

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

uint io_address = 0;         // I/O port base address
char io_device[MAXCHARS]=""; // The character device (COMx, ttySx, ...)

const char *err_msg;         // The error message of the last error occured
int cable_type;              // Used for debug

char *os;		     // Operating System type
TicablePortInfo pinfo;	     // Informations on I/O ports
TicableDataRate tdr;         // Data rate during transfers


/****************/
/* Entry points */
/****************/

/*
  This function should be the first one to call.
  It tries to list available I/O functions (I/O resources).
 */
int list_io_resources(void);
TIEXPORT int TICALL ticable_init()
{
  //ticable_detect_os(&os);
  //ticable_detect_port(&pinfo);
	list_io_resources();

  return 0;
}


/*
  This function should be called when the libticables library is
  no longer used.
 */
TIEXPORT int TICALL ticable_exit()
{
  return 0;
}


/***********/
/* Methods */
/***********/


TIEXPORT const char* TICALL ticable_get_version()
{
  return LIBTICABLES_VERSION;
}


TIEXPORT void TICALL ticable_set_timeout(int timeout_v)
{
  time_out=timeout_v;
}


TIEXPORT int TICALL ticable_get_timeout()
{
  return time_out;
}


TIEXPORT void TICALL ticable_set_delay(int delay_v)
{
  delay=delay_v;
}


TIEXPORT int TICALL ticable_get_delay()
{
  return delay;
}


TIEXPORT void TICALL ticable_set_baudrate(int br)
{
  baud_rate = br;
}


TIEXPORT int TICALL ticable_get_baudrate()
{
  return baud_rate;
}


TIEXPORT void TICALL ticable_set_io_address(uint io_addr)
{
  io_address = io_addr;
}


TIEXPORT uint TICALL ticable_get_io_address()
{
  return io_address;
}


TIEXPORT void TICALL ticable_set_io_device(char *dev)
{
  strcpy(io_device, dev);
}


TIEXPORT char* TICALL ticable_get_io_device()
{
  return io_device;
}


TIEXPORT void TICALL ticable_set_hfc(int action)
{
  hfc = action;
}


TIEXPORT int TICALL ticable_get_hfc(void)
{
  return hfc;
}


TIEXPORT void TICALL ticable_set_port(int p)
{
  port = p;
}


TIEXPORT int TICALL ticable_get_port(void)
{
  return port;
}


TIEXPORT void TICALL ticable_set_method(int m)
{
  method = m;
}


TIEXPORT int TICALL ticable_get_method(void)
{
  return method;
}

TIEXPORT int TICALL ticable_get_datarate(TicableDataRate **ptdr)
{
  *ptdr = &tdr;
  return tdr.count;
}

static int convert_port_into_device(void);

/*
  Set internal parameters starting at user configuration.
  Assign an I/O resources to use, too.
*/
TIEXPORT int TICALL ticable_set_param2(LinkParam lp)
{
  time_out = lp.timeout;
  delay = lp.delay;
  baud_rate = lp.baud_rate;
  hfc = lp.hfc;
  port = lp.port;
  method = lp.method;

  if((port == USER_PORT) || (port == OSX_SERIAL_PORT)) // force args
    {
      io_address = lp.io_addr;
      strcpy(io_device, lp.device);
    }

  return 0;
}


TIEXPORT int TICALL ticable_set_param(const LinkParam *lp)
{
  return ticable_set_param2(*lp);
}


TIEXPORT int TICALL ticable_get_param(LinkParam *lp)
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


TIEXPORT int TICALL ticable_get_default_param(LinkParam *lp)
{
  lp->calc_type = 2;          //CALC_TI92
#ifdef __MACOSX__
  lp->link_type = LINK_UGL;   // USB by default on Mac OS X
#else
  lp->link_type = LINK_TGL;   // does not change this (WinNT)
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


extern LinkCable *tcl;
static void print_informations();

/* HAVE_LIBINTL_H,ENABLE_NLS */
TIEXPORT int TICALL ticable_set_cable(int typ, LinkCable *lc)
{
  int type = typ;
  cable_type = type;

#ifdef ENABLE_NLS
#if !defined(__WIN32__)
  bindtextdomain(PACKAGE, NULL);
  textdomain(PACKAGE);
#endif
#endif

  // a void link cable
  set_default_cable(lc);

  // check for an usable I/O method to use and
  // determine the right I/O method starting at I/O resources and OS. 
  if(method & IOM_AUTO)
    method &= ~(IOM_ASM | IOM_DCB | IOM_DRV | IOM_OK);
  else
    method |= IOM_AUTO;
  
  if( (type == LINK_TGL) && (resources & IO_API) )
    method |= IOM_OK;
  else if ((type == LINK_TGL) && (resources & IO_OSX))
    method |= IOM_OK;

  if( (type == LINK_AVR) && (resources & IO_API) )
    method |= IOM_ASM | IOM_OK;

  if( (type == LINK_SER) && (resources & IO_TISER) )
    method |= IOM_DRV | IOM_OK;
  else if( (type == LINK_SER) && (resources & IO_ASM) )
    method |= IOM_ASM | IOM_OK;
  else if( (type == LINK_SER) && (resources & IO_DLL) )
    method |= IOM_DRV | IOM_OK;
  else if( (type == LINK_SER) && (resources & IO_DCB) )
    method |= IOM_DCB | IOM_OK;

  if( (type == LINK_PAR) && (resources & IO_TIPAR) )
    method |= IOM_DRV | IOM_OK;
  else if( (type == LINK_PAR) && (resources & IO_ASM) )
    method |= IOM_ASM | IOM_OK;
  else if( (type == LINK_PAR) && (resources & IO_DLL) )
    method |= IOM_DRV | IOM_OK;

  if( (type == LINK_UGL) && (resources & IO_TIUSB) )
    method |= IOM_DRV | IOM_OK;
  else if( (type == LINK_UGL) && (resources & IO_LIBUSB) )
    method |= IOM_RAW | IOM_OK;
  else if ((type == LINK_UGL) && (resources & IO_OSX))
      method |= IOM_OK;

  if((type == LINK_TIE) || (type == LINK_VTI) )
    method |= IOM_OK;

  //DISPLAY("Chosen method: %i\n", method);
  if(!(method & IOM_OK))
    {
      DISPLAY_ERROR("unable to find an I/O method.\n");
      return ERR_NO_RESOURCES;
    }

  // Fill device and io_addr fields
  convert_port_into_device();
  print_informations();

  // set the link cable
  if( ((resources & IO_LINUX) && !(method & IOM_DRV)) ||
      (resources & IO_WIN32) || (resources & IO_OSX))
    { // no kernel driver (such as tipar/tiser/tiusb)
      switch(type)
	{
#if !defined(__MACOSX__)
	case LINK_PAR: // IO_ASM, IO_DLL
	  if( (port != PARALLEL_PORT_1) && 
	      (port != PARALLEL_PORT_2) &&
	      (port != PARALLEL_PORT_3) &&
	      (port != USER_PORT))
	    return ERR_INVALID_PORT;
	  
	  lc->init  = par_init;
	  lc->open  = par_open;
	  lc->put   = par_put;
	  lc->get   = par_get;
	  lc->close = par_close;
	  lc->exit  = par_exit;
	  lc->probe = par_probe;
	  lc->check = par_check;

	  lc->set_red_wire   = par_set_red_wire;
	  lc->set_white_wire = par_set_white_wire;
	  lc->get_red_wire   = par_get_red_wire;
	  lc->get_white_wire = par_get_white_wire;
	  break;
	case LINK_SER: // IO_ASM, IO_DLL, IO_DCB
	  if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
	      (port != SERIAL_PORT_4) &&
	      (port != USER_PORT))
	    return ERR_INVALID_PORT;

	  if((resources & IO_LINUX) || !(method & IOM_DCB))
	    { // serial routines in IO mode (Linux & Win32)
	      lc->init  = ser_init;
	      lc->open  = ser_open;
	      lc->put   = ser_put;
	      lc->get   = ser_get;
	      lc->close = ser_close;
	      lc->exit  = ser_exit;
	      lc->probe = ser_probe;
	      lc->check = ser_check;
	      
	      lc->set_red_wire   = ser_set_red_wire;
	      lc->set_white_wire = ser_set_white_wire;
	      lc->get_red_wire   = ser_get_red_wire;
	      lc->get_white_wire = ser_get_white_wire;
	    }
	  else
	    { // serial routines in DCB mode (Win32 only)
#ifdef __WIN32__
	      lc->init  = ser_init2;
	      lc->open  = ser_open2;
	      lc->put   = ser_put2;
	      lc->get   = ser_get2;
	      lc->close = ser_close2;
	      lc->exit  = ser_exit2;
	      lc->probe = ser_probe2;
	      lc->check = ser_check2;
	      
	      lc->set_red_wire   = ser_set_red_wire2;
	      lc->set_white_wire = ser_set_white_wire2;
	      lc->get_red_wire   = ser_get_red_wire2;
	      lc->get_white_wire = ser_get_white_wire2;
#else
	      set_default_cable(lc);
#endif
	    }
	  break;
	case LINK_AVR: // IO_API
	  if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
              (port != SERIAL_PORT_4) &&
	      (port != USER_PORT))
	    return ERR_INVALID_PORT;

	  lc->init  = avr_init;
	  lc->open  = avr_open;
	  lc->put   = avr_put;
	  lc->get   = avr_get;
	  lc->close = avr_close;
	  lc->exit  = avr_exit;
	  lc->probe = avr_probe;
	  lc->check = avr_check;
	  break;
	case LINK_VTL:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
	    return ERR_INVALID_PORT;
	  
	  lc->init  = vtl_init;
	  lc->open  = vtl_open;
	  lc->put   = vtl_put;
	  lc->get   = vtl_get;
	  lc->close = vtl_close;
	  lc->exit  = vtl_exit;
	  lc->probe = vtl_probe;
	  lc->check = vtl_check;
	  break;
	case LINK_TIE:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
            return ERR_INVALID_PORT;

	  lc->init  = tie_init;
	  lc->open  = tie_open;
	  lc->put   = tie_put;
	  lc->get   = tie_get;
	  lc->close = tie_close;
	  lc->exit  = tie_exit;
	  lc->probe = tie_probe;
	  lc->check = tie_check;
	  break;
        case LINK_TPU:
          lc->init  = tpu_init;
          lc->open  = tpu_open;
          lc->put   = tpu_put;
          lc->get   = tpu_get;
          lc->close = tpu_close;
          lc->exit  = tpu_exit;
          lc->probe = tpu_probe;
          lc->check = tpu_check;
          break;
#endif /* !__MACOSX__ */
        case LINK_TGL: // IO_API
#ifndef __MACOSX__
          if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
              (port != SERIAL_PORT_4) &&
              (port != USER_PORT))
#else
          if (port != OSX_SERIAL_PORT)
#endif
            return ERR_INVALID_PORT;
          
          lc->init  = tig_init;
          lc->open  = tig_open;
          lc->put   = tig_put;
          lc->get   = tig_get;
          lc->close = tig_close;
          lc->exit  = tig_exit;
          lc->probe = tig_probe;
          lc->check = tig_check;
          break;
        case LINK_VTI:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
	    return ERR_INVALID_PORT;

	  lc->init  = vti_init;
	  lc->open  = vti_open;
	  lc->put   = vti_put;
	  lc->get   = vti_get;
	  lc->close = vti_close;
	  lc->exit  = vti_exit;
	  lc->probe = vti_probe;
	  lc->check = vti_check;
	  break;
	case LINK_UGL: // IO_LIBUSB or IO_MAC (IOKit)
#ifndef __MACOSX__
	  if( (port != USB_PORT_1) &&
              (port != USB_PORT_2) &&
              (port != USB_PORT_3) &&
              (port != USB_PORT_4) &&
              (port != USER_PORT))
#else
          if (port != OSX_USB_PORT)
#endif
            return ERR_INVALID_PORT;
#ifdef __LINUX__
	  lc->init  = ugl_init2;
	  lc->open  = ugl_open2;
	  lc->put   = ugl_put2;
	  lc->get   = ugl_get2;
	  lc->close = ugl_close2;
	  lc->exit  = ugl_exit2;
	  lc->probe = ugl_probe2;
	  lc->check = ugl_check2;
#else
	  lc->init  = ugl_init;
          lc->open  = ugl_open;
          lc->put   = ugl_put;
          lc->get   = ugl_get;
          lc->close = ugl_close;
          lc->exit  = ugl_exit;
          lc->probe = ugl_probe;
          lc->check = ugl_check;
#endif
	  break;
	default: // error !
	  DISPLAY(_("Function not implemented. This is a bug. Please report it."));
	  DISPLAY(_("Informations:\n"));
	  DISPLAY(_("Cable: %i\n"), type);
	  DISPLAY(_("Program halted before crashing...\n"));
	  exit(-1);
	  break;
	}
    }
  else
    { // kernel driver
#if defined(__LINUX__)
      switch(type)
        {
        case LINK_PAR: // IO_TIPAR
	case LINK_SER: // IO_TISER
	  if( (port != PARALLEL_PORT_1) &&
              (port != PARALLEL_PORT_2) &&
              (port != PARALLEL_PORT_3) &&
	      (port != SERIAL_PORT_1) &&
	      (port != SERIAL_PORT_2) &&
	      (port != SERIAL_PORT_3) &&
	      (port != SERIAL_PORT_4) &&
              (port != USER_PORT))
            return ERR_INVALID_PORT;

	  lc->init  = dev_init;
	  lc->open  = dev_open;
	  lc->put   = dev_put;
	  lc->get   = dev_get;
	  lc->close = dev_close;
	  lc->exit  = dev_exit;
	  lc->probe = dev_probe;
	  lc->check = dev_check;
	  break;
	case LINK_UGL: // IO_TIUSB, IO_LIBUSB
	  if( (port != USB_PORT_1) &&
              (port != USB_PORT_2) &&
              (port != USB_PORT_3) &&
              (port != USB_PORT_4) &&
              (port != USER_PORT))
            return ERR_INVALID_PORT;

          lc->init  = ugl_init;
          lc->open  = ugl_open;
          lc->put   = ugl_put;
          lc->get   = ugl_get;
          lc->close = ugl_close;
          lc->exit  = ugl_exit;
          lc->probe = ugl_probe;
          lc->check = ugl_check;
	  break;
	default: // error !
          DISPLAY(_("Function not implemented. This is a bug. Please report it."));
          DISPLAY(_("Informations:\n"));
          DISPLAY(_("Cable: %i\n"), type);
          DISPLAY(_("Program halted before crashing...\n"));
          exit(-1);
          break;
	}
#endif
    }
  tcl = lc; // for the error function
  
  return 0;
}

static char* convert_port[] = 
{
  "specified by user",
  "parallel port #1",
  "parallel port #2",
  "parallel port #3",
  "serial port #1",
  "serial port #2",
  "serial port #3",
  "serial port #4",
  "virtual port #1",
  "virtual port #2",
  "USB port #1",
  "USB port #2",
  "USB port #3",
  "USB port #4",
  "Handled through Mac OS X Preferences",
  "Handled through Mac OS X Preferences",
};

static char* convert_method(int v);

static void print_informations(void)
{
  DISPLAY(_("Libticables: current settings...\n"));
 
  switch(cable_type)
    {
    case LINK_PAR:
      DISPLAY(_("  Link cable type: parallel\n"));
      break;
    case LINK_SER:
      DISPLAY(_("  Link cable type: serial\n"));
      break;
    case LINK_TGL:
      DISPLAY(_("  Link cable type: Grey TIGL\n"));
      break;
    case LINK_AVR:
      DISPLAY(_("  Link cable type: fastAVRlink\n"));
      break;
    case LINK_VTL:
      DISPLAY(_("  Link cable type: generic virtual link\n"));
      break;
    case LINK_TIE:
      DISPLAY(_("  Link cable type: GtkTiEmu virtual link\n"));  
      break;
    case LINK_VTI:
      DISPLAY(_("  Link cable type: VTi virtual link\n"));
      break;
    case LINK_TPU:
      DISPLAY(_("  Link cable type: Ti/Pc USB link\n"));
      break;
    case LINK_UGL:
      DISPLAY(_("  Link cable type: SilverLink (USB Graph Link)\n"));
      break;
    default:
      DISPLAY(_("libticables error: unknown link cable.\n"));
      break;
    }

  DISPLAY(_("  Port: %s\n"), convert_port[port]);
  DISPLAY(_("  Method: %s\n"), convert_method(method));
  DISPLAY(_("  Timeout value: %i\n"), time_out);
  DISPLAY(_("  Delay value: %i\n"), delay);
  DISPLAY(_("  Baud-rate: %i\n"), baud_rate);
  DISPLAY(_("  Hardware flow control: %s\n"), hfc ? _("on") : _("off"));
  DISPLAY(_("  I/O address: 0x%03x\n"), io_address);
  DISPLAY(_("  Device name: %s\n"), io_device);
}


TIEXPORT int TICALL ticable_get_support(int type)
{
  int support = SUPPORT_OFF;
  int test = 1;

#if defined(__LINUX__)
  test = !(method & IOM_DRV);
#elif defined(__WIN32__)
  test = 1;
#endif
  if(test)
    {
      switch(type)
	{
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
	case LINK_TPU:
	  support = tpu_supported();
	  break;
	case LINK_UGL:
	  support = ugl_supported();
	  break;
	}
    }
  else
    {
      support |= dev_supported();
    }
  
  return support;
}

/* Initialize i18n support under Win32 platforms */
#if defined(__WIN32__)
BOOL WINAPI DllMain(  HINSTANCE hinstDLL,  // handle to DLL module
		      DWORD fdwReason,     // reason for calling function
		      LPVOID lpvReserved)   // reserved);
{
#ifdef ENABLE_NLS
  char buffer[65536];
  HINSTANCE hDLL = hinstDLL;
  int i=0;

  GetModuleFileName(hinstDLL, buffer, 65535);
  for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
  buffer[i]='\0';
  strcat(buffer, "\\locale\\");
  
  bindtextdomain (PACKAGE, buffer);
  //textdomain (PACKAGE);
#endif
}
#else
/*
_init()
{

}
*/
#endif

/*********************/
/* Utility functions */
/*********************/

static int convert_port_into_device(void)
{
  switch(port)
    {
    case USER_PORT:
      break;
#if !defined(__MACOSX__)
    case PARALLEL_PORT_1:
      if((method & IOM_DRV) && (resources & IO_LINUX))
	  strcpy(io_device, TIDEV_P0);
      else
	{
	  io_address = PP1_ADDR;
	  strcpy(io_device, PP1_NAME);
	}
      break;
    case PARALLEL_PORT_2:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_P1);
      else
        {
          io_address = PP2_ADDR;
          strcpy(io_device, PP2_NAME);
        }
      break;
    case PARALLEL_PORT_3:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_P2);
      else
        {
          io_address = PP3_ADDR;
          strcpy(io_device, PP3_NAME);
        }
      break;
    case SERIAL_PORT_1:
      if((method & IOM_DRV) && (resources & IO_LINUX))
	{
	  strcpy(io_device, TIDEV_S0);
	}     
      else
        {
          io_address = SP1_ADDR;
          strcpy(io_device, SP1_NAME);
        }
      break;
    case SERIAL_PORT_2:
      if((method & IOM_DRV) && (resources & IO_LINUX))
	{
	  strcpy(io_device, TIDEV_S1);
	}      
      else
        {
          io_address = SP2_ADDR;
          strcpy(io_device, SP2_NAME);
        }
      break;
    case SERIAL_PORT_3:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_S2);
      else
        {
          io_address = SP3_ADDR;
          strcpy(io_device, SP3_NAME);
        }
      break;
    case SERIAL_PORT_4:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_S3);
      else
        {
          io_address = SP4_ADDR;
          strcpy(io_device, SP4_NAME);
        }
      break;
    case USB_PORT_1:
      strcpy(io_device, UP1_NAME);
      break;
    case USB_PORT_2:
      strcpy(io_device, UP2_NAME);
      break;
    case USB_PORT_3:
      strcpy(io_device, UP3_NAME);
      break;
    case USB_PORT_4:
      strcpy(io_device, UP4_NAME);
      break;
#else
    case OSX_USB_PORT:
      strcpy(io_device, "");
      break;
    case OSX_SERIAL_PORT:
      break;
#endif /* !__MACOSX__ */
    case VIRTUAL_PORT_1:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_V0);
      else
        {
          io_address = VLINK0;
          strcpy(io_device, "");
        }
      break;
    case VIRTUAL_PORT_2:
      if((method & IOM_DRV) && (resources & IO_LINUX))
        strcpy(io_device, TIDEV_V1);
      else
        {
          io_address = VLINK1;
	    strcpy(io_device, "");
        }
      break;
    default:
      DISPLAY(_("libticables error: illegal argument !\n"));
      DISPLAY(_("Exiting...\n"));
      exit(1);
      break;
    }

  return 0;
}

static char* convert_method(int v)
{
  char *p1 = "";
  char *p2 = "internal";
  char *p3 = "";
  static char buffer[64]="";

  strcpy(buffer, "");

  //DISPLAY("method=%i\n", v);
  if(v & IOM_AUTO)
    p1 = "automatic";
  if(v & IOM_ASM)
    p2 = "internal ASM";
  if(v & IOM_RAW)
    p2 = "raw access";
  if(v & IOM_DCB)
    p3 = "DCB";
#if defined(__LINUX__)
  if(v & IOM_DRV)   
    p2 = "kernel module";
#elif defined(__WIN32__)
  if(v & IOM_DRV)
    p2 = "PortTalk driver";
#endif

  strcat(buffer, p1);
  strcat(buffer, " (");
  strcat(buffer, p2);
  if(strcmp(p2, "") && strcmp(p3, ""))
    strcat(buffer, " + ");
  strcat(buffer, p3);
  strcat(buffer, ")");

  return buffer;
}
