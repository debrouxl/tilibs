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
  This unit handles TI92 commands.
*/

#include <string.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "error.h"
#include "logging.h"
#include "macros.h"

#ifdef _MSC_VER
#pragma warning( disable : 4761 4244)
#endif

int ti92_send_VAR(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[32];
  char trans[17];

  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  ticalcs_info(" PC->TI: VAR (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);
  TRYF(dbus_send(handle, PC_TI92, CMD_VAR, 6 + strlen(varname), buffer));

  return 0;
}

int ti92_send_CTS(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: CTS");
  TRYF(dbus_send(handle, PC_TI92, CMD_CTS, 2, NULL));

  return 0;
}

int ti92_send_XDP(CalcHandle* handle, uint32_t length, uint8_t * data)
{
  ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
  TRYF(dbus_send(handle, PC_TI92, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti92_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
  TRYF(dbus_send(handle, PC_TI92, CMD_SKP, 1, &rej_code));
  ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);

  return 0;
}

int ti92_send_ACK(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ACK");
  TRYF(dbus_send(handle, PC_TI92, CMD_ACK, 2, NULL));

  return 0;
}

int ti92_send_ERR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ERR");
  TRYF(dbus_send(handle, PC_TI92, CMD_ERR, 2, NULL));

  return 0;
}

int ti92_send_RDY(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: RDY?");
  TRYF(dbus_send(handle, PC_TI92, CMD_RDY, 2, NULL));

  return 0;
}

int ti92_send_SCR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: SCR");
  TRYF(dbus_send(handle, PC_TI92, CMD_SCR, 2, NULL));

  return 0;
}

int ti92_send_CNT(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: CNT");
  TRYF(dbus_send(handle, PC_TI92, CMD_CNT, 2, NULL));

  return 0;
}

int ti92_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];
  
	buf[0] = PC_TI92;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	TRYF(ticables_cable_send(handle->cable, buf, 4));

	return 0;
}

int ti92_send_EOT(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: EOT");
  TRYF(dbus_send(handle, PC_TI92, CMD_EOT, 2, NULL));

  return 0;
}

int ti92_send_REQ(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[32] = { 0 };

  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  ticalcs_info(" PC->TI: REQ (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
  TRYF(dbus_send(handle, PC_TI92, CMD_REQ, 6 + strlen(varname), buffer));

  return 0;
}

int ti92_send_RTS(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[32] = { 0 };

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  ticalcs_info(" PC->TI: RTS (size=0x%08X=%i, id=%02X, name=%s)",
	  varsize, varsize, vartype, varname);
  TRYF(dbus_send(handle, PC_TI92, CMD_RTS, 6 + strlen(varname), buffer));

  return 0;
}


/* Variable (std var header: variable length) */
int ti92_recv_VAR(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  uint16_t length;
  uint8_t strl;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
  {
	  ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;		// not really an error
  }

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;

  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8) |
      (buffer[2] << 16) | (buffer[3] << 24);
  *vartype = buffer[4];
  strl = buffer[5];
  memcpy(varname, buffer + 6, strl);
  varname[strl] = '\0';

  if (length != (6 + strlen(varname)))
    return ERR_INVALID_PACKET;

  ticalcs_info(" TI->PC: VAR (size=0x%08X=%i, id=%02X, name=%s)",
	  *varsize, *varsize, *vartype, varname);

  return 0;
}

int ti92_recv_CTS(CalcHandle* handle)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t *buffer = (uint8_t *)handle->priv2;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;

  if (length != 0x0000)
    return ERR_CTS_ERROR;

  ticalcs_info(" TI->PC: CTS");

  return 0;
}

int ti92_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  *rej_code = 0;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (cmd == CMD_CTS) 
  {
    ticalcs_info("CTS");
    return 0;
  }

  if (cmd != CMD_SKP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: SKP (rejection code = %i)", *rej_code = buffer[0]);

  return 0;
}

int ti92_recv_XDP(CalcHandle* handle, uint32_t * length, uint8_t * data)
{
  uint8_t host, cmd;
  int err;
  uint16_t len;

  err = dbus_recv(handle, &host, &cmd, &len, data);
  *length = len;

  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);
  TRYF(err);

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti92_recv_ACK(CalcHandle* handle, uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t *buffer = (uint8_t *)handle->priv2;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (status != NULL)
    *status = length;
  else if (length != 0x0000)
    return ERR_NACK;

  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: ACK");

  return 0;
}

int ti92_recv_CNT(CalcHandle* handle)
{
  uint8_t host, cmd;
  uint16_t sts;

  TRYF(dbus_recv(handle, &host, &cmd, &sts, NULL));

  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error

  if (cmd != CMD_CNT)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: CNT");

  return 0;
}

int ti92_recv_EOT(CalcHandle* handle)
{
  uint8_t host, cmd;
  uint16_t length;

  TRYF(dbus_recv(handle, &host, &cmd, &length, NULL));

  if (cmd != CMD_EOT)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: EOT");

  return 0;
}

int ti92_recv_RTS(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  uint16_t length;
  uint8_t strl;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;

  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8) |
      (buffer[2] << 16) | (buffer[3] << 24);
  *vartype = buffer[4];
  strl = buffer[5];
  memcpy(varname, buffer + 6, strl);
  varname[strl] = '\0';

  if (length != (6 + strlen(varname)))
    return ERR_INVALID_PACKET;

  ticalcs_info(" TI->PC: VAR (size=0x%08X=%i, id=%02X, name=%s)",
	  *varsize, *varsize, *vartype, varname);

  return 0;
}
