/*  ti_link - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

/* "fastAVRlink" link cable unit (one of my own link cables) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "export.h"
#include "cabl_def.h"

#if defined(__LINUX__) || defined(__SPARC__)

#include <stdio.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "timeout.h"
#include "typedefs.h"
#include "cabl_err.h"
#include "cabl_ext.h"

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

int avr_close_port();

DLLEXPORT
int avr_init_port()
{
  int br = BR9600;

  /* Init some internal variables */
  cs.available = 0;
  cs.data = 0;
  strcpy(tty_dev, device);

  /* Give some perm for the probe function */
  // nothing for the moment

  /* Open the device */
  if( (dev_fd = open(device, O_RDWR | O_SYNC )) == -1 )
    {
      fprintf(stderr, "Unable to open this serial port: %s\n", device);
      return ERR_OPEN_SER_DEV;
    }
  /* Initialize it: 9600 bauds, 8 bits of data, no parity and 1 stop bit */
  tcgetattr(dev_fd, &termset);
#ifdef HAVE_CFMAKERAW
  //cfmakeraw(&termset);
  termset.c_iflag=0;
  termset.c_oflag=0;
  if(hfc == HFC_ON)
    termset.c_cflag=CS8|CLOCAL|CREAD|CRTSCTS;
  else
    termset.c_cflag=CS8|CLOCAL|CREAD;
  termset.c_lflag=0;
#else
  termset.c_iflag=0;
  termset.c_oflag=0;
  if(hfc == HFC_ON)
    termset.c_cflag=CS8|CLOCAL|CREAD|CRTSCTS;
  else
    termset.c_cflag=CS8|CLOCAL|CREAD;
  termset.c_lflag=0;
#endif /* HAVE_CFMAKERAW */
  //DISPLAY("fastAVRlink baud-rate: %i\n", baud_rate);
  if(baud_rate == 9600)
    br = B9600;
  else if(baud_rate == 19200)
    br = B19200;
  else if(baud_rate == 38400)
    br = B38400;
  else if(baud_rate == 57600)
    br = B57600;
  else
    br = B9600;
  cfsetispeed(&termset, br);
  cfsetospeed(&termset, br);

  return 0;
}

DLLEXPORT
int avr_open_port()
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
      //printf("Flushing...\n");
    }
  while(n!=0 && n!=-1);
  /* and set the timeout */
  termset.c_cc[VTIME] = 0; //time_out;
  tcsetattr(dev_fd, TCSANOW, &termset);

  return 0;
}

DLLEXPORT
int avr_put(byte data)
{
  int err;

  err=write(dev_fd, (void *)(&data), 1);
  switch(err)
    {
    case -1: //error
      avr_close_port();
      return ERR_SND_BYT;
      break;
    case 0: // timeout
      avr_close_port();
      return ERR_SND_BYT_TIMEOUT;
      break;
    }

  return 0;
}

