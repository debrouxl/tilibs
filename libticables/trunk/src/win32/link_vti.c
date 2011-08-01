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

/* "VTi" virtual link cable unit */

/* 
 *  This unit uses two circular buffer implemented as shared memory.
 *  Names of shm are exchanged thru a messaging system.
 */

#include <stdio.h>
#include <windows.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"

// VTi messages
#define WM_HELLO		WM_USER+101
#define WM_GOODBYE		WM_USER+102
#define WM_ENABLE_LINK	WM_USER+110	// used
#define WM_DISABLE_LINK WM_USER+111
#define WM_SEND_BUFFER	WM_USER+120	// used
#define WM_ENTER_DEBUG	WM_USER+130
#define WM_EXIT_DEBUG	WM_USER+131

#define BUFSIZE	256

/* VTi's LinkBuffer structure */
typedef struct 
{
	uint8_t	buf[BUFSIZE];
	int		start;
	int		end;
} LinkBuffer;

static LinkBuffer*	vSendBuf = NULL;
static LinkBuffer*	vRecvBuf = NULL;
static HANDLE		hMap = NULL;		// Handle on file-mapping object
static HWND			otherWnd = NULL;	// Handle on the VTi window

static int vti_prepare(CableHandle *h)
{
	// in fact, address & device are unused
	switch(h->port)
	{
	case PORT_0:	// automatic setting
		h->address = 1; h->device = strdup("TiLP"); 
		break;
	case PORT_1:	// reserved because used by VTi
		h->address = 0; h->device = strdup("VTi"); 
		break;
	case PORT_2:	// 
		h->address = 1; h->device = strdup("TiLP"); 
		break;
	default: return ERR_ILLEGAL_ARG;
	}

	return 0;
}

static int vti_open(CableHandle *h)
{
	int i;
	char vLinkFileName[32];
	char name[32];
	HANDLE hVLinkFileMap = NULL;
	HANDLE Handle;
	ATOM a;

	/* Get an handle on the VTi window */
	otherWnd = FindWindow("TEmuWnd", NULL);
	if (!otherWnd)
		return ERR_VTI_FINDWINDOW;

	/* Get the current DLL handle */
	Handle = GetModuleHandle("libticables2.dll");
	if(!Handle)
		Handle = GetModuleHandle("libticables2-5.dll");

	if (!Handle)
	{
		ticables_critical(_("FATAL ERROR: unable to get an handle on the ticables-2 library."));
		ticables_critical(_("Did you rename the library ?!"));
		return ERR_NO_LIBRARY;
	}

	/* Create a file mapping handle for the 'lib->VTi' communication channel */
	for (i = 0; ; i++)
	{
		sprintf(vLinkFileName, "Virtual Link %d", i);
		hVLinkFileMap = CreateFileMapping((HANDLE) - 1, NULL,
				      PAGE_READWRITE, 0,
				      sizeof(LinkBuffer), vLinkFileName);
  
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			break;
	}

	ticables_info("Virtual Link L->V %i", i);
	vSendBuf = (LinkBuffer *)MapViewOfFile(hVLinkFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));

	// Inform VTI of our virtual link so that it can enable it. It should return
	// its virtual link name in a message.
	SendMessage(otherWnd, WM_HELLO, 0, (LPARAM) Handle);
  
	/* Retrieve the VTi virtual link name */
	//b = GetMessage(&msg, NULL, WM_HELLO, WM_SEND_BUFFER);
	//WaitMessage();                                                                                // Waits VTi answer

	/* Create a file mapping handle for the 'Vti->lib' communication channel */
	ticables_info("Virtual Link V->L %i", i-1);
	sprintf(name, "Virtual Link %d", i - 1);
	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	if (hMap)
	{
		ticables_info(_("Opened %s"), name);
		vRecvBuf = (LinkBuffer *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));
	}
	else
		return ERR_VTI_OPENFILEMAPPING;

	/* Send to VTi the name of our virtual link. VTi should open it (lib -> Vti) */
	a = GlobalAddAtom(vLinkFileName);
	SendMessage(otherWnd, WM_SEND_BUFFER, 0, (LPARAM) a);
	GlobalDeleteAtom(a);

	/* Enable linking (check the VTi's Virtual Link|Enable cable link' item) */
	if (otherWnd)
		SendMessage(otherWnd, WM_ENABLE_LINK, 0, 0);

	vSendBuf->start = vSendBuf->end = 0;
	vRecvBuf->start = vRecvBuf->end = 0;

	return 0;
}

