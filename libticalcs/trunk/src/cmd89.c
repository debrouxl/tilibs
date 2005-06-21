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
  This unit handles TI89/92+/V200 commands.
*/

#include <stdio.h>
#include <string.h>

#include "headers.h"
#include "packets.h"
#include "calc_err.h"
#include "update.h"
#include "printl.h"

#ifdef __WIN32__
#pragma warning( disable : 4761 4244)
#endif

// Shares some commands between TI89/92+/V200/TI89 Titanium
extern int ticalcs_calc_type;

static uint8_t pc_ti9x(void)
{
  switch (ticalcs_calc_type) {
  case CALC_TI89:
  case CALC_TI89T:
    return PC_TI89;
    break;
  case CALC_TI92P:
    return PC_TI92p;
    break;
  case CALC_V200:
    return PC_V200;
    break;
  default:
    return PC_TIXX;
    break;
  }
  return 0;
}

#define PC_TI9X pc_ti9x()
#define TI9X_BKUP TI89_BKUP

int ti89_send_VAR(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32];
  uint8_t trans[32];
  uint8_t extra = (vartype == TI9X_BKUP) ? 0 : 1;

  tifiles_translate_varname(varname, trans, vartype);
  printl2(0, " PC->TI: VAR (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, trans);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));
  buffer[6 + strlen(varname)] = 0x03;

  TRYF(send_packet(PC_TI9X, CMD_VAR, 6 + strlen(varname) + extra, buffer));

  return 0;
}

int ti89_send_CTS(void)
{
  printl2(0, " PC->TI: CTS\n");
  TRYF(send_packet(PC_TI9X, CMD_CTS, 2, NULL));

  return 0;
}

int ti89_send_XDP(uint32_t length, uint8_t * data)
{
  printl2(0, " PC->TI: XDP (0x%04X = %i)\n", length, length);
  TRYF(send_packet(PC_TI9X, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti89_send_SKIP(uint8_t rej_code)
{
  uint8_t buffer[5] = { 0 };

  buffer[0] = rej_code;

  printl2(0, " PC->TI: SKIP");
  TRYF(send_packet(PC_TI9X, CMD_SKIP, 3, buffer));
  printl2(0, " (rejection code = %i)\n", rej_code);

  return 0;
}

int ti89_send_ACK(void)
{
  printl2(0, " PC->TI: ACK\n");
  TRYF(send_packet(PC_TI9X, CMD_ACK, 2, NULL));

  return 0;
}

int ti89_send_ERR(void)
{
  printl2(0, " PC->TI: ERR\n");
  TRYF(send_packet(PC_TI9X, CMD_ERR, 2, NULL));

  return 0;
}

int ti89_send_RDY(void)
{
  printl2(0, " PC->TI: RDY?\n");
  TRYF(send_packet(PC_TI9X, CMD_RDY, 2, NULL));

  return 0;
}

int ti89_send_SCR(void)
{
  printl2(0, " PC->TI: SCR\n");
  TRYF(send_packet(PC_TI9X, CMD_SCR, 2, NULL));

  return 0;
}

int ti89_send_CONT(void)
{
  printl2(0, " PC->TI: CONT\n");
  TRYF(send_packet(PC_TI9X, CMD_CONT, 2, NULL));

  return 0;
}

#include "externs.h"

int ti89_send_KEY(uint16_t scancode)
{
  printl2(0, " PC->TI: KEY\n");
  TRYF(cable->put(PC_TI9X));
  TRYF(cable->put(CMD_KEY));
  TRYF(cable->put(LSB(scancode)));
  TRYF(cable->put(MSB(scancode)));

  return 0;
}

int ti89_send_EOT(void)
{
  printl2(0, " PC->TI: EOT\n");
  TRYF(send_packet(PC_TI9X, CMD_EOT, 2, NULL));

  return 0;
}

int ti89_send_REQ(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32] = { 0 };
  uint16_t len;

  printl2(0, " PC->TI: REQ (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, varname);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));
  buffer[6 + strlen(varname)] = 0x00;

  len = 6 + strlen(varname) + 1;
  if (vartype != TI89_CLK)
    len--;
  TRYF(send_packet(PC_TI9X, CMD_REQ, len, buffer));

  return 0;
}

int ti89_send_RTS(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32] = { 0 };
  uint16_t len;

  printl2(0, " PC->TI: RTS (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, varname);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));
  buffer[6 + strlen(varname)] = 0x00;

  len = 6 + strlen(varname) + 1;
  // used by AMS <= 2.09 ?
  //if ((vartype == TI89_AMS) || (vartype == TI89_APPL)) len--;
  TRYF(send_packet(PC_TI9X, CMD_RTS, len, buffer));

  return 0;
}

int ti89_send_RTS2(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32] = { 0 };
  uint16_t len;

  printl2(0, " PC->TI: RTS (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, varname);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = 0x00;
  buffer[6] = 0x08;
  buffer[7] = 0x00;
  buffer[8] = 0x09;

  len = 9;
  TRYF(send_packet(PC_TI9X, CMD_RTS, len, buffer));

  return 0;
}


/* Variable (std var header: variable length) */
int ti89_recv_VAR(uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[32] = { 0 };
  uint16_t length;
  uint8_t strl;
  uint8_t flag;

  printl2(0, " TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8) |
      (buffer[2] << 16) | (buffer[3] << 24);
  *vartype = buffer[4];
  strl = buffer[5];
  memcpy(varname, buffer + 6, strl);
  varname[strl] = '\0';
  flag = buffer[6 + strl];

  if ((length != (6 + strlen(varname))) &&
      (length != (7 + strlen(varname))))
    return ERR_INVALID_PACKET;

  printl2(0, " (size=0x%08X=%i, id=%02X, name=<%s>, flag=%i)",
	  *varsize, *varsize, *vartype, varname, flag);
  printl2(0, ".\n");

  return 0;
}

int ti89_recv_CTS(void)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];

  printl2(0, " TI->PC: CTS");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;
  if (length != 0x0000)
    return ERR_CTS_ERROR;
  printl2(0, ".\n");

  return 0;
}

int ti89_recv_SKIP(uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];
  *rej_code = 0;

  printl2(0, " TI->PC: SKIP");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_CTS) {
    printl2(0, "CTS");
    return 0;
  }
  if (cmd != CMD_SKIP)
    return ERR_INVALID_CMD;
  printl2(0, " (rejection code = %i)", *rej_code = buffer[0]);
  printl2(0, ".\n");

  return 0;
}

