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
#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "logging.h"


#if defined(__WIN32__)
 #define BUFFER_SIZE 1024
 static HANDLE hCom=0;
#endif

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)
 static unsigned int com_addr;
 #define com_out (com_addr+4)
 #define com_in  (com_addr+6)
#endif

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
	// It seems I have the same problem as FlashZ !!!
	TRYC(open_com_port(io_device, &hCom));
#endif

#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)
  com_addr = io_address;

  TRYC(open_io(com_out, 1));
  io_permitted++;
  TRYC(open_io(com_in, 1));
  io_permitted++;

  wr_io(com_out, 3);
  wr_io(com_out, 0);
  wr_io(com_out, 3);
#endif

  START_LOGGING();

  return 0;
}

int ser_open()
{
  tdr.count = 0;
  toSTART(tdr.start);

  if(io_permitted)
    return 0;
  else
    return ERR_ROOT;
}

int ser_put(byte data)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)  
  int bit;
  int i;
  TIME clk;

  tdr.count++;
  LOG_DATA(data);
  for(bit=0; bit<8; bit++)
    {
      if(data & 1)
	{
	  wr_io(com_out, 2);
	  toSTART(clk);
	  do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(com_in) & 0x10));
	  wr_io(com_out, 3);
	  toSTART(clk);
	  do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(com_in) & 0x10)==0x00);
	}
      else
	{
	  wr_io(com_out, 1);
	  toSTART(clk);
          do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while(rd_io(com_in) & 0x20);
	  wr_io(com_out, 3);
	  toSTART(clk);
          do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((rd_io(com_in) & 0x20)==0x00);
        }
      data>>=1;
      for(i=0; i<delay; i++) rd_io(com_in);
    }
#endif

  return 0;
}

int ser_get(byte *ch)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)  
  int bit;
  byte data=0;
  byte v;
  int i;
  TIME clk;

  tdr.count++;
  for(bit=0; bit<8; bit++)
    {
      toSTART(clk);
      while((v=rd_io(com_in) & 0x30)==0x30)
	{
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BIT_TIMEOUT;
	}
      if(v==0x10)
	{
	  data=(data >> 1) | 0x80;
	  wr_io(com_out, 1);
	  while((rd_io(com_in) & 0x20)==0x00);
	  wr_io(com_out, 3);
	}
      else
	{
	  data=(data >> 1) & 0x7F;
	  wr_io(com_out, 2);
	  while((rd_io(com_in) & 0x10)==0x00);
	  wr_io(com_out, 3);
	}
      for(i=0; i<delay; i++) rd_io(com_in);
    }
  *ch=data;
  LOG_DATA(data);
#endif
  
  return 0;
}

int ser_probe()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)  
	
  int i, j;
  int seq[]={ 0x00, 0x20, 0x00, 0x20 };
  int data;

  for(i=3; i>=0; i--)
    {
      wr_io(com_out, 3);
      wr_io(com_out, i);
      for(j=0; j<10; j++) data = rd_io(com_in);
      //DISPLAY("%i: 0x%02x 0x%02x\n", i, data, seq[i]);
      if( (data & 0x30) != seq[i])
	{
	  wr_io(com_out, 3);
	  return ERR_PROBE_FAILED;
	}
    }
  wr_io(com_out, 3);
#else
  return ERR_PROBE_FAILED;   
#endif
  
  return 0;
}

int ser_close()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)  
	
  if(io_permitted == 2)
    wr_io(com_out, 3);
#endif

  return 0;
}

int ser_exit()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__) 
  TRYC(close_io(com_out, 1));
  io_permitted--;
  TRYC(close_io(com_in, 1));
  io_permitted--;
#endif

#ifdef __WIN32__
	TRYC(close_com_port(&hCom));
#endif

  STOP_LOGGING();

  return 0;
}

int ser_check(int *status)
{
  *status = STATUS_NONE;
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined (__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)  
  
  if(!( (rd_io(com_in) & 0x30) == 0x30 ))
    {
      *status = (STATUS_RX | STATUS_TX);
    }
#endif

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ser_set_red_wire(int b)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  int v = swap_bits(rd_io(com_in) >> 4);
  if(b)
    wr_io(com_out, v | 0x02);
  else
    wr_io(com_out, v & ~0x02);
#endif

  return 0;
}

int ser_set_white_wire(int b)
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  int v = swap_bits(rd_io(com_in) >> 4);
  if(b)
    wr_io(com_out, v | 0x01);
  else
    wr_io(com_out, v & ~0x01);
#endif

  return 0;
}

