/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#if !defined(__WIN32__)

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_SYS_IPC_H) && defined(HAVE_SYS_SHM_H)
# define USE_SHM
#endif

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#ifdef USE_SHM
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include "str.h"

#include "timeout.h"
#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "verbose.h"

/* Circular buffer (O: TIEmu, 1: TiLP) */
#define BUF_SIZE 1*1024 
struct _vti_buf
{
  byte buf[BUF_SIZE];
  int tSTART;
  int end;
};
typedef struct _vti_buf vti_buf;

/* Shm variables */
#ifdef USE_SHM
key_t ipc_key[2];     // IPC key
int shmid[2];         // Shm ID
vti_buf *shm[2];      // Shm address
vti_buf *send_buf[2], *recv_buf[2]; // Swapped buffer
#endif

#ifdef USE_SHM
static int p = 0; // a shortcut
#endif

/*
  The first call to init_port open the 2 shm
*/
DLLEXPORT
int vti_init_port()
{
#ifdef USE_SHM
  int i;

  if( (io_address < 1) || (io_address > 2))
    {
      fprintf(stderr, "Invalid io_address parameter passed to libticables.\n");
      io_address = 2;
    }
  p = io_address - 1;

  /* Get a unique (if possible) key */
  for(i=0; i<2; i++)
    {
      if((ipc_key[i] = ftok("/tmp", i)) == -1)
	{
	  fprintf(stderr, "ftok\n");
	  return ERR_IPC_KEY;
	}
      //DISPLAY("ipc_key[%i] = 0x%08x\n", i, ipc_key[i]);
    }

  /* Open a shared memory segment */
  for(i=0; i<2; i++)
    {
      if((shmid[i] = shmget(ipc_key[i], sizeof(vti_buf), 
			    IPC_CREAT | 0666)) == -1)
	{
	  fprintf(stderr, "ftok\n");    
	  return ERR_SHM_GET;
	}
      //DISPLAY("shmid[%i] = %i\n", i, shmid[i]);
    }

  /* Attach the shm */
  for(i=0; i<2; i++)
    {
      if((shm[i] = shmat(shmid[i], NULL, 0)) == NULL)
	{
	  fprintf(stderr, "shmat\n");
	  return ERR_SHM_ATT;
	}
    }

  /* Swap shm */
  send_buf[0] = shm[0]; // 0 -> 1: writing
  recv_buf[0] = shm[1]; // 0 <- 1: reading
  send_buf[1] = shm[1]; // 1 -> 0: writing
  recv_buf[1] = shm[0]; // 1 <- 0: reading
#endif
  return 0;
}

DLLEXPORT
int vti_open_port()
{
#ifdef USE_SHM
  int i;

  /* Init buffers */
  for(i=0; i<2; i++)
    {
      shm[i]->tSTART = shm[i]->end = 0;
    }
#endif
  return 0;
}

DLLEXPORT
int vti_put(byte data)
{
#ifdef USE_SHM
  TIME clk;

  //fprintf(stderr, "put: p=%i, tSTART=%i, end=%i\n", p, send_buf[p]->tSTART, send_buf[p]->end);
  tSTART(clk);
  do
    {
      if(tELAPSED(clk, time_out)) return ERR_SND_BYT_TIMEOUT;
    }
  while(((send_buf[p]->end + 1) & 255) == send_buf[p]->tSTART);
  
  send_buf[p]->buf[send_buf[p]->end] = data;       // put data in buffer
  send_buf[p]->end = (send_buf[p]->end+1) & 255;   // update circular buffer
#endif
  return 0;
}

