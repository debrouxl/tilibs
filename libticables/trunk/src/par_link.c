/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include "timeout.h"
#include "ioports.h"
#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "logging.h"
#include "cabl_ext.h"

static unsigned int lpt_adr;
#define lpt_out lpt_adr
#define lpt_in (lpt_adr+1)

static int io_permitted = 0;

int par_init()
{
  lpt_adr = io_address;
  
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  TRYC(open_io(lpt_adr, 2));
  
  io_permitted = 1;
  wr_io(lpt_out, 3);
#endif
  START_LOGGING();

  return 0;
}

int par_open()
{
  tdr.count = 0;
  toSTART(tdr.start);

  if(io_permitted)
    return 0;
  else
    return ERR_ROOT;
}

int par_put(byte data)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  int bit;
  int i;
  TIME clk;

  tdr.count++;
  LOG_DATA(data);
  for(bit=0; bit<8; bit++)
    {
      if(data & 1)
	{
	  wr_io(lpt_out, 2);
	  toSTART(clk);
	  do
	    {
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(lpt_in) & 0x10));
	  wr_io(lpt_out, 3);
	  toSTART(clk);
	  do
	    {
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(lpt_in) & 0x10)==0x00);
	}
      else
	{
	  wr_io(lpt_out, 1);
	  toSTART(clk);
	  do
	    {
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while(rd_io(lpt_in) & 0x20);
	  wr_io(lpt_out, 3);
	  toSTART(clk);
	  do
	    {
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(lpt_in) & 0x20)==0x00);
	}
      data>>=1;
      for(i=0; i<delay; i++) rd_io(lpt_in);
    }
#endif
  return 0;
}

int par_get(byte *d)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  int bit;
  byte data=0;
  byte v;
  int i;
  TIME clk;

  tdr.count++;
  for(bit=0; bit<8; bit++)
    {
      toSTART(clk);
      while((v=rd_io(lpt_in) & 0x30)==0x30)
	{
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BIT_TIMEOUT;
	}
      if(v==0x10)
	{
	  data=(data >> 1) | 0x80;
	  wr_io(lpt_out, 1);
	  while((rd_io(lpt_in) & 0x20)==0x00);
	  wr_io(lpt_out, 3);
	}
      else
	{
	  data=(data >> 1) & 0x7F;
	  wr_io(lpt_out, 2);
	  while((rd_io(lpt_in) & 0x10)==0x00);
	  wr_io(lpt_out, 3);
	}
      for(i=0; i<delay; i++) rd_io(lpt_in);
    }
  *d=data;
  LOG_DATA(data);

#endif
  return 0;
}

int par_probe()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  int i, j;
  int seq[]={ 0x00, 0x20, 0x10, 0x30 };
  byte data;

  for(i=3; i>=0; i--)
    {
      wr_io(lpt_out, 3);
      wr_io(lpt_out, i);
      for(j=0; j<10; j++) data = rd_io(lpt_in);
	  //DISPLAY("%i: 0x%02x 0x%02x\n", i, data & 0x30, seq[i]);
      if( (data & 0x30) != seq[i]) 
	{
	  wr_io(lpt_out, 3);  
	  return ERR_PROBE_FAILED;
	}
    } 
  wr_io(lpt_out, 3);
#else
  return ERR_PROBE_FAILED;
#endif
 
  return 0;
}

int par_close()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  if(io_permitted)
    wr_io(lpt_out, 3);
#endif

  return 0;
}

int par_exit()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__)
  TRYC(close_io(lpt_adr, 2));
    
  io_permitted = 0;
#endif
  STOP_LOGGING();

  return 0;
}

int par_check(int *status)
{
  *status = STATUS_NONE;
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  if(!( (rd_io(lpt_in) & 0x30) == 0x30 ))
    {
      *status = STATUS_RX | STATUS_TX;
    }
#endif
  
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int par_set_red_wire(int b)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  int v = swap_bits(rd_io(lpt_in) >> 4);
  if(b)
    wr_io(lpt_out, v | 0x02);
  else
    wr_io(lpt_out, v & ~0x02);
#endif

  return 0;
}

int par_set_white_wire(int b)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  int v = swap_bits(rd_io(lpt_in) >> 4);
  if(b)
    wr_io(lpt_out, v | 0x01);
  else
    wr_io(lpt_out, v & ~0x01);
#endif

  return 0;
}

int par_get_red_wire()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  return (0x10 & rd_io(lpt_in)) ? 1 : 0;
#endif

  return 0;
}

int par_get_white_wire()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  return (0x20 & rd_io(lpt_in)) ? 1 : 0;
#endif

  return 0;
}

int par_supported()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  return SUPPORT_ON | SUPPORT_IO;
#else
  return SUPPORT_OFF;
#endif
}



