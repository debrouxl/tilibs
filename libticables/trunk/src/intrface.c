/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(__WIN32__)
#include <windows.h>
#endif

#include "gettext.h"

#include "cabl_ver.h"
#include "cabl_def.h"
#include "detect.h"
#include "export.h"
#include "mapping.h"
#include "type2str.h"
#include "printl.h"


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
int resources = IO_NONE;     // I/O resources detected
TicableMethod method = IOM_AUTO;      // I/O methods useable

unsigned int io_address = 0; // I/O port base address
char io_device[256] = "";    // The character device (COMx, ttySx, ...)

const char *err_msg;         // The error message of the last error occured
int cable_type;              // Used for debug

TicableDataRate tdr;         // Data rate during transfers
TicablePortInfo pi;	     // I/O ports detected

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticables_instance = 0;	// counts # of instances

/*
	This function should be the first one to call.
  	It tries to list available I/O resources for later use.
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
	if (ticables_instance)
		return (++ticables_instance);
	printl1(0, _("ticables library version %s\n"), LIBTICABLES_VERSION);
  	errno = 0;

#if defined(ENABLE_NLS)
  	printl1(0, "setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	printl1(0, "bindtextdomain: <%s>\n", bindtextdomain(PACKAGE, LOCALEDIR));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	printl1(0, "textdomain: <%s>\n", textdomain(PACKAGE));
#endif

	// list built-in compiled options...
	printl1(0, _("built for %s target.\n"), 
#if defined(__LINUX__)
		"__LINUX__"
#elif defined(__BSD__)
		"__BSD__"
#elif defined(__MACOSX__)
		"__MACOSX__"
#elif defined(__MINGW32__)
		"__MINGW32__"
#elif defined(__CYGWIN__)
		"__CYGWIN__"
#elif defined(__WIN32__)
		"__WIN32__"
#else
		"not listed"
#endif
		);

	// get available I/O resources
  	detect_resources();

	// list I/O ports
	ticable_detect_port(&pi);

  	return (++ticables_instance);
}


/*
  	This function should be called when the libticables library is
  	no longer used.
 */
TIEXPORT int
TICALL ticable_exit()
{
  	return (--ticables_instance);
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

static void print_settings(void)
{
  	printl1(0, _("list of settings:\n"));
  	
  	printl1(0, _("  cable: %s\n"), 
		ticable_cabletype_to_string(cable_type));
  	
  	printl1(0, _("  port: %s\n"), ticable_port_to_string(port));
  	
  	printl1(0, _("  method: %s\n"), ticable_method_to_string(method));
  	  	
  	if(cable_type == LINK_AVR) {
		printl1(0, _("  baud-rate: %i\n"), baud_rate);
		printl1(0, _("  hardware flow control: %s\n"), 
			hfc ? _("on") : _("off"));
	}

  	if((cable_type == LINK_PAR) || (cable_type == LINK_SER))
		if(io_address != 0x000)
			printl1(0, _("  address: 0x%03x\n"), io_address);
	
	printl1(0, _("  device name: %s\n"), io_device);

	if((cable_type == LINK_PAR) || (cable_type == LINK_SER))
                printl1(0, _("  timeout value: %i\n"), time_out);

        printl1(0, _("  delay value: %i\n"), delay);
}


TIEXPORT int
TICALL ticable_set_cable(int type, TicableLinkCable * lc)
{
	int ret;

	// remove link cable
	mapping_unregister_cable(lc);
	cable_type = type;

	// compile informations (I/O resources & OS platform) in order to 
  	// determine the best I/O method to use.
	ret = mapping_get_method(type, resources, &method);
	if(ret)
		return ret;

  	// set the link cable
  	ret = mapping_register_cable(type, lc);
  	if(ret)
		return ret;

  	// displays useful infos
  	print_settings();
  	
  	// close connection on error
  	tcl = lc;

  	return 0;
}


TIEXPORT int
TICALL ticable_get_support(int type)
{
/*
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
 */
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
        	printl1(0, "%s: bindtextdomain to %s\n", PACKAGE, buffer);
		//bind_textdomain_codeset(PACKAGE, "ISO-8859-15");
		textdomain(PACKAGE);
#endif

		return TRUE;
}
#endif
*/
