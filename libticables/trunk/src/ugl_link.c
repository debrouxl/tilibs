/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin, Julien BLACHE.
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

/* TI-GRAPH LINK USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "verbose.h"

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   This link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   This is transparent for the user because the driver manages all these 
   things for us. Nethertheless, this fact has some consequences:
   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.
   - for reading, we try to read up to 32 bytes and we store them in a buffer 
   even if we need only a byte. Else, if we try to get byte per byte, it 
   will not work.
   - for writing, we store bytes in a buffer. The buffer is flushed (sent) is 
   buffer size > 32 or if the get function has been called.
   - particular effect: sometimes (usually when calc need to reply and takes 
   some time), a read call can returns with no data or timeout. Simply retry
   a read call and it works fine.
*/


/*********************************/
/* Linux : kernel module support */
/*********************************/

#if defined(__LINUX__)

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "timeout.h"
#include "export.h"
#include "verbose.h"
#include "logging.h"


//#define BUFFERED_W /* enable buffered write operations */ 
  #define BUFFERED_R /* enable buffered read operations (default) */

#define MAX_PACKET_SIZE 32 // 32 bytes max per packet
static int nBytesWrite = 0;
#ifdef BUFFERED_W
static byte wBuf[MAX_PACKET_SIZE];
#endif
static int nBytesRead = 0;
static byte rBuf[MAX_PACKET_SIZE];

static int dev_fd = 0;

static struct cs
{
  byte data;
  int available;
} cs;

#ifdef HAVE_TI_TIUSB_H
# define IOCTL_EXPORT  //use tiusb.h file
# include <ti/tiusb.h> //ioctl codes
# include <sys/ioctl.h>
#endif

int ugl_init()
{
  int mask;
  /* Init some internal variables */
  cs.available = 0;
  cs.data = 0;

  /* Open the device */
  mask = O_RDWR | O_NONBLOCK | O_SYNC;
  if( (dev_fd = open(io_device, mask)) == -1)
    {
      DISPLAY("Unable to open this device: %s\n", io_device);
      DISPLAY("Is the tiusb.c module loaded ?\n");
      return ERR_USB_OPEN;
    }
 
#ifdef HAVE_TI_TIUSB_H
  {
    int arg  = time_out;
    if(ioctl(dev_fd, IOCTL_TIGLUSB_TIMEOUT, arg) == -1)
      {
	DISPLAY("Unable to use IOCTL codes.\n");
	return ERR_IOCTL;
      }
  }
#endif

  START_LOGGING();

  return 0;
}

int ugl_open(void)
{
  /* Clear buffers */
  nBytesRead = 0;
  nBytesWrite = 0;

  /* Reset both endpoints */
#ifdef HAVE_TI_TIUSB_H
  {
    int arg = 0;
    if(ioctl(dev_fd, IOCTL_TIGLUSB_RESET, arg) == -1)
      {
	DISPLAY("Unable to use IOCTL codes.\n");
	return ERR_IOCTL;
      }
  }
#endif
  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int ugl_put(byte data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);
#ifndef BUFFERED_W
  /* Byte per byte */
  err = write(dev_fd, (void *)(&data), 1);
  switch(err)
    {
    case -1: //error
      return ERR_SND_BYT;
      break;
    case 0: // timeout
      return ERR_SND_BYT_TIMEOUT;
      break;
    }
#else 
  /* Packets (up to 32 bytes) */
  wBuf[nBytesWrite++] = data;
  if(nBytesWrite == MAX_PACKET_SIZE)
    {
      err = write(dev_fd, (void *)(&wBuf), nBytesWrite);
      nBytesWrite = 0;

      switch(err)
	{
	case -1: //error
	  return ERR_SND_BYT;
	  break;
	case 0: // timeout
	  return ERR_SND_BYT_TIMEOUT;
	  break;
	}
    }
#endif
  LOG_DATA(data);

  return 0;
}

int ugl_get(byte *data)
{
  TIME clk;
  static byte *rBufPtr;
  int ret;

  tdr.count++;
#ifdef BUFFERED_W
  /* Flush write buffer */
  if(nBytesWrite > 0)
    {
      ret = write(dev_fd, (void *)(&wBuf), nBytesWrite);
      nBytesWrite = 0;
      switch(err)
        {
        case -1: //error
          return ERR_SND_BYT;
          break;
        case 0: // timeout
          return ERR_SND_BYT_TIMEOUT;
          break;
        }
    }
#endif

#ifdef BUFFERED_R  
  /* This routine try to read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and 
     store them in a buffer for subsequent accesses */
  if(nBytesRead == 0)
    {
      toSTART(clk);
      do 
	{ 
	  ret = read(dev_fd, (void *)rBuf, MAX_PACKET_SIZE);
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
	  if(ret == 0)
	    DISPLAY_ERROR("usb_bulk_read returns without any data. Retrying...\n");
	} 
      while(!ret);

      if (ret < 0)
	{
	  nBytesRead = 0;
	  return ERR_RCV_BYT;
	}	
      nBytesRead = ret;
      rBufPtr = rBuf;
    }
	
  *data = *rBufPtr++;
  nBytesRead--;
#else
  nBytesRead = read(dev_fd, (void *)data, 1);
  if(nBytesRead == -1) return ERR_RCV_BYT;
  if(nBytesRead == 0) return ERR_RCV_BYT_TIMEOUT;
#endif

  LOG_DATA(*data);

  return 0;
}

