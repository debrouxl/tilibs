/*  ti_link - link program for TI calculators
 *  Copyright (C) 1999-2002  Romain Lievin
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
  This unit performs some auto-detection for:
  - Operating System
  - I/O ports such as parallel and serial ports
  - link cable type
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "str.h"
#include "intl.h"
#if defined(__LINUX__) || defined(__MACOSX__)
# include <dirent.h>
# include <sys/utsname.h> // for uname()
#elif defined(__WIN32__)
# include <windows.h>
# include <malloc.h>
# include <memory.h>
#endif

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "typedefs.h"
#include "cabl_ext.h"
#include "links.h"
#include "verbose.h"

void clear_portinfo_struct(PortInfo *pi)
{
  int i;

  pi->lpt_count = 1;
  for(i=0; i<MAX_LPT_PORTS; i++) 
    {
      pi->lpt_addr[i] = 0x0;
      pi->lpt_mode[i] = 0;
      strcpy((pi->lpt_name)[i], "");
    }
  pi->com_count = 1;
  for(i=0; i<MAX_COM_PORTS; i++)
    {
      pi->com_addr[i] = 0x0;
      pi->com_mode[i] = 0;
      strcpy((pi->com_name)[i], "");
    }
}

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
#if defined(__LINUX__) || defined(__MACOSX__)
  struct utsname buf;

  DISPLAY(_("Getting OS type...\r\n"));
  uname(&buf);
  DISPLAY(_("  System name: %s\r\n"), buf.sysname);
  DISPLAY(_("  Node name: %s\r\n"), buf.nodename);
  DISPLAY(_("  Release: %s\r\n"), buf.release);
  DISPLAY(_("  Version: %s\r\n"), buf.version);
  DISPLAY(_("  Machine: %s\r\n"), buf.machine);
#ifdef __LINUX__
  *os_type = "Linux";
#else
  *os_type = "Mac OS X";
#endif
  DISPLAY(_("Done.\r\n"));

  return 0;

#elif defined(__WIN32__) // Win9x/Me or WinNT/2000
     // Are we running Windows NT?
   OSVERSIONINFO os;
   
   DISPLAY(_("Getting OS type...\r\n"));
   memset(&os, 0, sizeof(OSVERSIONINFO));
   os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&os);
   DISPLAY(_("  Operating System: %s\r\n"), 
	   (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ?  
	   "Windows9x" : "WindowsNT");
   DISPLAY(_("  Version: major=%i, minor=%i\r\n"), 
		os.dwMajorVersion, os.dwMinorVersion);
	if(os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		*os_type = "Windows9x";
	}
	else if(os.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		*os_type = "WindowsNT";
	}
	else 
		*os_type = _("unknown");
	DISPLAY(_("Done.\r\n"));

#elif defined(__WIN16__)
  *os_type = "Windows3.1";
  return 0;

#elif defined(__DOS__)
  *os_type = "DOS";
  return 0;

#else
  *os_type = _("unknown");
  return -1;

#endif

  return 0;
}

/* Described further in the source code */
int DetectPortsLinux(PortInfo *pi);
int DetectPortsWindows(PortInfo *pi);
static int ticable_detect_mouse(PortInfo *pi);

/* 
   This function detects which ports are available according to
   the operating system type.
   It detects parallel and serial ports.
*/
TIEXPORT int TICALL ticable_detect_port(PortInfo *pi)
{
  int ret = 0;

  clear_portinfo_struct(pi);

#if defined(__LINUX__)
  ret = DetectPortsLinux(pi);
#elif defined(__WIN32__)
  ret = DetectPortsWindows(pi);
#else
  ret = -1;
#endif
  ticable_detect_mouse(pi);

  return ret;
}

