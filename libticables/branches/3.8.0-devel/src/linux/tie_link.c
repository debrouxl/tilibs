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

/* "TiEmu" virtual link cable unit */

/* 
 *  This unit use two FIFOs between 2 program which use this lib.
 *  Convention used: 0 is an emulator and 1 is a linking program.
 *  One pipe is used for transferring information from 0 to 1 and the other
 *  pipe is used for transferring from 1 to 0.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "intl.h"
#include "timeout.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "logging.h"
#include "verbose.h"

#define BUFFER_SIZE 256
#define HIGH 666		// upper limit (used for avoiding 'uint8_t timeout')
#define LOW  333		// lower limit

static int p;
static int ref_cnt = 0;		// Counter of library instances

static int rd[2] = { 0, 0 };	// Pipe 0 <- 1 or 1 <- 0
static int wr[2] = { 0, 0 };	// Pipe 0 -> 1 or 1 -> 0

static const char fifo_names[4][256] = {
  "/tmp/.vlc_1_0", "/tmp/.vlc_0_1",
  "/tmp/.vlc_0_1", "/tmp/.vlc_1_0"
};

int tie_init()
{
  if ((io_address < 1) || (io_address > 2)) {
    DISPLAY_ERROR
	(_("Invalid io_address parameter passed to libticables.\n"));
    io_address = 2;
  }
  p = io_address - 1;

  /* Check if the pipes already exist else create them */
  if (access(fifo_names[0], F_OK) | access(fifo_names[1], F_OK)) {
    mkfifo(fifo_names[0], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
    mkfifo(fifo_names[1], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
  }

  /* Open the pipes */
  // Open the 1->0 pipe in reading
  if ((rd[p] = open(fifo_names[2 * (p) + 0], O_RDONLY | O_NONBLOCK)) == -1) {
    DISPLAY_ERROR(_("error: %s\n"), strerror(errno));
    return ERR_OPEN_PIPE;
  }
  // Open the 0->1 pipe in writing (in reading at first)
  if ((wr[p] = open(fifo_names[2 * (p) + 1], O_RDONLY | O_NONBLOCK)) == -1) {
    return ERR_OPEN_PIPE;
  }
  if ((wr[p] = open(fifo_names[2 * (p) + 1], O_WRONLY | O_NONBLOCK)) == -1) {
    return ERR_OPEN_PIPE;
  }
  ref_cnt++;
  START_LOGGING();

  return 0;
}

int tie_open()
{
  uint8_t d;
  int n;

  /* Flush the pipe */
  do {
    n = read(rd[p], (void *) (&d), 1);
  } while (n > 0);

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int tie_put(uint8_t data)
{
  int n = 0;
  tiTIME clk;
  struct stat s;

  tdr.count++;
  /* Check if the other pipe is used */
  /* if(ref_cnt < 2)
     return ERR_OPP_NOT_AVAIL;
   */

  LOG_DATA(data);
  /* Transfer rate modulation */
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_WRITE_TIMEOUT;
    fstat(wr[p], &s);
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
    n = write(wr[p], (void *) (&data), 1);
  }
  while (n <= 0);

  return 0;
}

int tie_get(uint8_t * data)
{
  static int n = 0;
  tiTIME clk;

  // Read the uint8_t in a defined delay
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
    n = read(rd[p], (void *) data, 1);
  }
  while (n <= 0);

  if (n == -1) {
    return ERR_READ_ERROR;
  }

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int tie_probe()
{
  return 0;
}

int tie_close()
{
  return 0;
}

int tie_exit()
{
  STOP_LOGGING();

  if (rd[p]) {
    /* Close the pipe */
    if (close(rd[p]) == -1) {
      return ERR_CLOSE_PIPE;
    }
    rd[p] = 0;
  }
  if (wr[p]) {
    /* Close the pipe */
    if (close(wr[p]) == -1) {
      return ERR_CLOSE_PIPE;
    }
    wr[p] = 0;
  }
  ref_cnt--;

  return 0;
}

int tie_check(int *status)
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

int tie_supported()
{
  return SUPPORT_OFF;
}

int tie_register_cable(TicableLinkCable * lc)
{
  lc->init = tie_init;
  lc->open = tie_open;
  lc->put = tie_put;
  lc->get = tie_get;
  lc->close = tie_close;
  lc->exit = tie_exit;
  lc->probe = tie_probe;
  lc->check = tie_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
