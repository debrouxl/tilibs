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

/* TI-GRAPH LINK USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intl.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "verbose.h"
#include "logging.h"
#include "externs.h"
#include "timeout.h"

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   
   This link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
   This is transparent for the user because the driver manages all these 
   things for us. Nethertheless, this fact has some consequences:
   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.
   - for reading, we have to read up to 32 bytes at a time (even if we need 
   only 1 byte) and to store them in a buffer for subsequent acesses. 
   In fact, if we try and get byte per byte, it will not work.
   - for writing, we don't store bytes in a buffer. It seems better to send
   data byte per byte (latency ?!).
   - another particular effect (quirk): sometimes (usually when calc need to 
   reply and takes a while), a read call can returns with no data or timeout. 
   Simply retry a read call and it works fine.
*/


#if defined(__LINUX__) || defined(__BSD__)
// for linux kernel support and libusb support
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

//#define BUFFERED_W /* enable buffered write operations */ 
#define BUFFERED_R		/* enable buffered read operations (default) */

#define MAX_PACKET_SIZE 32	// 32 bytes max per packet
static int nBytesWrite = 0;
#ifdef BUFFERED_W
static uint8_t wBuf[MAX_PACKET_SIZE];
#endif
static int nBytesRead = 0;
static uint8_t rBuf[MAX_PACKET_SIZE];
#endif


/*********************************/
/* Linux : kernel module support */
/*********************************/


#if defined(__LINUX__)

static int dev_fd = 0;

#ifdef HAVE_TILP_TICABLE_H
# include <tilp/ticable.h>	//ioctl codes
# include <sys/ioctl.h>
#endif

int slv_init()
{
  int mask = O_RDWR | O_NONBLOCK | O_SYNC;

  if ((dev_fd = open(io_device, mask)) == -1) {
    DISPLAY(_("Unable to open this device: %s\n"), io_device);
    DISPLAY(_("Is the tiusb.c module loaded ?\n"));
    return ERR_OPEN_USB_DEV;
  }

  START_LOGGING();

  return 0;
}

int slv_open(void)
{
  /* Clear buffers */
  nBytesRead = 0;
  nBytesWrite = 0;

#ifdef HAVE_TILP_TICABLE_H
  {
    int arg = time_out;
    if (ioctl(dev_fd, IOCTL_TIUSB_TIMEOUT, arg) == -1) {
      DISPLAY_ERROR(_("Unable to use IOCTL codes.\n"));
      return ERR_IOCTL;
    }
  }
#endif

  /* Reset both endpoints */
#ifdef HAVE_TILP_TICABLE_H
  {
    int arg = 0;
    if (ioctl(dev_fd, IOCTL_TIUSB_RESET_PIPES, arg) == -1) {
      DISPLAY(_("Unable to use IOCTL codes.\n"));
      //return ERR_IOCTL;
    }
  }
#endif
  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int slv_put(uint8_t data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);
#ifndef BUFFERED_W
  /* Byte per uint8_t */
  err = write(dev_fd, (void *) (&data), 1);
  switch (err) {
  case -1:			//error
    return ERR_WRITE_ERROR;
    break;
  case 0:			// timeout
    return ERR_WRITE_TIMEOUT;
    break;
  }
#else
  /* Packets (up to 32 bytes) */
  wBuf[nBytesWrite++] = data;
  if (nBytesWrite == MAX_PACKET_SIZE) {
    err = write(dev_fd, (void *) (&wBuf), nBytesWrite);
    nBytesWrite = 0;

    switch (err) {
    case -1:			//error
      return ERR_WRITE_ERROR;
      break;
    case 0:			// timeout
      return ERR_WRITE_TIMEOUT;
      break;
    }
  }
#endif

  return 0;
}

