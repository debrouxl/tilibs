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

#ifndef __TICABLE_DEFS__
#define __TICABLE_DEFS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdints.h"
#include "export.h"
#include "timeout.h"


/*************/
/* Constants */
/*************/

/* Default values */
#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

/* Link type */
typedef enum {
  LINK_NONE,
  LINK_TGL, LINK_SER, LINK_PAR, LINK_AVR,
  LINK_VTL, LINK_TIE, LINK_VTI, LINK_TPU, LINK_SLV,
  TICABLETYPE_MAX
} TicableType;

#define LINK_UGL LINK_SLV

/* Automatic config */
#define AUTO_ADDR 0
#define AUTO_NAME ""

/* Baud rate values */
typedef enum {
  BR9600 = 9600, BR19200 = 19200, BR38400 = 38400, BR57600 = 57600
} TicableBaudRate;

/* Values returned by the check function */
typedef enum {
  STATUS_NONE, STATUS_RX, STATUS_TX,
  TICABLESTATUS_MAX
} TicableStatus;

/* Hardware flow control (RTS/CTS) */
typedef enum {
  HFC_OFF, HFC_ON
} TicableHfc;

/* Link cable support */
#define SUPPORT_OFF      0
#define SUPPORT_ON       1
#define SUPPORT_IO       2
#define SUPPORT_DCB      4
#define SUPPORT_TIPAR    8
#define SUPPORT_TISER    16
#define SUPPORT_USB      32

/* Ports */
typedef enum {
  USER_PORT,
  PARALLEL_PORT_1, PARALLEL_PORT_2, PARALLEL_PORT_3,
  SERIAL_PORT_1, SERIAL_PORT_2, SERIAL_PORT_3, SERIAL_PORT_4,
  VIRTUAL_PORT_1, VIRTUAL_PORT_2,
  USB_PORT_1, USB_PORT_2, USB_PORT_3, USB_PORT_4,
  OSX_SERIAL_PORT, OSX_USB_PORT,
  TICABLEPORT_MAX
} TicablePort;

/* I/O method to use */
typedef enum {
  IOM_AUTO = 1, IOM_ASM = 2, IOM_IOCTL = 4, IOM_DRV = 8, IOM_API = 32, IOM_OK = (1 << 15)
} TicableMethod;

/* Verbosity level for DISPLAY function */
typedef enum {
  DSP_OFF, DSP_ON, DSP_CLOSE,
  TICABLEDISPLAY_MAX,
} TicableDisplay;

/* OS probing */
#define OS_WIN9X "Windows9x"
#define OS_WINNT "WindowsNT"
#define OS_LINUX "Linux"
#define OS_MACOS "Mac OS X"
#define OS_BSD   "*BSD"
#define OS_NONE  "unknown"

/* Callback */
typedef int (*TICABLES_PRINTL) (int level, const char *format, ...);

/********************/
/* Type definitions */
/********************/

typedef struct {
  // cooked access
  int (*init) ();
  int (*open) ();
  int (*put) (uint8_t);
  int (*get) (uint8_t *);
  int (*probe) ();
  int (*close) ();
  int (*exit) ();
  int (*check) (int *);

  // raw access
  int (*set_red_wire) (int);
  int (*set_white_wire) (int);
  int (*get_red_wire) ();
  int (*get_white_wire) ();
} TicableLinkCable;

typedef struct {
  int calc_type;
  TicableType link_type;
  unsigned int io_addr;		// used for compatibility or forcing
  char device[1024];		// used for compatibility or forcing
  int timeout;
  int delay;
  TicableBaudRate baud_rate;
  TicableHfc hfc;
  TicablePort port;
  TicableMethod method;
} TicableLinkParam;

// for probe.c
#define MAX_LPT_PORTS	3	// up to 3
#define MAX_COM_PORTS	4	// up to 4

typedef struct {
	int lpt_count;		// Current number of parallel ports
	int lpt_addr[MAX_LPT_PORTS];
	int lpt_mode[MAX_LPT_PORTS];
	char lpt_name[MAX_LPT_PORTS][17];
	
	int com_count;		// Current number of serial ports
	int com_addr[MAX_COM_PORTS];
	int com_mode[MAX_COM_PORTS];
	char com_name[MAX_COM_PORTS][17];
	
	int usb_count;          // Current number of usb ports
	//...
} TicablePortInfo;