int ugl_probe(void)
{
  return 0;
}

int ugl_close(void)
{
  return 0;
}

int ugl_exit()
{
  STOP_LOGGING();
  if(dev_fd)
    {
      close(dev_fd);
      dev_fd=0;
    }

  return 0;
}

int ugl_check(int *status)
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

int ugl_supported()
{
  return SUPPORT_ON;
}


/*********************************/
/* Linux   : libusb support      */
/* Author  : Julien BLACHE       */
/* Contact : jb@technologeek.org */
/* Date    : 20011126            */
/*********************************/

#ifdef HAVE_LIBUSB /* change according to configure */

#include "usb.h"

#define TIGL_VENDOR_ID  0x0451 /* Texas Instruments, Inc.        */
#define TIGL_PRODUCT_ID 0xE001 /* TI-GRAPH LINK USB (SilverLink) */
#define TIGL_BULK_OUT 2
#define TIGL_BULK_IN  1

struct usb_bus    *bus      = NULL;
struct usb_device *dev      = NULL;
struct usb_device *tigl_dev = NULL;
usb_dev_handle    *tigl_han = NULL;

static void find_tigl_device(void)
{
  /* loop taken from testlibusb.c */
  for (bus = usb_busses; bus; bus = bus->next)
    {
      for (dev = bus->devices; dev; dev = dev->next)
	{
	  if ((dev->descriptor.idVendor == TIGL_VENDOR_ID) && 
	      (dev->descriptor.idProduct == TIGL_PRODUCT_ID))
	    {
	      /* keep track of the TIGL device */
	      DISPLAY("TIGL-USB found.\n");

	      tigl_dev = dev;
	      break;
	    }
	}

      /* if we found the device, then stop... */
      if (tigl_dev != NULL)
	break;
    }
}

int ugl_init2()
{
  int ret = 0;

  /* init the libusb */
  usb_init();

  /* find all usb busses on the system */
  ret = usb_find_busses();

  if (ret < 0)
    {
      DISPLAY_ERROR("%s\n", usb_strerror());
      return ERR_USB_INIT;
    }

  /* find all usb devices on all discovered busses */
  ret = usb_find_devices();

  if (ret < 0)
    {
      DISPLAY_ERROR("error, %s\n", usb_strerror());
      return ERR_USB_INIT;
    }

  /* iterate through the busses/devices */
  find_tigl_device();

  /* if we didn't find our TIGL USB, then ugl_init() and retry... */
  if (tigl_dev != NULL)
    {
      tigl_han = usb_open(tigl_dev);

      if (tigl_han != NULL)
	{
	  /* interface 0, configuration 1 */
	  ret = usb_claim_interface(tigl_han, 0);
	  
	  if (ret < 0)
	    {
	      DISPLAY_ERROR("%s\n", usb_strerror());
	      return ERR_USB_OPEN;
	    }

	  ret = usb_set_configuration(tigl_han, 1);

	  if (ret < 0)
	    {
	      DISPLAY_ERROR("%s\n", usb_strerror());
	      return ERR_USB_OPEN;
	    }

	  return 0;
	}
      else
	return ERR_USB_OPEN;
    }

  if(tigl_han == NULL)
    return ERR_USB_OPEN;

  START_LOGGING();

  return 0;
}

int ugl_open2()
{
  int ret = 0;

  /* Clear buffers */
  /*
  ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rdBuf, 
		      MAX_PACKET_SIZE, (time_out * 10));
  */

  /* Reset both endpoints */
  ret = usb_resetep(tigl_han, TIGL_BULK_IN);
  if (ret < 0)
    {
      DISPLAY_ERROR("%s\n", usb_strerror());
      return ERR_USB_OPEN;
    }
  ret = usb_resetep(tigl_han, TIGL_BULK_OUT);
  if (ret < 0)
    {
      DISPLAY_ERROR("%s\n", usb_strerror());
      return ERR_USB_OPEN;
    }

  /* Reset buffers */    
  nBytesRead = 0;
  nBytesWrite = 0;

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int ugl_close2()
{
  return 0;
}

int ugl_exit2()
{
  STOP_LOGGING();
  tigl_dev = NULL;
  
  if (tigl_han != NULL)
    {
      usb_release_interface(tigl_han, 0);
      
      usb_close(tigl_han);
      tigl_han = NULL;
    }

  return 0;
}

int ugl_put2(byte data)
{
  int ret = 0;

  tdr.count++;
  LOG_DATA(data);
#ifndef BUFFERED_W
  /* Byte per byte */
  ret = usb_bulk_write(tigl_han, TIGL_BULK_OUT, &data, 1, (time_out * 10));
  if (ret <= 0)
    {
      DISPLAY_ERROR("%s\n", usb_strerror());
      return ERR_SND_BYT;
    }
#else
  /* Packets (up to 32 bytes) */
  wBuf[nBytesWrite++] = data;
  if(nBytesWrite == MAX_PACKET_SIZE)
    {
      ret = usb_bulk_write(tigl_han, TIGL_BULK_OUT, wBuf, nBytesWrite, 
			   (time_out * 10));
      if (ret <= 0)
	{
	  DISPLAY_ERROR("%s\n", usb_strerror());
	  return ERR_SND_BYT;
	}
      nBytesWrite = 0;
    }
#endif

  return 0;
}

int ugl_get2(byte *data)
{
  int ret = 0;
  TIME clk;
  static byte *rBufPtr;

  tdr.count++;
#ifdef BUFFERED_W
  /* Flush write buffer */
  if(nBytesWrite > 0)
    {
      ret = usb_bulk_write(tigl_han, TIGL_BULK_OUT, wBuf, nBytesWrite, 
			   (time_out * 10));
      nBytesWrite = 0;
      if (ret <= 0)
	{
	  DISPLAY_ERROR("%s\n", usb_strerror());
	  return ERR_SND_BYT;
	}
    }
#endif

  if (nBytesRead <= 0)
    {
      toSTART(clk);
      do
	{
	  ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rBuf, 
			      MAX_PACKET_SIZE, (time_out * 10));
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
	  if(ret == 0)
	    DISPLAY_ERROR("usb_bulk_read returns without any data. Retrying...\n");
	}
      while(!ret);
      
      if (ret < 0)
	{
	  DISPLAY_ERROR("(ret = %d) %s\n", ret, usb_strerror());
	  nBytesRead = 0;
	  return ERR_RCV_BYT;
	}
      nBytesRead = ret;
      rBufPtr = rBuf;
    }

  *data = *rBufPtr++;
  nBytesRead--;
  LOG_DATA(*data);

  return 0;
}

