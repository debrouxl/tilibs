/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

#ifndef __TICALC_ERRCODES__
#define __TICALC_ERRCODES__

/* Error codes: ERR_...  */
/* Error codes must begin at 256 up to 511 */
#define ERR_ABORT              -1	// Operation aborted
#define ERR_CHECKSUM           307	// Checksum error
#define ERR_NOT_READY          320	// Calculator is not ready
#define ERR_VOID_FUNCTION      325	// This function does not exist for the calc
#define ERR_OUT_OF_MEMORY      258	// Calc reply out of *memory
#define ERR_OPEN_FILE          ERR_FILE_OPEN
#define ERR_SAVE_FILE          264	// Can not open (writing) file
#define ERR_PENDING_TRANSFER   333	// A transfer is in progress

#define ERR_INVALID_HOST       400	// Invalid host ID
#define ERR_INVALID_TARGET     401	// Invalid target ID
#define ERR_INVALID_CMD        402	// Invalid command ID
#define ERR_EOT                403	// Not really an error (internal use)
#define ERR_VAR_REJECTED       404	// The variable has been rejected by calc
#define ERR_CTS_ERROR          405	// Invalid CTS packet
#define ERR_NACK               406	// NACK received
#define ERR_INVALID_PACKET     407	// Packet is invalid
//#define ERR_FILE_OPEN          261 // Can not open (reading) file
//#define ERR_FILE_CLOSE         262 // Can not close file
#define ERR_FATAL_ERROR        408	// Fatal error (alloc, file, ...)
#define ERR_MISSING_VAR        409	// The requested var does not exist
#define ERR_NO_VARS            410	// No vars to backup

#endif
