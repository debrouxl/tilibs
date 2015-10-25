/*  rwpsetup - RWPorts setup program
 *
 *  Copyright (C) 2007 Romain Lievin
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

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

#include "rwports.h"

#define DRV_NAME		"RwPorts"
#define DRV_FILENAME	"rwports.sys"
#define DRV_VERSION		"1.0"

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

static int rwp_start(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	int result;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);
	
	result = StartService(hService, 0, NULL);
	if(!result) print_last_error("Error while starting service");

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

static int rwp_stop(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS ServiceStatus;
	int result;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);
    
	result = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
	if(!result) print_last_error("Error while stopping service");
    
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

static int rwp_install(void)
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
		print_last_error("Unable to register RwPorts Service");
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

static int rwp_uninstall(void)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	char szPath[MAX_PATH];
	int result = 0;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, DRV_NAME, SERVICE_ALL_ACCESS);

	rwp_stop();

	result = DeleteService(hService);
	if(!result) print_last_error("Error while deleting service");

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	GetWindowsDirectory(szPath, MAX_PATH);
	strcpy(szPath + strlen(szPath), "\\system32\\drivers\\" DRV_FILENAME);
	DeleteFile(szPath);

	return 0;
}

static int rwp_detect(int* result)
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

	printf(DRV_NAME "%sfound.\n", *result ? " " : " not ");
	return 0;
}

static int rwp_read_byte(unsigned long address, unsigned char *data)
{
	HANDLE hDriver;
	DWORD BytesReturned;
	int iError;
	int result = 0;

	unsigned char buf[3];

	hDriver = CreateFile("\\\\.\\RwPorts",
		  GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	*((unsigned short *)buf) = (unsigned short)address;
	iError = DeviceIoControl(hDriver, IOCTL_RWPORTS_READ_BYTE, &buf, 2, &buf, 1, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("RwPorts: error %d occured in IOCTL_RWPORTS_READ_BYTE");
		result = -1;
	}
	else
	{
		*data = buf[0];
		printf("I/O ports read at 0x%04lX: %02x\n", address, *data);
	}

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

static int rwp_write_byte(unsigned long address, unsigned char data)
{
	HANDLE hDriver;
	DWORD BytesReturned;
	int iError;
	int result = 0;
	unsigned char buf[3];

	hDriver = CreateFile("\\\\.\\RwPorts",
		      GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDriver == INVALID_HANDLE_VALUE) 
	{
		printf("Couldn't access " DRV_NAME " driver; Please ensure driver is installed/loaded.");
		return -1;
	}

	*((unsigned short *)buf) = (unsigned short)address;	buf[2] = data;
	iError = DeviceIoControl(hDriver, IOCTL_RWPORTS_WRITE_BYTE, &buf, 3, &buf, 1, &BytesReturned, NULL);
	if(!iError)
	{
		print_last_error("RwPorts: error %d occured in IOCTL_RWPORTS_WRITE_BYTE");
		result = -1;
	}
	else
		printf("I/O ports write at 0x%04lX.\n", address);

	CloseHandle(hDriver);
	hDriver = INVALID_HANDLE_VALUE;

	return result;
}

int main(int argc,char* argv[])
{
  SC_HANDLE hSCManager = NULL;
  SC_HANDLE hService = NULL;

  printf("rwpsetup (c) 2007 Romain Liévin\n");

  if(argc==1)
  {
	  char szPath[MAX_PATH];
	int result = 0;

	GetWindowsDirectory(szPath, MAX_PATH);
	strcpy(szPath + strlen(szPath), "\\system32\\drivers\\");

    printf("Usage:\n");
    printf("rwpsetup install - Copies rwports.sys from the current directory to system \n                    directory and configures it to start at boot.\n");
    printf("rwpsetup remove  - Removes the RwPorts driver.\n");
	printf("rwpsetup start   - Start driver.\n");
	printf("rwpsetup stop    - Stop driver.\n");
	printf("rwpsetup check   - Check if driver is present and loaded.\n");
	printf("\n");
	printf("System directory: %s\n", szPath);

    return 0;
  }

  if(!strcmp(argv[1], "install"))
  {
    printf("Installing RwPorts...\n");

	rwp_install();
	rwp_start();
  }
  else if(!strcmp(argv[1], "remove"))
  {
    printf("Removing RwPorts...\n");
    
	rwp_stop();
    rwp_uninstall();
  }
  else if(!strcmp(argv[1], "start"))
  {
	  printf("Starting RwPorts...\n");
	  rwp_start();
	  printf("Done!\n");
  }
  else if(!strcmp(argv[1], "stop"))
  {
	  printf("Stopping RwPorts...\n");
	  rwp_stop();
	  printf("Done!\n");
  }
  else if(!strcmp(argv[1], "check"))
  {
	  int result;

	  rwp_detect(&result);
	  printf("RwPorts driver is%spresent.\n", result ? " " : " not ");
  }
  else if(!strcmp(argv[1], "read"))
  {
	  unsigned char v = 0;

	  rwp_read_byte(0x378, &v);
	  printf("v = %02x\n", v);
  }
  else 
  {
    printf("unknown parameter: %s\n",argv[1]);
  }

  return 0;
}
