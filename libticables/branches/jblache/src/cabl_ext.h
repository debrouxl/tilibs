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

#ifndef __CABLE_EXTERN__
#define __CABLE_EXTERN__

#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int time_out;                // Timeout value for cables in 0.10 seconds
extern int delay;                   // Time between 2 bits (home-made cables)
extern int baud_rate;               // Baud rate setting for serial port
extern uint io_address;             // I/O port base address
extern char device[MAXCHARS];       // The character device
extern const char *err_msg;         // The error message (last error occured)
extern int hfc;
extern int method;

#ifdef __cplusplus
}
#endif

#endif
