/*  IOPorts - I/O low-level port access routines for Linux, Windows9x/Me,
 *		NT4/2000, DOS.
 *  A part of the TiLP project
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

/*
 * This unit manages direct low-level I/O operations depending on the platform type.
 * - Linux: inb/outb (super user privileges requires)
 * - Windows 9x/Me: assembly routines
 * - Windows NT4/2000: assembly routines with a kernel driver for granting access
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
#include "intl.h"
#include "plerror.h"
#include "cabl_int.h"
#include "cabl_ext.h"

/* Variables */
extern int cable_type;  // Link cable type, I/O method depends on it
#ifdef __WIN32__
 static HINSTANCE hDLL = NULL;		// Handle for PortTalk Driver
 static HANDLE hCom=0;				// COM port handle for DCB
 static int iDcbUse = 0;			// Internal use
#endif

/* Function pointers */
int (*rd_io) (unsigned int addr);
int (*wr_io) (unsigned int addr, int data);


/*******************/
/* Linux functions */
/*******************/
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
int linux_read_io(unsigned int addr)
{
  //printf("0x%03x -> 0x%02x\n", addr, inb(addr));
  return inb(addr);
}

int linux_write_io(unsigned int addr, int data)
{
  //printf("0x%03x <- 0x%02x\n", addr, data);
  outb(data, addr);
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


/****************************************************/
/* Functions used for initializing the I/O routines */
/****************************************************/
int open_io(unsigned long from, unsigned long num)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__ALPHA__)
   
  rd_io = linux_read_io;
  wr_io = linux_write_io;

  return (ioperm(from, num, 1) ? ERR_ROOT : 0);

