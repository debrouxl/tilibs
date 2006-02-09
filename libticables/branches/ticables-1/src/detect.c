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

/* Probing wrapper */

/*
  This unit performs some auto-detection for:
  - Operating System
  - I/O ports such as parallel and serial ports
  - link cable type
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(__LINUX__)
#include "linux/linux_detect.c"
#elif defined(__MACOSX__)
#include "macos/macos_detect.c"
#elif defined(__BSD__)
#include "bsd/bsd_detect.c"
#elif defined(__WIN32__)
#include "win32/win32_detect.c"
#else
#include "none.h"
#endif

/*
  This function tries to detect the Operating System type.
  The returned value can be:
  - "Linux"
  - "Mac OS X"
  - "Windows9x" for Windows95, 98 or Me
  - "WindowsNT" for WindowsNT4 or 2000 or XP
  - "unknown" if failed
*/
TIEXPORT int TICALL ticable_detect_os(char **os_type)
{
#if defined(__LINUX__)
	linux_detect_os(os_type);
#elif defined(__MACOSX__)
  	macos_detect_os(os_type);
#elif defined(__BSD__)
  	bsd_detect_os(os_type);
#elif defined(__WIN32__)
	win32_detect_os(os_type);
#else
  	*os_type = _("unknown");
  	return -1;
#endif
  	return 0;
}


/* 
   This function attempts to detect which ports are available according 
   to the operating system type.
*/
TIEXPORT int TICALL ticable_detect_port(TicablePortInfo * pi)
{
#if defined(__LINUX__)
	return linux_detect_port(pi);
#elif defined(__MACOSX__)
	return macos_detect_port(pi);
#elif defined(__BSD__)
	return bsd_detect_port(pi);	
#elif defined(__WIN32__)
  	return win32_detect_port(pi);
#else
	return -1;
#endif
}


/*
  This function attemps to detect a link cable on the listed ports.
  The returned value is placed in pi.
  
  Beware: this routine can hang up your mouse if you have a mouse connected 
  on a serial port other than the first one (under Linux or Windows9x, 
  not NT4/2000)
*/
TIEXPORT int TICALL ticable_detect_cable(TicablePortInfo * pi)
{
	return 0;
}


/*
  This function tries to detect a link cable.
  The returned value is placed in os and pi.
*/
int TICALL ticable_detect_all(char **os, TicablePortInfo * pi)
{
	return 0;
}


/*
	Determine available I/O resources (IO_...).
*/
int detect_resources(void)
{
#if defined(__LINUX__)
	return linux_detect_resources();
#elif defined(__MACOSX__)
	return macos_detect_resources();
#elif defined(__BSD__)
	return bsd_detect_resources();
#elif defined(__WIN32__)
  	return win32_detect_resources();
#else
	return -1;
#endif
  	return 0;
}