int ugl_probe2()
{
  if (tigl_dev != NULL)
    return 0;
  else
    return ERR_PROBE_FAILED;
}

int ugl_check2(int *status)
{
  TIME clk;
  int ret = 0;

  /* Since the select function does not work, I do it myself ! */
  *status = STATUS_NONE;
  
  if(tigl_han != NULL)
    {
      if (nBytesRead > 0)
	{
	  *status = STATUS_RX; 
	  return 0;
	}
      
      toSTART(clk);
      do
	{
	  ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rBuf, 
			      MAX_PACKET_SIZE, (time_out * 10));
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
	  if(ret == 0)
	    DISPLAY_ERROR("usb_bulk_read returns without any data. Retrying...\n");
	}
      while(!ret);

      if(ret > 0)
	{
	  nBytesRead = ret;
	  *status = STATUS_RX;	  
	  return 0;
	}
      else
	{
	  nBytesRead = 0;
	  *status = STATUS_NONE;
	  return 0;
	}
    }

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire2(int b)
{
  return 0;
}

int ugl_set_white_wire2(int b)
{
  return 0;
}

int ugl_get_red_wire2()
{
  return 0;
}

int ugl_get_white_wire2()
{
  return 0;
}

int ugl_supported2() /* HELL YES IT'S SUPPORTED ! :-) */
{
  return SUPPORT_ON;
}

#else

int ugl_init2()
{
  return 0;
}

TIEXPORT
int ugl_open2()
{
  return 0;
}

TIEXPORT
int ugl_put2(byte data)
{
  return 0;
}

TIEXPORT
int ugl_get2(byte *d)
{
  return 0;
}

TIEXPORT
int ugl_probe2()
{
  return 0;
}

TIEXPORT
int ugl_close2()
{
  return 0;
}

TIEXPORT
int ugl_exit2()
{
  return 0;
}

