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

/* "VTi" virtual link cable unit */

/* 
   Thanks to Mikael Magnusson for its simple program that I have
   used as an example for writing this unit.
   Mikael is also the developper & maintainer of the TI83+ support
   for the libTIcalcs library.
   <mikma@users.sourceforge.net> (or <mikma@hem.passagen.se>)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* 
   This part use Shared Memory Segment for implementing two circular buffers
   between 2 program which use this lib. Each segment is attached 2 times.
   Convention used: 0 is an emulator and 1 is a linking program.
   One shm is used for transferring information from 0 to 1 and the other
   shm is used for transferring from 1 to 0.
   shm0: W -> R
   shm1: R <- W
*/

#if defined(HAVE_SYS_IPC_H) && defined(HAVE_SYS_SHM_H)
# define USE_SHM
#endif

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#ifdef USE_SHM
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "intl1.h"
#include "timeout.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "printl.h"
#include "logging.h"

/* Circular buffer (O: TIEmu, 1: TiLP) */
#define BUF_SIZE 1*1024
struct _vti_buf {
  uint8_t buf[BUF_SIZE];
  int start;
  int end;
};
typedef struct _vti_buf vti_buf;

/* Shm variables */
#ifdef USE_SHM
key_t ipc_key[2];		// IPC key
int shmid[2];			// Shm ID
vti_buf *shm[2];		// Shm address
vti_buf *send_buf[2];		// Swapped buffer
vti_buf *recv_buf[2];
#endif

static int p = 0;		// a shortcut

/*
  The first call to init open the 2 shm
*/
int vti_init()
{
#ifdef USE_SHM
  int i;

  if ((io_address < 1) || (io_address > 2)) {
    printl1(2, "invalid io_address (bad port).\n");
    return ERR_ILLEGAL_ARG;
    io_address = 2;
  }
  p = io_address - 1;

  /* Get a unique (if possible) key */
  for (i = 0; i < 2; i++) {
    if ((ipc_key[i] = ftok("/tmp", i)) == -1) {
      printl1(2, "unable to get unique key (ftok).\n");
      return ERR_IPC_KEY;
    }
    //printl1(0, "ipc_key[%i] = 0x%08x\n", i, ipc_key[i]);
  }

  /* Open a shared memory segment */
  for (i = 0; i < 2; i++) {
    if ((shmid[i] = shmget(ipc_key[i], sizeof(vti_buf),
			   IPC_CREAT | 0666)) == -1) {
      printl1(2, "unable to open shared memory (shmget).\n");
      return ERR_SHM_GET;
    }
    //printl1(0, "shmid[%i] = %i\n", i, shmid[i]);
  }

  /* Attach the shm */
  for (i = 0; i < 2; i++) {
    if ((shm[i] = shmat(shmid[i], NULL, 0)) == NULL) {
      printl1(2, "unable to attach shared memory (shmat).\n");
      return ERR_SHM_ATTACH;
    }
  }

  /* Swap shm */
  send_buf[0] = shm[0];		// 0 -> 1: writing
  recv_buf[0] = shm[1];		// 0 <- 1: reading
  send_buf[1] = shm[1];		// 1 -> 0: writing
  recv_buf[1] = shm[0];		// 1 <- 0: reading
#endif

  START_LOGGING();

  return 0;
}

int vti_exit()
{
#ifdef USE_SHM
  int i;

  STOP_LOGGING();
  //printl1(0, "exit\n");
  /* Detach segment */
  for (i = 0; i < 2; i++) {
    if (shmdt(shm[i]) == -1) {
      printl1(2, "shmdt\n");
      return ERR_SHM_DETACH;
    }
    /* and destroy it */
    if (shmctl(shmid[i], IPC_RMID, NULL) == -1) {
      printl1(2, "shmctl\n");
      return ERR_SHM_RMID;
    }
  }
#endif
  return 0;
}

int vti_open()
{
#ifdef USE_SHM
  int i;

  /* Init buffers */
  for (i = 0; i < 2; i++) {
    shm[i]->start = shm[i]->end = 0;
  }
#endif

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int vti_close()
{
  return 0;
}

int vti_put(uint8_t data)
{
#ifdef USE_SHM
  tiTIME clk;

  tdr.count++;
  LOG_DATA(data);
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_WRITE_TIMEOUT;
  }
  while (((send_buf[p]->end + 1) & 255) == send_buf[p]->start);

  send_buf[p]->buf[send_buf[p]->end] = data;	// put data in buffer
  send_buf[p]->end = (send_buf[p]->end + 1) & 255;	// update circular buffer
#endif
  return 0;
}

int vti_get(uint8_t * data)
{
#ifdef USE_SHM
  tiTIME clk;

  tdr.count++;
  /* Wait that the buffer has been filled */
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
  }
  while (recv_buf[p]->start == recv_buf[p]->end);

  /* And retrieve the data from the circular buffer */
  *data = recv_buf[p]->buf[recv_buf[p]->start];
  recv_buf[p]->start = (recv_buf[p]->start + 1) & 255;
  LOG_DATA(*data);
#endif
  return 0;
}

int vti_check(int *status)
{
#ifdef USE_SHM
  *status = STATUS_NONE;

  /* Check if positions are the same */
  if (recv_buf[p]->start == recv_buf[p]->end) {
    *status = STATUS_NONE;
    return 0;
  } else {
    *status = STATUS_RX;
    return 0;
  }
#endif
  return 0;
}

int vti_probe()
{
#ifdef USE_SHM
	return !0;
#else
  	return 0;
#endif
}

int vti_supported()
{
#ifdef USE_SHM
  return SUPPORT_ON;
#else
  return SUPPORT_OFF;
#endif
}

int vti_register_cable(TicableLinkCable * lc)
{
  lc->init = vti_init;
  lc->open = vti_open;
  lc->put = vti_put;
  lc->get = vti_get;
  lc->close = vti_close;
  lc->exit = vti_exit;
  lc->probe = vti_probe;
  lc->check = vti_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
