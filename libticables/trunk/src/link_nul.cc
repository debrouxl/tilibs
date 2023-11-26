/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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

/* Initialize the LinkCable structure with default functions */
/* This module can be used as sample code.*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include "ticables.h"
#include "gettext.h"
#include "internal.h"

int noop_prepare(CableHandle *h)
{
	(void)h;
	return 0;
}

int noop_probe(CableHandle *h)
{
	(void)h;
	return 0;
}

int noop_open(CableHandle *h)
{
	(void)h;
	return 0;
}

int noop_close(CableHandle *h)
{
	(void)h;
	return 0;
}

int noop_reset(CableHandle *h)
{
	(void)h;
	return 0;
}

int noop_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	(void)h, (void)data, (void)len;
	return 0;
}

int noop_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	(void)h, (void)data, (void)len;
	return 0;
}

int noop_check(CableHandle *h, int *status)
{
	(void)h;
	*status = STATUS_NONE;
	return 0;
}

int noop_set_red_wire(CableHandle *h, int b)
{
	(void)h, (void)b;
	return 0;
}

int noop_set_white_wire(CableHandle *h, int b)
{
	(void)h, (void)b;
	return 0;
}

int noop_get_red_wire(CableHandle *h)
{
	(void)h;
	return 1;
}

int noop_get_white_wire(CableHandle *h)
{
	(void)h;
	return 1;
}

int noop_set_device(CableHandle *h, const char * device)
{
	(void)h, (void)device;
	return 0;
}

extern const CableFncts cable_nul = 
{
	CABLE_NUL,
	"NUL",
	N_("Dummy link"),
	N_("Dummy link used when no cable is set"),
	0,
	&noop_prepare,
	&noop_open, &noop_close, &noop_reset, &noop_probe, nullptr,
	&noop_put, &noop_get, &noop_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	nullptr, nullptr,
	&noop_set_device,
	nullptr
};

/* no const ! */ CableFncts cable_ilp =
{
	CABLE_ILP,
	"ILP",
	N_("Dummy link for TiEmu"),
	N_("Internal Link Port cable for TiEmu (internal use)"),
	!0,
	&noop_prepare,
	&noop_open, &noop_close, &noop_reset, &noop_probe, nullptr,
	&noop_put, &noop_get, &noop_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	nullptr, nullptr,
	&noop_set_device,
	nullptr
};
