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

#ifndef __TICABLE_ERRCODES__
#define __TICABLE_ERRCODES__

/* Error codes ERR_...  */
/* Error codes must begin at 0 up to 255 */
#define ERR_ABORT           -1	// Operation aborted
#define ERR_NO_ERROR         0	// No error (returns 0)
#define ERR_OPEN_SER_DEV     1
#define ERR_OPEN_SER_COMM    2
#define ERR_WRITE_ERROR      3
#define ERR_WRITE_TIMEOUT    4
#define ERR_READ_ERROR       5
#define ERR_READ_TIMEOUT     6
#define ERR_BYTE_LOST        7
#define ERR_CREATE_FILE      8
#define ERR_OPEN_TIDEV       9
#define ERR_ROOT            10	// Root permissions required
#define ERR_PROBE_FAILED    11
#define ERR_OPEN_PIPE       12
#define ERR_CLOSE_PIPE      13
#define ERR_OPP_NOT_AVAIL   14
#define ERR_IOCTL           15
#define ERR_SETUP_COMM      16
#define ERR_SET_COMMSTATE   17
#define ERR_GET_COMMSTATE   18
#define ERR_GET_COMMTIMEOUT 19
#define ERR_SET_COMMTIMEOUT 20
#define ERR_FLUSH_COMM      21
#define ERR_OPEN_USB_DEV    22
#define ERR_USB_DEVICE_CMD  23
#define ERR_LIBUSB_INIT     25
#define ERR_LIBUSB_OPEN     26
#define ERR_LIBUSB_RESET    27
#define ERR_FREELIBRARY     28
#define ERR_IPC_KEY         29
#define ERR_SHM_GET         	30
#define ERR_SHM_ATTACH      	31
#define ERR_SHM_DETACH      	32
#define ERR_SHM_RMID        	33
#define ERR_OPEN_FILE_MAP   	34
#define ERR_ILLEGAL_ARG     	35
#define ERR_NO_RESOURCES    	36
#define ERR_INVALID_PORT    	37
#define ERR_PORTTALK_NOT_FOUND	38
#define ERR_TIGLUSB_VERSION 	39

#define ERR_NODE_NONEXIST	40
#define ERR_NODE_PERMS		41
#define ERR_NOTLOADED		42
#define ERR_NOTMOUNTED		43

#endif
