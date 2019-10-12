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

/* "Home-made serial" link & "Black TIGraphLink" link unit (low-level I/O routines) */

#include <stdio.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "../internal.h"
#include "detect.h"
#include "ioports.h"

#define com_out (h->address + 4)
#define com_in  (h->address + 6)

#define hCom	(HANDLE)(h->priv)
#define BUFFER_SIZE 1024

static int ser_prepare(CableHandle *h)
{
	const char * device;
	switch(h->port)
	{
	case PORT_1: h->address = 0x3f8; device = "COM1"; break;
	case PORT_2: h->address = 0x2f8; device = "COM2"; break;
	case PORT_3: h->address = 0x3e8; device = "COM3"; break;
	case PORT_4: h->address = 0x3e8; device = "COM4"; break;
	default: return ERR_ILLEGAL_ARG;
	}

	if (h->device == NULL)
	{
		h->device = strdup(device);
	}

	// detect OS 
	if(win32_check_os() == WIN_NT)
	{
		if(win32_check_dha())
		{
			free(h->device); h->device = NULL;
			return ERR_DHA_NOT_FOUND;
		}
	}
	else if(win32_check_os() == WIN_64)
	{
		if(win32_check_rwp())
		{
			free(h->device); h->device = NULL;
			return ERR_DHA_NOT_FOUND;
		}
	}

	return 0;
}

static int ser_open(CableHandle *h)
{
	// Under Win2k: if we do not open the serial device as a COM
	// port, it may be impossible to transfer data.
	// This problem exists with Win2k and some UARTs.
	// It seems I got the same problem as FlashZ when I changed my 
	// motherboard. Are some ports broken ?
	int ret;
	HANDLE hcom;
	ret = win32_comport_open(h->device, &hcom);
	if (!ret)
	{
		h->priv = (void *)hcom;
		ret = io_open(com_out);
		if (!ret)
		{
			ret = io_open(com_in);
		}
	}

	return ret;
}

static int ser_close(CableHandle *h)
{
	int ret;
	ret = io_close(com_out);
	if (!ret)
	{
		ret = io_close(com_in);
		if (!ret)
		{
			HANDLE hcom = hCom;
			ret = win32_comport_close(&hcom);
			if (!ret)
			{
				h->priv = (void *)hcom;
			}
		}
	}

	return ret;
}

static int ser_reset(CableHandle *h)
{
	tiTIME clk;

	// wait for releasing of lines
    TO_START(clk);
	do 
	{
		io_wr(com_out, 3);
		if (TO_ELAPSED(clk, h->timeout))
	  		return 0;
    } 
	while ((io_rd(com_in) & 0x30) != 0x30);

	return 0;
}

static int ser_probe(CableHandle *h)
{
	unsigned int timeout = 1;
	tiTIME clk;

	// 1
	io_wr(com_out, 2);
	TO_START(clk);
	do
	{
		if (TO_ELAPSED(clk, timeout))
		{
			return ERR_WRITE_TIMEOUT;
		}
	} while ((io_rd(com_in) & 0x10));

	io_wr(com_out, 3);
	TO_START(clk);
	do 
	{
		if (TO_ELAPSED(clk, timeout))
		{
			return ERR_WRITE_TIMEOUT;
		}
	} while ((io_rd(com_in) & 0x10) == 0x00);

	// 0
	io_wr(com_out, 1);
	TO_START(clk);
	do 
	{
		if (TO_ELAPSED(clk, timeout))
		{
			return ERR_WRITE_TIMEOUT;
		}
	} while (io_rd(com_in) & 0x20);

	io_wr(com_out, 3);
	TO_START(clk);
	do 
	{
		if (TO_ELAPSED(clk, timeout))
		{
			return ERR_WRITE_TIMEOUT;
		}
	} while ((io_rd(com_in) & 0x20) == 0x00);

	return 0;
}

