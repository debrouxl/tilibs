/* Hey EMACS -*- win32-c -*- */
/* $Id: linux_detect.c 397 2004-03-29 17:21:12Z roms $ */

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

/* Win32 probing module */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include "intl.h"

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "externs.h"
#include "verbose.h"

int win32_detect_os(char **os_type)
{

	return 0;
}

int win32_detect_port(TicablePortInfo * pi)
{

	return 0;
}

int win32_detect_resources(void)
{
	DISPLAY(_("libticables: checking resources...\r\n"));
	resources = IO_WIN32;

  return 0;
}
