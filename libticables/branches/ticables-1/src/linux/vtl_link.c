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

/* 
 * General purpose virtual link cable unit (generic)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "stdints.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "timeout.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "logging.h"
#include "printl.h"

static int p;
static int ref_cnt = 0;		// Counter of library instances

static int rd[2] = { 0, 0 };	// Pipe 0 <- 1 or 1 <- 0
static int wr[2] = { 0, 0 };	// Pipe 0 -> 1 or 1 -> 0

#define HIGH 666		// 2 of 3
#define LOW  333		// 1 of 3

static const char fifo_names[4][256] = {
  "/tmp/.vlc_1_0", "/tmp/.vlc_0_1",
  "/tmp/.vlc_0_1", "/tmp/.vlc_1_0"
};

static struct cs {
  	uint8_t data;
  	int available;
} cs;

int vtl_init()
{
  	/* Init some internal variables */
  	cs.available = 0;
  	cs.data = 0;

  	/*
     	if(io_addr > 2)
     	return ERR_ILLEGAL_OP;
     	else
     	p = io_addr;
   	*/

  	p = io_address;
  	//printl1(2, "io_address: %i\n", io_address);

  	/* Check if the pipes already exist else create them */
  	if (access(fifo_names[0], F_OK) | access(fifo_names[1], F_OK)) {
    		mkfifo(fifo_names[0], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
    		mkfifo(fifo_names[1], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
  	}

  	/* Open pipes */
  	// Open the 1->0 pipe in reading
  	if ((rd[p - 1] = open(fifo_names[2 * (p - 1) + 0],
			O_RDONLY | O_NONBLOCK)) == -1) {
    		printl1(2, "error: %s\n", strerror(errno));
    		return ERR_OPEN_PIPE;
  	}
  	// Open the 0->1 pipe in writing (in reading at first)
  	if ((wr[p - 1] = open(fifo_names[2 * (p - 1) + 1],
			O_RDONLY | O_NONBLOCK)) == -1) {
		printl1(2, "error: %s\n", strerror(errno));
    		return ERR_OPEN_PIPE;
  	}
  	if ((wr[p - 1] = open(fifo_names[2 * (p - 1) + 1],
			O_WRONLY | O_NONBLOCK)) == -1) {
    		return ERR_OPEN_PIPE;
  	}
  	ref_cnt++;

  	return 0;
}

int vtl_exit()
{
  	if (rd[p - 1]) {
    		/* Close the pipe */
    		if (close(rd[p - 1]) == -1) {
      			return ERR_CLOSE_PIPE;
  	  	}
    		rd[p - 1] = 0;
  	}
  	
  	if (wr[p - 1]) {
    		/* Close the pipe */
    		if (close(wr[p - 1]) == -1) {
      			return ERR_CLOSE_PIPE;
    		}
    		wr[p - 1] = 0;
  	}
  	
  	ref_cnt--;

  	return 0;
}

int vtl_open()
{
  	uint8_t d;
  	int n;

  	/* Flush the pipe */
  	do {
    		n = read(rd[p - 1], (void *) (&d), 1);
  	}
  	while (n > 0);

  	tdr.count = 0;
  	toSTART(tdr.start);

  	return 0;
}

int vtl_close()
{
  	return 0;
}

int vtl_put(uint8_t data)
{
  	int n = 0;
  	tiTIME clk;
  	struct stat s;

  	/* Check if the other pipe is used */
  	/*
     	if(ref_cnt < 2)
     	return ERR_OPP_NOT_AVAIL;
   	*/

  	tdr.count++;
  	toSTART(clk);
  	do {
    		if (toELAPSED(clk, time_out))
      			return ERR_WRITE_TIMEOUT;
      		/* Transfer rate modulation */
    		fstat(wr[p - 1], &s);
    		if (s.st_size > HIGH)
      			n = 0;
    		else if (s.st_size < LOW)
      			n = 1;
  	}
  	while (n <= 0);

  	/* Write the data in a defined delay */
  	toSTART(clk);
  	do {
    		if (toELAPSED(clk, time_out))
      			return ERR_WRITE_TIMEOUT;
    		n = write(wr[p - 1], (void *) (&data), 1);
  	}
  	while (n <= 0);

  	return 0;
}

int vtl_get(uint8_t * data)
{
  	static int n = 0;
  	tiTIME clk;

  	tdr.count++;
  	if (cs.available) {
    		*data = cs.data;
    		cs.available = 0;
    		return 0;
  	}
  
  	// Read the uint8_t in a defined delay
  	toSTART(clk);
  	do {
    		if (toELAPSED(clk, time_out))
      			return ERR_READ_TIMEOUT;
    		n = read(rd[p - 1], (void *) data, 1);
  	}
  	while (n <= 0);

  	if (n == -1) {
    		return ERR_READ_ERROR;
  	}

  	return 0;
}

int vtl_check(int *status)
{
  	int n = 0;

  	/* Since the select function does not work, I do it myself ! */
  	*status = STATUS_NONE;
  	if (rd) {
    		n = read(rd[p - 1], (void *) (&cs.data), 1);
    		if (n > 0) {
      			if (cs.available == 1)
			return ERR_BYTE_LOST;

      			cs.available = 1;
      			*status = STATUS_RX;
      		
      			return 0;
    		} else {
	      		*status = STATUS_NONE;
      			return 0;
    		}
  	}

  	return 0;
}

int vtl_probe()
{
  	return 0;
}

int vtl_supported()
{
  	return SUPPORT_OFF;
}

int vtl_register_cable(TicableLinkCable * lc)
{
  lc->init = vtl_init;
  lc->open = vtl_open;
  lc->put = vtl_put;
  lc->get = vtl_get;
  lc->close = vtl_close;
  lc->exit = vtl_exit;
  lc->probe = vtl_probe;
  lc->check = vtl_check;

  return 0;
}
