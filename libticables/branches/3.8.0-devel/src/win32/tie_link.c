/* Hey EMACS -*- linux-c -*- */
/* $Id: tie_link.c 370 2004-03-22 18:47:32Z roms $ */

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
 *  "TiEmulator" virtual link cable unit
 *  This unit use two FIFOs between 2 program which use this lib.
 *  Convention used: 0 is an emulator and 1 is a linking program.
 *  One pipe is used for transferring information from 0 to 1 and the other
 *  pipe is used for transferring from 1 to 0.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define BUFFER_SIZE 256
#define HIGH 666		// upper limit (used for avoiding 'uint8_t timeout')
#define LOW  333		// lower limit

#if defined(__LINUX__)

/**************/
/* Linux part */
/**************/

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

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "intl.h"

#include "timeout.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "externs.h"
#include "export.h"
#include "logging.h"
#include "verbose.h"

extern int time_out;		// Timeout value for cables in 0.10 seconds
extern int delay;		// Time between 2 bits (home-made cables only)
static int p;

static const char name[4][256] = {
  "GtkTiEmu Virtual Link 0", "GtkTiEmu Virtual Link 1",
  "GtkTiEmu Virtual Link 1", "GtkTiEmu Virtual Link 0"
};

#ifdef __GNUC__			// Kevin Kofler
static int ref_cnt __attribute__ ((section(".shared"), shared)) = 0;
#else
#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")	// Share these variables between different instances
static int ref_cnt = 0;		// Counter of library instances
#pragma data_seg()
#endif

typedef struct {
  BYTE buf[BUFFER_SIZE];
  int start;
  int end;
} LinkBuffer;

static HANDLE hSendBuf, hRecvBuf;
static LinkBuffer *pSendBuf, *pRecvBuf;

int tie_init(void)
{
  /* Check if valid argument */
  if ((io_address < 1) || (io_address > 2)) {
    DISPLAY_ERROR
	(_("invalid io_address parameter passed to libticables.\n"));
    io_address = 2;
  } else {
    p = io_address - 1;
    ref_cnt++;
  }

  /* Create a FileMapping objects */
  hSendBuf = CreateFileMapping((HANDLE) (-1), NULL,
			       PAGE_READWRITE, 0, sizeof(LinkBuffer),
			       (LPCTSTR) name[2 * p + 0]);
  if (hSendBuf == NULL) {
    //print_last_error("CreateFileMapping");
    return ERR_OPP_NOT_AVAIL;
  }
  hRecvBuf = CreateFileMapping((HANDLE) (-1), NULL,
			       PAGE_READWRITE, 0, sizeof(LinkBuffer),
			       (LPCTSTR) name[2 * p + 1]);
  if (hRecvBuf == NULL) {
    //print_last_error("CreateFileMapping");
    return ERR_OPP_NOT_AVAIL;
  }

  /* Map them */
  pSendBuf =
      (LinkBuffer *) MapViewOfFile(hSendBuf, FILE_MAP_ALL_ACCESS, 0,
				   0, sizeof(LinkBuffer));
  if (pSendBuf == NULL) {
    //print_last_error("MapViewOfFile");
    return ERR_OPP_NOT_AVAIL;
  }
  pRecvBuf =
      (LinkBuffer *) MapViewOfFile(hRecvBuf, FILE_MAP_ALL_ACCESS, 0,
				   0, sizeof(LinkBuffer));
  if (pRecvBuf == NULL) {
    //print_last_error("MapViewOfFile");
    return ERR_OPP_NOT_AVAIL;
  }

  START_LOGGING();

  return 0;
}

int tie_open()
{
  pSendBuf->start = pSendBuf->end = 0;
  pRecvBuf->start = pRecvBuf->end = 0;

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int tie_put(uint8_t data)
{
  tiTIME clk;

  //if(!hMap)
  //      return ERR_OPEN_FILE_MAP;

  tdr.count++;
  LOG_DATA(data);
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_WRITE_TIMEOUT;
  }
  while (((pSendBuf->end + 1) & 255) == pSendBuf->start);

  pSendBuf->buf[pSendBuf->end] = data;	// put data in buffer
  pSendBuf->end = (pSendBuf->end + 1) & 255;	// update circular buffer

  return 0;
}

int tie_get(uint8_t * data)
{
  tiTIME clk;

  //if(!hMap)
  //      return ERR_OPEN_FILE_MAP;

  //DISPLAY("s: %i, e: %i\n", pSendBuf->start, pSendBuf->end);

  tdr.count++;
  /* Wait that the buffer has been filled */
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
  }
  while (pRecvBuf->start == pRecvBuf->end);

  /* And retrieve the data from the circular buffer */
  *data = pRecvBuf->buf[pRecvBuf->start];
  pRecvBuf->start = (pRecvBuf->start + 1) & 255;
  //DISPLAY("get: 0x%02x\n", *data);
  LOG_DATA(*data);

  return 0;
}

int tie_close()
{
  return 0;
}

int tie_exit()
{
  STOP_LOGGING();
  /* Close the shared buffer */
  if (hSendBuf) {
    UnmapViewOfFile(pSendBuf);
  }
  if (hRecvBuf) {
    UnmapViewOfFile(pRecvBuf);
  }

  return 0;
}

int tie_probe()
{
  return 0;
}

int tie_check(int *status)
{
  /* Check if positions are the same */
  if (pRecvBuf->start == pRecvBuf->end)
    *status = STATUS_NONE;
  else
    *status = STATUS_RX;

  return 0;
}

int tie_supported()
{
  return SUPPORT_ON;
}

#else

/************************/
/* Unsupported platform */
/************************/

#include "cabl_def.h"

int tie_init()
{
  return 0;
}

int tie_open()
{
  return 0;
}

int tie_put(uint8_t data)
{
  return 0;
}

int tie_get(uint8_t * d)
{
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
  return 0;
}

int tie_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int tie_set_red_wire(int b)
{
  return 0;
}

int tie_set_white_wire(int b)
{
  return 0;
}

int tie_get_red_wire()
{
  return 0;
}

int tie_get_white_wire()
{
  return 0;
}

int tie_supported()
{
  return SUPPORT_OFF;
}

#endif
