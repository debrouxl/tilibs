/* Hey EMACS -*- linux-c -*- */
/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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

#ifndef __PROBE_H__
#define __PROBE_H__

#include <stdio.h>
#include "export.h"

int TICALL ticable_detect_os(char **os_type);
int TICALL ticable_detect_port(TicablePortInfo * pi);
int TICALL ticable_detect_cable(TicablePortInfo * pi);
int TICALL ticable_detect_all(char **os, TicablePortInfo * pi);

int probe_io_resources(void);

#endif
