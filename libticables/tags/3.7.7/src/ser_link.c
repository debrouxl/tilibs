/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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

/* "Home-made serial" link & "Black TIGraphLink" link unit */

/* 
 * This part manage the link cable with low-level I/O routines
 */

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

#define TRYC(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

#if defined(__WIN32__)
#define BUFFER_SIZE 1024
static HANDLE hCom = 0;
#endif

static unsigned int com_addr;
#define com_out (com_addr+4)
#define com_in  (com_addr+6)

static int io_permitted = 0;

/**********************************************/
/* Multi-platform part (trough low-level I/O) */
/**********************************************/

int ser_init()
{
#ifdef __WIN32__
  // Under Win2k: if we do not open the serial device as a COM
  // port, it may be impossible to transfer data.
  // This problem exists with Win2k and some UARTs.
  // It seems I got the same problem as FlashZ when I changed my 
  // motherboard. Are some port broken ?
  TRYC(io_open_comport(io_device, &hCom));
#endif

  com_addr = io_address;

  TRYC(io_open(com_out, 1));
  io_permitted++;
  TRYC(io_open(com_in, 1));
  io_permitted++;

  io_wr(com_out, 3);		// wake-up calculator
  io_wr(com_out, 0);
  io_wr(com_out, 3);

  START_LOGGING();

  return 0;
}

int ser_open()
{
  tdr.count = 0;
  toSTART(tdr.start);

  if (io_permitted)
    return 0;
  else
    return ERR_ROOT;
}

int ser_put(uint8_t data)
{
  int bit;
  int i;
  tiTIME clk;

  tdr.count++;
  LOG_DATA(data);

  for (bit = 0; bit < 8; bit++) {
    if (data & 1) {
      io_wr(com_out, 2);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      } while ((io_rd(com_in) & 0x10));
      io_wr(com_out, 3);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      } while ((io_rd(com_in) & 0x10) == 0x00);
    } else {
      io_wr(com_out, 1);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      } while (io_rd(com_in) & 0x20);
      io_wr(com_out, 3);
      toSTART(clk);
      do {
	if (toELAPSED(clk, time_out))
	  return ERR_WRITE_TIMEOUT;
      } while ((io_rd(com_in) & 0x20) == 0x00);
    }

    data >>= 1;
    for (i = 0; i < delay; i++)
      io_rd(com_in);
  }

  return 0;
}

int ser_get(uint8_t * ch)
{
  int bit;
  uint8_t data = 0;
  uint8_t v;
  int i;
  tiTIME clk;

  tdr.count++;
  for (bit = 0; bit < 8; bit++) {
    toSTART(clk);
    while ((v = io_rd(com_in) & 0x30) == 0x30) {
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
    }
    if (v == 0x10) {
      data = (data >> 1) | 0x80;
      io_wr(com_out, 1);
      while ((io_rd(com_in) & 0x20) == 0x00);
      io_wr(com_out, 3);
    } else {
      data = (data >> 1) & 0x7F;
      io_wr(com_out, 2);
      while ((io_rd(com_in) & 0x10) == 0x00);
      io_wr(com_out, 3);
    }
    for (i = 0; i < delay; i++)
      io_rd(com_in);
  }

  *ch = data;
  LOG_DATA(data);

  return 0;
}

int ser_probe()
{
  int i, j;
  int seq[] = { 0x00, 0x20, 0x00, 0x20 };
  int data;

  for (i = 3; i >= 0; i--) {
    io_wr(com_out, 3);
    io_wr(com_out, i);
    for (j = 0; j < 10; j++)
      data = io_rd(com_in);
    //DISPLAY("%i: 0x%02x 0x%02x\n", i, data, seq[i]);
    if ((data & 0x30) != seq[i]) {
      io_wr(com_out, 3);
      return ERR_PROBE_FAILED;
    }
  }
  io_wr(com_out, 3);

  return 0;
}

int ser_close()
{
  if (io_permitted == 2)
    io_wr(com_out, 3);

  return 0;
}

int ser_exit()
{
  TRYC(io_close(com_out, 1));
  io_permitted--;
  TRYC(io_close(com_in, 1));
  io_permitted--;

#ifdef __WIN32__
  TRYC(io_close_comport(&hCom));
#endif

  STOP_LOGGING();

  return 0;
}

int ser_check(int *status)
{
  *status = STATUS_NONE;

  if (!((io_rd(com_in) & 0x30) == 0x30)) {
    *status = (STATUS_RX | STATUS_TX);
  }

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int ser_set_red_wire(int b)
{
  int v = swap_bits(io_rd(com_in) >> 4);

  if (b)
    io_wr(com_out, v | 0x02);
  else
    io_wr(com_out, v & ~0x02);

  return 0;
}

int ser_set_white_wire(int b)
{
  int v = swap_bits(io_rd(com_in) >> 4);

  if (b)
    io_wr(com_out, v | 0x01);
  else
    io_wr(com_out, v & ~0x01);

  return 0;
}

int ser_get_red_wire()
{
  return ((0x10 & io_rd(com_in)) ? 1 : 0);
}

int ser_get_white_wire()
{
  return ((0x20 & io_rd(com_in)) ? 1 : 0);
}

int ser_supported()
{
  return SUPPORT_ON | ((method & IOM_API) ? SUPPORT_DCB : SUPPORT_IO);
}

/* 
 * This part manage the link cable with DCB/API I/O routines (IOM_API).
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

int ser_init2()
{
  TRYC(io_open(com_out, 1));
  io_permitted++;
  TRYC(io_open(com_in, 1));
  io_permitted++;

  io_wr(com_out, 3);
  io_wr(com_out, 0);
  io_wr(com_out, 3);

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
    //DISPLAY("%i %02X %02X %02X\n", i, io_rd(com_in), io_rd(com_in) & 0x30, seq[i]);
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
  if (io_permitted == 2)
    io_wr(com_out, 3);

  return 0;
}

int ser_exit2()
{
  TRYC(io_close(com_out, 1));
  io_permitted--;
  TRYC(io_close(com_in, 1));
  io_permitted--;

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
