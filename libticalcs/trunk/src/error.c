/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

#include <stdio.h>
#include <string.h>

#include "calc_err.h"
#include "headers.h"
#include "update.h"
#include "intl2.h"

extern TicableLinkCable *cable;

/* 
   This function put in err_msg the error message corresponding to the
   error code.
   If the error code has been handled, the function returns 0 else it
   propagates the error code by returning it.

   The error message has the following format:
   - 1: the error message
   - 2: the cause(s), explanations on how to fix it
*/
TIEXPORT int TICALL ticalc_get_error(int err_num, char *error_msg)
{
  switch (err_num) {
  case ERR_ABORT:
    strcpy(error_msg, _("Msg: Transfer aborted"));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: the user !"));
    break;

  case ERR_CHECKSUM:
    strcpy(error_msg, _("Msg: Checksum error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: a transmission error. Try again."));
    break;

  case ERR_NOT_READY:
    strcpy(error_msg, _("Msg: Calculator is not ready."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: the calculator is off, your link cable is not plugged correctly, your calculator is busy, ..."));
    break;

  case ERR_VOID_FUNCTION:
    strcpy(error_msg,
	   _("Msg: This function does not exist for this calculator."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: the operation you attempted is not supported by your calculator model."));
    break;

  case ERR_OUT_OF_MEMORY:
    strcpy(error_msg, _("Msg: Calculator out of memory."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: the calculator does not have memory enough for doing the operation."));
    break;

  case ERR_OPEN_FILE:
    strcpy(error_msg, _("Msg: Unable to open (reading) the file."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: disk space or permissions."));
    break;

  case ERR_SAVE_FILE:
    strcpy(error_msg, _("Msg: Unable to open (writing) the file."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: disk space or permissions."));
    break;

  case ERR_INVALID_HOST:
    strcpy(error_msg, _("Msg: Invalid host ID."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: TiLP received an unexpected Machine ID, probably due to a transmission error."));
    break;

  case ERR_INVALID_TARGET:
    strcpy(error_msg, _("Msg: Invalid target ID."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: TiLP received an unexpected Target ID, probably due to a transmission error."));
    break;

  case ERR_INVALID_CMD:
    strcpy(error_msg, _("Msg: Invalid command ID."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: TiLP received an unexpected Command ID, probably due to a transmission error."));
    break;

  case ERR_EOT:
    strcpy(error_msg, _("Msg: Not really an error, should not appear !"));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: a bug. Drop us an email !"));
    break;

  case ERR_VAR_REJECTED:
    strcpy(error_msg,
	   _("Msg: The variable or FLASH application has been refused."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: you attempted to send a variable which is locked/archived on the target calculator. The calculator may be out of memory, too."));
    break;

  case ERR_CTS_ERROR:
    strcpy(error_msg, _("Invalid CTS packet."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: a transmission error."));
    break;

  case ERR_NACK:
    strcpy(error_msg, _("NACK received."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: the command has been refused.\n"));
    break;

  case ERR_INVALID_PACKET:
    strcpy(error_msg, _("Invalid packet."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: a transmission error.\n"));
    break;

  case ERR_MISSING_VAR:
    strcpy(error_msg, _("The requested var does not exist."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: you attempted to download a variable which does not exist on the calculaotr any longer. Maybe you have manually deleted it ?"));
    break;

  case ERR_PENDING_TRANSFER:
    strcpy(error_msg,
	   _("A transfer is in progress. You can not start a new one."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _
	   ("Cause: this should not happen. Probably a bug: drop us an email !"));
    break;

  case ERR_NO_VARS:
    strcpy(error_msg, _("There is nothing to backup."));
    strcat(error_msg, "\n");
    strcat(error_msg,
	   _("Cause: 0 variables stored onto your calculator.\n"));
    break;

  default:
    strcpy(error_msg,
	   _("Error code not found. This is a bug. Please report it.\n"));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: a bug. Drop us an email !"));
    UNLOCK_TRANSFER()
	return err_num;
    break;
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}
