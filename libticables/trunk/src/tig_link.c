/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

/* "Grey TIGraphLink" link cable unit */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(__LINUX__) || defined(__SPARC__) || defined(__MACOSX__) || defined(__BSD__)

#include <fcntl.h>
#include <stdio.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
# include <inttypes.h>
#endif
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "intl.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "timeout.h"
#include "verbose.h"
#include "logging.h"

static int dev_fd = 0;
static struct termios termset;

/**************/
/* Linux part */
/**************/

int tig_close();

int tig_init()
{
  int flags = 0;

#if defined(__MACOSX__)
  flags = O_RDWR | O_NDELAY;
#elif defined(__BSD__)
  flags = O_RDWR | O_FSYNC;
#else
  flags = O_RDWR | O_SYNC;
#endif

  if ((dev_fd = open(io_device, flags)) == -1) {
    DISPLAY_ERROR(_("unable to open the <%s> serial port.\n"), io_device);
    return ERR_OPEN_SER_DEV;
  }
  // Initialize it: 9600,8,N,1
  tcgetattr(dev_fd, &termset);
#ifdef HAVE_CFMAKERAW
  cfmakeraw(&termset);
#else
  termset.c_iflag = 0;
  termset.c_oflag = 0;
  termset.c_cflag = CS8 | CLOCAL | CREAD;
  termset.c_lflag = 0;
#endif

  cfsetispeed(&termset, B9600);
  cfsetospeed(&termset, B9600);

  START_LOGGING();

  return 0;
}

int tig_open()
{
  uint8_t unused[1024];
  int n;

  /* Flush the input */
  termset.c_cc[VMIN] = 0;
  termset.c_cc[VTIME] = 0;
  tcsetattr(dev_fd, TCSANOW, &termset);
  do {
    n = read(dev_fd, (void *) unused, 1024);
  } while ((n != 0) && (n != -1));

  /* and set/restore the timeout */
  termset.c_cc[VTIME] = time_out;
  tcsetattr(dev_fd, TCSANOW, &termset);

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int tig_put(uint8_t data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);

  err = write(dev_fd, (void *) (&data), 1);
  switch (err) {
  case -1:			//error
    tig_close();
    return ERR_WRITE_ERROR;
    break;
  case 0:			// timeout
    tig_close();
    return ERR_WRITE_TIMEOUT;
    break;
  }

  return 0;
}

