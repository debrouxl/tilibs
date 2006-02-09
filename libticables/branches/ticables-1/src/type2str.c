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
#include <string.h>

#include "gettext.h"
#include "cabl_def.h"

static const char *TICABLETYPE[TICABLETYPE_MAX] = {
  N_("none"), N_("GrayLink"), N_("BlackLink"), N_("ParallelLink"), N_("AVRlink"), N_("virtual"), N_("TiEMu"),
  N_("VTi"), N_("obsolete"), N_("SilverLink"),
};

TIEXPORT const char *TICALL ticable_cabletype_to_string(TicableType type)
{	
	int v;

	if (type < TICABLETYPE_MAX)
    		v = type;
  	else
    		v = 0;

  	return TICABLETYPE[v];
}


TIEXPORT TicableType TICALL ticable_string_to_cabletype(const char *str)
{
  	int i;

  	for (i = 0; i < TICABLETYPE_MAX; i++) {
    		if (!strcmp(TICABLETYPE[i], str))
      			break;
  	}
  
  	if (i == TICABLETYPE_MAX)
    		return 0;

  	return i;
}


TIEXPORT const char *TICALL ticable_baudrate_to_string(TicableBaudRate br)
{
  	switch (br) {
  	case BR9600:  return "9600 bauds";
  	case BR19200: return "19200 bauds";
  	case BR38400: return "38400 bauds";
  	case BR57600: return "57600 bauds";
  	default: return "unknown";
  	}
}


TIEXPORT TicableBaudRate TICALL ticable_string_to_baudrate(const char *str)
{
	if(!strcmp(str, "9600 bauds"))
		return BR9600;
  	else if(!strcmp(str, "19200 bauds"))
  		return BR19200;
  	else if(!strcmp(str, "38400 bauds"))
  		return BR38400;
  	else if(!strcmp(str, "57600 bauds"))
  		return BR57600;

  		return BR9600;
}


TIEXPORT const char *TICALL ticable_hfc_to_string(TicableHfc hfc)
{
	if(hfc == HFC_ON)
		return _("on");
	else
		return _("off");
}


TIEXPORT TicableHfc TICALL ticable_string_to_hfc(const char *str)
{
	if(!strcmp(str, _("on")))
		return HFC_ON;
	else
		return HFC_OFF;
}

static const char *TICABLEPORT[TICABLEPORT_MAX] = {
  N_("custom"), 
  N_("parallel port #1"), N_("parallel port #2"), N_("parallel port #3"), 
  N_("serial port #1"), N_("serial port #2"), 
  N_("serial port #3"), N_("serial port #4"), 
  N_("virtual port #1"), N_("virtual port #2"), 
  N_("USB port #1"), N_("USB port #2"), N_("USB port #3"), N_("USB port #4"), 
  N_("serial port"), N_("USB port"), 
  N_("null"),
};

TIEXPORT const char *TICALL ticable_port_to_string(TicablePort port)
{
	int v;

	if (port < TICABLEPORT_MAX)
    		v = port;
  	else
    		v = 0;

  	return TICABLEPORT[v];
}

TIEXPORT TicablePort TICALL ticable_string_to_port(const char *str)
{
	int i;

  	for (i = 0; i < TICABLEPORT_MAX; i++) {
    		if (!strcmp(TICABLEPORT[i], str))
      			break;
  	}
  
  	if (i == TICABLEPORT_MAX)
    		return 0;

  	return i;
}


TIEXPORT 
const char *TICALL ticable_method_to_string(TicableMethod method)
{
	static char buffer[33];

	strcpy(buffer, _("unknown"));
	
	if (method & IOM_ASM)
		strcpy(buffer, _("direct access (asm)"));
	if (method & IOM_API)
		strcpy(buffer, _("direct access (api)"));
	if (method & IOM_DRV)
		strcpy(buffer, _("kernel mode (module)"));
	if (method & IOM_IOCTL)
		strcpy(buffer, _("user mode (ioctl)"));
	if (method & IOM_NULL)
		strcpy(buffer, _("null"));

	return buffer;
}


TIEXPORT const char *TICALL ticable_display_to_string(TicableDisplay disp)
{
	if(disp == DSP_OFF)
		return _("off");
	else if(disp == DSP_ON)
		return _("on");
	else if(disp == DSP_CLOSE)
		return _("closed");

	return _("off");
}


TIEXPORT TicableDisplay TICALL ticable_string_to_display(const char *str)
{
	if(!strcmp(str, _("on")))
		return DSP_ON;
	else if(!strcmp(str, _("off")))
		return DSP_OFF;
	else if(!strcmp(str, _("closed")))
		return DSP_CLOSE;

	return DSP_OFF;
}
