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

/* Linux probing module */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif
#include <dirent.h>
#include <sys/utsname.h>	// for uname()
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

#include "intl.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "externs.h"
#include "verbose.h"

#define MAXCHARS 1024

int linux_detect_os(char **os_type)
{
#ifdef HAVE_UNAME
	struct utsname buf;

	uname(&buf);
  	DISPLAY(_("Getting OS type...\r\n"));
  	DISPLAY(_("  System name: %s\r\n"), buf.sysname);
  	DISPLAY(_("  Node name: %s\r\n"), buf.nodename);
  	DISPLAY(_("  Release: %s\r\n"), buf.release);
  	DISPLAY(_("  Version: %s\r\n"), buf.version);
  	DISPLAY(_("  Machine: %s\r\n"), buf.machine);
	DISPLAY(_("Done.\r\n"));
#endif
	*os_type = "Linux";

	return 0;
}


int linux_detect_port(TicablePortInfo * pi)
{
  int fd;
  FILE *f;
  char buffer[MAXCHARS];
  char info[MAXCHARS];
  int i, j;
  DIR *dir;
  struct dirent *file;
  int res;
  char path[MAXCHARS] = "/proc/sys/dev/parport/";

  bzero(pi, sizeof(TicablePortInfo));
  
  /* Use /proc/sys/dev/parport/parportX/base-addr where X=0, 1, ... */
  DISPLAY(_("Probing parallel ports...\r\n"));
  if ((dir = opendir("/proc/sys/dev/parport/")) == NULL) {
    DISPLAY_ERROR(_
		  ("Unable to open this directory: '/proc/sys/dev/parport/'.\r\n"));
    return -1;
  }

  while ((file = readdir(dir)) != NULL) {
    if (!strcmp(file->d_name, "."))
      continue;
    if (!strcmp(file->d_name, ".."))
      continue;

    if (strstr(file->d_name, "parport")) {
      res = sscanf(file->d_name, "parport%i", &i);
      if (res == 1) {
	if (i >= MAX_LPT_PORTS - 1)
	  break;
	strcpy(path, "/proc/sys/dev/parport/");
	strcat(path, file->d_name);
	strcat(path, "/");
	strcat(path, "base-addr");
	sprintf(pi->lpt_name[i], "/dev/par%i", i);
	f = fopen(path, "rt");
	if (f == NULL) {
	  DISPLAY_ERROR(_("unable to open this entry: <%s>\r\n"), path);
	} else {
	  fscanf(f, "%i", &(pi->lpt_addr[i]));
	  DISPLAY(_
		  ("  %s at address 0x%03x\r\n"),
		  pi->lpt_name[i], pi->lpt_addr[i]);
	  fclose(f);
	}
      } else {
	DISPLAY_ERROR(_("Invalid parport entry: <%s>.\r\n"), file->d_name);
      }
    }
  }

  if (closedir(dir) == -1) {
    DISPLAY_ERROR(_("Closedir\r\n"));
  }
  DISPLAY(_("Done.\r\n"));

  /* Use /proc/tty/driver/serial */
  DISPLAY(_("Probing serial ports...\r\n"));
  fd = access("/proc/tty/driver/serial", F_OK);
  if (fd < 0) {
    DISPLAY_ERROR(_
		  ("The file '/proc/tty/driver/serial' does not exist. Unable to probe serial port.\r\n"));
    DISPLAY(_("Done.\r\n"));
    return -1;
  }

  f = fopen("/proc/tty/driver/serial", "rt");
  if (f == NULL) {
    DISPLAY_ERROR(_("Unable to open this entry: <%s>\r\n"),
		  "/proc/tty/driver/serial");
    return -1;
  }

  fgets(buffer, 256, f);
  for (i = 0; i < MAX_COM_PORTS; i++) {
    fgets(buffer, 256, f);
    sscanf(buffer, "%i: uart:%s port:%03X ", &j, info,
	   &((pi->com_addr)[i]));
    if (strcmp(info, "unknown")) {
      sprintf(pi->com_name[i], "/dev/ttyS%i", j);
      DISPLAY("  /dev/ttyS%i: %8s adr:%03X\r\n", j, info, pi->com_addr[i]);
    } else
      pi->com_addr[i] = 0;
  }
  DISPLAY(_("Done.\r\n"));

  return 0;
}


char *result(int i)
{
  return ((i == 0) ? _("ok") : _("nok"));
}

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

int linux_detect_resources(void)
{
	DISPLAY(_("Libticables: checking resources...\r\n"));
	resources = IO_LINUX;

	/* API: for use with ttySx */

#if defined(HAVE_TERMIOS_H)
  	resources |= IO_API;
  	DISPLAY(_("  IO_API: found at compile time (HAVE_TERMIOS_H)\r\n"));
#else
	DISPLAY(_("  IO_API: not found at compile time (HAVE_TERMIOS_H)\r\n"));
#endif

	/* ASM: for use with low-level I/O */

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
	DISPLAY(_("  IO_ASM: found at compile time (HAVE_ASM_IO_H), "));
    	uid_t uid = getuid();

    	if (uid != 0) {
      		DISPLAY(_("but unuseable (non root)\r\n"));
      		resources &= ~IO_ASM;
    	} else {
      		DISPLAY(_("and useable (root)\r\n"));
      		resources |= IO_ASM;
    	}
#else
	DISPLAY(_("  IO_ASM: not found at compile time (HAVE_ASM_IO_H), "));
#endif

	/* TIPAR: tipar kernel module */ 

	check_for_tipar_module();
	
	/* TISER: tiser kernel module */ 

	check_for_tiser_module();
	
	/* TIGLUSB: tiglusb kernel module */ 

	check_for_tiusb_module();

#ifdef HAVE_LIBUSB
	resources |= IO_LIBUSB;
#endif
	DISPLAY(_("  IO_LIBUSB: %sfound at compile time (HAVE_LIBUSB).\r\n"),
	resources & IO_LIBUSB ? "" : "not ");

  return 0;
}
