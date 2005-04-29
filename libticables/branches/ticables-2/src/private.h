/* Hey EMACS -*- linux-c -*- */
/* $Id: ticables.h 987 2005-04-28 16:30:57Z roms $ */

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

#ifndef __PRIVATE__
#define __PRIVATE__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdints.h"
#include "export.h"
#include "timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef enum
{
    IO_NONE   = 0,		/* No I/O available                         */
    IO_ASM    = (1<<0),	/* Internal I/O routines (always available) */
    IO_API    = (1<<2),	/* Win32 or Linux API    (always available) */
    IO_DLL    = (1<<3),	/* PortTalk device driver (NT4/2000/XP)     */
    IO_TIPAR  = (1<<4),	/* tipar kernel module (Linux)              */
    IO_TISER  = (1<<5),	/* tiser kernel module (Linux)              */
    IO_TIUSB  = (1<<6),	/* tiglusb kernel module (Linux)            */
    IO_LIBUSB = (1<<7),	/* libusb (Linux)							*/
    IO_USB	  = (1<<8),	/* tiglusb (Win32)							*/ 
} TiCableResource;

typedef struct 
{
	int		count;			// Number of bytes exchanged
	tiTIME	start;			// Time when transfer has begun
	tiTIME	current;		// Current time (free for use)
} TiDataRate;

typedef struct _Cable	TiCable;
typedef struct _Cable	Cable;
typedef struct _Conn	Conn;

struct _Cable
{
	int			model;
	const char *name;
	const char *fullname;
	const char *description;

	int (*open)		(Conn *);
	int (*close)	(Conn *);

	int (*send)		(Conn *, uint8_t);
	int (*recv)		(Conn *, uint8_t *);

	int (*check)	(Conn *, int *);

	int (*reset)	(Conn *);
	int (*probe)	(Conn *);

	int (*set_d0)	(Conn *, int);
	int (*set_d1)	(Conn *, int);
	int (*get_d0)	(Conn *);
	int (*get_d1)	(Conn *);
};

struct _Conn
{
	TiCableModel	model;	// Cable model
	TiCablePort		port;	// Generic port
	int		timeout;		// Timeout value for cables in 0.10 seconds
	int		delay;			// Time between 2 bits (home-made cables only)

	TiCableMethod	method;	// I/O method used for access
	char	device[16];		// The character device: COMx, ttySx, ...
	int		address;		// I/O port base address

	TiCable			cable;
	TiDataRate		dr;		// Data rate during transfers

	void*			priv;	// Holding data

	int		open;			// Cable is open
	int		busy;			// Cable is used
};

#ifdef __cplusplus
}
#endif

#endif