typedef struct {
  int count;			// Number of bytes exchanged
  tiTIME start;			// Time when transfer has begun
  tiTIME current;		// Current time (free for use)
} TicableDataRate;


/****************/
/* Internal use */
/****************/

/* Parallel Port addresses */
#define PP1_ADDR 0x378
#define PP2_ADDR 0x278
#define PP3_ADDR 0x3bc

/* Parallel port devices */
#if defined(__LINUX__)
# define PP1_NAME "/dev/parport0"
# define PP2_NAME "/dev/parport1"
# define PP3_NAME "/dev/parport2"
#elif defined(__WIN32__)
# define PP1_NAME "LPT1"
# define PP2_NAME "LPT2"
# define PP3_NAME "LPT3"
#elif defined(__BSD__)
# define PP1_NAME "/dev/ppi0"
# define PP2_NAME "/dev/ppi1"
# define PP3_NAME "/dev/ppi2"
#else
# define PP1_NAME ""
# define PP2_NAME ""
# define PP3_NAME ""
#endif

/* Serial Port addresses */
#define SP1_ADDR 0x3f8
#define SP2_ADDR 0x2F8
#define SP3_ADDR 0x3E8
#define SP4_ADDR 0x2E8

/* Serial port devices */
#if defined(__LINUX__)
# define SP1_NAME "/dev/ttyS0"
# define SP2_NAME "/dev/ttyS1"
# define SP3_NAME "/dev/ttyS2"
# define SP4_NAME "/dev/ttyS3"
#elif defined(__BSD__)
# define SP1_NAME "/dev/cuaa0"
# define SP2_NAME "/dev/cuaa1"
# define SP3_NAME "/dev/cuaa2"
# define SP4_NAME "/dev/cuaa3"
#elif defined(__WIN32__)
# define SP1_NAME "COM1"
# define SP2_NAME "COM2"
# define SP3_NAME "COM3"
# define SP4_NAME "COM4"
#else				// default
# define SP1_NAME ""
# define SP2_NAME ""
# define SP3_NAME ""
# define SP4_NAME ""
#endif

/* USB port devices */
#if defined(__LINUX__)
# define UP1_NAME "/dev/tiusb0"
# define UP2_NAME "/dev/tiusb1"
# define UP3_NAME "/dev/tiusb2"
# define UP4_NAME "/dev/tiusb3"
#elif defined(__WIN32__)
# define UP1_NAME "//./TiglUsb0"
# define UP2_NAME "//./TiglUsb0"
# define UP3_NAME "//./TiglUsb0"
# define UP4_NAME "//./TiglUsb0"
#else				// default
# define UP1_NAME ""
# define UP2_NAME ""
# define UP3_NAME ""
# define UP4_NAME ""
#endif

/* Virtual link devices */
#define VLINK0 1		/* Virtual link (complementary to VL1) */
#define VLINK1 2		/* Virtual link (complementaty to VL0) */

/* Resources to detect */
#define IO_NONE   0		/* No I/O available                         */
#define IO_ASM    (1<<0)	/* Internal I/O routines (always available) */
#define IO_API    (1<<2)	/* Win32 or Linux API    (always available) */
#define IO_DLL    (1<<3)	/* PortTalk device driver (NT4/2000/XP)     */
#define IO_TIPAR  (1<<4)	/* tipar kernel module (Linux)              */
#define IO_TISER  (1<<5)	/* tiser kernel module (Linux)              */
#define IO_TIUSB  (1<<6)	/* tiglusb kernel module (Linux)            */
#define IO_LIBUSB (1<<7)	/* libusb (Linux)	                    */
#define IO_USB	  (1<<8)	/* tiglusb (Win32)			    */ 

#define IO_BSD   (1<<11)	/* Any *BSD platform                        */
#define IO_LINUX (1<<12)	/* Any Linux platform                       */
#define IO_WIN9X (1<<13)	/* Windows 9x/Me                            */
#define IO_WINNT (1<<14)	/* NT4/2000/XP                              */
#define IO_WIN32 (IO_WINNT | IO_WIN9X)	/* Any Windows platform             */
#define IO_OSX   (1<<15)	/* Mac OS-X                                 */

#endif