/*
  Detect ports on Linux OS
*/
int DetectPortsLinux(PortInfo *pi)
{
#if defined(__LINUX__)
  int fd;
  FILE *f;
  char buffer[MAXCHARS];
  char info[MAXCHARS];
  int i, j;
  DIR *dir;
  struct dirent *file;
  int res;
  char path[MAXCHARS] = "/proc/sys/dev/parport/";

  /* Use /proc/sys/dev/parport/parportX/base-addr where X=0, 1, ... */  
  DISPLAY(_("Probing parallel ports...\r\n"));
  if( (dir=opendir("/proc/sys/dev/parport/")) == NULL) 
    {
      DISPLAY(_("Error: unable to open this directory: '/proc/sys/dev/parport/'.\r\n"));
      return -1;
    }

  while( (file=readdir(dir)) != NULL) 
    {
      if(!strcmp(file->d_name, ".")) continue;
      if(!strcmp(file->d_name, "..")) continue;
      
      if(strstr(file->d_name, "parport"))
	{
	  res = sscanf(file->d_name, "parport%i", &i);
	  if(res == 1)
	    {
	      if(i >= MAX_LPT_PORTS-1)
		break;
	      strcpy(path, "/proc/sys/dev/parport/");
	      strcat(path, file->d_name);
	      strcat(path, "/");
	      strcat(path, "base-addr");
	      sprintf(pi->lpt_name[i], "/dev/par%i", i);
	      f = fopen(path, "rt");
	      if(f == NULL)
		{
		  DISPLAY_ERROR(_("Unable to open this entry: <%s>\r\n"),
			  path);
		}
	      else
		{
		  fscanf(f, "%i", &(pi->lpt_addr[i]));
		  DISPLAY(_("  %s at address 0x%03x\r\n"),
			  pi->lpt_name[i],
			  pi->lpt_addr[i]);
		  fclose(f);
		}
	    }
	  else 
	    {
	      DISPLAY_ERROR(_("Invalid parport entry: <%s>.\r\n"), 
				file->d_name);
	    }
	}
    }
  
  if(closedir(dir)==-1)
    {
      DISPLAY(_("Error: closedir\r\n"));
    }
  DISPLAY(_("Done.\r\n"));

  /* Use /proc/tty/driver/serial */
  DISPLAY(_("Probing serial ports...\r\n"));
  fd=access("/proc/tty/driver/serial", F_OK);
  if(fd<0)
    {
      DISPLAY_ERROR(_("The file '/proc/tty/driver/serial' does not exist. Unable to probe serial port.\r\n"));
      DISPLAY(_("Done.\r\n"));
      return -1;
    }
  
  f=fopen("/proc/tty/driver/serial", "rt");
  if(f == NULL)
    {
      DISPLAY_ERROR(_("Unable to open this entry: <%s>\r\n"),
	      "/proc/tty/driver/serial");
      return -1;
    }

  fgets(buffer, 256, f);
  for(i=0; i<MAX_COM_PORTS; i++)
    {
      fgets(buffer, 256, f);
      sscanf(buffer, "%i: uart:%s port:%03X ", &j, info, 
	     &((pi->com_addr)[i]));
      if(strcmp(info, "unknown"))
	{
	  sprintf(pi->com_name[i], "/dev/ttyS%i", j);
	  DISPLAY("  /dev/ttyS%i: %8s adr:%03X\r\n", j, info, 
		  pi->com_addr[i]);
	}
      else
	pi->com_addr[i] = 0;
    }
  DISPLAY(_("Done.\r\n"));
#endif
  return 0;
}

/***************************************/
/* Misc routines for Windows detection */
/***************************************/

/*
  About these routines: they comes from the TDLPortIO written by
  John Pappas (DiskDude) TDLPortIO <http://diskdude.cjb.net/>. 
  The detection routines comes in fact from:
  
  "Special thanks to Peter Holm <comtext3@post4.tele.dk> for his
  algorithm and code for detecting the number and addresses of the
  installed printer ports, on which the detection code below is based."
  
  I have added the detection of COM ports.	
*/

/*
  // Number of LPT ports on the system
  int  FLPTCount;     
  int	 FCOMCount;
  // List of port addresses installed on the system
  WORD FLPTAddress[MAX_LPT_PORTS+1];
  WORD FCOMAddress[MAX_COM_PORTS+1];
*/

#define FLPTCount	(pi->lpt_count)
#define FCOMCount	(pi->com_count)
#define FLPTAddress	(pi->lpt_addr)
#define FCOMAddress	(pi->com_addr)
#define FLPTName	(pi->lpt_name)
#define FCOMName	(pi->com_name)

