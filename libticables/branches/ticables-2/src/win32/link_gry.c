/* Hey EMACS -*- win32-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

/* "Grey TIGraphLink" link cable unit */

#include <stdio.h>
#include <windows.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

#define hCom	(HANDLE)(h->priv)

static struct cs {
  uint8_t data;
  BOOL avail;
} cs;

static int gry_prepare(TiHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0x3f8; h->device = strdup("COM1"); break;
	case PORT_2: h->address = 0x2f8; h->device = strdup("COM2"); break;
	case PORT_3: h->address = 0x3e8; h->device = strdup("COM3"); break;
	case PORT_4: h->address = 0x3e8; h->device = strdup("COM4"); break;
	default: return ERR_ILLEGAL_ARG;
	}

	return 0;
}

static int gry_open(TiHandle *h)
{
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;

	// Open device
	hCom = CreateFile(h->device, GENERIC_READ | GENERIC_WRITE, 0,
		    NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
			NULL);
	if (hCom == INVALID_HANDLE_VALUE) 
	{
		ticables_warning("CreateFile\n");
		return ERR_CREATEFILE;
	}
  
	// Setup buffer size
	fSuccess = SetupComm(hCom, 1024, 1024);
	if (!fSuccess) 
	{
		ticables_warning("SetupComm\n");
		return ERR_SETUPCOMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(hCom, &dcb);
	if (!fSuccess) 
	{
		ticables_warning("GetCommState\n");
		return ERR_GETCOMMSTATE;
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
		ticables_warning("SetCommState\n");
		return ERR_SETCOMMSTATE;
    }
  
    fSuccess = GetCommTimeouts(hCom, &cto);
    if (!fSuccess) 
    {
		ticables_warning("GetCommTimeouts\n");
		return ERR_GETCOMMTIMEOUT;
    }
  
    cto.ReadIntervalTimeout = 100 * h->timeout;

    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * h->timeout;  
    
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 100 * h->timeout;
  
    fSuccess = SetCommTimeouts(hCom, &cto);
    if (!fSuccess) 
    {
		ticables_warning("SetCommTimeouts\n");
		return ERR_SETCOMMTIMEOUT;
    }

	fSuccess = SetCommMask(hCom, EV_RXCHAR);
	if (!fSuccess)
    {
		ticables_warning("SetCommMask\n");
		return ERR_SETCOMMMASK;
    }

	return 0;
}

static int gry_close(TiHandle *h)
{
	if (hCom) 
	{
		CloseHandle(hCom);
		hCom = 0;
	}

	return 0;
}

static int gry_reset(TiHandle *h)
{
	BOOL fSuccess;

	memset((void *) (&cs), 0, sizeof(cs));

	fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!fSuccess) 
	{
		ticables_warning("PurgeComm\n");
		return ERR_PURGECOMM;
	}

	return 0;
}

#define MS_ON (MS_CTS_ON | MS_DTR_ON)

static int gry_probe(TiHandle *h)
{
	DWORD status;			//MS_CTS_ON or MS_DTR_ON

    EscapeCommFunction(hCom, SETDTR);
    EscapeCommFunction(hCom, SETRTS);
    GetCommModemStatus(hCom, &status);	// Get MCR values
    fprintf(stdout, "status: %i\n", status);
    if (status != 0x20)
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, SETDTR);
    EscapeCommFunction(hCom, CLRRTS);
    GetCommModemStatus(hCom, &status);
    fprintf(stdout, "status: %i\n", status);
    if (status != 0x20)
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, CLRDTR);
    EscapeCommFunction(hCom, CLRRTS);
    GetCommModemStatus(hCom, &status);
    fprintf(stdout, "status: %i\n", status);
    if (status != 0x00)
      return ERR_PROBE_FAILED;

    EscapeCommFunction(hCom, CLRDTR);
    EscapeCommFunction(hCom, SETRTS);
    GetCommModemStatus(hCom, &status);
    fprintf(stdout, "status: %i\n", status);
    if (status != 0x00)
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, SETDTR);
    EscapeCommFunction(hCom, SETRTS);
    GetCommModemStatus(hCom, &status);
	fprintf(stdout, "status: %i\n", status);
	if (status != 0x20)
		return ERR_PROBE_FAILED;

	return 0;
}

static int gry_put(TiHandle* h, uint8_t *data, uint16_t len)
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
		ticables_warning("WriteFile\n");
		return ERR_WRITE_ERROR;
    } 
    else if (nBytesWritten == 0) 
    {
		ticables_warning("WriteFile\n");
		return ERR_WRITE_TIMEOUT;
    }
	else if (nBytesWritten < len)
	{
		ticables_warning("WriteFile\n");
		return ERR_WRITE_ERROR;
	}

	return 0;
}

static int gry_get(TiHandle* h, uint8_t *data, uint16_t len)
{
	BOOL fSuccess;
	DWORD nBytesRead;
	OVERLAPPED ol;

	memset(&ol, 0, sizeof(OVERLAPPED));
    fSuccess = ReadFile(hCom, data, len, &nBytesRead, &ol);

	while(HasOverlappedIoCompleted(&ol) == FALSE) Sleep(0);

	fSuccess = GetOverlappedResult(hCom, &ol, &nBytesRead, FALSE);
	if (!fSuccess) 
    {
		ticables_warning("ReadFile\n");
		return ERR_READ_ERROR;
    }
	else if (nBytesRead == 0) 
    {
		ticables_warning("ReadFile\n");
		return ERR_READ_TIMEOUT;
    }
	else if (nBytesRead < len)
	{
		ticables_warning("ReadFile\n");
		return ERR_READ_ERROR;
	}
  	
  	return 0;
}

static int gry_check(TiHandle *h, int *status)
{
	BOOL fSuccess;
	DWORD dwEvtMask;
	OVERLAPPED ol;

	memset(&ol, 0, sizeof(OVERLAPPED));
    fSuccess = WaitCommEvent(hCom, &dwEvtMask, &ol);
	
	if(HasOverlappedIoCompleted(&ol))
		if(dwEvtMask & EV_RXCHAR)
			*status = STATUS_RX;

	return 0;
}

static int gry_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

static int gry_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

static int gry_get_red_wire(TiHandle *h)
{
	return 1;
}

static int gry_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_gry = 
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),

	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe,
	&gry_put, &gry_get, &gry_check,
	&gry_set_red_wire, &gry_set_white_wire,
	&gry_get_red_wire, &gry_get_white_wire,
};
