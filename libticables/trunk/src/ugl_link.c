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

/* TiGraphLink-USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"

#if defined(__LINUX__)

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "typedefs.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "timeout.h"
#include "export.h"
#include "verbose.h"

extern int time_out;

static int dev_fd = 0;

static struct cs
{
  byte data;
  int available;
} cs;

int ugl_init_port()
{
  int mask;

  /* Init some internal variables */
  cs.available = 0;
  cs.data = 0;

  /* Open the device */
  mask = O_RDWR | O_NONBLOCK | O_SYNC; // JB: you may have to change this...
  if( (dev_fd = open(device, mask)) == -1)
    {
      fprintf(stderr, "Unable to open this device: %s\n", device);
      return ERR_USB_OPEN;
    }

  return 0;
}

int ugl_open_port(void)
{
  /* Flush */

  return 0;
}

int ugl_put(byte data)
{
  int err;

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

  return 0;
}

int ugl_get(byte *data)
{
  static int n=0;
  TIME clk;

  /* If the ugl_check function was previously called, retrieve the byte */
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
      n = read(dev_fd, (void *)data, 1);
    }
  while(n == 0);
  
  if(n == -1)
    {
      return ERR_RCV_BYT;
    }

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

static struct cs
{
  byte data;
  int available;
} cs;

IOUSBDeviceInterface **dev = NULL;
IOUSBInterfaceInterface182 **intf = NULL;
char rcv_buffer[4 * TIGL_MAXPACKETSIZE];
int rcv_pos = 0;
int read_pos = 0;

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
    
    printf("Interface found.\n");
       
    kr = IOCreatePlugInInterfaceForService(usbInterface, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
    kr = IOObjectRelease(usbInterface);	// done with the usbInterface object now that I have the plugin
    if ((kIOReturnSuccess != kr) || !plugInInterface)
        {
            printf("unable to create a plugin (%08x)\n", kr);
            return kr;
        }
            
    // I have the interface plugin. I need the interface interface
    res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID) &intf);
    (*plugInInterface)->Release(plugInInterface);			// done with this
    if (res || !intf)
        {
            printf("couldn't create an IOUSBInterfaceInterface (%08x)\n", (int) res);
            return kr;
        }
        
    kr = (*intf)->GetInterfaceClass(intf, &intfClass);
    kr = (*intf)->GetInterfaceSubClass(intf, &intfSubClass);
        
    printf("Interface class %d, subclass %d\n", intfClass, intfSubClass);
        
    // Now open the interface. This will cause the pipes to be instantiated that are 
    // associated with the endpoints defined in the interface descriptor.
    kr = (*intf)->USBInterfaceOpen(intf);
    if (kIOReturnSuccess != kr)
        {
            printf("unable to open interface (%08x)\n", kr);
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    kr = (*intf)->GetNumEndpoints(intf, &intfNumEndpoints);
    if (kIOReturnSuccess != kr)
        {
            printf("unable to get number of endpoints (%08x)\n", kr);
            (void) (*intf)->USBInterfaceClose(intf);
            (void) (*intf)->Release(intf);
            return kr;
        }
        
    printf("Interface has %d endpoints.\n", intfNumEndpoints);

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
            printf("\tunable to get config descriptor for index %d (err = %08x)\n", 0, kr);
            return -1;
        }
    kr = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);
    if (kr)
        {
            printf("\tunable to set configuration to value %d (err=%08x)\n", 0, kr);
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
            printf("TIGL added.\n");
       
            kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
            kr = IOObjectRelease(usbDevice);	// done with the device object now that I have the plugin
            if ((kIOReturnSuccess != kr) || !plugInInterface)
                {
                    printf("unable to create a plugin (%08x)\n", kr);
                    continue;
                }
            
            // I have the device plugin, I need the device interface
            res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID)&dev);
            (*plugInInterface)->Release(plugInInterface);			// done with this
            if (res || !dev)
                {
                    printf("couldn't create a device interface (%08x)\n", (int) res);
                    continue;
                }
            // technically should check these kr values
            kr = (*dev)->GetDeviceVendor(dev, &vendor);
            kr = (*dev)->GetDeviceProduct(dev, &product);
            kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
        
            if ((vendor != kTIGLVendorID) || (product != kTIGLProductID))
                {
                    // we'll get here if TI releases a new version of the cable
                    printf("found device i didn't want (vendor = 0x%x, product = 0x%x, version = Ox%x)\n", vendor, product, release);
                    continue;
                }
            else if ((vendor == kTIGLVendorID) && (product == kTIGLProductID) && (release > kTIGLKnownVersion))
                {
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
                    printf("unable to open device: %08x\n", kr);
                    (*dev)->Release(dev);
                    continue;
                }
        
            kr = ConfigureTIGL(dev);
            if (kIOReturnSuccess != kr)
                {
                    printf("unable to configure device: %08x\n", kr);
                    (*dev)->USBDeviceClose(dev);
                    (*dev)->Release(dev);
                    continue;
                }

            kr = FindInterfaces(dev);
            if (kIOReturnSuccess != kr)
                {
                    printf("unable to find interfaces on device: %08x\n", kr);
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
  
    // init the cs struct
    cs.available = 0;
    cs.data = 0;
        
    // first create a master_port for my task
    kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (kr || !masterPort)
        {
            printf("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
            return -1;
        }

    printf("Looking for devices matching vendor ID=0x%x and product ID=0x%x\n", (int)usbVendor, (int)usbProduct);

    // Set up the matching criteria for the devices we're interested in
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);	// Interested in instances of class IOUSBDevice and its subclasses
    if (!matchingDict)
        {
            printf("Can't create a USB matching dictionary\n");
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
            printf("No matching devices !\n");
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

    read_pos = 0;
    rcv_pos = 0;

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
        
    read_pos = 0;
    rcv_pos = 0;
        
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
            printf("unable to do bulk write (%08x)\n", kr);
            ugl_close_port();
            return ERR_SND_BYT;
        }
    
    printf("Wrote 0x%x to bulk endpoint %d\n", data, TIGL_BULK_ENDPOINT_OUT);

    return 0;
}

