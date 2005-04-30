/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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

#include <glib.h>
#include <string.h>
#include "gettext.h"
#include "export.h"
#include "error.h"
#include "logging.h"

/**
 * tifiles_get_error:
 * @number: error number (see file_err.h for list).
 * @message: a newly allocated string which contains corresponding error *message.
 *
 * Attempt to match the message corresponding to the error number. The returned
 * string must be freed when no longer needed.
 *
 * Return value: 0 if error has been caught, the error number otherwise (propagation).
 **/
TIEXPORT int TICALL tifiles_error_get(TiFileError number, char **message)
{
	char *tmp;

	g_assert (message != NULL);

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
			_("either the file does not exist, either there is no room."),
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
			_("Msg: the size of a group file can not exceed 64KB."),
			"\n",
			_("Cause: too many variables/data."),
			NULL);
    break;

	case ERR_BAD_CALC:
		*message = g_strconcat(
			_("Msg: Unknown calculator type."),
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

	default:
		return number;
    break;
	}

	// don't use GLib allocator
	tmp = strdup(*message);
	g_free(*message);
	*message = tmp;

	return 0;
}