TIEXPORT
int ugl_check2(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire2(int b)
{
  return 0;
}

int ugl_set_white_wire2(int b)
{
  return 0;
}

int ugl_get_red_wire2()
{
  return 0;
}

int ugl_get_white_wire2()
{
  return 0;
}

TIEXPORT
int ugl_supported2()
{
  return SUPPORT_OFF;
}

#endif

#elif defined(__WIN32__)


/************************/
/* Windows 32 bits part */
/************************/

/* 
   This part talk with the USB device driver through the TiglUsb library.
   There is a subdirectory called TiglUsb which contains files specific to 
   this link cable (definitions, library, routines, ...).
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

#include "./tiglusb/tiglusb.h"

//#define BUFFERED_W /* enable buffered write operations */ 
#define BUFFERED_R   /* enable buffered read operations */

static HANDLE hRead  = INVALID_HANDLE_VALUE;	// Named pipe (IN)
static HANDLE hWrite = INVALID_HANDLE_VALUE;	// Named pipe (OUT)

/* Buffered I/O operations */
static DWORD nBytesWrite = 0;
static byte wBuf[TIGLUSB_MAX_PACKET_SIZE];
static DWORD nBytesRead = 0;
static byte rBuf[TIGLUSB_MAX_PACKET_SIZE];

/* Function pointers for dynamic loading */
TIGLUSB_OPENFILE	dynTiglUsbOpenFile	= NULL;
TIGLUSB_OPENDEV		dynTiglUsbOpenDev	= NULL;
TIGLUSB_RESETPIPES	dynTiglUsbResetPipes	= NULL;
TIGLUSB_SETTIMEOUT	dynTiglUsbSetTimeout	= NULL;
static HINSTANCE hDLL = NULL;		// DLL handle

extern int time_out;		// Timeout value for cables in 0.10 seconds
static struct cs
{
  byte data;
  int available;
} cs;

TIEXPORT
int ugl_init()
{
	// Create an handle on library and retrieve symbols
	hDLL = LoadLibrary("TIGLUSB.DLL");
	if (hDLL == NULL)
	{
	  DISPLAY_ERROR("TiglUsb library not found. Have you installed the driver ?\n");
	  return ERR_USB_OPEN;
	}

	dynTiglUsbOpenFile = (TIGLUSB_OPENFILE)GetProcAddress(hDLL,
								    "open_file");
	if (!dynTiglUsbOpenFile)
	{
		DISPLAY_ERROR("Unable to load TiglUsbOpenFile symbol.\n");
	    FreeLibrary(hDLL);       
	    return ERR_FREELIBRARY;
	}

	dynTiglUsbOpenDev = (TIGLUSB_OPENDEV)GetProcAddress(hDLL,
								    "open_dev");
	if (!dynTiglUsbOpenDev)
	{
		DISPLAY_ERROR("Unable to load TiglUsbOpenDev symbol.\n");
	    FreeLibrary(hDLL);       
	    return ERR_FREELIBRARY;
	}

	dynTiglUsbResetPipes = (TIGLUSB_RESETPIPES)GetProcAddress(hDLL,
								    "resetPipes");
	if (!dynTiglUsbResetPipes)
	{
		DISPLAY_ERROR("Unable to load TiglUsbOpenFile symbol.\n");
	    FreeLibrary(hDLL);       
	    return ERR_FREELIBRARY;
	}

	dynTiglUsbSetTimeout = (TIGLUSB_SETTIMEOUT)GetProcAddress(hDLL,
								    "setTimeout");
	if (!dynTiglUsbSetTimeout)
	{
		DISPLAY_ERROR("Unable to load TiglUsbSetTimeout symbol.\n");
	    FreeLibrary(hDLL);       
	    return ERR_FREELIBRARY;
	}

	// Init some internal variables
	memset((void *)(&cs), 0, sizeof(cs));

	// Open the USB device: 2 named pipes (endpoints)
	hWrite = dynTiglUsbOpenFile(OUT_PIPE_0);
	if(hWrite == INVALID_HANDLE_VALUE)
	{
		print_last_error();
		return ERR_USB_OPEN;
	}
	hRead = dynTiglUsbOpenFile(IN_PIPE_0);
	if(hRead == INVALID_HANDLE_VALUE)
	{
		print_last_error();
		return ERR_USB_OPEN;
	}
	
	// Setsup timeout
	dynTiglUsbSetTimeout(time_out);

	START_LOGGING();
	
	return 0;
}

TIEXPORT
int ugl_open()
{
	/* Clear buffers */
	dynTiglUsbResetPipes();

	nBytesRead = 0;
	nBytesWrite = 0;

	tdr.count = 0;
	toSTART(tdr.start);

	return 0;
}

TIEXPORT
int ugl_put(byte data)
{
	BOOL fSuccess;
	int nBytesWritten;
	TIME clk;

	tdr.count++;
	LOG_DATA(data);
#ifndef BUFFERED_W
	fSuccess=WriteFile(hWrite, &data, 1, &nBytesWritten, NULL);
	if(!fSuccess)
	{
		print_last_error();
		return ERR_SND_BYT;
	}
	else if(nBytesWritten == 0)
	{
		return ERR_SND_BYT_TIMEOUT;
	}
#else
	/* Write data by packets (up to 32 bytes) */
	wBuf[nBytesWrite++] = data;
	if(nBytesWrite == TIGLUSB_MAX_PACKET_SIZE)
	{
		fSuccess=WriteFile(hWrite, &wBuf, nBytesWrite, &nBytesWritten, NULL);
		nBytesWrite = 0;
	}
#endif

	return 0;
}

TIEXPORT
int ugl_get(byte *data)
{
	BOOL fSuccess;
	TIME clk;
	static byte *rBufPtr;
	int j;
	int nBytesWritten;

	tdr.count++;
#ifdef BUFFERED_W
	/* Flush write buffer */
	fSuccess=WriteFile(hWrite, &wBuf, nBytesWrite, &nBytesWritten, NULL);
	nBytesWrite = 0;
#endif

	/* This routine try to read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and store them
	in a buffer for subsequent accesses */
	if(nBytesRead == 0)
	{
		toSTART(clk);
		do	// it seems that ReadFile sometimes returns with no data...
		{
			fSuccess = ReadFile(hRead, rBuf, TIGLUSB_MAX_PACKET_SIZE, &nBytesRead, NULL);
			if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
		}
		while(!nBytesRead);
		rBufPtr = rBuf;
	}
	
	*data = *rBufPtr++;
	nBytesRead--;

	LOG_DATA(*data);

	return 0;
}

TIEXPORT
int ugl_close()
{
	return 0;
}

TIEXPORT
int ugl_exit()
{	
  STOP_LOGGING();
	/* Close pipes if needed */
	if(hWrite != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hWrite);
		hWrite = INVALID_HANDLE_VALUE;
	}

	if(hRead != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hRead);
		hRead = INVALID_HANDLE_VALUE;
	}	

	/* Free library handle */
	if(hDLL != NULL)
		FreeLibrary(hDLL);	
	hDLL = NULL;

	return 0;
}

TIEXPORT
int ugl_probe()
{
	HANDLE hDev = dynTiglUsbOpenDev();

	if(hDev == INVALID_HANDLE_VALUE)
	{
		return ERR_PROBE_FAILED;
	}
	else
	{
		CloseHandle(hDev);
		return 0;
	}

  return 0;
}