int ti89_recv_XDP(uint32_t * length, uint8_t * data)
{
  uint8_t host, cmd;
  int err;

  printl2(0, " TI->PC: XDP");
  err = recv_packet(&host, &cmd, (uint16_t *) length, data);
  fixup(*length);
  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;
  printl2(0, " (%04X=%i bytes)", *length, *length);
  printl2(0, ".\n");
  TRYF(err);

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti89_recv_ACK(uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];

  printl2(0, " TI->PC: ACK");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  if (status != NULL)
    *status = length;
  else if (length != 0x0000)
    return ERR_NACK;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  printl2(0, ".\n");

  return 0;
}

int ti89_recv_CONT(void)
{
  uint8_t host, cmd;
  uint16_t sts;


  printl2(0, " TI->PC: CONT");
  TRYF(recv_packet(&host, &cmd, &sts, NULL));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd != CMD_CONT)
    return ERR_INVALID_CMD;
  printl2(0, ".\n");

  return 0;
}

int ti89_recv_EOT(void)
{
  uint8_t host, cmd;
  uint16_t length;

  printl2(0, " TI->PC: EOT");
  TRYF(recv_packet(&host, &cmd, &length, NULL));
  if (cmd != CMD_EOT)
    return ERR_INVALID_CMD;
  printl2(0, ".\n");

  return 0;
}

int ti89_recv_RTS(uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[32] = { 0 };
  uint16_t length;
  uint8_t strl;

  printl2(0, " TI->PC: RTS");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
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

  printl2(0, " (size=0x%08X=%i, id=%02X, name=<%s>)",
	  *varsize, *varsize, *vartype, varname);
  printl2(0, ".\n");

  return 0;
}
