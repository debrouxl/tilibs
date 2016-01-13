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

/* Raw TCP server virtual link cable unit */

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

static int rtcps_prepare(CableHandle *h)
{
	return 0;
}

static int rtcps_open(CableHandle *h)
{
	return 0;
}

static int rtcps_close(CableHandle *h)
{
	return 0;
}

static int rtcps_reset(CableHandle *h)
{
	return 0;
}

static int rtcps_probe(CableHandle *h)
{
	return 0;
}

static int rtcps_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

static int rtcps_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

static int rtcps_check(CableHandle *h, int *status)
{
	return 0;
}

static int rtcps_set_device(CableHandle *h, const char * device)
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

const CableFncts cable_rtcps =
{
	CABLE_RTCPS,
	"RTCPS",
	N_("RTCPS"),
	N_("Virtual raw TCP server link"),
	0,
	&rtcps_prepare,
	&rtcps_open, &rtcps_close, &rtcps_reset, &rtcps_probe, NULL,
	&rtcps_put, &rtcps_get, &rtcps_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&rtcps_set_device
};
