/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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

#include <string.h>
#include "gettext.h"
#include "ticables.h"

/**
 * ticables_model_to_string:
 * @model: a cable model.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "BlackLink".
 **/
TIEXPORT const char *TICALL ticables_model_to_string(CableModel model)
{
  	switch (model) 
	{
	case CABLE_NUL: return "null";
	case CABLE_GRY: return "GrayLink";
	case CABLE_BLK: return "BlackLink";
	case CABLE_PAR: return "Parallel";
	case CABLE_SLV: return "SilverLink";
	case CABLE_VTI: return "VTi";
	case CABLE_TIE: return "TiEmu";
	case CABLE_VTL: return "virtual";
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
TIEXPORT CableModel TICALL ticables_string_to_model(const char *str)
{
	if(!strcmp(str, "null"))
		return CABLE_NUL;
	else if(!strcmp(str, "GrayLink"))
		return CABLE_GRY;
	else if(!strcmp(str, "BlackLink"))
		return CABLE_BLK;
	else if(!strcmp(str, "Parallel"))
		return CABLE_PAR;
	else if(!strcmp(str, "SilverLink"))
		return CABLE_SLV;
	else if(!strcmp(str, "VTi"))
		return CABLE_VTI;
	else if(!strcmp(str, "TiEmu"))
		return CABLE_TIE;
	else if(!strcmp(str, "virtual"))
		return CABLE_VTL;

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
TIEXPORT const char *TICALL ticables_port_to_string(CablePort port)
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
TIEXPORT CablePort TICALL ticables_string_to_port(const char *str)
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
