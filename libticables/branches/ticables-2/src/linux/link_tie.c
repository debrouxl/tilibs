/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* "TiEmu" virtual link cable unit */

/* 
 *  This unit use two FIFOs between 2 programs which use this lib.
 *  Convention used: 0 is an emulator and 1 is a linking program.
 *  One pipe is used for transferring information from 0 to 1 and the other
 *  pipe is used for transferring from 1 to 0.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

#define BUFFER_SIZE 256
#define HIGH 666		// upper limit (used for avoiding timeout)
#define LOW  333		// lower limit

static int p;
static int ref_cnt = 0;		// Counter of library instances

static int rd[2] = { 0, 0 };	// Pipe 0 <- 1 or 1 <- 0
static int wr[2] = { 0, 0 };	// Pipe 0 -> 1 or 1 -> 0

static const char fifo_names[4][256] = {
  "/tmp/.vlc_1_0", "/tmp/.vlc_0_1",
  "/tmp/.vlc_0_1", "/tmp/.vlc_1_0"
};

static int tie_prepare(TiHandle *h)
{
	h->address = 0;
	h->device = strdup("");

	return 0;
}

static int tie_open(TiHandle *h)
{
    if ((h->address < 1) || (h->address > 2)) {
	ticables_warning(_("Invalid h->address parameter passed to libticables.\n"));
    h->address = 2;
    }
    p = h->address - 1;
    
    /* Check if the pipes already exist else create them */
    if (access(fifo_names[0], F_OK) | access(fifo_names[1], F_OK)) {
	mkfifo(fifo_names[0], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
	mkfifo(fifo_names[1], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
    }
    
    /* Open the pipes */
    // Open the 1->0 pipe in reading
    if ((rd[p] = open(fifo_names[2 * (p) + 0], O_RDONLY | O_NONBLOCK)) == -1) 
    {
	ticables_warning(_("error: %s\n"), strerror(errno));
	return ERR_TIE_OPEN;
    }
    // Open the 0->1 pipe in writing (in reading at first)
    if ((wr[p] = open(fifo_names[2 * (p) + 1], O_RDONLY | O_NONBLOCK)) == -1) 
    {
	return ERR_TIE_OPEN;
    }
    if ((wr[p] = open(fifo_names[2 * (p) + 1], O_WRONLY | O_NONBLOCK)) == -1) 
    {
	return ERR_TIE_OPEN;
    }
    ref_cnt++;

  return 0;
}

static int tie_close(TiHandle *h)
{
    if (rd[p]) 
    {
	/* Close the pipe */
	if (close(rd[p]) == -1) 
	{
	    return ERR_TIE_CLOSE;
	}
	rd[p] = 0;
    }
    if (wr[p]) 
    {
	/* Close the pipe */
	if (close(wr[p]) == -1) {
	    return ERR_TIE_CLOSE;
	}
	wr[p] = 0;
    }
    ref_cnt--;
    
    return 0;
}

static int tie_reset(TiHandle *h)
{
    uint8_t d;
    int n;
    
    /* Flush the pipe */
    do 
    {
	n = read(rd[p], (void *) (&d), 1);
    } while (n > 0);
    
    return 0;
}

static int tie_put(TiHandle *h, uint8_t *data, uint16_t len)
{
    int n = 0;
    tiTIME clk;
    struct stat s;
    
    /* Transfer rate modulation */
    TO_START(clk);
    do {
	if (TO_ELAPSED(clk, h->timeout))
	    return ERR_WRITE_TIMEOUT;
	fstat(wr[p], &s);
	if (s.st_size > HIGH)
	    n = 0;
	else if (s.st_size < LOW)
	    n = 1;
    }
    while (n <= 0);
    
    /* Write the data in a defined delay */
    TO_START(clk);
    do {
	if (TO_ELAPSED(clk, h->timeout))
	    return ERR_WRITE_TIMEOUT;
	n = write(wr[p], (void *) (&data), 1);
    }
    while (n <= 0);
    
    return 0;
}

static int tie_get(TiHandle *h, uint8_t *data, uint16_t len)
{
    static int n = 0;
    tiTIME clk;
    
    // Read the uint8_t in a defined delay
    TO_START(clk);
    do {
	if (TO_ELAPSED(clk, h->timeout))
	    return ERR_READ_TIMEOUT;
	n = read(rd[p], (void *) data, 1);
    }
    while (n <= 0);
    
    if (n == -1) {
	return ERR_READ_ERROR;
    }
    
    return 0;
}

static int tie_probe(TiHandle *h)
{
	return 0;
}

static int tie_check(TiHandle *h, int *status)
{
    fd_set rdfs;
    struct timeval tv;
    int retval;
    
    *status = STATUS_NONE;
    
    FD_ZERO(&rdfs);
    FD_SET(rd[p], &rdfs);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    retval = select(rd[p] + 1, &rdfs, NULL, NULL, &tv);
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

static int tie_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

static int tie_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

static int tie_get_red_wire(TiHandle *h)
{
	return 1;
}

static int tie_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_tie = 
{
	CABLE_TIE,
	"TIE",
	N_("TiEmu"),
	N_("Virtual link for TiEmu"),
	0,
	&tie_prepare,
	&tie_open, &tie_close, &tie_reset, &tie_probe,
	&tie_put, &tie_get, &tie_check,
	&tie_set_red_wire, &tie_set_white_wire,
	&tie_get_red_wire, &tie_get_white_wire,
};
