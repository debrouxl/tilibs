/* Hey EMACS -*- linux-c -*- */
/* $Id: printl.c 523 2004-04-08 12:01:23Z roms $ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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

/*
  This unit contains the interface of the libtifiles library.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intl3.h"

#include "export.h"
#include "calc_def.h"

/* 
	Print to stdout as default behaviour unless changed by tifiles_set_print 
	Level: such as "warning", "error", "information", etc. "" = nothing.
*/
static int default_printl(int level, const char *format, ...)
{
        int ret = 0;
	va_list ap;

	fprintf(stdout, "ticalc ");
	if(level != 0)
		fprintf(stdout, "(%s): ", (level == 2) ? _("error") : _("warning"));
	va_start(ap, format);
        ret = vfprintf(stdout, format, ap);
        va_end(ap);

	return ret;
}

TICALC_PRINTL printl = default_printl;

/*
	Change print behaviour (callback).
*/
TIEXPORT TICALC_PRINTL TICALL ticalc_set_printl(TICALC_PRINTL new_printl)
{
  TICALC_PRINTL old_printl = printl;

  printf("printl = %p\n", printl);
  printf("old_printl = %p\n", old_printl);
  printf("new_printl = %p\n", new_printl);

  printl = new_printl;

  return old_printl;
}
