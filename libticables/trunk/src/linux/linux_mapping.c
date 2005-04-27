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
#include "links.h"

#include "linux_detect.h"

static int warning;
static int devfs = 0;

static int check_for_root(void);
static int check_for_tty(void);
static int check_for_tipar(void);
static int check_for_tiser(void);
static int check_for_tiusb(void);
static int check_for_libusb(void);


int linux_get_method(TicableType type, int resources, TicableMethod *method)
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
			if(check_for_tty())
				printl1(0, _("  warning, can't use IO_API\n"));
			*method |= IOM_API | IOM_OK;
		}
		break;

	case LINK_AVR:
		if(resources & IO_API) {
			if(check_for_tty())
				printl1(0, _("  warning: can't use IO_API\n"));
			*method |= IOM_API | IOM_OK;	
		}
		break;

	case LINK_SER:
		if(resources & IO_TISER) {
			if(check_for_tiser())
				printl1(0, _("  warning: can't use IO_TISER\n"));
			else {
				*method |= IOM_DRV | IOM_OK;
				break;
			}
		}
		       
		if (resources & IO_ASM) {
			if(check_for_root())
				printl1(0, _("  warning: can't use IO_ASM\n"));
			else {
				*method |= IOM_ASM | IOM_OK;
				break;
			}
		}
		
		if (resources & IO_API) {
			if(check_for_tty())
				printl1(0, _("  warning: can't use IO_API\n"));
			else {
				*method |= IOM_IOCTL | IOM_OK;
				break;
			}
		}
		break;

	case LINK_PAR:
		if(resources & IO_TIPAR) {
			if(check_for_tipar())
				printl1(0, _("  warning: can't use IO_TIPAR\n"));
			else {
				*method |= IOM_DRV | IOM_OK;
				break;
			}
		}
		
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
		if (resources & IO_TIUSB) {
			if(check_for_tiusb())
				printl1(0, _("  warning: can't use IO_TIUSB\n"));
			else {
				*method |= IOM_DRV | IOM_OK;
				break;
			}
		}
		
		if (resources & IO_LIBUSB) {
			if(check_for_libusb())
				printl1(0, _("  warning: can't use IO_LIBUSB\n"));
			else {
				*method |= IOM_IOCTL | IOM_OK;
				break;
			}
		}
		break;

	case LINK_TIE:
	case LINK_VTI:
	case LINK_VTL:
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


const char *tipar_node_names[2][3] = { 
	{ "/dev/tipar0", "/dev/tipar1", "/dev/tipar2"}, 
	{ "/dev/ticables/par/0", "/dev/ticables/par/1", "/dev/ticables/par/2" },
};

const char *tiser_node_names[2][4] = { 
	{ "/dev/tiser0", "/dev/tiser1", "/dev/tiser2", "/dev/tiser3"}, 
	{ "/dev/ticables/ser/0", "/dev/ticables/ser/1", "/dev/ticables/ser/2", "/dev/ticables/ser/3" },
};

const char *tiusb_node_names[2][4] = { 
	{ "/dev/tiusb0", "/dev/tiusb1", "/dev/tiusb2", "/dev/tiusb3"}, 
	{ "/dev/ticables/usb/0", "/dev/ticables/usb/1", "/dev/ticables/usb/2", "/dev/ticables/usb/3" },
};