TIEXPORT
int ugl_check(int *status)
{
	// TO DO...
	int n = 0;
	DWORD i;
	BOOL fSuccess;

	*status = STATUS_NONE;
	if(hRead)
    {
	    // Read the data: return 0 if error and i contains 1 or 0 (timeout)
		fSuccess = ReadFile(hRead, (&cs.data), 1, &i, NULL);
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

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire(int b)
{
  return 0;
}

int ugl_set_white_wire(int b)
{
  return 0;
}

int ugl_get_red_wire()
{
  return 0;
}

int ugl_get_white_wire()
{
  return 0;
}

TIEXPORT
int ugl_supported()
{
  return SUPPORT_ON;
}

/**********/
/* Unused */ 
/**********/

TIEXPORT
int ugl_init2()
{
  return 0;
}

TIEXPORT
int ugl_open2()
{
  return 0;
}

TIEXPORT
int ugl_put2(byte data)
{
  return 0;
}

TIEXPORT
int ugl_get2(byte *d)
{
  return 0;
}

TIEXPORT
int ugl_probe2()
{
  return 0;
}

TIEXPORT
int ugl_close2()
{
  return 0;
}

TIEXPORT
int ugl_exit2()
{
  return 0;
}

TIEXPORT
int ugl_check2(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire2(int b)
{
  return 0;
}

int ugl_set_white_wire2(int b)
{
  return 0;
}

int ugl_get_red_wire2()
{
  return 0;
}

int ugl_get_white_wire2()
{
  return 0;
}

TIEXPORT
int ugl_supported2()
{
  return SUPPORT_OFF;
}

#elif defined(__MACOSX__)

/*
 * TIGL USB driver for Mac OS X
 * Released under the LGPL
 * Author : Julien BLACHE <jb@technologeek.org>
 *
 * This code is derived from Apple Sample Code (USBNotificationExample)
 * Apple is not liable for anything regarding this code, according to
 * the Apple Sample Code License.
 */

#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

#include "timeout.h"
#include "cabl_ext.h"

#define kTIGLVendorID		0x0451  // Texas Instruments Inc.
#define kTIGLProductID		0xe001  // TI GraphLink USB
#define kTIGLKnownVersion	0x0103  // Last known version

#define TIGL_BULK_ENDPOINT_OUT 2
#define TIGL_BULK_ENDPOINT_IN 1
#define TIGL_MAXPACKETSIZE 32

// uncomment to add some tests
//#define OSX_UGL_DEBUG

#define IOKIT_ERROR(error)	DISPLAY_ERROR("IOKit Error : system 0x%x, subsystem 0x%x, code 0x%x\n", \
                                                              err_get_system(error), \
                                                              err_get_sub(error), \
                                                              err_get_code(error))

// globals

IOUSBDeviceInterface **dev = NULL;
IOUSBInterfaceInterface182 **intf = NULL; // interface version 1.8.2

static char rcv_buffer[TIGL_MAXPACKETSIZE + 1];
static UInt32 numBytesRead = 0;
static byte *rcv_buf_ptr;

// specific functions

IOReturn FindInterfaces(IOUSBDeviceInterface **dev)
{
    IOReturn			kr;
    IOUSBFindInterfaceRequest	request;
    io_iterator_t		iterator;
    io_service_t		usbInterface;
    IOCFPlugInInterface 	**plugInInterface = NULL;
    HRESULT 			res;
    SInt32 			score;
    UInt8			intfClass;
    UInt8			intfSubClass;
    UInt8			intfNumEndpoints;

#ifdef OSX_UGL_DEBUG
#warning OSX_UGL_DEBUG defined !
    UInt32			numBytesRead;
    UInt32			i;    
    char			test[4];
    static char			gBuffer[33];
        
    // this will display a "A" on the calc
    test[0] = 0x08;
    test[1] = 0x87;
    test[2] = 'A';
    test[3] = 0;
#endif /* OSX_UGL_DEBUG */

    request.bInterfaceClass = 255;  // proprietary device
    request.bInterfaceSubClass = 0;
    request.bInterfaceProtocol = 0;
    request.bAlternateSetting = 0;
   
    kr = (*dev)->CreateInterfaceIterator(dev, &request, &iterator);
    
    usbInterface = IOIteratorNext(iterator);

#ifdef OSX_DEBUG
    DISPLAY("Interface found.\n");
#endif
    
    kr = IOCreatePlugInInterfaceForService(usbInterface,
                                           kIOUSBInterfaceUserClientTypeID,
                                           kIOCFPlugInInterfaceID,
                                           &plugInInterface,
                                           &score);
    kr = IOObjectRelease(usbInterface);	// done with the usbInterface object now that I have the plugin
    
    if ((kIOReturnSuccess != kr) || !plugInInterface)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("unable to create a plugin (%08x)\n", kr);
#endif
            return kr;
        }
            
    // I have the interface plugin. I need the interface interface
    res = (*plugInInterface)->QueryInterface(plugInInterface,
                                             CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID),
                                             (LPVOID) &intf);
    (*plugInInterface)->Release(plugInInterface); // done with this
    
    if (res || !intf)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("couldn't create an IOUSBInterfaceInterface (%08x)\n", (int) res);
#endif
            return kr;
        }
        
    kr = (*intf)->GetInterfaceClass(intf, &intfClass);
    kr = (*intf)->GetInterfaceSubClass(intf, &intfSubClass);