DLLEXPORT
int avr_get(byte *data)
{
  int n=0;
  TIME clk;

  /* If the avr_check function was previously called, retrieve the byte */
  if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

  tcdrain(dev_fd); //waits until all output written
  tSTART(clk);
  do
    {
      if(tELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
      n = read(dev_fd, (void *)data, 1);
    }
  while(n == 0);

  if(n == -1)
    return ERR_RCV_BYT;

  return 0;
}

DLLEXPORT
int avr_probe_port()
{
  return ERR_ABORT;
}

DLLEXPORT
int avr_close_port()
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

DLLEXPORT
int avr_term_port()
{
  close(dev_fd);
  return 0;
}

DLLEXPORT
int avr_check_port(int *status)
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

DLLEXPORT
int DLLEXPORT2 avr_supported()
{
  return SUPPORT_ON;
}

#elif defined(__WIN32)

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
#include <time.h>

#include "timeout.h"
#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "plerror.h"
#include "cabl_ext.h"

#define BUFFER_SIZE 1024

extern int time_out; // Timeout value for cables in 0.10 seconds

static HANDLE hCom=0;
static char comPort[MAXCHARS];
static int byte_count = 0;

static struct cs
{
  byte data;
  int available;
} cs;

DLLEXPORT
int avr_init_port()
{
	DCB dcb;
	COMMTIMEOUTS cto;
	int br;
	char str[128];
	int graphLink = 1;

	cs.available = 0;
	cs.data = 0;
	strcpy(comPort, device);

	// Open COM port
	hCom=CreateFile(comPort,GENERIC_READ|GENERIC_WRITE,0,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "CreateFile\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}

	if(baud_rate == 9600)
	  br = CBR_9600;
	else if(baud_rate == 19200)
	  br = CBR_19200;
	else if(baud_rate == 38400)
	  br = CBR_38400;
	else if(baud_rate == 57600)
	  br = CBR_57600;
	else
	  br = CBR_38400;

	// Fills the structure with config
	dcb.DCBlength=sizeof(DCB);
    dcb.BaudRate = br; 
	dcb.ByteSize = 8; 
	dcb.Parity = 0;
    dcb.StopBits=graphLink?0:1;
    dcb.fNull = 0; 
	dcb.fBinary = 1; 
	dcb.fParity = 0;
    dcb.fOutxCtsFlow = 1; 
	dcb.fOutxDsrFlow = 1;
    dcb.fDtrControl = 0;
    dcb.fOutX = 0; 
	dcb.fInX = 0;
    dcb.fErrorChar = 0; 
	if(hfc == HFC_ON)
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	else
	  dcb.fRtsControl = RTS_CONTROL_ENABLE;
    SetupComm(hCom, BUFFER_SIZE, BUFFER_SIZE);
    GetCommState(hCom,&dcb);
    sprintf(str,"%s: baud=%i parity=N data=8 stop=1", comPort, baud_rate);
    BuildCommDCB(str, &dcb);
    dcb.fNull=0;
    SetCommState(hCom, &dcb);
    cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 100 * time_out;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(hCom, &cto);
	
	return 0;
}

DLLEXPORT
int avr_open_port()
{
	BOOL fSuccess;

	fSuccess = PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if(!fSuccess)
	{
		fprintf(stderr, "PurgeComm\n");
		print_last_error();
		return ERR_FLUSH;
	}
	byte_count = 0;

	return 0;
}

DLLEXPORT
int avr_put(byte data)
{
	DWORD i;
	BOOL fSuccess;

	// Write the data
	fSuccess=WriteFile(hCom, &data, 1, &i, NULL);
	if(!fSuccess)
	{
		fprintf(stderr, "WriteFile\n");
		print_last_error();
		return ERR_SND_BYT;
	}
	else if(i == 0)
	{
		return ERR_SND_BYT_TIMEOUT;
	}

	return 0;
}

DLLEXPORT
int avr_get(byte *data)
{
	DWORD i;
	BOOL fSuccess;
	TIME clk;

	/* If the avr_check function was previously called, retrieve the byte */
	if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

	tSTART(clk);
	do
    {
      if(tELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
	  fSuccess = ReadFile(hCom,data,1,&i,NULL);
    }
	while(i != 1);

/*
	if(n == -1)
    {
      return ERR_RCV_BYT;
    }


	// Read the data: return 0 if error and i contains 1 or 0 (timeout)
	fSuccess = ReadFile(hCom,data,1,&i,NULL);
	if(!fSuccess)
	{
		fprintf(stderr, "ReadFile failed with error %d.\n", 
			GetLastError());
		return ERR_READ_FILE;
	}
	if( fSuccess & (i == 0) )
	{
		//printf("Timeout\n");
		return ERR_RCV_BYT_TIMEOUT;
	}
*/
	return 0;
}

DLLEXPORT
int avr_close_port()
{
	return 0;
}

DLLEXPORT
int avr_term_port()
{
	if(hCom)
	{
		CloseHandle(hCom);
		hCom=0;
	}

	return 0;
}

DLLEXPORT
int avr_probe_port()
{
	return 0;
}

DLLEXPORT
int avr_check_port(int *status)
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

DLLEXPORT
int DLLEXPORT2 avr_supported()
{
  return SUPPORT_ON | SUPPORT_DCB;
}

#else

// unsupported platforms

/************************/
/* Unsupported platform */
/************************/

/* static unsigned int com_addr; */

DLLEXPORT
int avr_init_port()
{
  return 0;
}

DLLEXPORT
int avr_open_port()
{
  return 0;
}

DLLEXPORT
int avr_put(byte data)
{
  return 0;
}

DLLEXPORT
int avr_get(byte *d)
{
  return 0;
}

DLLEXPORT
int avr_probe_port()
{
  return 0;
}

DLLEXPORT
int avr_close_port()
{
  return 0;
}

DLLEXPORT
int avr_term_port()
{
  return 0;
}

DLLEXPORT
int avr_check_port(int *status)
{
  return 0;
}

 //#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int avr_set_red_wire(int b)
{
  return 0;
}

int avr_set_white_wire(int b)
{
  return 0;
}

int avr_get_red_wire()
{
  return 0;
}

int avr_get_white_wire()
{
  return 0;
}

DLLEXPORT
int avr_supported()
{
  return SUPPORT_OFF;
}

#endif