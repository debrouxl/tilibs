/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

/* Allow link cable handling trough exported functions rather than function
   pointers (may be useful for Visual Basic binding) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "stdints.h"


#include "export.h"
#include "cabl_def.h"
#include "cabl_int.h"
#include "externs.h"

static TicableLinkCable tlc;

TIEXPORT int TICALL ticable_link_set_cable(TicableType type)
{
  return ticable_set_cable(type, &tlc);
}

TIEXPORT int TICALL ticable_link_init(void)
{
  return tlc.init();
}

TIEXPORT int TICALL ticable_link_open(void)
{
  return tlc.open();
}

TIEXPORT int TICALL ticable_link_put(uint8_t data)
{
  return tlc.put(data);
}

TIEXPORT int TICALL ticable_link_get(uint8_t * data)
{
  return tlc.get(data);
}

TIEXPORT int TICALL ticable_link_probe(void)
{
  return tlc.probe();
}

TIEXPORT int TICALL ticable_link_close(void)
{
  return tlc.close();
}

TIEXPORT int TICALL ticable_link_exit(void)
{
  return tlc.exit();
}

TIEXPORT int TICALL ticable_link_check(int *status)
{
  return tlc.check(status);
}

TIEXPORT int TICALL ticable_link_set_red(int w)
{
  return tlc.set_red_wire(w);
}

TIEXPORT int TICALL ticable_link_set_white(int w)
{
  return tlc.set_white_wire(w);
}

TIEXPORT int TICALL ticable_link_get_red(void)
{
  return tlc.get_red_wire();
}

TIEXPORT int TICALL ticable_link_get_white(void)
{
  return tlc.get_white_wire();
}

TIEXPORT int TICALL ticable_link_reset(void)
{
	return tlc.reset();
}