/* Hey EMACS -*- linux-c -*- */
/* $Id: error.h 1035 2005-05-07 21:04:23Z roms $ */

/*  libticalcs - Ti Link Calc library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifndef __TICALCS_ERROR__
#define __TICALCS_ERROR__

/* Error codes must begin at 256 up to 511 */

typedef enum
{
	ERR_NO_ERROR = 0,		// No error (returns 0)

	ERR_ABORT = 256,		// Operation aborted (exported in ticalcs.h)
	ERR_NOT_READY,			// Calculator is not ready (don't change it !)
	ERR_CHECKSUM,			// Checksum error
	ERR_INVALID_HOST,		// Invalid host ID
	ERR_INVALID_TARGET,		// Invalid target ID
	ERR_INVALID_CMD,		// Invalid command ID
	ERR_EOT,				// Not really an error (internal use)
	ERR_VAR_REJECTED,		// The variable has been rejected by calc
	ERR_CTS_ERROR,			// Invalid CTS packet
	ERR_NACK,				// NACK received
	ERR_INVALID_PACKET,		// Packet is invalid

	ERR_MALLOC,				// No memory ?!
	ERR_NO_CABLE,			// No cable attached
	ERR_BUSY,				// Device is busy (transfer in progress)
	ERR_FILE_OPEN,			// Can't open file
	ERR_UNSUPPORTED,		// This function does not exist for the calc

// ---

	ERR_VOID_FUNCTION,		// This function does not exist for the calc
	ERR_OUT_OF_MEMORY,		// Calc reply out of *memory
	ERR_OPEN_FILE,
	ERR_SAVE_FILE,			// Can not open (writing) file
	ERR_PENDING_TRANSFER,	// A transfer is in progress

	ERR_FATAL_ERROR,		// Fatal error (alloc, file, ...)
	ERR_MISSING_VAR,		// The requested var does not exist
	ERR_NO_VARS,			// No vars to backup

// ---

	ERR_ROM_ERROR,
	ERR_NO_CALC,			// No calc probed
	ERR_INVALID_HANDLE,		// Invalid handle
	ERR_INVALID_PARAMETER,

// --- 300 to 349 are reserved for hand-held status (DUSB)

	ERR_CALC_ERROR2 = 300,	// Hand-held returned an error code

// --- 350 to 399 are reserved for hand-held status (DBUS)

	ERR_CALC_ERROR1 = 350,	// Hand-held returned an error code

// --- 400 to 459 are reserved for hand-held status (NSpire)

	ERR_CALC_ERROR3 = 400,

// ---

} CalcError;

#endif