int ser_get_red_wire()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  return ((0x10 & rd_io(com_in)) ? 1 : 0);
#endif

  return 0;
}

int ser_get_white_wire()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__)
  return ((0x20 & rd_io(com_in)) ? 1 : 0);
#endif

  return 0;
}

int ser_supported()
{
#if defined(__I386__) && defined(HAVE_ASM_IO_H) && defined(HAVE_SYS_PERM_H) || defined(__WIN32__) || defined(__WIN16__) || defined(__ALPHA__)
  return SUPPORT_ON | ((method & IOM_DCB) ? SUPPORT_DCB : SUPPORT_IO);
#else
  return SUPPORT_OFF;
#endif
}

/* 
 * This part manage the link cable with DCB I/O routines.
 *
 * Remark: the low-level part should not require DCB code but pratically,
 * some machines and link cables may require it for working properly
 * Especially FlashZ who had this boring problem !
 */

#ifdef __WIN32__
int ser_init2()
{
  TRYC(open_io(com_out, 1));
  io_permitted++;
  TRYC(open_io(com_in, 1));
  io_permitted++;

  wr_io(com_out, 3);
  wr_io(com_out, 0);
  wr_io(com_out, 3);

  return 0;
}

int ser_open2()
{
	tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int ser_put2(byte data)
{
  int i;
  TIME clk;

  tdr.count++;
  toSTART(clk);
  for (i=0;i<8;i++)
    {
      if (data&1)
	wr_io(com_out, 2);
      else
	wr_io(com_out, 1);
      while(rd_io(com_in)!=0)
        {
	  if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
        }
      wr_io(com_out, 3);
      while(rd_io(com_in)!=3)
        {
	  if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
        }
      data>>=1;
    }

  return 0;
}

int ser_get2(byte *ch)
{
  int bit;
  byte data=0;
  int i, j;
  TIME clk;

  tdr.count++;
  toSTART(clk);
  for (i=0,bit=1,*ch=0;i<8;i++)
    {
      while ((j=rd_io(com_in))==3)
        {
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BIT_TIMEOUT;
        }
      if (j==1)
        {
	  *ch|=bit;
	  wr_io(com_out, 1);
	  j=2;
        }
      else
        {
	  wr_io(com_out, 2);
	  j=1;
        }
      while ((rd_io(com_in)&j)==0)
        {
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BIT_TIMEOUT;
        }
      wr_io(com_out, 3);
      bit<<=1;
    }

  return 0;
}

int ser_probe2()
{
  int i, j;
  int seq[]={ 0x00, 0x20, 0x10, 0x30 };

  for(i=3; i>=0; i--)
    {
      wr_io(com_out, 3);
      wr_io(com_out, i);
      for(j=0; j<10; j++) rd_io(com_in);
      //DISPLAY("%i %02X %02X %02X\n", i, rd_io(com_in), rd_io(com_in) & 0x30, seq[i]);
      if( (rd_io(com_in) & 0x30) != seq[i])
	{
	  wr_io(com_out, 3);
	  return ERR_ROOT;
	}
    }
  wr_io(com_out, 3);
  
  return 0;
}

int ser_close2()
{
  if(io_permitted == 2)
    wr_io(com_out, 3);

  return 0;
}

int ser_exit2()
{
  TRYC(close_io(com_out, 1));
  io_permitted--;
  TRYC(close_io(com_in, 1));
  io_permitted--;

  return 0;
}

int ser_check2(int *status)
{
  *status = STATUS_NONE;
  
  if(!( (rd_io(com_in) & 0x30) == 0x30 ))
    {
      *status = (STATUS_RX | STATUS_TX);
    }

  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int ser_set_red_wire2(int b)
{
  int v = swap_bits(rd_io(com_in) >> 4);
  if(b)
    wr_io(com_out, v | 0x02);
  else
    wr_io(com_out, v & ~0x02);

  return 0;
}

int ser_set_white_wire2(int b)
{
  int v = swap_bits(rd_io(com_in) >> 4);
  if(b)
    wr_io(com_out, v | 0x01);
  else
    wr_io(com_out, v & ~0x01);

  return 0;
}

int ser_get_red_wire2()
{
  return ((0x10 & rd_io(com_in)) ? 1 : 0);

  return 0;
}

int ser_get_white_wire2()
{
  return ((0x20 & rd_io(com_in)) ? 1 : 0);

  return 0;
}

int ser_supported2()
{
  return SUPPORT_ON | ((method & IOM_DCB) ? SUPPORT_DCB : SUPPORT_IO);
}

#endif /* __WIN32__ */

