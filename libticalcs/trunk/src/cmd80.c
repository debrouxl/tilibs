/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

/*
  This unit handles TI80 commands.
  This unit shares its indentation with cmd85.c
*/

#include <string.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "dbus_pkt.h"
#include "error.h"
#include "logging.h"
#include "macros.h"

#ifdef _MSC_VER
#pragma warning( disable : 4761 )
#endif

int ti80_send_SCR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: SCR");
  TRYF(dbus_send(handle, PC_TI80, CMD_SCR, 0, NULL));

  return 0;
}

int ti80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
  uint8_t host, cmd;

  TRYF(dbus_recv(handle, &host, &cmd, length, data));

  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti80_recv_ACK(CalcHandle* handle, uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t sts;

  TRYF(dbus_recv(handle, &host, &cmd, &sts, NULL));

  if (status != NULL)
    *status = sts;
  else if (sts != 0x0000)
    return ERR_NACK;

  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: ACK");

  return 0;
}
