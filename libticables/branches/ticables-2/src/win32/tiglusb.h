// 
// TiglUsb driver for Windows 9x/Me & 2000/XP
// Library (user mode) for talking with the driver (kernel mode)
//
// Copyright (c) 2001-2005 Romain Liévin
// roms@lpg.ticalc.org
// http://lpg.ticalc.org/prj_usb
//
// May the 1st, 2005
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
	02/12/02: fixed some problem of calling convention when lib is used with C++ 
	25/05/02: typo fixes in the function pointers
	04/07/02: modified for WinDDK (Windows XP)
	06/10/02: TiglUsbCheck added
	08/10/02: calling convention changed from __cdecl to __stdcall (WINAPI, VB, C++ Builder)
    24/04/04: API modified to use buffered write I/O (API compat broken)
	02/10/04: added new function to get device information (PID)
	01/05/05: added extended functions for sending/receiving whole packets
	01/05/05: can parse available cables with characteristics (product id)
--*/

#ifndef TIGLUSB_INC
#define TIGLUSB_INC

#define NONBLOCKING			// Overlapped I/O operations enabled (default)

#ifdef __cplusplus
extern "C" {
#endif

	// The following ifdef block is the standard way of creating macros which make exporting 
	// from a DLL simpler. All files within this DLL are compiled with the TIGLUSB_EXPORTS
	// symbol defined on the command line. this symbol should not be defined on any project
	// that uses this DLL. This way any other project whose source files include this file see 
	// TIGLUSB_API functions as being imported from a DLL, wheras this DLL sees symbols
	// defined with this macro as being exported.
#ifdef TIGLUSB_EXPORTS
# define TIGLUSB_EXP __declspec(dllexport)
#else
# define TIGLUSB_EXP __declspec(dllimport)
#endif
	// This DLL (should) use the stdcall calling convention.
#define  TIGLUSB_API __cdecl	//__stdcall

/* ----------------------- For Developper use --------------------- */

	//
	// API: Win32 functions (exports)
	//

// Versionning
TIGLUSB_EXP PCHAR TIGLUSB_API TiglUsbVersion (VOID);	// Get version

// 3.0: I/O operations on device (use it if you have just 1 link cable)
TIGLUSB_EXP INT TIGLUSB_API TiglUsbOpen  (VOID);		// Open a device instance
TIGLUSB_EXP INT TIGLUSB_API TiglUsbClose (VOID);	    // Close the device instance

TIGLUSB_EXP INT TIGLUSB_API TiglUsbCheck (INT *status);	// Check whether data is available

TIGLUSB_EXP INT TIGLUSB_API TiglUsbRead  (UCHAR *data);	// Read a byte from cable
TIGLUSB_EXP INT TIGLUSB_API TiglUsbWrite (UCHAR data);	// Write a byte to cable

TIGLUSB_EXP INT TIGLUSB_API TiglUsbFlush (VOID);	    // Flush write buffer
TIGLUSB_EXP INT TIGLUSB_API TiglUsbReset (VOID);	    // Reset r/w pipes

TIGLUSB_EXP INT TIGLUSB_API TiglUsbSetTimeout (INT ts);	// Set timeout value (in tenth of seconds)
TIGLUSB_EXP INT TIGLUSB_API TiglUsbGetTimeout (INT *ts);// Retrieve timeout value

// 3.2: use it for multiple cable support
TIGLUSB_EXP INT TIGLUSB_API TiglUsbProbe2 (PUINT* list);// Parse available cables (0-terminated array)

TIGLUSB_EXP INT TIGLUSB_API TiglUsbOpen2  (INT id);		// Open a device instance
TIGLUSB_EXP INT TIGLUSB_API TiglUsbClose2 (INT handle); // Close the device instance

TIGLUSB_EXP INT TIGLUSB_API TiglUsbCheck2 (INT handle, 
										   INT *status);// Check whether data is available

TIGLUSB_EXP INT TIGLUSB_API TiglUsbRead2  (INT handle,
										   UCHAR *data,
										   INT len);	// Read a byte from cable
TIGLUSB_EXP INT TIGLUSB_API TiglUsbWrite2 (INT handle,
										   UCHAR *data,
										   INT len);	// Write a byte to cable

TIGLUSB_EXP INT TIGLUSB_API TiglUsbFlush2 (INT handle); // Flush write buffer
TIGLUSB_EXP INT TIGLUSB_API TiglUsbReset2 (INT handle); // Reset r/w pipes

TIGLUSB_EXP INT TIGLUSB_API TiglUsbSetTimeout2 (INT handle,
												INT ts);// Set timeout value (in tenth of seconds)
TIGLUSB_EXP INT TIGLUSB_API TiglUsbGetTimeout2 (INT handle,
												INT *ts);// Retrieve timeout value

// 3.0: function pointers for dynamic loading
typedef PCHAR (TIGLUSB_API *TIGLUSB_VERSION)    (VOID);

typedef INT (TIGLUSB_API *TIGLUSB_OPEN)	        (VOID);
typedef INT (TIGLUSB_API *TIGLUSB_CLOSE)        (VOID);

typedef INT (TIGLUSB_API *TIGLUSB_CHECK)        (PINT);

typedef INT (TIGLUSB_API *TIGLUSB_READ)	        (PUCHAR);
typedef INT (TIGLUSB_API *TIGLUSB_WRITE)	    (UCHAR);

typedef INT (TIGLUSB_API *TIGLUSB_FLUSH)	    (VOID);
typedef INT (TIGLUSB_API *TIGLUSB_RESET)        (VOID);

typedef INT	(TIGLUSB_API *TIGLUSB_SETTIMEOUT)	(INT);
typedef INT	(TIGLUSB_API *TIGLUSB_GETTIMEOUT)	(PINT);

// 3.2:
typedef INT (TIGLUSB_API *TIGLUSB_PROBE2)		(PUINT*);

typedef INT (TIGLUSB_API *TIGLUSB_OPEN2)	    (INT);
typedef INT (TIGLUSB_API *TIGLUSB_CLOSE2)       (INT);

typedef INT (TIGLUSB_API *TIGLUSB_CHECK2)       (INT,PINT);

typedef INT (TIGLUSB_API *TIGLUSB_READ2)	    (INT,PUCHAR,INT);
typedef INT (TIGLUSB_API *TIGLUSB_WRITE2)	    (INT,PUCHAR,INT);

typedef INT (TIGLUSB_API *TIGLUSB_FLUSH2)	    (INT);
typedef INT (TIGLUSB_API *TIGLUSB_RESET2)       (INT);

typedef INT	(TIGLUSB_API *TIGLUSB_SETTIMEOUT2)	(INT,INT);
typedef INT	(TIGLUSB_API *TIGLUSB_GETTIMEOUT2)	(INT,PINT);

// Error codes (TIGLERR for 3.0)
#define TIGLERR_NO_ERROR			    0

#define TIGLERR_DEV_ALREADY_OPEN		1
#define TIGLERR_DEV_OPEN_FAILED			2
#define TIGLERR_FLUSH_FAILED			3   // not used
#define TIGLERR_WRITE_TIMEOUT			4
#define TIGLERR_READ_TIMEOUT			5
#define TIGLERR_WRITE_ERROR			    6
#define TIGLERR_READ_ERROR			    7
#define TIGLERR_RESET_FAILED            8
#define TIGLERR_MALLOC					9
#define TIGLERR_ERRMAX                  10

// Error codes (TIGLERR for 3.2)
#define TIGLERR2_NO_ERROR			    0

#define TIGLERR2_DEV_ALREADY_OPEN		-1
#define TIGLERR2_DEV_OPEN_FAILED		-2
#define TIGLERR2_FLUSH_FAILED			-3   // not used
#define TIGLERR2_WRITE_TIMEOUT			-4
#define TIGLERR2_READ_TIMEOUT			-5
#define TIGLERR2_WRITE_ERROR			-6
#define TIGLERR2_READ_ERROR			    -7
#define TIGLERR2_RESET_FAILED           -8
#define TIGLERR2_MALLOC					-9
#define TIGLERR2_HANDLE_MAX				-10

#define TIGLERR_HANDLE_MAX				-11


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

/* ----------------------- For Internal/DDK use only --------------------- */

	//
	// Constants
	//

#define	USB_MAX		4		// 4 cables max

#define IN_PIPE  "PIPE00" // IN  pipe (TI->PC)
#define OUT_PIPE "PIPE01" // OUT pipe (PC->TI)

#define VID_TI		0x0451	// Texas Instruments, Inc.
#define PID_TIGLUSB 0xE001	// TI-GRAPH LINK USB (SilverLink)
#define PID_TI89TM  0xE004	// TI89 Titanium w/ embedded USB port
#define PID_TI84P   0xE008	// TI84+ w/ embedded USB port

typedef struct 
{
	UINT		number;	// link cable number
	UINT		max_ps;	// packet size
	UINT		timeout;// timeout

	HANDLE	hRead;		// read pipe
	HANDLE  hWrite;		// write pipe

	PUCHAR	rBuf;		// rx buf
	DWORD	nBytesRead;

	PUCHAR	wBuf;		// tx buf
	DWORD	nBytesWrite;

	OVERLAPPED olp;		// for overlapped i/o

	int		open;
	int		busy;

	HANDLE	hDev;		// handle on device (driver)
	UINT	pid;		// Product ID

} TiglUsbHandle;

	//
	// API: Win32 functions
	//

// Returns an handle on the device driver or INVALID_HANDLE_VALUE otherwise
HANDLE open_dev (UINT number);

// Returns an handle on a pipe ("PIPE00": IN or "PIPE01": OUT)
// or INVALID_HANDLE_VALUE otherwise.
HANDLE open_file (UINT number, char *filename);
 
// Do a formatted ascii dump to console of USB 
// configuration, interface, and endpoint descriptors
int  dumpUsbConfig (HANDLE hDEV );

// IOCTL operations: returns 0 if sucessful, -1 otherwise.
int resetPipe (HANDLE hDEV );		// reset current pipe (??)
int resetDevice (HANDLE hDEV );		// reset device (dangerous)
int resetPipes (HANDLE hDEV );		// reset IN & OUT pipes

// Retrieve maximum packet size
int get_max_ps (HANDLE, PUINT, PUINT);

// Parse available cables
UINT list_dev(PUINT* list);

#ifdef __cplusplus
}
#endif

#endif // end, #ifndef TIGLUSBH_INC
