/*  ti_link - link program for TI calculators
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
#include <string.h>

#include "calc_err.h"
#include "calc_ext.h"

/* Obsolete, for backward compatibility */
/*
DLLEXPORT
int obs_error(int err_num)
{
  char s[256];

  cable->close_port();
  if(err_num)
    {
      fprintf(stderr, "Error code %i: ", err_num);
    }  
  switch(err_num)
    {
    case 4: strcpy(s, MSG004); break;
    case 5: strcpy(s, MSG005); break;
    case 6: strcpy(s, MSG006); break;
    case 7: strcpy(s, MSG007); break;
    case 8: strcpy(s, MSG008); break;
    case 9: strcpy(s, MSG009); break;
    case 10: strcpy(s, MSG010); break;
    case 11: strcpy(s, MSG011); break;
    case 12: strcpy(s, MSG012); break;
    case 13: strcpy(s, MSG013); break;
    case 14: strcpy(s, MSG014); break;
    case 15: strcpy(s, MSG015); break;
    case 16: strcpy(s, MSG016); break;
    case 17: strcpy(s, MSG017); break;
    case 18: strcpy(s, MSG018); break;
    case 19: strcpy(s, MSG019); break;
    case 20: strcpy(s, MSG020); break;
    case 21: strcpy(s, MSG021); break;
    case 22: strcpy(s, MSG022); break;
    case 23: strcpy(s, MSG023); break;
    case 24: strcpy(s, MSG024); break;
    case 25: strcpy(s, MSG025); break;
    case 26: strcpy(s, MSG026); break;
    case 27: strcpy(s, MSG027); break;
    case 28: strcpy(s, MSG028); break;
    case 29: strcpy(s, MSG029); break;
    case 30: strcpy(s, MSG030); break;
    case 31: strcpy(s, MSG031); break;
    case 37: strcpy(s, MSG037); break;
    case 38: strcpy(s, MSG038); break;
    case 39: strcpy(s, MSG039); break;
    case 40: strcpy(s, MSG040); break;
    case 43: strcpy(s, MSG043); break;
    case 44: strcpy(s, MSG044); break;
    case 45: strcpy(s, MSG045); break;
    case 46: strcpy(s, MSG046); break;
    default: strcpy(s, "Not implemented error. Please report this bug."); break;

    }
  if(err_num)
    {
      strcat(s, "\n");
      fprintf(stderr, "%s", s);
      update->msg_box("Error", s);
    }
  return err_num;
}
*/

/* New function, the previous one is obsolete */
/* 
   This function put in err_msg the error message corresponding to the 
   error code.
   If the error code has been handled, the function returns 0 else it 
   propagates the error code by returning it.
*/
DLLEXPORT int DLLEXPORT2
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
    strcpy(error_msg, _("Invalid FLASH file"));
      break;
    case ERR_VAR_REFUSED:
      strcpy(error_msg, _("Variable refused."));
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
//      break;
    }

  return 0;
}

