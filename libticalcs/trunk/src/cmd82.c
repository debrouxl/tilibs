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
  This unit handles TI82 & TI83 commands.
  This unit shares its indentation with cmd85.c
*/

#include <stdio.h>
#include <string.h>

#include "headers.h"
#include "packets.h"
#include "calc_err.h"
#include "update.h"

// Shares some commands between TI82 & 83
extern int ticalcs_calc_type;
#define PC_TI8283 ((ticalcs_calc_type == CALC_TI82) ? PC_TI82 : PC_TI83)
#define TI8283_BKUP ((ticalcs_calc_type == CALC_TI82) ? TI82_BKUP : TI83_BKUP)

/* Variable (std var header: NUL padded, fixed length) */
int ti82_send_VAR(uint16_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[16];
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  printl(0, " PC->TI: VAR (size=0x%04X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, trans);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);

  if (vartype != TI8283_BKUP) {	// backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(PC_TI8283, CMD_VAR, 11, buffer));
  } else {
    TRYF(send_packet(PC_TI8283, CMD_VAR, 9, buffer));
  }

  return 0;
}

int ti82_send_CTS(void)
{
  printl(0, " PC->TI: CTS\n");
  TRYF(send_packet(PC_TI8283, CMD_CTS, 0, NULL));

  return 0;
}

int ti82_send_XDP(int length, uint8_t * data)
{
  printl(0, " PC->TI: XDP (0x%04X = %i bytes)\n", length, length);
  TRYF(send_packet(PC_TI8283, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti82_send_SKIP(uint8_t rej_code)
{
  printl(0, " PC->TI: SKIP");
  TRYF(send_packet(PC_TI8283, CMD_SKIP, 1, &rej_code));
  printl(0, " (rejection code = %i)\n", rej_code);

  return 0;
}

int ti82_send_ACK(void)
{
  printl(0, " PC->TI: ACK\n");
  TRYF(send_packet(PC_TI8283, CMD_ACK, 2, NULL));

  return 0;
}

int ti82_send_ERR(void)
{
  printl(0, " PC->TI: ERR\n");
  TRYF(send_packet(PC_TI8283, CMD_ERR, 2, NULL));

  return 0;
}

int ti82_send_SCR(void)
{
  printl(0, " PC->TI: SCR\n");
  TRYF(send_packet(PC_TI8283, CMD_SCR, 2, NULL));

  return 0;
}














int ti82_send_EOT(void)
{
  printl(0, " PC->TI: EOT\n");
  TRYF(send_packet(PC_TI8283, CMD_EOT, 2, NULL));

  return 0;
}

/* Request variable (std var header: NUL padded, fixed length) */
int ti82_send_REQ(uint16_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[16] = { 0 };
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  printl(0, " PC->TI: REQ (size=0x%04X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, trans);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  pad_buffer(buffer + 3, '\0');

  TRYF(send_packet(PC_TI8283, CMD_REQ, 11, buffer));

  return 0;
}










/* Request to send (std var header: NUL padded, fixed length) */
int ti82_send_RTS(uint16_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[16];
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  printl(0, " PC->TI: RTS (size=0x%04X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, trans);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);

  if (vartype != TI8283_BKUP) {	// backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(PC_TI8283, CMD_RTS, 11, buffer));
  } else {
    TRYF(send_packet(PC_TI8283, CMD_RTS, 9, buffer));
  }

  return 0;
}

int ti82_recv_VAR(uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[16] = { 0 };
  uint16_t length;
  uint8_t trans[9];


  printl(0, " TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;
  if ((length != 11) && (length != 9))
    return ERR_INVALID_PACKET;

  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';





  tifiles_translate_varname(varname, trans, *vartype);
  printl(0, " (size=0x%04X=%i, id=%02X, name=<%s>)",
	  *varsize, *varsize, *vartype, trans);
  printl(0, ".\n");

  return 0;
}

int ti82_recv_CTS(void)
{
  uint8_t host, cmd;
  uint16_t length;

  printl(0, " TI->PC: CTS");
  TRYF(recv_packet(&host, &cmd, &length, NULL));
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;
  if (length != 0x0000)
    return ERR_CTS_ERROR;
  printl(0, ".\n");

  return 0;
}

int ti82_recv_SKIP(uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  *rej_code = 0;

  printl(0, " TI->PC: SKIP");
  TRYF(recv_packet(&host, &cmd, &length, rej_code));
  if (cmd == CMD_CTS) {
    printl(0, "->CTS.\n");
    return 0;
  }
  if (cmd != CMD_SKIP)
    return ERR_INVALID_CMD;
  printl(0, " (rejection code = %i)", *rej_code);
  printl(0, ".\n");

  return 0;
}

int ti82_recv_XDP(uint16_t * length, uint8_t * data)
{
  uint8_t host, cmd;

  printl(0, " TI->PC: XDP");
  TRYF(recv_packet(&host, &cmd, length, data));
  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;
  printl(0, " (%04X=%i bytes)", *length, *length);
  printl(0, ".\n");

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti82_recv_ACK(uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t sts;


  printl(0, " TI->PC: ACK");
  TRYF(recv_packet(&host, &cmd, &sts, NULL));
  if (status != NULL)
    *status = sts;
  else if (sts != 0x0000)
    return ERR_NACK;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;


  printl(0, ".\n");

  return 0;
}

int ti82_recv_RTS(uint16_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[16];
  uint8_t trans[9];


  printl(0, " TI->PC: RTS");
  TRYF(recv_packet(&host, &cmd, varsize, buffer));
  if (cmd != CMD_RTS)
    return ERR_INVALID_CMD;
  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';


  tifiles_translate_varname(varname, trans, *vartype);
  printl(0, " (size=0x%04X=%i, id=%02X, name=<%s>)",
	  *varsize, *varsize, *vartype, trans);
  printl(0, ".\n");

  return 0;
}
