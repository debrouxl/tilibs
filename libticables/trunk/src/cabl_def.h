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

#ifndef __CABLE_DEFS__
#define __CABLE_DEFS__

#include "export.h"
#include "typedefs.h"
#include "macros.h"
#include "compat.h"

/********************/
/* Type definitions */
/********************/

struct ticable_link
{
  // cooked access
  int (*init_port)  ();
  int (*open_port)  ();
  int (*put)        (byte data);
  int (*get)        (byte *data);
  int (*probe_port) ();
  int (*close_port) ();
  int (*term_port)  ();
  int (*check_port) (int *status);

  // raw access
  int (*set_red_wire)   (int b);
  int (*set_white_wire) (int b);
  int (*get_red_wire)   ();
  int (*get_white_wire) ();
};
typedef struct ticable_link LINK_CABLE;
typedef struct ticable_link LinkCable;

struct ticable_param
{
  int calc_type;
  int link_type;
  unsigned int io_addr;  // used for compatability or forcing
  char device[MAXCHARS]; // used for compatability or forcing
  int timeout;
  int delay;
  int baud_rate;
  int hfc;

  // new fields starting at lib v2.x.x
  int port;
  int method;
};
typedef struct ticable_param LINK_PARAM;
typedef struct ticable_param LinkParam;

// for probe.c
#define MAX_LPT_PORTS	3	// up to 3
#define MAX_COM_PORTS	4       // up to 4
struct port_info_
{
  int lpt_count;	// Current number of the printer port, default=1
  int lpt_addr[MAX_LPT_PORTS+1];
  int lpt_mode[MAX_LPT_PORTS+1];
  char lpt_name[MAX_LPT_PORTS+1][17];
  
  int com_count;
  int com_addr[MAX_COM_PORTS+1];
  int com_mode[MAX_COM_PORTS+1];
  char com_name[MAX_COM_PORTS+1][17];
};
typedef struct port_info_ PortInfo;


/*********************/
/* Macro definitions */
/*********************/

/* Default values */
#define DFLT_TIMEOUT  15  /* 1.5 second */
#define DFLT_DELAY    10 /* 10 micro-seconds */

/* Link type */
#define LINK_TGL 1	/* Grey TI Graph Link */
#define LINK_SER 2	/* Home-made serial link or Black TI Graph Link */
#define LINK_PAR 3	/* Home-made parallel link */
#define LINK_AVR 4	/* My link cable: the AVRlink */
#define LINK_VTL 5      /* Virtual link */
#define LINK_TIE 6      /* Virtual link with TiE (TI Emulator) */
#define LINK_VTI 7      /* Virtual link with VTi (Virtual TI) */
#define LINK_TPU 8      /* My TI/PC USB link */
#define LINK_UGL 9      /* TI's USB GraphLink */

#define LINK_DEV 128    /* Not really a cable but an access to */
                        /* the 'tidev' kernel module (obsolete)*/

/* Automatic config */
#define AUTO_ADDR 0
#define AUTO_NAME ""

/* Parallel Port addresses */
#define PP3_ADDR 0x3bc
#define PP1_ADDR 0x378
#define PP2_ADDR 0x278

/* Parallel port devices */
#if defined(__LINUX__)
# define PP1_NAME "/dev/lp0"
# define PP2_NAME "/dev/lp1"
# define PP3_NAME "/dev/lp2"
#elif defined(__WIN32__)
# define PP1_NAME "LPT1"
# define PP2_NAME "LPT2"
# define PP3_NAME "LPT3"
#elif defined(__SOL__)
# define PP1_NAME "/dev/cua/a"
# define PP2_NAME "/dev/cua/b"
# define PP3_NAME "/dev/cua/a"
#else
# define PP1_NAME "/dev/lp0"
# define PP2_NAME "/dev/lp1"
# define PP3_NAME "/dev/lp2"
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
#elif defined(__WIN32__)
# define SP1_NAME "COM1"
# define SP2_NAME "COM2"
# define SP3_NAME "COM3"
# define SP4_NAME "COM4"
#elif defined(__SOL__) // Sparc + Solaris
# define SP1_NAME "/dev/cua/a"
# define SP2_NAME "/dev/cua/b"
# define SP3_NAME "/dev/cua/a"
# define SP4_NAME "/dev/cua/b"
#else // default
# define SP1_NAME "/dev/ttyS0"
# define SP2_NAME "/dev/ttyS1"
# define SP3_NAME "/dev/ttyS2"
# define SP4_NAME "/dev/ttyS3"
#endif

