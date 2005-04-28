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

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBTICABLES_VERSION "0.0.1"
#else
# define LIBTICABLES_VERSION VERSION
#endif

/* Types */

#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

typedef enum 
{
	LINK_NUL = 0,
	LINK_TGL, LINK_SER, LINK_PAR, LINK_SLV,
	LINK_VTL, LINK_TIE, LINK_VTI,
} TiCableModel;

/* Ports */
typedef enum 
{
	PORT_0 = 0,
	PORT_1, PORT_2, PORT_3, PORT_4,
} TiCablePort;

typedef enum 
{
	STATUS_NONE = 0, 
	STATUS_RX, STATUS_TX,
} TiCableStatus;

typedef enum
{
	SUPPORT_OFF     = 0,
	SUPPORT_ON      = 1,
	SUPPORT_IO      = 2,
	SUPPORT_DCB     = 4,
	SUPPORT_TIPAR   = 8,
	SUPPORT_TISER   = 16,
	SUPPORT_USB     = 32,	
} TiCableSupport;

typedef enum 
{
	IOM_NULL	= 0,
	IOM_AUTO	= 1, 
	IOM_ASM		= 2, 
	IOM_IOCTL	= 4, 
	IOM_DRV		= 8, 
	IOM_API		= 32, 
	IOM_OK		= (1 << 15)
} TiCableMethod;

typedef enum
{
	OS_NONE = 0,
	OS_WIN9X,
	OS_WINNT,
	OS_LINUX,
	OS_MACOS,
	OS_BSD,
} TiCableOs;

typedef struct 
{
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

typedef struct 
{
	int		count;			// Number of bytes exchanged
	tiTIME	start;			// Time when transfer has begun
	tiTIME	current;		// Current time (free for use)
} TiDataRate;

typedef struct
{
	TiCableModel	model;	// Cable model
	TiCablePort		port;	// Generic port

	int		timeout;		// Timeout value for cables in 0.10 seconds
	int		delay;			// Time between 2 bits (home-made cables only)

	//----------------------//

	int		device;			// The character device: COMx, ttySx, ... (private)
	int		address;		// I/O port base address (private)

	TiCableMethod	method;	// I/O method used for access (private)
	TiDataRate		dr;		// Data rate during transfers (private/public)

	void*			priv;	// Holding data (private)
} TiCableHandle;

/* Resources to detect */
#define IO_NONE   0			/* No I/O available                         */
#define IO_ASM    (1<<0)	/* Internal I/O routines (always available) */
#define IO_API    (1<<2)	/* Win32 or Linux API    (always available) */
#define IO_DLL    (1<<3)	/* PortTalk device driver (NT4/2000/XP)     */
#define IO_TIPAR  (1<<4)	/* tipar kernel module (Linux)              */
#define IO_TISER  (1<<5)	/* tiser kernel module (Linux)              */
#define IO_TIUSB  (1<<6)	/* tiglusb kernel module (Linux)            */
#define IO_LIBUSB (1<<7)	/* libusb (Linux)							*/
#define IO_USB	  (1<<8)	/* tiglusb (Win32)							*/ 

// namespace scheme: library_class_function like ticables_fext_get

	/****************/
	/* Entry points */
	/****************/
  
	TIEXPORT int TICALL ticables_library_init(void);
	TIEXPORT int TICALL ticables_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// ticables.c
	TIEXPORT const char* TICALL ticables_version_get (void);

	// error.c
	TIEXPORT int         TICALL ticables_error_get (int number, char **message);

	// type2str.c
	TIEXPORT const char *TICALL ticables_model_to_string(TiCableModel model);
	TIEXPORT TiCableModel TICALL ticables_string_to_model (const char *str);

	TIEXPORT const char *TICALL ticables_port_to_string(TiCablePort port);
	TIEXPORT TiCablePort TICALL ticables_string_to_port(const char *str);

	TIEXPORT const char *TICALL ticables_method_to_string(TiCableMethod method);

	TIEXPORT const char *TICALL ticables_os_to_string(TiCableOs port);
	TIEXPORT TiCableOs TICALL ticables_string_to_os(const char *str);
  
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
