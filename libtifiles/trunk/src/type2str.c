/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
#include <glib.h>

#include "gettext.h"
#include "tifiles.h"

/**
 * tifiles_model_to_string:
 * @model: a calculator model.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "TI92+".
 **/
TIEXPORT2 const char *TICALL tifiles_model_to_string(CalcModel model)
{
  	switch (model) 
	{
  	case CALC_NONE:  return "none";
  	case CALC_V200:  return "V200";
  	case CALC_TI92P: return "TI92+";
  	case CALC_TI92:  return "TI92";
	case CALC_TI89T: return "TI89t";
  	case CALC_TI89:  return "TI89";
  	case CALC_TI86:  return "TI86";
  	case CALC_TI85:  return "TI85";
	case CALC_TI84P: return "TI84+";
  	case CALC_TI83P: return "TI83+";
  	case CALC_TI83:  return "TI83";
  	case CALC_TI82:  return "TI82";
  	case CALC_TI73:  return "TI73";
	case CALC_TI84P_USB: return "TI84+ USB";
	case CALC_TI89T_USB: return "TI89t USB";
  	default: return "unknown";
  	}
}

/**
 * tifiles_string_to_model:
 * @str: a calculator model as string like "TI92".
 *
 * Do a string to integer conversion.
 *
 * Return value: a calculator model.
 **/
TIEXPORT2 CalcModel TICALL tifiles_string_to_model(const char *str)
{
	if(!g_ascii_strcasecmp(str, "TI73"))
		return CALC_TI73;
	else if(!g_ascii_strcasecmp(str, "TI82"))
		return CALC_TI82;
	else if(!g_ascii_strcasecmp(str, "TI83"))
		return CALC_TI83;
	else if(!g_ascii_strcasecmp(str, "TI83+"))
		return CALC_TI83P;
	else if(!g_ascii_strcasecmp(str, "TI84+"))
		return CALC_TI84P;
	else if(!g_ascii_strcasecmp(str, "TI85"))
		return CALC_TI85;
	else if(!g_ascii_strcasecmp(str, "TI86"))
		return CALC_TI86;
	else if(!g_ascii_strcasecmp(str, "TI89"))
		return CALC_TI89;
	else if(!g_ascii_strcasecmp(str, "TI89t"))
		return CALC_TI89T;
	else if(!g_ascii_strcasecmp(str, "TI92"))
		return CALC_TI92;
	else if(!g_ascii_strcasecmp(str, "TI92+"))
		return CALC_TI92P;
	else if(!g_ascii_strcasecmp(str, "V200"))
		return CALC_V200;
	else if(!g_ascii_strcasecmp(str, "TI84+ USB"))
		return CALC_TI84P_USB;
	else if(!g_ascii_strcasecmp(str, "TI89t USB"))
		return CALC_TI89T_USB;
		
	return CALC_NONE;
}

/**
 * tifiles_attribute_to_string:
 * @attrb: an attribute of variable.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "archived".
 **/
TIEXPORT2 const char *TICALL tifiles_attribute_to_string(FileAttr attrb)
{
  	switch (attrb) 
	{
  	case ATTRB_NONE:      return _("none     ");
  	case ATTRB_LOCKED:    return _("locked   ");
  	case ATTRB_ARCHIVED:  return _("archived ");
  	case ATTRB_PROTECTED: return _("protected");
  	default: return "unknown";
  	}
}

/**
 * tifiles_string_to_attribute:
 * @str: a variable attribute string like "protected".
 *
 * Do a string to integer conversion.
 *
 * Return value: a variable attribute.
 **/
TIEXPORT2 FileAttr TICALL tifiles_string_to_attribute(const char *str)
{
	if(!g_ascii_strcasecmp(str, _("none     ")))
		return ATTRB_NONE;
	else if(!g_ascii_strcasecmp(str, _("locked   ")))
		return ATTRB_LOCKED;
	else if(!g_ascii_strcasecmp(str, _("archived ")))
		return ATTRB_ARCHIVED;
	else if(!g_ascii_strcasecmp(str, _("protected")))
		return ATTRB_PROTECTED;
	
	return ATTRB_NONE;
}

/**
 * tifiles_class_to_string:
 * @klass: a class of file.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "backup".
 **/
TIEXPORT2 const char *TICALL tifiles_class_to_string(FileClass klass)
{
  	switch (klass) 
	{
  	case TIFILE_SINGLE: return _("single");
  	case TIFILE_GROUP:  return _("group");
  	case TIFILE_BACKUP: return _("backup");
  	case TIFILE_FLASH:  return _("flash");
	case TIFILE_TIGROUP:  return _("tigroup");
  	default: return _("unknown");
  	}
}

/**
 * tifiles_string_to_class:
 * @str: a file class string like "backup".
 *
 * Do a string to integer conversion.
 *
 * Return value: a file class.
 **/
TIEXPORT2 FileClass TICALL tifiles_string_to_class(const char *str)
{
	if(!g_ascii_strcasecmp(str, _("single")))
		return TIFILE_SINGLE;
	else if(!g_ascii_strcasecmp(str, _("group")))
		return TIFILE_GROUP;
	else if(!g_ascii_strcasecmp(str, _("backup")))
		return TIFILE_BACKUP;
	else if(!g_ascii_strcasecmp(str, _("flash")))
		return TIFILE_FLASH;
	else if(!g_ascii_strcasecmp(str, _("tigroup")))
		return TIFILE_TIGROUP;
		
	return TIFILE_SINGLE;
}
