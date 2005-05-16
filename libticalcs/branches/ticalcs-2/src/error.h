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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TICALCS_ERROR__
#define __TICALCS_ERROR__

/* Error codes must begin at 256 up to 511 */

typedef enum
{
    ERR_NO_ERROR = 0,		// No error (returns 0)

	ERR_ABORT = 256,		// Operation aborted
	ERR_NOT_READY,			// Calculator is not ready
	ERR_CHECKSUM,			// Checksum error
	
	ERR_MALLOC,				// No memory ?!
	ERR_NO_CABLE,			// No cable attached
	ERR_BUSY,				// Device is busy (transfer in progress)
		
	ERR_VOID_FUNCTION,		// This function does not exist for the calc
	ERR_OUT_OF_MEMORY,		// Calc reply out of *memory
	ERR_OPEN_FILE,
	ERR_SAVE_FILE,			// Can not open (writing) file
	ERR_PENDING_TRANSFER,	// A transfer is in progress

	ERR_INVALID_HOST,		// Invalid host ID
	ERR_INVALID_TARGET,		// Invalid target ID
	ERR_INVALID_CMD,		// Invalid command ID
	ERR_EOT,				// Not really an error (internal use)
	ERR_VAR_REJECTED,		// The variable has been rejected by calc
	ERR_CTS_ERROR,			// Invalid CTS packet
	ERR_NACK,				// NACK received
	ERR_INVALID_PACKET,		// Packet is invalid
	ERR_FATAL_ERROR,		// Fatal error (alloc, file, ...)
	ERR_MISSING_VAR,		// The requested var does not exist
	ERR_NO_VARS,			// No vars to backup

} CalcError;

#endif
