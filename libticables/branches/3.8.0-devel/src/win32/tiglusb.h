// 
// TiglUsb driver for Windows 9x/Me & 2000/XP
// Library (user mode) for talking with the driver (kernel mode)
//
// Copyright (c) 2001-2002 Romain Liévin
// roms@lpg.ticalc.org
// http://lpg.ticalc.org/prj_usb
//
// July the 4th, 2002
//

/*++

Module Name:

    TiglUsb.h

Abstract:

    Win32 Prototypes for TiglUsb library.
	This file is the same for both drivers (98DDK & XPDDK)

Environment:

    User mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 2001 Romain Liévin.  All Rights Reserved.
Revision History:

	11/17/97: created
	2001: modified
	2002: some adds
	02/12/2002: fixed some problem of calling convention when lib is used with C++ 
	25/05/2002: typo fixes in the function pointers
	04/07/2002: modified for WinDDK (Windows XP)
	06/10/2002: TiglUsbIsDataAvailable added
	08/10/2002: calling convention changed from __cdecl to __stdcall (WINAPI, VB, C++ Builder)
--*/

#ifndef TIGLUSB_INC
#define TIGLUSB_INC

#define NONBLOCKING		// Overlapped I/O operations enabled (default)

#ifdef __cplusplus
extern "C" {

#endif				/*  */

  // The following ifdef block is the standard way of creating macros which make exporting 
  // from a DLL simpler. All files within this DLL are compiled with the TIGLUSB_EXPORTS
  // symbol defined on the command line. this symbol should not be defined on any project
  // that uses this DLL. This way any other project whose source files include this file see 
  // TIGLUSB_API functions as being imported from a DLL, wheras this DLL sees symbols
  // defined with this macro as being exported.
#ifdef TIGLUSB_EXPORTS
# define TIGLUSB_EXP __declspec(dllexport)
#else				/*  */
# define TIGLUSB_EXP __declspec(dllimport)
#endif				/*  */
  // This DLL (should) use the stdcall calling convention.
#define  TIGLUSB_API __cdecl	//__stdcall

/* ----------------------- For Internal/DDK use only --------------------- */

  //
  // Constants
  //

#define TIGLUSB_MAX_PACKET_SIZE	32	// 32 bytes max can be read or written at a time

#define IN_PIPE_0  "PIPE00"	// First link cable, IN pipe (TI->PC)
#define OUT_PIPE_0 "PIPE01"	//                                      OUT pipe (PC->TI)
#define IN_PIPE_1  "PIPE02"	// Second link cable
#define OUT_PIPE_1 "PIPE03"
#define IN_PIPE_2  "PIPE04"	// Third link cable
#define OUT_PIPE_2 "PIPE05"
#define IN_PIPE_3  "PIPE06"	// Fourth link cable
#define OUT_PIPE_3 "PIPE07"

  //
  // API: Win32 functions
  //

// Returns an handle on the device driver or INVALID_HANDLE_VALUE otherwise
  HANDLE open_dev(void);

// Returns an handle on a pipe ("PIPE00": IN or "PIPE01": OUT)
// or INVALID_HANDLE_VALUE otherwise.
  HANDLE open_file(char *filename);

// Rarely used
  void rw_dev(HANDLE hDEV);
  char *usbDescriptorTypeString(UCHAR bDescriptorType);
  char *usbEndPointTypeString(UCHAR bmAttributes);
  char *usbConfigAttributesString(UCHAR bmAttributes);

// Do a formatted ascii dump to console of USB 
// configuration, interface, and endpoint descriptors
  int dumpUsbConfig(void);

// IOCTL operations: returns 0 if sucessful, -1 otherwise.
  int resetPipe(void);		// reset current pipe (??)
  int resetDevice(void);	// reset device (dangerous)
  int resetPipes(void);		// reset IN & OUT pipes

/* ----------------------- For Developper use --------------------- */

  //
  // API: Win32 functions (exports)
  //

// Versionning
  TIGLUSB_EXP PCHAR TIGLUSB_API TiglUsbVersion(VOID);	// Get version

// I/O operations on device
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbOpen(VOID);	// Open a device instance
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbFlush(VOID);	// Flush buffer
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbRead(UCHAR * data);	// Read a byte from cable
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbWrite(UCHAR data);	// Write a byte to cable
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbClose(VOID);	// Close the device instance
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbCheck(INT * status);	// Check whether data is available
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbSetTimeout(INT ts);	// Set timeout value (in tenth of seconds)
  TIGLUSB_EXP INT TIGLUSB_API TiglUsbGetTimeout(INT * ts);	// Retrieve timeout value

// Function pointers for dynamic loading
  typedef PCHAR(TIGLUSB_API * TIGLUSB_VERSION) (VOID);
  typedef INT(TIGLUSB_API * TIGLUSB_OPEN) (VOID);
  typedef INT(TIGLUSB_API * TIGLUSB_FLUSH) (VOID);
  typedef INT(TIGLUSB_API * TIGLUSB_READ) (PUCHAR);
  typedef INT(TIGLUSB_API * TIGLUSB_WRITE) (UCHAR);
  typedef INT(TIGLUSB_API * TIGLUSB_CLOSE) (VOID);
  typedef INT(TIGLUSB_API * TIGLUSB_CHECK) (PINT);
  typedef INT(TIGLUSB_API * TIGLUSB_SETTIMEOUT) (INT);
  typedef INT(TIGLUSB_API * TIGLUSB_GETTIMEOUT) (PINT);

// Error codes
#define TIGLERR_NO_ERROR				0
#define TIGLERR_DEV_ALREADY_OPEN		1
#define TIGLERR_DEV_OPEN_FAILED			2
#define TIGLERR_FLUSH_FAILED			3
#define TIGLERR_WRITE_TIMEOUT			4
#define TIGLERR_READ_TIMEOUT			5
#define TIGLERR_WRITE_ERROR				6
#define TIGLERR_READ_ERROR				7

/* If you need Dynamic Liking, defines your pointers like this:
	TIGLUSB_OPENFILE	dynTiglUsbOpenFile		= NULL;
	TIGLUSB_OPENDEV		dynTiglUsbOpenDev		= NULL;
	TIGLUSB_RESETPIPES	dynTiglUsbResetPipes	= NULL;
	TIGLUSB_SETTIMEOUT	dynTiglUsbSetTimeout	= NULL;

  Sample code:

	hDLL = LoadLibrary("TIGLUSB.DLL");
	if (hDLL == NULL)
	{
	  dERROR("TiglUsb library not found. Have you installed the driver ?\n");
	  return ERR_USB_OPEN;
	}

	dynTiglUsbOpenFile = (TIGLUSB_OPENFILE)GetProcAddress(hDLL,
								    "open_file");
	if (!dynTiglUsbOpenFile)
	{
		dERROR("Unable to load TiglUsbOpenFile symbol.\n");
	    FreeLibrary(hDLL);       
	    return ERR_FREELIBRARY;
	}

  FreeLibrary(hDLL);
*/

#ifdef __cplusplus
}
#endif				/*  */
#endif				// end, #ifndef TIGLUSBH_INC
