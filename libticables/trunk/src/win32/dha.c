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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


/*
	This unit provides DhaHelper access routines for NT I/O port accesses.
*/

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

#include "dha.h"
#include "./dha/dhahelper.h"

#ifdef __WIN32__

int dha_install(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	char szPath[MAX_PATH];
	int result = 0;

	GetWindowsDirectory(szPath, MAX_PATH);
	strcpy(szPath + strlen(szPath), "\\system32\\drivers\\" DRV_FILENAME);

    if(!CopyFile(DRV_FILENAME, szPath, FALSE))
	{
		printf("Copying " DRV_FILENAME " failed.\nEither " DRV_FILENAME " is not in the current directory or you lack sufficient\nprivileges to write to %s.", szPath);
		return -1;
    }

    // Install the driver
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    hService = CreateService(hSCManager,
                             DRV_NAME,
                             DRV_NAME,
                             SERVICE_ALL_ACCESS,
                             SERVICE_KERNEL_DRIVER,
                             SERVICE_SYSTEM_START,
                             SERVICE_ERROR_NORMAL,
                             szPath,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL);
    if(!hService)
	{
		printf("Unable to register " DRV_NAME " Service (0x%x).\n",GetLastError());
		result = -1;
    }
	else
	{
		printf("Success!\n");
		result = 0;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return result;
}

int dha_uninstall(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	char szPath[MAX_PATH];

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);

	dha_stop();

	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	GetWindowsDirectory(szPath, MAX_PATH);
	strcpy(szPath + strlen(szPath), "\\system32\\drivers\\" DRV_FILENAME);
	DeleteFile(szPath);

	return 0;
}

int dha_start(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);
	
	StartService(hService, 0, NULL);

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

int dha_stop(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS ServiceStatus;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);
    
	ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
    
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

int dha_detect(int* result)
{
	HANDLE hDriver;
	*result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDriver != INVALID_HANDLE_VALUE) 
	{
		*result = 1;
		CloseHandle(hDriver);
    }

	printf(DRV_NAME "%sfound.", *result ? " " : " not ");
	return 0;
}

#ifdef __WIN32__
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

        printf("%s (%i -> %s)", s, GetLastError(), lpMsgBuf);
}
#endif				//__WIN32__

static HINSTANCE hDriver = NULL;

int dha_enable(void)
{
	DWORD BytesReturned;
	int iError;
	int result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper",
		  GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	iError = DeviceIoControl(hDriver, IOCTL_DHAHELPER_ENABLEDIRECTIO, NULL, 0, NULL, 0, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("DhaHelper: error %d occured in IOCTL_DHAHELPER_ENABLEDIRECTIO");
		result = -1;
	}
	else
		printf("I/O ports have been granted access.");

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

int dha_disable(void)
{
	DWORD BytesReturned;
	int iError;
	int result = 0;

	hDriver = CreateFile("\\\\.\\DhaHelper",
		      GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	iError = DeviceIoControl(hDriver, IOCTL_DHAHELPER_DISABLEDIRECTIO, NULL, 0, NULL, 0, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("DhaHelper: error %d occured in IOCTL_DHAHELPER_DISABLEDIRECTIO");
		result = -1;
	}
	else
		printf("I/O ports have been locked access.");

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

#endif
