/* Hey EMACS -*- linux-c -*- */
/* $Id: link_ser2.c 1033 2005-05-06 18:17:02Z roms $ */

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

/* "Home-made serial" link & "Black TIGraphLink" link unit (low-level I/O routines) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

#if defined(__BSD__)
#if defined(__NetBSD__)
#define DEVNAME "dty0"
#else /* FreeBSD */
#define DEVNAME "cuad"
#endif
#else
#define DEVNAME "ttyS"
#endif

static int ser_prepare(CableHandle *h)
{
	int ret;

	switch(h->port)
	{
		case PORT_1: h->address = 0x3f8; h->device = strdup("/dev/"DEVNAME"0"); break;
		case PORT_2: h->address = 0x2f8; h->device = strdup("/dev/"DEVNAME"1"); break;
		case PORT_3: h->address = 0x3e8; h->device = strdup("/dev/"DEVNAME"2"); break;
		case PORT_4: h->address = 0x3e8; h->device = strdup("/dev/"DEVNAME"3"); break;
		default: return ERR_ILLEGAL_ARG;
	}

	// detect stuff
#if defined(__BSD__)
	ret = bsd_check_tty(h->device);
#else
	ret = linux_check_tty(h->device);
#endif
	if(ret)
	{
		free(h->device); h->device = NULL;
		return ret;
	}

    return 0;
}

static int ser_open(CableHandle *h)
{
    int fd;

    TRYC(ser_io_open(h->device, &fd));
    h->priv = GINT_TO_POINTER(fd);

    return 0;
}

static int ser_close(CableHandle *h)
{
    TRYC(ser_io_close(dev_fd));
    return 0;
}

static int ser_reset(CableHandle *h)
{
    tiTIME clk;
    
    // wait for releasing of lines
    TO_START(clk);
    do
    {
	ser_io_wr(dev_fd, 3);
	if (TO_ELAPSED(clk, h->timeout))
	    return 0;
	//printf("%i", ser_io_rd(dev_fd) >> 4);
    }
    while ((ser_io_rd(dev_fd) & 0x30) != 0x30);
    
    return 0;
}

static int ser_put(CableHandle *h, uint8_t *data, uint32_t len)
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
		ser_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10))
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    
		ser_io_wr(dev_fd, 3);
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10) == 0x00)
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    }
	    else
	    {
		ser_io_wr(dev_fd, 1);
                TO_START(clk);
		while (ser_io_rd(dev_fd) & 0x20)
		{  
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
		};
		
                ser_io_wr(dev_fd, 3);
                TO_START(clk);
                while ((ser_io_rd(dev_fd) & 0x20) == 0x00)
		{
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
                };
	    }
	    
	    byte >>= 1;
	    for (i = 0; i < h->delay; i++)
		ser_io_rd(dev_fd);
	}
    }
    
    return 0;
}

static int ser_get(CableHandle *h, uint8_t *data, uint32_t len)
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
	    while ((v = ser_io_rd(dev_fd) & 0x30) == 0x30) 
	    {
		if (TO_ELAPSED(clk, h->timeout))
		    return ERR_READ_TIMEOUT;
	    }
	    
	    if (v == 0x10) 
	    {
		byte = (byte >> 1) | 0x80;
		ser_io_wr(dev_fd, 1);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x20) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		ser_io_wr(dev_fd, 3);
	    } 
	    else 
	    {
		byte = (byte >> 1) & 0x7F;
		ser_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		ser_io_wr(dev_fd, 3);
	    }
	    
	    for (i = 0; i < h->delay; i++)
		ser_io_rd(dev_fd);
	}
	
	data[j] = byte;
    }
    
    return 0;
}

static int ser_probe(CableHandle *h)
{
    int timeout = 1;
    tiTIME clk;
    
    // 1
    ser_io_wr(dev_fd, 2);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x10))
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    ser_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x10) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    // 0
    ser_io_wr(dev_fd, 1);
    TO_START(clk);
    while (ser_io_rd(dev_fd) & 0x20)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    ser_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x20) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    return 0;
}

static int ser_check(CableHandle *h, int *status)
{
	*status = STATUS_NONE;

  	if (!((ser_io_rd(dev_fd) & 0x30) == 0x30)) 
    		*status = (STATUS_RX | STATUS_TX);

	return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

static int ser_set_red_wire(CableHandle *h, int b)
{
	int v = swap_bits(ser_io_rd(dev_fd) >> 4);

  	if (b)
    		ser_io_wr(dev_fd, v | 0x02);
  	else
    		ser_io_wr(dev_fd, v & ~0x02);

	return 0;
}

static int ser_set_white_wire(CableHandle *h, int b)
{
	int v = swap_bits(ser_io_rd(dev_fd) >> 4);

  	if (b)
    		ser_io_wr(dev_fd, v | 0x01);
  	else
    		ser_io_wr(dev_fd, v & ~0x01);

	return 0;
}

static int ser_get_red_wire(CableHandle *h)
{
	return ((0x10 & ser_io_rd(dev_fd)) ? 1 : 0);
}

static int ser_get_white_wire(CableHandle *h)
{
	return ((0x20 & ser_io_rd(dev_fd)) ? 1 : 0);
}

static int ser_set_raw(CableHandle *h, int state)
{
	ser_io_wr(dev_fd, swap_bits(state));
	return 0;
}

static int ser_get_raw(CableHandle *h, int *state)
{
	*state = (ser_io_rd(dev_fd) >> 4) & 3;
	return 0;
}

static int ser_set_device(CableHandle *h, const char * device)
{
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			h->device = device2;
		}
		else
		{
			ticables_warning(_("unable to set device %s.\n"), device);
		}
		return 0;
	}
	return ERR_ILLEGAL_ARG;
}

const CableFncts cable_ser = 
{
	CABLE_BLK,
	"BLK",
	N_("BlackLink"),
	N_("BlackLink or home-made serial cable"),
	!0,
	&ser_prepare,
	&ser_open, &ser_close, &ser_reset, &ser_probe, NULL,
	&ser_put, &ser_get, &ser_check,
	&ser_set_red_wire, &ser_set_white_wire,
	&ser_get_red_wire, &ser_get_white_wire,
	&ser_set_raw, &ser_get_raw,
	&ser_set_device
};
