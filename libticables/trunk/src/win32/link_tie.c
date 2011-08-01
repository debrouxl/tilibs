/* Hey EMACS -*- win32-c -*- */
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

/* "TiEmulator" virtual link cable unit */

/* 
 *  This unit uses two circular buffer implemented as shared memory.
 *  Names of shm are exchanged thru two FIFOs between the program which use this library.
 *
 *  Convention used: 0 is an emulator and 1 is a linking program.
 *  One pipe is used for transferring information from 0 to 1 and the other
 *  pipe is used for transferring from 1 to 0.
 */

#include <stdio.h>
#include <windows.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

#define BUFSIZE 256

static const char cnt_name[] = "TiEmu Virtual Link (2)";

static const char name[4][256] = 
{
	"GtkTiEmu Virtual Link 0", "GtkTiEmu Virtual Link 1",
	"GtkTiEmu Virtual Link 1", "GtkTiEmu Virtual Link 0"
};

typedef struct 
{
	uint8_t buf[BUFSIZE];
	int		start;
	int		end;
} LinkBuffer;

static HANDLE hRefCnt  = NULL;
static HANDLE hSendBuf = NULL;
static HANDLE hRecvBuf = NULL;

static int		  *pRefCnt  = NULL;
static LinkBuffer *pSendBuf = NULL;
static LinkBuffer *pRecvBuf = NULL;

static int shm_check(void)
{
	int ret;

	hRefCnt = CreateFileMapping((HANDLE) (-1), NULL, PAGE_READWRITE, 0, sizeof(int), (LPCTSTR) cnt_name);
	if (hRefCnt == NULL) 
		return ERR_TIE_OPENFILEMAPPING;
	ret = GetLastError() == ERROR_ALREADY_EXISTS ? 1 : 0;
	if(GetLastError() != ERROR_ALREADY_EXISTS)
		CloseHandle(hRefCnt);

	return ret ? 1 : 0;
}

static int tie_prepare(CableHandle *h)
{
	// in fact, address & device are unused
	switch(h->port)
	{
	case PORT_0:	// automatic setting
		h->address = shm_check();
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

static int tie_open(CableHandle *h)
{
	int p = h->address;
	int ret;

	/* Create shared counter */
	hRefCnt = CreateFileMapping((HANDLE) (-1), NULL, PAGE_READWRITE, 0, sizeof(int), (LPCTSTR) cnt_name);
	if (hRefCnt == NULL)
		return ERR_TIE_OPENFILEMAPPING;
	ret = GetLastError() == ERROR_ALREADY_EXISTS;

	/* Create a FileMapping objects */
	hSendBuf = CreateFileMapping((HANDLE) (-1), NULL, PAGE_READWRITE, 0, sizeof(LinkBuffer), (LPCTSTR) name[2 * p + 0]);
	if (hSendBuf == NULL)
		return ERR_TIE_OPENFILEMAPPING;

	hRecvBuf = CreateFileMapping((HANDLE) (-1), NULL, PAGE_READWRITE, 0, sizeof(LinkBuffer), (LPCTSTR) name[2 * p + 1]);
	if (hRecvBuf == NULL)
		return ERR_TIE_OPENFILEMAPPING;

	/* Map them */
	pSendBuf = (LinkBuffer *) MapViewOfFile(hSendBuf, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));
	if (pSendBuf == NULL)
		return ERR_TIE_MAPVIEWOFFILE;

	pRecvBuf = (LinkBuffer *) MapViewOfFile(hRecvBuf, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));
	if (pRecvBuf == NULL)
		return ERR_TIE_MAPVIEWOFFILE;

	/* Reset buffers */
	pSendBuf->start = pSendBuf->end = 0;
	pRecvBuf->start = pRecvBuf->end = 0;

	/* Increase ref counter */
	pRefCnt = (int *)MapViewOfFile(hRefCnt, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
	*pRefCnt = ret ? 2 : 1;

	//printf("pRefCnt = %i\n", *pRefCnt);

	return 0;
}

static int tie_close(CableHandle *h)
{
	/* Close mappings */
	if (hSendBuf)
	{
		UnmapViewOfFile(pSendBuf);
		pSendBuf = NULL;
		CloseHandle(hSendBuf);
	}

	if (hRecvBuf)
	{
		UnmapViewOfFile(pRecvBuf);
		pRecvBuf = NULL;
		CloseHandle(hRecvBuf);
	}

	/* Decrease ref counter */
	if (pRefCnt)
	{
		(*pRefCnt)--;
		if(*pRefCnt == 0)
		{
			UnmapViewOfFile(pRecvBuf);
			CloseHandle(hRefCnt);
		}
	}
	else
	{
		ticables_critical("tie_close(): reference counter busted !\n");
	}

	return 0;
}

static int tie_reset(CableHandle *h)
{
	if (pRefCnt)
	{
		if (*pRefCnt < 2)
			return 0;

		if(!hSendBuf) return 0;
		if(!hRecvBuf) return 0;

		if (pSendBuf)
			pSendBuf->start = pSendBuf->end = 0;
		else
			ticables_critical("tie_reset(): send buffer busted !\n");
		if (pRecvBuf)
			pRecvBuf->start = pRecvBuf->end = 0;
		else
			ticables_critical("tie_reset(): receive buffer busted !\n");
	}
	else
	{
		ticables_critical("tie_reset(): reference counter busted !\n");
	}

	return 0;
}

static int tie_probe(CableHandle *h)
{
	return shm_check() ? 0 : ERR_PROBE_FAILED;
}

static int tie_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	unsigned int i;
	tiTIME clk;

	if (pRefCnt)
	{
		if(*pRefCnt < 2)
			return 0;

		if(!hSendBuf) return 0;
		if (pSendBuf)
		{
			for(i = 0; i < len; i++)
			{
				TO_START(clk);
				do
				{
					if (TO_ELAPSED(clk, h->timeout))
						return ERR_WRITE_TIMEOUT;
				}
				while (((pSendBuf->end + 1) & (BUFSIZE-1)) == pSendBuf->start);

				pSendBuf->buf[pSendBuf->end] = data[i];
				pSendBuf->end = (pSendBuf->end + 1) & (BUFSIZE-1);
			}
		}
		else
		{
			ticables_critical("tie_put(): send buffer busted !\n");
		}
	}
	else
	{
		ticables_critical("tie_put(): reference counter busted !\n");
	}

	return 0;
}

