/* Hey EMACS -*- linux-c -*- */
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

/* 
   Thanks to Mikael Magnusson for its simple program that I have
   used as an example for writing this unit.
   Mikael is also the developper & maintainer of the TI83+ support
   for the libTIcalcs library.
   <mikma@users.sourceforge.net> (or <mikma@hem.passagen.se>)
*/

/* "VTi" virtual link cable unit */
/* 
   Exists only on Windows platform
   I have added a Linux part with the other implementation of
   the GtkTiEmu virtual link since they are very similar (shm).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(__LINUX__)

/**************/
/* Linux part */
/**************/

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

#include "intl.h"
#include "timeout.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "externs.h"
#include "verbose.h"
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
vti_buf *send_buf[2], *recv_buf[2];	// Swapped buffer
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
    DISPLAY_ERROR("Invalid io_address parameter passed to libticables.\n");
    return ERR_ILLEGAL_ARG;
    io_address = 2;
  }
  p = io_address - 1;

  /* Get a unique (if possible) key */
  for (i = 0; i < 2; i++) {
    if ((ipc_key[i] = ftok("/tmp", i)) == -1) {
      DISPLAY_ERROR("ftok\n");
      return ERR_IPC_KEY;
    }
    //DISPLAY("ipc_key[%i] = 0x%08x\n", i, ipc_key[i]);
  }

  /* Open a shared memory segment */
  for (i = 0; i < 2; i++) {
    if ((shmid[i] = shmget(ipc_key[i], sizeof(vti_buf),
			   IPC_CREAT | 0666)) == -1) {
      DISPLAY_ERROR("ftok\n");
      return ERR_SHM_GET;
    }
    //DISPLAY("shmid[%i] = %i\n", i, shmid[i]);
  }

  /* Attach the shm */
  for (i = 0; i < 2; i++) {
    if ((shm[i] = shmat(shmid[i], NULL, 0)) == NULL) {
      DISPLAY_ERROR("shmat\n");
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

int vti_probe()
{
  return 0;
}

int vti_close()
{
  return 0;
}

int vti_exit()
{
#ifdef USE_SHM
  int i;

  STOP_LOGGING();
  //DISPLAY("exit\n");
  /* Detach segment */
  for (i = 0; i < 2; i++) {
    if (shmdt(shm[i]) == -1) {
      DISPLAY_ERROR("shmdt\n");
      return ERR_SHM_DETACH;
    }
    /* and destroy it */
    if (shmctl(shmid[i], IPC_RMID, NULL) == -1) {
      DISPLAY_ERROR("shmctl\n");
      return ERR_SHM_RMID;
    }
  }
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

int vti_supported()
{
#ifdef USE_SHM
  return SUPPORT_ON;
#else
  return SUPPORT_OFF;
#endif
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
#include "export.h"
#include "logging.h"
#include "externs.h"
#include "verbose.h"

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
  //DISPLAY("Virtual Link L->V %i\n", i);
  vSendBuf = (LinkBuffer *) MapViewOfFile(hVLinkFileMap,
					  FILE_MAP_ALL_ACCESS, 0, 0,
					  sizeof(LinkBuffer));

  /* Get an handle on the VTi window */
  otherWnd = FindWindow("TEmuWnd", NULL);
  //DISPLAY("otherWnd: %p\n", otherWnd);
  if (!otherWnd)
    return ERR_OPP_NOT_AVAIL;

  /* Get the current DLL handle */
  Handle = GetModuleHandle("ticables.dll");
  if (!Handle) {
    DISPLAY_ERROR(_("Unable to get an handle on the libTIcables.\n"));
    DISPLAY_ERROR(_("Did you rename the library ?!\n"));
    DISPLAY_ERROR(_("Fatal error. Program terminated.\n"));
    exit(-1);
  }
  // Inform VTi of our virtual link so that it can enable it. It should returns 
  // its virtual link name in a message.
  SendMessage(otherWnd, WM_HELLO, 0, (LPARAM) Handle);

  /* Retrieve the VTi virtual link name */
  //b = GetMessage(&msg, NULL, WM_HELLO, WM_SEND_BUFFER);
  //WaitMessage();                                                                                // Waits VTi answer

  /* Create a file mapping handle for the 'Vti->lib' communication channel */
  //DISPLAY("Virtual Link V->L %i\n", i-1);
  sprintf(name, "Virtual Link %d", i - 1);
  hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
  if (hMap) {
    DISPLAY_ERROR(_("Opened %s\n"), name);
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

int vti_open()
{
  //vSendBuf->start = vSendBuf->end = 0;
  //vRecvBuf->start = vRecvBuf->end = 0;

  tdr.count = 0;
  toSTART(tdr.start);

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

int vti_close()
{
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

int vti_probe()
{
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

int vti_supported()
{
  return SUPPORT_ON;
}

#else

/************************/
/* Unsupported platform */
/************************/

#include "cabl_def.h"

int vti_init()
{
  return 0;
}

int vti_open()
{
  return 0;
}

int vti_put(uint8_t data)
{
  return 0;
}

int vti_get(uint8_t * d)
{
  return 0;
}

int vti_probe()
{
  return 0;
}

int vti_close()
{
  return 0;
}

int vti_exit()
{
  return 0;
}

int vti_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

int vti_set_red_wire(int b)
{
  return 0;
}

int vti_set_white_wire(int b)
{
  return 0;
}

int vti_get_red_wire()
{
  return 0;
}

int vti_get_white_wire()
{
  return 0;
}

int vti_supported()
{
  return SUPPORT_OFF;
}

#endif