/* USB port devices */
#if defined(__LINUX__)
# define UP1_NAME "/dev/usb/tigl0"
# define UP2_NAME "/dev/usb/tigl1"
# define UP3_NAME "/dev/usb/tigl2"
# define UP4_NAME "/dev/usb/tigl3"
#else // default
# define UP1_NAME ""
# define UP2_NAME ""
# define UP3_NAME ""
# define UP4_NAME ""
#endif

/* Baud rate values */
#define BR9600  9600
#define BR19200 19200
#define BR38400 38400
#define BR57600 57600

/* Characters devices of the 'tidev' kernel module */

#define TIDEV	"/dev/ti"    /* Symbolic link to one of the folowing devices */
#define TIDEV_P0 "/dev/tiP0" /* TI device for parallel link at 0x3BC */
#define TIDEV_P1 "/dev/tiP1" /* TI device for parallel link at 0x378 */
#define TIDEV_P2 "/dev/tiP2" /* TI device for parallel link at 0x278 */
#define TIDEV_S0 "/dev/tiS0" /* TI device for serial link at 0x3F8 (COM1) */
#define TIDEV_S1 "/dev/tiS1" /* TI device for serial link at 0x2F8 (COM2) */
#define TIDEV_S2 "/dev/tiS2" /* TI device for serial link at 0x3E8 (COM3) */
#define TIDEV_S3 "/dev/tiS3" /* TI device for serial link at 0x3E8 (COM4) */
#define TIDEV_V0 "/dev/ti0"  /* Virtual link device (compl. to /dev/ti1) */
#define TIDEV_V1 "/dev/ti1"  /* Virtual link device (compl. to /dev/ti0) */

/* TiDev or not */
#define TIANY_ENABLED  8
#define TISER_ENABLED  4
#define TIPAR_ENABLED  2
#define TIDEV_ENABLED  1
#define TIDEV_DISABLED 0

/* DlPortIO driver or not */
#define DLPORTIO_ENABLED  1
#define DLPORTIO_DISABLED 0

/* DCB (Direct Control Block) or not */
#define DCB_ENABLED  1
#define DCB_DISABLED 0

/* Virtual link devices */
#define VLINK0 1      /* Virtual link (complementary to VL1) */
#define VLINK1 2      /* Virtual link (complementaty to VL0) */

/* Values returned by the check_port function */
#define STATUS_NONE 0     /* Nothing */
#define STATUS_RX 1 /* At least one char has been received */
#define STATUS_TX 2 /* One char can be sent */

/* TI/PC USB link mode */
#define TPU_RAW_MODE    1
#define TPU_COOKED_MODE 2

/* Hardware flow control (RTS/CTS) */
#define HFC_ON  1
#define HFC_OFF 0

/* Link cable support */
#define SUPPORT_OFF      0
#define SUPPORT_ON       1
#define SUPPORT_IO       2
#define SUPPORT_DCB      4
#define SUPPORT_TIPAR    8
#define SUPPORT_TISER    16
#define SUPPORT_USB      32

/* Ports */
#define PARALLEL_PORT_1 1 /* Parallel port #1 */
#define PARALLEL_PORT_2 2 /* Parallel port #2 */
#define PARALLEL_PORT_3 3 /* Parallel port #3 */
#define SERIAL_PORT_1   4 /* Serial port #1 */
#define SERIAL_PORT_2   5 /* Serial port #2 */
#define SERIAL_PORT_3   6 /* Serial port #3 */
#define SERIAL_PORT_4   7 /* Serial port #4 */
#define VIRTUAL_PORT_1  8 /* Virtual port #1 */
#define VIRTUAL_PORT_2  9 /* Virtual port #2 */
#define USB_PORT_1      10 /* USB port #0 */
#define USB_PORT_2      11 /* USB port #1 */
#define USB_PORT_3      12 /* USB port #2 */
#define USB_PORT_4      13 /* USB port #3 */
#define OSX_SERIAL_PORT 14 /* serial port handled by Mac OS X */

/* I/O method to use */
#define IOM_AUTO      0  /* Automagically choose the I/O method to use */
#define IOM_ASM       1  /* Internal ASM routines */
#define IOM_DCB       2  /* Use DCB of Win32 API (serial ports only) */
#define IOM_DRV       4  /* Device driver (NT4/2000 or Linux) */

/* Verbosity level for DISPLAY function */
#define DSP_OFF   0
#define DSP_ON    1
#define DSP_CLOSE 2

#endif



