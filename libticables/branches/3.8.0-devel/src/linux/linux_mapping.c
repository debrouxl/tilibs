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

/* Linux resources mapping */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>

#include "intl.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "externs.h"
#include "verbose.h"

#include "links.h"

int linux_get_method(TicableType type, int resources, TicableMethod *method)
{
	DISPLAY(_("libticables: getting method from resources & cable...\n"));

	DISPLAY(_("  link cable: %s\n"), ticable_cabletype_to_string(type));
	
	// reset method
	*method &= ~IOM_OK;
  	if (*method & IOM_AUTO) {
    		*method &= ~(IOM_ASM | IOM_API | IOM_DRV);
		DISPLAY(_("  mode: automatic\n"));
  	} else
		DISPLAY(_("  mode: user-forced\n"));

	// depending on link type, do some checks
	switch(type)
	{
	case LINK_TGL:
		if(resources & IO_API)
			*method |= IOM_API | IOM_OK;
		break;
	case LINK_AVR:
		if(resources & IO_API)
                        *method |= IOM_API | IOM_OK;
		break;
	case LINK_SER:
		if(resources & IO_TISER)
			*method |= IOM_DRV | IOM_OK;
		else if (resources & IO_ASM)
			*method |= IOM_ASM | IOM_OK;
		else if (resources & IO_API)
			*method |= IOM_IOCTL | IOM_OK;
		break;
	case LINK_PAR:
		if(resources & IO_TIPAR)
			*method |= IOM_DRV | IOM_OK;
		else if (resources & IO_ASM)
			*method |= IOM_ASM | IOM_OK;
		break;
	case LINK_SLV:
		if (resources & IO_TIUSB)
			*method |= IOM_DRV | IOM_OK;
		else if (resources & IO_LIBUSB)
			*method |= IOM_IOCTL | IOM_OK;
		break;
	case LINK_TIE:
	case LINK_VTI:
 		*method |= IOM_API | IOM_OK;
		break;
	default:
		DISPLAY_ERROR("libticables: bad argument (invalid link cable).n");
		break;
	}
		
  	if (!(*method & IOM_OK)) {
    		DISPLAY_ERROR("unable to find an I/O method.\n");
		return ERR_NO_RESOURCES;
	} else {
		DISPLAY(_("  method: %s\n"), 
			ticable_method_to_string(*method));
	}
	
	return 0;
}

extern int resources, method;

