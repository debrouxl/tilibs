/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

#ifndef __TICABLES_ERROR__
#define __TICABLES_ERROR__

/* Error codes must begin at 0 up to 255 */

typedef enum
{
    ERR_ABORT = -1,			// Operation aborted
    ERR_NO_ERROR = 0,		// No error (returns 0)

	ERR_BUSY,
	ERR_ILLEGAL_ARG,
	ERR_PORTTALK_NOT_FOUND,
	ERR_WRITE_ERROR,
    ERR_WRITE_TIMEOUT,
    ERR_READ_ERROR,
    ERR_READ_TIMEOUT,
	ERR_PROBE_FAILED,
	ERR_CREATEFILE,
	ERR_SETUPCOMM,
	ERR_GETCOMMSTATE,
	ERR_SETCOMMSTATE,
	ERR_GETCOMMTIMEOUT,
	ERR_SETCOMMTIMEOUT,
	ERR_PURGECOMM,
	ERR_LOADLIBRARY,
	ERR_FREELIBRARY,
	ERR_TIGLUSB_VERSION,
	ERR_TIGLUSB_OPEN,
	ERR_TIGLUSB_FLUSH,
    ERR_TIGLUSB_RESET,
	ERR_BYTE_LOST,
	ERR_FINDWINDOW,
	ERR_OPENFILEMAPPING,
	ERR_MAPVIEWOFFILE,

	ERR_OPEN,
	ERR_IOCTL,
	ERR_OPEN_PIPE,
	ERR_CLOSE_PIPE,

	ERR_IPC_KEY,
	ERR_SHM_GET,
	ERR_SHM_ATTACH,
	ERR_SHM_DETACH,
	ERR_SHM_RMID,

} TiCableError;

#endif
