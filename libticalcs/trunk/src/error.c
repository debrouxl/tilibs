/*  libticalcs - calculator library, a part of the TiLP project
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
#include <string.h>

#include "calc_err.h"
#include "calc_ext.h"

/* 
   This function put in err_msg the error message corresponding to the 
   error code.
   If the error code has been handled, the function returns 0 else it 
   propagates the error code by returning it.
*/
TIEXPORT int TICALL
ticalc_get_error(int err_num, char *error_msg)
{
  switch(err_num)
    {
    case ERR_ABORT:
      strcpy(error_msg, _("Transfer aborted"));
      break;
    case ERR_NOT_REPLY:
      strcpy(error_msg, _("The calculator do not reply. Check link."));
      break;
    case ERR_NOT_ACK:
      strcpy(error_msg, _("Remote control. Calculator did not acknowledge."));
      break;
    case ERR_CHECKSUM:
      strcpy(error_msg, _("Checksum error."));
      break;  
    case ERR_VAR_NOTEXIST:
      strcpy(error_msg, _("The requested variable does not exist."));
      break;
    case ERR_DISCONTINUE:
      strcpy(error_msg, _("The calculator does not want continue."));
      break;
    case ERR_INVALID_TI92_FILE:
      strcpy(error_msg, _("Invalid TI92 file (restrictive mode)."));
      break;
    case ERR_NOT_READY:
      strcpy(error_msg, _("Calculator is not ready."));
      break;
    case ERR_BACKUP:
      strcpy(error_msg, _("Sending variable. This is a backup, not a variable."));
      break;
    case ERR_INVALID_BYTE:
      strcpy(error_msg, _("Invalid byte received."));
      break;
    case ERR_INVALID_TI92p_FILE:
      strcpy(error_msg, _("Invalid TI92+ file (restrictive mode)."));
      break;
    case ERR_INVALID_TI89_FILE:
      strcpy(error_msg, _("Invalid TI89 file (restrictive mode)."));
      break;
    case ERR_VOID_FUNCTION:
      strcpy(error_msg, _("This function does not exist for this calculator."));
      break;
    case ERR_INVALID_TI82_FILE:
      strcpy(error_msg, _("Invalid TI82 file (restrictive mode)."));
      break;
    case ERR_PACKET:
      strcpy(error_msg, _("Packet error."));
      break;
    case ERR_SIZE:
      strcpy(error_msg, _("Size error."));
      break;
    case ERR_INVALID_TI83_FILE:
      strcpy(error_msg, _("Invalid TI83 file (restrictive mode)."));
      break;
    case ERR_INVALID_TI85_FILE:
      strcpy(error_msg, _("Invalid TI85 file (restrictive mode)."));
      break;
    case ERR_INVALID_TI86_FILE:
      strcpy(error_msg, _("Invalid TI86 file (restrictive mode)."));
      break;
    case ERR_INVALID_FLASH_FILE:
    strcpy(error_msg, _("Invalid FLASH file."));
      break;
    case ERR_VAR_REFUSED:
      strcpy(error_msg, _("Variable or FLASH application refused (already exists or calculator out of memory)."));
      break;
    case ERR_OUT_OF_MEMORY:
      strcpy(error_msg, _("Calculator out of memory."));
      break;
    case ERR_INVALID_TIXX_FILE:
      strcpy(error_msg, _("Invalid TIxx file (compatibility mode)."));
      break;
    case ERR_GRP_SIZE_EXCEEDED:
      strcpy(error_msg, _("Group size exceeded (must be less than 64 KBytes)."));
      break;
    case ERR_OPEN_FILE:
      strcpy(error_msg, _("Unable to open the file."));
      break;

    default: strcpy(error_msg, _("Error code not found. This is a bug. Please report it.\n")); 
      return err_num;
      break;
    }

  return 0;
}

