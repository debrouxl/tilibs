/* Hey EMACS -*- linux-c -*- */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 2015  Lionel Debroux
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

/* TCP client virtual link cable unit */

/*
 * This unit uses a TCP socket between 2 programs which use this lib.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <errno.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "../internal.h"
#include "detect.h"

static int tcpc_prepare(CableHandle *h)
{
	// TODO pay attention to cable options.

	return 0;
}

static int tcpc_open(CableHandle *h)
{
	return 0;
}

static int tcpc_close(CableHandle *h)
{
	return 0;
}

static int tcpc_reset(CableHandle *h)
{
	return 0;
}

static int tcpc_probe(CableHandle *h)
{
	return 0;
}

static int tcpc_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

static int tcpc_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

static int tcpc_check(CableHandle *h, int *status)
{
	return 0;
}

static int tcpc_set_device(CableHandle *h, const char * device)
{
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			free(h->device);
			h->device = device2;
		}
		else
		{
			ticables_warning(_("unable to set device %s.\n"), device);
		}
		return 0;
	}
	return ERR_ILLEGAL_ARG;
}

static int tcpc_set_options(CableHandle * h, CableOptions * options)
{
	(void)h, (void)options;
	return ERR_ILLEGAL_ARG;
}

static int tcpc_get_options(CableHandle * h, CableOptions * options)
{
	ticables_copy_cable_options(h, options);
	options->version = 1;
	options->has_parameters = 1;
	options->parameters.parameters_tcpc.connect_address = NULL; // TODO
	options->parameters.parameters_tcpc.port = 0; // TODO
	options->parameters.parameters_tcpc.use_fd = 0; // TODO
	options->parameters.parameters_tcpc.fd = 0; // TODO
	return 0;
}

extern const CableFncts cable_tcpc =
{
	CABLE_TCPC,
	"TCPC",
	N_("TCPC"),
	N_("Virtual TCP client link"),
	0,
	&tcpc_prepare,
	&tcpc_open, &tcpc_close, &tcpc_reset, &tcpc_probe, NULL,
	&tcpc_put, &tcpc_get, &tcpc_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	NULL,
	&tcpc_set_options, &tcpc_get_options
};
