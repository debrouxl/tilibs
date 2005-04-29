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

#ifndef __TICABLES_ERRCODES__
#define __TICABLES_ERRCODES__

/* Error codes must begin at 0 up to 255 */

typedef enum
{
    ERR_ABORT = -1,			// Operation aborted
    ERR_NO_ERROR = 0,		// No error (returns 0)
    ERR_OPEN_SER_DEV,
    ERR_OPEN_SER_COMM,    
    ERR_WRITE_ERROR,
    ERR_WRITE_TIMEOUT,
    ERR_READ_ERROR,
    ERR_READ_TIMEOUT,
    ERR_BYTE_LOST,        
    ERR_CREATE_FILE,      
    ERR_OPEN_TIDEV,       
    ERR_ROOT,            	// Root permissions required
    ERR_PROBE_FAILED,    
    ERR_OPEN_PIPE,       
    ERR_CLOSE_PIPE,      
    ERR_OPP_NOT_AVAIL,   
    ERR_IOCTL,           
    ERR_SETUP_COMM,      
    ERR_SET_COMMSTATE,   
    ERR_GET_COMMSTATE,   
    ERR_GET_COMMTIMEOUT, 
    ERR_SET_COMMTIMEOUT, 
    ERR_FLUSH_COMM,      
    ERR_OPEN_USB_DEV,    
    ERR_USB_DEVICE_CMD,  
    ERR_LIBUSB_INIT,     
    ERR_LIBUSB_OPEN,     
    ERR_LIBUSB_RESET,    
    ERR_FREELIBRARY,     
    ERR_IPC_KEY,         
    ERR_SHM_GET,         	
    ERR_SHM_ATTACH,      	
    ERR_SHM_DETACH,      	
    ERR_SHM_RMID,        	
    ERR_OPEN_FILE_MAP,   	
    ERR_ILLEGAL_ARG,     	
    ERR_NO_RESOURCES,    	
    ERR_INVALID_PORT,    	
    ERR_PORTTALK_NOT_FOUND,	
    ERR_TIGLUSB_VERSION, 	

    ERR_NODE_NONEXIST,	
    ERR_NODE_PERMS,		
    ERR_NOTLOADED,		
    ERR_NOTMOUNTED,		

    ERR_TIGLUSB_FLUSH,   
    ERR_TIGLUSB_RESET,   

	ERR_LIB_INIT,
	ERR_BUSY,
} TiCableError;

#endif
