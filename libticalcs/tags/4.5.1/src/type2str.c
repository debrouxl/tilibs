/* Hey EMACS -*- linux-c -*- */
/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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

#include "intl2.h"
#include "calc_def.h"


TIEXPORT const char *TICALL ticalc_screen_to_string(TicalcScreenFormat
						    format)
{
  switch (format) {
  case FULL_SCREEN:
    return _("full");
  case CLIPPED_SCREEN:
    return _("clipped");
  default:
    DISPLAY_ERROR(_("libticalcs error: unknown screen format !\n"));
    return _("unknown");
  }
}


TIEXPORT const char *TICALL ticalc_path_to_string(TicalcPathType type)
{
  switch (type) {
  case FULL_PATH:
    return "full";
  case LOCAL_PATH:
    return "local";
  default:
    DISPLAY_ERROR(_("libticalcs error: unknown path type !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL ticalc_action_to_string(TicalcAction action)
{
  switch (action) {
  case ACT_SKIP:
    return "skip";
  case ACT_OVER:
    return "overwrite";
  default:
    DISPLAY_ERROR(_("libticalcs error: unknown action !\n"));
    return "unknown";
  }
}
