/* Hey EMACS -*- linux-c -*- */
/* $Id: par_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* "Home-made parallel" link cable unit */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include "timeout.h"
#include "ioports.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "logging.h"
#include "externs.h"
#include "verbose.h"

static unsigned int lpt_adr;
#define lpt_out  lpt_adr
#define lpt_in   (lpt_adr+1)
#define lpt_ctl  (lpt_adr+2)

static int io_permitted = 0;

int par_init()
{
  lpt_adr = io_address;

  if (io_open(lpt_adr, 3))
    return ERR_ROOT;
#ifdef __WIN32__
  // needed for circumventing a strange problem with PortTalk & Win2k
  if (io_open(lpt_adr, 3))
    return ERR_ROOT;
#endif

  io_permitted = 1;
  io_wr(lpt_ctl, io_rd(lpt_ctl) & ~0x20);	// ouput mode only

  io_wr(lpt_out, 3);		// wake-up calculator
  io_wr(lpt_out, 0);
  io_wr(lpt_out, 3);

  START_LOGGING();

  return 0;
}

int par_open()
{
  tdr.count = 0;
  toSTART(tdr.start);

  if (io_permitted)
    return 0;
  else
    return ERR_ROOT;
}

int par_put(uint8_t data)
{
  int bit;
  int i;
  tiTIME clk;

  tdr.count++;
  LOG_DATA(data);

  for (bit = 0; bit < 8; bit++) {
    if (data & 1) {
      io_wr(lpt_out, 2);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      }
      while ((io_rd(lpt_in) & 0x10));
      io_wr(lpt_out, 3);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      }
      while (!(io_rd(lpt_in) & 0x10));
    } else {
      io_wr(lpt_out, 1);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      }
      while (io_rd(lpt_in) & 0x20);
      io_wr(lpt_out, 3);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      }
      while (!(io_rd(lpt_in) & 0x20));
    }
    data >>= 1;
    for (i = 0; i < delay; i++)
      io_rd(lpt_in);
  }

  return 0;
}

int par_get(uint8_t * d)
{
  int bit;
  uint8_t data = 0;
  uint8_t v;
  int i;
  tiTIME clk;

  tdr.count++;
  for (bit = 0; bit < 8; bit++) {
    toSTART(clk);
    while ((v = io_rd(lpt_in) & 0x30) == 0x30) {
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
    }
    if (v == 0x10) {
      data = (data >> 1) | 0x80;
      io_wr(lpt_out, 1);
      while ((io_rd(lpt_in) & 0x20) == 0x00);
      io_wr(lpt_out, 3);
    } else {
      data = (data >> 1) & 0x7F;
      io_wr(lpt_out, 2);
      while ((io_rd(lpt_in) & 0x10) == 0x00);
      io_wr(lpt_out, 3);
    }
    for (i = 0; i < delay; i++)
      io_rd(lpt_in);
  }
  *d = data;
  LOG_DATA(data);

  return 0;
}

int par_probe()
{
  int i, j;
  int seq[] = { 0x00, 0x20, 0x10, 0x30 };
  uint8_t data;

  for (i = 3; i >= 0; i--) {
    io_wr(lpt_out, 3);
    io_wr(lpt_out, i);
    for (j = 0; j < 10; j++)
      data = io_rd(lpt_in);
    //DISPLAY("%i: 0x%02x 0x%02x\n", i, data & 0x30, seq[i]);
    if ((data & 0x30) != seq[i]) {
      io_wr(lpt_out, 3);
      return ERR_PROBE_FAILED;
    }
  }
  io_wr(lpt_out, 3);

  return 0;
}

int par_close()
{
  if (io_permitted)
    io_wr(lpt_out, 3);

  return 0;
}

int par_exit()
{
  if (io_close(lpt_adr, 2))
    return ERR_ROOT;

  io_permitted = 0;

  STOP_LOGGING();

  return 0;
}

int par_check(int *status)
{
  *status = STATUS_NONE;
  if (!((io_rd(lpt_in) & 0x30) == 0x30)) {
    *status = STATUS_RX | STATUS_TX;
  }

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int par_set_red_wire(int b)
{
  int v = swap_bits(io_rd(lpt_in) >> 4);

  if (b)
    io_wr(lpt_out, v | 0x02);
  else
    io_wr(lpt_out, v & ~0x02);

  return 0;
}

int par_set_white_wire(int b)
{
  int v = swap_bits(io_rd(lpt_in) >> 4);

  if (b)
    io_wr(lpt_out, v | 0x01);
  else
    io_wr(lpt_out, v & ~0x01);

  return 0;
}

int par_get_red_wire()
{
  return (0x10 & io_rd(lpt_in)) ? 1 : 0;
}

int par_get_white_wire()
{
  return (0x20 & io_rd(lpt_in)) ? 1 : 0;
}

int par_supported()
{
  if (method & IOM_OK)
    return SUPPORT_ON | SUPPORT_IO;
  else
    return SUPPORT_OFF;
}