// Bind the right I/O address & device according to I/O method
static int linux_map_io(TicableMethod method, TicablePort port)
{
	printl1(0, _("mapping I/O...\n"));
	
	switch (port) {
  	case USER_PORT:
		break;
		
	case NULL_PORT:
		strcpy(io_device, "/dev/null");
		io_address = 0;
		break;
		
	case PARALLEL_PORT_1:
		if(method & IOM_DRV)
    			strcpy(io_device, tipar_node_names[devfs][0]);
    		else {
      			io_address = PP1_ADDR;
      			strcpy(io_device, PP1_NAME);
    		}
    	break;

  	case PARALLEL_PORT_2:
    		if (method & IOM_DRV)
      			strcpy(io_device, tipar_node_names[devfs][1]);
    		else {
      			io_address = PP2_ADDR;
      			strcpy(io_device, PP2_NAME);
    		}
    	break;

  	case PARALLEL_PORT_3:
    		if (method & IOM_DRV)
      			strcpy(io_device, tipar_node_names[devfs][3]);
    		else {
      			io_address = PP3_ADDR;
      			strcpy(io_device, PP3_NAME);
    		}
    	break;

  	case SERIAL_PORT_1:
    		if (method & IOM_DRV)
      			strcpy(io_device, tiser_node_names[devfs][0]);
    		else {
      			io_address = SP1_ADDR;
      			strcpy(io_device, SP1_NAME);
    		}
    	break;

  	case SERIAL_PORT_2:
    		if (method & IOM_DRV)
      			strcpy(io_device, tiser_node_names[devfs][1]);
    		else {
      			io_address = SP2_ADDR;
      			strcpy(io_device, SP2_NAME);
    		}
    	break;

  	case SERIAL_PORT_3:
    	if (method & IOM_DRV)
      			strcpy(io_device, tiser_node_names[devfs][2]);
    		else {
      			io_address = SP3_ADDR;
      			strcpy(io_device, SP3_NAME);
    		}
    	break;

  	case SERIAL_PORT_4:
    	if (method & IOM_DRV)
      			strcpy(io_device, tiser_node_names[devfs][3]);
    		else {
      			io_address = SP3_ADDR;
      			strcpy(io_device, SP3_NAME);
    		}
    	break;

  	case USB_PORT_1:
		strcpy(io_device, tiusb_node_names[devfs][0]);
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

int linux_register_cable(TicableType type, TicableLinkCable *lc)
{
	int ret;
	
	// map I/O
	ret = linux_map_io((TicableMethod)method, port);
	if(ret)
		return ret;

	// set fields to default values
	nul_register_cable(lc);
	
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
		else if(method & IOM_DRV)
			dev_register_cable(lc);
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
		else if(method & IOM_DRV)
			dev_register_cable(lc);
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

#ifdef HAVE_LIBUSB
		if(method & IOM_IOCTL)
			slv_register_cable_2(lc);
		else
#endif
		if(method & IOM_DRV)
			slv_register_cable_1(lc);
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

/*
  Returns mode string from mode value.
*/
static const char *get_attributes(mode_t attrib)
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
static const char *get_user_name(uid_t uid)
{
	struct passwd *pwuid;

        if((pwuid = getpwuid(uid)) != NULL)
		return pwuid->pw_name;

	return "root";
}

/*
  Returns group name from id.
*/
static const char *get_group_name(uid_t uid)
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
	char buffer[80];
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

/* 
   Attempt to find if an user is attached to a group.
   - user [in] : a user name
   - group [in] : a group name
*/
static int search_for_user_in_group(const char *user, const char *group)
{
	FILE *f;
	char buffer[129];
	
	f = fopen("/etc/group", "rt");
	if (f == NULL) {
		printl1(2, _("Unable to open the '/etc/group' file\n"));
		return -1;
	}

	while (!feof(f)) {
		fgets(buffer, 129, f);
		
		if (strstr(buffer, group)) {
			if(strstr(buffer, user)) {
				fclose(f);
				return 0;
			} else {
				fclose(f);
				return -1;
			}
		}
	}

	fclose(f);
	return -1;
}

static int check_for_node_usability(const char *pathname)
{
	struct stat st;

	if(!access(pathname, F_OK))
		printl1(0, _("    node %s: exists\n"), pathname);
	else {
		printl1(0, _("    node %s: does not exists\n"), pathname);
		printl1(0, _("    => you will have to create the node.\n"));
		
		warning = ERR_NODE_NONEXIST;
		
		return -1;
	}

	if(!stat(pathname, &st)) {
		printl1(0, _("    permissions/user/group:%s%s %s\n"),
                        get_attributes(st.st_mode),
                        get_user_name(st.st_uid),
                        get_group_name(st.st_gid));
	} else {
		return -1;
	}	

	if(getuid() == st.st_uid) {
		printl1(0, _("    is user can r/w on device: yes\n"));
		return 0;
	} else {
		printl1(0, _("    is user can r/w on device: no\n"));
	}

	if((st.st_mode & S_IROTH) && (st.st_mode & S_IWOTH))
		printl1(0, _("    are others can r/w on device: yes\n"));
	else {
		char *user, *group;
		
		printl1(0, _("    are others can r/w on device: no\n"));

		user = strdup(get_user_name(getuid()));
		group = strdup(get_group_name(st.st_gid));
		
		if(!search_for_user_in_group(user, group))
			printl1(0, _("    is the user '%s' in the group '%s': yes\n"), user, group); 
		else {
			printl1(0, _("    is the user '%s' in the group '%s': no\n"), user, group);
			printl1(0, _("    => you should add your username at the group '%s' in '/etc/group'\n"), group);
			printl1(0, _("    => you will have to restart you session, too\n"), group);
			free(user); free(group);
			
			warning = ERR_NODE_PERMS;
			
			return -1;	
		}
		
		free(user); 
		free(group);
	}	

	return 0;
}

static int check_for_root(void)
{
	uid_t uid = getuid();
    	
    	printl1(0, _("  check for asm usability: %s\n"), uid ? "no" : "yes");
    	
    	warning = ERR_ROOT;

	return (uid ? -1 : 0);
}

static int check_for_tty(void)
{
	printl1(0, _("  check for tty usability:\n"));
	return check_for_node_usability(SP1_NAME);	

	return 0;
}

static int check_for_tipar(void)
{
	char name[15];

	printl1(0, _("  check for tipar usability:\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	printl1(0, _("      using devfs: %s\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tipar0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tipar"))
		printl1(0, _("      module: loaded\n"));
	else {
		printl1(0, _("      module: not loaded\n"));
		printl1(0, _("    => check the module exists (either as module, either as built-in)\n"));
		printl1(0, _("    => add an entry into your modutils file to automatically load it\n"));
		
		warning = ERR_NOTLOADED;		
		return -1;
	}

	return 0;
}

static int check_for_tiser(void)
{
	char name[15];

	printl1(0, _("  check for tiser usability:\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	printl1(0, _("    using devfs: %s\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiser0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tiser"))
		printl1(0, _("    module: loaded\n"));
	else {
		printl1(0, _("    module: not loaded\n"));
		printl1(0, _("    => check the module exists (compiled as module)\n"));
		printl1(0, _("    => add an entry into your modutils file to automatically load it.\n"));
		
		warning = ERR_NOTLOADED;
		return -1;
	}

	return 0;
}

static int check_for_tiusb(void)
{
	char name[15];

	printl1(0, _("  check for tiusb usability:\n"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	printl1(0, _("    using devfs: %s\n"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiusb0");
	else
		strcpy(name, "/dev/ticables/usb/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tiglusb"))
		printl1(0, _("    module: loaded\n"));
	else {
		printl1(0, _("    module: not loaded\n"));
		printl1(0, _("    => check the module exists (either as module, either as built-in)\n"));
		printl1(0, _("    => add an entry into your modutils file to automatically load it\n"));
		
		warning = ERR_NOTLOADED;
		return -1;
	}

	return 0;
}

#define	USBFS	"/proc/bus/usb"

static int check_for_libusb(void)
{
	printl1(0, _("  check for lib-usb usability:\n"));

	if(!access(USBFS, F_OK))
		printl1(0, _("    usb filesystem (/proc/bus/usb): %s\n"), "mounted");
	else {
		printl1(0, _("    usb filesystem (/proc/bus/usb): %s\n"), "not mounted");
		printl1(0, _("    => the usbfs must be supported by your kernel and you have to mount it\n"));
		printl1(0, _("    => add an 'none /proc/bus/usb usbfs defaults 0 0' in your /etc/fstab'\n"));
		
		warning = ERR_NOTMOUNTED;
		return -1;
	}
	
	if(check_for_node_usability(USBFS "/devices"))
		return -1;
	
	return 0;
}
