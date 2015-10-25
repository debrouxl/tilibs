/* Hey EMACS -*- win32-c -*- */
/* $Id: dha.c 3701 2007-08-07 18:03:36Z roms $ */

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
	This unit provides RwPorts access routines for NT-64 I/O port accesses.
*/

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

#include "rwp.h"
#include "./rwp/rwports.h"

#define RWP_DRV_NAME	"RwPorts"

#if defined(__WIN32__) || defined(_WIN64)

#if !defined(__MINGW32__) || defined(_WIN64)
/* Under MinGW, both dha.c and rwp.c are included into detect.c, so avoid the
   duplicate definition. */
static void print_last_error(const char *s)
{
        LPTSTR lpMsgBuf;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) & lpMsgBuf, 0, NULL);

		lpMsgBuf[strlen(lpMsgBuf)-2] = '\0';

        printf("%s (%lu -> %s)\n", s, (unsigned long)GetLastError(), lpMsgBuf);
}
#endif

int rwp_detect(int* result)
{
	HANDLE hDriver;
	*result = 0;

	hDriver = CreateFile("\\\\.\\RwPorts", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDriver == INVALID_HANDLE_VALUE) 
	{
		hDriver = CreateFile("\\\\.\\RwPorts", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	printf(RWP_DRV_NAME "%sfound.\n", *result ? " " : " not ");
	return 0;
}

static HANDLE hDriver = INVALID_HANDLE_VALUE;

int rwp_open(void)
{
	hDriver = CreateFile("\\\\.\\RwPorts",
		  GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " RWP_DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	return 0;
}

int rwp_close(void)
{
	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return 0;
}

int rwp_read_byte(unsigned short address, unsigned char *data)
{
	DWORD BytesReturned;
	int iError;
	int result = 0;
	unsigned char buf[3];

	*((unsigned short *)buf) = (unsigned short)address;
	iError = DeviceIoControl(hDriver, IOCTL_RWPORTS_READ_BYTE, &buf, 2, &buf, 1, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("RwPorts: error occured in IOCTL_RWPORTS_READ_BYTE");
		result = -1;
	}
	else
	{
		*data = buf[0];
		//printf("I/O ports read at 0x%04x: %02x\n", address, *data);
	}

	return result;
}

int rwp_write_byte(unsigned short address, unsigned char data)
{
	DWORD BytesReturned;
	int iError;
	int result = 0;
	unsigned char buf[3];

	*((unsigned short *)buf) = (unsigned short)address;	buf[2] = data;
	iError = DeviceIoControl(hDriver, IOCTL_RWPORTS_WRITE_BYTE, &buf, 3, &buf, 1, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("RwPorts: error occured in IOCTL_RWPORTS_WRITE_BYTE");
		result = -1;
	}
	//else printf("I/O ports write at 0x%04x.\n", address);

	return result;
}

#endif
