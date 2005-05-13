/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
    ERR_NO_ERROR = 0,		// No error (returns 0)
    
    ERR_BUSY,
    ERR_ILLEGAL_ARG,
    
    ERR_WRITE_ERROR,
    ERR_WRITE_TIMEOUT,
    ERR_READ_ERROR,
    ERR_READ_TIMEOUT,
    ERR_PROBE_FAILED,
    
    ERR_GRY_CREATEFILE,
    ERR_GRY_SETUPCOMM,
    ERR_GRY_GETCOMMSTATE, //10
    ERR_GRY_SETCOMMSTATE,
    ERR_GRY_GETCOMMTIMEOUT,
    ERR_GRY_SETCOMMTIMEOUT,
    ERR_GRY_PURGECOMM,
    ERR_GRY_SETCOMMMASK,
    
    ERR_SLV_LOADLIBRARY,
    ERR_SLV_FREELIBRARY,
    ERR_SLV_VERSION,
    ERR_SLV_OPEN,
    ERR_SLV_FLUSH, //20
    ERR_SLV_RESET,
    
    ERR_VTI_FINDWINDOW,	// open
    ERR_VTI_OPENFILEMAPPING,
    ERR_VTI_MAPVIEWOFFILE,
    
    ERR_TIE_OPENFILEMAPPING,	//open
    ERR_TIE_MAPVIEWOFFILE,
    
    ERR_VTI_IPCKEY,	//open
    ERR_VTI_SHMGET,
    ERR_VTI_SHMAT,
    ERR_VTI_SHMDT, //30	//close
    ERR_VTI_SHMCTL,
    
    ERR_TIE_OPEN,	//open
    ERR_TIE_CLOSE,
    
    ERR_GRY_OPEN,
    ERR_GRY_IOCTL,
    
    ERR_LIBUSB_OPEN,
    ERR_LIBUSB_CLAIM,
    ERR_LIBUSB_CONFIG,
    ERR_LIBUSB_RESET,	
    
    ERR_PORTTALK_NOT_FOUND, //40
    ERR_ROOT,    
    ERR_TTDEV,
    ERR_PPDEV,
    ERR_USBFS,

    ERR_TTY_OPEN,
    ERR_TTY_IOCTL,

    ERR_PPT_OPEN,
    ERR_PPT_IOCTL, //48

} CableError;

#endif
