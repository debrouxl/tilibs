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

#include "intl1.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "externs.h"
#include "printl.h"

#define MAXCHARS 1024

int linux_detect_os(char **os_type)
{
#ifdef HAVE_UNAME
	struct utsname buf;

	uname(&buf);
  	printl(0, _("Getting OS type...\r\n"));
  	printl(0, _("  System name: %s\r\n"), buf.sysname);
  	printl(0, _("  Node name: %s\r\n"), buf.nodename);
  	printl(0, _("  Release: %s\r\n"), buf.release);
  	printl(0, _("  Version: %s\r\n"), buf.version);
  	printl(0, _("  Machine: %s\r\n"), buf.machine);
	printl(0, _("Done.\r\n"));
#endif
	*os_type = OS_LINUX;

	return 0;
}

int linux_detect_port(TicablePortInfo * pi)
{
	int fd;
	FILE *f;
        int i;
        char name[10];
        int sa, ea;
        int nargs;
        char buffer[MAXCHARS];
	DIR *dir;
        struct dirent *file;
        int res;
        char path[25];

	// clear structure
	bzero(pi, sizeof(TicablePortInfo));

	/* Ensure /proc is mounted */

	fd = access("/proc/", F_OK);
        if (fd < 0) {
                printl(2, _("The pseudo-file '/proc' does not exist. Mount it with 'mount -t proc /proc proc'.\n"));
		return -1;
	}

	/* Do a first/rapid checking with /proc/ioports */
	
	printl(0, _("quick search for parallel/serial ports...\r\n"));
	
	// check for existence
	fd = access("/proc/ioports", F_OK);
	if (fd < 0) {
		printl(2, _("The pseudo-file '/proc/ioports' does not exist. Unable to probe ports.\r\n"));
		printl(0, _("Done.\r\n"));
		return -1;
	}
	
	// open file
	f = fopen("/proc/ioports", "rt");
	if (f == NULL) {
		printl(2, _("Unable to open /proc/ioports.\r\n"));
		return -1;
	}

	// parses all entries
	while(!feof(f)) {
		fgets(buffer, 256, f);
		// Form: '03f8-03ff : serial' or '0378-037a : parport'
		nargs = sscanf(buffer, "%x-%x : %s", &sa, &ea, name);
		if(nargs < 3)
			continue;
		
		if(strstr(name, "serial"))
			printl(0, _("  serial port found at 0x%03x\n"), sa);
		if(strstr(name, "parport"))
			printl(0, _("  parallel port found at 0x%03x\n"), sa);
	}

	// close file
	fclose(f);

	/* Do a thorough check */

	printl(0, _("search for all ports...\r\n"));

	/* Use /proc/sys/dev/parport/parportX/base-addr where X=0, 1, ...
	   to get infos on parallel ports */

	// open /proc/sys/dev/parport/ directory
	if ((dir = opendir("/proc/sys/dev/parport/")) == NULL) {
		printl(2, _("Unable to open '/proc/sys/dev/parport/'.\n"));
		return -1;
	}

	// parse for sub-directories
	while ((file = readdir(dir)) != NULL) {
		if (!strcmp(file->d_name, "."))
			continue;
		if (!strcmp(file->d_name, ".."))
			continue;
		
		// sub-dir such as parport0 ?
		if (strstr(file->d_name, "parport")) {
			res = sscanf(file->d_name, "parport%i", &i);
			if (res == 1) {
				if (i >= MAX_LPT_PORTS - 1)
					break;

				// yes, open base-addr file
				strcpy(path, "/proc/sys/dev/parport/");
				strcat(path, file->d_name);
				strcat(path, "/");
				strcat(path, "base-addr");
				sprintf(pi->lpt_name[i], "/dev/parport%i", i);
				f = fopen(path, "rt");
				if (f == NULL) {
					printl(2, _("unable to open this entry: <%s>\r\n"), path);
				} else {
					fscanf(f, "%i", &(pi->lpt_addr[i]));
					printl(0, _("  %s at 0x%03x\n"),
						pi->lpt_name[i], 
						pi->lpt_addr[i]);
					fclose(f);
				}
			} else {
				printl(2, _("Invalid parport entry: <%s>.\r\n"), file->d_name);
			}
		}
	}
	
	if (closedir(dir) == -1) {
		printl(2, _("Closedir\r\n"));
	}
	
	/* Use '/var/log/dmesg' to get infos on serial ports */

	// test for file access
	fd = access("/var/log/dmesg", F_OK);
	if (fd < 0) {
		printl(2, _("The file '/proc/tty/driver/serial' does not exist or is not accessible. Unable to probe serial ports.\r\n"));
		printl(0, _("Done.\r\n"));
		return -1;
	}

	// open it
	f = fopen("/var/log/dmesg", "rt");
	if (f == NULL) {
		printl(2, _("Unable to open this entry: <%s>\r\n"),
			      "/var/log/dmesg");
		return -1;
	}
	
	// read entries
	while(!feof(f)) {
                fgets(buffer, 256, f);

                // Form: 'ttyS0 at I/O 0x3f8 (irq = 4) is a 16550A'
                nargs = sscanf(buffer, "%s at I/O %x", name, &sa);
                if(nargs < 2)
                        continue;

                if(strstr(name, "ttyS")) {
			i = name[4] - '0';

			if (i >= MAX_LPT_PORTS - 1)
				break;

			sprintf(pi->com_name[i], "/dev/ttyS%i", i);
			(pi->com_addr)[i] = sa;
			printl(0, "  /dev/ttyS%i at 0x%03X\r\n", 
				i, pi->com_addr[i]);
		}
	}

	// close
	fclose(f);

	/* Use '/proc/bus/usb/devices' to get infos on usb ports */

	// to do...

	return 0;
}


