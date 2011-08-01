/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 2007  Romain Lievin
 *
 *  Some parts from dhasetup.c source code <http://svn.mplayerhq.hu/mplayer/trunk/vidix/dhahelperwin/>
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


/*
	This unit provides DhaHelper access routines for NT I/O port accesses.
*/

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

#include "dha.h"
#include "./dha/dhahelper.h"

#define DHA_DRV_NAME		"DhaHelper"

#if defined(__WIN32__) && !defined(_WIN64)

static void print_last_error(char *s)
{
        LPTSTR lpMsgBuf;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) & lpMsgBuf, 0, NULL);

		lpMsgBuf[strlen(lpMsgBuf)-2] = '\0';

        printf("%s (%i -> %s)\n", s, (int)GetLastError(), lpMsgBuf);
}

int dha_detect(int* result)
{
	HANDLE hDriver;
	*result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDriver == INVALID_HANDLE_VALUE) 
	{
		hDriver = CreateFile("\\\\.\\DhaHelper", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hDriver != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(hDriver);
			hDriver = INVALID_HANDLE_VALUE;
			*result = 1;
		}
    }
	else
	{
		CloseHandle(hDriver);
		hDriver = INVALID_HANDLE_VALUE;
		*result = 1;
	}

	printf(DHA_DRV_NAME "%sfound.\n", *result ? " " : " not ");
	return 0;
}

int dha_enable(void)
{
	HANDLE hDriver;
	DWORD BytesReturned;
	int iError;
	int result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper",
		  GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DHA_DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	iError = DeviceIoControl(hDriver, IOCTL_DHAHELPER_ENABLEDIRECTIO, NULL, 0, NULL, 0, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("DhaHelper: error %d occured in IOCTL_DHAHELPER_ENABLEDIRECTIO");
		result = -1;
	}
	else
		printf("I/O ports have been enabled.\n");

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

int dha_disable(void)
{
	HANDLE hDriver;
	DWORD BytesReturned;
	int iError;
	int result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper",
		      GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DHA_DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	iError = DeviceIoControl(hDriver, IOCTL_DHAHELPER_DISABLEDIRECTIO, NULL, 0, NULL, 0, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("DhaHelper: error %d occured in IOCTL_DHAHELPER_DISABLEDIRECTIO");
		result = -1;
	}
	else
		printf("I/O ports have been disabled.\n");

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

#endif
