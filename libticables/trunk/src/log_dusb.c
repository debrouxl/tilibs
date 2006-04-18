/* Hey EMACS -*- linux-c -*- */
/* $Id: data_log.c 1720 2006-01-20 22:34:58Z roms $ */

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

/* 
	D-USB logging.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <glib.h>

#include "logging.h"
#include "data_log.h"

#define LOG_FILE  "ticables-dusb.log"

static char *fn = NULL;
static FILE *log = NULL;

int log_dusb_start(void)
{
  // build filenames
#ifdef __WIN32__
	fn = g_strconcat("C:\\", LOG_FILE, NULL);
#else
	fn = g_strconcat(g_get_home_dir(), "/", LOG_FILE, NULL);
#endif

  	log = fopen(fn, "wt");
  	if (log == NULL)
		return -1;

	fprintf(log, "TI packet decompiler for D-USB\n");

  	return 0;
}

int log_dusb_1(int dir, uint8_t data)
{
	static int array[16];
  	static int i = 0;
	static int state = 1;
	static uint32_t raw_size;
	static uint8_t raw_type;
	static uint32_t vtl_size;
	static uint16_t vtl_type;
	static int cnt;

  	if (log == NULL)
    		return -1;

	//printf("<%i %i> ", i, state);
	array[i++ % 16] = data;

	switch(state)	// Finite State Machine
	{
	case 1: break;
	case 2: break;
	case 3: break;
	case 4: 
		raw_size = (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | (array[3] << 0);
		fprintf(log, "%08x ", raw_size);
		break;
	case 5: 
		raw_type = array[4];
		fprintf(log, "(%02X)\n", raw_type);
		break;
	case 6: break;
	case 7:
		if(raw_type == 5)
		{
			uint16_t tmp = (array[5] << 8) | (array[6] << 0);
			fprintf(log, "\t[%04x]\n", tmp);
			state = 0;
		}
		break;
	case 8: break;
	case 9:
		if(raw_type == 1 || raw_type == 2)
		{
			uint32_t tmp = (array[5] << 24) | (array[6] << 16) | (array[7] << 8) | (array[8] << 0);
			fprintf(log, "\t[%08x]\n", tmp);
			state = 0;
		}
		else if(raw_type == 3 || raw_type == 4)
		{
			vtl_size = (array[5] << 24) | (array[6] << 16) | (array[7] << 8) | (array[8] << 0);
			fprintf(log, "\t%08x ", vtl_size);
			cnt = 0;
		}
		break;
	case 10: break;
	case 11:
		vtl_type = (array[9] << 8) | (array[10] << 0);
		fprintf(log, "{%04x}\n\t\t", vtl_type);

		if(!vtl_size)
		{
			fprintf(log, "\n");
			state = 0;
		}
		break;
	default: 
		fprintf(log, "%02X ", data);

		if(!(++cnt % 12))
			fprintf(log, "\n\t\t");
		
		if(--vtl_size == 0)
		{
			fprintf(log, "\n");
			state = 0;
		}
		break;
	}

	if(state == 0)
	{
		fprintf(log, "\n");
		i = 0;
	}
	state++;	

	return 0;
#if 0
	array[i++] = data;
  	fprintf(log, "%02X ", data);

  	if ((i > 1 ) && !(i % 16)) 
	{
    	fprintf(log, "| ");
    	for (j = 0; j < 16; j++) 
		{
      		c = array[j];
      		if ((c < 32) || (c > 127))
				fprintf(log, " ");
      		else
				fprintf(log, "%c", c);
    	}
    	fprintf(log, "\n");
    	i = 0;
  	}
#endif
  	return 0;
}

int log_dusb_stop(void)
{
  	if (log != NULL)
	{
    		fclose(log);
			log = NULL;
	}
  	g_free(fn);

  	return 0;
}
