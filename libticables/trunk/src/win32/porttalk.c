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


/*
  This unit manages some operations with the PortTalk device driver for
  Windows NT.
*/

#include <windows.h>

#include "../gettext.h"
#include "../logging.h"

/****************************************************/
/* Utility functions for PortTalk driver			*/
/* Comes from Craig Peacock's AllowIo.exe program   */
/****************************************************/

#ifdef __WIN32__
void PortTalkInstallDriver(void)
{
  SC_HANDLE SchSCManager;
  SC_HANDLE schService;
  DWORD err;
  CHAR DriverFileName[80];

  /* Get Current Directory. Assumes PortTalk.SYS driver is in this directory.    */
  /* Doesn't detect if file exists, nor if file is on removable media - if this  */
  /* is the case then when windows next boots, the driver will fail to load and  */
  /* a error entry is made in the event viewer to reflect this */

  /* Get System Directory. This should be something like c:\windows\system32 or  */
  /* c:\winnt\system32 with a Maximum Character lenght of 20. As we have a       */
  /* buffer of 80 chars and a string of 24 chars to append, we can go for a max  */
  /* of 55 chars */

  if (!GetSystemDirectory(DriverFileName, 55)) {
    ticables_info(_(
    	 "PortTalk: Failed to get System Directory. Is System Directory Path > 55 Characters?\n"
	 "PortTalk: Please manually copy driver to your system32/driver directory."));
  }

  /* Append our Driver Name */
  lstrcat(DriverFileName, "\\Drivers\\PortTalk.sys");
  ticables_info(_("PortTalk: Copying driver to %s"), DriverFileName);

  /* Copy Driver to System32/drivers directory. This fails if the file doesn't exist. */

  if (!CopyFile("PortTalk.sys", DriverFileName, FALSE)) {
    ticables_info(_("PortTalk: Failed to copy driver to %s"),
		  DriverFileName);
    	ticables_info(_("PortTalk: Please manually copy driver to your system32/driver directory."));
  }

  /* Open Handle to Service Control Manager */
  SchSCManager = OpenSCManager(NULL,	/* machine (NULL == local) */
			       NULL,	/* database (NULL == default) */
			       SC_MANAGER_ALL_ACCESS);	/* access required */

  /* Create Service/Driver - This adds the appropriate registry keys in */
  /* HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services - It doesn't  */
  /* care if the driver exists, or if the path is correct.              */

  schService = CreateService(SchSCManager,	/* SCManager database */
			     "PortTalk",	/* name of service */
			     "PortTalk",	/* name to display */
			     SERVICE_ALL_ACCESS,	/* desired access */
			     SERVICE_KERNEL_DRIVER,	/* service type */
			     SERVICE_DEMAND_START,	/* start type */
			     SERVICE_ERROR_NORMAL,	/* error control type */
			     "System32\\Drivers\\PortTalk.sys",	/* service's binary */
			     NULL,	/* no load ordering group */
			     NULL,	/* no tag identifier */
			     NULL,	/* no dependencies */
			     NULL,	/* LocalSystem account */
			     NULL	/* no passuint16_t */
      );

  if (schService == NULL) {
    err = GetLastError();
    if (err == ERROR_SERVICE_EXISTS)
      ticables_info(_("PortTalk: Driver already exists. No action taken."));
    else
      ticables_info(_("PortTalk: Unknown error while creating Service."));
  } else
    ticables_info(_("PortTalk: Driver successfully installed."));

  /* Close Handle to Service Control Manager */
  CloseServiceHandle(schService);
}

unsigned char PortTalkStartDriver(void)
{
  SC_HANDLE SchSCManager;
  SC_HANDLE schService;
  BOOL ret;
  DWORD err;

  /* Open Handle to Service Control Manager */
  SchSCManager = OpenSCManager(NULL,	/* machine (NULL == local) */
			       NULL,	/* database (NULL == default) */
			       SC_MANAGER_ALL_ACCESS);	/* access required */

  if (SchSCManager == NULL)
    if (GetLastError() == ERROR_ACCESS_DENIED) {
      /* We do not have enough rights to open the SCM, therefore we must */
      /* be a poor user with only user rights. */
      ticables_info(_(
	   "PortTalk: You do not have rights to access the Service Control Manager and\n"
	   "PortTalk: the PortTalk driver is not installed or started. Please ask \n"
	   "PortTalk: your administrator to install the driver on your behalf."));
      return (0);
    }

  do {
    /* Open a Handle to the PortTalk Service Database */
    schService = OpenService(SchSCManager,	/* handle to service control manager database */
			     "PortTalk",	/* pointer to name of service to start */
			     SERVICE_ALL_ACCESS);	/* type of access to service */

    if (schService == NULL)
      switch (GetLastError()) {
      case ERROR_ACCESS_DENIED:
	ticables_info(_("PortTalk: You do not have rights to the PortTalk service database"));
	return (0);
      case ERROR_INVALID_NAME:
	ticables_info(_("PortTalk: The specified service name is invalid."));
	return (0);
      case ERROR_SERVICE_DOES_NOT_EXIST:
	ticables_info(_(
	     "PortTalk: The PortTalk driver does not exist. Installing driver.\n"
	     "PortTalk: This can take up to 30 seconds on some machines . ."));
	PortTalkInstallDriver();
	break;
      }
  } while (schService == NULL);

  /* Start the PortTalk Driver. Errors will occur here if PortTalk.SYS file doesn't exist */

  ret = StartService(schService,	/* service identifier */
		     0,		/* number of arguments */
		     NULL);	/* pointer to arguments */

  if (ret)
    ticables_info(_("PortTalk: The PortTalk driver has been successfully started."));
  else {
    err = GetLastError();
    if (err == ERROR_SERVICE_ALREADY_RUNNING)
      ticables_info(_("PortTalk: The PortTalk driver is already running."));
    else {
      ticables_info(_("PortTalk: Unknown error while starting PortTalk driver service.\n"
	   "PortTalk: Does PortTalk.SYS exist in your \\System32\\Drivers Directory?"));
      return (0);
    }
  }

  /* Close handle to Service Control Manager */
  CloseServiceHandle(schService);
  return (TRUE);
}
#endif
