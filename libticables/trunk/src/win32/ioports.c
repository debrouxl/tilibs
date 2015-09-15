/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * This unit manages direct low-level I/O operations depending on the 
 * platform type:
 * - Windows 9x/Me: assembly routines,
 * - Windows NT4/2000: assembly routines with a kernel driver for 
 *     granting I/O permissions.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <windows.h>

#include "../error.h"
#include "../logging.h"
#include "detect.h"
#include "ioports.h"

#include "../win32/dha.h"
#include "../win64/rwp.h"

/* Variables */

static int instance = 0;					// Instance counter

/* Function pointers */

int  (*io_rd) (unsigned int addr);
void (*io_wr) (unsigned int addr, int data);

/* Error helper */

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

        ticables_info("%s (%i -> %s)", s, GetLastError(), lpMsgBuf);
}
#endif				//__WIN32__

#ifndef _WIN64
/* I/O thru assembly code */

static int win32_read_io(unsigned int addr)
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

static void win32_write_io(unsigned int addr, int data)
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
#endif

/* I/O thru device driver IOCTL calls */

static int win64_read_io(unsigned int addr)
{
	unsigned char data;

	rwp_read_byte((unsigned short)addr, &data);
	return data;
}

static void win64_write_io(unsigned int addr, int data)
{
	rwp_write_byte((unsigned short)addr, (unsigned char)data);
}

/* Functions used for initializing the I/O routines */

int io_open(unsigned long from)
{
	int ret;

#ifndef _WIN64
	if(win32_check_os() == WIN_9X)
	{
		io_rd = win32_read_io;
		io_wr = win32_write_io;
	}
	else if(win32_check_os() == WIN_NT)
	{
		ret = dha_enable();
		if(ret) return ERR_DHA_NOT_FOUND;

		io_rd = win32_read_io;
		io_wr = win32_write_io;

		instance++;
	}
	else if(win32_check_os() == WIN_64)
#endif
	{
		ret = rwp_open();
		if(ret) return ERR_DHA_NOT_FOUND;

		io_rd = win64_read_io;
		io_wr = win64_write_io;

		instance++;
	}

  return 0;
}

int io_close(unsigned long from)
{
	int ret;

#ifndef _WIN64
	if(win32_check_os() == WIN_NT)
	{
		instance--;
		if(!instance)
		{
			ret = dha_disable();
			if(ret) return ERR_DHA_NOT_FOUND;
		}
	}
	else if(win32_check_os() == WIN_64)
#endif
	{
		instance--;
		if(!instance)
		{
			ret = rwp_close();
			if(ret) return ERR_DHA_NOT_FOUND;
		}
	}

	return 0;
}

/* Used by ser_link.c only (should be used by this module and tig_link.c) */

int win32_comport_open(char *comPort, PHANDLE hCom)
{
  DCB dcb;
  BOOL fSuccess;
  COMMTIMEOUTS cto;

  /* Open COM port */
  *hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0,
		     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hCom == INVALID_HANDLE_VALUE) 
  {
    ticables_info("CreateFile");
    print_last_error("CreateFile");
    return ERR_GRY_CREATEFILE;
  }
  // Setup buffer size
  fSuccess = SetupComm(*hCom, 1024, 1024);
  if (!fSuccess) 
  {
    ticables_info("SetupComm");
    print_last_error("SetupComm");
    return ERR_GRY_SETUPCOMM;
  }
  // Retrieve config structure
  fSuccess = GetCommState(*hCom, &dcb);
  if (!fSuccess) 
  {
    ticables_info("GetCommState");
    print_last_error("GetCOmmState");
    return ERR_GRY_GETCOMMSTATE;
  }
  // Fills the structure with config
  dcb.BaudRate = CBR_9600;	// 9600 bauds
  dcb.fBinary = TRUE;		// Binary mode
  dcb.fParity = FALSE;		// Parity checking disabled
  dcb.fOutxCtsFlow = FALSE;	// No output flow control
  dcb.fOutxDsrFlow = FALSE;	// Idem
  dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Provide power supply
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
  if (!fSuccess) 
  {
    ticables_info("SetCommState");
    print_last_error("SetCOmmState");
    return ERR_GRY_SETCOMMSTATE;
  }

  fSuccess = GetCommTimeouts(*hCom, &cto);
  if (!fSuccess) 
  {
    ticables_info("GetCommTimeouts");
    print_last_error("GetCommTimeouts");
    return ERR_GRY_GETCOMMTIMEOUT;
  }

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 100 * 10/*time_out*/;
  cto.WriteTotalTimeoutMultiplier = 0;
  cto.WriteTotalTimeoutConstant = 0;	// 0 make non-blocking

  fSuccess = SetCommTimeouts(*hCom, &cto);
  if (!fSuccess) 
  {
    ticables_info("SetCommTimeouts");
    print_last_error("SetCommTimeouts");
    return ERR_GRY_SETCOMMTIMEOUT;
  }

  return 0;
}

int win32_comport_close(PHANDLE hCom)
{
  if (*hCom) 
  {
    CloseHandle(*hCom);
    *hCom = INVALID_HANDLE_VALUE;
  }

  return 0;
}

