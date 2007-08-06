/* dhahelper setup program (c) 2004 Sascha Sommer */
/* compile with gcc -o dhasetup.exe dhasetup.c    */
/* LICENSE: GPL                                   */

#include <windows.h>
#include <stdio.h>

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

        printf("%s (%i -> %s)\n", s, GetLastError(), lpMsgBuf);
}

int main(int argc,char* argv[])
{
  SC_HANDLE hSCManager = NULL;
  SC_HANDLE hService = NULL;
  char path[MAX_PATH];
  int result;

  printf("dhasetup (c) 2004 Sascha Sommer\n");

  if(argc==1)
  {
    printf("Usage:\n");
    printf("dhasetup install -  Copies DhaHelper.sys from the current directory to\n%s and configures it to start at boot.\n", path);
    printf("dhasetup remove  -  Removes the DhaHelper utility.\n");
    return 0;
  }

  hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if(!strcmp(argv[1],"install"))
  {
    printf("Installing DhaHelper...\n");

	GetWindowsDirectory(path,MAX_PATH);
	strcpy(path+strlen(path),"\\system32\\drivers\\DhaHelper.sys");

    if(!CopyFile("DhaHelper.sys",path,FALSE))
	{
      printf("Copying DhaHelper.sys failed.\nEither DhaHelper.sys is not in the current directory or you lack sufficient\nprivileges to write to %s.", path);
      return 1;
    }

    // Install the driver
    hService = CreateService(hSCManager,
                             "DhaHelper",
                             "DhaHelper",
                             SERVICE_ALL_ACCESS,
                             SERVICE_KERNEL_DRIVER,
                             SERVICE_SYSTEM_START,
                             SERVICE_ERROR_NORMAL,
                             path,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL);
    if(!hService)
	{
	  print_last_error("Unable to register DhaHelper Service");
    }
	else
	  printf("Success!\n");
  }
  else if(!strcmp(argv[1],"remove"))
  {
    SERVICE_STATUS ServiceStatus;

    printf("Removing DhaHelper...\n");
    hService = OpenService(hSCManager, "DhaHelper", SERVICE_ALL_ACCESS);

    result = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
	if(!result) print_last_error("Error while stopping service");

    result = DeleteService(hService);
	if(!result) print_last_error("Error while deleting service");

    DeleteFile(path);
	printf("Done!\n");
  }
  else 
  {
    printf("unknown parameter: %s\n",argv[1]);
  }

  CloseServiceHandle(hService);
  CloseServiceHandle(hSCManager);

  printf("Please reboot to let the changes take effect.\n");

  return 0;
}