#elif defined(__WIN32__)
	int result = 0;
 	DWORD BytesReturned;	// Bytes Returned for DeviceIoControl()
	int offset;				// Offset for IOPM
	int Error; 		 		// Error Handling for DeviceIoControl()
	DWORD pid;				// PID of the program which use the library
  
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;
	char *comPort = "COM2";
  
	if(method & IOM_ASM)
    {
		rd_io = asm_read_io;
		wr_io = asm_write_io;
	}

    if(method & IOM_DRV)
	{
		if(hDLL != NULL)
			CloseHandle(hDLL);
		// Open PortTalk Driver
    	hDLL = CreateFile("\\\\.\\PortTalk", 
			GENERIC_READ, 
			0, 
			NULL,
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

    	if(hDLL == INVALID_HANDLE_VALUE) 
		{
	       	printf(_("Libticables: error, couldn't access PortTalk Driver, Please ensure driver is installed/loaded.\n"));
        	return ERR_DLPORTIO_NOT_FOUND;
    	}
		else
		{
			rd_io = asm_read_io;
			wr_io = asm_write_io;
		}

		// Turn off all access
		Error = DeviceIoControl(
                          	hDLL,
                          	0x10,   // RTL Fill Memory,
	                        NULL,
        	                0,		// Send Zero Bytes
                	        NULL,
                        	0,
                          	&BytesReturned,
                          	NULL
                         	);
  		if(!Error)
			print_last_error("Talking to device driver");

		// Turn on some access
		offset = from / 8;
		Error = DeviceIoControl(
			               		hDLL,
                          		0x18,   // IoControlCode,
  	                        	&offset,
        	                  	3,		// Send Zero Bytes
                	          	NULL,
                        	  	0,
                          		&BytesReturned,
                          		NULL
                         		);

  		if(!Error)
			print_last_error("Granting access");
			//printf(_("Error %d granting access to Address 0x%03X\n"), GetLastError(), from);
		else  
			printf(_("Address 0x%03X (IOPM Offset 0x%02X) has been granted access.\n"), from, offset);

		// Pass PID
		pid = _getpid();
		Error = DeviceIoControl(
                          	hDLL,
                          	0x04,
                          	&pid,
                          	4,
                          	NULL,
                          	0,
                          	&BytesReturned,
                          	NULL
                         	);

  		if(!Error)
			print_last_error("Talking to device driver");
			//printf(_("Error Occured talking to Device Driver %d\n"),GetLastError());
		else        
			printf(_("PortTalk Device Driver has set IOPM for ProcessID %d.\n"),pid);
	}

	if(method & IOM_DCB)
	{
		if(iDcbUse>0) 
		{
			iDcbUse++;
		}

		// Open COM port
		hCom = CreateFile(io_device, GENERIC_READ | GENERIC_WRITE, 0, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hCom == INVALID_HANDLE_VALUE)
		{
			  print_last_error();
			  return ERR_CREATE_FILE;
		}
      
		// Setup buffer size
		fSuccess = SetupComm(hCom, 1024, 1024);
		if(!fSuccess)
		{
			print_last_error();
			return ERR_SETUP_COMM;
		}
      
		// Retrieve config structure
		fSuccess = GetCommState(hCom, &dcb);
		if(!fSuccess)
		{
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
			print_last_error();
			return ERR_SET_COMMSTATE;
		}
      
		fSuccess=GetCommTimeouts(hCom,&cto);
		if(!fSuccess)
		{
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
			print_last_error();
			return ERR_SET_COMMTIMEOUT;
		}
      
		printf(_("Libticables: serial port %s successfully reconfigured.\n"), comPort);
		iDcbUse++;
      
		rd_io = dcb_read_io;
		wr_io = dcb_write_io;
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

  if(method & IOM_DRV)
  {
	if(hDLL != NULL)
	CloseHandle(hDLL);	
  }

  if(method & IOM_DCB)
  {
	if(iDcbUse>0)
	{
	  iDcbUse--;
	}
    
	if(hCom)
	{
	  CloseHandle(hCom);
	  hCom=0;
	  iDcbUse=0;
	}
  }

  return 0;
#endif
  return 0;
}

#ifdef __WIN32__
#define BUFFER_SIZE 1024

/* Used by tig_link, ser_link and this file */
/* Open a Win32 serial device */
int open_com_port(char *comPort, PHANDLE hCom)
{
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;
	char *name = comPort;

	/* Open COM port */
	*hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "CreateFile\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}

	// Setup buffer size
	fSuccess = SetupComm(*hCom, BUFFER_SIZE, BUFFER_SIZE);
	if(!fSuccess)
	{
		fprintf(stderr, "SetupComm\n");
		print_last_error();
		return ERR_SETUP_COMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(*hCom, &dcb);
	if(!fSuccess)
	{
		fprintf(stderr, "GetCommState\n");
		print_last_error();
		return ERR_GET_COMMSTATE;
	}

	// Fills the structure with config
	dcb.BaudRate = CBR_9600;				// 9600 bauds
	dcb.fBinary = TRUE;						// Binary mode
	dcb.fParity = FALSE;					// Parity checking disabled
	dcb.fOutxCtsFlow = FALSE;				// No output flow control
	dcb.fOutxDsrFlow = FALSE;				// Idem
	dcb.fDtrControl = DTR_CONTROL_DISABLE;	// Provide power supply
	dcb.fDsrSensitivity = FALSE;			// ignore DSR status
	dcb.fOutX = FALSE;						// no XON/XOFF flow control
	dcb.fInX = FALSE;						// idem
	dcb.fErrorChar = FALSE;					// no replacement
	dcb.fNull = FALSE;						// don't discard null chars
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Provide power supply
	dcb.fAbortOnError = FALSE;				// do not report errors

	dcb.ByteSize = 8;						// 8 bits
	dcb.Parity = NOPARITY;					// no parity checking
	dcb.StopBits = ONESTOPBIT;				// 1 stop bit

	// Config COM port
	fSuccess = SetCommState(*hCom, &dcb);
	if(!fSuccess)
	{
		fprintf(stderr, "SetCommState\n");
		print_last_error();
		return ERR_SET_COMMSTATE;
	}

    fSuccess=GetCommTimeouts(*hCom,&cto);
	if(!fSuccess)
	{
		fprintf(stderr, "GetCommTimeouts\n");
		print_last_error();
		return ERR_GET_COMMTIMEOUT;
	}
    
	cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * time_out;	
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;	// A value of 0 make non-blocking

    fSuccess=SetCommTimeouts(*hCom,&cto);
	if(!fSuccess)
	{
		fprintf(stderr, "SetCommTimeouts\n");
		print_last_error();
		return ERR_SET_COMMTIMEOUT;
	}

	return 0;
}

int close_com_port(PHANDLE hCom)
{
	if(*hCom)
	{
		CloseHandle(*hCom);
		*hCom=0;
	}

	return 0;
}
#endif
