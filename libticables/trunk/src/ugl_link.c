/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin, Julien BLACHE
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

/**************/
/* Linux part */
/**************/

/* 
   This part talk use the tiglusb.c kernel module for handling the TI-GRAPH 
   LINK USB link cable.

   Some important remarks: 
   - this link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE). This is transparent for the user because 
   the driver manages all these things for us. Nethertheless, it is better 
   (for USB & OS performances) to read/write a set of bytes rather than byte 
   per byte. This is due to the current version of our driver.
   A future release will allow to write byte per byte.
   - for reading, we try to read up to 32 bytes and we store them in a buffer 
   even if we need only a byte. Else, if we try to get byte per byte, it will 
   not work.
   - for writing, we store bytes in a buffer. The buffer is flushed (sent) is 
   buffer size > 32 or if the get function has been called.
*/

#if defined(__LINUX__)

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
#ifdef HAVE_TI_TIUSB_H
# define IOCTL_EXPORT  //use tiusb.h file
# include <ti/tiusb.h> //ioctl codes
# include <sys/ioctl.h>
#endif
*/

#include "typedefs.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "timeout.h"
#include "export.h"
#include "verbose.h"
#include "logging.h"

#define BUFFERED_R   /* enable buffered read operations */
//#define BUFFERED_W /* enable buffered write operations */ 
#define MAX_PACKET_SIZE 32 // 32 bytes max per packet

extern int time_out;
static int dev_fd = 0;

static struct cs
{
  byte data;
  int available;
} cs;

/* For buffered writing */
static int nBytesWrite = 0;
#ifdef BUFFERED_W
static byte wBuf[MAX_PACKET_SIZE];
#endif
/* For buffered reading */
static int nBytesRead = 0;
static byte rBuf[MAX_PACKET_SIZE];
static byte *rBufPtr = NULL;

int ugl_init_port()
{
  int mask;

  /* Init some internal variables */
  cs.available = 0;
  cs.data = 0;

  /* Open the device */
  mask = O_RDWR | O_NONBLOCK | O_SYNC;
    if( (dev_fd = open(device, mask)) == -1)
    {
      DISPLAY("Unable to open this device: %s\n", device);
      DISPLAY("Is the tiusb.c module loaded ?\n");
      return ERR_USB_OPEN;
    }
  START_LOGGING();
  /*  
#ifdef HAVE_TI_TIUSB_H
  mask = time_out;
  if(ioctl(dev_fd, IOCTL_TIGLUSB_TIMEOUT, mask) == -1)
    {
      DISPLAY("Unable to use IOCTL codes.\n");
      return ERR_IOCTL;
    }
#endif
  */

  return 0;
}

int ugl_open_port(void)
{
  /* Clear buffers */
  nBytesRead = 0;
  nBytesWrite = 0;

  /*  
#ifdef HAVE_TI_TIUSB_H
  value = 0;
  if(ioctl(dev_fd, IOCTL_TIGLUSB_RESET, value) == -1)
    {
      DISPLAY("Unable to use IOCTL codes.\n");
      return ERR_IOCTL;
    }
#endif
  */

  return 0;
}

int ugl_put(byte data)
{
  int err;
  
#ifndef BUFFERED_W /* Write data byte per byte */
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
#else /* Write data by packets (up to 32 bytes) */
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
#ifdef BUFFERED_W
  int err;
  int i;

  /* Flush write buffer */
  if(nBytesWrite > 0)
    {
      err = write(dev_fd, (void *)(&wBuf), nBytesWrite);
      nBytesWrite = 0;
      //DISPLAY("get, write flushed, nBytesWritten = %i\n", err);
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
      nBytesRead = read(dev_fd, (void *)rBuf, MAX_PACKET_SIZE);
      rBufPtr = rBuf;
      if(nBytesRead == -1) return ERR_RCV_BYT;
      if(nBytesRead == 0) return ERR_RCV_BYT_TIMEOUT;
    }
       
  *data = *rBufPtr;
  rBufPtr++;
  nBytesRead--;
#else
  nBytesRead = read(dev_fd, (void *)data, 1);
  if(nBytesRead == -1) return ERR_RCV_BYT;
  if(nBytesRead == 0) return ERR_RCV_BYT_TIMEOUT;
#endif
  //DISPLAY("get: 0x%02x\n", *data);
  LOG_DATA(*data);

  return 0;
}

