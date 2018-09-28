/* Hey EMACS -*- win32-c -*- */
/* $Id: link_gry.c 1622 2005-09-10 06:33:36Z roms $ */

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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

//!!!!!!!!!!!!!!!!!! Not used, for use of overlapped i/o !!!!!!!!!!!!!!!!!!!!

/* "Grey TIGraphLink" link cable unit */

#include <stdio.h>
#include <windows.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "../internal.h"
#include "detect.h"

#define hCom	(HANDLE)(h->priv)

static int gry_prepare(CableHandle *h)
{
	const char * device;
	switch(h->port)
	{
	case PORT_1: h->address = 0x3f8; device = "COM1"; break;
	case PORT_2: h->address = 0x2f8; device = "COM2"; break;
	case PORT_3: h->address = 0x3e8; device = "COM3"; break;
	case PORT_4: h->address = 0x3e8; device = "COM4"; break;
	default: return ERR_ILLEGAL_ARG;
	}

	if (h->device == NULL)
	{
		h->device = strdup(device);
	}

	return 0;
}

static int gry_open(CableHandle *h)
{
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;

	// Open device
	h->priv = (void *)CreateFile(h->device, GENERIC_READ | GENERIC_WRITE, 0,
		    NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED /*| FILE_FLAG_NO_BUFFERING*/,
			NULL);
	if (hCom == INVALID_HANDLE_VALUE) 
	{
		ticables_warning("CreateFile");
		return ERR_GRY_CREATEFILE;
	}
  
	// Setup buffer size
	fSuccess = SetupComm(hCom, 1024, 1024);
	if (!fSuccess) 
	{
		ticables_warning("SetupComm");
		return ERR_GRY_SETUPCOMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(hCom, &dcb);
	if (!fSuccess) 
	{
		ticables_warning("GetCommState");
		return ERR_GRY_GETCOMMSTATE;
	}

	// Fills the structure with config
	dcb.BaudRate = CBR_9600;	// 9600 bauds
	dcb.fBinary = TRUE;			// Binary mode
	dcb.fParity = FALSE;		// Parity checking disabled
	dcb.fOutxCtsFlow = FALSE;	// No output flow control
	dcb.fOutxDsrFlow = FALSE;	// Idem
	dcb.fDtrControl = DTR_CONTROL_DISABLE;	// Provide power supply
	dcb.fDsrSensitivity = FALSE;	// ignore DSR status
	dcb.fOutX = FALSE;			// no XON/XOFF flow control
	dcb.fInX = FALSE;			// idem
	dcb.fErrorChar = FALSE;		// no replacement
	dcb.fNull = FALSE;			// don't discard null chars
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Provide power supply
	dcb.fAbortOnError = FALSE;	// do not report errors

	dcb.ByteSize = 8;			// 8 bits
	dcb.Parity = NOPARITY;		// no parity checking
	dcb.StopBits = ONESTOPBIT;	// 1 stop bit

	// Config COM port
	fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess) 
	{
		ticables_warning("SetCommState");
		return ERR_GRY_SETCOMMSTATE;
	}

	// Wait for GrayLink to be ready
	Sleep(250);

	// Set timeouts
	fSuccess = GetCommTimeouts(hCom, &cto);
	if (!fSuccess) 
	{
		ticables_warning("GetCommTimeouts");
		return ERR_GRY_GETCOMMTIMEOUT;
	}

	cto.ReadIntervalTimeout = 0;

	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = 100 * h->timeout;  

	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 100 * h->timeout;

	fSuccess = SetCommTimeouts(hCom, &cto);
	if (!fSuccess) 
	{
		ticables_warning("SetCommTimeouts");
		return ERR_GRY_SETCOMMTIMEOUT;
	}

	// Monitor receiving of chars
	fSuccess = SetCommMask(hCom, EV_RXCHAR);
	if (!fSuccess)
	{
		ticables_warning("SetCommMask");
		return ERR_GRY_SETCOMMMASK;
	}

	// Flush/Dicard buffers
	fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!fSuccess) 
	{
		ticables_warning("PurgeComm");
		return ERR_GRY_PURGECOMM;
	}

	return 0;
}

static int gry_close(CableHandle *h)
{
	if (hCom) 
	{
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}

	return 0;
}

static int gry_reset(CableHandle *h)
{
	BOOL fSuccess;

	fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!fSuccess) 
	{
		ticables_warning("PurgeComm");
		return ERR_GRY_PURGECOMM;
	}

	return 0;
}