static int ser_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	int bit;
	unsigned int i;
	unsigned int j;
	tiTIME clk;

	for (j = 0; j < len; j++)
	{
		uint8_t curbyte = data[j];

		for (bit = 0; bit < 8; bit++)
		{
			if (curbyte & 1)
			{
				io_wr(com_out, 2);
				TO_START(clk);
				do
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_WRITE_TIMEOUT;
					}
				} while ((io_rd(com_in) & 0x10));

				io_wr(com_out, 3);
				TO_START(clk);
				do
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_WRITE_TIMEOUT;
					}
				} while ((io_rd(com_in) & 0x10) == 0x00);
			}
			else
			{
				io_wr(com_out, 1);
				TO_START(clk);
				do
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_WRITE_TIMEOUT;
					}
				} while (io_rd(com_in) & 0x20);

				io_wr(com_out, 3);
				TO_START(clk);
				do 
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_WRITE_TIMEOUT;
					}
				} while ((io_rd(com_in) & 0x20) == 0x00);
			}

			curbyte >>= 1;
			for (i = 0; i < h->delay; i++)
			{
				io_rd(com_in);
			}
		}
	}

	return 0;
}

static int ser_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	int bit;
	unsigned int i;
	unsigned int j;
	tiTIME clk;

	for(j = 0; j < len; j++)
	{
		uint8_t v, curbyte = 0;

		for (bit = 0; bit < 8; bit++) 
		{
			TO_START(clk);
			while ((v = io_rd(com_in) & 0x30) == 0x30)
			{
				if (TO_ELAPSED(clk, h->timeout))
				{
					return ERR_READ_TIMEOUT;
				}
			}

			if (v == 0x10) 
			{
				curbyte = (curbyte >> 1) | 0x80;
				io_wr(com_out, 1);

				TO_START(clk);
				while ((io_rd(com_in) & 0x20) == 0x00)
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_READ_TIMEOUT;
					}
				}
				io_wr(com_out, 3);
			} 
			else 
			{
				curbyte = (curbyte >> 1) & 0x7F;
				io_wr(com_out, 2);

				TO_START(clk);
				while ((io_rd(com_in) & 0x10) == 0x00)
				{
					if (TO_ELAPSED(clk, h->timeout))
					{
						return ERR_READ_TIMEOUT;
					}
				}
				io_wr(com_out, 3);
			}

			for (i = 0; i < h->delay; i++)
			{
				io_rd(com_in);
			}
		}

		data[j] = curbyte;
	}

	return 0;
}

static int ser_check(CableHandle *h, int *status)
{
	*status = STATUS_NONE;

	if (!((io_rd(com_in) & 0x30) == 0x30))
	{
		*status = STATUS_RX;
	}

	return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

static int ser_set_red_wire(CableHandle *h, int b)
{
	int v = swap_bits(io_rd(com_in) >> 4);

	if (b)
	{
		io_wr(com_out, v | 0x02);
	}
	else
	{
		io_wr(com_out, v & ~0x02);
	}

	return 0;
}

static int ser_set_white_wire(CableHandle *h, int b)
{
	int v = swap_bits(io_rd(com_in) >> 4);

	if (b)
	{
		io_wr(com_out, v | 0x01);
	}
	else
	{
		io_wr(com_out, v & ~0x01);
	}

	return 0;
}

static int ser_get_red_wire(CableHandle *h)
{
	return ((0x10 & io_rd(com_in)) ? 1 : 0);
}

static int ser_get_white_wire(CableHandle *h)
{
	return ((0x20 & io_rd(com_in)) ? 1 : 0);
}

static int ser_set_raw(CableHandle *h, int state)
{
	io_wr(com_out, swap_bits(state));
	return 0;
}

static int ser_get_raw(CableHandle *h, int *state)
{
	*state = (io_rd(com_in) >> 4) & 3;
	return 0;
}

static int ser_set_device(CableHandle *h, const char * device)
{
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			free(h->device);
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

extern const CableFncts cable_ser = 
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
	&ser_set_device,
	NULL
};