static int tie_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	unsigned int i;
	tiTIME clk;

	if (pRefCnt)
	{
		if(*pRefCnt < 2)
			return 0;

		if(!hRecvBuf) return 0;
		if (pRecvBuf)
		{
			for(i = 0; i < len; i++)
			{
				TO_START(clk);
				do
				{
					if (TO_ELAPSED(clk, h->timeout))
						return ERR_READ_TIMEOUT;
				}
				while (pRecvBuf->start == pRecvBuf->end);

				data[i] = pRecvBuf->buf[pRecvBuf->start];
				pRecvBuf->start = (pRecvBuf->start + 1) & (BUFSIZE-1);
			}
		}
		else
		{
			ticables_critical("tie_get(): receive buffer busted !\n");
		}
	}
	else
	{
		ticables_critical("tie_get(): reference counter busted !\n");
	}

	return 0;
}

static int tie_check(CableHandle *h, int *status)
{
	if (pRefCnt)
	{
		if(*pRefCnt < 2)
			return 0;

		if(!hRecvBuf) return 0;
		if (pRecvBuf)
		{
			*status = !(pRecvBuf->start == pRecvBuf->end);
		}
		else
		{
			ticables_critical("tie_check(): receive buffer busted !\n");
		}
	}
	else
	{
		ticables_critical("tie_check(): reference counter busted !\n");
	}

	return 0;
}

static int tie_set_red_wire(CableHandle *h, int b)
{
	return 0;
}

static int tie_set_white_wire(CableHandle *h, int b)
{
	return 0;
}

static int tie_get_red_wire(CableHandle *h)
{
	return 1;
}

static int tie_get_white_wire(CableHandle *h)
{
	return 1;
}

const CableFncts cable_tie = 
{
	CABLE_TIE,
	"TIE",
	N_("TiEmu"),
	N_("Virtual link for TiEmu"),
	0,
	&tie_prepare,
	&tie_open, &tie_close, &tie_reset, &tie_probe, NULL,
	&tie_put, &tie_get, &tie_check,
	&tie_set_red_wire, &tie_set_white_wire,
	&tie_get_red_wire, &tie_get_white_wire,
};