#ifdef OSX_DEBUG
    DISPLAY("Interface class %d, subclass %d\n", intfClass, intfSubClass);
#endif

    // Now open the interface. This will cause the pipes to be instantiated that are 
    // associated with the endpoints defined in the interface descriptor.
    kr = (*intf)->USBInterfaceOpen(intf);
    
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("unable to open interface (%08x)\n", kr);
#endif
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    kr = (*intf)->GetNumEndpoints(intf, &intfNumEndpoints);
    
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("unable to get number of endpoints (%08x)\n", kr);
#endif
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }

#ifdef OSX_DEBUG
    DISPLAY("Interface has %d endpoints.\n", intfNumEndpoints);
#endif

#ifdef OSX_UGL_DEBUG    
    // We can now address endpoints 1 through intfNumEndpoints. Or, we can also address endpoint 0,
    // the default control endpoint. But it's usually better to use (*usbDevice)->DeviceRequest() instead.

    kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, test, 4); // endpoint 2
    
    if (kIOReturnSuccess != kr)
        {
            DISPLAY_ERROR("unable to do bulk write (%08x)\n", kr);
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    DISPLAY("Wrote");
    for (i = 0; i < 4; i++)
        {
            DISPLAY(" 0x%x", test[i]);
        }
          
    DISPLAY(" (4 bytes) to bulk endpoint\n");
    
    numBytesRead = sizeof(gBuffer) - 1; // leave one byte at the end for NUL termination
    
    kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, gBuffer, &numBytesRead); // endpoint 1
    
    if (kIOReturnSuccess != kr)
        {
            DISPLAY_ERROR("unable to do bulk read (%08x)\n", kr);
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }

    DISPLAY("Calc reply :");
    for (i = 0; i < 4; i++)  // 4 bytes only
        {
            DISPLAY(" 0x%x", gBuffer[i]);
        }
    DISPLAY("\n");
#endif /* OSX_UGL_DEBUG */

    return kr;
}

IOReturn ConfigureTIGL(IOUSBDeviceInterface **dev)
{
    UInt8				numConf;
    IOReturn				kr;
    IOUSBConfigurationDescriptorPtr	confDesc;
    
    kr = (*dev)->GetNumberOfConfigurations(dev, &numConf);
    
    if (!numConf)
        return -1;
    
    // get the configuration descriptor for index 0
    kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &confDesc);
    
    if (kr)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("\tunable to get config descriptor for index %d (err = %08x)\n", 0, kr);
#endif
            return -1;
        }
    kr = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);
    
    if (kr)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("\tunable to set configuration to value %d (err=%08x)\n", 0, kr);
#endif
            return -1;
        }
    
    return kIOReturnSuccess;
}

void tiglusbFindDevice(io_iterator_t iterator)
{
    kern_return_t		kr;
    io_service_t		usbDevice;
    IOCFPlugInInterface 	**plugInInterface=NULL;
    HRESULT 			res;
    SInt32 			score;
    UInt16			vendor;
    UInt16			product;
    UInt16			release;
    
    while ((usbDevice = IOIteratorNext(iterator)))
        {
#ifdef OSX_DEBUG
            DISPLAY("TIGL added.\n");
#endif
            kr = IOCreatePlugInInterfaceForService(usbDevice,
                                                   kIOUSBDeviceUserClientTypeID,
                                                   kIOCFPlugInInterfaceID,
                                                   &plugInInterface,
                                                   &score);
            kr = IOObjectRelease(usbDevice);	// done with the device object now that I have the plugin
            if ((kIOReturnSuccess != kr) || !plugInInterface)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("unable to create a plugin (%08x)\n", kr);
#endif
                    continue;
                }
            
            // I have the device plugin, I need the device interface
            res = (*plugInInterface)->QueryInterface(plugInInterface,
                                                     CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
                                                     (LPVOID)&dev);
            (*plugInInterface)->Release(plugInInterface); // done with this
            
            if (res || !dev)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("couldn't create a device interface (%08x)\n", (int) res);
#endif
                    continue;
                }
            // technically should check these kr values
            kr = (*dev)->GetDeviceVendor(dev, &vendor);
            kr = (*dev)->GetDeviceProduct(dev, &product);
            kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
        
            if ((vendor != kTIGLVendorID) || (product != kTIGLProductID))
                {
#ifdef OSX_DEBUG
                    DISPLAY("Found a device I didn't want (vendor = 0x%x, product = 0x%x, version = Ox%x)\n", vendor, product, release);
#endif
                    continue;
                }
            else if ((vendor == kTIGLVendorID) && (product == kTIGLProductID) && (release > kTIGLKnownVersion))
                {
                    // we'll get here if TI releases a new version of the cable
                    
                    DISPLAY_ERROR("Found TIGL USB : vendor = 0x%x, product = 0x%x, version = Ox%x)\n", vendor, product, release);
                    DISPLAY_ERROR("This version of the TIGL USB has not been tested.\n");
                    DISPLAY_ERROR("Contact <jb@technologeek.org> about this\n");
                    
                    (*dev)->Release(dev);
                    
                    continue;
                }

            // need to open the device in order to change its state
            kr = (*dev)->USBDeviceOpen(dev);
            
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("unable to open device: %08x\n", kr);
#endif
                    (*dev)->Release(dev);
                    continue;
                }
        
            kr = ConfigureTIGL(dev);
            
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("unable to configure device: %08x\n", kr);
#endif
                    (*dev)->USBDeviceClose(dev);
                    (*dev)->Release(dev);
                    continue;
                }

            kr = FindInterfaces(dev);
            
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("unable to find interfaces on device: %08x\n", kr);
#endif
                    (*dev)->USBDeviceClose(dev);
                    (*dev)->Release(dev);
                    continue;
                }
        }
}



