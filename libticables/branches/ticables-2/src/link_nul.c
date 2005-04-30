/* Hey EMACS -*- linux-c -*- */
/* $Id: nul_link.c 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

int nul_prepare(TiHandle *h)
{
	return 0;
}

int nul_open(TiHandle *h)
{
	return 0;
}

int nul_close(TiHandle *h)
{
	return 0;
}

int nul_reset(TiHandle *h)
{
	return 0;
}

int nul_put(TiHandle *h, uint8_t data)
{
	return 0;
}

int nul_get(TiHandle *h, uint8_t *data)
{
	return 0;
}

int nul_probe(TiHandle *h)
{
	return 0;
}

int nul_check(TiHandle *h, int *status)
{
	*status = STATUS_NONE;
	return 0;
}

int nul_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

int nul_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

int nul_get_red_wire(TiHandle *h)
{
	return 1;
}

int nul_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_nul = 
{
	CABLE_NUL,
	"NUL",
	N_("Dummy link"),
	N_("Dummy link used when no cable is set"),

	&nul_prepare, 
	&nul_open, &nul_close,
	&nul_reset,
	&nul_put, &nul_get,
	&nul_check,
	&nul_set_red_wire, &nul_set_white_wire,
	&nul_get_red_wire, &nul_get_white_wire,
};