DLLEXPORT
int ugl_get(byte *d)
{
    extern int 	time_out;

    char	buffer[TIGL_MAXPACKETSIZE + 1];
    int		i;
    UInt32	numBytesRead = TIGL_MAXPACKETSIZE;
    UInt32	timeout = time_out;
    IOReturn	kr;

    fprintf(stderr, "IN UGL_GET\n");

    if (intf == NULL)
        return ERR_RCV_BYT;

    if (read_pos == rcv_pos) // we're at the end of the buffer
        {
            printf("rcv_buffer empty, reading bulk pipe...\n");
            kr = (*intf)->ReadPipeTO(intf, TIGL_BULK_ENDPOINT_IN, buffer, &numBytesRead, timeout, timeout);
    
            if (kIOReturnSuccess != kr)
                {
                    printf("unable to do bulk read (0x%x)\n", kr);
                    ugl_close_port();
                    return ERR_RCV_BYT;
                }
                
            printf("Buffer is %ld bytes, copying to rcv_buffer\n", strlen(buffer));

            printf("Buffer content:");

            for (i = 0; i < strlen(buffer); i++)
                {
                    if (rcv_pos == (4 * TIGL_MAXPACKETSIZE))
                        rcv_pos = 0;
                        
                    printf(" 0x%x", buffer[i]);
                        
                    memcpy(&rcv_buffer[rcv_pos], &buffer[i], 1);
                    rcv_pos++;
                }
            printf("\n");
        }
     
    printf("Calc reply : 0x%x on bulk endpoint %d\n", rcv_buffer[read_pos], TIGL_BULK_ENDPOINT_IN);

    memcpy(d, &rcv_buffer[read_pos], 1);
    read_pos++;
    
    if (read_pos == (4 * TIGL_MAXPACKETSIZE))
        read_pos = 0;

    return 0;
}

DLLEXPORT
int ugl_check_port(int *status)
{
    IOReturn	kr;
    int		i;
    char	buffer[TIGL_MAXPACKETSIZE + 1];
    UInt32	numBytesRead = TIGL_MAXPACKETSIZE;
    
    // we cannot use select() nor poll()
    // so...
    
    *status = STATUS_NONE;
    if(intf != NULL)
        {
            kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, buffer, &numBytesRead);
            if(kr = kIOReturnSuccess)
                {
                    printf("In ugl_check_port: buffer is %ld bytes, copying to rcv_buffer\n", strlen(buffer));
        
                    for (i = 0; i < strlen(buffer); i++)
                        {
                            if (rcv_pos == (4 * TIGL_MAXPACKETSIZE))
                                rcv_pos = 0;
                        
                            memcpy(&rcv_buffer[rcv_pos], &buffer[i], 1);
                            rcv_pos++;
                        }
                    
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

/************************/
/* Unsupported platform */
/************************/

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
