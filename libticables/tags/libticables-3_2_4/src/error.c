/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

TicableLinkCable *tcl;

/* 
   This function put in err_msg the error message corresponding to the 
   error code.
   If the error code has been handled, the function returns 0 else it 
   propagates the error code by returning it.
*/
TIEXPORT int TICALL ticable_get_error(int err_num, char *error_msg)
{
  switch(err_num)
    {
    case ERR_ROOT: 
      strcpy(error_msg, _("Unable to open parallel/serial port. Check that you have required permissions (super user privileges). Else, you will need to use a kernel module."));
      break;
    case ERR_SND_BIT_TIMEOUT: 
      strcpy(error_msg, _("Send bit time out."));
      break;
    case ERR_RCV_BIT_TIMEOUT: 
      strcpy(error_msg, _("Receive bit time out."));
      break;
    case ERR_OPEN_SER_DEV:
      strcpy(error_msg, _("Unable to open serial device. Check that you have required rights on the node or the device is not locked by another application (modem ?)."));
      break;
    case ERR_SND_BYT: 
      strcpy(error_msg, _("Unable to send a byte."));
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
      strcpy(error_msg, _("CreateFile error. Check that the device is not locked by another application (modem ?)"));
      break;
    case ERR_OPEN_COM_PORT: 
      strcpy(error_msg, _("Unable to open COM port (Win32)."));
      break;
    case ERR_READ_FILE: 
      strcpy(error_msg, _("ReadFile failed."));
      break;
    case ERR_OPEN_TIDEV_DEV:
      strcpy(error_msg, _("Unable to open node on kernel module. Check that you have required rights on the node and/or your kernel module is loaded."));
      break;
    case ERR_VT0_ALREADY_USED:
      strcpy(error_msg, _("The virtual link #0 is already used by another application."));
      break;
    case ERR_VT1_ALREADY_USED:
      strcpy(error_msg, _("The virtual link #1 is already used by another application."));
      break;
    case ERR_OPEN_PIPE:
      strcpy(error_msg, _("Unable to open pipes for virtual linking. Check that you have permissions to create a pipe in the /tmp directory."));
      break;
    case ERR_PIPE_FCNTL:
      strcpy(error_msg, _("Unable to modify the pipe characteristics."));
      break;
    case ERR_OPP_NOT_AVAIL:
      strcpy(error_msg, _("Trying to communicate without correspondent. Did you launch the emulator ?"));
      break;
    case ERR_CLOSE_PIPE:
      strcpy(error_msg, _("Unable to close pipes."));
      break;
    case ERR_BYTE_LOST: 
      strcpy(error_msg, _("A byte have been lost."));
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
      strcpy(error_msg, _("VTi seems to be not launched yet (Win32)."));
      break;
    case ERR_USB_DEVICE_CMD:
      strcpy(error_msg, _("DeviceIoControl function error (Win32): unable to send USB request."));
      break;
    case ERR_GETPROCADDRESS:
      strcpy(error_msg, _("GetProcAddress function error (Win32): unable to load a DLL symbol."));
      break;
    case ERR_DLPORTIO_NOT_FOUND:
      strcpy(error_msg, _("DLPortIO driver & library not found. The DLPortIO kernel driver is required for home-made parallel/serial link cables under Windows NT4/2000/XP. You can get this driver on the TiLP web-page at <http://lpg.ticalc.org/prj_tilp/download.html>.."));
      break;
    case ERR_FREELIBRARY:
      strcpy(error_msg, _("FreeLibrary function error (Win32): unable to release the DLL."));
      break;
    case ERR_USB_OPEN:
#ifndef __MACOSX__
      strcpy(error_msg, _("Unable to open the USB device. Check that you have required rights on the node and/or your driver is loaded."));
#else
      strcpy(error_msg, _("Unable to open the USB device. Your cable is not connected or stalled."));
#endif
      break;
    case ERR_USB_INIT:
#ifndef __MACOSX__
      strcpy(error_msg, _("Error occurred while initializing the libusb."));
#else
      strcpy(error_msg, _("Unable to initialize the USB device. Your cable is probably not connected."));
#endif
      break;
    case ERR_IOCTL:
      strcpy(error_msg, _("IOCTL error. Check that you have required rights on the node and/or your kernel module is loaded."));
      break;
    case ERR_NO_RESOURCES:
      strcpy(error_msg, _("No I/O resource available ! Check for:\n- I/O permissions (parallel/serial link cable)\n- device driver (parallel/serial cable under Win NT4/2000/XP or USB)\n- kernel module (parallel/serial or USB under Linux\n- ..."));
      break;
    case ERR_INVALID_PORT:
      strcpy(error_msg, _("Invalid port: try to use an I/O port device which is incompatible with the link cable."));
      break;
    case ERR_PROBE_FAILED:
      strcpy(error_msg, _("Probing has failed."));
      break;
    case ERR__IPC_KEY:
      strcpy(error_msg, _("Unable to get a unique IPC (Inter Process Communication) key. Check that you have enough resources for allocating a shared memory segment."));
      break;
    case ERR__SHM_GET:
      strcpy(error_msg, _("Unable to open a shared memory segment. Do you have any resources ?"));
      break;
    case ERR__SHM_ATT:
      strcpy(error_msg, _("Unable to attach shared memory segment. Too many attachements ?"));
      break;
    case ERR__SHM_DTCH:
      strcpy(error_msg, _("Unable to detach the shared memory segment. Is segment locked ?"));
      break;
    case ERR__SHM_RMID:
      strcpy(error_msg, _("Unable to destroy the shared memory segment. Check that no applications are still attached on it."));
      break;
    default:
      strcpy(error_msg, _("Error code not found in the list.\nThis is a bug. Please report it.\n."));
      return err_num;
      break;
  }
  if(tcl != NULL)
	tcl->close();	// Close the connection
  
  return 0;
}

