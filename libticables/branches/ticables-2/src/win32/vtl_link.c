/* Hey EMACS -*- win32-c -*- */
/* $Id: vtl_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* General purpose virtual link cable unit (generic) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "externs.h"
#include "printl.h"

static const char *pipeName = "\\\\.\\pipe\\vtl";	// string: \\.\pipe\pipename
HANDLE hPipe;

static struct cs 
{
  uint8_t data;
  int available;
} cs;

int vtl_init()
{
	int p;

  /* Check if valid argument */
  if (io_address > 2)
    return ERR_ILLEGAL_ARG;
  else
    p = io_address;

  /* Create the pipe (in non-blocking mode) */
  hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_NOWAIT, 2, 256, 256, 0 * time_out, NULL);	// 100 * time_out
  if (hPipe == INVALID_HANDLE_VALUE) 
    return ERR_OPEN_PIPE;

  return 0;
}

int vtl_exit()
{
  if (hPipe) 
  {
    CloseHandle(hPipe);
    hPipe = 0;
  }

  return 0;
}

int vtl_open()
{
  BOOL fSuccess;
  DWORD i;
  uint8_t data;

  /* Flush the pipe */
  do 
  {
    fSuccess = ReadFile(hPipe, &data, 1, &i, NULL);
  }
  while (i > 0);

  tdr.count = 0;
  toSTART(tdr.start);

  return 0;
}

int vtl_close()
{
  return 0;
}

int vtl_put(uint8_t data)
{
  DWORD i;
  BOOL fSuccess;

  tdr.count++;

  // Write the data
  fSuccess = WriteFile(hPipe, &data, 1, &i, NULL);
  if (!fSuccess) 
    return ERR_WRITE_ERROR;
  else if (i == 0) 
    return ERR_WRITE_TIMEOUT;

  return 0;
}

int vtl_get(uint8_t * data)
{
  DWORD i;
  BOOL fSuccess;
  tiTIME clk;

  tdr.count++;
  /* If the tig_check function was previously called, retrieve the uint8_t */
  if (cs.available) 
  {
    *data = cs.data;
    cs.available = 0;
    return 0;
  }

  toSTART(clk);
  do 
  {
    if (toELAPSED(clk, time_out))
      return ERR_READ_TIMEOUT;
    fSuccess = ReadFile(hPipe, data, 1, &i, NULL);
  }
  while (i != 1);

  return 0;
}

int vtl_check(int *status)
{
  DWORD i;
  BOOL fSuccess;

  *status = STATUS_NONE;
  
  if (hPipe) 
  {
    // Read the data: return 0 if error and i contains 1 or 0 (timeout)
    fSuccess = ReadFile(hPipe, (&cs.data), 1, &i, NULL);
    if (fSuccess && (i == 1)) 
	{
      if (cs.available == 1)
		return ERR_BYTE_LOST;

      cs.available = 1;
      *status = STATUS_RX;
      return 0;
    } 
	else 
	{
      *status = STATUS_NONE;
      return 0;
    }
  }

  return 0;
}

int vtl_probe()
{
  return 0;
}

int vtl_supported()
{
  return SUPPORT_ON;
}

int vtl_register_cable(TicableLinkCable * lc)
{
  lc->init = vtl_init;
  lc->open = vtl_open;
  lc->put = vtl_put;
  lc->get = vtl_get;
  lc->close = vtl_close;
  lc->exit = vtl_exit;
  lc->probe = vtl_probe;
  lc->check = vtl_check;

  lc->set_red_wire = NULL;
  lc->set_white_wire = NULL;
  lc->get_red_wire = NULL;
  lc->get_white_wire = NULL;

  return 0;
}
