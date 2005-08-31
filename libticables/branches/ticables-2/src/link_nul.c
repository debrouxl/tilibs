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

/* Initialize the LinkCable structure with default functions */
/* This module can be used as sample code.*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "stdints.h"

#include "ticables.h"
#include "gettext.h"

int nul_prepare(CableHandle *h)
{
	return 0;
}

int nul_probe(CableHandle *h)
{
	return 0;
}

int nul_open(CableHandle *h)
{
	return 0;
}

int nul_close(CableHandle *h)
{
	return 0;
}

int nul_reset(CableHandle *h)
{
	return 0;
}

int nul_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

int nul_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	return 0;
}

int nul_check(CableHandle *h, int *status)
{
	*status = STATUS_NONE;
	return 0;
}

int nul_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

int nul_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

int nul_get_red_wire(CableHandle *h)
{
	return 1;
}

int nul_get_white_wire(CableHandle *h)
{
	return 1;
}

const CableFncts cable_nul = 
{
	CABLE_NUL,
	"NUL",
	N_("Dummy link"),
	N_("Dummy link used when no cable is set"),
	0,
	&nul_prepare,
	&nul_open, &nul_close, &nul_reset, &nul_probe, NULL,
	&nul_put, &nul_get, &nul_check,
	&nul_set_red_wire, &nul_set_white_wire,
	&nul_get_red_wire, &nul_get_white_wire,
};

/* no const ! */ CableFncts cable_ilp =
{
	CABLE_ILP,
	"ILP",
	N_("Dummy link for TiEmu"),
	N_("Internal Link Port cable for TiEmu (internal use)"),
	!0,
	&nul_prepare,
	&nul_open, &nul_close, &nul_reset, &nul_probe, NULL,
	&nul_put, &nul_get, &nul_check,
	&nul_set_red_wire, &nul_set_white_wire,
	&nul_get_red_wire, &nul_get_white_wire,
};