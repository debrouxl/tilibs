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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "gettext.h"
#include "export3.h"
#include "error.h"
#include "logging.h"

extern void vtl_pkt_purge(void);
extern void cpca_purge(void);

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
TIEXPORT int TICALL ticalcs_error_get(CalcError number, char **message)
{
	char error_msg[2048];

	vtl_pkt_purge();
	cpca_purge();

	g_assert (message != NULL);

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
    		_("Msg: Calculator is not ready."),
    		"\n",
    		 _("Cause: the calculator is busy (Var-Link menu, Window, ...). Press HOME..."),
			 NULL);
		break;

	case ERR_CHECKSUM:
		strcpy(error_msg, _("Msg: Checksum error."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a transmission error. Try again."));
		*message = g_strdup(error_msg);
    break;

	case ERR_INVALID_HOST:
		strcpy(error_msg, _("Msg: Invalid host ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: TiLP received an unexpected Machine ID, probably due to a transmission error."));
		*message = g_strdup(error_msg);
    break;

	case ERR_INVALID_TARGET:
		strcpy(error_msg, _("Msg: Invalid target ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: TiLP received an unexpected Target ID, probably due to a transmission error."));
		*message = g_strdup(error_msg);
    break;

	case ERR_INVALID_CMD:
		strcpy(error_msg, _("Msg: Invalid command ID."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: TiLP received an unexpected Command ID, probably due to a transmission error."));
		*message = g_strdup(error_msg);
    break;

	case ERR_EOT:
		strcpy(error_msg, _("Msg: Not really an error, should not appear!"));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a bug. Drop us an email!"));
		*message = g_strdup(error_msg);
    break;

	case ERR_VAR_REJECTED:
		strcpy(error_msg,
		   _("Msg: The content has been refused."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: you attempted to send a variable which is locked/archived on the target calculator. The calculator may be out of memory. The certificate may be invalid or already installed."));
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
		strcat(error_msg, _("Cause: the command has been refused.\n"));
		*message = g_strdup(error_msg);
    break;

	case ERR_INVALID_PACKET:
		strcpy(error_msg, _("Invalid packet."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: a transmission error.\n"));
		*message = g_strdup(error_msg);
    break;

  case ERR_MALLOC:
	  break;

  case ERR_NO_CABLE:
		strcpy(error_msg, _("The cable can not be used."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: the cable has not been initialized due to a previous/current error.\n"));
		*message = g_strdup(error_msg);
	  break;

  case ERR_BUSY:
	  break;

  case ERR_FILE_OPEN:
	  break;

  case ERR_VOID_FUNCTION:
	case ERR_UNSUPPORTED:
		strcpy(error_msg,
		   _("Msg: This function does not exist for this calculator."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: the operation you attempted is not supported by your calculator model."));
		*message = g_strdup(error_msg);
    break;

case ERR_OUT_OF_MEMORY:
		strcpy(error_msg, _("Msg: Calculator out of memory."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: the calculator does not have memory enough for doing the operation."));
		*message = g_strdup(error_msg);
    break;

  case ERR_OPEN_FILE:
		strcpy(error_msg, _("Msg: Unable to open (reading) the file."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: disk space or permissions."));
		*message = g_strdup(error_msg);
    break;

  case ERR_SAVE_FILE:
		strcpy(error_msg, _("Msg: Unable to open (writing) the file."));
		strcat(error_msg, "\n");
		strcat(error_msg, _("Cause: disk space or permissions."));
		*message = g_strdup(error_msg);
    break;

	case ERR_PENDING_TRANSFER:
		strcpy(error_msg,
		   _("A transfer is in progress. You can not start a new one."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: this should not happen. Probably a bug: drop us an email!"));
		*message = g_strdup(error_msg);
    break;

	case ERR_FATAL_ERROR:
		break;

	case ERR_MISSING_VAR:
		strcpy(error_msg, _("The requested var does not exist."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: you attempted to download a variable which does not exist on the calculaotr any longer. Maybe you have manually deleted it ?"));
		*message = g_strdup(error_msg);
    break;

  case ERR_NO_VARS:
		strcpy(error_msg, _("There is nothing to backup."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _("Cause: 0 variables stored onto your calculator.\n"));
		*message = g_strdup(error_msg);
    break;

  case ERR_CALC_ERROR1:
		strcpy(error_msg, _("Msg: Hand-held returned an error (not catched)."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: hand-held returned an uncatched error. Please report log."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR1+1:
	    strcpy(error_msg, _("Msg: can not delete application."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR1+2:
	    strcpy(error_msg, _("Msg: invalid variable name."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR1+3:
	    strcpy(error_msg, _("Msg: can not overwrite variable (locked/archived)."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR1+4:
		strcpy(error_msg, _("Msg: can not delete variable (locked/archived)."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR1+5:
		strcpy(error_msg, _("Msg: FLASH application rejected (bad target model)."));
		*message = g_strdup(error_msg);
    break;

  case ERR_CALC_ERROR2:
		strcpy(error_msg, _("Msg: Hand-held returned an error (not catched)."));
		strcat(error_msg, "\n");
		strcat(error_msg,
		   _
		   ("Cause: hand-held returned an uncatched error. Please report log."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR2+1:
	    strcpy(error_msg, _("Msg: invalid argument or name."));
		*message = g_strdup(error_msg);
    break;
  case ERR_CALC_ERROR2+2:
	    strcpy(error_msg, _("Msg: can not delete application."));
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
	    strcpy(error_msg, _("Msg: battery low."));
		*message = g_strdup(error_msg);
    break;

	default:
		// propagate error code
	    return number;
    break;
	}

	return 0;
}
