/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

/* "Home-made serial" link & "Black TIGraphLink" link unit (ioctl routines) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "timeout.h"
#include "ioports.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "logging.h"

/* 
 * This part manage the link cable with DCB/ioctl() I/O routines (IOM_API).
 *
 * Remark: the low-level part should not require DCB/API code but pratically,
 * some machines and link cables may require it for working properly
 * Especially FlashZ who had this boring problem (and me, later) !
 *
 * This allows to use this link cable under NT/Linux machines without a driver
 * but with a significant drop of performances.
 *
 * This allows to use this link cable thru a USB to SERIAL adapter, too.
 */

#define TRYC(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

static unsigned int com_addr;

#define com_out (com_addr+4)
#define com_in  (com_addr+6)

static int io_permitted2 = 0;

int ser_init2()
{
  	TRYC(io_open(com_out, 1));
  	io_permitted2++;
  	TRYC(io_open(com_in, 1));
  	io_permitted2++;

  	// removed: this perturbates communication start-up
  	//io_wr(com_out, 3);
  	//io_wr(com_out, 0);
  	//io_wr(com_out, 3);

  	START_LOGGING();

  	return 0;
}

int ser_open2()
{
  	tdr.count = 0;
  	toSTART(tdr.start);

  	return 0;
}

int ser_put2(uint8_t data)
{
  	int bit;
  	tiTIME clk;

  	tdr.count++;
  	LOG_DATA(data);
  	
  	toSTART(clk);
  	for (bit = 0; bit < 8; bit++) {
    		if (data & 1)
      			io_wr(com_out, 2);
    		else
      			io_wr(com_out, 1);

    		while (io_rd(com_in) != 0) {
      			if (toELAPSED(clk, time_out))
			return ERR_WRITE_TIMEOUT;
    		}

	    	io_wr(com_out, 3);
	    	while (io_rd(com_in) != 3) {
	      		if (toELAPSED(clk, time_out))
			return ERR_WRITE_TIMEOUT;
	    	}
	
	    	data >>= 1;
  	}

  	return 0;
}

int ser_get2(uint8_t * ch)
{
  	int bit;
  	int i, j;
  	tiTIME clk;

  	tdr.count++;
  	toSTART(clk);

  	for (i = 0, bit = 1, *ch = 0; i < 8; i++) {
    		while ((j = io_rd(com_in)) == 3) {
      			if (toELAPSED(clk, time_out))
			return ERR_READ_TIMEOUT;
    		}

    		if (j == 1) {
      			*ch |= bit;
      			io_wr(com_out, 1);
      			j = 2;
    		} else {
      			io_wr(com_out, 2);
      			j = 1;
    		}

    		while ((io_rd(com_in) & j) == 0) {
      			if (toELAPSED(clk, time_out))
			return ERR_READ_TIMEOUT;
    		}

    		io_wr(com_out, 3);
    		bit <<= 1;
  	}

  	LOG_DATA(*ch);

  	return 0;
}

int ser_probe2()
{
  	int i, j;
  	int seq[] = { 0x00, 0x20, 0x10, 0x30 };

  	for (i = 3; i >= 0; i--) {
    		io_wr(com_out, 3);
    		io_wr(com_out, i);

	    	for (j = 0; j < 10; j++)
	      		io_rd(com_in);
	    
	    	//printl1(0, "%i %02X %02X %02X\n", i, io_rd(com_in), io_rd(com_in) & 0x30, seq[i]);
	    	if ((io_rd(com_in) & 0x30) != seq[i]) {
	      		io_wr(com_out, 3);
	      		return ERR_ROOT;
	    	}
  	}
  	io_wr(com_out, 3);

  	return 0;
}

int ser_close2()
{
  	if (io_permitted2 == 2)
    		io_wr(com_out, 3);

  	return 0;
}

int ser_exit2()
{
  	TRYC(io_close(com_out, 1));
  	io_permitted2--;
  	TRYC(io_close(com_in, 1));
  	io_permitted2--;

  	return 0;
}

int ser_check2(int *status)
{
  	*status = STATUS_NONE;

  	if (!((io_rd(com_in) & 0x30) == 0x30)) {
    		*status = (STATUS_RX | STATUS_TX);
  	}

  	return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int ser_set_red_wire2(int b)
{
  	int v = swap_bits(io_rd(com_in) >> 4);
  	
  	if (b)
    		io_wr(com_out, v | 0x02);
  	else
    		io_wr(com_out, v & ~0x02);

  return 0;
}

int ser_set_white_wire2(int b)
{
  	int v = swap_bits(io_rd(com_in) >> 4);

  	if (b)
    		io_wr(com_out, v | 0x01);
  	else
    		io_wr(com_out, v & ~0x01);

  	return 0;
}

int ser_get_red_wire2()
{
  	return ((0x10 & io_rd(com_in)) ? 1 : 0);

  	return 0;
}

int ser_get_white_wire2()
{
  	return ((0x20 & io_rd(com_in)) ? 1 : 0);

  	return 0;
}

int ser_supported2()
{
  	return SUPPORT_ON | ((method & IOM_API) ? SUPPORT_DCB : SUPPORT_IO);
}

int ser_register_cable_2(TicableLinkCable * lc)
{
  lc->init = ser_init2;
  lc->open = ser_open2;
  lc->put = ser_put2;
  lc->get = ser_get2;
  lc->close = ser_close2;
  lc->exit = ser_exit2;
  lc->probe = ser_probe2;
  lc->check = ser_check2;

  lc->set_red_wire = ser_set_red_wire2;
  lc->set_white_wire = ser_set_white_wire2;
  lc->get_red_wire = ser_get_red_wire2;
  lc->get_white_wire = ser_get_white_wire2;

  return 0;
}
