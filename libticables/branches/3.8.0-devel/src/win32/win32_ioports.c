/* Hey EMACS -*- win32-c -*- */
/* $Id: ioports.c 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
 *  Copyright (c) 2002, Kevin Kofler for the __MINGW32__ & __GNUC__ extensions.
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
 * This unit manages direct low-level I/O operations depending on the 
 * platform type:
 * - Linux: inb/outb (super user privileges required) or ioctl calls,
 * - Windows 9x/Me: assembly routines,
 * - Windows NT4/2000: assembly routines with a kernel driver for 
 *     granting I/O permissions.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <windows.h>
#include <process.h>		// getpid
#include <winioctl.h>		// PortTalk
#include "porttalk_IOCTL.h"	// PortTalk

#include "../cabl_err.h"
#include "../cabl_def.h"
#include "../printl1.h"
#include "../export.h"
#include "../intl1.h"
#include "../cabl_int.h"
#include "../externs.h"
#include "../ioports.h"

/* Variables */

static HINSTANCE hDll = NULL;	// Handle for PortTalk Driver
static HANDLE hCom = 0;		// COM port handle for Win32 DCB (API)
static int iDcbUse = 0;		// Internal use

/* Function pointers */

int (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

/* Error helper */

#ifdef __WIN32__
static void print_last_error(char *s)
{
        LPVOID lpMsgBuf;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) & lpMsgBuf, 0, NULL);
        printl(0, "%s (%i -> %s)\n", s, GetLastError(), lpMsgBuf);
}
#endif				//__WIN32__

/* I/O thru assembly code */

static int win32_asm_read_io(unsigned int addr)
{
        int c;

#ifdef __GNUC__
asm("movl $0,%%eax \n movw %1,%%dx \n inb %%dx,%%al \n movl %%eax,%0": "=g"(c): "g"(addr):"eax",
      "dx");
#else
        __asm {
	 mov eax, 0 
	 mov edx, addr 
	 in al, dx 
	 mov c, eax}
#endif
        return c;
}

static void win32_asm_write_io(unsigned int addr, int data)
{
#ifdef __GNUC__
asm("movw %0,%%dx \n movw %1,%%ax \n outb %%al,%%dx"::"g"(addr), "g"(data):"ax",
      "dx");
#else
        __asm {
	  mov edx, addr 
	  mov eax, data 
	  out dx, al}
#endif
}

/* I/O thru ioctl() calls */

static int win32_dcb_read_io(unsigned int addr)
{
        DWORD s;

        GetCommModemStatus(hCom, &s);
        return (s & MS_CTS_ON ? 1 : 0) | (s & MS_DSR_ON ? 2 : 0);
}

static void win32_dcb_write_io(unsigned int address, int data)
{
        EscapeCommFunction(hCom, (data & 2) ? SETRTS : CLRRTS);
        EscapeCommFunction(hCom, (data & 1) ? SETDTR : CLRDTR);
}

/* Functions used for initializing the I/O routines */

