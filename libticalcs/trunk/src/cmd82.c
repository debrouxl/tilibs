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
  This unit handles TI82 & TI83 commands.
  This unit shares its indentation with cmd85.c
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

// Shares some commands between TI82 & 83
#define PC_TI8283 ((handle->model == CALC_TI82) ? PC_TI82 : PC_TI83)
#define TI8283_BKUP ((handle->model == CALC_TI82) ? TI82_BKUP : TI83_BKUP)

/* Variable (std var header: NUL padded, fixed length) */
int ti82_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[17];

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy(buffer + 3, varname, 8);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: VAR (size=0x%04X=%i, id=%02X, name=%s)",
	  varsize, varsize, vartype, trans);

	if (vartype != TI8283_BKUP) 
	{	
		// backup: special header
		pad_buffer_to_8_chars(buffer + 3, '\0');
		TRYF(dbus_send(handle, PC_TI8283, CMD_VAR, 11, buffer));
	} 
	else 
	{

		TRYF(dbus_send(handle, PC_TI8283, CMD_VAR, 9, buffer));
	}

	return 0;
}

int ti82_send_CTS(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: CTS");
	TRYF(dbus_send(handle, PC_TI8283, CMD_CTS, 0, NULL));

  return 0;
}

int ti82_send_XDP(CalcHandle* handle, int length, uint8_t * data)
{
  ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
  TRYF(dbus_send(handle, PC_TI8283, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti82_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
  TRYF(dbus_send(handle, PC_TI8283, CMD_SKP, 1, &rej_code));
  ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);

  return 0;
}

int ti82_send_ACK(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ACK");
  TRYF(dbus_send(handle, PC_TI8283, CMD_ACK, 2, NULL));

  return 0;
}

int ti82_send_ERR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ERR");
  TRYF(dbus_send(handle, PC_TI8283, CMD_ERR, 2, NULL));

  return 0;
}

int ti82_send_SCR(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: SCR");
  TRYF(dbus_send(handle, PC_TI8283, CMD_SCR, 2, NULL));

  return 0;
}

int ti83_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];
  
	buf[0] = PC_TI83;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	TRYF(ticables_cable_send(handle->cable, buf, 4));

	return 0;
}

int ti82_send_EOT(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: EOT");
  TRYF(dbus_send(handle, PC_TI8283, CMD_EOT, 2, NULL));

  return 0;
}

/* Request variable (std var header: NUL padded, fixed length) */
int ti82_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[16] = { 0 };
  char trans[9];

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  pad_buffer_to_8_chars(buffer + 3, '\0');

  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
  ticalcs_info(" PC->TI: REQ (size=0x%04X=%i, id=%02X, name=%s)",
	  varsize, varsize, vartype, trans);

  TRYF(dbus_send(handle, PC_TI8283, CMD_REQ, 11, buffer));

  return 0;
}

/* Request to send (std var header: NUL padded, fixed length) */
int ti82_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
  uint8_t buffer[16];
  char trans[9];

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);

  ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
  ticalcs_info(" PC->TI: RTS (size=0x%04X=%i, id=%02X, name=%s)",
	  varsize, varsize, vartype, trans);

  if (vartype != TI8283_BKUP) 
  {	// backup: special header
    pad_buffer_to_8_chars(buffer + 3, '\0');
    TRYF(dbus_send(handle, PC_TI8283, CMD_RTS, 11, buffer));
  } 
  else 
  {
    TRYF(dbus_send(handle, PC_TI8283, CMD_RTS, 9, buffer));
  }

  return 0;
}