DLLEXPORT
int vti_get(byte *data)
{
#ifdef USE_SHM
  TIME clk;

  //fprintf(stderr, "get: p=%i, tSTART=%i, end=%i\n", p, recv_buf[p]->tSTART, recv_buf[p]->end);
  /* Wait that the buffer has been filled */
  tSTART(clk);
  do
    {
      if(tELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
    }
  while(recv_buf[p]->tSTART == recv_buf[p]->end);
  
  /* And retrieve the data from the circular buffer */
  *data = recv_buf[p]->buf[recv_buf[p]->tSTART];
  recv_buf[p]->tSTART = (recv_buf[p]->tSTART+1) & 255;
#endif
  return 0;
}

DLLEXPORT
int vti_probe_port()
{
  return 0;
}

DLLEXPORT
int vti_close_port()
{
  return 0;
}

DLLEXPORT
int vti_term_port()
{
#ifdef USE_SHM
  int i;

  //DISPLAY("term_port\n");
  /* Detach segment */
  for(i=0; i<2; i++)
    {
      if(shmdt(shm[i]) == -1)
	{
	  fprintf(stderr, "shmdt\n");
	  return ERR_SHM_DTCH;
	}
      /* and destroy it */
      if(shmctl(shmid[i], IPC_RMID, NULL) == -1)
	{
	  fprintf(stderr, "shmctl\n");
	  return ERR_SHM_RMID;
	}
    }
#endif  
  return 0;
}

DLLEXPORT
int vti_check_port(int *status)
{
#ifdef USE_SHM
  *status = STATUS_NONE;

  /* Check if positions are the same */
  if(recv_buf[p]->tSTART == recv_buf[p]->end)
    {
      *status = STATUS_NONE;
      return 0;
    }
  else
    {
      *status = STATUS_RX;
      return 0;
    }
#endif
  return 0;
}

DLLEXPORT
int DLLEXPORT2 vti_supported()
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

#include "timeout.h"
#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"

extern int time_out; // Timeout value for cables in 0.10 seconds
extern int delay;    // Time between 2 bits (home-made cables only)

#define WM_HELLO WM_USER+101
#define WM_GOODBYE WM_USER+102
#define WM_ENABLE_LINK WM_USER+110
#define WM_DISABLE_LINK WM_USER+111
#define WM_SEND_BUFFER WM_USER+120
#define WM_ENTER_DEBUG WM_USER+130
#define WM_EXIT_DEBUG WM_USER+131

/* VTi's LinkBuffer structure */
typedef struct
{
    BYTE buf[256];
    int tSTART;
    int end;
} LinkBuffer;

static LinkBuffer *vSendBuf, *vRecvBuf;	//
static HANDLE hMap = NULL;				// Handle on
static HWND otherWnd = NULL;			// Handle on the VTi window

DLLEXPORT
int vti_init_port(uint io_addr, char *dev)
{
	char name[32];
	char vLinkFileName[32];
	HANDLE hVLinkFileMap = NULL;		// Handle on the 
	HANDLE Handle;
	int i;
	ATOM a;

	/* Create a file mapping handle for the 'lib->VTi' communication channel */
	for (i=0;;i++)
    {
        sprintf(vLinkFileName,"Virtual Link %d", i);
        hVLinkFileMap = CreateFileMapping((HANDLE)-1, NULL,
            PAGE_READWRITE,0,sizeof(LinkBuffer), vLinkFileName);
        if (GetLastError() != ERROR_ALREADY_EXISTS)
            break;
    }
	//DISPLAY("Virtual Link L->V %i\n", i);
    vSendBuf = (LinkBuffer*)MapViewOfFile(hVLinkFileMap,
        FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkBuffer));

	/* Get an handle on the VTi window */
	otherWnd = FindWindow("TEmuWnd", NULL);
	//DISPLAY("otherWnd: %p\n", otherWnd);
	if(!otherWnd)
		return ERR_OPP_NOT_AVAIL;

	/* Get the current DLL handle */
	Handle = GetModuleHandle("libticables.dll");
	if(!Handle)
	{
		fprintf(stderr, "Unable to get an handle on the libTIcables.\n");
		fprintf(stderr, "Did you rename the library ?!\n");
		fprintf(stderr, "Fatal error. Program terminated.\n");
		exit(-1);
	}

	// Inform VTi of our virtual link so that it can enable it. It should returns 
	// its virtual link name in a message.
	SendMessage(otherWnd, WM_HELLO, 0, (LPARAM)Handle);	
	
	/* Retrieve the VTi virtual link name */
	//b = GetMessage(&msg, NULL, WM_HELLO, WM_SEND_BUFFER);
	//WaitMessage();										// Waits VTi answer
	//printf("waitmessage: %i\n", b);

	/* Create a file mapping handle for the 'Vti->lib' communication channel */
	//DISPLAY("Virtual Link V->L %i\n", i-1);
	sprintf(name,"Virtual Link %d", i-1);
	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	if(hMap) 
	{
		fprintf(stderr, "Opened %s\n", name);
		vRecvBuf = (LinkBuffer *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS,
			0, 0, sizeof(LinkBuffer));
	}
	else
		return ERR_OPEN_FILE_MAP;
	
	/* Send to VTi the name of our virtual link. VTi should open it (lib -> Vti) */
    a = GlobalAddAtom(vLinkFileName);
    SendMessage(otherWnd, WM_SEND_BUFFER, 0, (LPARAM)a);
    GlobalDeleteAtom(a);

	/* Enable linking (check the VTi's Virtual Link|Enable cable link' item) */
	if (otherWnd)
		SendMessage(otherWnd, WM_ENABLE_LINK, 0, 0);

  return 0;
}