//---------------------------------------------------------------------------
// DetectPorts9x()
//---------------------------------------------------------------------------
void DetectPorts9x(PortInfo *pi)
{
#if defined(__WIN32__)
   const char *BASE_KEY = "Config Manager\\Enum";
   const char *PROBLEM = "Problem";
   const char *ALLOCATION = "Allocation";
   const char *PORTNAME = "PortName";
   const char *HARDWARE_KEY = "HardwareKey";

   const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |
                                  KEY_QUERY_VALUE;

   HKEY CurKey;               // Current key when using the registry
   char KeyName[MAX_PATH];    // A key name when using the registry

   char **KeyList;            // List of keys
   DWORD KeyCount;            // Count of the number of keys in KeyList

	DWORD index = 0;
	FILETIME DummyFileTime;
	DWORD DummyLength = MAX_PATH;
	DWORD KeyIndex;
	BOOL HasProblem;
	DWORD DataType, DataSize;
	char PortName[MAX_PATH];
	int pos;


   // Clear the port count
   FLPTCount = 0;

   // Clear the port array
   for(index=0; index<=MAX_LPT_PORTS; index++)
      FLPTAddress[index] = 0;

   // Open the registry
   RegOpenKeyEx(HKEY_DYN_DATA, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey);

   // Grab all the key names under HKEY_DYN_DATA
   //
   // Do this by first counting the number of keys,
   // then creating an array big enough to hold them
   // using the KeyList pointer.

   KeyCount = 0;
   while (RegEnumKeyEx(
            CurKey, KeyCount++, KeyName, &DummyLength,
            NULL, NULL, NULL, &DummyFileTime
                       ) != ERROR_NO_MORE_ITEMS)
   {
      DummyLength = MAX_PATH;
   }

   
   KeyList = (char **)malloc((KeyCount+1) * sizeof(char *));	//KeyList = new char*[KeyCount];

   KeyCount = 0;
   DummyLength = MAX_PATH;
   while (RegEnumKeyEx(
            CurKey, KeyCount, KeyName, &DummyLength,
            NULL, NULL, NULL, &DummyFileTime
                       ) != ERROR_NO_MORE_ITEMS)
   {
      
	  KeyList[KeyCount] = (char *)malloc((DummyLength+1) * sizeof(char));	//KeyList[KeyCount] = new char[DummyLength+1];
      strcpy(KeyList[KeyCount], KeyName);
      DummyLength = MAX_PATH;
      KeyCount++;
   }

   // Close the key
   RegCloseKey(CurKey);

   // Cycle through all keys; looking for a string valued subkey called
   // 'HardWareKey' which is not NULL, and another subkey called 'Problem'
   // whose fields are all valued 0.
   for (KeyIndex=0; KeyIndex<KeyCount; KeyIndex++)
   {
      HasProblem = FALSE; // Is 'Problem' non-zero? Assume it is Ok

      // Open the key
      strcpy(KeyName, BASE_KEY);
      strcat(KeyName, "\\");
      strcat(KeyName, KeyList[KeyIndex]);

      if (RegOpenKeyEx(
            HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey
                        ) != ERROR_SUCCESS)
         continue;

      // Test for a 0 valued Problem sub-key,
      // which must only consist of raw data
      RegQueryValueEx(CurKey, PROBLEM, NULL, &DataType, NULL, &DataSize);
      if (DataType == REG_BINARY)
      {
         // We have a valid, binary "Problem" sub-key
         // Test to see if the fields are zero

         char HardwareSubKey[MAX_PATH];
               // Data from the "Hardware" sub-key

		 BYTE *Data = (BYTE *)malloc(DataSize * sizeof(BYTE));	//BYTE *Data = new BYTE[DataSize];
               // Data from "Problem" sub-key

         // Read the data from the "Problem" sub-key
         if (RegQueryValueEx(
                  CurKey, PROBLEM, NULL,
                  NULL, Data, &DataSize
                             ) == ERROR_SUCCESS)
         {
            // See if it has any problems
            for (index=0; index<DataSize; index++)
               HasProblem |= Data[index];
         }
         else
            HasProblem = TRUE; // No good

         free(Data);	//delete[] Data;
		 

         // Now try and read the Hardware sub-key
         DataSize = MAX_PATH;
         RegQueryValueEx(
            CurKey, HARDWARE_KEY, NULL, &DataType, HardwareSubKey, &DataSize
                         );
         if (DataType != REG_SZ)
            HasProblem = TRUE; // No good

         // Do we have no problem, and a non-null Hardware sub-key?
         if (!HasProblem && strlen(HardwareSubKey) > 0)
         {
            // Now open the key which is "pointed at" by HardwareSubKey
            RegCloseKey(CurKey);

            strcpy(KeyName, "Enum\\");
            strcat(KeyName, HardwareSubKey);
            if (RegOpenKeyEx(
                  HKEY_LOCAL_MACHINE, KeyName, 0, KEY_PERMISSIONS, &CurKey
                              ) != ERROR_SUCCESS)
               continue;

            // Now read in the PortName and obtain the LPT number from it
            DataSize = MAX_PATH;
            RegQueryValueEx(
               CurKey, PORTNAME, NULL, &DataType, PortName, &DataSize
                            );
            if (DataType != REG_SZ)
               strcpy(PortName, ""); // No good

            // Make sure it has LPT in it
            if (strstr(PortName, "LPT") != NULL)
            {
               int PortNumber;
                     // The nubmer of the port
               char PortNumberStr[MAX_PATH];
                     // Holds the number of the port as a string

               WORD Allocation[64];
                     // Holds the registry data for the port address allocation

               memset(PortNumberStr, '\0', MAX_PATH);
               strncpy(PortNumberStr,
                       strstr(PortName, "LPT")+3,
                       strlen(PortName)-(strstr(PortName, "LPT")-PortName)-2);

               // Find the port number
               PortNumber = atoi(PortNumberStr);

               // Find the address
               RegCloseKey(CurKey);

               strcpy(KeyName, BASE_KEY);
               strcat(KeyName, "\\");
               strcat(KeyName, KeyList[KeyIndex]);
               RegOpenKeyEx(HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey);

               DataSize = sizeof(Allocation);
               RegQueryValueEx(
                  CurKey, ALLOCATION, NULL, &DataType,
                  (unsigned char*)Allocation, &DataSize
                               );
               if (DataType == REG_BINARY)
               {
                  // Decode the Allocation data: the port address is present
                  // directly after a 0x000C entry (which does not have 0x0000
                  // after it).
                  for (pos=0; pos<63; pos++)
                     if (Allocation[pos] == 0x000C &&
                         Allocation[pos+1] != 0x0000 &&
                         PortNumber<=MAX_LPT_PORTS)
                     {
                        // Found a port; add it to the list
                        FLPTAddress[PortNumber] = Allocation[pos+1];
                        FLPTCount++;
						sprintf(FLPTName[PortNumber], "LPT%i", PortNumber);
						DISPLAY("  %s at 0x%03x\r\n", 
								FLPTName[PortNumber], FLPTAddress[PortNumber]);

                        break;
                     }

               }

            }

			// Make sure it has COM in it
            if (strstr(PortName, "COM") != NULL)
            {
               int PortNumber;
                     // The nubmer of the port
               char PortNumberStr[MAX_PATH];
                     // Holds the number of the port as a string

               WORD Allocation[64];
                     // Holds the registry data for the port address allocation

               memset(PortNumberStr, '\0', MAX_PATH);
               strncpy(PortNumberStr,
                       strstr(PortName, "COM")+3,
                       strlen(PortName)-(strstr(PortName, "COM")-PortName)-2);

               // Find the port number
               PortNumber = atoi(PortNumberStr);

               // Find the address
               RegCloseKey(CurKey);

               strcpy(KeyName, BASE_KEY);
               strcat(KeyName, "\\");
               strcat(KeyName, KeyList[KeyIndex]);
               RegOpenKeyEx(HKEY_DYN_DATA, KeyName, 0, KEY_PERMISSIONS, &CurKey);

               DataSize = sizeof(Allocation);
               RegQueryValueEx(
                  CurKey, ALLOCATION, NULL, &DataType,
                  (unsigned char*)Allocation, &DataSize
                               );
               if (DataType == REG_BINARY)
               {
                  // Decode the Allocation data: the port address is present
                  // directly after a 0x000C entry (which does not have 0x0000
                  // after it).
                  for (pos=0; pos<63; pos++)
                     if (Allocation[pos] == 0x000C &&
                         Allocation[pos+1] != 0x0000 &&
                         PortNumber<=MAX_COM_PORTS)
                     {
                        // Found a port; add it to the list
                        FCOMAddress[PortNumber] = Allocation[pos+1];
                        FCOMCount++;
						sprintf(FCOMName[PortNumber], "COM%i", PortNumber);
						DISPLAY("  %s at 0x%03x\r\n", 
								FCOMName[PortNumber], FCOMAddress[PortNumber]);
                        break;
                     }

               }

            }

         }
      }

      RegCloseKey(CurKey);
   }
   // Destroy our key list
   for (index=0; index<KeyCount; index++)
      free(KeyList[index]);		//delete[] KeyList[index];
   free(KeyList);	//delete KeyList;
#endif
}