static int gry_probe(CableHandle *h)
{
	DWORD status;			//MS_CTS_ON or MS_DTR_ON

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);	// Get MCR values
	if (status != 0x20)
	{
		return ERR_PROBE_FAILED;
	}

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, CLRRTS);
	GetCommModemStatus(hCom, &status);
	if (status != 0x20)
	{
		return ERR_PROBE_FAILED;
	}

	EscapeCommFunction(hCom, CLRDTR);
	EscapeCommFunction(hCom, CLRRTS);
	GetCommModemStatus(hCom, &status);
	if (status != 0x00)
	{
		return ERR_PROBE_FAILED;
	}

	EscapeCommFunction(hCom, CLRDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);
	if (status != 0x00)
	{
		return ERR_PROBE_FAILED;
	}

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);
	if (status != 0x20)
	{
		return ERR_PROBE_FAILED;
	}

	return 0;
}

static int gry_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	BOOL fSuccess;
	DWORD nBytesWritten;
	OVERLAPPED ol;

	memset(&ol, 0, sizeof(OVERLAPPED));
	fSuccess = WriteFile(hCom, data, len, &nBytesWritten, &ol);

	while(HasOverlappedIoCompleted(&ol) == FALSE) Sleep(0);

	fSuccess = GetOverlappedResult(hCom, &ol, &nBytesWritten, FALSE);
	if (!fSuccess) 
	{
		ticables_warning("WriteFile");
		return ERR_WRITE_ERROR;
	}
	else if (nBytesWritten == 0) 
	{
		ticables_warning("WriteFile");
		return ERR_WRITE_TIMEOUT;
	}
	else if (nBytesWritten < len)
	{
		ticables_warning("WriteFile");
		return ERR_WRITE_ERROR;
	}

	return 0;
}

static int gry_get(CableHandle* h, uint8_t *data, uint32_t len)
{
	BOOL fSuccess;
	DWORD nBytesRead;
	OVERLAPPED ol;
	uint32_t i;

	for(i = 0; i < len;)
	{
		memset(&ol, 0, sizeof(OVERLAPPED));
		fSuccess = ReadFile(hCom, data + i, len - i, &nBytesRead, &ol);

		while(HasOverlappedIoCompleted(&ol) == FALSE) Sleep(0);
		fSuccess = GetOverlappedResult(hCom, &ol, &nBytesRead, FALSE);

		if (!fSuccess) 
		{
			ticables_warning("ReadFile");
			return ERR_READ_ERROR;
		}
		else if (nBytesRead == 0) 
		{
			ticables_warning("ReadFile");
			return ERR_READ_TIMEOUT;
		}

		i += nBytesRead;
	}
	printf("get : %i %i %i\n", fSuccess, nBytesRead, len);

	return 0;
}

// pb with this code: works if no receiving of chars has been previously done
static int gry_check(CableHandle *h, int *status)
{
	BOOL fSuccess;
	static DWORD dwEvtMask = 0;
	static OVERLAPPED ol = { 0 };

	static BOOL iop;
	static BOOL ioPending = FALSE;

	if (ioPending == FALSE)
	{
		memset(&ol, 0, sizeof(OVERLAPPED));
		fSuccess = WaitCommEvent(hCom, &dwEvtMask, &ol);

		ioPending = TRUE;
		printf("$ (%i)\n", ioPending);
	}
	else
	{
		if (HasOverlappedIoCompleted(&ol))
		{
			if (dwEvtMask & EV_RXCHAR)
			{
				*status = STATUS_RX;
				printf("#\n");
				ioPending = FALSE;
			}
		}
	}

	return 0;
}

static int gry_timeout(CableHandle *h)
{
	BOOL fSuccess;
	COMMTIMEOUTS cto;

	cto.ReadIntervalTimeout = 0;

	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = 100 * h->timeout;  

	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 100 * h->timeout;

	fSuccess = SetCommTimeouts(hCom, &cto);
	if (!fSuccess) 
	{
		ticables_warning("SetCommTimeouts");
		return ERR_GRY_SETCOMMTIMEOUT;
	}

	return 0;
}

static int gry_set_device(CableHandle *h, const char * device)
{
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			free(h->device);
			h->device = device2;
		}
		else
		{
			ticables_warning(_("unable to set device %s.\n"), device);
		}
		return 0;
	}
	return ERR_ILLEGAL_ARG;
}

extern const CableFncts cable_gry = 
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),
	!0,
	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe, &gry_timeout,
	&gry_put, &gry_get, &gry_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&gry_set_device,
	NULL
};
