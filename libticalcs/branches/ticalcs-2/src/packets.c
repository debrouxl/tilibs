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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	This unit manages packets
*/


#include "ticalcs.h"
#include "packets.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

/*
    Send a packet from PC (host) to TI (target):
    - target [in]	: a machine ID uint8_t
    - cmd [in]	: a command ID uint8_t
    - length [in]	: length of buffer
    - data [in]	: data to send (or 0x00 if NULL)
    - int [out]	: an error code
*/
int send_packet(CalcHandle* handle,
				uint8_t target, uint8_t cmd, uint16_t len, uint8_t* data)
{
	int i;
	uint16_t sum;
	uint32_t length = (len == 0x0000) ? 65536 : len;	//  wrap around

	TRYF(ticables_cable_put(handle->cable, target));
	TRYF(ticables_cable_put(handle->cable, cmd));

	if (data == NULL) 
	{		
		// short packet (no data)
		TRYF(ticables_cable_put(handle->cable, 0x00));
		TRYF(ticables_cable_put(handle->cable, 0x00));
	} 
	else 
	{
		// std packet (data + checksum)
		TRYF(ticables_cable_put(handle->cable, LSB(length)))
		TRYF(ticables_cable_put(handle->cable, MSB(length)));

		handle->update->max1 = length;
		for (i = 0; i < (int)length; i++) 
		{
			TRYF(ticables_cable_put(handle->cable, data[i]));

			handle->update->cnt1 = i;
			handle->update->pbar();
			if (handle->update->cancel)
				return ERR_ABORT;
		}

		sum = tifiles_checksum(data, length);
		TRYF(ticables_cable_put(handle->cable, LSB(sum)));
		TRYF(ticables_cable_put(handle->cable, MSB(sum)));
	}

	return 0;
}

static uint8_t host_ids(CalcHandle *handle)
{
  switch (handle->model) 
  {
  case CALC_TI73:
    return TI73_PC;
    break;
  case CALC_TI82:
    return TI82_PC;
    break;
  case CALC_TI83:
    return TI83_PC;
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return TI83p_PC;
    break;
  case CALC_TI85:
    return TI85_PC;
    break;
  case CALC_TI86:
    return TI86_PC;
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return TI89_PC;
    break;
  case CALC_TI92:
    return TI92_PC;
    break;
  case CALC_TI92P:
    return TI92p_PC;
    break;
  case CALC_V200:
    return V200_PC;
    break;
  }
  return 0x00;
}

/*
  Receive a packet from TI (target) to PC (host):
  - host [out]	: a machine ID uint8_t
  - cmd [out]	: a command ID uint8_t
  - length [out]: length of buffer
  - data [out]	: received data (depending on command)
  - int [out]	: an error code
*/
int recv_packet(CalcHandle* handle, 
				uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data)
{
	uint8_t d;
	int i;
	uint16_t chksum;

	TRYF(ticables_cable_get(handle->cable, host));
	if (*host != host_ids(handle)) return ERR_INVALID_HOST;

	TRYF(ticables_cable_get(handle->cable, cmd));
	if (*cmd == CMD_ERR) return ERR_CHECKSUM;

	TRYF(ticables_cable_get(handle->cable, &d));
	*length = d;

	TRYF(ticables_cable_get(handle->cable, &d));
	*length |= d << 8;

	  switch (*cmd) 
	  {
	  case CMD_VAR:
	  case CMD_XDP:
	  case CMD_SKIP:
	  case CMD_SID:
	  case CMD_REQ:
	  case CMD_IND:
	  case CMD_RTS:		
		  // std packet ( data + checksum)
			handle->update->max1 = *length;
			for (i = 0; i < *length; i++) 
			{
				TRYF(ticables_cable_get(handle->cable, &(data[i])));

				handle->update->cnt1 = i;
				handle->update->pbar();
				if (handle->update->cancel)
					return ERR_ABORT;
			}

			TRYF(ticables_cable_get(handle->cable, &d));
			chksum = d;
			TRYF(ticables_cable_get(handle->cable, &d));
			chksum |= d << 8;

			if (chksum != tifiles_checksum(data, *length))
				return ERR_CHECKSUM;
		break;
	  case CMD_CTS:
	  case CMD_ACK:
	  case CMD_ERR:
	  case CMD_RDY:
	  case CMD_SCR:
	  case CMD_RID:
	  case CMD_KEY:
	  case CMD_EOT:
	  case CMD_CONT:
			// short packet (no data)
			break;
	  default:
			return ERR_INVALID_CMD;
	  }

	return 0;
}

/* Complete a 8-chars buffer with NUL chars */
void pad_buffer(char *varname, uint8_t value)
{
	int i, len = strlen(varname);

	for (i = len; i < 8; i++)
		varname[i] = value;
}
