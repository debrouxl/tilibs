/* Hey EMACS -*- macos-c -*- */
/* $Id: slv_link.c 370 2004-03-22 18:47:32Z roms $ */

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

#include "gettext.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "printl.h"
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

#define IOKIT_ERROR(error)	printl1(2, "IOKit Error : system 0x%x, subsystem 0x%x, code 0x%x\n", \
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
  printl1(0, "Interface found.\n");
#endif

  kr = IOCreatePlugInInterfaceForService(usbInterface,
					 kIOUSBInterfaceUserClientTypeID,
					 kIOCFPlugInInterfaceID,
					 &plugInInterface, &score);
  kr = IOObjectRelease(usbInterface);	// done with the usbInterface object now that I have the plugin

  if ((kIOReturnSuccess != kr) || !plugInInterface) {
#ifdef OSX_DEBUG
    printl1(2, "unable to create a plugin (%08x)\n", kr);
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
    printl1(2, "couldn't create an IOUSBInterfaceInterface (%08x)\n", (int) res);
#endif
    return kr;
  }

  kr = (*intf)->GetInterfaceClass(intf, &intfClass);
  kr = (*intf)->GetInterfaceSubClass(intf, &intfSubClass);

#ifdef OSX_DEBUG
  printl1(0, "Interface class %d, subclass %d\n", intfClass, intfSubClass);
#endif

  // Now open the interface. This will cause the pipes to be instantiated that are 
  // associated with the endpoints defined in the interface descriptor.
  kr = (*intf)->USBInterfaceOpen(intf);

  if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
    printl1(2, "unable to open interface (%08x)\n", kr);
#endif
    (void) (*intf)->Release(intf);
    return kr;
  }

  kr = (*intf)->GetNumEndpoints(intf, &intfNumEndpoints);

  if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
    printl1(2, "unable to get number of endpoints (%08x)\n", kr);
#endif
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    return kr;
  }
#ifdef OSX_DEBUG
  printl1(0, "Interface has %d endpoints.\n", intfNumEndpoints);
#endif

#ifdef OSX_UGL_DEBUG
  // We can now address endpoints 1 through intfNumEndpoints. Or, we can also address endpoint 0,
  // the default control endpoint. But it's usually better to use (*usbDevice)->DeviceRequest() instead.

  kr = (*intf)->WritePipe(intf, TIGL_BULK_ENDPOINT_OUT, test, 4);	// endpoint 2

  if (kIOReturnSuccess != kr) {
    printl1(2, "unable to do bulk write (%08x)\n", kr);
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    return kr;
  }

  printl1(0, "Wrote");
  for (i = 0; i < 4; i++) {
    printl1(0, " 0x%x", test[i]);
  }

  printl1(0, " (4 bytes) to bulk endpoint\n");

  numBytesRead = sizeof(gBuffer) - 1;	// leave one uint8_t at the end for NUL termination

  kr = (*intf)->ReadPipe(intf, TIGL_BULK_ENDPOINT_IN, gBuffer, &numBytesRead);	// endpoint 1

  if (kIOReturnSuccess != kr) {
    printl1(2, "unable to do bulk read (%08x)\n", kr);
    (void) (*intf)->USBInterfaceClose(intf);
    (void) (*intf)->Release(intf);
    return kr;
  }

  printl1(0, "Calc reply :");
  for (i = 0; i < 4; i++)	// 4 bytes only
  {
    printl1(0, " 0x%x", gBuffer[i]);
  }
  printl1(0, "\n");
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
    printl1(2, "\tunable to get config descriptor for index %d (err = %08x)\n",
	 0, kr);
#endif
    return -1;
  }
  kr = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);

  if (kr) {
#ifdef OSX_DEBUG
    printl1(2, "\tunable to set configuration to value %d (err=%08x)\n", 0, kr);
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
    printl1(0, "TIGL added.\n");
#endif
    kr = IOCreatePlugInInterfaceForService(usbDevice,
					   kIOUSBDeviceUserClientTypeID,
					   kIOCFPlugInInterfaceID,
					   &plugInInterface, &score);
    kr = IOObjectRelease(usbDevice);	// done with the device object now that I have the plugin
    if ((kIOReturnSuccess != kr) || !plugInInterface) {
#ifdef OSX_DEBUG
      printl1(2, "unable to create a plugin (%08x)\n", kr);
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
      printl1(2, "couldn't create a device interface (%08x)\n", (int) res);
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

      printl1(2, "Found TIGL USB : vendor = 0x%x, product = 0x%x, version = Ox%x)\n",
	   vendor, product, release);
      printl1(2, "This version of the TIGL USB has not been tested.\n");
      printl1(2, "Contact <jb@technologeek.org> about this\n");

      (*dev)->Release(dev);

      continue;
    }
    // need to open the device in order to change its state
    kr = (*dev)->USBDeviceOpen(dev);

    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      printl1(2, "unable to open device: %08x\n", kr);
#endif
      (*dev)->Release(dev);
      continue;
    }

    kr = ConfigureTIGL(dev);

    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      printl1(2, "unable to configure device: %08x\n", kr);
#endif
      (*dev)->USBDeviceClose(dev);
      (*dev)->Release(dev);
      continue;
    }

    kr = FindInterfaces(dev);

    if (kIOReturnSuccess != kr) {
#ifdef OSX_DEBUG
      printl1(2, "unable to find interfaces on device: %08x\n", kr);
#endif
      (*dev)->USBDeviceClose(dev);
      (*dev)->Release(dev);
      continue;
    }
  }
}
