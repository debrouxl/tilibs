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
  This unit handles TI85 & TI86 commands.
  This unit shares its indentation with cmd82.c
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
#pragma warning( disable : 4761 )
#endif

// Shares some commands between TI85 & 86
#define PC_TI8586 ((handle->model == CALC_TI85) ? PC_TI85 : PC_TI86)
#define TI8586_BKUP ((handle->model == CALC_TI85) ? TI85_BKUP : TI86_BKUP)

/* Variable (std var header: NUL padded, variable length) */
int ti85_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[16];
  char trans[17];

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;


  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
  ticalcs_info(" PC->TI: VAR (size=0x%04X, id=%02X, name=%s)",
	  varsize, vartype, trans);

  if (vartype != TI8586_BKUP) 
  {	// backup: special header
    buffer[3] = strlen(varname);
    memcpy(buffer + 4, varname, 8);
    TRYF(dbus_send(handle, PC_TI8586, CMD_VAR, 4 + strlen(varname), buffer));
  } 
  else 
  {
    memcpy(buffer + 3, varname, 6);
    TRYF(dbus_send(handle, PC_TI8586, CMD_VAR, 9, buffer));
  }

  return 0;
}

int ti85_send_CTS(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: CTS");
  TRYF(dbus_send(handle, PC_TI8586, CMD_CTS, 0, NULL));

  return 0;
}

int ti85_send_XDP(CalcHandle* handle, int length, uint8_t * data)
{
  ticalcs_info(" PC->TI: XDP (0x%04X bytes)", length);
  TRYF(dbus_send(handle, PC_TI8586, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti85_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
  TRYF(dbus_send(handle, PC_TI8586, CMD_SKP, 1, &rej_code));
  ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);

  return 0;
}

int ti85_send_ACK(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ACK");
  TRYF(dbus_send(handle, PC_TI8586, CMD_ACK, 2, NULL));

  return 0;
}

int ti85_send_ERR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ERR");
  TRYF(dbus_send(handle, PC_TI8586, CMD_ERR, 2, NULL));

  return 0;
}

int ti85_send_SCR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: SCR");
  TRYF(dbus_send(handle, PC_TI8586, CMD_SCR, 2, NULL));

  return 0;
}

int ti85_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];

	buf[0] = PC_TI8586;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	TRYF(ticables_cable_send(handle->cable, buf, 4));

	return 0;
}

int ti85_send_EOT(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: EOT");
  TRYF(dbus_send(handle, PC_TI8586, CMD_EOT, 2, NULL));

  return 0;
}

/* Variable request (var header: NUL padded, fixed length) */
int ti85_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[16] = { 0 };
  char trans[9];

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  buffer[3] = strlen(varname);
  memcpy(buffer + 4, varname, 8);

  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
  ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s)",
	  varsize, vartype, trans);
  if ((handle->model == CALC_TI86) && (vartype >= TI86_DIR) && (vartype <= TI86_ZRCL)) 
  {
    memset(buffer, 0, 9);
    buffer[2] = vartype;
    TRYF(dbus_send(handle, PC_TI86, CMD_REQ, 5, buffer));
  } 
  else if((handle->model == CALC_TI86) && (vartype == TI86_BKUP))
  {
	  memset(buffer, 0, 12);
	  buffer[2] = vartype;
      TRYF(dbus_send(handle, PC_TI86, CMD_REQ, 11, buffer));
  }
  else
  {
    pad_buffer_to_8_chars(buffer + 4, '\0');
    TRYF(dbus_send(handle, PC_TI8586, CMD_REQ, 4 + strlen(varname), buffer));
  }

  return 0;
}

/* Request to send (var header: SPC padded, fixed length) */
int ti85_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[16];
  char trans[9];

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  buffer[3] = strlen(varname);
  memcpy(buffer + 4, varname, 8);
  pad_buffer_to_8_chars(buffer + 4, ' ');

  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
  ticalcs_info(" PC->TI: RTS (size=0x%04X, id=%02X, name=%s)",
	  varsize, vartype, trans);

  TRYF(dbus_send(handle, PC_TI8586, CMD_RTS, 12, buffer));

  return 0;
}

/* Variable (std var header: NUL padded, variable length) */
int ti85_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  uint16_t length;
  char trans[9];
  uint8_t strl;

  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;

  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  //if((length != (4+strlen(varname))) && (length != 9)) 
  //return ERR_INVALID_PACKET; 

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  if (*vartype != TI8586_BKUP) 
  {
    strl = buffer[3];
    memcpy(varname, buffer + 4, strl);
    varname[strl] = '\0';
  } 
  else 
  {
    memcpy(varname, buffer + 3, 8);
  }

  ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
  ticalcs_info(" TI->PC: VAR (size=0x%04X, id=%02X, name=%s)",
	  *varsize, *vartype, trans);

  return 0;
}

int ti85_recv_CTS(CalcHandle* handle)
{
  uint8_t host, cmd;
  uint16_t length;

  TRYF(dbus_recv(handle, &host, &cmd, &length, NULL));

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;

  if (length != 0x0000)
    return ERR_CTS_ERROR;

  ticalcs_info(" TI->PC: CTS");

  return 0;
}

int ti85_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  *rej_code = 0;

  TRYF(dbus_recv(handle, &host, &cmd, &length, rej_code));

  if (cmd == CMD_CTS) 
  {
    ticalcs_info("->CTS.");
    return 0;
  }

  if (cmd != CMD_SKP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: SKP (rejection code = %i)", *rej_code);

  return 0;
}

int ti85_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
  uint8_t host, cmd;

  TRYF(dbus_recv(handle, &host, &cmd, length, data));

  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: XDP (%04X bytes)", *length);

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti85_recv_ACK(CalcHandle* handle, uint16_t * status)
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

int ti85_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  char trans[9];
  uint8_t strl;

  TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

  if (cmd != CMD_RTS)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  strl = buffer[3];
  memcpy(varname, buffer + 4, strl);
  varname[strl] = '\0';

  ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
  ticalcs_info(" TI->PC: RTS (size=0x%04X, id=%02X, name=%s)",
	  *varsize, *vartype, trans);

  return 0;
}