int tig_get(uint8_t * data)
{
  int err;

  tcdrain(dev_fd);		// waits for all output written

  err = read(dev_fd, (void *) data, 1);
  switch (err) {
  case -1:			//error
    tig_close();
    return ERR_READ_ERROR;
    break;
  case 0:			// timeout
    tig_close();
    return ERR_READ_TIMEOUT;
    break;
  }

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

// Migrate these functions into ioports.c
static int dcb_read_io()
{
  unsigned int flags;

  if (ioctl(dev_fd, TIOCMGET, &flags) == -1)
    return ERR_IOCTL;

  return (flags & TIOCM_CTS ? 1 : 0) | (flags & TIOCM_DSR ? 2 : 0);
}

static int dcb_write_io(int data)
{
  unsigned int flags = 0;

  flags |= (data & 2) ? TIOCM_RTS : 0;
  flags |= (data & 1) ? TIOCM_DTR : 0;
  if (ioctl(dev_fd, TIOCMSET, &flags) == -1)
    return ERR_IOCTL;

  return 0;
}

int tig_probe()
{
  int i;
  int seq[] = { 0x0, 0x2, 0x0, 0x2 };

  dcb_write_io(3);
  for (i = 3; i >= 0; i--) {
    dcb_write_io(i);
    if ((dcb_read_io() & 0x3) != seq[i]) {
      dcb_write_io(3);
      return ERR_PROBE_FAILED;
    }
  }
  dcb_write_io(3);

  return 0;
}

int tig_close()
{
  return 0;
}

int tig_exit()
{
  STOP_LOGGING();
  close(dev_fd);
  return 0;
}

int tig_check(int *status)
{
  fd_set rdfs;
  struct timeval tv;
  int retval;

  *status = STATUS_NONE;

  FD_ZERO(&rdfs);
  FD_SET(dev_fd, &rdfs);
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  retval = select(dev_fd + 1, &rdfs, NULL, NULL, &tv);
  switch (retval) {
  case -1:			//error
    return ERR_READ_ERROR;
  case 0:			//no data
    return 0;
  default:			// data available
    *status = STATUS_RX;
    break;
  }

  return 0;
}

int tig_supported()
{
  return SUPPORT_ON;
}

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

#include <stdio.h>
#include <windows.h>

#include "timeout.h"
#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "externs.h"
#include "logging.h"
#include "verbose.h"

#define BUFFER_SIZE 1024

extern int time_out;		// Timeout value for cables in 0.10 seconds

static char comPort[1024];
static HANDLE hCom = 0;
static struct cs {
  uint8_t data;
  BOOL avail;
} cs;


int tig_init()
{
  DCB dcb;
  BOOL fSuccess;
  COMMTIMEOUTS cto;

  strcpy(comPort, io_device);

  hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0,
		    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hCom == INVALID_HANDLE_VALUE) {
    DISPLAY_ERROR("CreateFile\n");
    return ERR_OPEN_SER_COMM;
  }
  // Setup buffer size
  fSuccess = SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
  if (!fSuccess) {
    DISPLAY_ERROR("SetupComm\n");
    return ERR_SETUP_COMM;
  }
  // Retrieve config structure
  fSuccess = GetCommState(hCom, &dcb);
  if (!fSuccess) {
    DISPLAY_ERROR("GetCommState\n");
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
  fSuccess = SetCommState(hCom, &dcb);
  if (!fSuccess) {
    DISPLAY_ERROR("SetCommState\n");
    return ERR_SET_COMMSTATE;
  }

  fSuccess = GetCommTimeouts(hCom, &cto);
  if (!fSuccess) {
    DISPLAY_ERROR("GetCommTimeouts\n");
    return ERR_GET_COMMTIMEOUT;
  }

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 0;	//100 * time_out;      
  cto.WriteTotalTimeoutMultiplier = 0;
  cto.WriteTotalTimeoutConstant = 100 * time_out;	// A value of 0 make non-blocking

  fSuccess = SetCommTimeouts(hCom, &cto);
  if (!fSuccess) {
    DISPLAY_ERROR("SetCommTimeouts\n");
    return ERR_SET_COMMTIMEOUT;
  }

  START_LOGGING();

  return 0;
}

int tig_open()
{
  BOOL fSuccess;

  memset((void *) (&cs), 0, sizeof(cs));

  fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
  if (!fSuccess) {
    DISPLAY_ERROR("PurgeComm\n");
    return ERR_FLUSH_COMM;
  }

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int tig_put(uint8_t data)
{
  DWORD i;
  BOOL fSuccess;

  tdr.count++;
  LOG_DATA(data);

  fSuccess = WriteFile(hCom, &data, 1, &i, NULL);
  if (!fSuccess) {
    DISPLAY_ERROR("WriteFile\n");
    return ERR_WRITE_ERROR;
  } else if (i == 0) {
    DISPLAY_ERROR("WriteFile\n");
    return ERR_WRITE_TIMEOUT;
  }

  return 0;
}

int tig_get(uint8_t * data)
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
    DISPLAY_ERROR("ReadFile\n");
    return ERR_READ_ERROR;
  }

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int tig_close()
{
  return 0;
}

int tig_exit()
{
  STOP_LOGGING();
  if (hCom) {
    CloseHandle(hCom);
    hCom = 0;
  }

  return 0;
}

#define MS_ON (MS_CTS_ON | MS_DTR_ON)

int tig_probe()
{
  DWORD status;			//MS_CTS_ON or MS_DTR_ON

  EscapeCommFunction(hCom, SETDTR);
  EscapeCommFunction(hCom, SETRTS);
  GetCommModemStatus(hCom, &status);	// Get MCR values
  //DISPLAY("status: %i\n", status);
  if (status != 0x20)
    return ERR_PROBE_FAILED;

  EscapeCommFunction(hCom, SETDTR);
  EscapeCommFunction(hCom, CLRRTS);
  GetCommModemStatus(hCom, &status);
  //DISPLAY("status: %i\n", status);
  if (status != 0x20)
    return ERR_PROBE_FAILED;

  EscapeCommFunction(hCom, CLRDTR);
  EscapeCommFunction(hCom, CLRRTS);
  GetCommModemStatus(hCom, &status);
  //DISPLAY("status: %i\n", status);
  if (status != 0x00)
    return ERR_PROBE_FAILED;

  EscapeCommFunction(hCom, CLRDTR);
  EscapeCommFunction(hCom, SETRTS);
  GetCommModemStatus(hCom, &status);
  //DISPLAY("status: %i\n", status);
  if (status != 0x00)
    return ERR_PROBE_FAILED;

  EscapeCommFunction(hCom, SETDTR);
  EscapeCommFunction(hCom, SETRTS);
  GetCommModemStatus(hCom, &status);
  //DISPLAY("status: %i\n", status);
  if (status != 0x20)
    return ERR_PROBE_FAILED;

  return 0;
}

int tig_check(int *status)
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

int tig_supported()
{
  return SUPPORT_ON;
}

#else				// unsupported platforms

/************************/
/* Unsupported platform */
/************************/

int tig_init()
{
  return 0;
}

int tig_open()
{
  return 0;
}

int tig_put(uint8_t data)
{
  return 0;
}

int tig_get(uint8_t * d)
{
  return 0;
}

int tig_probe()
{
  return 0;
}

int tig_close()
{
  return 0;
}

int tig_exit()
{
  return 0;
}

int tig_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int tig_set_red_wire(int b)
{
  return 0;
}

int tig_set_white_wire(int b)
{
  return 0;
}

int tig_get_red_wire()
{
  return 0;
}

int tig_get_white_wire()
{
  return 0;
}

int tig_supported()
{
  return SUPPORT_OFF;
}

#endif

/* Old code, up to libticables v1.7.1 */
/*
int tig_init()
{
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;


	cs.available = 0;
	cs.data = 0;
	strcpy(comPort, io_device);
	// Open COM port
	hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		DISPLAY_ERROR("CreateFile\n");
		//print_last_error();
		return ERR_CREATE_FILE;
	}

	// Setup buffer size
	fSuccess = SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetupComm\n");
		//print_last_error();
		return ERR_SETUP_COMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommState\n");
		//print_last_error();
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
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Provide power supply

	dcb.ByteSize = 8;						// 8 bits
	dcb.Parity = NOPARITY;					// no parity checking
	dcb.StopBits = ONESTOPBIT;				// 1 stop bit

	// Config COM port
	fSuccess = SetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetCommState\n");
		//print_last_error();
		return ERR_SET_COMMSTATE;
	}

    fSuccess=GetCommTimeouts(hCom,&cto);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommTimeouts\n");
		//print_last_error();
		return ERR_GET_COMMTIMEOUT;
	}
    
	cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * time_out;	
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;	// A value of 0 make ReadFile non-blocking

    fSuccess=SetCommTimeouts(hCom,&cto);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetCommTimeouts\n");
		//print_last_error();
		return ERR_SET_COMMTIMEOUT;
	}

	//DISPLAY("Serial port %s successfully reconfigured.\n", comPort);

	return 0;
}
*/
