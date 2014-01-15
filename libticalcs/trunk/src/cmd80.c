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

TIEXPORT3 int TICALL ti80_send_SCR(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, PC_TI80, CMD_SCR, 0, NULL);
}

TIEXPORT3 int TICALL ti80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host, cmd;
	int ret;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ret = dbus_recv(handle, &host, &cmd, length, data);
	if (ret == 0)
	{
		if (cmd != CMD_XDP)
		{
			ret = ERR_INVALID_CMD;
		}

		if (length != NULL)
		{
			ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);
		}
	}

	return ret;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
TIEXPORT3 int TICALL ti80_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	uint8_t host, cmd;
	uint16_t sts;
	int ret;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ret = dbus_recv(handle, &host, &cmd, &sts, NULL);
	if (ret == 0)
	{
		if (status != NULL)
		{
			*status = sts;

			if (cmd != CMD_ACK)
			{
				ret = ERR_INVALID_CMD;
			}
		}
		else
		{
			if (sts != 0x0000)
			{
				ret = ERR_NACK;
			}
			else
			{
				if (cmd != CMD_ACK)
				{
					ret = ERR_INVALID_CMD;
				}
			}
		}

		ticalcs_info(" TI->PC: ACK");
	}

	return ret;
}
