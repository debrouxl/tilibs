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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* "Grey TIGraphLink" link cable unit */

#include <stdio.h>
#include <windows.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

typedef struct 
{
  uint8_t	data;
  BOOL		avail;
} CHK;

#define hCom	((HANDLE)(h->priv))
#define	sCheck	((CHK*)(h->priv2))

static int gry_prepare(CableHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0x3f8; h->device = strdup("COM1"); break;
	case PORT_2: h->address = 0x2f8; h->device = strdup("COM2"); break;
	case PORT_3: h->address = 0x3e8; h->device = strdup("COM3"); break;
	case PORT_4: h->address = 0x3e8; h->device = strdup("COM4"); break;
	default: return ERR_ILLEGAL_ARG;
	}

	h->priv2 = calloc(1, sizeof(CHK));

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
			FILE_ATTRIBUTE_NORMAL /*| FILE_FLAG_NO_BUFFERING*/,
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
  
    cto.ReadIntervalTimeout = MAXDWORD;	// don't use time-outs (make non-blocking)

    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 0;	
    
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

	// Flush/Dicard buffers (= gry_reset)
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
		h->priv = (void *)INVALID_HANDLE_VALUE;

		free(h->priv2);
		h->priv2 = NULL;
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
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, SETDTR);
    EscapeCommFunction(hCom, CLRRTS);
    GetCommModemStatus(hCom, &status);
    if (status != 0x20)
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, CLRDTR);
    EscapeCommFunction(hCom, CLRRTS);
    GetCommModemStatus(hCom, &status);
    if (status != 0x00)
      return ERR_PROBE_FAILED;

    EscapeCommFunction(hCom, CLRDTR);
    EscapeCommFunction(hCom, SETRTS);
    GetCommModemStatus(hCom, &status);
    if (status != 0x00)
      return ERR_PROBE_FAILED;
  
    EscapeCommFunction(hCom, SETDTR);
    EscapeCommFunction(hCom, SETRTS);
    GetCommModemStatus(hCom, &status);
	if (status != 0x20)
		return ERR_PROBE_FAILED;

	return 0;
}

static int gry_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	BOOL fSuccess;
	DWORD nBytesWritten;

    fSuccess = WriteFile(hCom, data, len, &nBytesWritten, NULL);

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
	tiTIME clk;
	uint32_t i;

	if(sCheck->avail) 
	{
		*data = sCheck->data;
		sCheck->avail = FALSE;

		data++;
		len--;
	}

	// get data per chunks
	for(i = 0; i < len;)
	{
		TO_START(clk);
		do
		{
			fSuccess = ReadFile(hCom, data + i, len - i, &nBytesRead, NULL);
			if (TO_ELAPSED(clk, h->timeout))
				return ERR_READ_TIMEOUT;
		}
		while(nBytesRead == 0);

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
  	
  	return 0;
}

static int gry_check(CableHandle *h, int *status)
{
	BOOL fSuccess;
	DWORD nBytesRead;
	uint8_t data;
	
	*status = 0;
	fSuccess = ReadFile(hCom, &sCheck->data, 1, &nBytesRead, NULL);

    if (fSuccess && (nBytesRead == 1)) 
	{
		data = sCheck->data;
		sCheck->avail = TRUE;
		*status = STATUS_RX;

		return 0;
    } 

	return 0;
}

static int gry_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

static int gry_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

static int gry_get_red_wire(CableHandle *h)
{
	return 1;
}

static int gry_get_white_wire(CableHandle *h)
{
	return 1;
}

static int gry_timeout(CableHandle *h)
{
	BOOL fSuccess;
	COMMTIMEOUTS cto;

    cto.ReadIntervalTimeout = MAXDWORD;	// don't use time-outs (make non-blocking)

    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 0;	
    
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

const CableFncts cable_gry = 
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),
	!0,
	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe, &gry_timeout,
	&gry_put, &gry_get, &gry_check,
	&gry_set_red_wire, &gry_set_white_wire,
	&gry_get_red_wire, &gry_get_white_wire,
};
