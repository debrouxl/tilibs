/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef __WIN32__
#include <sys/time.h>
#endif
#include <string.h>

#include "stdints.h"
#include "gettext.h"
#include "logging.h"
#include "data_log.h"

#define LOG_FILE  "ticables.log"
#define TIME_FILE "ticables.tim"

static char *fn1 = NULL;
static char *fn2 = NULL;

static FILE *log1 = NULL;
static FILE *log2 = NULL;

#ifndef __WIN32__
static struct timeval tv_start;
static struct timezone tz;
#endif

int start_logging()
{
  // build filenames
#ifdef __WIN32__
	fn1 = g_strconcat("C:\\", LOG_FILE, NULL);
	fn2 = g_strconcat("C:\\", TIME_FILE, NULL);
#else
	fn1 = g_strconcat(g_get_home_dir(), "/", LOG_FILE, NULL);
	fn2 = g_strconcat(g_get_home_dir(), "/", TIME_FILE, NULL);
#endif

  	//ticables_info(_("Logging started."));

  	log1 = fopen(fn1, "wt");
  	if (log1 == NULL) 
	{
    		ticables_error(_("Unable to open <%s> for logging.\n"), fn1);
    		return -1;
  	}

  	log2 = fopen(fn2, "wt");
  	if (log2 == NULL) 
	{
    		ticables_error(_("Unable to open <%s> for logging.\n"), fn2);
    		return -1;
  	}
#ifndef __WIN32__
  	memset((void *) (&tz), 0, sizeof(tz));
  	gettimeofday(&tv_start, &tz);
#endif

  	return 0;
}

int log_data(uint8_t d)
{
  	static int array[16];
  	static int i = 0;
  	int j;
  	int c;
#ifndef __WIN32__
  	struct timeval tv;
  	static int k = 0;
#endif

  	if (log1 == NULL)
    		return -1;
  	array[i++] = d;

  	fprintf(log1, "%02X ", d);
  	if (!(i % 16) && (i > 1)) 
	{
    	fprintf(log1, "| ");
    	for (j = 0; j < 16; j++) 
		{
      		c = array[j];
      		if ((c < 32) || (c > 127))
			fprintf(log1, " ");
      		else
			fprintf(log1, "%c", c);
    	}
    	fprintf(log1, "\n");
    	i = 0;
  	}
#ifndef __WIN32__
  	gettimeofday(&tv, &tz);
  	k++;
  	fprintf(log2, "%i: %i.%2i\n", k,
	  (int) (tv.tv_sec - tv_start.tv_sec),
	  (int) (tv.tv_usec - tv_start.tv_usec));
#endif

  	return 0;
}

int log_n_data(uint8_t* d, int n)
{
	int i;

	for(i = 0; i < n; i++)
		log_data(d[i]);
  	
  	return 0;
}

int stop_logging()
{
  	//ticables_info("Logging stopped.");

  	if (log1 != NULL)
    		fclose(log1);
  	if (log2 != NULL)
    		fclose(log2);

  	g_free(fn1);
  	g_free(fn2);

  	return 0;
}

// ---

int start_void()
{
  	return 0;
}

int log_void(int d)
{
  	return 0;
}

int stop_void()
{
  	return 0;
}
