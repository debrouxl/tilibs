/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* "VTi" virtual link cable unit */

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
#include "../internal.h"
#include "detect.h"

#define BUF_SIZE 1*1024

/* Circular buffer (0: TIEmu, 1: TiLP) */
typedef struct
{
	uint8_t buf[BUF_SIZE];
	int start;
	int end;
} LinkBuffer;

/* Shm variables */
static key_t ipc_key[2];	// IPC key
static int shmid[2];		// Shm ID
static LinkBuffer *shm[2];	// Shm address

static LinkBuffer* send_buf[2];	// Swapped buffer
static LinkBuffer* recv_buf[2];

static int vti_prepare(CableHandle *h)
{
	// in fact, address & device are unused
	switch(h->port)
	{
	case PORT_0:	// automatic setting
		h->address = 0; //ref_cnt;
		break;
	case PORT_1:	// forced setting, for compatibility
	case PORT_3:
		h->address = 0; h->device = strdup("0->1");
		break;
	case PORT_2:
	case PORT_4:
		h->address = 1; h->device = strdup("1->0");
		break;
	default: return ERR_ILLEGAL_ARG;
	}

	return 0;
}

static int vti_open(CableHandle *h)
{
	int i;

	/* Get a unique (if possible) key */
	for (i = 0; i < 2; i++)
	{
		if ((ipc_key[i] = ftok("/tmp", i)) == -1)
		{
			ticables_warning("unable to get unique key (ftok).\n");
			return ERR_VTI_IPCKEY;
		}
	}

	/* Open a shared memory segment */
	for (i = 0; i < 2; i++)
	{
		if ((shmid[i] = shmget(ipc_key[i], sizeof(LinkBuffer), IPC_CREAT | 0666)) == -1)
		{
			ticables_warning("unable to open shared memory (shmget).\n");
			return ERR_VTI_SHMGET;
		}
	}

	/* Attach the shm */
	for (i = 0; i < 2; i++)
	{
		if ((shm[i] = shmat(shmid[i], NULL, 0)) == NULL)
		{
			ticables_warning("unable to attach shared memory (shmat).\n");
			return ERR_VTI_SHMAT;
		}
	}

	/* Swap shm */
	send_buf[0] = shm[0];		// 0 -> 1: writing
	recv_buf[0] = shm[1];		// 0 <- 1: reading
	send_buf[1] = shm[1];		// 1 -> 0: writing
	recv_buf[1] = shm[0];		// 1 <- 0: reading

	for (i = 0; i < 2; i++)
	{
		shm[i]->start = shm[i]->end = 0;
	}

	return 0;
}

static int vti_close(CableHandle *h)
{
	int i;

	/* Detach segment */
	for (i = 0; i < 2; i++)
	{
		if (shmdt(shm[i]) == -1)
		{
			ticables_warning("shmdt\n");
			return ERR_VTI_SHMDT;
		}

		/* and destroy it */
		if (shmctl(shmid[i], IPC_RMID, NULL) == -1)
		{
			ticables_warning("shmctl\n");
			return ERR_VTI_SHMCTL;
		}
	}

	return 0;
}

static int vti_reset(CableHandle *h)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		shm[i]->start = shm[i]->end = 0;
	}

	return 0;
}

static int vti_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	int p = h->address;
	uint32_t i;
	tiTIME clk;

	for(i = 0; i < len; i++)
	{
		TO_START(clk);
		do
		{
			if (TO_ELAPSED(clk, h->timeout))
			{
				return ERR_WRITE_TIMEOUT;
			}
		}
		while (((send_buf[p]->end + 1) & 255) == send_buf[p]->start);

		send_buf[p]->buf[send_buf[p]->end] = data[i];
		send_buf[p]->end = (send_buf[p]->end + 1) & 255;
	}

	return 0;
}

static int vti_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	int p = h->address;
	uint32_t i;
	tiTIME clk;

	for(i = 0; i < len; i++)
	{
		TO_START(clk);
		do
		{
			if (TO_ELAPSED(clk, h->timeout))
			{
				return ERR_READ_TIMEOUT;
			}
		}
		while (recv_buf[p]->start == recv_buf[p]->end);

		data[i] = recv_buf[p]->buf[recv_buf[p]->start];
		recv_buf[p]->start = (recv_buf[p]->start + 1) & 255;
	}

	return 0;
}

static int vti_probe(CableHandle *h)
{
	return 0;
}

static int vti_check(CableHandle *h, int *status)
{
	int p = h->address;
	*status = !(recv_buf[p]->start == recv_buf[p]->end);
	return 0;
}

const CableFncts cable_vti = 
{
	CABLE_VTI,
	"VTI",
	N_("Virtual TI"),
	N_("Virtual link for VTi"),
	0,
	&vti_prepare,
	&vti_open, &vti_close, &vti_reset, &vti_probe, NULL,
	&vti_put, &vti_get, &vti_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&noop_set_device,
	NULL
};