int ugl_probe_port(void)
{
  return 0;
}

int ugl_close_port(void)
{
  return 0;
}

int ugl_term_port()
{
  STOP_LOGGING();
  if(dev_fd)
    {
      close(dev_fd);
      dev_fd=0;
    }

  return 0;
}

int ugl_check_port(int *status)
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
int DLLEXPORT2 ugl_supported()
{
  return SUPPORT_ON;
}

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

/* 
	This part talk with the USB device driver through ugl_drv.c. The ugl_drv.c file is
	widely inspired from rwbulk.c, a sample program provided with the Win98 DDK.
	There is a subdirectory named TiglUsb which contains files specific to this link cable
	(definitions, library, routines, ...). These files comes from DDK and DDK's sample.

	Some important remarks: 
	- this link cable use Bulk mode with packets. The max size of a 
	packet is 32 bytes (BULKUSB_MAX_TRANSFER_SIZE). This is transparent for the user because 
	the driver manages all these things for us. Nethertheless, it is better (for USB & OS 
	performances) to read/write a set of bytes rather than byte per byte.
	- for reading, we try to read up to 32 bytes and we store them in a buffer even
	if we need only a byte. Else, if we try to get byte per byte, it will not work.
	- for writing, we store bytes in a buffer. The buffer is flushed (sent) is buffer
	size > 32 or if the get function has been called.
*/

#include <stdio.h>
#include <windows.h>

#include "timeout.h"
#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "plerror.h"
#include "cabl_ext.h"

//#define BUFFERED_W /* enable buffered write operations */ 
#define BUFFERED_R   /* enable buffered read operations */

#include "./tiglusb/devioctl.h"
#include "./tiglusb/bulkusb.h"

extern char inPipe[32];		// Pipe name for bulk input pipe on our TIGL USB
extern char outPipe[32];	// Pipe name for bulk output pipe on our TIGL USB
extern char completeDeviceName[256];  //generated from the GUID registered by the driver itself 

static HANDLE hRead  = INVALID_HANDLE_VALUE;	// Named pipe
static HANDLE hWrite = INVALID_HANDLE_VALUE;	// Named pipe
static HANDLE hDevice = INVALID_HANDLE_VALUE;	// for DeviceIoCtl

/* For buffered writing */
static DWORD nBytesWrite = 0;
static byte wBuf[32];
/* For buffered reading */
static DWORD nBytesRead = 0;
static byte rBuf[32];

extern int time_out;		// Timeout value for cables in 0.10 seconds
static struct cs
{
  byte data;
  int available;
} cs;

