/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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
#include <windows.h>
#endif
#include "str.h"

#include "intl.h"
#include "cabl_ver.h"
#include "cabl_def.h"
#include "links.h"
#include "verbose.h"
#include "default.h"

/*****************/
/* Internal data */
/*****************/
int time_out = DFLT_TIMEOUT; // Timeout value for cables in 0.10 seconds
int delay = DFLT_DELAY;      // Time between 2 bits (home-made cables only)
int baud_rate = BR9600;      // Baud rate setting for serial port
int hfc = HFC_ON;            // Hardware flow control for fastAVRlink
int port = PARALLEL_PORT_1;  // Use LPT1 by default
int method = IOM_AUTO;       // Automatic I/O method

uint io_address = 0;         // I/O port base address
char device[MAXCHARS]="";    // The character device (COMx, ttySx, ...)

const char *err_msg;         // The error message of the last error occured
int cable_type;              // Used for debug
int overriden = 0;           // If io_addr & device has been passed

#if defined(__LINUX__) || defined(__MACOSX__) /* not sure for MACOSX */
# define lx (1)
#elif defined(__WIN32__)
# define lx (0)
#endif

/***********/
/* Methods */
/***********/

DLLEXPORT
const char* DLLEXPORT2 ticable_get_version()
{
  return LIBTICABLES_VERSION;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_timeout(int timeout_v)
{
  time_out=timeout_v;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_timeout()
{
  return time_out;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_delay(int delay_v)
{
  delay=delay_v;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_delay()
{
  return delay;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_baudrate(int br)
{
  baud_rate = br;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_baudrate()
{
  return baud_rate;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_io_address(uint io_addr)
{
  io_address = io_addr;
}

DLLEXPORT uint DLLEXPORT2 
ticable_get_io_address()
{
  return io_address;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_io_device(char *dev)
{
  strcpy(device, dev);
}

DLLEXPORT
char* DLLEXPORT2 ticable_get_io_device()
{
  return device;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_hfc(int action)
{
  hfc = action;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_hfc(void)
{
  return hfc;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_port(int p)
{
  port = p;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_port(void)
{
  return port;
}

DLLEXPORT
void DLLEXPORT2 ticable_set_method(int m)
{
  method = m;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_emthod(void)
{
  return method;
}

static int convert_port_into_device(LinkParam lp);

DLLEXPORT
void DLLEXPORT2 ticable_set_param(LINK_PARAM lp)
{
  time_out = lp.timeout;
  delay = lp.delay;
  baud_rate = lp.baud_rate;
  convert_port_into_device(lp);
  hfc = lp.hfc;

  port = lp.port;
  method = lp.method;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_param(LINK_PARAM *lp)
{
  lp->timeout = time_out;
  lp->delay = delay;
  lp->baud_rate = baud_rate;
  lp->hfc = hfc;

  lp->io_addr = io_address;
  strcpy(lp->device, device);

  lp->port = port;
  lp->method = method;

  return 0;
}

DLLEXPORT
int DLLEXPORT2 ticable_get_default_param(LINK_PARAM *lp)
{
  lp->calc_type = 2;   //CALC_TI92
  lp->link_type = LINK_PAR;
  lp->timeout = DFLT_TIMEOUT;
  lp->delay = DFLT_DELAY;
  lp->baud_rate = BR9600;
  lp->io_addr = AUTO_ADDR;
  strcpy(lp->device, AUTO_NAME);
  lp->hfc = HFC_ON;

  lp->port = PARALLEL_PORT_1;
  lp->method = IOM_AUTO;

  return 0;
}

extern LINK_CABLE *tcl;
static void print_informations();

DLLEXPORT
void DLLEXPORT2 ticable_set_cable(int typ, LINK_CABLE *lc)
{
  int type = typ;
  int test = 1;
  cable_type = type;

#ifdef ENABLE_NLS
#if !defined(__WIN32__)
  bindtextdomain(PACKAGE, NULL);
  textdomain(PACKAGE);
#endif
#endif

  print_informations();
  set_default_cable(lc);

#if defined(__LINUX__)
  test = !(method & IOM_DRV);
#elif defined(__WIN32__)
  test = 1;
#endif
  if(test)
    { // no kernel driver (tipar/tiser)
      switch(type)
	{
#if !defined(__MACOSX__)
	case LINK_PAR:
	  if( (port != PARALLEL_PORT_1) && 
	      (port != PARALLEL_PORT_2) &&
	      (port != PARALLEL_PORT_3))
	    DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = par_init_port;
	  lc->open_port  = par_open_port;
	  lc->put        = par_put;
	  lc->get        = par_get;
	  lc->close_port = par_close_port;
	  lc->term_port  = par_term_port;
	  lc->probe_port = par_probe_port;
	  lc->check_port = par_check_port;

	  lc->set_red_wire   = par_set_red_wire;
	  lc->set_white_wire = par_set_white_wire;
	  lc->get_red_wire   = par_get_red_wire;
	  lc->get_white_wire = par_get_white_wire;
	  break;
	case LINK_SER:
	  if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
	      (port != SERIAL_PORT_4))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  if(lx || (method != IOM_DCB))
	    { // serial routines in IO mode (Linux & Win32)
	      lc->init_port  = ser_init_port;
	      lc->open_port  = ser_open_port;
	      lc->put        = ser_put;
	      lc->get        = ser_get;
	      lc->close_port = ser_close_port;
	      lc->term_port  = ser_term_port;
	      lc->probe_port = ser_probe_port;
	      lc->check_port = ser_check_port;
	      
	      lc->set_red_wire   = ser_set_red_wire;
	      lc->set_white_wire = ser_set_white_wire;
	      lc->get_red_wire   = ser_get_red_wire;
	      lc->get_white_wire = ser_get_white_wire;
	    }
	  else
	    { // serial routines in DCB mode (Win32 only)
	      lc->init_port  = ser_init_port2;
	      lc->open_port  = ser_open_port2;
	      lc->put        = ser_put2;
	      lc->get        = ser_get2;
	      lc->close_port = ser_close_port2;
	      lc->term_port  = ser_term_port2;
	      lc->probe_port = ser_probe_port2;
	      lc->check_port = ser_check_port2;
	      
	      lc->set_red_wire   = ser_set_red_wire2;
	      lc->set_white_wire = ser_set_white_wire2;
	      lc->get_red_wire   = ser_get_red_wire2;
	      lc->get_white_wire = ser_get_white_wire2;
	    }
	  break;
	case LINK_TGL:
	  if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
              (port != SERIAL_PORT_4))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = tig_init_port;
	  lc->open_port  = tig_open_port;
	  lc->put        = tig_put;
	  lc->get        = tig_get;
	  lc->close_port = tig_close_port;
	  lc->term_port  = tig_term_port;
	  lc->probe_port = tig_probe_port;
	  lc->check_port = tig_check_port;
	  break;
	case LINK_AVR:
	  if( (port != SERIAL_PORT_1) &&
              (port != SERIAL_PORT_2) &&
              (port != SERIAL_PORT_3) &&
              (port != SERIAL_PORT_4))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = avr_init_port;
	  lc->open_port  = avr_open_port;
	  lc->put        = avr_put;
	  lc->get        = avr_get;
	  lc->close_port = avr_close_port;
	  lc->term_port  = avr_term_port;
	  lc->probe_port = avr_probe_port;
	  lc->check_port = avr_check_port;
	  break;
	case LINK_VTL:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = vtl_init_port;
	  lc->open_port  = vtl_open_port;
	  lc->put        = vtl_put;
	  lc->get        = vtl_get;
	  lc->close_port = vtl_close_port;
	  lc->term_port  = vtl_term_port;
	  lc->probe_port = vtl_probe_port;
	  lc->check_port = vtl_check_port;
	  break;
	case LINK_TIE:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = tie_init_port;
	  lc->open_port  = tie_open_port;
	  lc->put        = tie_put;
	  lc->get        = tie_get;
	  lc->close_port = tie_close_port;
	  lc->term_port  = tie_term_port;
	  lc->probe_port = tie_probe_port;
	  lc->check_port = tie_check_port;
	  break;
        case LINK_TPU:
	  lc->init_port  = tpu_init_port;
	  lc->open_port  = tpu_open_port;
	  lc->put        = tpu_put;
	  lc->get        = tpu_get;
	  lc->close_port = tpu_close_port;
	  lc->term_port  = tpu_term_port;
	  lc->probe_port = tpu_probe_port;
	  lc->check_port = tpu_check_port;
	  break;

#endif /* !__MACOSX__ */
	case LINK_VTI:
	  if( (port != VIRTUAL_PORT_1) &&
              (port != VIRTUAL_PORT_2))
            DISPLAY(_("libticables error: port incompatible with cable.\n"));

	  lc->init_port  = vti_init_port;
	  lc->open_port  = vti_open_port;
	  lc->put        = vti_put;
	  lc->get        = vti_get;
	  lc->close_port = vti_close_port;
	  lc->term_port  = vti_term_port;
	  lc->probe_port = vti_probe_port;
	  lc->check_port = vti_check_port;
	  break;
        case LINK_UGL:
	  lc->init_port  = ugl_init_port;
	  lc->open_port  = ugl_open_port;
	  lc->put        = ugl_put;
	  lc->get        = ugl_get;
	  lc->close_port = ugl_close_port;
	  lc->term_port  = ugl_term_port;
	  lc->probe_port = ugl_probe_port;
	  lc->check_port = ugl_check_port;
	  break;
	default: // error !
	  fprintf(stderr, "Function not implemented. This is a bug. Please report it.");
	  fprintf(stderr, "Informations:\n");
	  fprintf(stderr, "Cable: %i\n", type);
	  fprintf(stderr, "Program halted before crashing...\n");
	  exit(-1);
	  break;
	}
    }
  else
    { // kernel driver
#if defined(__LINUX__)
      lc->init_port  = dev_init_port;
      lc->open_port  = dev_open_port;
      lc->put        = dev_put;
      lc->get        = dev_get;
      lc->close_port = dev_close_port;
      lc->term_port  = dev_term_port;
      lc->probe_port = dev_probe_port;
      lc->check_port = dev_check_port;
#endif
    }
  tcl = lc;	// for the error function
}

static char* convert_port[] = {
  "",
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
};

static char* convert_method(int v);

static void print_informations(void)
{
  DISPLAY(_("Libticables settings...\n"));
 
  switch(cable_type & ~LINK_DEV)
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
      DISPLAY(_("  Link cable type: Blue USB Graph Link\n"));
      break;
    default: // error !
      DISPLAY(_("libticables error: unknown link cable.\n"));
      break;
    }

  if(overriden)
    DISPLAY(_("  Port: I/O address & device forced (see below)\n"));
  else
    DISPLAY(_("  Port: %s\n"), convert_port[port]);
  DISPLAY(_("  Method: %s\n"), convert_method(method));
  DISPLAY(_("  Timeout value: %i\n"), time_out);
  DISPLAY(_("  Delay value: %i\n"), delay);
  DISPLAY(_("  Baud-rate: %i\n"), baud_rate);
  DISPLAY(_("  Hardware flow control: %s\n"), hfc ? _("on") : _("off"));
  DISPLAY(_("   I/O address: 0x%03x\n"), io_address);
  DISPLAY(_("   Device name: %s\n"), device);
}

DLLEXPORT
int DLLEXPORT2 ticable_get_support(int cable_type)
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
      switch(cable_type)
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
  //DISPLAY("libticables, filename: <%s>\n", buffer);
  
  for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
  buffer[i]='\0';
  strcat(buffer, "\\locale\\");
  //DISPLAY("libticables, locale_dir: <%s>\n", buffer);
  
  bindtextdomain (PACKAGE, buffer);	
  textdomain (PACKAGE);
#endif
}
#endif

/*********************/
/* Utility functions */
/*********************/

static int convert_port_into_device(LinkParam lp)
{
  /* 
     When io_addr or device is passed, we get them.
     Either user want force some parameters, either he wants use an old
     library version so we maitain compatability.
  */
  if( (lp.io_addr != 0) || strcmp(lp.device, "") ) // force args
    {
      overriden = 1;
      io_address = lp.io_addr;
      strcpy(device, lp.device);
      return 1;
    }
  else
    overriden = 0;

  switch(lp.port)
    {
#if !defined(__MACOSX__)
    case PARALLEL_PORT_1:
      if((lp.method & IOM_DRV) && lx)
	strcpy(device, TIDEV_P0);
      else
	{
	  io_address = PP1_ADDR;
	  strcpy(device, PP1_NAME);
	}
      break;
    case PARALLEL_PORT_2:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_P1);
      else
        {
          io_address = PP2_ADDR;
          strcpy(device, PP2_NAME);
        }
      break;
    case PARALLEL_PORT_3:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_P2);
      else
        {
          io_address = PP3_ADDR;
          strcpy(device, PP3_NAME);
        }
      break;
    case SERIAL_PORT_1:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_S0);
      else
        {
          io_address = SP1_ADDR;
          strcpy(device, SP1_NAME);
        }
      break;
    case SERIAL_PORT_2:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_S1);
      else
        {
          io_address = SP2_ADDR;
          strcpy(device, SP2_NAME);
        }
      break;
    case SERIAL_PORT_3:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_S2);
      else
        {
          io_address = SP3_ADDR;
          strcpy(device, SP3_NAME);
        }
      break;
    case SERIAL_PORT_4:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_S3);
      else
        {
          io_address = SP4_ADDR;
          strcpy(device, SP4_NAME);
        }
      break;
#endif /* !__MACOSX__ */
    case VIRTUAL_PORT_1:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_V0);
      else
        {
          io_address = VLINK0;
          strcpy(device, "");
        }
      break;
    case VIRTUAL_PORT_2:
      if((lp.method & IOM_DRV) && lx)
        strcpy(device, TIDEV_V1);
      else
        {
          io_address = VLINK1;
	    strcpy(device, "");
        }
      break;
    case USB_PORT_1:
      strcpy(device, UP1_NAME);
      break;
    case USB_PORT_2:
      strcpy(device, UP2_NAME);
      break;
    case USB_PORT_3:
      strcpy(device, UP3_NAME);
      break;
    case USB_PORT_4:
      strcpy(device, UP4_NAME);
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
  char *p2 = "";
  static char buffer[64]="";

  strcpy(buffer, "");

  if(v == IOM_AUTO)
    p1 = "automatic";
  else if(v & IOM_ASM)
    p1 = "internal ASM";
  else if(v & IOM_DCB)
    p2 = "DCB";
  else if(v & IOM_DRV)
#if defined(__LINUX__)
    p1 = "kernel module";
#elif defined(__WIN32__)
  p1 = "DLPortIO driver";
#endif

  strcat(buffer, p1);
  if(strcmp(p1, "") && strcmp(p2, ""))
    strcat(buffer, " + ");
  strcat(buffer, p2);

  return buffer;
}
