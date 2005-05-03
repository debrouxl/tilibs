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

/* Linux probing module */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stdints.h"
#include <dirent.h>
#include <sys/utsname.h>	// for uname()
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "../gettext.h"
#include "../error.h"
#include "../logging.h"

static int warning = 0;
static int devfs = 0;

/*static char *result(int i)
{
  return ((i == 0) ? _("ok") : _("nok"));
}*/

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
		ticables_warning(_("Unable to open the '/etc/group' file"));
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

int check_for_node_usability(const char *pathname)
{
	struct stat st;

	if(!access(pathname, F_OK))
		ticables_info(_("    node %s: exists"), pathname);
	else 
	{
		ticables_info(_("    node %s: does not exists"), pathname);
		ticables_info(_("    => you will have to create the node."));
		
		//warning = ERR_NODE_NONEXIST;
		
		return -1;
	}

	if(!stat(pathname, &st)) 
	{
		ticables_info(_("    permissions/user/group:%s%s %s"),
                        get_attributes(st.st_mode),
                        get_user_name(st.st_uid),
                        get_group_name(st.st_gid));
	} 
	else 
	{
		return -1;
	}	

	if(getuid() == st.st_uid) 
	{
		ticables_info(_("    is user can r/w on device: yes"));
		return 0;
	} 
	else 
	{
		ticables_info(_("    is user can r/w on device: no"));
	}

	printf("!!!\n");
	if((st.st_mode & S_IROTH) && (st.st_mode & S_IWOTH))
		ticables_info(_("    are others can r/w on device: yes"));
	else 
	{
		char *user, *group;
		
		ticables_info(_("    are others can r/w on device: no"));

		user = strdup(get_user_name(getuid()));
		group = strdup(get_group_name(st.st_gid));
		
		if(!search_for_user_in_group(user, group))
			ticables_info(_("    is the user '%s' in the group '%s': yes"), user, group); 
		else {
			ticables_info(_("    is the user '%s' in the group '%s': no"), user, group);
			ticables_info(_("    => you should add your username at the group '%s' in '/etc/group'"), group);
			ticables_info(_("    => you will have to restart you session, too"), group);
			free(user); free(group);
			
			//warning = ERR_NODE_PERMS;
			
			return -1;	
		}
		
		free(user); 
		free(group);
	}	

	return 0;
}

int check_for_root(void)
{
	uid_t uid = getuid();
    	
    	ticables_info(_("  check for asm usability: %s"), uid ? "no" : "yes");
    	
    	warning = ERR_ROOT;

	return (uid ? -1 : 0);
}

int check_for_tty(const char *devname)
{
	ticables_info(_("  check for tty usability:"));
	return check_for_node_usability(devname);	

	return 0;
}

int check_for_tipar(const char *devname)
{
	char name[15];

	ticables_info(_("  check for tipar usability:"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	ticables_info(_("      using devfs: %s"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tipar0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tipar"))
		ticables_info(_("      module: loaded"));
	else {
		ticables_info(_("      module: not loaded"));
		ticables_info(_("    => check the module exists (either as module, either as built-in)"));
		ticables_info(_("    => add an entry into your modutils file to automatically load it"));
		
		//warning = ERR_NOTLOADED;		
		return -1;
	}

	return 0;
}

int check_for_tiser(const char *devname)
{
	char name[15];

	ticables_info(_("  check for tiser usability:"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	ticables_info(_("    using devfs: %s"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiser0");
	else
		strcpy(name, "/dev/ticables/par/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tiser"))
		ticables_info(_("    module: loaded"));
	else {
		ticables_info(_("    module: not loaded"));
		ticables_info(_("    => check the module exists (compiled as module)"));
		ticables_info(_("    => add an entry into your modutils file to automatically load it."));
		
		//warning = ERR_NOTLOADED;
		return -1;
	}

	return 0;
}

int check_for_tiusb(const char *devname)
{
	char name[15];

	ticables_info(_("  check for tiusb usability:"));

	if(!access("/dev/.devfs", F_OK))
		devfs = !0;
	ticables_info(_("    using devfs: %s"), devfs ? "yes" : "no");

	if(!devfs)
		strcpy(name, "/dev/tiusb0");
	else
		strcpy(name, "/dev/ticables/usb/0");

	if(check_for_node_usability(name))
		return -1;
 
	if (find_string_in_proc("/proc/devices", "tiglusb"))
		ticables_info(_("    module: loaded"));
	else {
		ticables_info(_("    module: not loaded"));
		ticables_info(_("    => check the module exists (either as module, either as built-in)"));
		ticables_info(_("    => add an entry into your modutils file to automatically load it"));
		
		//warning = ERR_NOTLOADED;
		return -1;
	}

	return 0;
}

#define	USBFS	"/proc/bus/usb"

int check_for_libusb(void)
{
	ticables_info(_("  check for lib-usb usability:"));

	if(!access(USBFS, F_OK))
		ticables_info(_("    usb filesystem (/proc/bus/usb): %s"), "mounted");
	else {
		ticables_info(_("    usb filesystem (/proc/bus/usb): %s"), "not mounted");
		ticables_info(_("    => the usbfs must be supported by your kernel and you have to mount it"));
		ticables_info(_("    => add an 'none /proc/bus/usb usbfs defaults 0 0' in your /etc/fstab'"));
		
		//warning = ERR_NOTMOUNTED;
		return -1;
	}
	
	if(check_for_node_usability(USBFS "/devices"))
		return -1;
	
	return 0;
}
