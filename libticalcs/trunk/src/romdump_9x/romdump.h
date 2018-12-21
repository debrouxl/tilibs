/*  RomDumper - an TI89/92/92+/V200PLT/Titanium ROM dumper
 *
 *  Copyright (c) 2004-2005, Romain Liévin for the TiLP and TiEmu projects
 *  Copyright (c) 2005, Kevin Kofler for the Fargo-II port
 *  Copyright (c) 2006, Romain Liévin for the Direct USB port
 *  Copyright (c) 2017-2018, Lionel Debroux for the code unification
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef DUSB_DUMPER
#define VERSION		"1.02"			// Version
#define TIMEOUT		20			// 1s ~ 20 ticks
#else
#define VERSION		"1.02 USB"			// Version
#define TIMEOUT		2000				// in ms
#endif

/* CMD | LEN | DATA | CHK */
#define	CMD_IS_READY	0xAA55
#define CMD_KO				0x0000
#define CMD_OK				0x0001
#define CMD_EXIT			0x0002
#define CMD_REQ_SIZE	0x0003
#define CMD_ERROR			0x0004	// unused !
#define CMD_REQ_BLOCK	0x0005
#define CMD_DATA1			0x0006
#define CMD_DATA2			0x0007
#define CMD_NONE			0xffff

#define MSB(v)	(v >> 8)
#define LSB(v)	(v & 0xff)

#define LE_BE(v) (((v & 0xff000000UL) >> 24) | ((v & 0x00ff0000UL) >> 8) | ((v & 0x0000ff00UL) << 8) | ((v & 0x000000ffUL) << 24))

