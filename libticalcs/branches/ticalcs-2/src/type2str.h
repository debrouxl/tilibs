/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#ifndef __TICABLE_INTERFACE__
#define __TICABLE_INTERFACE__

#include "export.h"
#include "cabl_def.h"

TIEXPORT const char *TICALL ticable_cabletype_to_string(TicableType type);
TIEXPORT const char *TICALL ticable_baudrate_to_string(TicableBaudRate br);
TIEXPORT const char *TICALL ticable_hfc_to_string(TicableHfc hfc);
TIEXPORT const char *TICALL ticable_port_to_string(TicablePort port);
TIEXPORT const char *TICALL ticable_method_to_string(TicableMethod method);

#endif
