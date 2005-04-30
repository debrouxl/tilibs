/* Hey EMACS -*- macos-c -*- */
/* $Id$ */

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

/* Mac OSX resources mapping */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "../gettext.h"

#include "../cabl_def.h"
#include "../cabl_err.h"
#include "../externs.h"
#include "../type2str.h"
#include "../printl.h"

#include "macos_mapping.h"
#include "links.h"

int macos_get_method(TicableType type, int resources, TicableMethod *method)
{
	printl1(0, _("getting method from resources"));
	
	// reset method
	*method &= ~IOM_OK;
  	if (*method & IOM_AUTO) {
    		*method &= ~(IOM_ASM | IOM_API | IOM_DRV);
		printl1(0, _(" (automatic)...\n"));
  	} else
		printl1(0, _(" (user-forced)...\n"));

	// depending on link type, do some checks
	switch(type)
	{
	case LINK_TGL:
		if(resources & IO_API) {
			*method |= IOM_API | IOM_OK;
		}
		break;

	case LINK_AVR:
		if(resources & IO_API) {
			*method |= IOM_API | IOM_OK;	
		}
		break;

	case LINK_SLV:
		if (resources & IO_API) {
			*method |= IOM_API | IOM_OK;
		}
		break;

	default:
		printl1(2, "bad argument (invalid link cable).\n");
		return ERR_ILLEGAL_ARG;
		break;
	}
		
  	if (!(*method & IOM_OK)) {
    		printl1(2, "unable to find an I/O method.\n");
		return ERR_NO_RESOURCES;
	}
	
	return 0;
}

// Bind the right I/O address & device according to I/O method
static int macos_map_io(TicableMethod method, TicablePort port)
{
	printl1(0, _("mapping I/O...\n"));
	
	switch (port) {
  	case OSX_USB_PORT:
    		strcpy(io_device, "");
    	break;

  	case OSX_SERIAL_PORT:
    	break;

  	default:
    		printl1(2, "bad argument (invalid port).\n");
		return ERR_ILLEGAL_ARG;
	break;
	}
	
	return 0;
}


int macos_register_cable(TicableType type, TicableLinkCable *lc)
{
	int ret;

	// map I/O
	ret = macos_map_io((TicableMethod)method, port);
	if(ret)
		return ret;
	
	// set the link cable
	printl1(0, _("registering cable...\n"));
    	switch (type) {
    	case LINK_TGL:
    		if(port != OSX_SERIAL_PORT)
			return ERR_INVALID_PORT;

		tig_register_cable(lc);
		break;

    	/*case LINK_SLV:
      		if(port != OSX_USB_PORT)
			return ERR_INVALID_PORT;

		slv_register_cable(lc);
		break;*/

    	default:
	      	printl1(2, _("invalid argument (bad cable)."));
	      	return ERR_ILLEGAL_ARG;
		break;
    	}

	return 0;
}