DLLEXPORT
int ugl_init_port()
{
	/* Init some internal variables */
	memset((void *)(&cs), 0, sizeof(cs));

	/* Open the USB device: 2 named pipes (endpoints) */
	hWrite = open_file(outPipe);
	if(hWrite == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "open_file error (write)\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}

	hRead = open_file(inPipe);
	if(hRead == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "open_file error (read)\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}

	return 0;
}

DLLEXPORT
int ugl_open_port()
{
	/* Clear buffers */
	nBytesRead = 0;
	nBytesWrite = 0;

	return 0;
}

DLLEXPORT
int ugl_put(byte data)
{
	BOOL fSuccess;
	int nBytesWritten;
	TIME clk;

#ifndef BUFFERED_W
	fSuccess=WriteFile(hWrite, &data, 1, &nBytesWritten, NULL);
	if(!fSuccess)
	{
		fprintf(stderr, "WriteFile\n");
		print_last_error();
		return ERR_SND_BYT;
	}
	else if(nBytesWritten == 0)
	{
		return ERR_SND_BYT_TIMEOUT;
	}
#else
	/* Write data by packets (up to 32 bytes) */
	//DISPLAY("put: 0x%02x\n", data);
	wBuf[nBytesWrite++] = data;
	if(nBytesWrite == 32)
	{
		fSuccess=WriteFile(hWrite, &wBuf, nBytesWrite, &nBytesWritten, NULL);
		nBytesWrite = 0;
	}
#endif

	return 0;
}

DLLEXPORT
int ugl_get(byte *data)
{
	BOOL fSuccess;
	TIME clk;
	static byte *rBufPtr;
	int j;
	int nBytesWritten;

#ifdef BUFFERED_W
	/* Flush write buffer */
	fSuccess=WriteFile(hWrite, &wBuf, nBytesWrite, &nBytesWritten, NULL);
	nBytesWrite = 0;
#endif

	/* This routine try to read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and store them
	in a buffer for subsequent accesses */
	//DISPLAY("get before\n");
	if(nBytesRead == 0)
	{
		fSuccess = ReadFile(hRead, rBuf, 32, &nBytesRead, NULL);
		rBufPtr = rBuf;
	}
	
	*data = *rBufPtr++;
	nBytesRead--;
	//DISPLAY("get: 0x%02x\n", *data);

	return 0;
}

DLLEXPORT
int ugl_close_port()
{
	return 0;
}

DLLEXPORT
int ugl_term_port()
{	
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

	return 0;
}

DLLEXPORT
int ugl_probe_port()
{
  return 0;
}

DLLEXPORT
int ugl_check_port(int *status)
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

DLLEXPORT
int ugl_supported()
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

#define kTIGLVendorID		0x0451  // Texas Instruments Inc.
#define kTIGLProductID		0xe001  // TI GraphLink USB
#define kTIGLKnownVersion	0x0103  // Last known version

#define TIGL_BULK_ENDPOINT_OUT 2
#define TIGL_BULK_ENDPOINT_IN 1
#define TIGL_MAXPACKETSIZE 32

// uncomment to add some tests
//#define OSX_UGL_DEBUG

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
    printf("Interface found.\n");
#endif
    
    kr = IOCreatePlugInInterfaceForService(usbInterface, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
    kr = IOObjectRelease(usbInterface);	// done with the usbInterface object now that I have the plugin
    if ((kIOReturnSuccess != kr) || !plugInInterface)
        {
#ifdef OSX_DEBUG
            printf("unable to create a plugin (%08x)\n", kr);
#endif
            return kr;
        }
            
    // I have the interface plugin. I need the interface interface
    res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID) &intf);
    (*plugInInterface)->Release(plugInInterface);			// done with this
    if (res || !intf)
        {
#ifdef OSX_DEBUG
            printf("couldn't create an IOUSBInterfaceInterface (%08x)\n", (int) res);
#endif
            return kr;
        }
        
    kr = (*intf)->GetInterfaceClass(intf, &intfClass);
    kr = (*intf)->GetInterfaceSubClass(intf, &intfSubClass);

#ifdef OSX_DEBUG
    printf("Interface class %d, subclass %d\n", intfClass, intfSubClass);
#endif

    // Now open the interface. This will cause the pipes to be instantiated that are 
    // associated with the endpoints defined in the interface descriptor.
    kr = (*intf)->USBInterfaceOpen(intf);
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            printf("unable to open interface (%08x)\n", kr);
#endif
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    kr = (*intf)->GetNumEndpoints(intf, &intfNumEndpoints);
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            printf("unable to get number of endpoints (%08x)\n", kr);
#endif
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }

#ifdef OSX_DEBUG
    printf("Interface has %d endpoints.\n", intfNumEndpoints);
#endif

#ifdef OSX_UGL_DEBUG    
    // We can now address endpoints 1 through intfNumEndpoints. Or, we can also address endpoint 0,
    // the default control endpoint. But it's usually better to use (*usbDevice)->DeviceRequest() instead.

    kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, test, 4); // endpoint 2
    if (kIOReturnSuccess != kr)
        {
            printf("unable to do bulk write (%08x)\n", kr);
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    printf("Wrote");
    for (i = 0; i < 4; i++)
        {
            printf(" 0x%x", test[i]);
        }
          
    printf(" (4 bytes) to bulk endpoint\n");
    
    numBytesRead = sizeof(gBuffer) - 1; // leave one byte at the end for NUL termination
    kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, gBuffer, &numBytesRead); // endpoint 1
    if (kIOReturnSuccess != kr)
        {
            printf("unable to do bulk read (%08x)\n", kr);
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }

    printf("Calc reply :");
    for (i = 0; i < 4; i++)  // 4 bytes only
        {
            printf(" 0x%x", gBuffer[i]);
        }
    printf("\n");
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
            printf("\tunable to get config descriptor for index %d (err = %08x)\n", 0, kr);