/***************************
 * libticables begins here *
 ***************************/

int ugl_init()
{
    mach_port_t 		masterPort;
    CFMutableDictionaryRef 	matchingDict;
    kern_return_t		kr;
    SInt32			usbVendor = kTIGLVendorID;
    SInt32			usbProduct = kTIGLProductID;
    io_iterator_t		iterator;
  
    // first create a master_port for my task
    kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    
    if (kr || !masterPort)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
#endif
            return ERR_USB_INIT;
        }

#ifdef OSX_DEBUG
    DISPLAY("Looking for devices matching vendor ID=0x%x and product ID=0x%x\n", (int)usbVendor, (int)usbProduct);
#endif

    // Set up the matching criteria for the devices we're interested in
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);	// Interested in instances of class IOUSBDevice and its subclasses
    if (!matchingDict)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("Can't create a USB matching dictionary\n");
#endif
            mach_port_deallocate(mach_task_self(), masterPort);
            return ERR_USB_INIT;
        }
    
    // Add our vendor and product IDs to the matching criteria
    CFDictionarySetValue(matchingDict, 
                         CFSTR(kUSBVendorID), 
                         CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor)); 
    CFDictionarySetValue(matchingDict, 
                         CFSTR(kUSBProductID), 
                         CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct)); 

    // get the matching devices list
    kr = IOServiceGetMatchingServices(masterPort, matchingDict, &iterator);

    // Now done with the masterPort
    mach_port_deallocate(mach_task_self(), masterPort);
    masterPort = 0;

    // check for the return from IOServiceGetMatchingServices()
    if (iterator == NULL)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("No matching devices !\n");
#endif
            return ERR_USB_INIT;
        }
    
    tiglusbFindDevice(iterator);
    
    IOObjectRelease(iterator);

    return 0;
}

int ugl_open()
{
    // if already open, then close...
    numBytesRead = 0;
    rcv_buf_ptr = rcv_buffer;
        
    if (dev == NULL)
        {
            if (ugl_init() != 0)
                return ERR_USB_INIT;
        }
        
    if (intf == NULL)
        {
            if (FindInterfaces(dev) != kIOReturnSuccess)
                return ERR_USB_OPEN;
        }
        
    tdr.count = 0;
    toSTART(tdr.start);

    return 0;
}

int ugl_probe()
{
  return 0;
}

int ugl_close()
{
  return 0;
}

int ugl_exit()
{
    if (intf != NULL)
        {
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            intf = NULL;
        }

    numBytesRead = 0;
    rcv_buf_ptr = rcv_buffer;
        
    if (dev != NULL)
        {
            (*dev)->Release(dev);
            dev = NULL;
        }

  return 0;
}

int ugl_put(byte data)
{
    IOReturn	kr;
    
    if (intf == NULL)
        return ERR_SND_BYT;

    kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, &data, 1);
                
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("Unable to do bulk write (%08x)\n", kr);
            
            IOKIT_ERROR(kr);
#endif
            ugl_close();
            return ERR_SND_BYT;
        }
        
#ifdef OSX_DEBUG
    DISPLAY("Wrote 0x%x to bulk endpoint %d\n", data, TIGL_BULK_ENDPOINT_OUT);
#endif

    tdr.count++;

    return 0;
}

int ugl_get(byte *d)
{
    extern int time_out;

    IOReturn	kr;

    TIME clk;
    
#ifdef OSX_DEBUG
    int		i;

    DISPLAY_ERROR("IN UGL_GET\n");
#endif

    if (intf == NULL)
        return ERR_RCV_BYT;

    if (numBytesRead <= 0) // we're at the end of the buffer
        {
#ifdef OSX_DEBUG
            DISPLAY_ERROR("rcv_buffer empty, reading bulk pipe...\n");
#endif

            usleep(100);

            memset(rcv_buffer, 0, TIGL_MAXPACKETSIZE + 1);

            toSTART(clk);

            do {
                    // Use ReadPipe(), not ReadPipeTO() (trouble with FLASHing)
                    // the do { } while () will handle the special case
                    // where a ReadPipeTO() returns BEFORE the timeout
                    // with NO DATA and NO ERROR
                    
                    numBytesRead = TIGL_MAXPACKETSIZE;
                    
                    kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, rcv_buffer, &numBytesRead);
                    
                    if (kIOReturnSuccess == kr)
                        {
                            if (numBytesRead > 0) // regardless of the timeout, we HAVE DATA !
                                break;
                            else if ((numBytesRead == 0) && !(toELAPSED(clk, time_out)))
                                DISPLAY_ERROR("ReadPipeTO returned before timeout with no data. Retrying...\n");
                            else if ((numBytesRead == 0) && (toELAPSED(clk, time_out)))
                                return ERR_RCV_BYT_TIMEOUT;
                        }
                    else // There was an error, let's see what happened
                        break;
                }
            while(numBytesRead == 0);

    
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    DISPLAY_ERROR("Unable to do bulk read (0x%x)\n", kr);
                    
                    IOKIT_ERROR(kr);
#endif
                    numBytesRead = 0;
                    
                    ugl_close();
                    
                    return ERR_RCV_BYT;
                }
                
            rcv_buf_ptr = rcv_buffer;
            
