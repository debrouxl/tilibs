/* Hey EMACS -*- win32-c -*- */
/* $Id: linux_mapping.c 399 2004-03-29 19:50:38Z roms $ */

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

/* Linux resources mapping */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "../intl.h"

#include "../cabl_def.h"
#include "../cabl_err.h"
#include "../externs.h"
#include "../type2str.h"
#include "../verbose.h"

#include "links.h"

int win32_get_method(TicableType type, int resources, TicableMethod *method)
{
	DISPLAY(_("libticables: getting method from resources"));
	
	// reset method
	*method &= ~IOM_OK;
  	if (*method & IOM_AUTO) {
    		*method &= ~(IOM_ASM | IOM_API | IOM_DRV | IOM_IOCTL);
		DISPLAY(_(" (automatic)...\n"));
  	} else
		DISPLAY(_(" (user-forced)...\n"));

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

	case LINK_SER:
		if(resources & IO_DLL) {
			*method |= IOM_DRV | IOM_OK;
		}
		       
		if (resources & IO_ASM) {
			*method |= IOM_ASM | IOM_OK;
		}
		
		if (resources & IO_API) {
			*method |= IOM_IOCTL | IOM_OK;
		}
		break;

	case LINK_PAR:
		if(resources & IO_DLL) {
			*method |= IOM_DRV | IOM_OK;
		}
		
		if (resources & IO_ASM) {
			*method |= IOM_ASM | IOM_OK;
		}
		break;

	case LINK_SLV:
		if (resources & IO_USB) {
			*method |= IOM_DRV | IOM_OK;
		}
		break;

	case LINK_TIE:
	case LINK_VTI:
 		*method |= IOM_API | IOM_OK;
		break;

	default:
		DISPLAY_ERROR("libticables: bad argument (invalid link cable).\n");
		return ERR_ILLEGAL_ARG;
		break;
	}
		
  	if (!(*method & IOM_OK)) {
    		DISPLAY_ERROR("libticables: unable to find an I/O method.\n");
		return ERR_NO_RESOURCES;
	}
	
	return 0;
}

// Bind the right I/O address & device according to I/O method
static int win32_map_io(TicableMethod method, TicablePort port)
{
	DISPLAY(_("libticables: mapping I/O...\n"));
	
	switch (port) {
  	case USER_PORT:
    	break;

	case PARALLEL_PORT_1:
		io_address = PP1_ADDR;
      		strcpy(io_device, PP1_NAME);
    	break;

  	case PARALLEL_PORT_2:
		io_address = PP2_ADDR;
		strcpy(io_device, PP2_NAME);
    	break;

  	case PARALLEL_PORT_3:
    		io_address = PP3_ADDR;
      		strcpy(io_device, PP3_NAME);
    	break;

  	case SERIAL_PORT_1:
		io_address = SP1_ADDR;
      		strcpy(io_device, SP1_NAME);
    	break;

  	case SERIAL_PORT_2:
    		io_address = SP2_ADDR;
      		strcpy(io_device, SP2_NAME);
    	break;

  	case SERIAL_PORT_3:
    		io_address = SP3_ADDR;
      		strcpy(io_device, SP3_NAME);
    	break;

  	case SERIAL_PORT_4:
		io_address = SP3_ADDR;
		strcpy(io_device, SP3_NAME);
    	break;

  	case USB_PORT_1:
		strcpy(io_device, UP1_NAME);
	break;

	case VIRTUAL_PORT_1:
		io_address = VLINK0;
      		strcpy(io_device, "");
	break;

  	case VIRTUAL_PORT_2:
		io_address = VLINK1;
      		strcpy(io_device, "");
	break;

  	default:
    		DISPLAY_ERROR("libticables: bad argument (invalid port).\n");
		return ERR_ILLEGAL_ARG;
	break;
	}
	
	return 0;
}


int win32_register_cable(TicableType type, TicableLinkCable *lc)
{
	int ret;

	// map I/O
	ret = win32_map_io((TicableMethod)method, port);
	if(ret)
			return ret;
	
	// set the link cable
	DISPLAY(_("libticables: registering cable...\n"));
    	switch (type) {
    	case LINK_PAR:
      		if ((port != PARALLEL_PORT_1) &&
		    (port != PARALLEL_PORT_2) &&
		    (port != PARALLEL_PORT_3) && (port != USER_PORT))
			return ERR_INVALID_PORT;
		
		if(method & IOM_ASM)
			par_register_cable(lc);
		else if(method & IOM_DRV)
			par_register_cable(lc);
		break;
		
    	case LINK_SER:
      		if ((port != SERIAL_PORT_1) &&
	  		(port != SERIAL_PORT_2) &&
	  		(port != SERIAL_PORT_3) &&
	  		(port != SERIAL_PORT_4) &&
	  		(port != USER_PORT))
		return ERR_INVALID_PORT;

		if(method & IOM_ASM)
			ser_register_cable_1(lc);
		else if(method & IOM_DRV)
			ser_register_cable_1(lc);
		else if(method & IOM_IOCTL)
			ser_register_cable_2(lc);
		break;

    	case LINK_AVR:
      		if ((port != SERIAL_PORT_1) &&
	  		(port != SERIAL_PORT_2) &&
	  		(port != SERIAL_PORT_3) &&
	  		(port != SERIAL_PORT_4) && (port != USER_PORT))
		return ERR_INVALID_PORT;

		avr_register_cable(lc);
		break;

    	case LINK_VTL:
      		if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
		return ERR_INVALID_PORT;

      		vtl_register_cable(lc);
		break;

    	case LINK_TIE:
      		if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
			return ERR_INVALID_PORT;

      		tie_register_cable(lc);
		break;

    	case LINK_TGL:
	      	if ((port != SERIAL_PORT_1) &&
		  	(port != SERIAL_PORT_2) &&
		  	(port != SERIAL_PORT_3) &&
		  	(port != SERIAL_PORT_4) && (port != USER_PORT))
		return ERR_INVALID_PORT;

		tig_register_cable(lc);
		break;

    	case LINK_VTI:
      		if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
		return ERR_INVALID_PORT;
	
      		vti_register_cable(lc);
		break;

    	case LINK_SLV:
      		if ((port != USB_PORT_1) &&
		  	(port != USB_PORT_2) &&
		  	(port != USB_PORT_3) &&
		  	(port != USB_PORT_4) && (port != USER_PORT))
		return ERR_INVALID_PORT;

		if(method & IOM_DRV)
			slv_register_cable(lc);
		break;

    	default:
	      	DISPLAY_ERROR(_("libticables: invalid argument (bad cable)."));
	      	return ERR_ILLEGAL_ARG;
		break;
    	}

	return 0;
}


/***********/
/* Helpers */
/***********/

