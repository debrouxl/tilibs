/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 2013-2016  Lionel Debroux
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

#ifndef __TIOPERS_ERROR__
#define __TIOPERS_ERROR__

/* Error codes must begin at 768 up to 1023 */

typedef enum
{
	ERR_NO_ERROR = 0,		// No error (returns 0)

	ERR_MALLOC,				// No memory ?!
	ERR_NO_CABLE,			// No cable attached
	ERR_NO_CALC,			// No calc attached
	ERR_BUSY,				// Device is busy (transfer in progress)

	ERR_ATTACHING_CABLE,		// Error while attaching cable
	ERR_ATTACHING_CALC,		// Error while attaching calc

	ERR_INVALID_HANDLE,		// Invalid handle
	ERR_INVALID_PARAMETER,

	ERR_SAVE_FILE,			// Cannot open file for writing
	ERR_UNSUPPORTED		// This function does not exist for the calc

} OperError;

#endif