int io_open(unsigned long from, unsigned long num)
{
  DWORD BytesReturned;		// Bytes Returned for DeviceIoControl()
  int offset;			// Offset for IOPM
  int iError;			// Error Handling for DeviceIoControl()
  DWORD pid;			// PID of the program which use the library

  DCB dcb;			// DCB fallback
  BOOL fSuccess;
  COMMTIMEOUTS cto;
  char *comPort = "COM2";

  if (method & IOM_ASM) {
    io_rd = win32_asm_read_io;
    io_wr = win32_asm_write_io;
  }
  
#ifndef __MINGW32__
  else if (method & IOM_DRV) {
    // At this point, the driver should have been installed 
    // and started in probe.c
    hDll = CreateFile("\\\\.\\PortTalk",
		      GENERIC_READ,
		      0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDll == INVALID_HANDLE_VALUE) {
      printl(2, _
		    ("couldn't access PortTalk Driver, Please ensure driver is installed/loaded.\n"));
      return ERR_PORTTALK_NOT_FOUND;
    } else {
      io_rd = win32_asm_read_io;
      io_wr = win32_asm_write_io;
    }

    // Turn off all access
    iError = DeviceIoControl(hDll,
			     IOCTL_IOPM_RESTRICT_ALL_ACCESS,
			     NULL, 0, NULL, 0, &BytesReturned, NULL);

    if (!iError)
      print_last_error
	  ("PortTalk: error %d occured in IOCTL_IOPM_RESTRICT_ALL_ACCESS\n");

    // Turn on some access
    offset = from / 8;
    iError = DeviceIoControl(hDll,
			     IOCTL_SET_IOPM,
			     &offset, 3, NULL, 0, &BytesReturned, NULL);
    if (!iError)
      print_last_error("Granting access");
    else
      printl(0, _
	      ("Address 0x%03X (IOPM Offset 0x%02X) has been granted access.\n"),
	      from, offset);

    // Pass PID
    pid = getpid();

    iError = DeviceIoControl(hDll,
			     IOCTL_ENABLE_IOPM_ON_PROCESSID,
			     &pid, 4, NULL, 0, &BytesReturned, NULL);

    if (!iError)
      print_last_error("Talking to device driver");
    //printl(0, _("Error Occured talking to Device Driver %d\n"),GetLastError());
    else
      printl(0, _
	      ("PortTalk Device Driver has set IOPM for ProcessID %d.\n"),
	      pid);

    //CloseHandle(hDll);
  }
#endif

  else if (method & IOM_API) {
    if (iDcbUse > 0)
      return 0;
    iDcbUse++;

    // Open COM port
    hCom =
	CreateFile(io_device, GENERIC_READ | GENERIC_WRITE, 0,
		   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hCom == INVALID_HANDLE_VALUE) {
      print_last_error("CreateFile");
      return ERR_OPEN_SER_COMM;
    }
    // Setup buffer size
    fSuccess = SetupComm(hCom, 1024, 1024);
    if (!fSuccess) {
      print_last_error("SetupComm");
      return ERR_SETUP_COMM;
    }
    // Retrieve config structure
    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) {
      print_last_error("GetCommState");
      return ERR_GET_COMMSTATE;
    }
    // Fills the structure with config
    dcb.BaudRate = CBR_9600;	// 9600 bauds
    dcb.fBinary = TRUE;		// Binary mode
    dcb.fParity = FALSE;	// Parity checking disabled
    dcb.fOutxCtsFlow = FALSE;	// No output flow control
    dcb.fOutxDsrFlow = FALSE;	// Idem
    dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Provide power supply
    dcb.fDsrSensitivity = FALSE;	// ignore DSR status
    dcb.fOutX = FALSE;		// no XON/XOFF flow control
    dcb.fInX = FALSE;		// idem
    dcb.fErrorChar = FALSE;	// no replacement
    dcb.fNull = FALSE;		// don't discard null chars
    dcb.fRtsControl = RTS_CONTROL_ENABLE;	// FlashZ
    dcb.fAbortOnError = FALSE;	// do not report errors

    dcb.ByteSize = 8;		// 8 bits
    dcb.Parity = NOPARITY;	// no parity checking
    dcb.StopBits = ONESTOPBIT;	// one stop bit

    // Config COM port
    fSuccess = SetCommState(hCom, &dcb);
    if (!fSuccess) {
      print_last_error("SetCommState");
      return ERR_SET_COMMSTATE;
    }

    fSuccess = GetCommTimeouts(hCom, &cto);
    if (!fSuccess) {
      print_last_error("GetCommTimeouts");
      return ERR_GET_COMMTIMEOUT;
    }

    cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * 10;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;	// 0 makes non-blocking

    fSuccess = SetCommTimeouts(hCom, &cto);
    if (!fSuccess) {
      print_last_error("SetCOmmTimeouts");
      return ERR_SET_COMMTIMEOUT;
    }

    printl(0, _
	    ("Libticables: serial port %s successfully reconfigured.\n"),
	    comPort);
    iDcbUse++;

    io_rd = win32_dcb_read_io;
    io_wr = win32_dcb_write_io;
  } else {
		printl(2, "libticables: bad argument (invalid method).\n");
                return ERR_ILLEGAL_ARG;
  }

  return 0;
}

