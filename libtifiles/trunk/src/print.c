/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
#include "file_ver.h"
#include "file_int.h"
#include "file_def.h"

/* 
	Print to stdout as default behaviour unless changed by tifiles_set_print 
	Level: such as "warning", "error", "information", etc. "" = nothing.
*/
int default_print(const char *level, const char *format, ...)
{
	va_list ap;
	int ret;

	fprintf(stdout, "libtifiles ");
	if(strcmp(level, ""))
		fprintf(stdout, "(%s): ", level);
	va_start(ap, format);
        ret = vfprintf(stdout, format, ap);
        va_end(ap);

	return ret;
}

TIFILES_PRINT print = default_print;

/*
	Change print behaviour (callback).
*/
TIEXPORT TIFILES_PRINT tifiles_set_print(TIFILES_PRINT new_print)
{
  TIFILES_PRINT old_print = print;

  printf("printf = %p\n", print);
  printf("old_printf = %p\n", old_print);
  printf("new_printf = %p\n", new_print);

  print = new_print;

  return old_print;
}
