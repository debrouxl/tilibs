/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

/* Circular buffer (O: TIEmu, 1: TiLP) */
#define BUF_SIZE 1*1024
struct _vti_buf {
  uint8_t buf[BUF_SIZE];
  int start;
  int end;
};
typedef struct _vti_buf vti_buf;

/* Shm variables */
key_t ipc_key[2];		// IPC key
int shmid[2];			// Shm ID
vti_buf *shm[2];		// Shm address
vti_buf *send_buf[2];		// Swapped buffer
vti_buf *recv_buf[2];

static int p = 0;		// a shortcut

static int vti_prepare(CableHandle *h)
{
	h->address = 0;
	h->device = strdup("");

	return 0;
}

static int vti_open(CableHandle *h)
{
  int i;

  if ((h->address < 1) || (h->address > 2)) {
    ticables_warning("invalid h->address (bad port).\n");
    return ERR_ILLEGAL_ARG;
    h->address = 2;
  }
  p = h->address - 1;

  /* Get a unique (if possible) key */
  for (i = 0; i < 2; i++) {
    if ((ipc_key[i] = ftok("/tmp", i)) == -1) {
      ticables_warning("unable to get unique key (ftok).\n");
      return ERR_VTI_IPCKEY;
    }
    //printl1(0, "ipc_key[%i] = 0x%08x\n", i, ipc_key[i]);
  }

  /* Open a shared memory segment */
  for (i = 0; i < 2; i++) {
    if ((shmid[i] = shmget(ipc_key[i], sizeof(vti_buf),
			   IPC_CREAT | 0666)) == -1) {
      ticables_warning("unable to open shared memory (shmget).\n");
      return ERR_VTI_SHMGET;
    }
    //printl1(0, "shmid[%i] = %i\n", i, shmid[i]);
  }

  /* Attach the shm */
  for (i = 0; i < 2; i++) {
    if ((shm[i] = shmat(shmid[i], NULL, 0)) == NULL) {
      ticables_warning("unable to attach shared memory (shmat).\n");
      return ERR_VTI_SHMAT;
    }
  }

  /* Swap shm */
  send_buf[0] = shm[0];		// 0 -> 1: writing
  recv_buf[0] = shm[1];		// 0 <- 1: reading
  send_buf[1] = shm[1];		// 1 -> 0: writing
  recv_buf[1] = shm[0];		// 1 <- 0: reading

  return 0;
}

static int vti_close(CableHandle *h)
{
  int i;

  //printl1(0, "exit\n");
  /* Detach segment */
  for (i = 0; i < 2; i++) {
    if (shmdt(shm[i]) == -1) {
      ticables_warning("shmdt\n");
      return ERR_VTI_SHMDT;
    }
    /* and destroy it */
    if (shmctl(shmid[i], IPC_RMID, NULL) == -1) {
      ticables_warning("shmctl\n");
      return ERR_VTI_SHMCTL;
    }
  }

  return 0;
}

static int vti_reset(CableHandle *h)
{
  int i;

  /* Init buffers */
  for (i = 0; i < 2; i++) {
    shm[i]->start = shm[i]->end = 0;
  }

  return 0;
}

static int vti_put(CableHandle *h, uint8_t *data, uint16_t len)
{
  tiTIME clk;

  TO_START(clk);
  do {
    if (TO_ELAPSED(clk, h->timeout))
      return ERR_WRITE_TIMEOUT;
  }
  while (((send_buf[p]->end + 1) & 255) == send_buf[p]->start);

  send_buf[p]->buf[send_buf[p]->end] = data;	// put data in buffer
  send_buf[p]->end = (send_buf[p]->end + 1) & 255;	// update circular buffer
  return 0;
}

static int vti_get(CableHandle *h, uint8_t *data, uint16_t len)
{
  tiTIME clk;

  TO_START(clk);
  do {
    if (TO_ELAPSED(clk, h->timeout))
      return ERR_READ_TIMEOUT;
  }
  while (recv_buf[p]->start == recv_buf[p]->end);

  /* And retrieve the data from the circular buffer */
  *data = recv_buf[p]->buf[recv_buf[p]->start];
  recv_buf[p]->start = (recv_buf[p]->start + 1) & 255;

  return 0;
}

static int vti_probe(CableHandle *h)
{
	return 0;
}

static int vti_check(CableHandle *h, int *status)
{
  *status = STATUS_NONE;

  /* Check if positions are the same */
  if (recv_buf[p]->start == recv_buf[p]->end) {
    *status = STATUS_NONE;
    return 0;
  } else {
    *status = STATUS_RX;
    return 0;
  }

  return 0;
}

static int vti_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

static int vti_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

static int vti_get_red_wire(CableHandle *h)
{
	return 1;
}

static int vti_get_white_wire(CableHandle *h)
{
	return 1;
}

const CableFncts cable_vti = 
{
	CABLE_VTI,
	"VTI",
	N_("Virtual TI"),
	N_("Virtual link for VTi"),
	0,
	&vti_prepare,
	&vti_open, &vti_close, &vti_reset, &vti_probe,
	&vti_put, &vti_get, &vti_check,
	&vti_set_red_wire, &vti_set_white_wire,
	&vti_get_red_wire, &vti_get_white_wire,
};
