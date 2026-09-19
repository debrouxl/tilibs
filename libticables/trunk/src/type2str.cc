/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <glib.h>

#include "gettext.h"
#include "ticables.h"
#include "logging.h"

/**
 * ticables_model_to_string:
 * @model: a cable model.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "BlackLink".
 **/
const char *TICALL ticables_model_to_string(CableModel model)
{
  	switch (model) 
	{
	case CABLE_NUL: return "null";
	case CABLE_GRY: return "GrayLink";
	case CABLE_BLK: return "BlackLink";
	case CABLE_PAR: return "Parallel";
	case CABLE_SLV: return "SilverLink";
	case CABLE_USB: return "DirectLink";
	case CABLE_VTI: return "VTi";
	case CABLE_TIE: return "TiEmu";
	case CABLE_ILP: return "linkport";
	//case CABLE_DEV: return "UsbKernel";
	case CABLE_TCPC: return "TCPC";
	case CABLE_TCPS: return "TCPS";
	default: return "unknown";
	}
}

/**
 * ticables_string_to_model:
 * @str: a cable model as string like "BlackLink".
 *
 * Do a string to integer conversion.
 *
 * Return value: a cable model.
 **/
CableModel TICALL ticables_string_to_model(const char *str)
{
	if (str == NULL)
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return CABLE_NUL;
	}

	/*if (!g_ascii_strcasecmp(str, "null") || !g_ascii_strcasecmp(str, "nul"))
		return CABLE_NUL;
	else*/ if (!g_ascii_strcasecmp(str, "GrayLink") || !g_ascii_strcasecmp(str, "gry"))
		return CABLE_GRY;
	else if (!g_ascii_strcasecmp(str, "BlackLink") || !g_ascii_strcasecmp(str, "blk") || !g_ascii_strcasecmp(str, "ser"))
		return CABLE_BLK;
	else if (!g_ascii_strcasecmp(str, "Parallel") || !g_ascii_strcasecmp(str, "par"))
		return CABLE_PAR;
	else if (!g_ascii_strcasecmp(str, "SilverLink") || !g_ascii_strcasecmp(str, "slv"))
		return CABLE_SLV;
	else if (!g_ascii_strcasecmp(str, "DirectLink") || !g_ascii_strcasecmp(str, "usb"))
		return CABLE_USB;
	else if (!g_ascii_strcasecmp(str, "VTi"))
		return CABLE_VTI;
	else if (   !g_ascii_strcasecmp(str, "TiEmu") || !g_ascii_strcasecmp(str, "tie")
	         || !g_ascii_strcasecmp(str, "TilEm") || !g_ascii_strcasecmp(str, "TiEmu/TilEm"))
		return CABLE_TIE;
	else if (!g_ascii_strcasecmp(str, "linkport"))
		return CABLE_ILP;
	else if (!g_ascii_strcasecmp(str, "tcpc") || !g_ascii_strcasecmp(str, "tcpclient"))
		return CABLE_TCPC;
	else if (!g_ascii_strcasecmp(str, "tcps") || !g_ascii_strcasecmp(str, "tcpserver"))
		return CABLE_TCPS;
	// else fall through.

	return CABLE_NUL;
}

/**
 * ticables_port_to_string:
 * @port: a link port.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "#1".
 **/
const char *TICALL ticables_port_to_string(CablePort port)
{
	switch (port)
	{
	case PORT_0: return "null";
	case PORT_1: return "#1";
	case PORT_2: return "#2";
	case PORT_3: return "#3";
	case PORT_4: return "#4";
	default: return "unknown";
	}
}

/**
 * ticables_string_to_port:
 * @str: a link port as string like "#1".
 *
 * Do a string to integer conversion.
 *
 * Return value: a link port.
 **/
CablePort TICALL ticables_string_to_port(const char *str)
{
	if (str == NULL)
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return PORT_0;
	}

	if(!g_ascii_strcasecmp(str, "null"))
		return PORT_0;
	else if(!strcmp(str, "#1"))
		return PORT_1;
	else if(!strcmp(str, "#2"))
		return PORT_2;
	else if(!strcmp(str, "#3"))
		return PORT_3;
	else if(!strcmp(str, "#4"))
		return PORT_4;

	return PORT_0;
}

/**
 * ticables_usbpid_to_string:
 * @pid: a Usb Product Id.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "SilverLink".
 **/
const char *TICALL ticables_usbpid_to_string(UsbPid pid)
{
	switch (pid)
	{
	case PID_TIGLUSB: return "SilverLink";	// must match ticables name
	case PID_TI89TM: return "TI89t";		// must match tifiles name
	case PID_TI84P: return "TI84+";			// must match tifiles name
	case PID_TI84P_SE: return "TI84+";		// remap
	case PID_NSPIRE: return "Nspire";		// must match tifiles name
	case PID_NSPIRE_CRADLE: return "Nspire Cradle";		// must match tifiles name
	case PID_NSPIRE_CXII: return "Nspire CX II";		// must match tifiles name
	default: return "unknown";
	}
}

/**
 * ticables_usbpid_to_port:
 * @str: a Usb Product Id as string like "SilverLink".
 *
 * Do a string to integer conversion.
 *
 * Return value: a Usb Product Id.
 **/
UsbPid TICALL ticables_string_to_usbpid(const char *str)
{
	if (str == NULL)
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
		return PID_UNKNOWN;
	}

	if(!strcmp(str, "SilverLink"))
		return PID_TIGLUSB;
	else if(!strcmp(str, "TI89t"))
		return PID_TI89TM;
	else if(!strcmp(str, "TI84+"))
		return PID_TI84P;
	else if(!strcmp(str, "TI84+SE"))
		return PID_TI84P_SE;
	else if(!strcmp(str, "Nspire Cradle"))
		return PID_NSPIRE_CRADLE;
	else if(!strcmp(str, "Nspire CX II"))
		return PID_NSPIRE_CXII;
	else if(!strncmp(str, "Nspire", 6) || !strncmp(str, "NSpire", 6))
		return PID_NSPIRE;

	return PID_UNKNOWN;
}
