/*  tilp - link program for TI calculators
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

#include <stdio.h>
#include "str.h"

#include "intl.h"
#include "cabl_err.h"
#include "export.h"
#include "cabl_def.h"

LINK_CABLE *tcl;

/* 
   This function put in err_msg the error message corresponding to the 
   error code.
   If the error code has been handled, the function returns 0 else it 
   propagates the error code by returning it.
*/
DLLEXPORT
int DLLEXPORT2 ticable_get_error(int err_num, char *error_msg)
{
  switch(err_num)
    {
      /*    
	    case ERR_ABORT: 
	    strcpy(error_msg, _("Transfer aborted");
	    break;
      */
    case ERR_ROOT: 
      strcpy(error_msg, _("Unable to open parallel/serial port. You must be 'root' for this (Linux).."));
      break;
    case ERR_SND_BIT_TIMEOUT: 
      strcpy(error_msg, _("Send bit time out.."));
      break;
    case ERR_RCV_BIT_TIMEOUT: 
      strcpy(error_msg, _("Receive bit time out.."));
      break;
    case ERR_OPEN_SER_DEV:
      strcpy(error_msg, _("Unable to open serial device.."));
      break;
    case ERR_SND_BYT: 
      strcpy(error_msg, _("Unable to send a byte.."));
      break;
    case ERR_RCV_BYT: 
      strcpy(error_msg, _("Unable to receive a byte."));
      break;
    case ERR_RCV_BYT_TIMEOUT: 
      strcpy(error_msg, _("Receive byte timeout."));
      break;
    case ERR_SND_BYT_TIMEOUT: 
      strcpy(error_msg, _("Send byte timeout."));
      break;
    case ERR_CREATE_FILE: 
      strcpy(error_msg, _("CreateFile error (Win32)."));
      break;
    case ERR_OPEN_COM_PORT: 
      strcpy(error_msg, _("Unable to open COM port (Win32)."));
      break;
    case ERR_READ_FILE: 
      strcpy(error_msg, _("ReadFile failed."));
      break;
    case ERR_OPEN_TIDEV_DEV:
      strcpy(error_msg, _("Unable to open a 'tidev' character device."));
      break;
    case ERR_VT0_ALREADY_USED:
      strcpy(error_msg, _("The virtual link #0 is already used by another application."));
      break;
    case ERR_VT1_ALREADY_USED:
      strcpy(error_msg, _("The virtual link #1 is already used by another application."));
      break;
    case ERR_OPEN_PIPE:
      strcpy(error_msg, _("Unable to open a pipe for virtual linking."));
      break;
    case ERR_PIPE_FCNTL:
      strcpy(error_msg, _("Unable to modify the pipe characteristics."));
      break;
    case ERR_OPP_NOT_AVAIL:
      strcpy(error_msg, _("Trying to communicate without correspondent."));
      break;
    case ERR_CLOSE_PIPE:
      strcpy(error_msg, _("Unable to close a pipe."));
      break;
    case ERR_BYTE_LOST: 
      strcpy(error_msg, _("A byte might have been lost in 'check_port'."));
    break;
    case ERR_ILLEGAL_OP: 
      strcpy(error_msg, _("Illegal operation or argument."));
      break;
    case ERR_FLUSH:
      strcpy(error_msg, _("Error while flushing the buffer."));
      break;
    case ERR_SET_COMMSTATE:
      strcpy(error_msg, _("SetCommState error (Win32)."));
      break;
    case ERR_GET_COMMSTATE:
      strcpy(error_msg, _("GetCommState error (Win32)."));
      break;
    case ERR_SETUP_COMM:
      strcpy(error_msg, _("SetupComm error (Win32)."));
      break;
    case ERR_GET_COMMTIMEOUT:
      strcpy(error_msg, _("GetCommTimeouts error (Win32)."));
      break;
    case ERR_SET_COMMTIMEOUT:
      strcpy(error_msg, _("SetCommTimeouts error (Win32)."));
      break;
    case ERR_OPEN_FILE_MAP:
      strcpy(error_msg, _("VTi seems to be not launched (Win32)."));
      break;
	case ERR_USB_DEVICE_CMD:
	  strcpy(error_msg, _("DeviceIoControl function error (Win32): unable to send USB request."));
	  break;
	case ERR_GETPROCADDRESS:
		strcpy(error_msg, _("GetProcAddress function error (Win32): unable to load a DLL symbol."));
		break;
	case ERR_DLPORTIO_NOT_FOUND:
		strcpy(error_msg, _("Currently running on WinNT. The DLPortIO kernel driver is required for home-made parallel/serial link cables. You can get this driver on the TiLP web-page <http://lpg.ticalc.org/prj_tilp/download.html>.."));
		break;
	case ERR_FREELIBRARY:
		strcpy(error_msg, _("FreeLibrary function error (Win32): unable to release the DLL."));
		break;

    default:
      strcpy(error_msg, _("Error code not found in the list.\nThis is a bug. Please report it.\n."));
      return err_num;
//      break;
  }
  if(tcl != NULL)
	tcl->close_port();	// Close the connection
  
  return 0;
}

