/*  IOPorts - I/O low-level port access routines for Linux, Windows9x/Me,
 *		NT4/2000, DOS.
 *	A part of the TiLP project
 *  Copyright (C) 1999, 2000  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributelabeld in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H)
#  include <sys/perm.h>
#  include <asm/io.h>
#elif defined(__WIN32__)
#  include <conio.h>
#  include <stdio.h>
#  include <windows.h>
#elif defined(__ALPHA__)
#  include <sys/io.h>
#endif

#ifdef __LINUX__
# include <unistd.h>
# include <sys/types.h>
#endif

#include "cabl_err.h"
#include "cabl_def.h"
#include "verbose.h"
#include "export.h"
#include "ioports.h"
#include "iocompat.h"
#include "intl.h"
#include "plerror.h"

/* Functions to use */
extern int method;   // I/O mode (asm, dcb, kernel driver)
extern int cable_type;
int iDcbUse = 0;

/* Function pointers */
int (*rd_io) (unsigned int addr);
int (*wr_io) (unsigned int addr, int data);

#ifdef __WIN32__		// Win32: DLPortIO, HwPort95 or Direct95

static HINSTANCE hDLL = NULL;		// DLL handle
static HANDLE hCom=0;			// COM port handle for DCB


/* DlPortIO functions pointers */
typedef UCHAR (CALLBACK* DLPORTREADPORTUCHAR)  (ULONG);
typedef VOID  (CALLBACK* DLPORTWRITEPORTUCHAR) (ULONG, UCHAR);

DLPORTREADPORTUCHAR DlPortReadPortUchar;
DLPORTWRITEPORTUCHAR DlPortWritePortUchar;

#endif


/*******************/
/* Linux functions */
/*******************/
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
int linux_read_io(unsigned int addr)
{
  return inb(addr);
}

int linux_write_io(unsigned int addr, int data)
{
  outb(data, addr);
  return 0;
}

#endif

#if defined(__WIN32__)
/****************************/
/* Win32 DLPortIO functions */
/****************************/
int dlportio_read_io(unsigned int addr)
{ 
  return DlPortReadPortUchar(addr);
}

int dlportio_write_io(unsigned int addr, int data)
{ 
  DlPortWritePortUchar((unsigned long)addr, (unsigned char)data);
  return 0;
}
#endif

#if defined(__WIN32__)
/*************************/
/* Win32 CONIO functions */
/*************************/
int conio_read_io(unsigned int addr)
{ 
  return _inp((unsigned short)addr);
}

int conio_write_io(unsigned int addr, int data)
{ 
  return _outp((unsigned short)addr, (int)data);
}
#endif

#if defined(__WIN32__)
/***********************/
/* Win32 ASM functions */
/***********************/
int asm_read_io(unsigned int addr)
{ 
  return inp_((unsigned short)addr);
}

int asm_write_io(unsigned int addr, int data)
{ 
  outp_((unsigned short)addr, (short)data);
  return 0;
}
#endif

#if defined(__WIN32__)
/***********************/
/* Win32 DCB functions */
/***********************/
int dcb_read_io(unsigned int addr)
{ 
  DWORD s;
  
  GetCommModemStatus(hCom, &s);
  return (s&MS_CTS_ON?1:0) | (s&MS_DSR_ON?2:0);
}

int dcb_write_io(unsigned int addr, int data)
{ 
    EscapeCommFunction(hCom, (data&2)?SETRTS:CLRRTS);
    EscapeCommFunction(hCom, (data&1)?SETDTR:CLRDTR);
    return 0;
}
#endif

int ticable_detect_os(char **os_type);

/****************************************************/
/* Functions used for initializing the I/O routines */
/****************************************************/
int open_io(unsigned long from, unsigned long num)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
  uid_t uid;
  
  rd_io = linux_read_io;
  wr_io = linux_write_io;
  
  uid = getuid();
  if(uid != 0)
    {
      DISPLAY("Using libticables as normal user: direct I/O will not work !\n");
      DISPLAY("Try to use a kernel module (tipar/tiser) instead.\n");
      method = IOM_DRV;
    }	/*
  else
    DISPLAY("  Using libticables as super user. This is not advised !\n");
	*/
  return (ioperm(from, num, 1) ? ERR_ROOT : 0);
