/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>

#include "timeout.h"
#include "ioports.h"
#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "logging.h"
#include "cabl_ext.h"

int dfl_init()
{
  return 0;
}

int dfl_open()
{
  return 0;
}

int dfl_put(byte data)
{
  return 0;
}

int dfl_get(byte *d)
{
  return 0;
}

int dfl_probe()
{ 
  return 0;
}

int dfl_close()
{
  return 0;
}

int dfl_exit()
{
  return 0;
}

int dfl_check(int *status)
{
  *status = STATUS_NONE;
  return 0;
}

int dfl_set_red_wire(int b)
{
  return 0;
}

int dfl_set_white_wire(int b)
{
  return 0;
}

int dfl_get_red_wire()
{
  return 0;
}

int dfl_get_white_wire()
{
  return 0;
}

int dfl_supported()
{
  return SUPPORT_OFF;
}

int set_default_cable(LINK_CABLE *lc)
{
  lc->init  = dfl_init;
  lc->open  = dfl_open;
  lc->put        = dfl_put;
  lc->get        = dfl_get;
  lc->close = dfl_close;
  lc->exit  = dfl_exit;
  lc->probe = dfl_probe;
  lc->check = dfl_check;
  
  lc->set_red_wire   = dfl_set_red_wire;
  lc->set_white_wire = dfl_set_white_wire;
  lc->get_red_wire   = dfl_get_red_wire;
  lc->get_white_wire = dfl_get_white_wire;

  return 0;
}