#endif
            return -1;
        }
    kr = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);
    if (kr)
        {
#ifdef OSX_DEBUG
            printf("\tunable to set configuration to value %d (err=%08x)\n", 0, kr);
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
    
    while (usbDevice = IOIteratorNext(iterator))
        {
#ifdef OSX_DEBUG
            printf("TIGL added.\n");
#endif
            kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
            kr = IOObjectRelease(usbDevice);	// done with the device object now that I have the plugin
            if ((kIOReturnSuccess != kr) || !plugInInterface)
                {
#ifdef OSX_DEBUG
                    printf("unable to create a plugin (%08x)\n", kr);
#endif
                    continue;
                }
            
            // I have the device plugin, I need the device interface
            res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID)&dev);
            (*plugInInterface)->Release(plugInInterface);			// done with this
            if (res || !dev)
                {
#ifdef OSX_DEBUG
                    printf("couldn't create a device interface (%08x)\n", (int) res);
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
                    printf("found device i didn't want (vendor = 0x%x, product = 0x%x, version = Ox%x)\n", vendor, product, release);
#endif
                    continue;
                }
            else if ((vendor == kTIGLVendorID) && (product == kTIGLProductID) && (release > kTIGLKnownVersion))
                {
                    // we'll get here if TI releases a new version of the cable
                    
                    printf("Found TIGL USB : vendor = 0x%x, product = 0x%x, version = Ox%x)\n", vendor, product, release);
                    printf("This version of the TIGL USB has not been tested.\n");
                    printf("Contact <jb@technologeek.org> about this\n");
                    (*dev)->Release(dev);
                    continue;
                }

            // need to open the device in order to change its state
            kr = (*dev)->USBDeviceOpen(dev);
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    printf("unable to open device: %08x\n", kr);
#endif
                    (*dev)->Release(dev);
                    continue;
                }
        
            kr = ConfigureTIGL(dev);
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    printf("unable to configure device: %08x\n", kr);
#endif
                    (*dev)->USBDeviceClose(dev);
                    (*dev)->Release(dev);
                    continue;
                }

            kr = FindInterfaces(dev);
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    printf("unable to find interfaces on device: %08x\n", kr);
#endif
                    (*dev)->USBDeviceClose(dev);
                    (*dev)->Release(dev);
                    continue;
                }
        }
}



/***************************/
/* libticables begins here */
/***************************/

DLLEXPORT
int ugl_init_port()
{
    mach_port_t 		masterPort;
    CFMutableDictionaryRef 	matchingDict;
    kern_return_t		kr;
    SInt32			usbVendor = kTIGLVendorID;
    SInt32			usbProduct = kTIGLProductID;
    io_iterator_t iterator;
  
    // first create a master_port for my task
    kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (kr || !masterPort)
        {
#ifdef OSX_DEBUG
            printf("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
#endif
            return -1;
        }

#ifdef OSX_DEBUG
    printf("Looking for devices matching vendor ID=0x%x and product ID=0x%x\n", (int)usbVendor, (int)usbProduct);
#endif

    // Set up the matching criteria for the devices we're interested in
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);	// Interested in instances of class IOUSBDevice and its subclasses
    if (!matchingDict)
        {
#ifdef OSX_DEBUG
            printf("Can't create a USB matching dictionary\n");
#endif
            mach_port_deallocate(mach_task_self(), masterPort);
            return -1;
        }
    
    // Add our vendor and product IDs to the matching criteria
    CFDictionarySetValue( 
            matchingDict, 
            CFSTR(kUSBVendorID), 
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor)); 
    CFDictionarySetValue( 
            matchingDict, 
            CFSTR(kUSBProductID), 
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct)); 

    // get the matching devices list
    kr = IOServiceGetMatchingServices(masterPort, matchingDict, &iterator);

    // Now done with the master_port
    mach_port_deallocate(mach_task_self(), masterPort);
    masterPort = 0;

    // check for the return from IOServiceGetMatchingServices()
    if (iterator == NULL)
        {
#ifdef OSX_DEBUG
            printf("No matching devices !\n");
#endif
            return ERR_USB_OPEN; // hmm...
        }
    
    tiglusbFindDevice(iterator);
    
    IOObjectRelease(iterator);

    return 0;
}

DLLEXPORT
int ugl_open_port()
{
    // if already open, then close...
    numBytesRead = 0;
    rcv_buf_ptr = rcv_buffer;

    if (intf != NULL)
        {
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            intf = NULL;
        }
    
    if (dev == NULL)
        {
            if (ugl_init_port() != 0)
                return ERR_USB_OPEN;
        }
    
    if (FindInterfaces(dev) == kIOReturnSuccess)
        return 0;
    else
        return ERR_USB_OPEN;
}

