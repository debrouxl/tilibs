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

#include <stdio.h>
#include <string.h>
#include <glib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tifiles.h"
#include "gettext.h"
#include "export2.h"
#include "error.h"
#include "logging.h"

/**
 * tifiles_get_error:
 * @number: error number (see file_err.h for list).
 * @message: a newly glib allocated string which contains corresponding error *message.
 *
 * Attempt to match the message corresponding to the error number. The returned
 * string must be freed when no longer needed.
 *
 * Return value: 0 if error has been caught, the error number otherwise (propagation).
 **/
int TICALL tifiles_error_get(int number, char **message)
{
	if (message == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return number;
	}

	switch (number) 
	{
	case ERR_MALLOC:
		*message = g_strconcat(
			_("Msg: unable to allocate memory (malloc)."),
			"\n",
			_("Cause: memory too low ?"), 
			NULL);
	break;

	case ERR_FILE_OPEN:
		*message = g_strconcat(
			_("Msg: unable to open file."),
			"\n",
			_("Cause: either the file does not exist, either there is no room."),
			NULL);
	break;

	case ERR_FILE_CLOSE:
		*message = g_strconcat(
			_("Msg: unable to close file."),
			"\n",
			_("Cause: either the file does not exist, either there is no room."),
			NULL);
	break;

	case ERR_GROUP_SIZE:
		*message = g_strconcat(
			_("Msg: the size of a group file cannot exceed 64 KB."),
			"\n",
			_("Cause: too many variables/data."),
			NULL);
	break;

	case ERR_BAD_CALC:
		*message = g_strconcat(
			_("Msg: unknown calculator type."),
			"\n",
			_("Cause: probably due to a bug, mail to: tilp-users@lists.sf.net."),
			NULL);
	break;

	case ERR_INVALID_FILE:
	case ERR_BAD_FILE:
		*message = g_strconcat(
			_("Msg: invalid file."),
			"\n",
			_("Cause: it's probably not a TI formatted file."),
			NULL);
	break;

	case ERR_FILE_CHECKSUM:
		*message = g_strconcat(
			_("Msg: checksum error."),
			"\n",
			_("Cause: the file has an incorrect checksum and may be corrupted."),
			NULL);
	break;

	case ERR_FILE_ZIP:
		*message = g_strconcat(
			_("Msg: (Un)Zip error."),
			"\n",
			_("Cause: unknown..."),
			NULL);
	break;

	case ERR_UNSUPPORTED:
		*message = g_strconcat(
			_("Msg: unsupported function."),
			"\n",
			_("Cause: the function you attempted to use is not available."),
			NULL);
	break;

	case ERR_FILE_IO:
		*message = g_strconcat(
			_("Msg: I/O file error."),
			"\n",
			_("Cause: file is corrupted or invalid."),
			NULL);
	break;

	case ERR_INVALID_PARAM:
		*message = g_strconcat(
			_("Msg: invalid parameter."),
			"\n",
			_("Cause: the program which uses this library is buggy. Fire-up the developer!"),
			NULL);
	break;

	default:
		// propagate error code
		return number;
	}

	return 0;
}

/**
 * tifiles_error_free:
 * @message: a message previously allocated by tifiles_error_get()
 *
 * Free the given message string allocated by tifiles_error_get();
 *
 * Return value: 0 if the argument was valid and the message was freed, nonzero otherwise.
 **/
int TICALL tifiles_error_free(char *message)
{
	if (message == nullptr)
	{
		tifiles_critical("tifiles_error_free(NULL)\n");
		return ERR_INVALID_PARAM;
	}
	g_free(message);
	return 0;
}