#ifdef OSX_DEBUG
            DISPLAY("numBytesRead : %ld\n", numBytesRead);

            DISPLAY("Buffer content:");

            for (i = 0; i < numBytesRead; i++)
                {
                    DISPLAY(" 0x%x", rcv_buffer[i]);
                }
            DISPLAY("\n");
#endif
        }
#ifdef OSX_DEBUG
    DISPLAY("Calc reply : 0x%x on bulk endpoint %d\n", rcv_buf_ptr[0], TIGL_BULK_ENDPOINT_IN);
#endif

    tdr.count++;

    *d = *rcv_buf_ptr++;
    numBytesRead--;
 
    return 0;
}

int ugl_check(int *status)
{
    extern int time_out;

    IOReturn	kr;
    
    UInt32 timeout = 100 * time_out; // time_out is specified in tenth of seconds
    
    TIME clk;

    // we cannot use select() nor poll()
    // so...
    
    *status = STATUS_NONE;
    if(intf != NULL)
        {
            if (numBytesRead > 0)
                {
                    *status = STATUS_RX;
                    
                    return 0;
                }
        
            numBytesRead = TIGL_MAXPACKETSIZE;
            
            toSTART(clk);
            
            do {
                    // Use ReadPipeTO(), it handles the timeout itself
                    // the do { } while () will handle the special case
                    // where a ReadPipeTO() returns BEFORE the timeout
                    // with NO DATA and NO ERROR
                    kr = (*intf)->ReadPipeTO(intf, TIGL_BULK_ENDPOINT_IN, rcv_buffer, &numBytesRead, timeout, timeout);
                    
                    if (kIOReturnSuccess == kr)
                        {
                            if (numBytesRead > 0) // regardless of the timeout, we HAVE DATA !
                                break;
                            else if ((numBytesRead == 0) && !(toELAPSED(clk, time_out)))
                                DISPLAY_ERROR("ReadPipeTO returned before timeout with no data. Retrying...\n");
                            else if ((numBytesRead == 0) && (toELAPSED(clk, time_out)))
                                return ERR_RCV_BYT_TIMEOUT;
                        }
                    else // There was an error, let's see what happened
                        break;
                }
            while(numBytesRead == 0);
            
            if(kr == kIOReturnSuccess)
                {
#ifdef OSX_DEBUG
		  DISPLAY("In ugl_check: numBytesRead = %ld\n", numBytesRead);
#endif
                    rcv_buf_ptr = rcv_buffer;
                            
                    *status = STATUS_RX;

                    return 0;
                }
            else
                {
                    numBytesRead = 0;
                    *status = STATUS_NONE;
                    return 0;
                }
        }

    return 0;
}

// we can't drive it at low-level, so these functions are unused

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire(int b)
{
    return 0;
}

int ugl_set_white_wire(int b)
{
    return 0;
}

int ugl_get_red_wire()
{
    return 0;
}

int ugl_get_white_wire()
{
    return 0;
}

int ugl_supported()
{
    return SUPPORT_ON;
}

#else // unsupported platform

/************************/
/* Unsupported platform */
/************************/

TIEXPORT
int ugl_init()
{
  return 0;
}

TIEXPORT
int ugl_open()
{
  return 0;
}

TIEXPORT
int ugl_put(byte data)
{
  return 0;
}

TIEXPORT
int ugl_get(byte *d)
{
  return 0;
}

TIEXPORT
int ugl_probe()
{
  return 0;
}

TIEXPORT
int ugl_close()
{
  return 0;
}

TIEXPORT
int ugl_exit()
{
  return 0;
}

TIEXPORT
int ugl_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire(int b)
{
  return 0;
}

int ugl_set_white_wire(int b)
{
  return 0;
}

int ugl_get_red_wire()
{
  return 0;
}

int ugl_get_white_wire()
{
  return 0;
}

TIEXPORT
int ugl_supported()
{
  return SUPPORT_OFF;
}

TIEXPORT
int ugl_init2()
{
  return 0;
}

TIEXPORT
int ugl_open2()
{
  return 0;
}

TIEXPORT
int ugl_put2(byte data)
{
  return 0;
}

TIEXPORT
int ugl_get2(byte *d)
{
  return 0;
}

TIEXPORT
int ugl_probe2()
{
  return 0;
}

TIEXPORT
int ugl_close2()
{
  return 0;
}

TIEXPORT
int ugl_exit2()
{
  return 0;
}

TIEXPORT
int ugl_check2(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ugl_set_red_wire2(int b)
{
  return 0;
}

int ugl_set_white_wire2(int b)
{
  return 0;
}

int ugl_get_red_wire2()
{
  return 0;
}

int ugl_get_white_wire2()
{
  return 0;
}

TIEXPORT
int ugl_supported2()
{
  return SUPPORT_OFF;
}

#endif