int linux_register_cable(TicableType type, TicableLinkCable *lc)
{
//	int ret;

	// fill device and io_addr fields
	convert_port_into_device();
	
	// set the link cable
  	if (((resources & IO_LINUX) && !(method & IOM_DRV)) || (resources & IO_WIN32) || (resources & IO_OSX) || (resources & IO_BSD)) {	// no kernel driver (tipar/tiser/tiusb)
    	switch (type) {
    case LINK_PAR:		// IOM_ASM, IOM_DRV&Win32
      if ((port != PARALLEL_PORT_1) &&
	  (port != PARALLEL_PORT_2) &&
	  (port != PARALLEL_PORT_3) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	par_register_cable(lc);
      break;

    case LINK_SER:		// IOM_ASM, IOM_API, IOM_DRV&Win32
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) &&
	  (port != USER_PORT))
	return ERR_INVALID_PORT;

      if ((method & IOM_ASM) || (method & IOM_DRV)) {
	// serial routines in IOM_ASM/DLL mode
	ser_register_cable_1(lc);
	
      } else if (method & IOM_API) {
	ser_register_cable_2(lc);
      } else {
	      //set_default_cable(lc);
      }
      break;

    case LINK_AVR:		// IOM_API
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	avr_register_cable(lc);
      break;

    case LINK_VTL:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;

      vtl_register_cable(lc);
      break;

    case LINK_TIE:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;

      tie_register_cable(lc);
      break;

    case LINK_TGL:		// IOM_API
      if ((port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	tig_register_cable(lc);
      break;

    case LINK_VTI:		// IOM_API
      if ((port != VIRTUAL_PORT_1) && (port != VIRTUAL_PORT_2))
	return ERR_INVALID_PORT;
	
      vti_register_cable(lc);
      break;

    case LINK_SLV:		// IOM_LIBUSB (Linux) or IOM_TIUSB (Win32) or IOM_MAC (IOKit)
      if ((port != USB_PORT_1) &&
	  (port != USB_PORT_2) &&
	  (port != USB_PORT_3) &&
	  (port != USB_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	slv_register_cable(lc);
      break;

    default:			// error !
      DISPLAY(_
	      ("Function not implemented. This is a bug. Please report it."));
      DISPLAY(_("Informations:\n"));
      DISPLAY(_("Cable: %i\n"), type);
      DISPLAY(_("Program halted before crashing...\n"));
      exit(-1);
      break;
    }
  } else {			// kernel driver
    switch (type) {
    case LINK_PAR:		// IOM_TIPAR
    case LINK_SER:		// IOM_TISER
      if ((port != PARALLEL_PORT_1) &&
	  (port != PARALLEL_PORT_2) &&
	  (port != PARALLEL_PORT_3) &&
	  (port != SERIAL_PORT_1) &&
	  (port != SERIAL_PORT_2) &&
	  (port != SERIAL_PORT_3) &&
	  (port != SERIAL_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	dev_register_cable(lc);
      break;

    case LINK_SLV:		// IOM_TIUSB
      if ((port != USB_PORT_1) &&
	  (port != USB_PORT_2) &&
	  (port != USB_PORT_3) &&
	  (port != USB_PORT_4) && (port != USER_PORT))
	return ERR_INVALID_PORT;

	slv_register_cable(lc);
      break;

    default:			// error !
      DISPLAY(_
	      ("Function not implemented. This is a bug. Please report it."));
      DISPLAY(_("Informations:\n"));
      DISPLAY(_("Cable: %i\n"), type);
      DISPLAY(_("Program halted before crashing...\n"));
      exit(-1);
      break;
    }
  }
}


static int convert_port_into_device(void)
{
  switch (port) {
  case USER_PORT:
    break;

  case PARALLEL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tipar0");
    else {
      io_address = PP1_ADDR;
      strcpy(io_device, PP1_NAME);
    }
    break;

  case PARALLEL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tipar1");
    else {
      io_address = PP2_ADDR;
      strcpy(io_device, PP2_NAME);
    }
    break;

  case PARALLEL_PORT_3:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tipar2");
    else {
      io_address = PP3_ADDR;
      strcpy(io_device, PP3_NAME);
    }
    break;

  case SERIAL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tiser0");
    else {
      io_address = SP1_ADDR;
      strcpy(io_device, SP1_NAME);
    }
    break;

  case SERIAL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tiser1");
    else {
      io_address = SP2_ADDR;
      strcpy(io_device, SP2_NAME);
    }
    break;

  case SERIAL_PORT_3:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tiser2");
    else {
      io_address = SP3_ADDR;
      strcpy(io_device, SP3_NAME);
    }
    break;

  case SERIAL_PORT_4:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, "/dev/tiser");
    else {
      io_address = SP4_ADDR;
      strcpy(io_device, SP4_NAME);
    }
    break;

  case USB_PORT_1:
    strcpy(io_device, search_for_tiusb_node(0));
    break;

  case USB_PORT_2:
    strcpy(io_device, search_for_tiusb_node(1));
    break;

  case USB_PORT_3:
    strcpy(io_device, search_for_tiusb_node(2));
    break;

  case USB_PORT_4:
    strcpy(io_device, search_for_tiusb_node(3));
    break;
  case VIRTUAL_PORT_1:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, TIDEV_V0);
    else {
      io_address = VLINK0;
      strcpy(io_device, "");
    }
    break;

  case VIRTUAL_PORT_2:
    if ((method & IOM_DRV) && (resources & IO_LINUX))
      strcpy(io_device, TIDEV_V1);
    else {
      io_address = VLINK1;
      strcpy(io_device, "");
    }
    break;

  default:
    DISPLAY(_("libticables error: port value is illegal (%i) !\n"), port);
    DISPLAY(_("Exiting...\n"));
    //exit(1);
    break;
  }

  return 0;
}

#if 0

/*
  Returns mode string from mode value.
*/
const char *get_attributes(mode_t attrib)
{
	static char s[13] = " ---------- ";
      
        if (attrib & S_IRUSR)
                s[2] = 'r';
        if (attrib & S_IWUSR)
                s[3] = 'w';
        if (attrib & S_ISUID) {
                if (attrib & S_IXUSR)
                        s[4] = 's';
		else
                        s[4] = 'S';
        }
        else if (attrib & S_IXUSR)
                s[4] = 'x';
        if (attrib & S_IRGRP)
                s[5] = 'r';
        if (attrib & S_IWGRP)
                s[6] = 'w';
        if (attrib & S_ISGID) {
                if (attrib & S_IXGRP)
			s[7] = 's';
		else
                        s[7] = 'S';
        }
        else if (attrib & S_IXGRP)
                s[7] = 'x';
	if (attrib & S_IROTH)
                s[8] = 'r';
        if (attrib & S_IWOTH)
                s[9] = 'w';
        if (attrib & S_ISVTX) {
                if (attrib & S_IXOTH)
                        s[10] = 't';
                else
                        s[10] = 'T';
        }
	return s;
}

/*
   Returns user name from id.
*/
const char *get_user_name(uid_t uid)
{
	struct passwd *pwuid;

        if((pwuid = getpwuid(uid)) != NULL)
		return pwuid->pw_name;

	return "root";
}

/*
  Returns group name from id.
*/
const char *get_group_name(uid_t uid)
{
	struct group *grpid;
        
	if ((grpid = getgrgid(uid)) != NULL)
                return grpid->gr_name;

	return "root";
}

/* 
   Attempt to find a specific string in /proc (vfs) 
   - entry [in] : an entry such as '/proc/devices'
   - str [in) : an occurence to find (such as 'tipar')
*/
static int find_string_in_proc(char *entry, char *str)
{
	FILE *f;
	char buffer[MAXCHARS];
	int found = 0;
	
	f = fopen(entry, "rt");
	if (f == NULL) {
		return -1;
	}
	while (!feof(f)) {
		fscanf(f, "%s", buffer);
		if (strstr(buffer, str)) {
			found = 1;
		}
	}
	fclose(f);
	
	return found;
}

static void check_for_tipar_module(void)
{
	int devfs = 0;
	struct stat st;
	char name[15];
	int ret = !0;

#ifndef HAVE_LINUX_TICABLE_H
	DISPLAY(_("  IO_TIPAR: not found at compile time (HAVE_LINUX_TICABLE_H).\r\n"));
#else
	DISPLAY(_("  IO_TIPAR: checking for various stuffs\r\n"));
	DISPLAY(_("      found at compile time (HAVE_LINUX_TICABLE_H).\r\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	DISPLAY(_("      using devfs: %s\r\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tipar0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(!access(name, F_OK))
		DISPLAY(_("      node %s: exists.\r\n"), name);
	else {
		DISPLAY(_("      node %s: does not exists.\r\n"), name);
		ret = 0;
}

	if(!stat(name, &st)) {
		DISPLAY(_("      permissions/user/group:%s%s %s\r\n"),
                        get_attributes(st.st_mode),
                        get_user_name(st.st_uid),
                        get_group_name(st.st_gid));
	}
 
	if (find_string_in_proc("/proc/devices", "tipar") ||
            find_string_in_proc("/proc/modules", "tipar"))
		DISPLAY(_("      module: loaded\r\n"));
	else
		DISPLAY(_("      module: not loaded\r\n"));

	resources |= ret ? IO_TIPAR : 0;
 #endif
}

static void check_for_tiser_module(void)
{
	int devfs = 0;
	struct stat st;
	char name[15];
	int ret = !0;

#ifndef HAVE_LINUX_TICABLE_H
	DISPLAY(_("  IO_TISER: not found at compile time (HAVE_LINUX_TICABLE_H).\r\n"));
#else
	DISPLAY(_("  IO_TISER: checking for various stuffs\r\n"));
	DISPLAY(_("      found at compile time (HAVE_LINUX_TICABLE_H).\r\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	DISPLAY(_("      using devfs: %s\r\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiser0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(!access(name, F_OK))
		DISPLAY(_("      node %s: exists.\r\n"), name);
	else {
		DISPLAY(_("      node %s: does not exists.\r\n"), name);
		ret = 0;
}

	if(!stat(name, &st)) {
		DISPLAY(_("      permissions/user/group:%s%s %s\r\n"),
                        get_attributes(st.st_mode),
                        get_user_name(st.st_uid),
                        get_group_name(st.st_gid));
	}
 
	if (find_string_in_proc("/proc/devices", "tiser") ||
            find_string_in_proc("/proc/modules", "tiser"))
		DISPLAY(_("      module: loaded\r\n"));
	else
		DISPLAY(_("      module: not loaded\r\n"));

	resources |= ret ? IO_TISER : 0;
#endif
}

static void check_for_tiusb_module(void)
{
	int devfs = 0;
	struct stat st;
	char name[15];
	int ret = !0;

#ifndef HAVE_LINUX_TIGLUSB_H
	DISPLAY(_("  IO_TIUSB: not found at compile time (HAVE_LINUX_TIGLUSB_H).\r\n"));
#else
	DISPLAY(_("  IO_TIUSB: checking for various stuffs\r\n"));
	DISPLAY(_("      found at compile time (HAVE_LINUX_TIGLUSB_H).\r\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	DISPLAY(_("      using devfs: %s\r\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiusb0");
	else
		strcpy(name, "/dev/ticables/usb/0");

	if(!access(name, F_OK))
		DISPLAY(_("      node %s: exists.\r\n"), name);
	else {
		DISPLAY(_("      node %s: does not exists.\r\n"), name);
		ret = 0;
}

	if(!stat(name, &st)) {
		DISPLAY(_("      permissions/user/group:%s%s %s\r\n"),
			get_attributes(st.st_mode),
			get_user_name(st.st_uid),
			get_group_name(st.st_gid));
	}
 
	if (find_string_in_proc("/proc/devices", "tiglusb") ||
            find_string_in_proc("/proc/modules", "tiglusb"))
		DISPLAY(_("      module: loaded\r\n"));
	else
		DISPLAY(_("      module: not loaded\r\n"));

	resources |= ret ? IO_TIUSB : 0;
#endif
}

#endif
