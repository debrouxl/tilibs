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

#include "../stdints.h"

#include "../gettext.h"
#include "../cabl_def.h"
#include "../cabl_err.h"
#include "../export.h"
#include "../externs.h"
#include "../printl.h"

#include "porttalk.h"

#include "win32_detect.h"

int win32_detect_os(char **os_type)
{
	OSVERSIONINFO os;

  	printl1(0, _("getting OS type:\n"));
  	
  	memset(&os, 0, sizeof(OSVERSIONINFO));
  	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  	GetVersionEx(&os);
  	printl1(0, _("  operating System: %s\n"),
	  (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ?
	  "Windows9x" : "WindowsNT");
  	printl1(0, _("  version: %i.%i\n"),
	  os.dwMajorVersion, os.dwMinorVersion);
  
  	if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
    		*os_type = OS_WIN9X;
  	} else if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    		*os_type = OS_WINNT;
  	} else
    		*os_type = _("unknown");

	return 0;
}

int win32_detect_port(TicablePortInfo * pi)
{
	// I will put them later...

	return 0;
}

int win32_detect_resources(void)
{
	HINSTANCE hDll = NULL;	/* Handle for TiglUsb driver */
    HANDLE PortTalk_Handle;	/* Handle for PortTalk Driver */
	char *os;
    	
	win32_detect_os(&os);
	printl1(0, _("checking resources:\n"));
	
	/* Windows NT need permissions */
	
	if (!strcmp(os, "Windows9x"))
      		resources = IO_WIN9X;
    	else if (!strcmp(os, "WindowsNT"))
      		resources = IO_WINNT;
    	else
      		return ERR_NO_RESOURCES;

    	/* API: for use with COMx */
    	
    	resources |= IO_API;
    	printl1(0, _("  IO_API: %sfound at compile time.\n"),
		resources & IO_API ? "" : "not ");

	/* ASM: for use with low-level I/O and Win9x */
	
    	if (!strcmp(os, "Windows9x")) {
      		resources |= IO_ASM;
      		printl1(0, _("  IO_ASM: available (Win9x).\n"));
	} else {
		printl1(0, _("  IO_ASM: not available (WinNT).\n"));
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
	  			printl1(2, "PortTalk: Couldn't access PortTalk Driver, Please ensure driver is loaded.\n\n");
	  			//return -1;
			}
      		}

      		if (PortTalk_Handle != INVALID_HANDLE_VALUE) {
			resources |= IO_DLL;
			CloseHandle(PortTalk_Handle);
      		}
      
      		printl1(0, _("  IO_DLL: %s (PortTalk)\n"),
	      		resources & IO_DLL ? "available" : "not available");
    	}
    
    	/* TIUSB: for use with my SilverLink driver */
    
	hDll = LoadLibrary("TiglUsb.DLL");
    	if (hDll != NULL) {
      		resources |= IO_USB;
                FreeLibrary(hDll);
    	}
    	printl1(0, _("  IO_USB: %s (TiglUsb)\n"),
	    resources & IO_USB ? "available" : "not available");

	return 0;
}
