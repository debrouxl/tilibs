/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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

#include "intl3.h"
#include "export.h"

#include "file_err.h"

/* 
   This function put in err_msg the error message corresponding to the 
   error code.
   If the error code has been handled, the function returns 0 else it 
   propagates the error code by returning it.

   The error message has the following format:
   - 1: the error message
   - 2: the cause(s), explanations on how to fix it
*/
TIEXPORT int TICALL tifiles_get_error(int err_num, char *error_msg)
{
  switch (err_num) {
  case ERR_MALLOC:
    strcpy(error_msg, _("Msg: unable to allocate memory (malloc)."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: memory too low ?"));
    break;

  case ERR_FILE_OPEN:
    strcpy(error_msg, _("Msg: unable to open file."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("either the file does not exist, either there is no room."));
    break;

  case ERR_FILE_CLOSE:
    strcpy(error_msg, _("Msg: unable to close file."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: either the file does not exist, either there is no room."));
    break;

  case ERR_GROUP_SIZE:
    strcpy(error_msg, _("Msg: the size of a group file can not exceed 64KB."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: too many variables/data."));
    break;

  case ERR_BAD_CALC:
    strcpy(error_msg, _("Msg: Unknown calculator type."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: probably due to a bug, mail to: tilp-users@lists.sf.net."));
    break;

  case ERR_INVALID_FILE:
  case ERR_BAD_FILE:
    strcpy(error_msg, _("Msg: invalid file."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: it's probably not a TI formatted file."));
    break;

  case ERR_FILE_CHECKSUM:
    strcpy(error_msg, _("Msg: checksum error."));
    strcat(error_msg, "\n");
    strcat(error_msg, _("Cause: the file has an incorrect checksum and may be corrupted."));
    break;

  default:
    strcpy(error_msg, _("Error code not found in the list.\nThis is a bug. Please report it.\n."));
    return err_num;
    break;
  }

  return 0;
}
