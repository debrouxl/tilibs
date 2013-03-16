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

#include "config.h"

#include "gettext.h"
#include "export3.h"
#include "error.h"
#include "logging.h"

// extern helpers
extern void dusb_vtl_pkt_purge(void);
extern void nsp_vtl_pkt_purge(void);
extern void dusb_cpca_purge(void);

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
TIEXPORT3 int TICALL ticalcs_error_get(CalcError number, char **message)
{
	char error_msg[2048];

	// free memory
	dusb_vtl_pkt_purge();
	dusb_cpca_purge();
	nsp_vtl_pkt_purge();

	if (message == NULL)
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
		strcpy(error_msg, _("Msg: checksum error."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a transmission error. Try again."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_HOST:
		strcpy(error_msg, _("Msg: invalid host ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: received an unexpected Machine ID from calculator, probably due to a transmission error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_TARGET:
		strcpy(error_msg, _("Msg: invalid target ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: received an unexpected Target ID from calculator, probably due to a transmission error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_CMD:
		strcpy(error_msg, _("Msg: invalid command ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: received an unexpected Command ID from calculator, probably due to a transmission error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_EOT:
		strcpy(error_msg, _("Msg: not really an error, should not appear!"));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a bug. Drop us an email!"));
		*message = g_strdup(error_msg);
		break;

	case ERR_VAR_REJECTED:
		strcpy(error_msg,
		_("Msg: contents has been refused."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: you attempted to send a variable which is locked/archived on the target calculator. The calculator may be out of memory. The certificate may be invalid or already installed."));
		*message = g_strdup(error_msg);
		break;

	case ERR_CTS_ERROR:
		strcpy(error_msg, _("Invalid CTS packet."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a transmission error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_NACK:
		strcpy(error_msg, _("NACK received."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: the command has been refused."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_PACKET:
		strcpy(error_msg, _("Invalid packet."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a transmission error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_MALLOC:
		break;

	case ERR_NO_CABLE:
		strcpy(error_msg, _("The cable cannot be used."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: the cable has not been initialized due to a previous/current error."));
		*message = g_strdup(error_msg);
		break;

	case ERR_BUSY:
		break;

	case ERR_FILE_OPEN:
		break;

	case ERR_VOID_FUNCTION:
	case ERR_UNSUPPORTED:
		strcpy(error_msg,
		_("Msg: this function does not exist for this calculator."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: the operation you attempted is not supported by your calculator model."));
		*message = g_strdup(error_msg);
		break;

	case ERR_OUT_OF_MEMORY:
		strcpy(error_msg, _("Msg: calculator out of memory."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: the calculator does not have memory enough for doing the operation."));
		*message = g_strdup(error_msg);
		break;

	case ERR_OPEN_FILE:
		strcpy(error_msg, _("Msg: unable to open (reading) the file."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: disk space or permissions."));
		*message = g_strdup(error_msg);
		break;

	case ERR_SAVE_FILE:
		strcpy(error_msg, _("Msg: unable to open (writing) the file."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: disk space or permissions."));
		*message = g_strdup(error_msg);
		break;

	case ERR_PENDING_TRANSFER:
		strcpy(error_msg,
		_("A transfer is in progress. You cannot start a new one."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: this should not happen. Probably a bug: drop us an email!"));
		*message = g_strdup(error_msg);
		break;

	case ERR_FATAL_ERROR:
		break;

	case ERR_MISSING_VAR:
		strcpy(error_msg, _("The requested var does not exist."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: you attempted to download a variable which does not exist on the calculator any longer. Maybe you have manually deleted it ?"));
		*message = g_strdup(error_msg);
		break;

	case ERR_NO_VARS:
		strcpy(error_msg, _("There is nothing to backup."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: 0 variables stored onto your calculator."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_HANDLE:
		strcpy(error_msg, _("Invalid ticalcs handle."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: bad cable or calc model."));
		*message = g_strdup(error_msg);
		break;

	case ERR_INVALID_PARAMETER:
		strcpy(error_msg, _("Invalid parameter."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: if you see this in normal usage, a bug: drop us an email ! Otherwise, check the arguments you're passing to the function."));
		*message = g_strdup(error_msg);
		break;

	case ERR_CALC_ERROR1:	// must be synchronized with cmd89.c (static uint8_t dbus_errors[])
		strcpy(error_msg, _("Msg: hand-held returned an error."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: hand-held returned an uncaught error. Please report log."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+1:
		strcpy(error_msg, _("Msg: cannot delete application."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+2:
		strcpy(error_msg, _("Msg: invalid variable name."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+3:
		strcpy(error_msg, _("Msg: cannot overwrite variable (locked/archived)."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+4:
		strcpy(error_msg, _("Msg: cannot delete variable (locked/archived)."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+5:
		strcpy(error_msg, _("Msg: FLASH application rejected (bad target model)."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+6:
		strcpy(error_msg, _("Msg: malformed RTS packet (AMS version conflict)."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR1+7:
		strcpy(error_msg, _("Msg: FLASH application rejected (signature does not match)."));
		*message = g_strdup(error_msg);
		break;

	case ERR_CALC_ERROR2:	// must be synchronized with dusb_cmd.c (static uint16_t usb_errors[])
		strcpy(error_msg, _("Msg: hand-held returned an error (not caught)."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: hand-held returned an uncaught error. Please report log."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+1:
		strcpy(error_msg, _("Msg: invalid argument or name."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+2:
		strcpy(error_msg, _("Msg: cannot delete var/app from archive."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+3:
		strcpy(error_msg, _("Msg: transmission error."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+4:
		strcpy(error_msg, _("Msg: using basic mode while being in boot mode."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+5:
		strcpy(error_msg, _("Msg: out of memory."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+6:
		strcpy(error_msg, _("Msg: invalid name."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+7:
		strcpy(error_msg, _("Msg: invalid name."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+8:
		strcpy(error_msg, _("Msg: busy?."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+9:
		strcpy(error_msg, _("Msg: can't overwrite, variable is locked."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+10:
		strcpy(error_msg, _("Msg: mode token too small."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+11:
		strcpy(error_msg, _("Msg: mode token too large."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+12:
		strcpy(error_msg, _("Msg: invalid parameter ID."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+13:
		strcpy(error_msg, _("Msg: wrong modify request?"));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+14:
		strcpy(error_msg, _("Msg: remote control?"));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+15:
		strcpy(error_msg, _("Msg: battery low."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+16:
		strcpy(error_msg, _("Msg: FLASH application rejected (signature does not match)."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR2+17:
		strcpy(error_msg, _("Msg: hand-held is busy (set your calculator to HOME screen)."));
		*message = g_strdup(error_msg);
		break;

	case ERR_CALC_ERROR3+0:	// must be synchronized with nsp_cmd.c (static uint8_t usb_errors[])
		strcpy(error_msg, _("Msg: hand-held returned an error (not caught)."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		_("Cause: hand-held returned an uncaught error. Please report log."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+1:
		strcpy(error_msg, _("Msg: out of memory."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+2:
		strcpy(error_msg, _("Msg: OS installation failed. File is corrupted/wrong."));
		*message = g_strdup(error_msg);
		break;
	// The following one is returned when the filename does not fit in a 254-byte packet.
	case ERR_CALC_ERROR3+3:
		strcpy(error_msg, _("Msg: packet too large."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+4:
		strcpy(error_msg, _("Msg: the variable or directory does not exist."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+5:
		strcpy(error_msg, _("Msg: the directory name is invalid."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+6:
		strcpy(error_msg, _("Msg: not currently listing a directory."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+7:
		strcpy(error_msg, _("Msg: no more files to list."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+8:
		strcpy(error_msg, _("Msg: the directory name already exists."));
		*message = g_strdup(error_msg);
		break;
	// The following one is returned with folder names >= 230 bytes long
	case ERR_CALC_ERROR3+9:
		strcpy(error_msg, _("Msg: the destination (folder or file) path is too long."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+10:
		strcpy(error_msg, _("Msg: the file name is invalid."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+11:
		strcpy(error_msg, _("Msg: no file extension."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+12:
		strcpy(error_msg, _("Msg: forbidden characters in folder name."));
		*message = g_strdup(error_msg);
		break;
	// The following one (FF 20) is returned immediately by a e.g. CX CAS running 3.0.1.1753,
	// when ones tries to send it ZiDian.tcc (English-Chinese dictionary) 3.1.0.392.
	// A CX CAS running 3.1.0.392 sends FF 04 (OS refused because it's not suitable / corrupted / whatever) instead.
	//
	// 3.0.1.1753 behaves exactly the same if the magic in the header is edited from __OSEXT__1 to __OTEXT__1.
	// Therefore, it seems that FF 20 is a generic error message, rather than just "this particular OS extension is refused".
	case ERR_CALC_ERROR3+13:
		strcpy(error_msg, _("Msg: OS upgrade type not recognized."));
		*message = g_strdup(error_msg);
		break;
	case ERR_CALC_ERROR3+14:
		strcpy(error_msg, _("Msg: anti-downgrade protection refuses this OS version."));
		*message = g_strdup(error_msg);
		break;

	default:
		// propagate error code
		return number;
	}

	return 0;
}
