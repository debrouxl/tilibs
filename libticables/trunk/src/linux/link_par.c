/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* "Home-made parallel" link cable unit */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "ioports.h"

#define dev_fd  (GPOINTER_TO_INT(h->priv))

static int par_prepare(CableHandle *h)
{
	int ret;

	switch(h->port)
	{
	case PORT_1: h->address = 0x378; h->device = strdup("/dev/parport0"); 
	    break;
	case PORT_2: h->address = 0x278; h->device = strdup("/dev/parport1"); 
	    break;
	case PORT_3: h->address = 0x3bc; h->device = strdup("/dev/parport2"); 
	    break;
	default: return ERR_ILLEGAL_ARG;
	}

	// detect stuffs
	ret = linux_check_parport(h->device);
	if(ret)
	{
		free(h->device); h->device = NULL;
		return ret;
	}

	return 0;
}

static int par_open(CableHandle *h)
{
    int fd;

    TRYC(par_io_open(h->device, &fd));
    h->priv = GINT_TO_POINTER(fd);

    return 0;
}

static int par_close(CableHandle *h)
{
    TRYC(par_io_close(dev_fd));
    return 0;
}

static int par_reset(CableHandle *h)
{
    tiTIME clk;
    
    // wait for releasing of lines
    TO_START(clk);
    do
    {
	par_io_wr(dev_fd, 3);
	if (TO_ELAPSED(clk, h->timeout))
	    return 0;
    }
    while ((par_io_rd(dev_fd) & 0x30) != 0x30);
    
    return 0;
}

static int par_put(CableHandle *h, uint8_t *data, uint32_t len)
{
    int bit;
    unsigned int i;
    uint32_t j;
    tiTIME clk;
    
    for(j = 0; j < len; j++)
    {
	uint8_t byte = data[j];
	
	for (bit = 0; bit < 8; bit++) 
	{
	    if (byte & 1) 
	    {
		par_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((par_io_rd(dev_fd) & 0x10))
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    
		par_io_wr(dev_fd, 3);
		TO_START(clk);
		while ((par_io_rd(dev_fd) & 0x10) == 0x00);
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    }
	    else
	    {
		par_io_wr(dev_fd, 1);
                TO_START(clk);
		while (par_io_rd(dev_fd) & 0x20)
		{  
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
		};
		
                par_io_wr(dev_fd, 3);
                TO_START(clk);
                while ((par_io_rd(dev_fd) & 0x20) == 0x00)
		{
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
                };
	    }
	    
	    byte >>= 1;
	    for (i = 0; i < h->delay; i++)
		par_io_rd(dev_fd);
	}
    }
    
    return 0;
}

static int par_get(CableHandle *h, uint8_t *data, uint32_t len)
{
    int bit;
    unsigned int i;
    uint32_t j;
    tiTIME clk;
    
    for(j = 0; j < len; j++)
    {
	uint8_t v, byte = 0;
  	
	for (bit = 0; bit < 8; bit++) 
	{
	    TO_START(clk);
	    while ((v = par_io_rd(dev_fd) & 0x30) == 0x30) 
	    {
		if (TO_ELAPSED(clk, h->timeout))
		    return ERR_READ_TIMEOUT;
	    }
	    
	    if (v == 0x10) 
	    {
		byte = (byte >> 1) | 0x80;
		par_io_wr(dev_fd, 1);
		
		TO_START(clk);
		while ((par_io_rd(dev_fd) & 0x20) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		par_io_wr(dev_fd, 3);
	    } 
	    else 
	    {
		byte = (byte >> 1) & 0x7F;
		par_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((par_io_rd(dev_fd) & 0x10) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		par_io_wr(dev_fd, 3);
	    }
	    
	    for (i = 0; i < h->delay; i++)
		par_io_rd(dev_fd);
	}
	
	data[j] = byte;
    }
    
    return 0;
}

static int par_probe(CableHandle *h)
{
    int timeout = 1;
    tiTIME clk;
    
    // 1
    par_io_wr(dev_fd, 2);
    TO_START(clk);
    while ((par_io_rd(dev_fd) & 0x10))
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    par_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((par_io_rd(dev_fd) & 0x10) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    // 0
    par_io_wr(dev_fd, 1);
    TO_START(clk);
    while (par_io_rd(dev_fd) & 0x20)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    par_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((par_io_rd(dev_fd) & 0x20) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    return 0;
}

static int par_check(CableHandle *h, int *status)
{
    *status = STATUS_NONE;
    
    if (!((par_io_rd(dev_fd) & 0x30) == 0x30)) 
	*status = (STATUS_RX | STATUS_TX);
    
    return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

static int par_set_red_wire(CableHandle *h, int b)
{
int v = swap_bits(par_io_rd(dev_fd) >> 4);

  	if (b)
    		par_io_wr(dev_fd, v | 0x02);
  	else
    		par_io_wr(dev_fd, v & ~0x02);

	return 0;
}

static int par_set_white_wire(CableHandle *h, int b)
{
int v = swap_bits(par_io_rd(dev_fd) >> 4);

  	if (b)
    		par_io_wr(dev_fd, v | 0x01);
  	else
    		par_io_wr(dev_fd, v & ~0x01);

	return 0;
}

static int par_get_red_wire(CableHandle *h)
{
    return ((0x10 & par_io_rd(dev_fd)) ? 1 : 0);
}

static int par_get_white_wire(CableHandle *h)
{
    return ((0x20 & par_io_rd(dev_fd)) ? 1 : 0);
}

const CableFncts cable_par = 
{
	CABLE_PAR,
	"PAR",
	N_("Parallel"),
	N_("Home-made parallel cable"),
	!0,
	&par_prepare,
	&par_open, &par_close, &par_reset, &par_probe, NULL,
	&par_put, &par_get, &par_check,
	&par_set_red_wire, &par_set_white_wire,
	&par_get_red_wire, &par_get_white_wire,
};
