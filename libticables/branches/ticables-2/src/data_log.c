/* Hey EMACS -*- linux-c -*- */
/* $Id: logging.c 665 2004-04-29 18:25:16Z tijl $ */

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

/* This unit allows to trace uint8_ts which are transferred between PC
   and TI calculator.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef __WIN32__
#include <sys/time.h>
#endif
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#include "logging.h"

#define LOG_FILE  "libticables.log"
#define TIME_FILE "libticables.time"

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
  	fn1 = (char *) malloc(strlen(LOG_FILE) + strlen("C:\\"));
  	fn2 = (char *) malloc(strlen(TIME_FILE) + strlen("C:\\"));

  	strcpy(fn1, "C:\\" LOG_FILE);
  	strcpy(fn2, "C:\\" TIME_FILE);
#else
  	char *home_dir = getenv("HOME");

  	fn1 = (char *) malloc(strlen(home_dir) + strlen(LOG_FILE) + 2);
  	fn2 = (char *) malloc(strlen(home_dir) + strlen(TIME_FILE) + 2);

  	strcpy(fn1, home_dir);
  	strcat(fn1, "/");
  	strcat(fn1, LOG_FILE);
  	strcpy(fn2, home_dir);
  	strcat(fn2, "/");
  	strcat(fn2, TIME_FILE);
#endif

  	ticables_info(_("Logging STARTED.\n"));

  	log1 = fopen(fn1, "wt");
  	if (log1 == NULL) {
    		ticables_error(_("Unable to open <%s> for logging.\n"), fn1);
    		return -1;
  	}

  	log2 = fopen(fn2, "wt");
  	if (log2 == NULL) {
    		ticables_error(_("Unable to open <%s> for logging.\n"), fn2);
    		return -1;
  	}
#ifndef __WIN32__
  	memset((void *) (&tz), 0, sizeof(tz));
  	gettimeofday(&tv_start, &tz);
#endif

  	return 0;
}

int log_data(int d)
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
  	if (!(i % 16) && (i > 1)) {
    		fprintf(log1, "| ");
    		for (j = 0; j < 16; j++) {
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

int stop_logging()
{
  	ticables_info("Logging stopped.\n");

  	if (log1 != NULL)
    		fclose(log1);
  	if (log2 != NULL)
    		fclose(log2);

  	free(fn1);
  	free(fn2);

  	return 0;
}

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