#elif defined(__WIN32__)
  int result = 0;
  char *os;
  
  DCB dcb;
  BOOL fSuccess;
  COMMTIMEOUTS cto;
  char *comPort = "COM2";
  
  ticable_detect_os(&os);		// Get OS type
  
  /* DCB is legal only with serial cable */
  if((cable_type != LINK_SER) && method==IOM_DCB)
    method = IOM_AUTO;

 reparse:
  switch(method)
    {
    case IOM_AUTO:
      method = IOM_DRV;
      goto reparse;
      break;
    case IOM_ASM:
      rd_io = asm_read_io;
      wr_io = asm_write_io;
	  DISPLAY("Use internal routines...\n");
      break;
    case IOM_DRV:
      if(hDLL != NULL)
	FreeLibrary(hDLL);
      hDLL = LoadLibrary("DLPORTIO.DLL");
      if (hDLL == NULL)
	{
	  DISPLAY("DLPortIO driver not found. Try to use internal routines...\n");
	  DISPLAY("Try to use ASM routines instead.\n");
	  method = IOM_ASM;
	  // NT4/2000 detected and parallel port access -> error
	  if(!strcmp(os, "WindowsNT")) 
	    {
	      if(cable_type == LINK_SER)
		{
		  DISPLAY("Use DirectControlBlock routines (Win32 API)\n");
		  method = IOM_DCB;
		  goto reparse;
		}
	      else
		{
			  /*
		  HANDLE console;
		  DWORD mode;
		  DWORD written;
		  */
		  char *lpBuffer  = _("Currently running on WinNT.\nThe DLPortIO kernel driver is required for home-made parallel link cables, advised for serial/BlackLink cables (for better performances).\nYou can get this driver on the TiLP web-page <http://lpg.ticalc.org/prj_tilp/download.html>.\n");
		  char *lpBuffer2 = _("Press any key for exiting.\n");
		  
		  MessageBox(NULL, lpBuffer,"Error",MB_OK);
		  exit(1);
				/*
				  DISPLAY(lpBuffer);
				  FreeConsole();	// Free a possible console (_CONSOLE mode)
				  AllocConsole();	// Create a console
				  console = GetStdHandle(STD_OUTPUT_HANDLE);
				  if(GetConsoleMode(console, &mode) != 0)
				  {
				  WriteConsole(console, lpBuffer, strlen(lpBuffer), 
				  &written, NULL);
				  WriteConsole(console, lpBuffer2, strlen(lpBuffer2), 
				  &written, NULL);
				  
				  _getch();
				  exit(-1);	//return ERR_DLPORTIO_NOT_FOUND;
				  }
				*/
		}
	    }
	  else goto reparse;
	}
      else
	{
	  DISPLAY("DLPortIO driver found. Use DLPportIO routines under NT4/2000.\n");
	  DlPortReadPortUchar = (DLPORTREADPORTUCHAR)GetProcAddress(hDLL,
								    "DlPortReadPortUchar");
	  if (!DlPortReadPortUchar)
	    {
				// handle the error
	      DISPLAY("Unable to load DlPortReadPortUchar symbol.\n");
	      FreeLibrary(hDLL);       
	      return ERR_FREELIBRARY;
	    }
	  DlPortWritePortUchar = (DLPORTWRITEPORTUCHAR)GetProcAddress(hDLL,
								      "DlPortWritePortUchar");
	  if (!DlPortWritePortUchar)
	    {
				// handle the error
	      DISPLAY("Unable to load DlPortWritePortUchar symbol.\n");
	      FreeLibrary(hDLL);       
	      return ERR_FREELIBRARY;
	    }
	  DISPLAY("DLPortIO functions initialized.\n");
	  rd_io = dlportio_read_io;
	  wr_io = dlportio_write_io;
	}
      break;
    case IOM_DCB:
      if(iDcbUse>0) 
	{
	  iDcbUse++;
	  break;
	}
      
      switch(from) // I will have to change this trough DetectPort for a better correspondance
	{
	case 0x3f8: comPort = "COM1"; break;
	case 0x2f8: comPort = "COM2"; break;
	case 0x3e8: comPort = "COM3"; break;
	case 0x2e8: comPort = "COM4"; break;
	default: comPort = "COM2"; break;
	}
      
      // Open COM port
      hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if(hCom == INVALID_HANDLE_VALUE)
	{
	  DISPLAY("CreateFile\n");
	  print_last_error();
	  return ERR_CREATE_FILE;
	}
      
      // Setup buffer size
      fSuccess = SetupComm(hCom, 1024, 1024);
      if(!fSuccess)
	{
	  DISPLAY("SetupComm\n");
	  print_last_error();
	  return ERR_SETUP_COMM;
	}
      
      // Retrieve config structure
      fSuccess = GetCommState(hCom, &dcb);
      if(!fSuccess)
	{
	  DISPLAY("GetCommState\n");
	  print_last_error();
	  return ERR_GET_COMMSTATE;
	}
      
      // Fills the structure with config
      dcb.BaudRate = CBR_9600;				// 9600 bauds
      dcb.fBinary = TRUE;						// Binary mode
      dcb.fParity = FALSE;					// Parity checking disabled
      dcb.fOutxCtsFlow = FALSE;				// No output flow control
      dcb.fOutxDsrFlow = FALSE;				// Idem
      dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Provide power supply
      dcb.fDsrSensitivity = FALSE;			// ignore DSR status
      dcb.fOutX = FALSE;						// no XON/XOFF flow control
      dcb.fInX = FALSE;						// idem
      dcb.fErrorChar = FALSE;					// no replacement
      dcb.fNull = FALSE;						// don't discard null chars
      dcb.fRtsControl = RTS_CONTROL_ENABLE;	// FlashZ
      dcb.fAbortOnError = FALSE;				// do not report errors
      
      dcb.ByteSize = 8 ;						// 8 bits
      dcb.Parity = NOPARITY;					// no parity checking
      dcb.StopBits = ONESTOPBIT;				// one stop bit
      
      // Config COM port
      fSuccess = SetCommState(hCom, &dcb);
      if(!fSuccess)
	{
	  DISPLAY("SetCommState\n");
	  print_last_error();
	  return ERR_SET_COMMSTATE;
	}
      
      fSuccess=GetCommTimeouts(hCom,&cto);
      if(!fSuccess)
	{
	  DISPLAY("GetCommTimeouts\n");
	  print_last_error();
	  return ERR_GET_COMMTIMEOUT;
	}
      
      cto.ReadIntervalTimeout = MAXDWORD;
      cto.ReadTotalTimeoutMultiplier = 0;
      cto.ReadTotalTimeoutConstant = 100 * 10;	
      cto.WriteTotalTimeoutMultiplier = 0;
      cto.WriteTotalTimeoutConstant = 0;	// A value of 0 make non-blocking
      
      fSuccess=SetCommTimeouts(hCom,&cto);
      if(!fSuccess)
	{
	  DISPLAY("SetCommTimeouts\n");
	  print_last_error();
	  return ERR_SET_COMMTIMEOUT;
	}
      
      DISPLAY("Serial port %s successfully reconfigured.\n", comPort);
      iDcbUse++;
      
      rd_io = dcb_read_io;
      wr_io = dcb_write_io;
      break;
    default:
      break;
    }
  return 0;
#endif
  return 0;
}

int close_io(unsigned long from, unsigned long num)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
  return (ioperm(from, num, 0) ? ERR_ROOT : 0);
#elif defined(__WIN32__)
  switch(method)
    {
    case IOM_ASM:
      break;
    case IOM_DRV:
      if(hDLL != NULL)
	FreeLibrary(hDLL);	
      break;
    case IOM_DCB:
      if(iDcbUse>0)
	{
	  iDcbUse--;
	  break;
	}
      if(hCom)
	{
	  CloseHandle(hCom);
	  hCom=0;
	  iDcbUse=0;
	}
      break;
    default:
      break;
    }
  return 0;
#endif
  return 0;
}
