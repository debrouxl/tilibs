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

/* TI-GRAPH LINK USB support (kernel module) */

/* 
   Some important remarks... (http://lpg.ticalc.org/prj_usb/index.html)
   
   This link cable use Bulk mode with packets. The max size of a packet is 
   32 bytes (MAX_PACKET_SIZE/BULKUSB_MAX_TRANSFER_SIZE). 
   
   This is transparent for the user because the libusb manages all these 
   things for us. Nethertheless, this fact has some consequences:

   - it is better (for USB & OS performances) to read/write a set of bytes 
   rather than byte per byte.

   - for reading, we have to read up to 32 bytes at a time (even if we need 
   only 1 byte) and to store them in a buffer for subsequent accesses. 
   In fact, if we try and get byte per byte, it will not work.

   - for writing, we don't store bytes in a buffer. It seems better to send
   data byte per byte (latency ?!). But, this make data-rate significantly 
   decrease (1KB/s instead of 5KB/s).
   Another way is to use partially buffered write operations: send consecutive
   blocks as a whole but partial block byte per byte. This is the best 
   compromise.

   - another particular effect (quirk): sometimes (usually when calc need to 
   reply and takes a while), a read call can returns with neither data nor 
   timeout. Simply retry a read call and it works fine. The best example is 
   to get IDLIST.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

#include "intl1.h"
#include "export.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "printl.h"
#include "logging.h"
#include "externs.h"
#include "timeout.h"

#define BUFFERED_W    /* enable buffered write operations	  */ 
#define BUFFERED_R    /* enable buffered read operations (always) */

#define MAX_PACKET_SIZE 32	// 32 bytes max per packet
static int nBytesWrite = 0;
#ifdef BUFFERED_W
static uint8_t wBuf[MAX_PACKET_SIZE];
#endif
static int nBytesRead = 0;
static uint8_t rBuf[MAX_PACKET_SIZE];

static int dev_fd = 0;

#ifdef HAVE_LINUX_TICABLE_H
# include <linux/ticable.h>	//ioctl codes
# include <sys/ioctl.h>
#endif

int slv_init()
{
	int mask = O_RDWR | O_NONBLOCK | O_SYNC;
	
	if ((dev_fd = open(io_device, mask)) == -1) {
		printl1(2, _("unable to open this device: %s.\n"), 
			      io_device);
		return ERR_OPEN_USB_DEV;
	}
	
	START_LOGGING();

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

int slv_open(void)
{
#ifdef HAVE_LINUX_TICABLE_H
  {
    int arg = time_out;

    if (ioctl(dev_fd, IOCTL_TIUSB_TIMEOUT, arg) == -1) {
	    printl1(2, _("unable to set timeout (ioctl).\n"));
	    //return ERR_IOCTL;
    }
  }
#endif

  /* Reset both endpoints */
#ifdef HAVE_LINUX_TICABLE_H
 {
    int arg = 0;

    if (ioctl(dev_fd, IOCTL_TIUSB_RESET_PIPES, arg) == -1) {
	    printl1(2, _("unable to reset pipes (ioctl).\n"));
	    //return ERR_IOCTL;
    }
  }
#endif

	/* Clear buffers */
	nBytesRead = 0;
	nBytesWrite = 0;

	tdr.count = 0;
	toSTART(tdr.start);

  return 0;
}

static int send_fblock(uint8_t *data, int length);
static int send_pblock(uint8_t *data, int length);

int slv_close(void)
{
#if defined( BUFFERED_W )
	int ret;

	/* Flush write buffer byte per byte (last command) */
	if (nBytesWrite2 > 0) {
		ret = send_pblock(wBuf2, nBytesWrite2);
		nBytesWrite2 = 0;
		if(ret) return ret;
	}
#endif	
	
  	return 0;
}

int slv_put(uint8_t data)
{
  	int ret;

  	tdr.count++;
  	LOG_DATA(data);

#if !defined( BUFFERED_W )
  	/* Byte per uint8_t */
  	ret = write(dev_fd, (void *) (&data), 1);
  	
  	if(ret == -1)
  		return ERR_WRITE_ERROR;
  	if(!ret)
  		return ERR_WRITE_TIMEOUT;
#else
  	/* Fill buffer (up to 32 bytes) */
  	wBuf[nBytesWrite++] = data;
  	
  	/* Buffer full? Send the whole buffer at once */
  	if (nBytesWrite == MAX_PACKET_SIZE) {
    		ret = send_fblock(wBuf2, nBytesWrite2);
    		nBytesWrite = 0;
		if(ret) return ret;
  	}
#endif

  	return 0;
}

int slv_get(uint8_t * data)
{
	int ret;
  	tiTIME clk;
  	static uint8_t *rBufPtr;

#if defined( BUFFERED_W )
  	/* Flush write buffer byte per byte (more reliable) */
  	if (nBytesWrite > 0) {
    		ret = send_pblock(wBuf2, nBytesWrite2);
		nBytesWrite2 = 0;
		if(ret) return ret;
	}
#endif

#ifdef BUFFERED_R
  	/* Read up to 32 bytes (BULKUSB_MAX_TRANSFER_SIZE) and 
     		store them in a buffer for subsequent accesses */
  	if (nBytesRead <= 0) {
    		toSTART(clk);
    		do {
      			ret = read(dev_fd, (void *) rBuf, MAX_PACKET_SIZE);
      			
      			if (toELAPSED(clk, time_out))
				return ERR_READ_TIMEOUT;
      			if (ret == 0)		// quirk (seems to be due to Cypress components)
				printl1(2, _("weird, read returns without any data; retrying to circumvent the quirk...\n"));
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
#endif

  	tdr.count++;
  	LOG_DATA(*data);

  	return 0;
}

int slv_probe(void)
{
	// could be done by parsing /proc/bus/usb/devices but usbfs must be mounted
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

int slv_register_cable_1(TicableLinkCable * lc)
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

/***/

#if defined( BUFFERED_W )
static int send_fblock(uint8_t *data, int length)
{
	int ret;
	
	ret = write(dev_fd, (void *) (&data), length);
  	
  	if(!ret)
  		return ERR_WRITE_TIMEOUT;
  	if(ret == -1)
  		return ERR_WRITE_ERROR;

	return 0;
}

static int send_pblock(uint8_t *data, int length)
{
	int i, ret;

	for(i=0; i<length; i++) {
		ret = send_fblock(&wBuf2[i], 1);
		if(ret) return ret;
	}

	return 0;
}
#endif