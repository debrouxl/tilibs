/* Hey EMACS -*- linux-c -*- */
/* $Id: nul_link.c 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#include "timeout.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "logging.h"
#include "externs.h"


int nul_init()
{
  return 0;
}

int nul_open()
{
  return 0;
}

int nul_put(uint8_t data)
{
  return 0;
}

int nul_get(uint8_t * d)
{
  return 0;
}

int nul_probe()
{
  return 0;
}

int nul_close()
{
  return 0;
}

int nul_exit()
{
  return 0;
}

int nul_check(int *status)
{
  *status = STATUS_NONE;
  return 0;
}

int nul_set_red_wire(int b)
{
  return 0;
}

int nul_set_white_wire(int b)
{
  return 0;
}

int nul_get_red_wire()
{
  return 1;
}

int nul_get_white_wire()
{
  return 1;
}

int nul_supported()
{
  return SUPPORT_OFF;
}

int nul_reset()
{
	return 0;
}

int nul_register_cable(TicableLinkCable * lc, TicableMethod method)
{
  lc->init = nul_init;
  lc->open = nul_open;
  lc->put = nul_put;
  lc->get = nul_get;
  lc->close = nul_close;
  lc->exit = nul_exit;
  lc->probe = nul_probe;
  lc->check = nul_check;
  lc->reset = nul_reset;

  lc->set_red_wire = nul_set_red_wire;
  lc->set_white_wire = nul_set_white_wire;
  lc->get_red_wire = nul_get_red_wire;
  lc->get_white_wire = nul_get_white_wire;

  return 0;
}
