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

/* Linux tidev device driver support
 * Copyright (c) 2000 Leonard Stiles <ljs@uk2.net> 
 */

/* I have modified this module so that it can support the new functionnalities
 * that bring about my kernel modules (tipar & tiser modules).
 * But, it still support the 'tidev' kernel module.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "export.h"
#include "cabl_def.h"

#if defined(__LINUX__)

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#ifdef HAVE_TI_TIPAR_H
# include <ti/tipar.h>
# include <sys/ioctl.h>
#endif
#ifdef HAVE_TI_TISER_H
# include <ti/tiser.h>
# include <sys/ioctl.h>
#endif

#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "timeout.h"
#include "export.h"
#include "verbose.h"
#include "logging.h"

//#define TI_DEV_FILE "/dev/ti" /* Removed by roms */
extern const char * ti_dev_file;
extern int time_out;

//int tidev; /* Removed by roms */
static int dev_fd = 0;

static struct cs
{
  byte data;
  int available;
} cs;

int dev_init()
{
#if defined(HAVE_TI_TIPAR_H) || defined(HAVE_TI_TISER_H)
  int value;
#endif
  int mask;

  /* Init some internal variables */
  cs.available = 0;
  cs.data = 0;

  /* Open the device */
  mask = O_RDWR | O_NONBLOCK | O_SYNC;
  if( (dev_fd = open(io_device, mask)) == -1)
    {
      DISPLAY_ERROR("unable to open this device: <%s>\n", io_device);
      DISPLAY_ERROR("is the module loaded ?\n");
      return ERR_OPEN_TIDEV_DEV;
    }

  /* Initialize it */
#if defined(HAVE_TI_TIPAR_H) || defined(HAVE_TI_TISER_H)
  value = delay;
  if(ioctl(dev_fd, TIDEV_DELAY, value) == -1)
    {}
  value = time_out;
  if(ioctl(dev_fd, TIDEV_TIMEOUT, value) == -1)
    {}
#endif

  START_LOGGING();

  return 0;
}

int dev_open(void)
{
  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int dev_put(byte data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);
  err = write(dev_fd, (void *)(&data), 1);
  switch(err)
    {
    case -1: //error
      return ERR_SND_BYT;
      break;
    case 0: // timeout
      return ERR_SND_BYT_TIMEOUT;
      break;
    }

  return 0;
}

int dev_get(byte *data)
{
  static int n=0;
  TIME clk;

  tdr.count++;
  /* If the dev_check function was previously called, retrieve the byte */
  if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

  toSTART(clk);
  do
    {
      if(toELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
      n = read(dev_fd, (void *)data, 1);
    }
  while(n == 0);
  
  if(n == -1)
    {
      return ERR_RCV_BYT;
    }
  LOG_DATA(*data);

  return 0;
}

int dev_probe(void)
{
  return 0;
}

int dev_close(void)
{
  return 0;
}

int dev_exit()
{
  STOP_LOGGING();
  if(dev_fd)
    {
      close(dev_fd);
      dev_fd=0;
    }

  return 0;
}

int dev_check(int *status)
{
  int n = 0;

  /* Since the select function does not work, I do it myself ! */
  *status = STATUS_NONE;
  if(dev_fd)
    {
      n = read(dev_fd, (void *) (&cs.data), 1);
      if(n > 0)
	{
	  if(cs.available == 1)
	    return ERR_BYTE_LOST;

	  cs.available = 1;
	  *status = STATUS_RX;
	  return 0;
	}
      else
	{
	  *status = STATUS_NONE;
	  return 0;
	}
    }

  return 0;
}

int dev_supported()
{
#if defined(HAVE_TI_TIPAR_H) 
  return SUPPORT_ON | SUPPORT_TIPAR;
#elif defined(HAVE_TI_TISER_H)
  return SUPPORT_ON | SUPPORT_TISER;
#else
  return SUPPORT_ON;
#endif
}

#else

/************************/
/* Unsupported platform */
/************************/

int dev_init()
{
  return 0;
}

int dev_open()
{
  return 0;
}

int dev_put(byte data)
{
  return 0;
}

int dev_get(byte *d)
{
  return 0;
}

int dev_probe()
{
  return 0;
}

int dev_close()
{
  return 0;
}

int dev_exit()
{
  return 0;
}

int dev_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int dev_set_red_wire(int b)
{
  return 0;
}

int dev_set_white_wire(int b)
{
  return 0;
}

int dev_get_red_wire()
{
  return 0;
}

int dev_get_white_wire()
{
  return 0;
}

int dev_supported()
{
  return SUPPORT_OFF;
}

#endif