int io_close(unsigned long from, unsigned long num)
{
  if (method & IOM_DRV) {
    if (hDll != NULL)
      CloseHandle(hDll);
		hDll = NULL;
  }

  else if (method & IOM_API) {
    if (iDcbUse == 0)
      return 0;

    if (iDcbUse > 0)
      iDcbUse--;

    if (hCom) {
      CloseHandle(hCom);
      hCom = 0;
      iDcbUse = 0;
    }
  }  else {
		printl(2, "libticables: bad argument (invalid method).\n");
                return ERR_ILLEGAL_ARG;
	}

  return 0;
}

/* Used by ser_link.c only (should be used by this module and tig_link.c) */

#define BUFFER_SIZE 1024


int win32_comport_open(char *comPort, PHANDLE hCom)
{
  DCB dcb;
  BOOL fSuccess;
  COMMTIMEOUTS cto;

  /* Open COM port */
  *hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0,
		     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hCom == INVALID_HANDLE_VALUE) {
    printl(2, "CreateFile\n");
    print_last_error("CreateFile");
    return ERR_CREATE_FILE;
  }
  // Setup buffer size
  fSuccess = SetupComm(*hCom, BUFFER_SIZE, BUFFER_SIZE);
  if (!fSuccess) {
    printl(2, "SetupComm\n");
    print_last_error("SetupComm");
    return ERR_SETUP_COMM;
  }
  // Retrieve config structure
  fSuccess = GetCommState(*hCom, &dcb);
  if (!fSuccess) {
    printl(2, "GetCommState\n");
    print_last_error("GetCOmmState");
    return ERR_GET_COMMSTATE;
  }
  // Fills the structure with config
  dcb.BaudRate = CBR_9600;	// 9600 bauds
  dcb.fBinary = TRUE;		// Binary mode
  dcb.fParity = FALSE;		// Parity checking disabled
  dcb.fOutxCtsFlow = FALSE;	// No output flow control
  dcb.fOutxDsrFlow = FALSE;	// Idem
  dcb.fDtrControl = DTR_CONTROL_DISABLE;	// Provide power supply
  dcb.fDsrSensitivity = FALSE;	// ignore DSR status
  dcb.fOutX = FALSE;		// no XON/XOFF flow control
  dcb.fInX = FALSE;		// idem
  dcb.fErrorChar = FALSE;	// no replacement
  dcb.fNull = FALSE;		// don't discard null chars
  dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Provide power supply
  dcb.fAbortOnError = FALSE;	// do not report errors

  dcb.ByteSize = 8;		// 8 bits
  dcb.Parity = NOPARITY;	// no parity checking
  dcb.StopBits = ONESTOPBIT;	// 1 stop bit

  // Config COM port
  fSuccess = SetCommState(*hCom, &dcb);
  if (!fSuccess) {
    printl(2, "SetCommState\n");
    print_last_error("SetCOmmState");
    return ERR_SET_COMMSTATE;
  }

  fSuccess = GetCommTimeouts(*hCom, &cto);
  if (!fSuccess) {
    printl(2, "GetCommTimeouts\n");
    print_last_error("GetCommTimeouts");
    return ERR_GET_COMMTIMEOUT;
  }

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 100 * time_out;
  cto.WriteTotalTimeoutMultiplier = 0;
  cto.WriteTotalTimeoutConstant = 0;	// 0 make non-blocking

  fSuccess = SetCommTimeouts(*hCom, &cto);
  if (!fSuccess) {
    printl(2, "SetCommTimeouts\n");
    print_last_error("SetCommTimeouts");
    return ERR_SET_COMMTIMEOUT;
  }

  return 0;
}

int win32_comport_close(PHANDLE hCom)
{
  if (*hCom) {
    CloseHandle(*hCom);
    *hCom = 0;
  }

  return 0;
}

