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

/* This unit allows to trace uint8_ts which are transferred between PC
   and TI calculator.
*/

#include <stdio.h>
#include <stdarg.h>
#ifdef __LINUX__
#include <sys/time.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "verbose.h"

#ifdef __LINUX__
static char *fn1 = "/tmp/libTIcables.log";
static char *fn2 = "/tmp/libTIcables.time";
#else
static char *fn1 = "C:\\libTIcables.log";
static char *fn2 = "C:\\libTIcables.time";
#endif

static FILE *log = NULL;
static FILE *log2 = NULL;
static int i = 0;

#ifdef __LINUX__
static struct timeval tv_start;
static struct timezone tz;
#endif

int start_logging()
{
  DISPLAY("Logging STARTED.\n");
  log = fopen(fn1, "wt");
  if(log == NULL)
    return -1;
  
  log2 = fopen(fn2, "wt");
  if(log2 == NULL)
    return -1;

#ifdef __LINUX__
  memset((void *)(&tz), 0, sizeof(tz));
  gettimeofday(&tv_start, &tz);
#endif

  return 0;
}

int log_data(int d)
{
  static int array[16];
  int j;
  int c;
#ifdef __LINUX__
  struct timeval tv;
  static int k = 0;
#endif
  
  array[i++] = d;

  if(log == NULL)
    return -1;

  fprintf(log, "%02X ", d);
  if(!(i % 16) && (i > 1))
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
#ifdef __LINUX__  
  gettimeofday(&tv, &tz);
  k++;
  fprintf(log2, "%i: %i.%2i\n", k, 
	  (int)(tv.tv_sec - tv_start.tv_sec), 
	  (int)(tv.tv_usec - tv_start.tv_usec));
#endif

  return 0;
}

int stop_logging()
{
  fprintf(stdout, "Logging stopped.\n");
  if(log != NULL)
    fclose(log);
  if(log2 != NULL)
    fclose(log2);
  
  return 0;
}

int start_void()
{ return 0; }

int log_void(int d)
{ return 0; }

int stop_void()
{ return 0; }
