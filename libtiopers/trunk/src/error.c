/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 2013-2016  Lionel Debroux
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

#include "config.h"

#include "gettext.h"
#include "tiopers.h"
#include "logging.h"
#include "error.h"

/**
 * tiopers_error_get:
 * @number: error number (see error.h for list).
 * @message: a newly glib allocated string which contains corresponding error *message.
 *
 * Attempt to match the message corresponding to the error number. The returned
 * string must be freed when no longer needed.
 * The error message has the following format:
 * - 1: the error message
 * - 2: the cause(s), explanations on how to fix it
 * - 3: the error returned by the system
 *
 * Return value: 0 if error has been caught, the error number otherwise (propagation).
 **/
TIEXPORT5 int TICALL tiopers_error_get(int number, char **message)
{
	if (message == NULL)
	{
		tiopers_critical("tiopers_error_get(NULL)\n");
		return number;
	}

	switch(number)
	{
	case ERR_MALLOC:
		break;

	case ERR_NO_CABLE:
		*message = g_strconcat(
		_("The cable cannot be used."),
		"\n",
		_("Cause: the cable has not been initialized due to a previous/current error."),
		NULL);
		break;

	case ERR_NO_CALC:
		*message = g_strconcat(
		_("The calc cannot be used."),
		"\n",
		_("Cause: the calc has not been initialized due to a previous/current error."),
		NULL);
		break;

	case ERR_BUSY:
		break;

	case ERR_ATTACHING_CABLE:
		*message = g_strconcat(
		_("The cable cannot be attached."),
		"\n",
		_("Cause: the cable is invalid due to a previous/current error."),
		NULL);
		break;

	case ERR_ATTACHING_CALC:
		*message = g_strconcat(
		_("The calc cannot be attached."),
		"\n",
		_("Cause: the calc is invalid due to a previous/current error."),
		NULL);
		break;

	case ERR_INVALID_HANDLE:
		*message = g_strconcat(
		_("Invalid tiopers handle."),
		"\n",
		_("Cause: bad cable or calc model."),
		NULL);
		break;

	case ERR_INVALID_PARAMETER:
		*message = g_strconcat(
		_("Invalid parameter."),
		"\n",
		_("Cause: if you see this in normal usage, a bug: drop us an email ! Otherwise, check the arguments you're passing to the function."),
		NULL);
		break;

	case ERR_SAVE_FILE:
		*message = g_strconcat(
		_("Msg: unable to open (writing) the file."),
		"\n",
		_("Cause: disk space or permissions."),
		NULL);
		break;

	case ERR_UNSUPPORTED:
		*message = g_strconcat(
		_("Msg: this function does not exist for this calculator."),
		"\n",
		_("Cause: the operation you attempted is not supported by your calculator model."),
		NULL);
		break;

	default:
		// propagate error code
		return number;
	}

	return 0;
}


/**
 * tiopers_error_free:
 * @message: a message previously allocated by tiopers_error_get()
 *
 * Free the given message string allocated by tiopers_error_get();
 *
 * Return value: 0 if the argument was valid and the message was freed, nonzero otherwise.
 **/
TIEXPORT5 int TICALL tiopers_error_free(char *message)
{
	if (message == NULL)
	{
		tiopers_critical("tiopers_error_free(NULL)\n");
		return ERR_INVALID_PARAMETER;
	}
	g_free(message);
	return 0;
}
