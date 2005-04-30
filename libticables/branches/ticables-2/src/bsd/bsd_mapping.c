/* Hey EMACS -*- linux-c -*- */
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

/* Linux resources mapping */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gettext.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "externs.h"
#include "type2str.h"
#include "printl.h"

#include "bsd_mapping.h"
#include "links.h"

static int warning;

static int check_for_root(void);

int bsd_get_method(TicableType type, int resources, TicableMethod *method)
{
        // init warning
	warning = ERR_NO_ERROR;
	
	// reset method
	*method &= ~IOM_OK;
  	if (*method & IOM_AUTO) {
    		*method &= ~(IOM_ASM | IOM_API | IOM_DRV | IOM_IOCTL);
		printl1(0, _("getting method from resources (automatic):\n"));
  	} else
		printl1(0, _("getting method from resources (user-forced):\n"));

	// depending on link type, do some checks
	switch(type)
	{
	case LINK_NUL:
		*method |= IOM_NULL | IOM_OK;
		break;
		
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
		if (resources & IO_ASM) {
			if(check_for_root())
				printl1(0, _("  warning: can't use IO_ASM\n"));
			else {
				*method |= IOM_ASM | IOM_OK;
				break;
			}
		}
		
		if (resources & IO_API) {
			*method |= IOM_IOCTL | IOM_OK;
		}
		break;

	case LINK_PAR:
		if (resources & IO_ASM) {
			if(check_for_root())
				printl1(0, _("  warning: can't use IO_ASM\n"));
			else {
				*method |= IOM_ASM | IOM_OK;
				break;
			}
		}
		break;

	case LINK_SLV:
		if (resources & IO_LIBUSB) {
			*method |= IOM_IOCTL | IOM_OK;
			break;
		}
		break;

	case LINK_TIE:
	case LINK_VTI:
 		*method |= IOM_API | IOM_OK;
		break;

	default:
		printl1(2, "bad argument (invalid link cable).\n");
		return ERR_ILLEGAL_ARG;
		break;
	}
		
  	if (!(*method & IOM_OK)) {
    		printl1(2, "unable to find an I/O method.\n");
		return warning;	//ERR_NO_RESOURCES;
	}
	
	return 0;
}


// Bind the right I/O address & device according to I/O method
static int bsd_map_io(TicableMethod method, TicablePort port)
{
	printl1(0, _("mapping I/O...\n"));
	
	switch (port) {
	case NULL_PORT:
		strcpy(io_device, "/dev/null");
		io_address = 0;
		break;
		
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
    		printl1(2, "bad argument (invalid port).\n");
		return ERR_ILLEGAL_ARG;
	break;
	}
	
	return 0;
}


int bsd_register_cable(TicableType type, TicableLinkCable *lc)
{
	int ret;
	
	// map I/O
	ret = bsd_map_io((TicableMethod)method, port);
	if(ret)
		return ret;
	
	// set the link cable
	printl1(0, _("registering cable...\n"));
    	switch (type) {
    	case LINK_NUL:
		nul_register_cable(lc);
		break;
    		
    	case LINK_PAR:
      		if ((port != PARALLEL_PORT_1) &&
		    (port != PARALLEL_PORT_2) &&
		    (port != PARALLEL_PORT_3) && (port != USER_PORT))
			return ERR_INVALID_PORT;
		
		if(method & IOM_ASM)
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

		if(method & IOM_IOCTL)
			slv_register_cable_2(lc);
		break;

    	default:
	      	printl1(2, _("invalid argument (bad cable)."));
	      	return ERR_ILLEGAL_ARG;
		break;
    	}

	return 0;
}


/***********/
/* Helpers */
/***********/

static int check_for_root(void)
{
	uid_t uid = getuid();
    	
    	printl1(0, _("  check for asm usability: %s\n"), uid ? "no" : "yes");
    	
    	warning = ERR_ROOT;

	return (uid ? -1 : 0);
}