int slv_get(uint8_t * data)
{
  tiTIME clk;
  static uint8_t *rBufPtr;
  int ret;

#ifdef BUFFERED_W
  /* Flush write buffer */
  if (nBytesWrite > 0) {
    ret = write(dev_fd, (void *) (&wBuf), nBytesWrite);
    nBytesWrite = 0;
    switch (err) {
    case -1:			//error
      return ERR_READ_ERROR;
      break;
    case 0:			// timeout
      return ERR_READ_TIMEOUT;
      break;
    }
  }
#endif

#ifdef BUFFERED_R
  /* This routine try to read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and 
     store them in a buffer for subsequent accesses */
  if (nBytesRead == 0) {
    toSTART(clk);
    do {
      ret = read(dev_fd, (void *) rBuf, MAX_PACKET_SIZE);
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
      if (ret == 0)
	DISPLAY_ERROR
	    (_
	     ("read returns without any data. Retrying for circumventing quirk...\n"));
    }
    while (!ret);

    if (ret < 0) {
      nBytesRead = 0;
      return ERR_READ_ERROR;
    }
    nBytesRead = ret;
    rBufPtr = rBuf;
  }

  *data = *rBufPtr++;
  nBytesRead--;
#else
  nBytesRead = read(dev_fd, (void *) data, 1);
  if (nBytesRead == -1)
    return ERR_READ_ERROR;
  if (nBytesRead == 0)
    return ERR_READ_TIMEOUT;
#endif

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int slv_probe(void)
{
  return 0;
}

int slv_close(void)
{
  return 0;
}

int slv_exit()
{
  if (dev_fd) {
    close(dev_fd);
    dev_fd = 0;
  }

  STOP_LOGGING();

  return 0;
}

int slv_check(int *status)
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

int slv_supported()
{
  return SUPPORT_ON;
}


#elif defined(__WIN32__)


/************************/
/* Windows 32 bits part */
/************************/

/* 
   This part talk with the USB device driver through the TiglUsb library.
   There are 2 other files: TiglUsb.h (interface) & TiglUsb.lib (linkage).
*/

#include <stdio.h>
#include <windows.h>

#include "tiglusb.h"

#define MIN_VERSION "2.2"

static HINSTANCE hDLL = NULL;	// DLL handle on TiglUsb.dll
static dllOk = FALSE;		// Dll loading is OK

TIGLUSB_VERSION dynTiglUsbVersion = NULL;	// Functions pointers for 
TIGLUSB_OPEN dynTiglUsbOpen = NULL;	// dynamic loading
TIGLUSB_FLUSH dynTiglUsbFlush = NULL;
TIGLUSB_READ dynTiglUsbRead = NULL;
TIGLUSB_WRITE dynTiglUsbWrite = NULL;
TIGLUSB_CLOSE dynTiglUsbClose = NULL;
TIGLUSB_SETTIMEOUT dynTiglUsbSetTimeout = NULL;
TIGLUSB_CHECK dynTiglUsbCheck = NULL;

extern int time_out;		// Timeout value for cables in 0.10 seconds

int slv_init()
{
  int ret;

  // Create an handle on library and retrieve symbols
  hDLL = LoadLibrary("TIGLUSB.DLL");
  if (hDLL == NULL) {
    DISPLAY_ERROR
	(_
	 ("TiglUsb library not found. Have you installed the TiglUsb driver ?\n"));
    return ERR_OPEN_USB_DEV;
  }

  dynTiglUsbVersion =
      (TIGLUSB_VERSION) GetProcAddress(hDLL, "TiglUsbVersion");
  if (!dynTiglUsbVersion || (strcmp(dynTiglUsbVersion(), MIN_VERSION) < 0)) {
    char buffer[256];
    sprintf(buffer,
	    _
	    ("TiglUsb.dll: version %s mini needed, got version %s.\nPlease download the latest release on <http://ti-lpg.org/prj_usb>."),
	    MIN_VERSION, dynTiglUsbVersion());
    DISPLAY_ERROR(buffer);
    MessageBox(NULL, buffer, "Error in SilverLink support", MB_OK);
    FreeLibrary(hDLL);
    return ERR_TIGLUSB_VERSION;
  }

  dynTiglUsbOpen = (TIGLUSB_OPEN) GetProcAddress(hDLL, "TiglUsbOpen");
  if (!dynTiglUsbOpen) {
    DISPLAY_ERROR(_("Unable to load TiglUsbOpen symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbFlush = (TIGLUSB_FLUSH) GetProcAddress(hDLL, "TiglUsbFlush");
  if (!dynTiglUsbOpen) {
    DISPLAY_ERROR(_("Unable to load TiglUsbFlush symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbRead = (TIGLUSB_READ) GetProcAddress(hDLL, "TiglUsbRead");
  if (!dynTiglUsbRead) {
    DISPLAY_ERROR(_("Unable to load TiglUsbRead symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbWrite = (TIGLUSB_WRITE) GetProcAddress(hDLL, "TiglUsbWrite");
  if (!dynTiglUsbWrite) {
    DISPLAY_ERROR(_("Unable to load TiglUsbWrite symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbClose = (TIGLUSB_CLOSE) GetProcAddress(hDLL, "TiglUsbClose");
  if (!dynTiglUsbClose) {
    DISPLAY_ERROR(_("Unable to load TiglUsbClose symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbSetTimeout = (TIGLUSB_SETTIMEOUT) GetProcAddress(hDLL,
							     "TiglUsbSetTimeout");
  if (!dynTiglUsbSetTimeout) {
    DISPLAY_ERROR(_("Unable to load TiglUsbSetTimeout symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dynTiglUsbCheck = (TIGLUSB_CHECK) GetProcAddress(hDLL, "TiglUsbCheck");
  if (!dynTiglUsbCheck) {
    DISPLAY_ERROR(_("Unable to load TiglUsbCheck symbol.\n"));
    FreeLibrary(hDLL);
    return ERR_FREELIBRARY;
  }

  dllOk = TRUE;

  ret = dynTiglUsbOpen();
  switch (ret) {
  case TIGLERR_DEV_OPEN_FAILED:
    return ERR_OPEN_USB_DEV;
  case TIGLERR_DEV_ALREADY_OPEN:
    return ERR_OPEN_USB_DEV;
  default:
    break;
  }

  dynTiglUsbSetTimeout(time_out);

  START_LOGGING();

  return 0;
}

int slv_open()
{
  int ret;

  if (!hDLL)
    ERR_TIGLUSB_VERSION;

  dynTiglUsbSetTimeout(time_out);
  ret = dynTiglUsbFlush();
  switch (ret) {
  case TIGLERR_FLUSH_FAILED:
    return ERR_IOCTL;
  default:
    break;
  }

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int slv_put(uint8_t data)
{
  int ret;

  tdr.count++;
  LOG_DATA(data);

  ret = dynTiglUsbWrite(data);
  switch (ret) {
  case TIGLERR_WRITE_TIMEOUT:
    return ERR_WRITE_TIMEOUT;
  default:
    break;
  }

  return 0;
}

int slv_get(uint8_t * data)
{
  int ret;

  ret = dynTiglUsbRead(data);
  switch (ret) {
  case TIGLERR_READ_TIMEOUT:
    return ERR_READ_TIMEOUT;
  default:
    break;
  }

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int slv_close()
{
  return 0;
}

int slv_exit()
{
  int ret;

  STOP_LOGGING();

  ret = dynTiglUsbClose();

  /* Free library handle */
  if (hDLL != NULL)
    FreeLibrary(hDLL);
  hDLL = NULL;

  dllOk = FALSE;

  return 0;
}

int slv_probe()
{
  /*
     HANDLE hDev = dynTiglUsbOpen();

     if(hDev == INVALID_HANDLE_VALUE)
     {
     return ERR_PROBE_FAILED;
     }
     else
     {
     CloseHandle(hDev);
     return 0;
     }
   */

  return 0;
}

int slv_check(int *status)
{
  return dynTiglUsbCheck(status);
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire(int b)
{
  return 0;
}

int slv_set_white_wire(int b)
{
  return 0;
}

int slv_get_red_wire()
{
  return 0;
}

int slv_get_white_wire()
{
  return 0;
}

int slv_supported()
{
  return SUPPORT_ON;
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
#include <mach/mach.h>

#define kTIGLVendorID		0x0451	// Texas Instruments Inc.
#define kTIGLProductID		0xe001	// TI GraphLink USB
#define kTIGLKnownVersion	0x0103	// Last known version

#define TIGL_BULK_ENDPOINT_OUT 2
#define TIGL_BULK_ENDPOINT_IN 1
#define TIGL_MAXPACKETSIZE 32

// uncomment to add some tests
//#define OSX_UGL_DEBUG
//#define OSX_DEBUG

#define IOKIT_ERROR(error)	DISPLAY_ERROR("IOKit Error : system 0x%x, subsystem 0x%x, code 0x%x\n", \
                                                              err_get_system(error), \
                                                              err_get_sub(error), \
                                                              err_get_code(error))

// globals

IOUSBDeviceInterface **dev = NULL;
IOUSBInterfaceInterface182 **intf = NULL;	// interface version 1.8.2

static char rcv_buffer[TIGL_MAXPACKETSIZE + 1];
static UInt32 numBytesRead = 0;
static uint8_t *rcv_buf_ptr;

// specific functions

IOReturn FindInterfaces(IOUSBDeviceInterface ** dev)
{
  IOReturn kr;
  IOUSBFindInterfaceRequest request;
  io_iterator_t iterator;
  io_service_t usbInterface;
  IOCFPlugInInterface **plugInInterface = NULL;
  HRESULT res;
  SInt32 score;
  UInt8 intfClass;
  UInt8 intfSubClass;
  UInt8 intfNumEndpoints;

#ifdef OSX_UGL_DEBUG
#warning OSX_UGL_DEBUG defined !
  UInt32 numBytesRead;
  UInt32 i;
  char test[4];
  static char gBuffer[33];

  // this will display a "A" on the calc
  test[0] = 0x08;
  test[1] = 0x87;
  test[2] = 'A';
  test[3] = 0;
#endif				/* OSX_UGL_DEBUG */

  request.bInterfaceClass = 255;	// proprietary device
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
					 &plugInInterface, &score);
  kr = IOObjectRelease(usbInterface);	// done with the usbInterface object now that I have the plugin

  if ((kIOReturnSuccess != kr) || !plugInInterface) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("unable to create a plugin (%08x)\n", kr);
#endif
    return kr;
  }
  // I have the interface plugin. I need the interface interface
  res = (*plugInInterface)->QueryInterface(plugInInterface,
					   CFUUIDGetUUIDBytes
					   (kIOUSBInterfaceInterfaceID),
					   (LPVOID) & intf);
  (*plugInInterface)->Release(plugInInterface);	// done with this

  if (res || !intf) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR
	("couldn't create an IOUSBInterfaceInterface (%08x)\n", (int) res);
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

  if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("unable to open interface (%08x)\n", kr);
#endif
    (void) (*intf)->Release(intf);
    return kr;
  }

  kr = (*intf)->GetNumEndpoints(intf, &intfNumEndpoints);

  if (kIOReturnSuccess != kr) {
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

  kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, test, 4);	// endpoint 2

  if (kIOReturnSuccess != kr) {
    DISPLAY_ERROR("unable to do bulk write (%08x)\n", kr);
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    return kr;
  }

  DISPLAY("Wrote");
  for (i = 0; i < 4; i++) {
    DISPLAY(" 0x%x", test[i]);
  }

  DISPLAY(" (4 bytes) to bulk endpoint\n");

  numBytesRead = sizeof(gBuffer) - 1;	// leave one uint8_t at the end for NUL termination

  kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, gBuffer, &numBytesRead);	// endpoint 1

  if (kIOReturnSuccess != kr) {
    DISPLAY_ERROR("unable to do bulk read (%08x)\n", kr);
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    return kr;
  }

  DISPLAY("Calc reply :");
  for (i = 0; i < 4; i++)	// 4 bytes only
  {
    DISPLAY(" 0x%x", gBuffer[i]);
  }
  DISPLAY("\n");
#endif				/* OSX_UGL_DEBUG */

  return kr;
}

IOReturn ConfigureTIGL(IOUSBDeviceInterface ** dev)
{
  UInt8 numConf;
  IOReturn kr;
  IOUSBConfigurationDescriptorPtr confDesc;

  kr = (*dev)->GetNumberOfConfigurations(dev, &numConf);

  if (!numConf)
    return -1;

  // get the configuration descriptor for index 0
  kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &confDesc);

  if (kr) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR
	("\tunable to get config descriptor for index %d (err = %08x)\n",
	 0, kr);
#endif
    return -1;
  }
  kr = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);

  if (kr) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR
	("\tunable to set configuration to value %d (err=%08x)\n", 0, kr);
#endif
    return -1;
  }

  return kIOReturnSuccess;
}

void tiglusbFindDevice(io_iterator_t iterator)
{
  kern_return_t kr;
  io_service_t usbDevice;
  IOCFPlugInInterface **plugInInterface = NULL;
  HRESULT res;
  SInt32 score;
  UInt16 vendor;
  UInt16 product;
  UInt16 release;

  while ((usbDevice = IOIteratorNext(iterator))) {
#ifdef OSX_DEBUG
    DISPLAY("TIGL added.\n");
#endif
    kr = IOCreatePlugInInterfaceForService(usbDevice,
					   kIOUSBDeviceUserClientTypeID,
					   kIOCFPlugInInterfaceID,
					   &plugInInterface, &score);
    kr = IOObjectRelease(usbDevice);	// done with the device object now that I have the plugin
    if ((kIOReturnSuccess != kr) || !plugInInterface) {
#ifdef OSX_DEBUG
      DISPLAY_ERROR("unable to create a plugin (%08x)\n", kr);
#endif
      continue;
    }
    // I have the device plugin, I need the device interface
    res = (*plugInInterface)->QueryInterface(plugInInterface,
					     CFUUIDGetUUIDBytes
					     (kIOUSBDeviceInterfaceID),
					     (LPVOID) & dev);
    (*plugInInterface)->Release(plugInInterface);	// done with this

    if (res || !dev) {
#ifdef OSX_DEBUG
      DISPLAY_ERROR
	  ("couldn't create a device interface (%08x)\n", (int) res);
#endif
      continue;
    }
    // technically should check these kr values
    kr = (*dev)->GetDeviceVendor(dev, &vendor);
    kr = (*dev)->GetDeviceProduct(dev, &product);
    kr = (*dev)->GetDeviceReleaseNumber(dev, &release);

    if ((vendor != kTIGLVendorID)
	|| (product != kTIGLProductID)) {
#ifdef OSX_DEBUG
      DISPLAY
	  ("Found a device I didn't want (vendor = 0x%x, product = 0x%x, version = Ox%x)\n",
	   vendor, product, release);
#endif
      continue;
    } else if ((vendor == kTIGLVendorID)
	       && (product == kTIGLProductID)
	       && (release > kTIGLKnownVersion)) {
      // we'll get here if TI releases a new version of the cable

      DISPLAY_ERROR
	  ("Found TIGL USB : vendor = 0x%x, product = 0x%x, version = Ox%x)\n",
	   vendor, product, release);
      DISPLAY_ERROR("This version of the TIGL USB has not been tested.\n");
      DISPLAY_ERROR("Contact <jb@technologeek.org> about this\n");

      (*dev)->Release(dev);

      continue;
    }
    // need to open the device in order to change its state
    kr = (*dev)->USBDeviceOpen(dev);

    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      DISPLAY_ERROR("unable to open device: %08x\n", kr);
#endif
      (*dev)->Release(dev);
      continue;
    }

    kr = ConfigureTIGL(dev);

    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      DISPLAY_ERROR("unable to configure device: %08x\n", kr);
#endif
      (*dev)->USBDeviceClose(dev);
      (*dev)->Release(dev);
      continue;
    }

    kr = FindInterfaces(dev);

    if (kIOReturnSuccess != kr) {
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

int slv_init()
{
  mach_port_t masterPort;
  CFMutableDictionaryRef matchingDict;
  kern_return_t kr;
  SInt32 usbVendor = kTIGLVendorID;
  SInt32 usbProduct = kTIGLProductID;
  io_iterator_t iterator;

  // first create a master_port for my task
  kr = IOMasterPort(MACH_PORT_NULL, &masterPort);

  if (kr || !masterPort) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
#endif
    return ERR_LIBUSB_INIT;
  }
#ifdef OSX_DEBUG
  DISPLAY
      ("Looking for devices matching vendor ID=0x%x and product ID=0x%x\n",
       (int) usbVendor, (int) usbProduct);
#endif

  // Set up the matching criteria for the devices we're interested in
  matchingDict = IOServiceMatching(kIOUSBDeviceClassName);	// Interested in instances of class IOUSBDevice and its subclasses
  if (!matchingDict) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("Can't create a USB matching dictionary\n");
#endif
    mach_port_deallocate(mach_task_self(), masterPort);
    return ERR_LIBUSB_INIT;
  }
  // Add our vendor and product IDs to the matching criteria
  CFDictionarySetValue(matchingDict,
		       CFSTR(kUSBVendorID),
		       CFNumberCreate(kCFAllocatorDefault,
				      kCFNumberSInt32Type, &usbVendor));
  CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID),
		       CFNumberCreate(kCFAllocatorDefault,
				      kCFNumberSInt32Type, &usbProduct));

  // get the matching devices list
  kr = IOServiceGetMatchingServices(masterPort, matchingDict, &iterator);

  // Now done with the masterPort
  mach_port_deallocate(mach_task_self(), masterPort);
  masterPort = 0;

  // check for the return from IOServiceGetMatchingServices()
  if (iterator == NULL) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("No matching devices !\n");
#endif
    return ERR_LIBUSB_INIT;
  }

  tiglusbFindDevice(iterator);

  IOObjectRelease(iterator);

  return 0;
}

int slv_open()
{
  // if already open, then close...
  numBytesRead = 0;
  rcv_buf_ptr = rcv_buffer;

  if (dev == NULL) {
    if (slv_init() != 0)
      return ERR_LIBUSB_INIT;
  }

  if (intf == NULL) {
    if (FindInterfaces(dev) != kIOReturnSuccess)
      return ERR_OPEN_USB_DEV;
  }

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int slv_probe()
{
  return 0;
}

int slv_close()
{
  return 0;
}

int slv_exit()
{
  if (intf != NULL) {
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    intf = NULL;
  }

  numBytesRead = 0;
  rcv_buf_ptr = rcv_buffer;

  if (dev != NULL) {
    (*dev)->Release(dev);
    dev = NULL;
  }

  return 0;
}

int slv_put(uint8_t data)
{
  IOReturn kr;

  if (intf == NULL)
    return ERR_WRITE_ERROR;

  kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, &data, 1);

  if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
    DISPLAY_ERROR("Unable to do bulk write (%08x)\n", kr);

    IOKIT_ERROR(kr);
#endif
    slv_close();
    return ERR_WRITE_ERROR;
  }
#ifdef OSX_DEBUG
  DISPLAY("Wrote 0x%x to bulk endpoint %d\n", data,
	  TIGL_BULK_ENDPOINT_OUT);
#endif

  tdr.count++;

  return 0;
}

int slv_get(uint8_t * d)
{
  extern int time_out;

  IOReturn kr;

  tiTIME clk;

#ifdef OSX_DEBUG
  int i;

  DISPLAY_ERROR("IN UGL_GET\n");
#endif

  if (intf == NULL)
    return ERR_READ_ERROR;

  if (numBytesRead <= 0)	// we're at the end of the buffer
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

      kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN,
			     rcv_buffer, &numBytesRead);

      if (kIOReturnSuccess == kr) {
	if (numBytesRead > 0)	// regardless of the timeout, we HAVE DATA !
	  break;
	else if ((numBytesRead == 0)
		 && !(toELAPSED(clk, time_out)))
	  DISPLAY_ERROR
	      ("ReadPipeTO returned before timeout with no data. Retrying...\n");
	else if ((numBytesRead == 0)
		 && (toELAPSED(clk, time_out)))
	  return ERR_READ_TIMEOUT;
      } else			// There was an error, let's see what happened
	break;
    }
    while (numBytesRead == 0);


    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      DISPLAY_ERROR("Unable to do bulk read (0x%x)\n", kr);

      IOKIT_ERROR(kr);
#endif
      numBytesRead = 0;

      slv_close();

      return ERR_READ_ERROR;
    }

    rcv_buf_ptr = rcv_buffer;

#ifdef OSX_DEBUG
    DISPLAY("numBytesRead : %ld\n", numBytesRead);

    DISPLAY("Buffer content:");

    for (i = 0; i < numBytesRead; i++) {
      DISPLAY(" 0x%x", rcv_buffer[i]);
    }
    DISPLAY("\n");
#endif
  }
#ifdef OSX_DEBUG
  DISPLAY("Calc reply : 0x%x on bulk endpoint %d\n", rcv_buf_ptr[0],
	  TIGL_BULK_ENDPOINT_IN);
#endif

  tdr.count++;

  *d = *rcv_buf_ptr++;
  numBytesRead--;

  return 0;
}

int slv_check(int *status)
{
  extern int time_out;

  IOReturn kr;

  UInt32 timeout = 100 * time_out;	// time_out is specified in tenth of seconds

  tiTIME clk;

  // we cannot use select() nor poll()
  // so...

  *status = STATUS_NONE;
  if (intf != NULL) {
    if (numBytesRead > 0) {
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
      kr = (*intf)->ReadPipeTO(intf,
			       TIGL_BULK_ENDPOINT_IN,
			       rcv_buffer, &numBytesRead,
			       timeout, timeout);

      if (kIOReturnSuccess == kr) {
	if (numBytesRead > 0)	// regardless of the timeout, we HAVE DATA !
	  break;
	else if ((numBytesRead == 0)
		 && !(toELAPSED(clk, time_out)))
	  DISPLAY_ERROR
	      ("ReadPipeTO returned before timeout with no data. Retrying...\n");
	else if ((numBytesRead == 0)
		 && (toELAPSED(clk, time_out)))
	  return ERR_READ_TIMEOUT;
      } else			// There was an error, let's see what happened
	break;
    }
    while (numBytesRead == 0);

    if (kr == kIOReturnSuccess) {
#ifdef OSX_DEBUG
      DISPLAY("In slv_check: numBytesRead = %ld\n", numBytesRead);
#endif
      rcv_buf_ptr = rcv_buffer;

      *status = STATUS_RX;

      return 0;
    } else {
      numBytesRead = 0;
      *status = STATUS_NONE;
      return 0;
    }
  }

  return 0;
}

// we can't drive it at low-level, so these functions are unused

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire(int b)
{
  return 0;
}

int slv_set_white_wire(int b)
{
  return 0;
}

int slv_get_red_wire()
{
  return 0;
}

int slv_get_white_wire()
{
  return 0;
}

int slv_supported()
{
  return SUPPORT_ON;
}

#else				// unsupported platform

/************************/
/* Unsupported platform */
/************************/

int slv_init()
{
  return 0;
}

int slv_open()
{
  return 0;
}

int slv_put(uint8_t data)
{
  return 0;
}

int slv_get(uint8_t * d)
{
  return 0;
}

int slv_probe()
{
  return 0;
}

int slv_close()
{
  return 0;
}

int slv_exit()
{
  return 0;
}

int slv_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire(int b)
{
  return 0;
}

int slv_set_white_wire(int b)
{
  return 0;
}

int slv_get_red_wire()
{
  return 0;
}

int slv_get_white_wire()
{
  return 0;
}

int slv_supported()
{
  return SUPPORT_OFF;
}

#endif


/*********************************/
/* Linux   : libusb support      */
/* Author  : Julien BLACHE       */
/* Contact : jb@technologeek.org */
/* Date    : 20011126            */
/*********************************/


#if defined(HAVE_LIBUSB) && (defined(__LINUX__) || defined(__BSD__))

#include <usb.h>

#define TIGL_VENDOR_ID  0x0451	/* Texas Instruments, Inc.        */
#define TIGL_PRODUCT_ID 0xE001	/* TI-GRAPH LINK USB (SilverLink) */

#define TIGL_BULK_IN    0x81	// 0x81?
#define TIGL_BULK_OUT   0x02

struct usb_bus *bus = NULL;
struct usb_device *dev = NULL;
struct usb_device *tigl_dev = NULL;
usb_dev_handle *tigl_han = NULL;

#define DISPLAY_ERR DISPLAY_ERROR

static void find_tigl_device(void)
{
  /* loop taken from testlibusb.c */
  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if ((dev->descriptor.idVendor == TIGL_VENDOR_ID) &&
	  (dev->descriptor.idProduct == TIGL_PRODUCT_ID)) {
	/* keep track of the TIGL device */
	DISPLAY(_("TIGL-USB found with libusb.\n"));

	tigl_dev = dev;
	break;
      }
    }

    /* if we found the device, then stop... */
    if (tigl_dev != NULL)
      break;
  }
}

static int enumerate_tigl_device(void)
{
  int ret = 0;

  /* init the libusb */
  usb_init();

  /* find all usb busses on the system */
  ret = usb_find_busses();
  if (ret < 0) {
    DISPLAY_ERR(_("usb_find_busses: %s\n"), usb_strerror());
    return ERR_LIBUSB_OPEN;
  }

  /* find all usb devices on all discovered busses */
  ret = usb_find_devices();
  if (ret < 0) {
    DISPLAY_ERR(_("usb_find_devices: %s\n"), usb_strerror());
    return ERR_LIBUSB_OPEN;
  }

  /* iterate through the busses/devices */
  find_tigl_device();

  /* if we didn't find our TIGL USB, then slv_init() and retry... */
  if (tigl_dev != NULL) {
    tigl_han = usb_open(tigl_dev);
    if (tigl_han != NULL) {
      /* interface 0, configuration 1 */
      ret = usb_claim_interface(tigl_han, 0);
      if (ret < 0) {
	DISPLAY_ERR("usb_claim_interface: %s\n", usb_strerror());
	return ERR_LIBUSB_INIT;
      }

      ret = usb_set_configuration(tigl_han, 1);
      if (ret < 0) {
	DISPLAY_ERR("usb_set_configuration: %s\n", usb_strerror());
	return ERR_LIBUSB_INIT;
      }

      return 0;
    } else
      return ERR_LIBUSB_OPEN;
  }

  if (tigl_han == NULL)
    return ERR_LIBUSB_OPEN;

  return 0;
}

int slv_init2()
{
  START_LOGGING();

  return enumerate_tigl_device();
}

int slv_open2()
{
  int ret = 0;

  if (tigl_han == NULL) {
    if (slv_init2() != 0)
      return ERR_LIBUSB_OPEN;
  }

  /* Flush buffer */
  /*
     ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rBuf, 
     MAX_PACKET_SIZE, (time_out * 10));
   */

#if !defined(__BSD__)
  /* Reset endpoints */
  ret = usb_clear_halt(tigl_han, TIGL_BULK_OUT);
  if (ret < 0) {
    DISPLAY_ERR("usb_clear_halt: %s\n", usb_strerror());

    ret = usb_resetep(tigl_han, TIGL_BULK_OUT);
    if (ret < 0) {
      DISPLAY_ERR("usb_resetep: %s\n", usb_strerror());

      ret = usb_reset(tigl_han);
      if (ret < 0) {
	DISPLAY_ERR("usb_reset: %s\n", usb_strerror());
	return ERR_LIBUSB_RESET;
      }
    }
  }

  ret = usb_clear_halt(tigl_han, TIGL_BULK_IN);
  if (ret < 0) {
    DISPLAY_ERR("usb_clear_halt: %s\n", usb_strerror());

    ret = usb_resetep(tigl_han, TIGL_BULK_OUT);
    if (ret < 0) {
      DISPLAY_ERR("usb_resetep: %s\n", usb_strerror());

      ret = usb_reset(tigl_han);
      if (ret < 0) {
	DISPLAY_ERR("usb_reset: %s\n", usb_strerror());
	return ERR_LIBUSB_RESET;
      }
    }
  }
#endif

  /* Reset buffers */
  nBytesRead = 0;
  nBytesWrite = 0;

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int slv_close2()
{
  return 0;
}

int slv_exit2()
{
  tigl_dev = NULL;

  STOP_LOGGING();

  if (tigl_han != NULL) {
    usb_release_interface(tigl_han, 0);
    usb_close(tigl_han);
    tigl_han = NULL;
  }

  return 0;
}

int slv_put2(uint8_t data)
{
  int ret = 0;

  tdr.count++;
  LOG_DATA(data);
#ifndef BUFFERED_W
  /* Byte per byte */
  ret = usb_bulk_write(tigl_han, TIGL_BULK_OUT, &data, 1, (time_out * 10));
  if (ret <= 0) {
    DISPLAY_ERR("usb_bulk_write: %s\n", usb_strerror());
    return ERR_WRITE_ERROR;
  }
#else
  /* Packets (up to 32 bytes) */
  wBuf[nBytesWrite++] = data;
  if (nBytesWrite == MAX_PACKET_SIZE) {
    ret =
	usb_bulk_write(tigl_han, TIGL_BULK_OUT, wBuf,
		       nBytesWrite, (time_out * 10));
    if (ret <= 0) {
      DISPLAY_ERR("usb_bulk_write: %s\n", usb_strerror());
      return ERR_WRITE_ERROR;
    }
    nBytesWrite = 0;
  }
#endif

  return 0;
}

int slv_get2(uint8_t * data)
{
  int ret = 0;
  tiTIME clk;
  static uint8_t *rBufPtr;

  //printf(".");

  tdr.count++;
#ifdef BUFFERED_W
  /* Flush write buffer */
  if (nBytesWrite > 0) {
    ret =
	usb_bulk_write(tigl_han, TIGL_BULK_OUT, wBuf,
		       nBytesWrite, (time_out * 10));
    nBytesWrite = 0;
    if (ret <= 0) {
      DISPLAY_ERR("usb_bulk_write: %s\n", usb_strerror());
      return ERR_WRITE_ERROR;
    }
  }
#endif

  if (nBytesRead <= 0) {
    toSTART(clk);
    do {
      ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rBuf,
			  MAX_PACKET_SIZE, (time_out * 10));
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
      if (ret == 0)
	DISPLAY_ERR
	    (_
	     ("usb_bulk_read returns without any data. Retrying for circumventing quirk...\n"));
    }
    while (!ret);

    if (ret < 0) {
      DISPLAY_ERR("usb_bulk_read: %s\n", usb_strerror());
      nBytesRead = 0;
      return ERR_READ_ERROR;
    }
    nBytesRead = ret;
    rBufPtr = rBuf;
  }

  *data = *rBufPtr++;
  nBytesRead--;
  LOG_DATA(*data);

  return 0;
}

int slv_probe2()
{
  if (tigl_dev != NULL)
    return 0;
  else
    return ERR_PROBE_FAILED;
}

int slv_check2(int *status)
{
  tiTIME clk;
  int ret = 0;

  /* Since the select function does not work, I do it myself ! */
  *status = STATUS_NONE;

  if (tigl_han != NULL) {
    if (nBytesRead > 0) {
      *status = STATUS_RX;
      return 0;
    }

    toSTART(clk);
    do {
      ret = usb_bulk_read(tigl_han, TIGL_BULK_IN, rBuf,
			  MAX_PACKET_SIZE, (time_out * 10));
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
      if (ret == 0)
	DISPLAY_ERR
	    ("usb_bulk_read returns without any data. Retrying...\n");
    }
    while (!ret);

    if (ret > 0) {
      nBytesRead = ret;
      *status = STATUS_RX;
      return 0;
    } else {
      nBytesRead = 0;
      *status = STATUS_NONE;
      return 0;
    }
  }

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire2(int b)
{
  return 0;
}

int slv_set_white_wire2(int b)
{
  return 0;
}

int slv_get_red_wire2()
{
  return 0;
}

int slv_get_white_wire2()
{
  return 0;
}

int slv_supported2()
{				/* HELL YES IT'S SUPPORTED ! :-) */
  return SUPPORT_ON;
}

#else				// unsupported platforms

int slv_init2()
{
  return 0;
}

int slv_open2()
{
  return 0;
}

int slv_put2(uint8_t data)
{
  return 0;
}

int slv_get2(uint8_t * d)
{
  return 0;
}

int slv_probe2()
{
  return 0;
}

int slv_close2()
{
  return 0;
}

int slv_exit2()
{
  return 0;
}

int slv_check2(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int slv_set_red_wire2(int b)
{
  return 0;
}

int slv_set_white_wire2(int b)
{
  return 0;
}

int slv_get_red_wire2()
{
  return 0;
}

int slv_get_white_wire2()
{
  return 0;
}

int slv_supported2()
{
  return SUPPORT_OFF;
}

#endif