//---------------------------------------------------------------------------
// DetectPortsNT1()
//---------------------------------------------------------------------------
void DetectPortsNT1(PortInfo *pi)
{
#if defined(__WIN32__)
  const char *BASE_KEY = "HARDWARE\\DEVICEMAP\\PARALLEL PORTS";
   const char *LOADED_KEY = "HARDWARE\\RESOURCEMAP\\LOADED PARALLEL DRIVER RESOURCES\\Parport";
   const char *DOS_DEVICES = "\\DosDevices\\LPT";
   const char *DEVICE_PARALLEL = "\\Device\\Parallel";

   const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |
                                  KEY_QUERY_VALUE;

   HKEY CurKey;               // Current key when using the registry
   char KeyName[MAX_PATH];    // A key name when using the registry

   char **ValueList;          // List of value names
   DWORD ValueCount;          // Count of the number of value names in ValueList

	DWORD index;
	DWORD DummyLength = MAX_PATH;
   DWORD ValueType;

   // Clear the port count
   FLPTCount = 0;

   // Clear the port array
   for (index=0; index<=MAX_LPT_PORTS; index++)
      FLPTAddress[index] = 0;

   // Open the registry
   if (RegOpenKeyEx(
         HKEY_LOCAL_MACHINE, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey
                     ) != ERROR_SUCCESS)
     return; /* Can't do anything without this BASE_KEY */

   // Grab all the value names under HKEY_LOCAL_MACHINE
   //
   // Do this by first counting the number of value names,
   // then creating an array big enough to hold them
   // using the ValueList pointer.
   ValueCount = 0;
   while (RegEnumValue(
            CurKey, ValueCount++, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
      DummyLength = MAX_PATH;
   }

   ValueList = (char **)malloc((ValueCount+1) * sizeof(char *));	//ValueList = new char*[ValueCount];

   ValueCount = 0;
   DummyLength = MAX_PATH;
   while (RegEnumValue(
            CurKey, ValueCount, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
	   ValueList[ValueCount] = (char *)malloc((DummyLength+1) * sizeof(char));	//ValueList[ValueCount] = new char[DummyLength+1];
      strcpy(ValueList[ValueCount], KeyName);
      DummyLength = MAX_PATH;
      ValueCount++;
   }

   // Close the key
   RegCloseKey(CurKey);

   for (index=0; index<ValueCount; index++)
   {
      char DosDev[MAX_PATH];    // Key value for \DosDevices\LPT
      DWORD DataType, DataSize; // Type and size of data read from the registry

      // Is it a \DosDevices\LPT key?
      strcpy(KeyName, BASE_KEY);
      if (RegOpenKeyEx(
            HKEY_LOCAL_MACHINE, KeyName, 0, KEY_PERMISSIONS, &CurKey
                        ) == ERROR_SUCCESS)
      {
         DataSize = MAX_PATH;
         RegQueryValueEx(
            CurKey, ValueList[index], NULL, &DataType, DosDev, &DataSize
                         );
         RegCloseKey(CurKey);

         // Make sure it was a string
         if (DataType != REG_SZ)
            strcpy(DosDev, "");
      }
      else
         strcpy(DosDev, "");

      if (strstr(DosDev, DOS_DEVICES) != NULL)
      {
         int PortNumber;                  // The nubmer of the port
         char PortNumberStr[MAX_PATH];    // String version of PortNumber
         char PortIDStr[MAX_PATH];        // PortID

         memset(PortNumberStr, '\0', MAX_PATH);
         strncpy(PortNumberStr,
                 strstr(DosDev, DOS_DEVICES) + strlen(DOS_DEVICES),
                 strlen(DosDev) - (strstr(DosDev, DOS_DEVICES)-DosDev)
                                - strlen(DOS_DEVICES) + 1
                 );

         // Get the Port ID
         memset(PortIDStr, '\0', MAX_PATH);
         strncpy(PortIDStr,
                 strstr(ValueList[index], DEVICE_PARALLEL) + strlen(DEVICE_PARALLEL),
                 strlen(ValueList[index])
                     - (strstr(ValueList[index], DEVICE_PARALLEL)-ValueList[index])
                     - strlen(DEVICE_PARALLEL) + 1
                 );

         // Get the port number
         PortNumber = atoi(PortNumberStr);

         // Get the port address
         RegOpenKeyEx(HKEY_LOCAL_MACHINE, LOADED_KEY, 0, KEY_PERMISSIONS, &CurKey);

         strcpy(DosDev, "\\Device\\ParallelPort");
         strcat(DosDev, PortIDStr);
         strcat(DosDev, ".Raw");

         if (RegQueryValueEx(
               CurKey, DosDev, NULL, &DataType, NULL, NULL
                             ) == ERROR_SUCCESS &&
             DataType == REG_RESOURCE_LIST)
         {
            WORD Allocation[64]; // Binary data with port number inside

            // Read in the binary data
            DataSize = sizeof(Allocation);
            RegQueryValueEx(
               CurKey, DosDev, NULL, NULL,
               (unsigned char*)Allocation, &DataSize
                            );

            // Found a port; add it to the list
            if (DataSize>0 && PortNumber<=MAX_LPT_PORTS)
            {
               FLPTAddress[PortNumber] = Allocation[12];
               FLPTCount++;
			   sprintf(FLPTName[PortNumber], "LPT%i", PortNumber);
			   DISPLAY("  %s at 0x%03x\r\n", 
								FLPTName[PortNumber], FLPTAddress[PortNumber]);
            }
         }

         RegCloseKey(CurKey);
      }
   }

   // Destroy our key value list
   for (index=0; index<ValueCount; index++)
      free(ValueList[index]);	//delete[] ValueList[index];
   free(ValueList);	//delete ValueList;
#endif
}

//---------------------------------------------------------------------------
// DetectPortsNT2()
//---------------------------------------------------------------------------
void DetectPortsNT2(PortInfo *pi)
{
#if defined(__WIN32__)
  const char *BASE_KEY = "HARDWARE\\DEVICEMAP\\SERIALCOMM";
   const char *LOADED_KEY = "HARDWARE\\RESOURCEMAP\\LOADED SERIAL DRIVER RESOURCES\\Serial";
   const char *DOS_DEVICES = "COM";
   const char *DEVICE_PARALLEL = "Serial";

   const REGSAM KEY_PERMISSIONS = KEY_ENUMERATE_SUB_KEYS |
                                  KEY_QUERY_VALUE;

   HKEY CurKey;               // Current key when using the registry
   char KeyName[MAX_PATH];    // A key name when using the registry

   char **ValueList;          // List of value names
   DWORD ValueCount;          // Count of the number of value names in ValueList

	DWORD index;
	DWORD DummyLength = MAX_PATH;
   DWORD ValueType;

   // Clear the port count
   FCOMCount = 0;

   // Clear the port array
   for (index=0; index<=MAX_COM_PORTS; index++)
      FCOMAddress[index] = 0;

   // Open the registry
   if (RegOpenKeyEx(
         HKEY_LOCAL_MACHINE, BASE_KEY, 0, KEY_PERMISSIONS, &CurKey
                     ) != ERROR_SUCCESS)
      return; // Can not do anything without this BASE_KEY

   // Grab all the value names under HKEY_LOCAL_MACHINE
   //
   // Do this by first counting the number of value names,
   // then creating an array big enough to hold them
   // using the ValueList pointer.
   ValueCount = 0;
   while (RegEnumValue(
            CurKey, ValueCount++, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
      DummyLength = MAX_PATH;
   }

   ValueList = (char **)malloc((ValueCount+1) * sizeof(char *));	//ValueList = new char*[ValueCount];

   ValueCount = 0;
   DummyLength = MAX_PATH;
   while (RegEnumValue(
            CurKey, ValueCount, KeyName, &DummyLength,
            NULL, &ValueType, NULL, NULL
                       ) != ERROR_NO_MORE_ITEMS)
   {
	   ValueList[ValueCount] = (char *)malloc((DummyLength+1) * sizeof(char));	//ValueList[ValueCount] = new char[DummyLength+1];
      strcpy(ValueList[ValueCount], KeyName);
      DummyLength = MAX_PATH;
      ValueCount++;
   }

   // Close the key
   RegCloseKey(CurKey);

   for (index=0; index<ValueCount; index++)
   {
      char DosDev[MAX_PATH];    // Key value for \DosDevices\COM
      DWORD DataType, DataSize; // Type and size of data read from the registry

      // Is it a \DosDevices\LPT key?
      strcpy(KeyName, BASE_KEY);
      if (RegOpenKeyEx(
            HKEY_LOCAL_MACHINE, KeyName, 0, KEY_PERMISSIONS, &CurKey
                        ) == ERROR_SUCCESS)
      {
         DataSize = MAX_PATH;
         RegQueryValueEx(
            CurKey, ValueList[index], NULL, &DataType, DosDev, &DataSize
                         );
         RegCloseKey(CurKey);

         // Make sure it was a string
         if (DataType != REG_SZ)
            strcpy(DosDev, "");
      }
      else
         strcpy(DosDev, "");

      if (strstr(DosDev, DOS_DEVICES) != NULL)
      {
         int PortNumber;                  // The nubmer of the port
         char PortNumberStr[MAX_PATH];    // String version of PortNumber
         char PortIDStr[MAX_PATH];        // PortID

         memset(PortNumberStr, '\0', MAX_PATH);
         strncpy(PortNumberStr,
                 strstr(DosDev, DOS_DEVICES) + strlen(DOS_DEVICES),
                 strlen(DosDev) - (strstr(DosDev, DOS_DEVICES)-DosDev)
                                - strlen(DOS_DEVICES) + 1
                 );

         // Get the Port ID
         memset(PortIDStr, '\0', MAX_PATH);
         strncpy(PortIDStr,
                 strstr(ValueList[index], DEVICE_PARALLEL) + strlen(DEVICE_PARALLEL),
                 strlen(ValueList[index])
                     - (strstr(ValueList[index], DEVICE_PARALLEL)-ValueList[index])
                     - strlen(DEVICE_PARALLEL) + 1
                 );

         // Get the port number
         PortNumber = atoi(PortNumberStr);

         // Get the port address
         RegOpenKeyEx(HKEY_LOCAL_MACHINE, LOADED_KEY, 0, KEY_PERMISSIONS, &CurKey);

         strcpy(DosDev, "\\Device\\Serial");
         strcat(DosDev, PortIDStr);
         strcat(DosDev, ".Raw");

         if (RegQueryValueEx(
               CurKey, DosDev, NULL, &DataType, NULL, NULL
                             ) == ERROR_SUCCESS &&
             DataType == REG_RESOURCE_LIST)
         {
            WORD Allocation[64]; // Binary data with port number inside

            // Read in the binary data
            DataSize = sizeof(Allocation);
            RegQueryValueEx(
               CurKey, DosDev, NULL, NULL,
               (unsigned char*)Allocation, &DataSize
                            );

            // Found a port; add it to the list
            if (DataSize>0 && PortNumber<=MAX_COM_PORTS)
            {
               FCOMAddress[PortNumber] = Allocation[12];
               FCOMCount++;
			   sprintf(FCOMName[PortNumber], "COM%i", PortNumber);
			   DISPLAY("  %s at 0x%03x\r\n", 
								FCOMName[PortNumber], FCOMAddress[PortNumber]);
            }
         }

         RegCloseKey(CurKey);
      }
   }

   // Destroy our key value list
   for (index=0; index<ValueCount; index++)
      free(ValueList[index]);	//delete[] ValueList[index];
   free(ValueList);	//delete ValueList;
#endif
}

/*
	Detect ports on Win32 OS
*/
int DetectPortsWindows(PortInfo *pi)
{
#if defined(__WIN32__)
   BOOL RunningWinNT;
   OSVERSIONINFO os;

   DISPLAY(_("Probing ports...\r\n"));
   // Are we running Windows NT?
   memset(&os, 0, sizeof(OSVERSIONINFO));
   os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&os);
   RunningWinNT=(os.dwPlatformId==VER_PLATFORM_WIN32_NT);

   // Detect the printer ports available
   if (RunningWinNT)
   {
     DetectPortsNT1(pi); // WinNT version
     DetectPortsNT2(pi); // WinNT version
   }
   else
      DetectPorts9x(pi); // Win9x version
   DISPLAY(_("Done\r\n"));
#endif
   return 0;
}

/*
  Try to detect a mouse. This avoid to hang up it when we try to detect
  a link cable.
 */
static int ticable_detect_mouse(PortInfo *pi)
{
#ifdef __LINUX__
  FILE *f;
  char buffer[MAXCHARS];
  int found = 0;

  f = fopen("/proc/devices", "rt");
  if(f == NULL)
    {
      return -1;
    }
  while(!feof(f))
    {
      fscanf(f, "%s", buffer);
      if(strstr(buffer, "ps2"))
	{
	  found = 1;
	}
    }
  fclose(f);
  found = 1; // to improve...
  if(found)
    DISPLAY(_("A PS/2 mouse has been found, no serial port used.\r\n"));
  else
    {
      DISPLAY(_("No PS/2 mouse found, the first serial port may be used by mouse.\r\n"));
      pi->com_addr[0] = 0;
    }

  return 0;

#endif

  return 0;
}

char* result(int i)
{
  return ((i == 0) ? _("ok") : _("nok"));
}

/*
  This function tries to detect a link cable on the listed ports.
  The returned value is placed in pi.
  
  Beware: this routine can hang up your mouse if you have a mouse connected 
  on a serial port other than the first one (under Linux or Windows9x, 
  not NT4/2000)
*/
TIEXPORT int TICALL ticable_detect_cable(PortInfo *pi)
{
  int i;
  int res;
  
  //tcl.exit();
  DISPLAY(_("Probing link cables on each port...\r\n"));
  for(i=0; i<MAX_LPT_PORTS; i++)
    {
      if(pi->lpt_addr[i] != 0)
	{
	  DISPLAY("  Probing on %s at 0x%03x :\r\n",
		  pi->lpt_name[i], pi->lpt_addr[i]);
	  
	  io_address = pi->lpt_addr[i];
	  par_init();
	  res = par_probe();
	  pi->lpt_mode[i]=LINK_PAR;
	  DISPLAY("    parallel cable (%s)\r\n", result(res));
	}
    }
  for(i=0; i<MAX_COM_PORTS; i++)
    {
      if( (pi->com_addr[i] != 0))
	{
	  DISPLAY("  Probing on %s at 0x%03x :\r\n",
		  pi->com_name[i], pi->com_addr[i]);

	  io_address = pi->com_addr[i];
	  ser_init(); ser_open();
	  res = ser_probe();
	  pi->com_mode[i]=LINK_SER;
	  ser_close(); ser_exit();
	  DISPLAY("    serial cable (%s)\r\n", result(res));
	  
	  strcpy(io_device, pi->com_name[i]);
	  tig_init(); tig_open();
	  res = tig_probe();
	  pi->com_mode[i]=LINK_TGL;
	  tig_close(); tig_exit();
	  DISPLAY("    GreyTIGL cable (%s)\r\n", result(res));
	  
	  strcpy(io_device, pi->com_name[i]);
	  avr_init(); avr_open();
	  res = avr_probe();
	  pi->com_mode[i]=LINK_AVR;
	  avr_close(); avr_exit();
	  DISPLAY("    AVRlink cable (%s)\r\n", result(res));
	}
    }

  return 0;
}

/*
  This function tries to detect a link cable.
  The returned value is placed in os and pi.
*/
int TICALL ticable_detect_all(char **os, PortInfo *pi)
{
  if(!ticable_detect_os(os))
    { if(!ticable_detect_port(pi))
      { if(!ticable_detect_mouse(pi))
	{ if(!ticable_detect_cable(pi))
	  { return 0;
	  }
	}
      }
    }
  else
    return -1;

  return 0;
}


/******************/
/* Misc functions */
/******************/

#ifdef __LINUX__

/*
  Internal use.
  Try to find a specific string in /proc (vfs)
 */
static int find_string_in_proc(char *entry, char *str)
{
  FILE *f;
  char buffer[MAXCHARS];
  int found = 0;

  f = fopen(entry, "rt");
  if(f == NULL)
    {
      return -1;
    }
  while(!feof(f))
    {
      fscanf(f, "%s", buffer);
      if(strstr(buffer, str))
	{
	  found = 1;
	}
    }
  fclose(f);

  return found;
}

#endif

extern int resources; // defined in intrface.c

int list_io_resources(void)
{
  char *os;
  
  ticable_detect_os(&os);
  /* Windows part */
  
#ifdef __WIN32__
  {
    HINSTANCE hDLL = NULL;
    
    if(!strcmp(os, "Windows9x"))
      resources = IO_WIN9X;
    else if(!strcmp(os, "WindowsNT"))
      resources = IO_WINNT;
    else return ERR_NO_RESOURCES;
    
    DISPLAY(_("Libticables: checking resources...\r\n"));
    
    resources |= (IO_API | IO_DCB);
    DISPLAY(_("  IO_API: ok\r\n"));
    DISPLAY(_("  IO_DCB: ok\r\n"));
    if(!strcmp(os, "Windows9x"))
      resources |= IO_ASM;
    DISPLAY(_("  IO_ASM: %s\r\n"), resources & IO_ASM ? "ok" : "nok");
    
    // Open PortTalk Driver
    hDLL = CreateFile("\\\\.\\PortTalk", 
		      GENERIC_READ, 
		      0, 
		      NULL,
		      OPEN_EXISTING, 
		      FILE_ATTRIBUTE_NORMAL, 
		      NULL);

    if(hDLL != INVALID_HANDLE_VALUE) 
      {
	resources |= IO_DLL;
	CloseHandle(hDLL);
      }
    DISPLAY(_("  IO_DLL: %s (PortTalk)\r\n"), resources & IO_DLL ? "ok" : "nok");
    
    // Open TiglUsb Driver
    hDLL = LoadLibrary("TiglUsb.DLL");
    if (hDLL != NULL)
      {
	resources |= IO_LIBUSB;
	//CloseHandle(hDLL);
      }
    DISPLAY(_("  IO_LIBUSB: %s (TiglUsb)\r\n"), resources & IO_LIBUSB ? "ok" : "nok");
    
  }
#endif

  /* Linux part */

#ifdef __LINUX__
  DISPLAY(_("Libticables: checking resources...\r\n"));

  resources = IO_LINUX;
  resources |= IO_API;  
  DISPLAY(_("  IO_API: ok\r\n"));

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
  { // check super or normal user
    uid_t uid = getuid();
    if(uid != 0)
      {
	DISPLAY(_("  IO_ASM: nok (normal user -> kernel module)\r\n"));
	resources &= ~IO_ASM;
      }
    else
      {
	DISPLAY(_("  IO_ASM: ok (super user)\r\n"));
	resources |= IO_ASM;
      }
  }
#endif
  
  if(find_string_in_proc("/proc/devices", "tipar") ||
     find_string_in_proc("/proc/modules", "tipar"))
    resources |= IO_TIPAR;
    DISPLAY(_("  IO_TIPAR: %s\r\n"), resources & IO_TIPAR ? "ok" : "nok");

  if(find_string_in_proc("/proc/devices", "tiser") ||
     find_string_in_proc("/proc/modules", "tiser"))
    resources |= IO_TISER;
  DISPLAY(_("  IO_TISER: %s\r\n"), resources & IO_TISER ? "ok" : "nok");

  if(find_string_in_proc("/proc/devices", "tiusb") ||
     find_string_in_proc("/proc/modules", "tiusb"))
    resources |= IO_TIUSB;
  DISPLAY(_("  IO_TIUSB: %s\r\n"), resources & IO_TIUSB ? "ok" : "nok");
  
#ifdef HAVE_LIBUSB
  resources |= IO_LIBUSB;
#endif
  DISPLAY(_("  IO_LIBUSB: %s\r\n"), resources & IO_LIBUSB ? "ok" : "nok");
#endif

#ifdef __MACOSX__
  DISPLAY(_("Libticables: checking resources...\r\n"));

  resources = IO_OSX;
#endif

  DISPLAY(_("Done.\r\n"));

  return 0;
}
