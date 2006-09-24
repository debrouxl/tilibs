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

#include <string.h>
#include "gettext.h"
#include "ticalcs.h"

/**
 * ticalcs_model_to_string:
 * @model: a calculator model.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "TI92+".
 **/
TIEXPORT3 const char *TICALL ticalcs_model_to_string(CalcModel model)
{
	return tifiles_model_to_string(model);
}

/**
 * ticalcs_string_to_model:
 * @str: a calculator model as string like "TI92".
 *
 * Do a string to integer conversion.
 *
 * Return value: a calculator model.
 **/
TIEXPORT3 CalcModel TICALL ticalcs_string_to_model(const char *str)
{
	return tifiles_string_to_model(str);
}


TIEXPORT3 const char *TICALL ticalcs_scrfmt_to_string(CalcScreenFormat format)
{
  	switch (format) 
	{
  	case SCREEN_FULL: return _("full");
  	case SCREEN_CLIPPED: return _("clipped");
  	default: return _("unknown");
  	}
}

TIEXPORT3 CalcScreenFormat TICALL ticalc_string_to_scrfmt(const char *str)
{
  	if(!strcmp(str, _("full")))
  		return SCREEN_FULL;
  	else if(!strcmp(str, _("clipped")))
  		return SCREEN_CLIPPED;
  		
  	return SCREEN_CLIPPED;
}


TIEXPORT3 const char *TICALL ticalc_pathtype_to_string(CalcPathType type)
{
  	switch (type) 
	{
  	case PATH_FULL: return _("full");
  	case PATH_LOCAL: return _("local");
  	default: return _("unknown");
  	}
}

TIEXPORT3 CalcPathType TICALL ticalc_string_to_pathtype(const char *str)
{
  	if(!strcmp(str, _("full")))
  		return PATH_FULL;
  	else if(!strcmp(str, _("local")))
  		return PATH_LOCAL;
  	
  	return PATH_FULL;
}


TIEXPORT3 const char *TICALL ticalc_memtype_to_string(CalcMemType type)
{
  	switch (type) 
	{
  	case MEMORY_FREE: return _("free");
  	case MEMORY_USED: return _("used");
  	default: return _("unknown");
  	}
}

TIEXPORT3 CalcMemType TICALL ticalc_string_to_memtype(const char *str)
{
  	if(!strcmp(str, _("free")))
  		return MEMORY_FREE;
  	else if(!strcmp(str, _("used")))
  		return MEMORY_USED;
  	
  	return MEMORY_NONE;
}
