/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

/* This unit allow to trace bytes which are transferred between PC
	and TI calculator.
*/

#include <stdio.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define LOGGING
//#undef LOGGING

#ifdef LOGGING
static FILE *log = NULL;
static char *filename = "libTIcables.log";
static int i = 0;
#endif

int START_LOGGING()
{
#ifdef LOGGING
  fprintf(stdout, "Logging tSTARTed.\n");
  log = fopen(filename, "wt");
  if(log == NULL)
    return 1;

  return 0;
#else
  return 0;
#endif
}

int LOG_DATA(int d)
{
#ifdef LOGGING
  static int array[16];
  int j;
  int c;
  
  array[i++] = d;
  fprintf(log, "%02X ", d);
  if(!(i % 16) && (i > 1))// && (i != 8))
    {
      fprintf(log, "| ");
      for(j=0; j<16; j++)
	{
	  c = array[j];
	  if( (c < 32) || (c > 127) )
	    fprintf(log, " ");
	  else
	    fprintf(log, "%c", c);
	}
      fprintf(log, "\n");
      i = 0;
    }
  
  return 0;
#else
  return 0;
#endif
}

int STOP_LOGGING()
{
#ifdef LOGGING
  fprintf(stdout, "Logging stopped.\n");
  if(log != NULL)
    fclose(log);
  
  return 0;
#else
  return 0;
#endif
}
