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

#include "intl2.h"
#include "calc_def.h"


TIEXPORT const char *TICALL ticalc_screen_to_string(TicalcScreenFormat format)
{
  	switch (format) {
  	case SCREEN_FULL: return _("full");
  	case SCREEN_CLIPPED: return _("clipped");
  	default: return _("unknown");
  	}
}

TIEXPORT TicalcScreenFormat TICALL ticalc_string_to_screen(const char *str)
{
  	if(!strcmp(str, _("full")))
  		return SCREEN_FULL;
  	else if(!strcmp(str, _("clipped")))
  		return SCREEN_CLIPPED;
  		
  	return SCREEN_CLIPPED;
}


TIEXPORT const char *TICALL ticalc_path_to_string(TicalcPathType type)
{
  	switch (type) {
  	case PATH_FULL: return _("full");
  	case PATH_LOCAL: return _("local");
  	default: return _("unknown");
  	}
}

TIEXPORT TicalcPathType TICALL ticalc_string_to_path(const char *str)
{
  	if(!strcmp(str, _("full")))
  		return PATH_FULL;
  	else if(!strcmp(str, _("local")))
  		return PATH_LOCAL;
  	
  	return PATH_FULL;
}




TIEXPORT const char *TICALL ticalc_action_to_string(TicalcAction action)
{
  	switch (action) {
  	case ACT_SKIP: return _("skip");
  	case ACT_OVER: return _("overwrite");
	default: return _("unknown");
  	}
}
