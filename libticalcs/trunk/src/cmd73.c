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
  This unit handles TI73 & TI83+ & TI84+ commands.
*/

#include <stdio.h>
#include <string.h>

#include "headers.h"
#include "packets.h"
#include "calc_err.h"
#include "update.h"
#include "printl.h"

#ifdef __WIN32__
#pragma warning( disable : 4761 )
#endif

// Shares some commands between TI73 & 83+ & 84+
extern int ticalcs_calc_type;
#define PC_TI7383 ((ticalcs_calc_type == CALC_TI73) ? PC_TI73 : PC_TI83p)
#define TI7383_BKUP ((ticalcs_calc_type == CALC_TI73) ? TI73_BKUP : TI83p_BKUP)
#define EXTRAS ((ticalcs_calc_type == CALC_TI83P) || (ticalcs_calc_type == CALC_TI84P) ? 2 : 0)

/* Variable (std var header: NUL padded, fixed length) */
int ti73_send_VAR(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr)
{
  uint8_t buffer[16];

  DISPLAY
      (" PC->TI: VAR (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)\n",
       varsize, varsize, vartype, varname, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != TI7383_BKUP) {	// backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(PC_TI7383, CMD_VAR, 11 + EXTRAS, buffer));
  } else {
    TRYF(send_packet(PC_TI7383, CMD_VAR, 9, buffer));
  }

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_send_VAR2(uint32_t length, uint8_t type, uint8_t flag,
		   uint16_t offset, uint16_t page)
{
  uint8_t buffer[11];

  DISPLAY
      (" PC->TI: VAR (size=0x%04X=%i, id=%02X, flag=%02X, offset=%04X, page=%02X)\n",
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

  TRYF(send_packet(PC_TI7383, CMD_VAR, 10, buffer));

  return 0;
}

int ti73_send_CTS(void)
{
  printl2(0, " PC->TI: CTS\n");
  TRYF(send_packet(PC_TI7383, CMD_CTS, 0, NULL));

  return 0;
}

int ti73_send_XDP(int length, uint8_t * data)
{
  printl2(0, " PC->TI: XDP (0x%04X = %i bytes)\n", length, length);
  TRYF(send_packet(PC_TI7383, CMD_XDP, length, data));

  return 0;
}

int ti73_send_FLSH(void)
{
  printl2(0, " PC->TI: FLASH app is following...\n");
  TRYF(send_packet(PC_TI7383, CMD_FLSH, 2, NULL));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti73_send_SKIP(uint8_t rej_code)
{
  printl2(0, " PC->TI: SKIP\n");
  TRYF(send_packet(PC_TI7383, CMD_SKIP, 1, &rej_code));
  printl2(0, " (rejection code = %i)\n", rej_code);

  return 0;
}

int ti73_send_ACK(void)
{
  printl2(0, " PC->TI: ACK\n");
  TRYF(send_packet(PC_TI7383, CMD_ACK, 2, NULL));

  return 0;
}

int ti73_send_ERR(void)
{
  printl2(0, " PC->TI: ERR\n");
  TRYF(send_packet(PC_TI7383, CMD_ERR, 2, NULL));

  return 0;
}

int ti73_send_RDY(void)
{
  printl2(0, " PC->TI: RDY?\n");
  TRYF(send_packet(PC_TI7383, CMD_RDY, 2, NULL));

  return 0;
}

int ti73_send_SCR(void)
{
  printl2(0, " PC->TI: SCR\n");
  TRYF(send_packet(PC_TI7383, CMD_SCR, 2, NULL));

  return 0;
}

#include "externs.h"

int ti73_send_KEY(uint16_t scancode)
{
  printl2(0, " PC->TI: KEY\n");
  TRYF(cable->put(PC_TI7383));
  TRYF(cable->put(CMD_KEY));
  TRYF(cable->put(LSB(scancode)));
  TRYF(cable->put(MSB(scancode)));

  return 0;
}

int ti73_send_EOT(void)
{
  printl2(0, " PC->TI: EOT\n");
  TRYF(send_packet(PC_TI7383, CMD_EOT, 2, NULL));

  return 0;
}

/* Request variable (std var header: NUL padded, fixed length) */
int ti73_send_REQ(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr)
{
  uint8_t buffer[16] = { 0 };
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  DISPLAY
      (" PC->TI: REQ (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)\n",
       varsize, varsize, vartype, trans, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  pad_buffer(buffer + 3, '\0');
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != TI83p_IDLIST) {
    TRYF(send_packet(PC_TI7383, CMD_REQ, 11 + EXTRAS, buffer));
  } else {
    TRYF(send_packet(PC_TI7383, CMD_REQ, 11, buffer));
  }

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_send_REQ2(uint16_t appsize, uint8_t apptype, char *appname,
		   uint8_t appattr)
{
  uint8_t buffer[16] = { 0 };

  printl2(0, " PC->TI: REQ (size=0x%04X=%i, id=%02X, name=<%s>)\n",
	  appsize, appsize, apptype, appname);

  buffer[0] = LSB(appsize);
  buffer[1] = MSB(appsize);
  buffer[2] = apptype;
  memcpy(buffer + 3, appname, 8);
  pad_buffer(buffer + 3, '\0');

  TRYF(send_packet(TI83p_PC, CMD_REQ, 11, buffer));	// TI_PC73 !

  return 0;
}

/* Request to send (std var header: NUL padded, fixed length) */
int ti73_send_RTS(uint16_t varsize, uint8_t vartype, char *varname,
		  uint8_t varattr)
{
  uint8_t buffer[16];
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  DISPLAY
      (" PC->TI: RTS (size=0x%04X=%i, id=%02X, name=<%s>, attr=%i)\n",
       varsize, varsize, vartype, trans, varattr);

  buffer[0] = LSB(varsize);
  buffer[1] = MSB(varsize);
  buffer[2] = vartype;
  memcpy(buffer + 3, varname, 8);
  buffer[11] = 0x00;
  buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

  if (vartype != TI7383_BKUP) {	// backup: special header
    pad_buffer(buffer + 3, '\0');
    TRYF(send_packet(PC_TI7383, CMD_RTS, 11 + EXTRAS, buffer));
  } else {
    TRYF(send_packet(PC_TI7383, CMD_RTS, 9, buffer));
  }

  return 0;
}

int ti73_recv_VAR(uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr)
{
  uint8_t host, cmd;
  uint8_t buffer[65536+6] = { 0 };
  uint16_t length;
  uint8_t trans[9];

  printl2(0, " TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
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

  tifiles_translate_varname(varname, trans, *vartype);
  printl2(0, " (size=0x%04X=%i, id=%02X, name=<%s>, attrb=%i)",
	  *varsize, *varsize, *vartype, trans, *varattr);
  printl2(0, ".\n");

  return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int ti73_recv_VAR2(uint16_t * length, uint8_t * type, char *name,
		   uint16_t * offset, uint16_t * page)
{
  uint8_t host, cmd;
  uint8_t buffer[65536+6] = { 0 };
  uint16_t len;

  printl2(0, " TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &len, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
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

  DISPLAY
      (" (size=0x%04X=%i, type=%02X, name=<%s>, offset=%04X, page=%02X)\n",
       *length, *length, *type, name, *offset, *page & 0xff);

  return 0;
}

int ti73_recv_CTS(uint16_t length)
{
  uint8_t host, cmd;
  uint16_t len;
  uint8_t buffer[65536+6];

  printl2(0, " TI->PC: CTS");
  TRYF(recv_packet(&host, &cmd, &len, buffer));
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;
  if (length != len)
    return ERR_CTS_ERROR;
  printl2(0, ".\n");

  return 0;
}

int ti73_recv_SKIP(uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[65536+6];
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

int ti73_recv_XDP(uint16_t * length, uint8_t * data)
{
  uint8_t host, cmd;

  printl2(0, " TI->PC: XDP");
  TRYF(recv_packet(&host, &cmd, length, data));
  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;
  printl2(0, " (%04X=%i bytes)", *length, *length);
  printl2(0, ".\n");

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti73_recv_ACK(uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[65536+6];

  printl2(0, " TI->PC: ACK");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (status != NULL)
    *status = length;
  else if (length != 0x0000)
    return ERR_NACK;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  printl2(0, ".\n");

  return 0;
}

int ti73_recv_RTS(uint16_t * varsize, uint8_t * vartype, char *varname,
		  uint8_t * varattr)
{
  uint8_t host, cmd;
  uint8_t buffer[65536+6];
  uint8_t trans[9];


  printl2(0, " TI->PC: RTS");
  TRYF(recv_packet(&host, &cmd, varsize, buffer));
  if (cmd != CMD_RTS)
    return ERR_INVALID_CMD;
  *varsize = buffer[0] | (buffer[1] << 8);
  *vartype = buffer[2];
  memcpy(varname, buffer + 3, 8);
  varname[8] = '\0';
  *varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

  tifiles_translate_varname(varname, trans, *vartype);
  printl2(0, " (size=0x%04X=%i, id=%02X, name=<%s>, attrb=%i)",
	  *varsize, *varsize, *vartype, trans, *varattr);
  printl2(0, ".\n");

  return 0;
}
