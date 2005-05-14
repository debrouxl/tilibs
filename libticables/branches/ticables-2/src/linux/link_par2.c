/* Hey EMACS -*- linux-c -*- */
/* $Id: link_par.c 1015 2005-05-02 14:00:00Z roms $ */

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

/* "Home-made parallel" link cable unit */

#include <stdio.h>
#include <string.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "ioports.h"

#define lpt_out (h->address + 0)
#define lpt_in  (h->address + 1)
#define lpt_ctl (h->address + 2)

static int par_prepare(CableHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0x378; h->device = strdup("/dev/lp0"); break;
	case PORT_2: h->address = 0x278; h->device = strdup("/dev/lp1"); break;
	case PORT_3: h->address = 0x3bc; h->device = strdup("/dev/lp2"); break;
	default: return ERR_ILLEGAL_ARG;
	}

	// detect stuffs 
	TRYC(check_for_root());

	return 0;
}

static int par_open(CableHandle *h)
{
	TRYC(io_open(h->address));
#ifdef __WIN32__
	// needed for circumventing a strange problem with PortTalk & Win2k
  	TRYC(io_open(h->address))
#endif
  	io_wr(lpt_ctl, io_rd(lpt_ctl) & ~0x20);	// ouput mode only

	return 0;
}

static int par_close(CableHandle *h)
{
	TRYC(io_close(h->address));

	return 0;
}

static int par_reset(CableHandle *h)
{
	io_wr(lpt_out, 3);
	return 0;
}

static int par_put(CableHandle *h, uint8_t *data, uint16_t len)
{
	int bit;
  	int i, j;
  	tiTIME clk;

	for(j = 0; j < len; j++)
	{
		uint8_t byte = data[j];

  		for (bit = 0; bit < 8; bit++) 
		{
    			if (byte & 1) {
      				io_wr(lpt_out, 2);
      				TO_START(clk);
	      			do 
					{
						if (TO_ELAPSED(clk, h->timeout))
		  				return ERR_WRITE_TIMEOUT;
	      			}
	      			while ((io_rd(lpt_in) & 0x10));
	      			
	      			io_wr(lpt_out, 3);
	      			TO_START(clk);
	      			do 
					{
						if (TO_ELAPSED(clk, h->timeout))
		  				return ERR_WRITE_TIMEOUT;
	      			}
	      			while (!(io_rd(lpt_in) & 0x10));
    			} 
				else 
				{
      				io_wr(lpt_out, 1);
      				TO_START(clk);
		      		do 
					{
						if (TO_ELAPSED(clk, h->timeout))
			  			return ERR_WRITE_TIMEOUT;
		      		}
		      		while (io_rd(lpt_in) & 0x20);
		      		
		      		io_wr(lpt_out, 3);
		      		TO_START(clk);
		      		do 
					{
						if (TO_ELAPSED(clk, h->timeout))
			  			return ERR_WRITE_TIMEOUT;
		      		}
		      		while (!(io_rd(lpt_in) & 0x20));
    		}
    		
    		byte >>= 1;
    		for (i = 0; i < h->delay; i++)
      			io_rd(lpt_in);
  		}	
	}

	return 0;
}

static int par_get(CableHandle *h, uint8_t *data, uint16_t len)
{
	int bit;
	int i, j;
  	tiTIME clk;

	for(j = 0; j < len; j++)
	{
		uint8_t v, byte = 0;

  		for (bit = 0; bit < 8; bit++) 
		{
    			TO_START(clk);
    			while ((v = io_rd(lpt_in) & 0x30) == 0x30) 
				{
      				if (TO_ELAPSED(clk, h->timeout))
						return ERR_READ_TIMEOUT;
    			}
    			
    			if (v == 0x10) 
				{
      				byte = (byte >> 1) | 0x80;
      				io_wr(lpt_out, 1);
      				TO_START(clk);
      				while ((io_rd(lpt_in) & 0x20) == 0x00) 
					{
      					if (TO_ELAPSED(clk, h->timeout))
			  				return ERR_WRITE_TIMEOUT;
      				}
      				io_wr(lpt_out, 3);
    			} 
				else 
				{
      				byte = (byte >> 1) & 0x7F;
      				io_wr(lpt_out, 2);
      				TO_START(clk);
      				while ((io_rd(lpt_in) & 0x10) == 0x00) 
					{
      					if (TO_ELAPSED(clk, h->timeout))
			  				return ERR_WRITE_TIMEOUT;
      				}
      				io_wr(lpt_out, 3);
    		}
    		
    		for (i = 0; i < h->delay; i++)
      			io_rd(lpt_in);
  		}
  
  		data[j] = byte;
	}

	return 0;
}

static int par_probe(CableHandle *h)
{
	int i, j;
  	int seq[] = { 0x00, 0x20, 0x10, 0x30 };
  	uint8_t data;

  	for (i = 3; i >= 0; i--) 
	{
    		io_wr(lpt_out, 3);
    		io_wr(lpt_out, i);
    		
    		for (j = 0; j < 10; j++)
      			data = io_rd(lpt_in);
      			
    		//printl1(0, "%i: 0x%02x 0x%02x\n", i, data & 0x30, seq[i]);
    		if ((data & 0x30) != seq[i]) 
			{
      			io_wr(lpt_out, 3);
      			return ERR_PROBE_FAILED;
    		}
  	}
  	io_wr(lpt_out, 3);

	return 0;
}

static int par_check(CableHandle *h, int *status)
{
	*status = STATUS_NONE;

  	if (!((io_rd(lpt_in) & 0x30) == 0x30))
    		*status = STATUS_RX | STATUS_TX;

	return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

static int par_set_red_wire(CableHandle *h, int b)
{
	int v = swap_bits(io_rd(lpt_in) >> 4);

  	if (b)
    		io_wr(lpt_out, v | 0x02);
  	else
    		io_wr(lpt_out, v & ~0x02);

	return 0;
}

static int par_set_white_wire(CableHandle *h, int b)
{
	int v = swap_bits(io_rd(lpt_in) >> 4);

  	if (b)
    		io_wr(lpt_out, v | 0x01);
  	else
    		io_wr(lpt_out, v & ~0x01);

	return 0;
}

static int par_get_red_wire(CableHandle *h)
{
	return (0x10 & io_rd(lpt_in)) ? 1 : 0;
}

static int par_get_white_wire(CableHandle *h)
{
	return (0x20 & io_rd(lpt_in)) ? 1 : 0;
}

const CableFncts cable_par = 
{
	CABLE_PAR,
	"PAR",
	N_("Parallel"),
	N_("Home-made parallel cable"),
	!0,
	&par_prepare,
	&par_open, &par_close, &par_reset, &par_probe,
	&par_put, &par_get, &par_check,
	&par_set_red_wire, &par_set_white_wire,
	&par_get_red_wire, &par_get_white_wire,
};
