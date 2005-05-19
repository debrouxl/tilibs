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
  This unit handles TI73 & TI83+ & TI84+ commands.
*/

#include "ticalcs.h"
#include "packets.h"
#include "error.h"
#include "logging.h"
#include "macros.h"

#ifdef __WIN32__
#pragma warning( disable : 4761 )
#endif

// Shares some commands between TI73 & 83+ & 84+
#define PC_TI7383 ((handle->model == CALC_TI73) ? PC_TI73 : PC_TI83p)
#define TI7383_BKUP ((handle->model == CALC_TI73) ? 0x13/*TI73_BKUP*/ : 0x13/*TI83p_BKUP*/)
#define EXTRAS ((handle->model == CALC_TI83P) || (handle->model == CALC_TI84P) ? 2 : 0)

/* Variable (std var header: NUL padded, fixed length) */
int ti73_send_VAR_h(CalcHandle* handle, uint16_t varsize, uint8_t vartype, char *varname, uint8_t varattr)
{
  uint8_t buffer[16];

  ticalcs_info(" PC->TI: VAR (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)",
		varsize, varsize, vartype, varname, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != TI7383_BKUP) 
  {	// backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(handle, PC_TI7383, CMD_VAR, 11 + EXTRAS, buffer));
  } 
  else 
  {
    TRYF(send_packet(handle, PC_TI7383, CMD_VAR, 9, buffer));
  }

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_send_VAR2_h(CalcHandle* handle, uint32_t length, uint8_t type, uint8_t flag,
		   uint16_t offset, uint16_t page)
{
  uint8_t buffer[11];

  ticalcs_info(" PC->TI: VAR (size=0x%04X=%i, id=%02X, flag=%02X, offset=%04X, page=%02X)",
       length, length, type, flag, offset, page);

  buffer[0] = LSB(LSW(length));
  buffer[1] = MSB(LSW(length));
  buffer[2] = type;
  buffer[3] = LSB(MSW(length));
  buffer[4] = MSB(MSW(length));
  buffer[5] = flag;
  buffer[6] = LSB(offset);
  buffer[7] = MSB(offset);
  buffer[8] = LSB(page);
  buffer[9] = MSB(page);

  TRYF(send_packet(handle, PC_TI7383, CMD_VAR, 10, buffer));

  return 0;
}

int ti73_send_CTS_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: CTS");
  TRYF(send_packet(handle, PC_TI7383, CMD_CTS, 0, NULL));

  return 0;
}

int ti73_send_XDP_h(CalcHandle* handle, int length, uint8_t * data)
{
  ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
  TRYF(send_packet(handle, PC_TI7383, CMD_XDP, length, data));

  return 0;
}

int ti73_send_FLSH_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: FLASH app is following...");
  TRYF(send_packet(handle, PC_TI7383, CMD_FLSH, 2, NULL));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti73_send_SKP_h(CalcHandle* handle, uint8_t rej_code)
{
  ticalcs_info(" PC->TI: SKP");
  TRYF(send_packet(handle, PC_TI7383, CMD_SKP, 1, &rej_code));
  ticalcs_info(" (rejection code = %i)", rej_code);

  return 0;
}

int ti73_send_ACK_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ACK");
  TRYF(send_packet(handle, PC_TI7383, CMD_ACK, 2, NULL));

  return 0;
}

int ti73_send_ERR_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: ERR");
  TRYF(send_packet(handle, PC_TI7383, CMD_ERR, 2, NULL));

  return 0;
}

int ti73_send_RDY_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: RDY?");
  TRYF(send_packet(handle, PC_TI7383, CMD_RDY, 2, NULL));

  return 0;
}

int ti73_send_SCR_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: SCR");
  TRYF(send_packet(handle, PC_TI7383, CMD_SCR, 2, NULL));

  return 0;
}

int ti73_send_KEY_h(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];
  
	ticalcs_info(" PC->TI: KEY");
	buf[0] = PC_TI7383;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);
	TRYF(ticables_cable_send(handle->cable, buf, 4));

	return 0;
}

int ti73_send_EOT_h(CalcHandle* handle)
{
  ticalcs_info(" PC->TI: EOT");
  TRYF(send_packet(handle, PC_TI7383, CMD_EOT, 2, NULL));

  return 0;
}

