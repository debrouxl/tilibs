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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
const char *TICALL tifiles_model_to_string(CalcModel model)
{
	return ticonv_model_to_string(model);
}

/**
 * tifiles_string_to_model:
 * @str: a calculator model as string like "TI92".
 *
 * Do a string to integer conversion.
 *
 * Return value: a calculator model.
 **/
CalcModel TICALL tifiles_string_to_model(const char *str)
{
	return ticonv_string_to_model(str);
}

/**
 * tifiles_attribute_to_string:
 * @attrb: an attribute of variable.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "archived".
 **/
const char *TICALL tifiles_attribute_to_string(FileAttr attrb)
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
FileAttr TICALL tifiles_string_to_attribute(const char *str)
{
	if (str != nullptr)
	{
		if(!g_ascii_strcasecmp(str, _("none     ")))
			return ATTRB_NONE;
		else if(!g_ascii_strcasecmp(str, _("locked   ")))
			return ATTRB_LOCKED;
		else if(!g_ascii_strcasecmp(str, _("archived ")))
			return ATTRB_ARCHIVED;
		else if(!g_ascii_strcasecmp(str, _("protected")))
			return ATTRB_PROTECTED;
	}

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
const char *TICALL tifiles_class_to_string(FileClass klass)
{
	switch (klass)
	{
	case TIFILE_SINGLE: return _("single");
	case TIFILE_GROUP:  return _("group");
	case TIFILE_REGULAR:return _("regular");
	case TIFILE_BACKUP: return _("backup");
	case TIFILE_TIGROUP:return _("tigroup");
	case TIFILE_OS:		return _("os");
	case TIFILE_APP:	return _("application");
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
FileClass TICALL tifiles_string_to_class(const char *str)
{
	if (str != nullptr)
	{
		if(!g_ascii_strcasecmp(str, _("single")))
			return TIFILE_SINGLE;
		else if(!g_ascii_strcasecmp(str, _("group")))
			return TIFILE_GROUP;
		else if(!g_ascii_strcasecmp(str, _("regular")))
			return TIFILE_REGULAR;
		else if(!g_ascii_strcasecmp(str, _("backup")))
			return TIFILE_BACKUP;
		else if(!g_ascii_strcasecmp(str, _("os")))
			return TIFILE_OS;
		else if(!g_ascii_strcasecmp(str, _("application")))
			return TIFILE_APP;
		else if(!g_ascii_strcasecmp(str, _("flash")))
			return TIFILE_FLASH;
		else if(!g_ascii_strcasecmp(str, _("tigroup")))
			return TIFILE_TIGROUP;
	}

	return TIFILE_NONE;
}
