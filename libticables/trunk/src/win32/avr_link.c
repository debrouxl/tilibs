/* Hey EMACS -*- win32-c -*- */
/* $Id: avr_link.c 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

/* "fastAVRlink" link cable unit (my own link cables) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <windows.h>

#include "../timeout.h"
#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "externs.h"
#include "logging.h"
#include "printl.h"
#include "logging.h"

#define BUFFER_SIZE 1024

extern int time_out;		// Timeout value for cables in 0.10 seconds

static HANDLE hCom = 0;
static char comPort[1024];
static struct cs {
  uint8_t data;
  int avail;
} cs;

int avr_init()
{
  DCB dcb;
  COMMTIMEOUTS cto;
  int br;
  BOOL fSuccess;

  strcpy(comPort, io_device);

  // Open COM port
  hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hCom == INVALID_HANDLE_VALUE) {
    printl1(2, "CreateFile\n");
    return ERR_OPEN_SER_COMM;
  }
  // Setup buffer size
  fSuccess = SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
  if (!fSuccess) {
    printl1(2, "SetupComm\n");
    return ERR_SETUP_COMM;
  }
  // Retrieve config structure
  fSuccess = GetCommState(hCom, &dcb);
  if (!fSuccess) {
    printl1(2, "GetCommState\n");
    return ERR_GET_COMMSTATE;
  }
  // select baud-rate
  if (baud_rate == 9600)
    br = CBR_9600;
  else if (baud_rate == 19200)
    br = CBR_19200;
  else if (baud_rate == 38400)
    br = CBR_38400;
  else if (baud_rate == 57600)
    br = CBR_57600;
  else
    br = CBR_38400;

  // Fills the structure with config
  dcb.BaudRate = br;		// 9600 bauds
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
  if (hfc == HFC_ON)
    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
  else
    dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Hardware flow control
  dcb.fAbortOnError = FALSE;	// do not report errors

  dcb.ByteSize = 8;		// 8 bits
  dcb.Parity = NOPARITY;	// no parity checking
  dcb.StopBits = ONESTOPBIT;	// 1 stop bit

  // Config COM port
  fSuccess = SetCommState(hCom, &dcb);
  if (!fSuccess) {
    printl1(2, "SetCommState\n");
    return ERR_SET_COMMSTATE;
  }

  fSuccess = GetCommTimeouts(hCom, &cto);
  if (!fSuccess) {
    printl1(2, "GetCommTimeouts\n");
    return ERR_GET_COMMTIMEOUT;
  }

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 0;	//100 * time_out;      
  cto.WriteTotalTimeoutMultiplier = 0;
  cto.WriteTotalTimeoutConstant = 100 * time_out;	// A value of 0 make non-blocking

  fSuccess = SetCommTimeouts(hCom, &cto);
  if (!fSuccess) {
    printl1(2, "SetCommTimeouts\n");
    return ERR_SET_COMMTIMEOUT;
  }

  START_LOGGING();

  return 0;
}

int avr_exit()
{
  STOP_LOGGING();
  if (hCom) {
    CloseHandle(hCom);
    hCom = 0;
  }

  return 0;
}

int avr_open()
{
  BOOL fSuccess;

  memset((void *) (&cs), 0, sizeof(cs));

  fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
  if (!fSuccess) {
    printl1(2, "PurgeComm\n");
    //print_last_error();
    return ERR_FLUSH_COMM;
  }

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int avr_close()
{
  return 0;
}

int avr_put(uint8_t data)
{
  DWORD i;
  BOOL fSuccess;

  tdr.count++;
  LOG_DATA(data);

  fSuccess = WriteFile(hCom, &data, 1, &i, NULL);
  if (!fSuccess) {
    printl1(2, "WriteFile\n");
    return ERR_WRITE_ERROR;
  } else if (i == 0) {
    printl1(2, "WriteFile\n");
    return ERR_WRITE_TIMEOUT;
  }

  return 0;
}

int avr_get(uint8_t * data)
{
  BOOL fSuccess;
  DWORD i;
  tiTIME clk;

  if (cs.avail) {
    *data = cs.data;
    cs.avail = FALSE;
    return 0;
  }

  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
    fSuccess = ReadFile(hCom, data, 1, &i, NULL);
  }
  while (i != 1);
  if (!fSuccess) {
    printl1(2, "ReadFile\n");
    return ERR_READ_ERROR;
  }

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int avr_check(int *status)
{
  DWORD i;
  BOOL fSuccess;

  *status = STATUS_NONE;
  if (hCom) {
    // Read the data: return 0 if error and i contains 1 or 0 (timeout)
    fSuccess = ReadFile(hCom, &cs.data, 1, &i, NULL);
    if (fSuccess && (i == 1)) {
      if (cs.avail == TRUE)
	return ERR_BYTE_LOST;

      cs.avail = TRUE;
      *status = STATUS_RX;
      return 0;
    } else {
      *status = STATUS_NONE;
      return 0;
    }
  }

  return 0;
}

int avr_probe()
{
  return 0;
}

int avr_supported()
{
  return SUPPORT_ON | SUPPORT_DCB;
}

int avr_register_cable(TicableLinkCable * lc)
{
  lc->init = avr_init;
  lc->open = avr_open;
  lc->put = avr_put;
  lc->get = avr_get;
  lc->close = avr_close;
  lc->exit = avr_exit;
  lc->probe = avr_probe;
  lc->check = avr_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
