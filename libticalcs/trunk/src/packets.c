/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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
   This unit manages packets
*/


#include <stdio.h>
#include <string.h>

#include "headers.h"
#include "externs.h"
#include "update.h"
#include "packets.h"
#include "calc_err.h"

/*
  Send a packet from PC (host) to TI (target):
  - target [in]	: a machine ID uint8_t
  - cmd [in]	: a command ID uint8_t
  - length [in]	: length of buffer
  - data [in]	: data to send (or 0x00 if NULL)
  - int [out]	: an error code
*/
int send_packet(uint8_t target, uint8_t cmd, uint16_t len, uint8_t * data)
{
  int i;
  uint16_t sum;
  uint32_t length = (len == 0x0000) ? 65536 : len;	//  wrap around

  TRYF(cable->put(target));
  TRYF(cable->put(cmd));

  if (data == NULL) {		// short packet (no data)
    TRYF(cable->put(0x00));
    TRYF(cable->put(0x00));
  } else {			// std packet (data + checksum)
    TRYF(cable->put(LSB(length)))
	TRYF(cable->put(MSB(length)));

    update->total = length;
    for (i = 0; i < (int)length; i++) {
      TRYF(cable->put(data[i]));

      update->count = i;
      update_pbar();
      if (update->cancel)
	return ERR_ABORT;
    }

    sum = tifiles_compute_checksum(data, length);
    TRYF(cable->put(LSB(sum)));
    TRYF(cable->put(MSB(sum)));
  }

  return 0;
}

static uint8_t host_ids()
{
  switch (ticalcs_calc_type) {
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
int recv_packet(uint8_t * host, uint8_t * cmd, uint16_t * length,
		uint8_t * data)
{
  uint8_t d;
  int i;
  uint16_t chksum;

  TRYF(cable->get(host));
  if (*host != host_ids())
    return ERR_INVALID_HOST;
  TRYF(cable->get(cmd));
  if (*cmd == CMD_ERR)
    return ERR_CHECKSUM;
  TRYF(cable->get(&d));
  *length = d;
  TRYF(cable->get(&d));
  *length |= d << 8;

  switch (*cmd) {
  case CMD_VAR:
  case CMD_XDP:
  case CMD_SKIP:
  case CMD_SID:
  case CMD_REQ:
  case CMD_IND:
  case CMD_RTS:		// std packet ( data + checksum)

    update->total = *length;
    for (i = 0; i < *length; i++) {
      TRYF(cable->get(&(data[i])));

      update->count = i;
      update_pbar();
      if (update->cancel)
	return ERR_ABORT;
    }

    TRYF(cable->get(&d));
    chksum = d;
    TRYF(cable->get(&d));
    chksum |= d << 8;

    if (chksum != tifiles_compute_checksum(data, *length))
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
