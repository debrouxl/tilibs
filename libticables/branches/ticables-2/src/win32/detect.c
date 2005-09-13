/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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

/* Win32 probing module */

#include <windows.h>

#include "../logging.h"
#include "detect.h"
#include "porttalk.h"

#ifdef __MINGW32__
#include "porttalk.c"
#endif

int win32_detect_os(void)
{
	OSVERSIONINFO os;

  	memset(&os, 0, sizeof(OSVERSIONINFO));
  	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  	GetVersionEx(&os);
	/*
  	ticables_info("  operating System: %s",
	  (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ?
	  "Windows9x" : "WindowsNT");
  	ticables_info("  version: %i.%i\n",
	  os.dwMajorVersion, os.dwMinorVersion);
	*/
  	if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return WIN_9X;
  	else if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return WIN_NT;
  	else
		return WIN_NT;

	return 0;
}

int win32_detect_porttalk(void)
{
	HANDLE PortTalk_Handle;	/* Handle for PortTalk Driver */
	int result = 0;

	PortTalk_Handle = CreateFile("\\\\.\\PortTalk",
				   GENERIC_READ,
				   0,
				   NULL,
				   OPEN_EXISTING,
				   FILE_ATTRIBUTE_NORMAL, NULL);

    if (PortTalk_Handle == INVALID_HANDLE_VALUE) 
	{
		// Start or Install PortTalk Driver
		PortTalkStartDriver();

		// Don't be too hurry !
		Sleep(1000);

		// Then try to open once more, before failing
		PortTalk_Handle =
			CreateFile("\\\\.\\PortTalk",
				GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (PortTalk_Handle == INVALID_HANDLE_VALUE) 
		{
	  		ticables_info("PortTalk: Couldn't access PortTalk Driver, Please ensure driver is loaded.");
	  		//return -1;
		}
    }

    if (PortTalk_Handle != INVALID_HANDLE_VALUE) 
	{
		result = 1;
		CloseHandle(PortTalk_Handle);
    }

	ticables_info("PorTalk%sfound.", result ? " " : " not ");

	return result;
}

int win32_detect_tiglusb(void)
{
	HINSTANCE hDll = NULL;	/* Handle for TiglUsb driver */
	int result = 0;

	hDll = LoadLibrary("TiglUsb.DLL");
    if (hDll != NULL) 
	{
		result = 1;
        FreeLibrary(hDll);
	}

    ticables_info("TiglUsb driver%sfound.", result ? " " : " not ");
	
	return result;
}
