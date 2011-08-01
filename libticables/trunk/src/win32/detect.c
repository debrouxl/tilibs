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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Win32 probing module */

#include <windows.h>

#include "../error.h"
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

int win32_check_os(void)
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

int win32_check_dha(void)
{
#ifndef _WIN64
	int result = 0;

	dha_detect(&result);

	return result ? 0 : ERR_DHA_NOT_FOUND;
#else
	return 0;
#endif
}

int win32_check_rwp(void)
{
#ifdef _WIN64
	int result = 0;

	rwp_detect(&result);

	return result ? 0: ERR_RWP_NOT_FOUND;
#else
	return 0;
#endif
}

int win32_check_libusb(void)
{
	return 0;
}
