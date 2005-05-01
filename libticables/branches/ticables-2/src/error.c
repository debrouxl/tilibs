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

#include <stdio.h>
#include <string.h>
#ifdef __WIN32__
#include <windows.h>
#endif
#include <glib.h>

#include "gettext.h"
#include "export.h"
#include "error.h"
#include "logging.h"

/**
 * ticables_error_get:
 * @number: error number (see error.h for list).
 * @message: a newly allocated string which contains corresponding error *message.
 *
 * Attempt to match the message corresponding to the error number. The returned
 * string must be freed when no longer needed.
 * The error message has the following format:
 * - 1: the error message
 * - 2: the cause(s), explanations on how to fix it
 * - 3: the error returned by the system
 *
 * Return value: 0 if error has been caught, the error number otherwise (propagation).
 **/
TIEXPORT int TICALL ticables_error_get(TiCableError number, char **message)
{
	char *tmp;

	g_assert (message != NULL);

	switch(number)
	{
	case ERR_BUSY:
		*message = g_strconcat(
    		_("Msg: Link cable is busy."),
    		"\n",
    		 _("Cause: a transfer is in progress."),
			 NULL);
    	break;
/*
  	case ERR_CREATEFILE:
		*message = g_strconcat(
    		_("Msg: Unable to open serial device."),
    		"\n",
    		 _("Cause: check your permissions on the node. Check that the device is not locked by another application (modem ?)."),
			 NULL);
    	break;
  	case ERR_OPEN_SER_COMM:
    		strcpy(error_msg, _("Msg: Unable to open COM port."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Check that the device is not used/locked by another application (modem ?)."));
    	break;

  	case ERR_WRITE_ERROR:
    		strcpy(error_msg, _("Msg: Error occured while writing to the device."));
    	break;

  	case ERR_WRITE_TIMEOUT:
    		strcpy(error_msg, _("Msg: Timeout occured while writing to the device."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Check that your link cable is plugged and/or the calculator is ready."));
    	break;

  	case ERR_READ_ERROR:
    		strcpy(error_msg, _("Msg: Error occured while reading to the device."));
    	break;

  	case ERR_READ_TIMEOUT:
    		strcpy(error_msg, _("Msg: Timeout occured while reading to the device."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Check that your link cable is plugged and/or the calculator is ready."));
    	break;

  	case ERR_BYTE_LOST:
    		strcpy(error_msg, _("Msg: A uint8_t have been lost."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Application too slow."));
    	break;

  	case ERR_CREATE_FILE:
    		strcpy(error_msg, _("Msg: CreateFile error."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Check that the device is not used/locked by another application (modem ?)."));
    	break;

  	case ERR_OPEN_TIDEV:
    		strcpy(error_msg, _("Msg: Unable to open a node in /dev."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause:Check that the node exists. Check your devfs. Check that you have installed the driver."));
    	break;

  	case ERR_ROOT:
#if defined(__LINUX__) || defined(__BSD__)
	    	strcpy(error_msg, _("Msg: Unable to use parallel/serial port: access refused."));
	    	strcat(error_msg, "\n");
	    	strcat(error_msg, _("Cause: Check that you have needed permissions (super user privileges). Else, you will need to use a kernel module (tipar/tiser)."));
#elif defined(__WIN32__)
	    	strcpy(error_msg, _("Msg: Unable to use parallel/serial port: access refused."));
	    	strcat(error_msg, "\n");
	    	strcat(error_msg, _("Cause: Is PortTalk installed/started ? You may have to install TiLP with administrator privileges."));
#endif
    	break;

  	case ERR_PROBE_FAILED:
	    	strcpy(error_msg, _("Msg: No link cable has been found on the scanned port."));
	    	strcat(error_msg, "\n");
	    	strcat(error_msg, _("Cause: ??"));
    	break;

  	case ERR_OPEN_PIPE:
    		strcpy(error_msg, _("Msg: Unable to open pipes for virtual linking."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Check that you have permissions to create a pipe in the /tmp directory."));
    	break;

  	case ERR_CLOSE_PIPE:
    		strcpy(error_msg, _("Msg: Unable to close pipes."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: System error ?!"));
    	break;

  	case ERR_OPP_NOT_AVAIL:
    		strcpy(error_msg, _("Msg: CreateFileMapping error."));
    		strcat(error_msg, "\n");
    		strcat(error_msg, _("Cause: Trying to communicate without correspondent. Did you launch the emulator before running TiLP ?"));
    	break;

  case ERR_IOCTL:
    strcpy(error_msg, _("Msg: IOCTL error."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that you have required rights on the node and/or your kernel module is loaded."));
    break;

  case ERR_SETUP_COMM:
    strcpy(error_msg, _("Msg: SetupComm error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error."));
    break;

  case ERR_SET_COMMSTATE:
    strcpy(error_msg, _("Msg: SetCommState error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error."));
    break;

  case ERR_GET_COMMSTATE:
    strcpy(error_msg, _("Msg: GetCommState error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error."));
    break;

  case ERR_GET_COMMTIMEOUT:
    strcpy(error_msg, _("Msg: GetCommTimeouts error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error."));
    break;

  case ERR_SET_COMMTIMEOUT:
    strcpy(error_msg, _("Msg: SetCommTimeouts error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error."));
    break;

  case ERR_FLUSH_COMM:
    strcpy(error_msg, _("Msg: Error while flushing the buffer(s)."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal Win32 error"));
    break;

  case ERR_USB_DEVICE_CMD:
    strcpy(error_msg, _("Msg: IOCTL code unsuccessful."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Internal device driver error."));
    break;

  case ERR_OPEN_USB_DEV:
#if defined(__LINUX__) || defined(__BSD__)
    strcpy(error_msg, _("Msg: Unable to open the USB device."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that you have required rights on the node and/or your driver is loaded."));
#elif defined(__WIN32__)
    strcpy(error_msg, _("Msg: Unable to open the USB device."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check your SilverLink is correcly installed in the Windows' Control Panel. The ticables library need a specific driver for this cable. You will find it on http://lpg.ticalc.org/prj_usb."));
#elif defined(__MACOSX__)
    strcpy(error_msg, _("Msg: Unable to open the USB device."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Your cable is not connected or stalled."));
#endif
    break;

  case ERR_LIBUSB_INIT:
#ifndef __MACOSX__
    strcpy(error_msg,
	   _("Msg: Error occurred while initializing the libusb."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that your cable is connected or not stalled. Check your libusb and usbfs, too."));
#else
    strcpy(error_msg, _("Msg: Unable to initialize the USB device."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Your cable is probably not connected."));
#endif
    break;

  case ERR_LIBUSB_OPEN:
    strcpy(error_msg, _("Msg: Unable to open/find a USB device."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that your cable is connected or not stalled. Check you rlibusb and usbfs, too."));
    break;

  case ERR_LIBUSB_RESET:
    strcpy(error_msg, _("Msg: Error while reseting USB endpoints."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _("Cause: Your cable may be stalled. Unplug and replug it !."));
    break;

  case ERR_FREELIBRARY:
    strcpy(error_msg,
	   _("Msg: FreeLibrary error. Unable to release the DLL."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: ??"));
    break;

  case ERR_IPC_KEY:
    strcpy(error_msg,
	   _
	   ("Msg: Unable to get a unique IPC (Inter Process Communication) key."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that you have enough resources for allocating a shared memory segment."));
    break;

  case ERR_SHM_GET:
    strcpy(error_msg, _("Msg: Unable to open a shared memory segment."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Do you have any resources ?"));
    break;

  case ERR_SHM_ATTACH:
    strcpy(error_msg, _("Msg: Unable to attach shared memory segment."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Too many attachements ?"));
    break;

  case ERR_SHM_DETACH:
    strcpy(error_msg,
	   _("Msg: Unable to detach the shared memory segment."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: Is segment locked ?"));
    break;

  case ERR_SHM_RMID:
    strcpy(error_msg,
	   _("Msg: Unable to destroy the shared memory segment."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: Check that no applications are still attached on it."));
    break;

  case ERR_OPEN_FILE_MAP:
    strcpy(error_msg, _("Msg: Unable to open a file mapping."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: VTi seems to be not launched yet."));
    break;

  case ERR_ILLEGAL_ARG:
    strcpy(error_msg, _("Msg: Illegal operation or argument."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: A bug in TiLP, mail to: roms@tilp.info."));
    break;

  case ERR_NO_RESOURCES:
    strcpy(error_msg, _("Msg: No I/O resource available !"));
    strcat(error_msg, "\n");
#ifdef __WIN32__
    strcat(error_msg, _("Cause: several ways. Check for:\n- PortTalk device driver (parallel/serial cable under Win NT4/2000/XP)\n- TiglUsb device driver (SilverLink)."));
#else
    strcat(error_msg, _
	   ("Cause: several ways. Check for:\n- I/O permissions (parallel/serial link cable)\n- kernel module (tipar/tiser/tiusb)."));
#endif
    break;

  case ERR_INVALID_PORT:
    strcpy(error_msg,
	   _
	   ("Msg: Invalid port: try to use an I/O port device which is incompatible with the link cable."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: It's due to a misuse of library, probably a TiLP bug. Mail to: roms@tilp.info"));
    break;

  case ERR_PORTTALK_NOT_FOUND:
    strcpy(error_msg, _("Msg: PortTalk driver and/or library not found."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: TiLP should start this driver automagically through the SCM. Check that it's loaded and TiLP as been installed with admin privileges !"));
    break;

  case ERR_TIGLUSB_VERSION:
    strcpy(error_msg,
	   _
	   ("Msg: SilverLink library has an insufficient version number."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: the SilverLink driver currently installed has a wrong version. Please upgrade !"));
    break;
    
    	case ERR_NODE_NONEXIST:
    	strcpy(error_msg, _("Msg: the node does not exists."));
    	strcat(error_msg, "\n");
    	strcat(error_msg, _("Cause: the node has not been created by the module."));
    	break;
    	
	case ERR_NODE_PERMS:
	strcpy(error_msg, _("Msg: you don't have permissions for reading/writing the node."));
    	strcat(error_msg, "\n");
    	strcat(error_msg, _("Cause: others does not have r/w access or you are not in the group."));
	break;
	
	case ERR_NOTLOADED:
	strcpy(error_msg, _("Msg: module not loaded."));
    	strcat(error_msg, "\n");
    	strcat(error_msg, _("Cause: the module does not exist or has not been loaded."));
	break;
	
	case ERR_NOTMOUNTED:
	strcpy(error_msg, _("Msg: the usb pseudo-filesystem is not mounted."));
    	strcat(error_msg, "\n");
    	strcat(error_msg, _("Cause: your kernel does not have usbfs support or it is not mounted by your /etc/fstab."));
	break;

    case ERR_TIGLUSB_RESET:
	strcpy(error_msg, _("Msg: failed to reset USB endpoints."));
    	strcat(error_msg, "\n");
    	strcat(error_msg, _("Cause: your SilverLink cable may be stalled. Try to unplug/plug it."));
	break;
*/
  	default:
		if (GetLastError()) 
	{
    		LPVOID lpMsgBuf;
			char buf[256];

    		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) & lpMsgBuf, 0, NULL);
    		fprintf(stdout,  "\n");
    		fprintf(stdout,  "System: ");
    		//snprintf(buf, 256, "GetLastError = %i -> ", GetLastError());
    		sprintf(buf, "GetLastError = %li -> ", GetLastError());
    		fprintf(stdout,  buf);
    		fprintf(stdout,  lpMsgBuf);
    		fprintf(stdout,  "\n");
	}
		return number;
    break;
  	}

	/*
#ifndef __WIN32__
  	if (errno != 0) 
	{
		gchar *str;
		tmp = *message;
		
		str = g_strdup-printf(" (errno = %i)", errno);
		*message = g_strconcat(tmp, "\n", "System: ", strerror(errno), str, "\n", NULL);
		g_free(tmp);
		g_fre(str);
  	}
#else
  	if (GetLastError()) {
    		LPVOID lpMsgBuf;

    		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) & lpMsgBuf, 0, NULL);
    		strcat(error_msg, "\n");
    		strcat(error_msg, "System: ");
    		//snprintf(buf, 256, "GetLastError = %i -> ", GetLastError());
    		sprintf(buf, "GetLastError = %li -> ", GetLastError());
    		strcat(error_msg, buf);
    		strcat(error_msg, lpMsgBuf);
    		strcat(error_msg, "\n");
  	}
#endif
*/

  	// don't use GLib allocator
	tmp = strdup(*message);
	g_free(*message);
	*message = tmp;

	return 0;
}