/* Send an invalid packet that causes the calc to execute assembly
   code stored in the most recently transferred variable.

   The program must perform whatever cleanup is necessary, including
   restoring (FPS), (OPS), and (errSP).  You can do so by calling
   ResetStacks, or by jumping to JForceCmdNoChar when you exit.  For
   ROM-independent methods, see romdump.asm.
*/
int ti82_send_asm_exec(CalcHandle* handle, VarEntry * var)
{
	uint16_t ioData  = (handle->model == CALC_TI82 ? 0x81fd : 0x831e);
	uint16_t errSP   = (handle->model == CALC_TI82 ? 0x821a : 0x8338);
	uint16_t onSP    = (handle->model == CALC_TI82 ? 0x8143 : 0x81bc);
	uint16_t tempMem = (handle->model == CALC_TI82 ? 0x8d0a : 0x8bdd);
	uint16_t fpBase  = (handle->model == CALC_TI82 ? 0x8d0c : 0x8bdf);
	uint8_t buffer[50];
	uint16_t length, offset, endptr, es, sum;

	if (handle->model != CALC_TI82 && handle->model != CALC_TI85)
	{
		ticalcs_critical("asm_exec not supported for this model");
		return ERR_UNSUPPORTED;
	}

	buffer[0] = (handle->model == CALC_TI82 ? PC_TI82 : PC_TI85);
	buffer[1] = CMD_VAR;

	/* Warning: Heavy wizardry begins here. ;) */

	length = errSP + 2 - ioData;
	buffer[2] = LSB(length);
	buffer[3] = MSB(length);

	memset(buffer + 4, 0, length);

	/* ld sp, (onSP) */
	buffer[4] = 0xed; buffer[5] = 0x7b; buffer[6] = LSB(onSP); buffer[7] = MSB(onSP);
	/* ld hl, (endptr) */
	endptr = (var->name[0] == 0x24 ? fpBase : tempMem);
	buffer[8] = 0x2a; buffer[9] = LSB(endptr); buffer[10] = MSB(endptr);
	/* ld de, -program_size */
	offset = -(var->size - 2);
	buffer[11] = 0x11; buffer[12] = LSB(offset); buffer[13] = MSB(offset);
	/* add hl, de */
	buffer[14] = 0x19;
	/* jp (hl) */
	buffer[15] = 0xe9;

	es = 4 + errSP - ioData;
	buffer[es] = LSB(errSP - 11); buffer[es + 1] = MSB(errSP - 11);

	buffer[es - 4] = (handle->model == CALC_TI82 ? 0x88 : 0);
	buffer[es - 3] = LSB(ioData); buffer[es - 2] = MSB(ioData);

	sum = tifiles_checksum(buffer + 4, length) + 0x5555;
	buffer[4 + length] = LSB(sum);
	buffer[4 + length + 1] = MSB(sum);

	ticalcs_info(" PC->TI: VAR (exec assembly; program size = 0x%04X)", var->size);

	return ticables_cable_send(handle->cable, buffer, length + 6);
}

int ti82_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  uint16_t length;
  char trans[9];


  TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error

  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;

  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  if ((length != 11) && (length != 9))
    return ERR_INVALID_PACKET;

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';

  ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
  ticalcs_info(" TI->PC: VAR (size=0x%04X=%i, id=%02X, name=%s)",
	  *varsize, *varsize, *vartype, trans);

  return 0;
}

int ti82_recv_CTS(CalcHandle* handle)
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

int ti82_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  *rej_code = 0;

  TRYF(dbus_recv(handle, &host, &cmd, &length, rej_code));

  if (cmd == CMD_CTS) 
  {
    ticalcs_info(" TI->PC: CTS");
    return 0;
  }

  if (cmd != CMD_SKP)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: SKP (rejection code = %i)", *rej_code);

  return 0;
}

int ti82_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
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
int ti82_recv_ACK(CalcHandle* handle, uint16_t * status)
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

int ti82_recv_ERR(CalcHandle* handle, uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t sts;
  int ret;

  ret = dbus_recv(handle, &host, &cmd, &sts, NULL);
  if (ret && ret != ERR_CHECKSUM)
    return ret;

  if (status != NULL)
    *status = sts;

  if (cmd != CMD_ERR)
    return ERR_INVALID_CMD;

  ticalcs_info(" TI->PC: ERR");

  return 0;
}

int ti82_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t *buffer = (uint8_t *)handle->priv2;
  char trans[9];


  TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

  if (cmd != CMD_RTS)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';

  ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
  ticalcs_info(" TI->PC: RTS (size=0x%04X=%i, id=%02X, name=%s)",
	  *varsize, *varsize, *vartype, trans);

  return 0;
}
