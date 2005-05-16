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

#include <string.h>

#include "ticalcs.h"

#ifdef __WIN32__
#define strcasecmp _stricmp
#endif

#define MAX_FORMAT 8

const char *TI_CLOCK[MAX_FORMAT + 2] = 
{
  "",
  "MM/DD/YY",
  "DD/MM/YY",
  "MM.DD.YY",
  "DD.MM.YY",
  "YY.MM.DD",
  "MM-DD-YY",
  "DD-MM-YY",
  "YY-MM-DD",
  ""
};

TIEXPORT const char *TICALL ticalcs_clock_format2date(int value)
{
  int v;

  if (value < 1)
    v = 1;
  else if (value > MAX_FORMAT)
    v = MAX_FORMAT;
  else
    v = value;

  return TI_CLOCK[v];
}

TIEXPORT int TICALL ticalcs_clock_date2format(const char *format)
{
  int i;

  for (i = 1; i <= MAX_FORMAT; i++) 
  {
    if (!strcasecmp(TI_CLOCK[i], format))
      break;
  }
  if (i > MAX_FORMAT)
    return 1;

  return i;
}
