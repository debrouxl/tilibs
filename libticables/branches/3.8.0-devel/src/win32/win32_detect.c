/* Hey EMACS -*- win32-c -*- */
/* $Id: linux_detect.c 397 2004-03-29 17:21:12Z roms $ */

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

/* Win32 probing module */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include "../intl.h"
#include "../cabl_def.h"
#include "../cabl_err.h"
#include "../export.h"
#include "../externs.h"
#include "../verbose.h"

#include "porttalk.h"

int win32_detect_os(char **os_type)
{
	OSVERSIONINFO os;

  	DISPLAY(_("Getting OS type...\r\n"));
  	
  	memset(&os, 0, sizeof(OSVERSIONINFO));
  	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  	GetVersionEx(&os);
  	DISPLAY(_("  operating System: %s\r\n"),
	  (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ?
	  "Windows9x" : "WindowsNT");
  	DISPLAY(_("  version: major=%i, minor=%i\r\n"),
	  os.dwMajorVersion, os.dwMinorVersion);
  
  	if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
    		*os_type = OS_WIN9X;
  	} else if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    		*os_type = OS_WINNT;
  	} else
    		*os_type = _("unknown");
  	
  	DISPLAY(_("Done.\r\n"));

	return 0;
}

int win32_detect_port(TicablePortInfo * pi)
{
	// I will put them later...

	return 0;
}

int win32_detect_resources(void)
{
	HINSTANCE hDLL = NULL;	/* Handle for TiglUsb driver */
    HANDLE PortTalk_Handle;	/* Handle for PortTalk Driver */
	char *os;
    	
	DISPLAY(_("libticables: checking resources...\r\n"));
	win32_detect_os(&os);
	
	/* Windows NT need permissions */
	
	if (!strcmp(os, "Windows9x"))
      		resources = IO_WIN9X;
    	else if (!strcmp(os, "WindowsNT"))
      		resources = IO_WINNT;
    	else
      		return ERR_NO_RESOURCES;

    	/* API: for use with COMx */
    	
    	resources |= IO_API;
    	DISPLAY(_("  IO_API: %sfound at compile time.\n"),
		resources & IO_API ? "" : "not ");

	/* ASM: for use with low-level I/O and Win9x */
	
    	if (!strcmp(os, "Windows9x")) {
      		resources |= IO_ASM;
      		DISPLAY(_("  IO_ASM: available (Win9x).\n"));
	} else {
		DISPLAY(_("  IO_ASM: not available (WinNT).\n"));
	}	

	/* DLL: for use with low-level I/O and WinNT */

    	if (!strcmp(os, "WindowsNT")) {
      		// Open PortTalk Driver. If we cannot open it, try installing and starting it
      		PortTalk_Handle = CreateFile("\\\\.\\PortTalk",
				   GENERIC_READ,
				   0,
				   NULL,
				   OPEN_EXISTING,
				   FILE_ATTRIBUTE_NORMAL, NULL);

      		if (PortTalk_Handle == INVALID_HANDLE_VALUE) {
			// Start or Install PortTalk Driver
			PortTalkStartDriver();

			// Don't be too hurry !
			Sleep(1000);
	
			// Then try to open once more, before failing
			PortTalk_Handle =
	    		CreateFile("\\\\.\\PortTalk",
		       		GENERIC_READ, 0, NULL,
		       		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (PortTalk_Handle == INVALID_HANDLE_VALUE) {
	  			DISPLAY_ERROR("PortTalk: Couldn't access PortTalk Driver, Please ensure driver is loaded.\n\n");
	  			return -1;
			}
      		}

      		if (PortTalk_Handle != INVALID_HANDLE_VALUE) {
			resources |= IO_DLL;
			CloseHandle(PortTalk_Handle);
      		}
      
      		DISPLAY(_("  IO_DLL: %s (PortTalk)\r\n"),
	      		resources & IO_DLL ? "available" : "not available");
    	}
    
    	/* TIUSB: for use with my SilverLink driver */
    
	hDLL = LoadLibrary("TiglUsb.DLL");
    	if (hDLL != NULL) {
      		resources |= IO_USB;
#ifndef __BORLANDC__
      		CloseHandle(hDLL);
#endif
    	}
    	DISPLAY(_("  IO_USB: %s (TiglUsb)\r\n"),
	    resources & IO_USB ? "available" : "not available");

	return 0;
}