DLLEXPORT
int ugl_probe_port()
{
  return 0;
}

DLLEXPORT
int ugl_close_port()
{
    if (intf != NULL)
        {
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            intf = NULL;
        }
        
  numBytesRead = 0;
  rcv_buf_ptr = rcv_buffer;
        
  return 0;
}

DLLEXPORT
int ugl_term_port()
{
    ugl_close_port();
        
    if (dev != NULL)
        {
            (*dev)->Release(dev);
            dev = NULL;
        }

  return 0;
}

DLLEXPORT
int ugl_put(byte data)
{
    extern int	time_out;
    
    IOReturn	kr;
    UInt32	timeout = time_out;
    
    if (intf == NULL)
        return ERR_SND_BYT;
        
    kr = (*intf)->WritePipeTO(intf, TIGL_BULK_ENDPOINT_OUT, &data, 1, timeout, timeout);
    if (kIOReturnSuccess != kr)
        {
#ifdef OSX_DEBUG
            printf("unable to do bulk write (%08x)\n", kr);
#endif
            ugl_close_port();
            return ERR_SND_BYT;
        }
        
#ifdef OSX_DEBUG
    printf("Wrote 0x%x to bulk endpoint %d\n", data, TIGL_BULK_ENDPOINT_OUT);
#endif

    return 0;
}

DLLEXPORT
int ugl_get(byte *d)
{
    extern int 	time_out;

#ifdef OSX_DEBUG
    int		i;
#endif
    UInt32	timeout = time_out;
    IOReturn	kr;

#ifdef OSX_DEBUG
    fprintf(stderr, "IN UGL_GET\n");
#endif

    if (intf == NULL)
        return ERR_RCV_BYT;

    if (numBytesRead <= 0) // we're at the end of the buffer
        {
#ifdef OSX_DEBUG
            printf("rcv_buffer empty, reading bulk pipe...\n");
#endif

            memset(rcv_buffer, 0, TIGL_MAXPACKETSIZE + 1);
         
            numBytesRead = TIGL_MAXPACKETSIZE;
                
            kr = (*intf)->ReadPipeTO(intf, TIGL_BULK_ENDPOINT_IN, rcv_buffer, &numBytesRead, timeout, timeout);
    
            if (kIOReturnSuccess != kr)
                {
#ifdef OSX_DEBUG
                    printf("unable to do bulk read (0x%x)\n", kr);
#endif
                    numBytesRead = 0;
                    
                    ugl_close_port();
                    
                    return ERR_RCV_BYT;
                }
                
            rcv_buf_ptr = rcv_buffer;
            
#ifdef OSX_DEBUG
            printf("numBytesRead : %ld\n", numBytesRead);

            printf("Buffer content:");

            for (i = 0; i < numBytesRead; i++)
                {
                    printf(" 0x%x", rcv_buffer[i]);
                }
            printf("\n");
#endif
        }
#ifdef OSX_DEBUG
    printf("Calc reply : 0x%x on bulk endpoint %d\n", rcv_buf_ptr[0], TIGL_BULK_ENDPOINT_IN);
#endif

    *d = *rcv_buf_ptr++;
    numBytesRead--;
 
    return 0;
}

DLLEXPORT
int ugl_check_port(int *status)
{
    IOReturn	kr;

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
            
            kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, rcv_buffer, &numBytesRead);
            if(kr = kIOReturnSuccess)
                {
#ifdef OSX_DEBUG
                    printf("In ugl_check_port: numBytesRead = %ld\n", numBytesRead);
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

DLLEXPORT
int ugl_supported()
{
    return SUPPORT_ON;
}

#else

/*************************/
/* Unsupported platforms */
/*************************/

DLLEXPORT
int ugl_init_port()
{
  return 0;
}

DLLEXPORT
int ugl_open_port()
{
  return 0;
}

DLLEXPORT
int ugl_put(byte data)
{
  return 0;
}

DLLEXPORT
int ugl_get(byte *d)
{
  return 0;
}

DLLEXPORT
int ugl_probe_port()
{
  return 0;
}

DLLEXPORT
int ugl_close_port()
{
  return 0;
}

DLLEXPORT
int ugl_term_port()
{
  return 0;
}

DLLEXPORT
int ugl_check_port(int *status)
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

DLLEXPORT
int ugl_supported()
{
  return SUPPORT_OFF;
}

#endif
