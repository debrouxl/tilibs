/*  ti_link - link program for TI calculators
 *  Copyright (C) 1999-2002  Romain Lievin
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
#include "cabl_def.h"
#include "export.h"

#if defined(__LINUX__) || defined(__SPARC__) || defined(__MACOSX__)

#include <config.h>
#include <stdio.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "timeout.h"
#include "verbose.h"
#include "logging.h"

static char tty_dev[MAXCHARS];
static int dev_fd = 0;
static struct termios termset;

static struct cs
{
  byte data;
  int available;
} cs;

/**************/
/* Linux part */
/**************/

int tig_close();

static unsigned int com_addr;
# define com_out (com_addr+4)
# define com_in  (com_addr+6)

int tig_init()
{
  /* Init some internal variables */
  memset((void *)&cs, 0, sizeof(cs));
  strcpy(tty_dev, io_device);
  com_addr = io_address;

  /* Give some perm for the probe function */
  // nothing for the moment

  /* Open the device */
#ifndef __MACOSX__
  if((dev_fd = open(io_device, O_RDWR | O_SYNC)) == -1)
#else
  if((dev_fd = open(io_device, O_RDWR)) == -1)
#endif
    {
      DISPLAY_ERROR("unable to open this serial port: %s\n", io_device);
      return ERR_OPEN_SER_DEV;
    }

  /* Initialize it: 9600 bauds, 8 bits of data, no parity and 1 stop bit */
  tcgetattr(dev_fd, &termset);
#ifdef HAVE_CFMAKERAW
  cfmakeraw(&termset);
#else
  termset.c_iflag=0;
  termset.c_oflag=0;
  termset.c_cflag=CS8|CLOCAL|CREAD;
  termset.c_lflag=0;
#endif

  cfsetispeed(&termset, B9600);
  cfsetospeed(&termset, B9600);

  START_LOGGING();

  return 0;
}

int tig_open()
{
  byte d;
  int n;

  /* Flush the input */
  termset.c_cc[VMIN]=0;
  termset.c_cc[VTIME]=1;
  tcsetattr(dev_fd, TCSANOW, &termset);
  do
    {
      n=read(dev_fd, (void *)(&d), 1);
    }
  while(n!=0 && n!=-1);

  /* and set the timeout */
  termset.c_cc[VTIME] = 0; //time_out;
  tcsetattr(dev_fd, TCSANOW, &termset);

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int tig_put(byte data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);
  err=write(dev_fd, (void *)(&data), 1);
  switch(err)
    {
    case -1: //error
      tig_close();
      return ERR_SND_BYT;
      break;
    case 0: // timeout
      tig_close();
      return ERR_SND_BYT_TIMEOUT;
      break;
    }

  return 0;
}

int tig_get(byte *data)
{
  static int n=0;
  TIME clk;

  tdr.count++;
  /* If the tig_check function was previously called, retrieve the byte */
  if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

  tcdrain(dev_fd); //waits until all output written
  toSTART(clk);
  do
    {
      if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
      n = read(dev_fd, (void *)data, 1);
    }
  while(n == 0);

  if(n == -1)
    return ERR_RCV_BYT;

  LOG_DATA(*data);
  
  return 0;
}

// Migrate these functions into ioports.c
static int dcb_read_io()
{
  unsigned int flags;

  if(ioctl(dev_fd, TIOCMGET, &flags) == -1)
    return ERR_IOCTL;
  /*
  DISPLAY("flags: rts=%i dtr=%i cts=%i dsr=%i\n",
         (flags & TIOCM_RTS) >> 2, (flags & TIOCM_DTR) >> 1,
         (flags & TIOCM_CTS) >> 5, (flags & TIOCM_DSR) >> 8);
  */
  return (flags&TIOCM_CTS?1:0) | (flags&TIOCM_DSR?2:0);
}

static int dcb_write_io(int data)
{
  unsigned int flags=0;

  flags |= (data&2)?TIOCM_RTS:0;
  flags |= (data&1)?TIOCM_DTR:0;
  if(ioctl(dev_fd, TIOCMSET, &flags) == -1)
    return ERR_IOCTL;
  return 0;
}

int tig_probe()
{
  int i;
  int seq[]={ 0x0, 0x2, 0x0, 0x2 };

  dcb_write_io(3);
  for(i=3; i>=0; i--)
    {
      dcb_write_io(i);
      if( (dcb_read_io() & 0x3) != seq[i])
        {
          dcb_write_io(3);
          return ERR_PROBE_FAILED;
        }
    }
  dcb_write_io(3);

  return 0;
}

