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

#include "intl.h"
#include "file_def.h"


TIEXPORT const char *TICALL tifiles_calctype_to_string(TicalcType type)
{
  switch (type) {
  case CALC_NONE:
    return "none";
  case CALC_V200:
    return "V200";
  case CALC_TI92P:
    return "TI92+";
  case CALC_TI92:
    return "TI92";
  case CALC_TI89:
    return "TI89";
  case CALC_TI86:
    return "TI86";
  case CALC_TI85:
    return "TI85";
  case CALC_TI83P:
    return "TI83+";
  case CALC_TI83:
    return "TI83";
  case CALC_TI82:
    return "TI82";
  case CALC_TI73:
    return "TI73";
  default:
    fprintf(stderr, _("libtifiles error: unknown calc type !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL tifiles_attribute_to_string(TifileAttr atrb)
{
  switch (atrb) {
  case ATTRB_NONE:
    return "none     ";
  case ATTRB_LOCKED:
    return "locked   ";
  case ATTRB_ARCHIVED:
    return "archived ";
  case ATTRB_PROTECTED:
    return "protected";
  default:
    fprintf(stderr, _("libtifiles error: unknown attribute !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL tifiles_filetype_to_string(TifileType type)
{
  switch (type) {
  case TIFILE_SINGLE:
    return "single";
  case TIFILE_GROUP:
    return "group";
  case TIFILE_BACKUP:
    return "backup";
  case TIFILE_FLASH:
    return "flash";
  default:
    return "unknown";
  }
}
