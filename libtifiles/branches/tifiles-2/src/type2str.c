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
TIEXPORT const char *TICALL tifiles_model_to_string(TiCalcModel model)
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
TIEXPORT TiCalcModel TICALL tifiles_string_to_model(const char *str)
{
	if(!strcmp(str, "TI73"))
		return CALC_TI73;
	else if(!strcmp(str, "TI82"))
		return CALC_TI82;
	else if(!strcmp(str, "TI83"))
		return CALC_TI83;
	else if(!strcmp(str, "TI83+"))
		return CALC_TI83P;
	else if(!strcmp(str, "TI84+"))
		return CALC_TI84P;
	else if(!strcmp(str, "TI85"))
		return CALC_TI85;
	else if(!strcmp(str, "TI86"))
		return CALC_TI86;
	else if(!strcmp(str, "TI89"))
		return CALC_TI89;
	else if(!strcmp(str, "TI89t"))
		return CALC_TI89T;
	else if(!strcmp(str, "TI92"))
		return CALC_TI92;
	else if(!strcmp(str, "TI92+"))
		return CALC_TI92P;
	else if(!strcmp(str, "V200"))
		return CALC_V200;
		
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
TIEXPORT const char *TICALL tifiles_attribute_to_string(TiFileAttr attrb)
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
TIEXPORT TiFileAttr TICALL tifiles_string_to_attribute(const char *str)
{
	if(!strcmp(str, _("none     ")))
		return ATTRB_NONE;
	else if(!strcmp(str, _("locked   ")))
		return ATTRB_LOCKED;
	else if(!strcmp(str, _("archived ")))
		return ATTRB_ARCHIVED;
	else if(!strcmp(str, _("protected")))
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
TIEXPORT const char *TICALL tifiles_class_to_string(TiFileClass klass)
{
  	switch (klass) 
	{
  	case TIFILE_SINGLE: return _("single");
  	case TIFILE_GROUP:  return _("group");
  	case TIFILE_BACKUP: return _("backup");
  	case TIFILE_FLASH:  return _("flash");
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
TIEXPORT TiFileClass TICALL tifiles_string_to_class(const char *str)
{
	if(!strcmp(str, _("single")))
		return TIFILE_SINGLE;
	else if(!strcmp(str, _("group")))
		return TIFILE_GROUP;
	else if(!strcmp(str, _("backup")))
		return TIFILE_BACKUP;
	else if(!strcmp(str, _("flash")))
		return TIFILE_FLASH;
		
	return TIFILE_SINGLE;
}
