/* Hey EMACS -*- bsd-c -*- */
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

/* *Bsd probing module */

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
#include "printl1.h"

int bsd_detect_os(char **os_type)
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
	*os_type = OS_BSD;

	return 0;
}

int bsd_detect_port(TicablePortInfo * pi)
{
	return 0;
}

int bsd_detect_resources(void)
{
	printl(0, _("libticables: checking resources...\r\n"));
	resources = IO_BSD;

	/* API: for use with ttySx */

#if defined(HAVE_TERMIOS_H)
  	resources |= IO_API;
  	printl(0, _("  IO_API: found at compile time (HAVE_TERMIOS_H)\r\n"));
#else
	printl(0, _("  IO_API: not found at compile time (HAVE_TERMIOS_H)\r\n"));
#endif

	/* ASM: for use with low-level I/O */

#if defined(__I386__)
	resources |= IO_ASM;
#endif
	printl(0, _("  IO_ASM: %sfound at compile time (HAVE_ASM_IO_H).\n"),
		resources & IO_ASM ? "" : "not ");

	/* LIBUSB: lib-usb userland module */

#ifdef HAVE_LIBUSB
	resources |= IO_LIBUSB;
#endif
	printl(0, _("  IO_LIBUSB: %sfound at compile time (HAVE_LIBUSB)\r\n"),
		resources & IO_LIBUSB ? "" : "not ");

  	return 0;
}
