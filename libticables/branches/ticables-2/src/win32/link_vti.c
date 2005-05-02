/* Hey EMACS -*- win32-c -*- */
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

/* "VTi" virtual link cable unit */

/* 
   Thanks to Mikael Magnusson for its simple program that I have
   used as an example for writing this unit.
   Mikael is also the developper & maintainer of the TI83+ support
   for the libTIcalcs library.
   <mikma@users.sourceforge.net> (or <mikma@hem.passagen.se>)
*/

#include <stdio.h>

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
	BYTE	buf[BUFSIZE];
	int	start;
	int	end;
} LinkBuffer;

static LinkBuffer*	vSendBuf;
static LinkBuffer*	vRecvBuf;
static HANDLE		hMap = NULL;		// Handle on file-mapping object
static HWND			otherWnd = NULL;	// Handle on the VTi window

static int vti_prepare(TiHandle *h)
{
	switch(h->port)
	{
	case PORT_1: h->address = 0; h->device = strdup("VTi"); break;
	case PORT_2: h->address = 1; h->device = strdup("libticables"); break;
	default: return ERR_ILLEGAL_ARG;
	}

	return 0;
}

static int vti_open(TiHandle *h)
{
	int i;
	char vLinkFileName[32];
	char name[32];
	HANDLE hVLinkFileMap = NULL;	// Handle on the 
	HANDLE Handle;	
	ATOM a;

    /* Create a file mapping handle for the 'lib->VTi' communication channel */
    for (i = 0;; i++) 
    {
		sprintf(vLinkFileName, "Virtual Link %d", i);
		hVLinkFileMap = CreateFileMapping((HANDLE) - 1, NULL,
				      PAGE_READWRITE, 0,
				      sizeof(LinkBuffer), vLinkFileName);
  
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			break;
    }
    
    ticables_info("Virtual Link L->V %i\n", i);
	vSendBuf = (LinkBuffer *)MapViewOfFile(hVLinkFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));
  
    /* Get an handle on the VTi window */
    otherWnd = FindWindow("TEmuWnd", NULL);
    if (!otherWnd)
		return ERR_FINDWINDOW;
  
    /* Get the current DLL handle */
    Handle = GetModuleHandle("ticables-2.dll");
	if(!Handle)
	  Handle = GetModuleHandle("libticables-3.dll");
  
    if (!Handle) 
    {
      ticables_warning(_("Unable to get an handle on the libTIcables.\n"));
      ticables_warning(_("Did you rename the library ?!\n"));
      ticables_warning(_("Fatal error. Program terminated.\n"));
      exit(-1);
	}
  
    // Inform VTi of our virtual link so that it can enable it. It should returns 
    // its virtual link name in a message.
	SendMessage(otherWnd, WM_HELLO, 0, (LPARAM) Handle);
  
    /* Retrieve the VTi virtual link name */
    //b = GetMessage(&msg, NULL, WM_HELLO, WM_SEND_BUFFER);
	//WaitMessage();                                                                                // Waits VTi answer
  
    /* Create a file mapping handle for the 'Vti->lib' communication channel */
    ticables_info("Virtual Link V->L %i\n", i-1);
    sprintf(name, "Virtual Link %d", i - 1);
    hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
    if (hMap) 
    {
      ticables_info(_("Opened %s\n"), name);
      vRecvBuf = (LinkBuffer *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));
    } 
    else
		return ERR_OPENFILEMAPPING;
  
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

static int vti_close(TiHandle *h)
{
	if (otherWnd) 
  {
    SendMessage(otherWnd, WM_DISABLE_LINK, 0, 0);
    SendMessage(otherWnd, WM_GOODBYE, 0, 0);
  }

  /* Close the shared buffer */
  if (hMap) 
  {
    UnmapViewOfFile(vSendBuf);
    UnmapViewOfFile(vRecvBuf);
    CloseHandle(hMap);
  }

	return 0;
}

static int vti_reset(TiHandle *h)
{
	vSendBuf->start = vSendBuf->end = 0;
	vRecvBuf->start = vRecvBuf->end = 0;

	return 0;
}

static int vti_probe(TiHandle *h)
{
	return 1;
}

static int vti_put(TiHandle *h, uint8_t *data, uint16_t len)
{
	int i;
	tiTIME clk;

	for(i = 0; i < len; i++)
	{
		TO_START(clk);
		do 
		{
			if (TO_ELAPSED(clk, h->timeout))
				return ERR_WRITE_TIMEOUT;
		}
		while (((vSendBuf->end + 1) & (BUFSIZE-1)) == vSendBuf->start);

		vSendBuf->buf[vSendBuf->end] = data[i];					// put data in buffer
		vSendBuf->end = (vSendBuf->end + 1) & (BUFSIZE-1);	// update circular buffer
	}

	return 0;
}

static int vti_get(TiHandle *h, uint8_t *data, uint16_t len)
{
	int i;
	tiTIME clk;

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

	return 0;
}

static int vti_check(TiHandle *h, int *status)
{
	*status = !(vRecvBuf->start == vRecvBuf->end);

	return 0;
}

static int vti_set_red_wire(TiHandle *h, int b)
{
	return 0;
}

static int vti_set_white_wire(TiHandle *h, int b)
{
	return 0;
}

static int vti_get_red_wire(TiHandle *h)
{
	return 1;
}

static int vti_get_white_wire(TiHandle *h)
{
	return 1;
}

const TiCable cable_vti = 
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