DLLEXPORT
int vti_open_port()
{
	//vSendBuf->tSTART = vSendBuf->end = 0;
	//vRecvBuf->tSTART = vRecvBuf->end = 0;

  return 0;
}

DLLEXPORT
int vti_put(byte data)
{
	TIME clk;

	if(!hMap)
		return ERR_OPEN_FILE_MAP;

	tSTART(clk);
	  do 
	  { 
		  if(tELAPSED(clk, time_out)) return ERR_SND_BYT_TIMEOUT;
	  }
	while(((vSendBuf->end + 1) & 255) == vSendBuf->tSTART);

    vSendBuf->buf[vSendBuf->end] = data;		// put data in buffer
    vSendBuf->end = (vSendBuf->end+1) & 255;	// update circular buffer

	return 0;
}

DLLEXPORT
int vti_get(byte *data)
{
	TIME clk;

	if(!hMap)
		return ERR_OPEN_FILE_MAP;

	//DISPLAY("s: %i, e: %i\n", vSendBuf->tSTART, vSendBuf->end);

	/* Wait that the buffer has been filled */
	tSTART(clk);
	do
    {
      if(tELAPSED(clk, time_out)) return ERR_RCV_BYT_TIMEOUT;
    }
	while(vRecvBuf->tSTART == vRecvBuf->end);
	
	/* And retrieve the data from the circular buffer */
	*data = vRecvBuf->buf[vRecvBuf->tSTART];
    vRecvBuf->tSTART = (vRecvBuf->tSTART+1) & 255;
	//DISPLAY("get: 0x%02x\n", *data);

  return 0;
}

DLLEXPORT
int vti_close_port()
{
  return 0;
}

DLLEXPORT
int vti_term_port()
{
	/* Send an atom */
	if (otherWnd)
	{
		SendMessage(otherWnd, WM_DISABLE_LINK, 0, 0);
        SendMessage(otherWnd, WM_GOODBYE, 0, 0);
	}

	/* Close the shared buffer */
	if(hMap)
	{
		UnmapViewOfFile(vSendBuf);
		UnmapViewOfFile(vRecvBuf);
		CloseHandle(hMap);
	}

	return 0;
}

DLLEXPORT
int vti_probe_port()
{
	return 0;
}

DLLEXPORT
int vti_check_port(int *status)
{
	if(!hMap)
		return ERR_OPEN_FILE_MAP;

	/* Check if positions are the same */
	*status = !(vRecvBuf->tSTART == vRecvBuf->end);

  return 0;
}

DLLEXPORT
int DLLEXPORT2 vti_supported()
{
  return SUPPORT_ON;
}

#else

/************************/
/* Unsupported platform */
/************************/

DLLEXPORT
int vti_init_port()
{
  return 0;
}

DLLEXPORT
int vti_open_port()
{
  return 0;
}

DLLEXPORT
int vti_put(byte data)
{
  return 0;
}

DLLEXPORT
int vti_get(byte *d)
{
  return 0;
}

DLLEXPORT
int vti_probe_port()
{
  return 0;
}

DLLEXPORT
int vti_close_port()
{
  return 0;
}

DLLEXPORT
int vti_term_port()
{
  return 0;
}

DLLEXPORT
int vti_check_port(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

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

DLLEXPORT
int vti_supported()
{
  return SUPPORT_OFF;
}

#endif
