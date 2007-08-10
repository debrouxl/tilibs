/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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

#include "../win32/dha.h"
#include "../win64/rwp.h"

#ifdef __MINGW32__
#include "../win32/dha.c"
#include "../win64/rwp.c"
#endif

// Note: the kernel version of XP x64 is 5.2, like Windows 2003, not 5.1 as 
// Windows XP 32-Bit.

int win32_detect_os(void)
{
	OSVERSIONINFO os;
	SYSTEM_INFO si;

	GetSystemInfo(&si);	// should use GetNativeSystemInfo(&si); & si.wProcessorArchitecture but MSVC too old
	if(si.dwProcessorType > 586)
		return WIN_64;

  	memset(&os, 0, sizeof(OSVERSIONINFO));
  	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  	GetVersionEx(&os);

  	if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return WIN_9X;
  	else if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return WIN_NT;
  	else
		return WIN_NT;

	return 0;
}

int win32_detect_dha(void)
{
	int result = 0;
	int ret = 0;

	ret = dha_detect(&result);
	if(ret) 
		return 0;

	return result;
}

int win32_detect_rwp(void)
{
	int result = 0;
	int ret = 0;

	ret = rwp_detect(&result);
	if(ret) 
		return 0;

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
	if(!result)
		ticables_info("TiglUsb support will be unavailable...");
	
	return result;
}