int tig_close()
{
  /* Do not close the port else the communication will not work fine */
  /* Don't ask me why, I don't know ! */
  /*
  if(dev_fd)
    {
      close(dev_fd);
      dev_fd=0;
    }
  */
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
  int n = 0;

  /* Since the select function does not work, I do it myself ! */
  *status = STATUS_NONE;
  if(dev_fd)
    {
      n = read(dev_fd, (void *) (&cs.data), 1);
      if(n > 0)
	{
	  if(cs.available == 1)
	    return ERR_BYTE_LOST;

	  cs.available = 1;
	  *status = STATUS_RX;
	  return 0;
	}
		else
	{
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

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

/*
	Thanks to Laurent Goujon for the exhaustive documentation who sent
	to me about the use of COM ports under Windows.
	A great thanks !!!
 */

#include <stdio.h>
#include <windows.h>

#include "timeout.h"
#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "plerror.h"
#include "cabl_ext.h"
#include "logging.h"

#define BUFFER_SIZE 1024

extern int time_out; // Timeout value for cables in 0.10 seconds

static HANDLE hCom=0;
static char comPort[MAXCHARS];

static struct cs
{
  byte data;
  int available;
} cs;

int tig_init()
{
	DCB dcb;
	BOOL fSuccess;
	COMMTIMEOUTS cto;
	int graphLink = 1;

	/* Init some internal variables */
	memset((void *)(&cs), 0, sizeof(cs));
	strcpy(comPort, io_device);

	/* Open COM port */
	hCom = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		DISPLAY_ERROR("CreateFile\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}

	// Setup buffer size
	fSuccess = SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetupComm\n");
		print_last_error();
		return ERR_SETUP_COMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommState\n");
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
	fSuccess = SetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetCommState\n");
		print_last_error();
		return ERR_SET_COMMSTATE;
	}

    fSuccess=GetCommTimeouts(hCom,&cto);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommTimeouts\n");
		print_last_error();
		return ERR_GET_COMMTIMEOUT;
	}
    
	cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * time_out;	
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;	// A value of 0 make non-blocking

    fSuccess=SetCommTimeouts(hCom,&cto);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetCommTimeouts\n");
		print_last_error();
		return ERR_SET_COMMTIMEOUT;
	}

	START_LOGGING();

	return 0;
}

int tig_open()
{
	BOOL fSuccess;

	fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if(!fSuccess)
	{
		DISPLAY_ERROR("PurgeComm\n");
		print_last_error();
		return ERR_FLUSH;
	}

	tdr.count = 0;
	toSTART(tdr.start);

	return 0;
}

int tig_put(byte data)
{
	DWORD i;
	BOOL fSuccess;

	tdr.count++;
	LOG_DATA(data);
	// Write the data
	fSuccess=WriteFile(hCom, &data, 1, &i, NULL);
	if(!fSuccess)
	{
		DISPLAY_ERROR("WriteFile\n");
		print_last_error();
		return ERR_SND_BYT;
	}
	else if(i == 0)
	{
		return ERR_SND_BYT_TIMEOUT;
	}

	return 0;
}

int tig_get(byte *data)
{
	DWORD i;
	BOOL fSuccess;
	TIME clk;

	tdr.count++;
	/* If the tig_check function was previously called, retrieve the byte */
	if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

	toSTART(clk);
	do
    {
      if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
	  fSuccess = ReadFile(hCom,data,1,&i,NULL);
    }
	while(i != 1);

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

	if(hCom)
	{
		CloseHandle(hCom);
		hCom=0;
	}

	return 0;
}

#define MS_ON (MS_CTS_ON | MS_DTR_ON)

int tig_probe()
{
	DWORD status;				//MS_CTS_ON or MS_DTR_ON

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);	// Get MCR values
	//DISPLAY("status: %i\n", status);
	if(status != 0x20) return ERR_ABORT;

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, CLRRTS);
	GetCommModemStatus(hCom, &status);
	//DISPLAY("status: %i\n", status);
	if(status != 0x20) return ERR_ABORT;

	EscapeCommFunction(hCom, CLRDTR);
	EscapeCommFunction(hCom, CLRRTS);
	GetCommModemStatus(hCom, &status);
	//DISPLAY("status: %i\n", status);
	if(status != 0x00) return ERR_ABORT;

	EscapeCommFunction(hCom, CLRDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);
	//DISPLAY("status: %i\n", status);
	if(status != 0x00) return ERR_ABORT;

	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, SETRTS);
	GetCommModemStatus(hCom, &status);
	//DISPLAY("status: %i\n", status);
	if(status != 0x20) return ERR_ABORT;

	return 0;
}

int tig_check(int *status)
{
	int n = 0;
	DWORD i;
	BOOL fSuccess;

	*status = STATUS_NONE;
	if(hCom)
    {
	    // Read the data: return 0 if error and i contains 1 or 0 (timeout)
		fSuccess = ReadFile(hCom, (&cs.data), 1, &i, NULL);
		if(fSuccess && (i==1))
		{
			if(cs.available == 1)
				return ERR_BYTE_LOST;

			cs.available = 1;
			*status = STATUS_RX;
			return 0;
		}
		else
		{
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

#else // unsupported platforms

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

int tig_put(byte data)
{
  return 0;
}

int tig_get(byte *d)
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

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

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
		print_last_error();
		return ERR_CREATE_FILE;
	}

	// Setup buffer size
	fSuccess = SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetupComm\n");
		print_last_error();
		return ERR_SETUP_COMM;
	}

	// Retrieve config structure
	fSuccess = GetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommState\n");
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
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Provide power supply

	dcb.ByteSize = 8;						// 8 bits
	dcb.Parity = NOPARITY;					// no parity checking
	dcb.StopBits = ONESTOPBIT;				// 1 stop bit

	// Config COM port
	fSuccess = SetCommState(hCom, &dcb);
	if(!fSuccess)
	{
		DISPLAY_ERROR("SetCommState\n");
		print_last_error();
		return ERR_SET_COMMSTATE;
	}

    fSuccess=GetCommTimeouts(hCom,&cto);
	if(!fSuccess)
	{
		DISPLAY_ERROR("GetCommTimeouts\n");
		print_last_error();
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
		print_last_error();
		return ERR_SET_COMMTIMEOUT;
	}

	//DISPLAY("Serial port %s successfully reconfigured.\n", comPort);

	return 0;
}
*/
