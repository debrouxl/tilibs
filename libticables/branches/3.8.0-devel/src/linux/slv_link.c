/* Hey EMACS -*- linux-c -*- */
/* $Id: slv_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* TI-GRAPH LINK USB support (kernel module) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intl.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "verbose.h"
#include "logging.h"
#include "externs.h"
#include "timeout.h"

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   
   This link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
   This is transparent for the user because the driver manages all these 
   things for us. Nethertheless, this fact has some consequences:
   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.
   - for reading, we have to read up to 32 bytes at a time (even if we need 
   only 1 byte) and to store them in a buffer for subsequent acesses. 
   In fact, if we try and get byte per byte, it will not work.
   - for writing, we don't store bytes in a buffer. It seems better to send
   data byte per byte (latency ?!).
   - another particular effect (quirk): sometimes (usually when calc need to 
   reply and takes a while), a read call can returns with no data or timeout. 
   Simply retry a read call and it works fine.
*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

//#define BUFFERED_W /* enable buffered write operations */ 
#define BUFFERED_R		/* enable buffered read operations (default) */

#define MAX_PACKET_SIZE 32	// 32 bytes max per packet
static int nBytesWrite = 0;
#ifdef BUFFERED_W
static uint8_t wBuf[MAX_PACKET_SIZE];
#endif
static int nBytesRead = 0;
static uint8_t rBuf[MAX_PACKET_SIZE];

static int dev_fd = 0;

#ifdef HAVE_TILP_TICABLE_H
# include <tilp/ticable.h>	//ioctl codes
# include <sys/ioctl.h>
#endif

int slv_init()
{
  int mask = O_RDWR | O_NONBLOCK | O_SYNC;

  if ((dev_fd = open(io_device, mask)) == -1) {
    DISPLAY(_("Unable to open this device: %s\n"), io_device);
    DISPLAY(_("Is the tiusb.c module loaded ?\n"));
    return ERR_OPEN_USB_DEV;
  }

  START_LOGGING();

  return 0;
}

int slv_open(void)
{
  /* Clear buffers */
  nBytesRead = 0;
  nBytesWrite = 0;

#ifdef HAVE_TILP_TICABLE_H
  {
    int arg = time_out;
    if (ioctl(dev_fd, IOCTL_TIUSB_TIMEOUT, arg) == -1) {
      DISPLAY_ERROR(_("Unable to use IOCTL codes.\n"));
      return ERR_IOCTL;
    }
  }
#endif

  /* Reset both endpoints */
#ifdef HAVE_TILP_TICABLE_H
  {
    int arg = 0;
    if (ioctl(dev_fd, IOCTL_TIUSB_RESET_PIPES, arg) == -1) {
      DISPLAY(_("Unable to use IOCTL codes.\n"));
      //return ERR_IOCTL;
    }
  }
#endif
  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int slv_put(uint8_t data)
{
  int err;

  tdr.count++;
  LOG_DATA(data);
#ifndef BUFFERED_W
  /* Byte per uint8_t */
  err = write(dev_fd, (void *) (&data), 1);
  switch (err) {
  case -1:			//error
    return ERR_WRITE_ERROR;
    break;
  case 0:			// timeout
    return ERR_WRITE_TIMEOUT;
    break;
  }
#else
  /* Packets (up to 32 bytes) */
  wBuf[nBytesWrite++] = data;
  if (nBytesWrite == MAX_PACKET_SIZE) {
    err = write(dev_fd, (void *) (&wBuf), nBytesWrite);
    nBytesWrite = 0;

    switch (err) {
    case -1:			//error
      return ERR_WRITE_ERROR;
      break;
    case 0:			// timeout
      return ERR_WRITE_TIMEOUT;
      break;
    }
  }
#endif

  return 0;
}

int slv_get(uint8_t * data)
{
  tiTIME clk;
  static uint8_t *rBufPtr;
  int ret;

#ifdef BUFFERED_W
  /* Flush write buffer */
  if (nBytesWrite > 0) {
    ret = write(dev_fd, (void *) (&wBuf), nBytesWrite);
    nBytesWrite = 0;
    switch (err) {
    case -1:			//error
      return ERR_READ_ERROR;
      break;
    case 0:			// timeout
      return ERR_READ_TIMEOUT;
      break;
    }
  }
#endif

#ifdef BUFFERED_R
  /* This routine try to read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and 
     store them in a buffer for subsequent accesses */
  if (nBytesRead == 0) {
    toSTART(clk);
    do {
      ret = read(dev_fd, (void *) rBuf, MAX_PACKET_SIZE);
      if (toELAPSED(clk, time_out))
	return ERR_READ_TIMEOUT;
      if (ret == 0)
	DISPLAY_ERROR
	    (_
	     ("read returns without any data. Retrying for circumventing quirk...\n"));
    }
    while (!ret);

    if (ret < 0) {
      nBytesRead = 0;
      return ERR_READ_ERROR;
    }
    nBytesRead = ret;
    rBufPtr = rBuf;
  }

  *data = *rBufPtr++;
  nBytesRead--;
#else
  nBytesRead = read(dev_fd, (void *) data, 1);
  if (nBytesRead == -1)
    return ERR_READ_ERROR;
  if (nBytesRead == 0)
    return ERR_READ_TIMEOUT;
#endif

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int slv_probe(void)
{
  return 0;
}

int slv_close(void)
{
  return 0;
}

int slv_exit()
{
  if (dev_fd) {
    close(dev_fd);
    dev_fd = 0;
  }

  STOP_LOGGING();

  return 0;
}

int slv_check(int *status)
{
  fd_set rdfs;
  struct timeval tv;
  int retval;

  *status = STATUS_NONE;

  FD_ZERO(&rdfs);
  FD_SET(dev_fd, &rdfs);
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  retval = select(dev_fd + 1, &rdfs, NULL, NULL, &tv);
  switch (retval) {
  case -1:			//error
    return ERR_READ_ERROR;
  case 0:			//no data
    return 0;
  default:			// data available
    *status = STATUS_RX;
    break;
  }

  return 0;
}

int slv_supported()
{
  return SUPPORT_ON;
}

int slv_register_cable(TicableLinkCable * lc, TicableMethod method)
{
  lc->init = slv_init;
  lc->open = slv_open;
  lc->put = slv_put;
  lc->get = slv_get;
  lc->close = slv_close;
  lc->exit = slv_exit;
  lc->probe = slv_probe;
  lc->check = slv_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
