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

#include <string.h>
#include "gettext.h"
#include "ticables.h"

TIEXPORT const char *TICALL ticables_model_to_string(TiCableModel model)
{
  	switch (model) 
	{
	case CABLE_NUL: return "null";
	case CABLE_TGL: return "GrayLink";
	case CABLE_SER: return "BlackLink";
	case CABLE_PAR: return "Parallel";
	case CABLE_SLV: return "SilverLink";
	case CABLE_VTI: return "Virtual TI";
	case CABLE_TIE: return "TiEmu";
	case CABLE_VTL: return "virtual";
	default: return "unknown";
	}
}

TIEXPORT TiCableModel TICALL ticables_string_to_model(const char *str)
{
	if(!strcmp(str, "null"))
		return CABLE_NUL;
	else if(!strcmp(str, "GrayLink"))
		return CABLE_TGL;
	else if(!strcmp(str, "BlackLink"))
		return CABLE_SER;
	else if(!strcmp(str, "Parallel"))
		return CABLE_PAR;
	else if(!strcmp(str, "SilverLink"))
		return CABLE_SLV;
	else if(!strcmp(str, "Virtual TI"))
		return CABLE_VTI;
	else if(!strcmp(str, "TiEmu"))
		return CABLE_TIE;
	else if(!strcmp(str, "virtual"))
		return CABLE_VTL;

	return CABLE_NUL;
}

TIEXPORT const char *TICALL ticables_port_to_string(TiCablePort port)
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

TIEXPORT TiCablePort TICALL ticables_string_to_port(const char *str)
{
	if(!strcmp(str, "null"))
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

TIEXPORT const char *TICALL ticables_method_to_string(TiCableMethod method)
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

TIEXPORT const char *TICALL ticables_os_to_string(TiCableOs port)
{
  	switch (port) 
	{
	case OS_WIN9X: return "Windows 9x";
	case OS_WINNT: return "Windows NT";
	case OS_LINUX: return "Linux";
	case OS_MACOS: return "Mac OS-X";
	case OS_BSD: return "*BSD";
	default: return "unknown";	
	}
}

TIEXPORT TiCableOs TICALL ticables_string_to_os(const char *str)
{
	if(!strcmp(str, "Windows 9x"))
		return OS_WIN9X;
	else if(!strcmp(str, "Windows NT"))
		return OS_WINNT;
	else if(!strcmp(str, "Linux"))
		return OS_LINUX;
	else if(!strcmp(str, "Mac OS-X"))
		return OS_MACOS;
	else if(!strcmp(str, "*BSD"))
		return OS_BSD;

	return OS_NONE;
}
