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
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include <dirent.h>
#include <sys/utsname.h>	// for uname()
#include <unistd.h>

#include "intl.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "externs.h"
#include "verbose.h"

#define MAXCHARS 1024

int linux_detect_os(char **os_type)
{
	struct utsname buf;

	uname(&buf);
  	DISPLAY(_("Getting OS type...\r\n"));
  	DISPLAY(_("  System name: %s\r\n"), buf.sysname);
  	DISPLAY(_("  Node name: %s\r\n"), buf.nodename);
  	DISPLAY(_("  Release: %s\r\n"), buf.release);
  	DISPLAY(_("  Version: %s\r\n"), buf.version);
  	DISPLAY(_("  Machine: %s\r\n"), buf.machine);
	DISPLAY(_("Done.\r\n"));
	
	*os_type = "Linux";

	return 0;
}


int linux_detect_port(TicablePortInfo * pi)
{
  int ret = 0;
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


/* Try to find a specific string in /proc (vfs) */
#ifdef __LINUX__
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

int linux_detect_resources(void)
{
	DISPLAY(_("Libticables: checking resources...\r\n"));
	resources = IO_LINUX;
	
  	resources |= IO_API;
  	DISPLAY(_("  IO_API: ok\r\n"));

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
	// check super or normal user
    	uid_t uid = getuid();
    	if (uid != 0) {
      		DISPLAY(_("  IO_ASM: nok (a kernel module is needed)\r\n"));
      		resources &= ~IO_ASM;
    	} else {
      		DISPLAY(_("  IO_ASM: ok (super user)\r\n"));
      		resources |= IO_ASM;
    	}
#endif

  	if (find_string_in_proc("/proc/devices", "tipar") ||
      		find_string_in_proc("/proc/modules", "tipar"))
    		resources |= IO_TIPAR;
  		DISPLAY(_("  IO_TIPAR: %s\r\n"), resources & IO_TIPAR ? "ok" : "nok");

  	if (find_string_in_proc("/proc/devices", "tiser") ||
      		find_string_in_proc("/proc/modules", "tiser"))
    		resources |= IO_TISER;
  		DISPLAY(_("  IO_TISER: %s\r\n"), resources & IO_TISER ? "ok" : "nok");

  	if (find_string_in_proc("/proc/devices", "tiusb") ||
      	find_string_in_proc("/proc/modules", "tiusb") ||
	find_string_in_proc("/proc/devices", "tiglusb") ||
      	find_string_in_proc("/proc/modules", "tiglusb")
      	)
    		resources |= IO_TIUSB;
  	DISPLAY(_("  IO_TIUSB: %s\r\n"), resources & IO_TIUSB ? "ok" : "nok");

#ifdef HAVE_LIBUSB
	resources |= IO_LIBUSB;
#endif
	DISPLAY(_("  IO_LIBUSB: %s\r\n"), resources & IO_LIBUSB ? "ok" : "N/A");
#endif /*__LINUX__*/

  DISPLAY(_("Done.\r\n"));

  return 0;
}