static int vti_close(CableHandle *h)
{
	if (otherWnd) 
	{
		SendMessage(otherWnd, WM_DISABLE_LINK, 0, 0);
		SendMessage(otherWnd, WM_GOODBYE, 0, 0);

		otherWnd = NULL;
	}

	/* Close the shared buffer */
	if (hMap) 
	{
		UnmapViewOfFile(vSendBuf);
		vSendBuf = NULL;

		UnmapViewOfFile(vRecvBuf);
		vRecvBuf = NULL;

		CloseHandle(hMap);
	}

	return 0;
}

static int vti_reset(CableHandle *h)
{
	if(!hMap) return 0;

	if (vSendBuf)
		vSendBuf->start = vSendBuf->end = 0;
	else
		ticables_critical("vti_reset(): send buffer busted !\n");
	if (vRecvBuf)
		vRecvBuf->start = vRecvBuf->end = 0;
	else
		ticables_critical("vti_reset(): receive buffer busted !\n");

	return 0;
}

static int vti_probe(CableHandle *h)
{
	/* Get an handle on the VTi window */
    otherWnd = FindWindow("TEmuWnd", NULL);
    if (!otherWnd)
		return ERR_VTI_FINDWINDOW;
	else
		otherWnd = NULL;

	return 0;
}

static int vti_put(CableHandle *h, uint8_t *data, uint32_t len)
{
	unsigned int i;
	tiTIME clk;

	if(!hMap) return 0;
	if(vSendBuf)
	{
		for(i = 0; i < len; i++)
		{
			TO_START(clk);
			do
			{
				if (TO_ELAPSED(clk, h->timeout))
					return ERR_WRITE_TIMEOUT;
			}
			while (((vSendBuf->end + 1) & (BUFSIZE-1)) == vSendBuf->start);

			vSendBuf->buf[vSendBuf->end] = data[i];
			vSendBuf->end = (vSendBuf->end + 1) & (BUFSIZE-1);
		}
	}
	else
	{
		ticables_critical("vti_put(): send buffer busted !\n");
	}

	return 0;
}

static int vti_get(CableHandle *h, uint8_t *data, uint32_t len)
{
	unsigned int i;
	tiTIME clk;

	if(!hMap) return 0;
	if(vRecvBuf)
	{
		/* Wait that the buffer has been filled */
		for(i = 0; i < len; i++)
		{
			TO_START(clk);
			do
			{
				if (TO_ELAPSED(clk, h->timeout))
					return ERR_READ_TIMEOUT;
			}
			while (vRecvBuf->start == vRecvBuf->end);

			/* And retrieve the data from the circular buffer */
			data[i] = vRecvBuf->buf[vRecvBuf->start];
			vRecvBuf->start = (vRecvBuf->start + 1) & (BUFSIZE-1);
		}
	}
	else
	{
		ticables_critical("vti_get(): receive buffer busted !\n");
	}

	return 0;
}

static int vti_check(CableHandle *h, int *status)
{
	if(!hMap) return 0;
	if(vRecvBuf)
	{
		*status = !(vRecvBuf->start == vRecvBuf->end);
	}
	else
	{
		ticables_critical("vti_check(): receive buffer busted !\n");
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
	&vti_open, &vti_close, &vti_reset, &vti_probe, NULL,
	&vti_put, &vti_get, &vti_check,
	&vti_set_red_wire, &vti_set_white_wire,
	&vti_get_red_wire, &vti_get_white_wire,
};