char *result(int i)
{
  return ((i == 0) ? _("ok") : _("nok"));
}

int linux_detect_resources(void)
{
	printl(0, _("checking resources...\r\n"));
	resources = IO_LINUX;

	/* API: for use with ttySx */

#if defined(HAVE_TERMIOS_H)
  	resources |= IO_API;
  	printl(0, _("  IO_API: found at compile time (HAVE_TERMIOS_H)\r\n"));
#else
	printl(0, _("  IO_API: not found at compile time (HAVE_TERMIOS_H)\r\n"));
#endif

	/* ASM: for use with low-level I/O */

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
	resources |= IO_ASM;
#endif
	printl(0, _("  IO_ASM: %sfound at compile time (HAVE_ASM_IO_H).\n"),
		resources & IO_ASM ? "" : "not ");

	/* TIPAR: tipar kernel module */ 

#ifdef HAVE_LINUX_TICABLE_H
        resources |= IO_TIPAR;
#endif
        printl(0, _("  IO_TIPAR: %sfound at compile time (HAVE_LINUX_TICABLE_H)\r\n"), resources & IO_TIPAR ? "" : "not ");
	
	/* TISER: tiser kernel module */

#ifdef HAVE_LINUX_TICABLE_H
	resources |= IO_TISER;
#endif
        printl(0, _("  IO_TISER: %sfound at compile time (HAVE_LINUX_TICABLE_H)\r\n"), resources & IO_TISER ? "" : "not ");
	
	/* TIGLUSB: tiglusb kernel module */ 
	
#ifdef HAVE_LINUX_TIGLUSB_H
	resources |= IO_TIUSB;
#endif
	printl(0, _("  IO_TIUSB: %sfound at compile time (HAVE_LINUX_TIGLUSB_H)\r\n"),
		resources & IO_TIUSB ? "" : "not ");

	/* LIBUSB: lib-usb userland module */

#ifdef HAVE_LIBUSB
	resources |= IO_LIBUSB;
#endif
	printl(0, _("  IO_LIBUSB: %sfound at compile time (HAVE_LIBUSB)\r\n"),
		resources & IO_LIBUSB ? "" : "not ");

  	return 0;
}
