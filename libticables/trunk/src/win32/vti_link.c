/* Hey EMACS -*- win32-c -*- */
/* $Id: vti_link.c 370 2004-03-22 18:47:32Z roms $ */

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

#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "intl1.h"
#include "timeout.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "logging.h"
#include "externs.h"
#include "printl.h"

extern int time_out;		// Timeout value for cables in 0.10 seconds
extern int delay;		// Time between 2 bits (home-made cables only)

#define WM_HELLO WM_USER+101
#define WM_GOODBYE WM_USER+102
#define WM_ENABLE_LINK WM_USER+110
#define WM_DISABLE_LINK WM_USER+111
#define WM_SEND_BUFFER WM_USER+120
#define WM_ENTER_DEBUG WM_USER+130
#define WM_EXIT_DEBUG WM_USER+131

/* VTi's LinkBuffer structure */
typedef struct {
  BYTE buf[256];
  int start;
  int end;
} LinkBuffer;

static LinkBuffer *vSendBuf, *vRecvBuf;	//
static HANDLE hMap = NULL;	// Handle on
static HWND otherWnd = NULL;	// Handle on the VTi window

int vti_init(unsigned int io_addr, char *dev)
{
  char name[32];
  char vLinkFileName[32];
  HANDLE hVLinkFileMap = NULL;	// Handle on the 
  HANDLE Handle;
  int i;
  ATOM a;

  /* Create a file mapping handle for the 'lib->VTi' communication channel */
  for (i = 0;; i++) {
    sprintf(vLinkFileName, "Virtual Link %d", i);
    hVLinkFileMap = CreateFileMapping((HANDLE) - 1, NULL,
				      PAGE_READWRITE, 0,
				      sizeof(LinkBuffer), vLinkFileName);
    if (GetLastError() != ERROR_ALREADY_EXISTS)
      break;
  }
  //printl1(0, "Virtual Link L->V %i\n", i);
  vSendBuf = (LinkBuffer *) MapViewOfFile(hVLinkFileMap,
					  FILE_MAP_ALL_ACCESS, 0, 0,
					  sizeof(LinkBuffer));

  /* Get an handle on the VTi window */
  otherWnd = FindWindow("TEmuWnd", NULL);
  //printl1(0, "otherWnd: %p\n", otherWnd);
  if (!otherWnd)
    return ERR_OPP_NOT_AVAIL;

  /* Get the current DLL handle */
  Handle = GetModuleHandle("ticables.dll");
  if (!Handle) {
    printl1(2, _("Unable to get an handle on the libTIcables.\n"));
    printl1(2, _("Did you rename the library ?!\n"));
    printl1(2, _("Fatal error. Program terminated.\n"));
    exit(-1);
  }
  // Inform VTi of our virtual link so that it can enable it. It should returns 
  // its virtual link name in a message.
  SendMessage(otherWnd, WM_HELLO, 0, (LPARAM) Handle);

  /* Retrieve the VTi virtual link name */
  //b = GetMessage(&msg, NULL, WM_HELLO, WM_SEND_BUFFER);
  //WaitMessage();                                                                                // Waits VTi answer

  /* Create a file mapping handle for the 'Vti->lib' communication channel */
  //printl1(0, "Virtual Link V->L %i\n", i-1);
  sprintf(name, "Virtual Link %d", i - 1);
  hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
  if (hMap) {
    printl1(2, _("Opened %s\n"), name);
    vRecvBuf =
	(LinkBuffer *) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS,
				     0, 0, sizeof(LinkBuffer));
  } else
    return ERR_OPEN_FILE_MAP;

  /* Send to VTi the name of our virtual link. VTi should open it (lib -> Vti) */
  a = GlobalAddAtom(vLinkFileName);
  SendMessage(otherWnd, WM_SEND_BUFFER, 0, (LPARAM) a);
  GlobalDeleteAtom(a);

  /* Enable linking (check the VTi's Virtual Link|Enable cable link' item) */
  if (otherWnd)
    SendMessage(otherWnd, WM_ENABLE_LINK, 0, 0);

  START_LOGGING();

  return 0;
}

int vti_exit()
{
  /* Send an atom */
  STOP_LOGGING();
  if (otherWnd) {
    SendMessage(otherWnd, WM_DISABLE_LINK, 0, 0);
    SendMessage(otherWnd, WM_GOODBYE, 0, 0);
  }

  /* Close the shared buffer */
  if (hMap) {
    UnmapViewOfFile(vSendBuf);
    UnmapViewOfFile(vRecvBuf);
    CloseHandle(hMap);
  }

  return 0;
}

int vti_open()
{
  //vSendBuf->start = vSendBuf->end = 0;
  //vRecvBuf->start = vRecvBuf->end = 0;

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
  tiTIME clk;

  if (!hMap)
    return ERR_OPEN_FILE_MAP;

  tdr.count++;
  LOG_DATA(data);

  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_WRITE_TIMEOUT;
  }
  while (((vSendBuf->end + 1) & 255) == vSendBuf->start);

  vSendBuf->buf[vSendBuf->end] = data;	// put data in buffer
  vSendBuf->end = (vSendBuf->end + 1) & 255;	// update circular buffer

  return 0;
}

int vti_get(uint8_t * data)
{
  tiTIME clk;

  if (!hMap)
    return ERR_OPEN_FILE_MAP;

  /* Wait that the buffer has been filled */
  toSTART(clk);
  do {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
  }
  while (vRecvBuf->start == vRecvBuf->end);

  /* And retrieve the data from the circular buffer */
  *data = vRecvBuf->buf[vRecvBuf->start];
  vRecvBuf->start = (vRecvBuf->start + 1) & 255;

  tdr.count++;
  LOG_DATA(*data);

  return 0;
}

int vti_check(int *status)
{
  if (!hMap)
    return ERR_OPEN_FILE_MAP;

  /* Check if positions are the same */
  *status = !(vRecvBuf->start == vRecvBuf->end);

  return 0;
}

int vti_probe()
{
  return 0;
}

int vti_supported()
{
  return SUPPORT_ON;
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