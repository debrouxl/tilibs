/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
#include <string.h>
#include <glib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ticalcs.h"
#include "gettext.h"
#include "export3.h"
#include "error.h"
#include "logging.h"
#include "internal.h"

// extern helpers

extern int nsp_reset;

/**
 * ticalcs_error_get:
 * @number: error number (see error.h for list).
 * @message: a newly glib allocated string which contains corresponding error *message.
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
int TICALL ticalcs_error_get(int number, char **message)
{
	// free memory
	//dusb_vtl_pkt_purge();
	//dusb_cpca_purge();
	//nsp_vtl_pkt_purge();

	if (message == nullptr)
	{
		ticalcs_critical("ticalcs_error_get(NULL)\n");
		return number;
	}

	switch(number)
	{
	case ERR_ABORT:
		*message = g_strconcat(
		_("Msg: transfer aborted."),
		"\n",
		_("Cause: the user!"),
		NULL);
		break;

	case ERR_NOT_READY:
		*message = g_strconcat(
		_("Msg: calculator is not ready."),
		"\n",
		_("Cause: the calculator is busy (Var-Link menu, Window, ...). Press HOME..."),
		NULL);
		break;

	case ERR_CHECKSUM:
		*message = g_strconcat(
		_("Msg: checksum error."),
		"\n",
		_("Cause: a transmission error. Try again."),
		NULL);
		break;

	case ERR_INVALID_HOST:
		*message = g_strconcat(
		_("Msg: invalid host ID."),
		"\n",
		_("Cause: received an unexpected Machine ID from calculator, probably due to a transmission error."),
		NULL);
		break;

	case ERR_INVALID_TARGET:
		*message = g_strconcat(
		_("Msg: invalid target ID."),
		"\n",
		_("Cause: received an unexpected Target ID from calculator, probably due to a transmission error."),
		NULL);
		break;

	case ERR_INVALID_CMD:
		*message = g_strconcat(
		_("Msg: invalid command ID."),
		"\n",
		_("Cause: received an unexpected Command ID from calculator, probably due to a transmission error."),
		NULL);
		break;

	case ERR_EOT:
		*message = g_strconcat(
		_("Msg: not really an error, should not appear!"),
		"\n",
		_("Cause: a bug. Drop us an email!"),
		NULL);
		break;

	case ERR_VAR_VERSION:
		*message = g_strconcat(
		_("Msg: program or variable is incompatible with this OS version."),
		"\n",
		_("Cause: this variable type may not be supported by the target calculator, or you may need to upgrade your calculator OS."),
		NULL);
		break;

	case ERR_VAR_REJECTED:
		*message = g_strconcat(
		_("Msg: contents has been refused."),
		"\n",
		_("Cause: you attempted to send a variable which is locked/archived on the target calculator. The calculator may be out of memory. The certificate may be invalid or already installed."),
		NULL);
		break;

	case ERR_CTS_ERROR:
		*message = g_strconcat(
		_("Invalid CTS packet."),
		"\n",
		_("Cause: a transmission error."),
		NULL);
		break;

	case ERR_NACK:
		*message = g_strconcat(
		_("NACK received."),
		"\n",
		_("Cause: the command has been refused."),
		NULL);
		break;

	case ERR_INVALID_PACKET:
		*message = g_strconcat(
		_("Invalid packet."),
		"\n",
		_("Cause: a transmission error."),
		NULL);
		break;

	case ERR_MALLOC:
		break;

	case ERR_NO_CABLE:
		*message = g_strconcat(
		_("The cable cannot be used."),
		"\n",
		_("Cause: the cable has not been initialized due to a previous/current error."),
		NULL);
		break;

	case ERR_BUSY:
		break;

	case ERR_FILE_OPEN:
		break;

	case ERR_VOID_FUNCTION:
	case ERR_UNSUPPORTED:
		*message = g_strconcat(
		_("Msg: this function does not exist for this calculator."),
		"\n",
		_("Cause: the operation you attempted is not supported by your calculator model."),
		NULL);
		break;

	case ERR_OUT_OF_MEMORY:
		*message = g_strconcat(
		_("Msg: calculator out of memory."),
		"\n",
		_("Cause: the calculator does not have memory enough for doing the operation."),
		NULL);
		break;

	case ERR_OPEN_FILE:
		*message = g_strconcat(
		_("Msg: unable to open (reading) the file."),
		"\n",
		_("Cause: disk space or permissions."),
		NULL);
		break;

	case ERR_SAVE_FILE:
		*message = g_strconcat(
		_("Msg: unable to open (writing) the file."),
		"\n",
		_("Cause: disk space or permissions."),
		NULL);
		break;

	case ERR_PENDING_TRANSFER:
		*message = g_strconcat(
		_("A transfer is in progress. You cannot start a new one."),
		"\n",
		_("Cause: this should not happen. Probably a bug: drop us an email!"),
		NULL);
		break;

	case ERR_FATAL_ERROR:
		break;

	case ERR_MISSING_VAR:
		*message = g_strconcat(
		_("The requested var does not exist."),
		"\n",
		_("Cause: you attempted to download a variable which does not exist on the calculator any longer. Maybe you have manually deleted it ?"),
		NULL);
		break;

	case ERR_NO_VARS:
		*message = g_strconcat(
		_("There is nothing to backup."),
		"\n",
		_("Cause: 0 variables stored onto your calculator."),
		NULL);
		break;

	case ERR_INVALID_HANDLE:
		*message = g_strconcat(
		_("Invalid ticalcs handle."),
		"\n",
		_("Cause: bad cable or calc model."),
		NULL);
		break;

	case ERR_INVALID_PARAMETER:
		*message = g_strconcat(
		_("Invalid parameter."),
		"\n",
		_("Cause: if you see this in normal usage, a bug: drop us an email ! Otherwise, check the arguments you're passing to the function."),
		NULL);
		break;

	case ERR_INVALID_SCREENSHOT:
		*message = g_strconcat(
		_("Unknown screenshot format."),
		"\n",
		_("Cause: a transmission error, or unsupported calculator version."),
		NULL);
		break;

	case ERR_CALC_ERROR1:	// must be synchronized with cmd68k.c (static uint8_t dbus_errors[])
		*message = g_strconcat(
		_("Msg: hand-held returned an error."),
		"\n",
		_("Cause: hand-held returned an uncaught error. Please report log."),
		NULL);
		break;
	case ERR_CALC_ERROR1+1:
		*message = g_strdup(
		_("Msg: cannot delete application."));
		break;
	case ERR_CALC_ERROR1+2:
		*message = g_strdup(
		_("Msg: invalid variable name."));
		break;
	case ERR_CALC_ERROR1+3:
		*message = g_strdup(
		_("Msg: cannot overwrite variable (locked/archived)."));
		break;
	case ERR_CALC_ERROR1+4:
		*message = g_strdup(
		_("Msg: cannot delete variable (locked/archived)."));
		break;
	case ERR_CALC_ERROR1+5:
		*message = g_strdup(
		_("Msg: FLASH application rejected (bad target model)."));
		break;
	case ERR_CALC_ERROR1+6:
		*message = g_strdup(
		_("Msg: malformed RTS packet (AMS version conflict)."));
		break;
	case ERR_CALC_ERROR1+7:
		*message = g_strdup(
		_("Msg: FLASH application rejected (signature does not match)."));
		break;

	case ERR_CALC_ERROR2:	// must be synchronized with dusb_cmd.c (static uint16_t usb_errors[])
		*message = g_strconcat(
		_("Msg: hand-held returned an error (not caught)."),
		"\n",
		_("Cause: hand-held returned an uncaught error. Please report log."),
		NULL);
		break;
	case ERR_CALC_ERROR2+1:
		*message = g_strdup(
		_("Msg: invalid argument or name."));
		break;
	case ERR_CALC_ERROR2+2:
		*message = g_strdup(
		_("Msg: cannot delete var/app from archive."));
		break;
	case ERR_CALC_ERROR2+3:
		*message = g_strdup(
		_("Msg: transmission error."));
		break;
	case ERR_CALC_ERROR2+4:
		*message = g_strdup(
		_("Msg: using basic mode while being in boot mode."));
		break;
	case ERR_CALC_ERROR2+5:
		*message = g_strdup(
		_("Msg: out of memory."));
		break;
	case ERR_CALC_ERROR2+6:
		*message = g_strdup(
		_("Msg: invalid name."));
		break;
	case ERR_CALC_ERROR2+7:
		*message = g_strdup(
		_("Msg: invalid name."));
		break;
	case ERR_CALC_ERROR2+8:
		*message = g_strdup(
		_("Msg: busy?"));
		break;
	case ERR_CALC_ERROR2+9:
		*message = g_strdup(
		_("Msg: can't overwrite, variable is locked."));
		break;
	case ERR_CALC_ERROR2+10:
		*message = g_strdup(
		_("Msg: variable too large."));
		break;
	case ERR_CALC_ERROR2+11:
		*message = g_strdup(
		_("Msg: mode token too small."));
		break;
	case ERR_CALC_ERROR2+12:
		*message = g_strdup(
		_("Msg: mode token too large."));
		break;
	case ERR_CALC_ERROR2+13:
		*message = g_strdup(
		_("Msg: wrong size for parameter."));
		break;
	case ERR_CALC_ERROR2+14:
		*message = g_strdup(
		_("Msg: invalid parameter ID."));
		break;
	case ERR_CALC_ERROR2+15:
		*message = g_strdup(
		_("Msg: read-only parameter."));
		break;
	case ERR_CALC_ERROR2+16:
		*message = g_strdup(
		_("Msg: wrong modify request?"));
		break;
	case ERR_CALC_ERROR2+17:
		*message = g_strdup(
		_("Msg: remote control?"));
		break;
	case ERR_CALC_ERROR2+18:
		*message = g_strdup(
		_("Msg: battery low."));
		break;
	case ERR_CALC_ERROR2+19:
		*message = g_strdup(
		_("Msg: FLASH application rejected (e.g. TI-68k FL_addCert 6)."));
		break;
	case ERR_CALC_ERROR2+20:
		*message = g_strdup(
		_("Msg: FLASH application rejected (e.g. TI-68k FL_addCert 7)."));
		break;
	case ERR_CALC_ERROR2+21:
		*message = g_strdup(
		_("Msg: FLASH application rejected (signature does not match)."));
		break;
	case ERR_CALC_ERROR2+22:
		*message = g_strdup(
		_("Msg: FLASH application rejected (e.g. TI-68k FL_addCert 9)."));
		break;
	case ERR_CALC_ERROR2+23:
		*message = g_strdup(
		_("Msg: FLASH application rejected (e.g. TI-68k FL_addCert A)."));
		break;
	case ERR_CALC_ERROR2+24:
		*message = g_strdup(
		_("Msg: hand-held is busy (set your calculator to HOME screen)."));
		break;

	case ERR_CALC_ERROR3+0:	// must be synchronized with nsp_cmd.c (static uint8_t usb_errors[])
		*message = g_strconcat(
		_("Msg: hand-held returned an error (not caught)."),
		"\n",
		_("Cause: hand-held returned an uncaught error. Please report log."),
		NULL);
		break;
	case ERR_CALC_ERROR3+1:
		*message = g_strdup(
		_("Msg: out of memory."));
		break;
	case ERR_CALC_ERROR3+2:
		*message = g_strdup(
		_("Msg: OS installation failed. File is corrupted/wrong."));
		break;
	// The following one is returned when the filename does not fit in a 254-byte packet.
	case ERR_CALC_ERROR3+3:
		*message = g_strdup(
		_("Msg: packet too large."));
		break;
	case ERR_CALC_ERROR3+4:
		*message = g_strdup(
		_("Msg: the variable or directory does not exist."));
		break;
	case ERR_CALC_ERROR3+5:
		*message = g_strdup(
		_("Msg: the directory name is invalid."));
		break;
	case ERR_CALC_ERROR3+6:
		*message = g_strdup(
		_("Msg: not currently listing a directory."));
		break;
	case ERR_CALC_ERROR3+7:
		*message = g_strdup(
		_("Msg: no more files to list."));
		break;
	case ERR_CALC_ERROR3+8:
		*message = g_strdup(
		_("Msg: the directory name already exists."));
		break;
	// The following one is returned with folder names >= 230 bytes long
	case ERR_CALC_ERROR3+9:
		*message = g_strdup(
		_("Msg: the destination (folder or file) path is too long."));
		break;
	case ERR_CALC_ERROR3+10:
		*message = g_strdup(
		_("Msg: the file name is invalid."));
		break;
	case ERR_CALC_ERROR3+11:
		*message = g_strdup(
		_("Msg: no file extension."));
		break;
	case ERR_CALC_ERROR3+12:
		*message = g_strdup(
		_("Msg: forbidden characters in folder name."));
		break;
	// The following one (FF 20) is returned immediately by a e.g. CX CAS running 3.0.1.1753,
	// when ones tries to send it ZiDian.tcc (English-Chinese dictionary) 3.1.0.392.
	// A CX CAS running 3.1.0.392 sends FF 04 (OS refused because it's not suitable / corrupted / whatever) instead.
	//
	// 3.0.1.1753 behaves exactly the same if the magic in the header is edited from __OSEXT__1 to __OTEXT__1.
	// Therefore, it seems that FF 20 is a generic error message, rather than just "this particular OS extension is refused".
	case ERR_CALC_ERROR3+13:
		*message = g_strdup(
		_("Msg: OS upgrade type not recognized."));
		break;
	case ERR_CALC_ERROR3+14:
		*message = g_strdup(
		_("Msg: anti-downgrade protection refuses this OS version."));
		break;

	default:
		// propagate error code
		return number;
	}

	return 0;
}

/**
 * ticalcs_error_free:
 * @message: a message previously allocated by ticalcs_error_get()
 *
 * Free the given message string allocated by ticalcs_error_get();
 *
 * Return value: 0 if the argument was valid and the message was freed, nonzero otherwise.
 **/
int TICALL ticalcs_error_free(char *message)
{
	if (message == nullptr)
	{
		ticalcs_critical("ticalcs_error_free(NULL)\n");
		return ERR_INVALID_PARAMETER;
	}
	g_free(message);
	return 0;
}
