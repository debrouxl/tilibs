/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32__
#include <windows.h>
#endif
#include <glib.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ticables.h"
#include "gettext.h"
#include "export1.h"
#include "error.h"
#include "logging.h"

/**
 * ticables_error_get:
 * @number: error number (see error.h for list).
 * @message: a newly GLib allocated string which contains corresponding error *message.
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
int TICALL ticables_error_get(int number, char **message)
{
	if (message == nullptr)
	{
		ticables_critical("ticables_error_get(NULL)\n");
		return number;
	}

	switch(number)
	{
		case ERR_BUSY:
			*message = g_strconcat(
			_("Msg: link cable is busy."),
			"\n",
			_("Cause: a transfer is in progress."),
			NULL);
		break;

		case ERR_ROOT:
			*message = g_strconcat(
			_("Msg: unable to use parallel/serial port: access refused."),
			"\n",
			_("Cause: you are not running program as root or setuid root."),
			NULL);
		break;

		case ERR_TTDEV:
			*message = g_strconcat(
			_("Msg: unable to use serial (/dev/ttySx) device."),
			"\n",
			_("Cause: the /dev/ttySx node doesn't exist or you don't have required permissions. " \
				"How to grant permission to devices to the user depends on the " \
				"distribution, it may involve adding the user to a group such " \
				"as 'dialout' or 'uucp'. This can be done by editing /etc/group " \
				"and add the username after the ':'. Example: dialout:x:20:romain " \
				""
				),
			NULL);
		break;

		case ERR_PPDEV:
			*message = g_strconcat(
			_("Msg: unable to use parport (/dev/parportX) device."),
			"\n",
			_("Cause: the /dev/parportX node doesn't exist or you don't have required permissions. "
				"How to grant permission to devices to the user depends on the " \
				"distribution, it may involve adding the user to a group such " \
				"as 'lp'. This can be done by editing /etc/group " \
				"and add the username after the ':'. Example: lp:x:7:romain " \
				""
				),
			NULL);
		break;

		case ERR_USBFS:
			*message = g_strconcat(
			_("Msg: unable to use USB pseudo file system."),
			"\n",
			_("Cause: the USB filesystem is not mounted or " \
				"cannot access USB device because current user doesn't have the necessary " \
				"permissions. How to grant permission to devices to the user depends on the " \
				"distribution; it may involve adding udev and/or pam_console configuration " \
				"files, and you may in addition have to add your user to a group such " \
				"as 'usb' or set-up libusb mounting parameters." \
				"If packages of TiLP and its libraries for your distribution exist, " \
				"it is recommended to use those, as everything which can be configured " \
				"automatically will be set up for you. Otherwise, manual setup steps, if any, are " \
				"explained in the CONFIG file shipped with the ticables2 library. " \
				""
				),
			NULL);
		break;

		case ERR_ILLEGAL_ARG:
			*message = g_strconcat(
			_("Msg: illegal operation or argument."),
			"\n",
			_("Cause: the program which uses this library is buggy. Fire-up the developer!"),
			NULL);
		break;

		case ERR_DHA_NOT_FOUND:
			*message = g_strconcat(
			_("Msg: DhaHelper driver and/or library not found."),
			"\n",
			_("Cause: the driver may have been not started yet; it may have been uninstalled; the library has been installed without administrator privileges."),
			NULL);
		break;

		case ERR_RWP_NOT_FOUND:
			*message = g_strconcat(
			_("Msg: RwPorts driver and/or library not found."),
			"\n",
			_("Cause: the driver may have been not started yet; it may have been uninstalled; the library has been installed without administrator privileges."),
			NULL);
		break;

		case ERR_WRITE_ERROR:
			*message = g_strdup(
			_("Msg: error occurred while writing to the device."));
		break;

		case ERR_WRITE_TIMEOUT:
			*message = g_strconcat(
			_("Msg: timeout occurred while writing to the device."),
			"\n",
			_("Cause: check that link cable is plugged and/or the calculator is ready."),
			NULL);
		break;

		case ERR_READ_ERROR:
			*message = g_strdup(
			_("Msg: error occurred while reading from the device."));
		break;

		case ERR_READ_TIMEOUT:
			*message = g_strconcat(
			_("Msg: timeout occurred while reading from the device."),
			"\n",
			_("Cause: check that link cable is plugged and/or the calculator is ready."),
			NULL);
		break;

		case ERR_PROBE_FAILED:
			*message = g_strconcat(
			_("Msg: the probing of device failed."),
			"\n",
			_("Cause: internal error. If you get it, this is a bug!"),
			NULL);
		break;

		case ERR_GRY_CREATEFILE:
		case ERR_GRY_SETUPCOMM:
		case ERR_GRY_GETCOMMSTATE:
		case ERR_GRY_SETCOMMSTATE:
		case ERR_GRY_GETCOMMTIMEOUT:
		case ERR_GRY_SETCOMMTIMEOUT:
		case ERR_GRY_PURGECOMM:
		case ERR_GRY_SETCOMMMASK:
			*message = g_strconcat(
			_("Msg: unable to open serial port."),
			"\n",
			_("Cause: check that device is not used/locked by another application."),
			NULL);
		break;

		case ERR_VTI_FINDWINDOW:
		case ERR_VTI_OPENFILEMAPPING:
		case ERR_VTI_MAPVIEWOFFILE:
			*message = g_strconcat(
			_("Msg: failed to get VTI handle."),
			"\n",
			_("Cause: VTI may not be running. VTI must be launched before."),
			NULL);
		break;

		case ERR_VTI_IPCKEY:
		case ERR_VTI_SHMGET:
		case ERR_VTI_SHMAT:
		case ERR_VTI_SHMDT:
		case ERR_VTI_SHMCTL:
			*message = g_strconcat(
			_("Msg: unable to get a unique IPC (Inter Process Communication) key."),
			"\n",
			_("Cause: check that you have enough resources for allocating a shared memory segment."),
			NULL);
		break;

		case ERR_TIE_OPEN:
		case ERR_TIE_CLOSE:
		*message = g_strconcat(
			_("Msg: unable to open/close pipes for virtual linking."),
			"\n",
			_("Cause: check that you have permissions to create a pipe in the /tmp directory."),
			NULL);
		break;

		case ERR_TTY_OPEN:
		case ERR_GRY_OPEN:
			*message = g_strconcat(
			_("Msg: unable to open serial device."),
			"\n",
			_("Cause: check that you have permissions on /dev/ttySx device. Check that device is not locked."),
			NULL);
		break;

		case ERR_TTY_IOCTL:
		case ERR_GRY_IOCTL:
			*message = g_strconcat(
			_("Msg: unable to issue a specific command on serial device."),
			"\n",
			_("Cause: check that you have permissions on /dev/ttySx device. Check that device is not locked."),
			NULL);
		break;

		case ERR_PPT_OPEN:
			*message = g_strconcat(
			_("Msg: unable to open parallel device."),
			"\n",
			_("Cause: check that you have permissions on /dev/parportX device. Check that device is not locked."),
			NULL);
		break;

		case ERR_PPT_IOCTL:
			*message = g_strconcat(
			_("Msg: unable to issue a specific command on parallel device."),
			"\n",
			_("Cause: check that you have permissions on /dev/parportX device. Check that device is not locked."),
			NULL);
		break;

		case ERR_LIBUSBWIN32_NOT_PRESENT:
			*message = g_strconcat(
			_("Msg: unable to find the libusb-win32 driver."),
			"\n",
			_("Cause: the driver is not present and/or installed."),
			NULL);
		break;

		case ERR_LIBUSB_OPEN:
		case ERR_LIBUSB_CLAIM:
		case ERR_LIBUSB_CONFIG:
			*message = g_strconcat(
			_("Msg: failed to open the USB device."),
			"\n",
			_("Cause: Check that the USB cable is plugged in and that the calculator is turned ON! Also, check libusb and usbfs for valid permissions."),
			NULL);
		break;

		case ERR_LIBUSB_RESET:
			*message = g_strconcat(
			_("Msg: unable to reset USB device."),
			"\n",
			_("Cause: Check that cable is connected or not stalled. Try to unplug/plug it."),
			NULL);
		break;

		case ERR_NOT_OPEN:
			*message = g_strconcat(
			_("Msg: attempting to use a cable which has not been open before."),
			"\n",
			_("Cause: Internal error."),
			NULL);
		break;

		case ERR_NO_CABLE:
			*message = g_strconcat(
			_("Msg: no cable found."),
			"\n",
			_("Cause: probing failed or invalid parameters."),
			NULL);
		break;

		case ERR_NO_LIBRARY:
			*message = g_strconcat(
			_("Msg: DLL not found."),
			"\n",
			_("Cause: maybe you renamed the DLL ?"),
			NULL);
		break;

		case ERR_RAW_IO_UNSUPPORTED:
			*message = g_strconcat(
			_("Msg: this cable does not support raw I/O."),
			"\n",
			_("Cause: unsupported operation type on cable."),
			NULL);
		break;

		case ERR_TCPC_OPEN:
		case ERR_TCPC_CLOSE:
		case ERR_TCPS_OPEN:
		case ERR_TCPS_CLOSE:
		*message = g_strconcat(
			_("Msg: unable to open/close sockets for virtual linking."),
			"\n",
			_("Cause: check that you have permissions to open sockets."),
			NULL);
		break;

		default:
		// propagate error code
		return number;

	}

#ifndef __WIN32__
	if (errno != 0) 
	{
		char * tmp = *message;

		gchar* str = g_strdup_printf(" (errno = %i)", errno);
		*message = g_strconcat(tmp, "\n", "System: ", strerror(errno), str, "\n", NULL);
		g_free(tmp);
		g_free(str);
	}
#else
	if (GetLastError()) 
	{
		LPVOID lpMsgBuf;
		gchar *str;
		char * tmp = *message;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 
			0, 
			NULL);

		str = g_strdup_printf("(%li) %s", (long)GetLastError, (char *)lpMsgBuf);
		*message = g_strconcat(tmp, "\n", "System: ", str, NULL);
		g_free(tmp);
		g_free(str);
	}
#endif

	return 0;
}

/**
 * ticables_error_free:
 * @message: a message previously allocated by ticables_error_get()
 *
 * Free the given message string allocated by ticables_error_get();
 *
 * Return value: 0 if the argument was valid and the message was freed, nonzero otherwise.
 **/
int TICALL ticables_error_free(char *message)
{
	if (message == nullptr)
	{
		ticables_critical("ticables_error_free(NULL)\n");
		return ERR_ILLEGAL_ARG;
	}
	g_free(message);
	return 0;
}