/* Request variable (std var header: NUL padded, fixed length) */
int ti73_send_REQ_h(CalcHandle* handle, uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr)
{
  uint8_t buffer[16] = { 0 };
  uint8_t trans[9];

  tifiles_transcode_detokenize(handle->model, trans, varname, vartype);
  ticalcs_info(" PC->TI: REQ (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)",
       varsize, varsize, vartype, trans, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  pad_buffer(buffer + 3, '\0');
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != 0x26/*TI83p_IDLIST*/) 
  {
    TRYF(send_packet(handle, PC_TI7383, CMD_REQ, 11 + EXTRAS, buffer));
  } 
  else 
  {
    TRYF(send_packet(handle, PC_TI7383, CMD_REQ, 11, buffer));
  }

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_send_REQ2_h(CalcHandle* handle, uint16_t appsize, uint8_t apptype, char *appname,
		   uint8_t appattr)
{
  uint8_t buffer[16] = { 0 };

  ticalcs_info(" PC->TI: REQ (size=0x%04X=%i, id=%02X, name=<%s>)",
	  appsize, appsize, apptype, appname);

  buffer[0] = LSB(appsize);
  buffer[1] = MSB(appsize);
  buffer[2] = apptype;
  memcpy(buffer + 3, appname, 8);
  pad_buffer(buffer + 3, '\0');

  TRYF(send_packet(handle, TI83p_PC, CMD_REQ, 11, buffer));	// TI_PC73 !

  return 0;
}

/* Request to send (std var header: NUL padded, fixed length) */
int ti73_send_RTS_h(CalcHandle* handle, uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr)
{
  uint8_t buffer[16];
  uint8_t trans[9];

  tifiles_transcode_detokenize(handle->model, trans, varname, vartype);
  ticalcs_info(" PC->TI: RTS (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)",
       varsize, varsize, vartype, trans, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != TI7383_BKUP) 
  {	
	  // backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(handle, PC_TI7383, CMD_RTS, 11 + EXTRAS, buffer));
  } 
  else 
  {
    TRYF(send_packet(handle, PC_TI7383, CMD_RTS, 9, buffer));
  }

  return 0;
}

int ti73_recv_VAR_h(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr)
{
  uint8_t host, cmd;
  uint8_t buffer[16] = { 0 };
  uint16_t length;
  uint8_t trans[9];

  ticalcs_info(" TI->PC: VAR");
  TRYF(recv_packet(handle, &host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;
  if ((length != (11 + EXTRAS)) && (length != 9))
    return ERR_INVALID_PACKET;

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';
  *varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

  tifiles_transcode_detokenize(handle->model, trans, varname, *vartype);
  ticalcs_info(" (size=0x%04X=%i, id=%02X, name=<%s>, attrb=%i)",
	  *varsize, *varsize, *vartype, trans, *varattr);
  ticalcs_info(".");

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_recv_VAR2_h(CalcHandle* handle, uint16_t * length, uint8_t * type, char *name,
		   uint16_t * offset, uint16_t * page)
{
  uint8_t host, cmd;
  uint8_t buffer[16] = { 0 };
  uint16_t len;

  ticalcs_info(" TI->PC: VAR");
  TRYF(recv_packet(handle, &host, &cmd, &len, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;
  if (len != 10)
    return ERR_INVALID_PACKET;

  *length = buffer[0] | (buffer[1] << 8);
  *type = buffer[2];
  memcpy(name, buffer + 3, 3);
  name[3] = '\0';
  *offset = buffer[6] | (buffer[7] << 8);
  *page = buffer[8] | (buffer[9] << 8);

  ticalcs_info(" (size=0x%04X=%i, type=%02X, name=<%s>, offset=%04X, page=%02X)",
       *length, *length, *type, name, *offset, *page & 0xff);

  return 0;
}

int ti73_recv_CTS_h(CalcHandle* handle, uint16_t length)
{
  uint8_t host, cmd;
  uint16_t len;
  uint8_t buffer[5];

  ticalcs_info(" TI->PC: CTS");
  TRYF(recv_packet(handle, &host, &cmd, &len, buffer));
  if (cmd == CMD_SKP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;
  if (length != len)
    return ERR_CTS_ERROR;
  ticalcs_info(".");

  return 0;
}

int ti73_recv_SKP_h(CalcHandle* handle, uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];
  *rej_code = 0;

  ticalcs_info(" TI->PC: SKP");
  TRYF(recv_packet(handle, &host, &cmd, &length, buffer));
  if (cmd == CMD_CTS) 
  {
    ticalcs_info("CTS");
    return 0;
  }
  if (cmd != CMD_SKP)
    return ERR_INVALID_CMD;
  ticalcs_info(" (rejection code = %i)", *rej_code = buffer[0]);
  ticalcs_info(".");

  return 0;
}

int ti73_recv_XDP_h(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
  uint8_t host, cmd;

  ticalcs_info(" TI->PC: XDP");
  TRYF(recv_packet(handle, &host, &cmd, length, data));
  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;
  ticalcs_info(" (%04X=%i bytes)", *length, *length);
  ticalcs_info(".");

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti73_recv_ACK_h(CalcHandle* handle, uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];

  ticalcs_info(" TI->PC: ACK");
  TRYF(recv_packet(handle, &host, &cmd, &length, buffer));
  if (status != NULL)
    *status = length;
  else if (length != 0x0000)
    return ERR_NACK;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  ticalcs_info(".");

  return 0;
}

int ti73_recv_RTS_h(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr)
{
  uint8_t host, cmd;
  uint8_t buffer[16];
  uint8_t trans[9];

  ticalcs_info(" TI->PC: RTS");
  TRYF(recv_packet(handle, &host, &cmd, varsize, buffer));
  if (cmd != CMD_RTS)
    return ERR_INVALID_CMD;
  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';
  *varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	tifiles_transcode_detokenize(handle->model, trans, varname, *vartype);
  ticalcs_info(" (size=0x%04X=%i, id=%02X, name=<%s>, attrb=%i)",
	  *varsize, *varsize, *vartype, trans, *varattr);
  ticalcs_info(".");

  return 0;
}
