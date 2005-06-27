/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>

#include "gettext.h"
#include "logging.h"
#include "ticables.h"

/**
 * ticables_probe:
 * @array: address of an array of integers to put result. 
 *
 * Returns cables which have been detected. All cables must be CLOSED before !
 * The array contains 5 columns (PORT_0 to PORT_4) and 5 lines (CABLE_GRY to CABLE_USB).
 * The array must be freed when no longer used.
 *
 * Return value: always 0.
 **/
TIEXPORT int TICALL ticables_probing_do(int ***result, int timeout)
{
	CablePort port;
	CableModel model;
	int **array;

	ticables_info(_("Link cable probing:"));
	array = *result = (int **)calloc(5, sizeof(int));

	for(model = CABLE_GRY; model <= CABLE_USB; model++)
	{
		array[model] = (int *)calloc(5, sizeof(int));

		for(port = PORT_1; port <= PORT_4; port++)
		{
			CableHandle* handle;
			int err, ret;

			handle = ticables_handle_new(model, port);
			if(handle)
			{
				ticables_options_set_timeout(handle, timeout);
				err = ticables_cable_probe(handle, &ret);
				array[model][port] = (ret && !err) ? 1: 0;
			}
			ticables_handle_del(handle);
		}
		/*
		ticables_info(_(" %i: %i %i %i %i"), model, 
			array[model][1], array[model][2], array[model][3], array[model][4]);
		*/
	}

	return 0;
}

TIEXPORT int TICALL ticables_probing_finish(int ***result)
{
	int i;

	for(i = 1; i <= 5; i++)
		free((*result)[i]);
	free(*result);
	*result = NULL;

	return 0;
}