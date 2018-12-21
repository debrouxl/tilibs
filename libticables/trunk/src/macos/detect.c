/* Hey EMACS -*- linux-c -*- */
/* $Id: detect.c 2618 2006-07-20 08:44:49Z kevinkofler $ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (C) 2006  Kevin Kofler
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

/* Mac OS X probing module */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#include "../gettext.h"
#include "../error.h"
#include "../logging.h"
#include "detect.h"

/*
  Returns mode string from mode value.
*/
static const char *get_attributes(mode_t attrib)
{
    const char *i = " ---------- ";
    static char s[13];

    strcpy(s, i);

    if (attrib & S_IRUSR)
	s[2] = 'r';
    if (attrib & S_IWUSR)
	s[3] = 'w';
    if (attrib & S_ISUID)
    {
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
    if (attrib & S_ISGID)
    {
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
    if (attrib & S_ISVTX)
    {
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

    return "not found";
}

/*
  Returns group name from id.
*/
static const char *get_group_name(uid_t uid)
{
    struct group *grpid;

    if ((grpid = getgrgid(uid)) != NULL)
	return grpid->gr_name;

    return "not found";
}

/*
   Attempt to find if an user is attached to a group.
   - user [in] : a user name
   - group [in] : a group name
*/
static int search_for_user_in_group(const char *user, const char *group)
{
    FILE *f;
    char buffer[256];
    const char *entry = "/etc/group";

    f = fopen(entry, "rt");
    if (f == NULL)
    {
	ticables_warning(_("can't open '%s'."), entry);
	return -1;
    }

    while (!feof(f))
    {
	if (!fgets(buffer, 256, f))
		break;

	if (strstr(buffer, group))
	{
	    if(strstr(buffer, user))
	    {
		fclose(f);
		return 0;
	    }
	    else
	    {
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
    {
	ticables_info(_("    node %s: exists"), pathname);
    }
    else
    {
	ticables_info(_("    node %s: does not exist"), pathname);
	ticables_info(_("    => you will have to create the node."));

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
	ticables_warning("can't stat '%s'.", pathname);
	return -1;
    }

    if(getuid() == st.st_uid)
    {
	ticables_info(_("    user can r/w on device: yes"));
	return 0;
    }
    else
    {
	ticables_info(_("    user can r/w on device: no"));
    }

    if((st.st_mode & S_IROTH) && (st.st_mode & S_IWOTH))
    {
	ticables_info(_("    others can r/w on device: yes"));
    }
    else
    {
	char *user, *group;

	ticables_info(_("    others can r/w on device: no"));

	user = strdup(get_user_name(getuid()));
	group = strdup(get_group_name(st.st_gid));

	if(!search_for_user_in_group(user, group))
	{
	    ticables_info(_("    is the user '%s' in the group '%s': yes"),
			  user, group);
	}
	else
	{
	    ticables_info(_("    is the user '%s' in the group '%s': no"), user, group);
	    ticables_info(_("    => you should add your username at the group '%s' in '/etc/group'"), group);
	    ticables_info(_("    => you will have to restart your session, too"));
	    free(user);
	    free(group);

	    return -1;
	}

	free(user);
	free(group);
    }

    return 0;
}

int macosx_check_root(void)
{
    uid_t uid = getuid();

    ticables_info(_("Check for super-user access: %s"),
		  uid ? "no" : "yes");

    return (uid ? ERR_ROOT : 0);
}

int macosx_check_tty(const char *devname)
{
    ticables_info(_("Check for tty usability:"));
    if(check_for_node_usability(devname) == -1)
	return ERR_TTDEV;

    return 0;
}

int macosx_check_parport(const char *devname)
{
    return ERR_PPDEV;
}

int macosx_check_libusb(void)
{
	ticables_info(_("Check for libusb support:"));
#if defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0)
	ticables_info(_("    usb support: available."));
	return 0;
#else
	ticables_info(_("    usb support: not compiled."));
	return ERR_USBFS;
#endif
}
