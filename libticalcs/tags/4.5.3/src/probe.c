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
  This unit provides probing support.
*/

#include <stdio.h>

#include "intl2.h"
#include "export.h"
#include "headers.h"
#include "calc_err.h"
#include "calc_int.h"
#include "externs.h"
#include "packets.h"
#include "update.h"
#include "printl.h"

/* 
   Get the first uint8_t sent by the calc (Machine ID)
*/
int tixx_recv_ACK(uint8_t * mid)
{
  uint8_t host, cmd;
  uint16_t status;

  printl2(0, " TI->PC: ACK");
  TRYF(recv_packet(&host, &cmd, &status, NULL));

  *mid = host;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  return 0;
}


/* 
   !!! Obsolete !!!
   This function try to detect the calculator type by requesting a screedump
   and analyzing the Machine ID uint8_t. It supposes that the communication port 
   is correctly initialized and your calc is on.

   PC: 08  6D 00 00		PC request a screen dump
   TI: MId 56 00 00		TI reply OK
   
   Beware: the call sequence is very important: 89, 92+, 92, 86, 85, 83, 82 !!!
*/
TIEXPORT int TICALL ticalc_detect_calc(TicalcType * calc_type)
{
  int err;
  uint8_t data;

  printl2(0, _("Probing calculator...\n"));

  /* Test for a TI 89 or a TI92+ */
  printl2(0, _("Trying TI89/TI92+... "));
  TRYF(cable->open());

  printl2(0, " PC->TI: SCR\n");
  TRYF(send_packet(PC_TI89, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X/%02X> ", PC_TI89, data);
  TRYF(cable->close());

  if (!err && (data == TI89_PC)) {
    printl2(0, "OK (TI89) !\n");
    *calc_type = CALC_TI89;

    return 0;
  } else if (!err && (data == TI92p_PC)) {
    printl2(0, "OK (TI92+) !\n");
    *calc_type = CALC_TI92P;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }

  /* Test for a TI92 */
  printl2(0, _("Trying TI92... "));
  TRYF(send_packet(PC_TI92, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X/%02X> ", PC_TI92, data);
  TRYF(cable->close());

  if (!err && (data == TI92_PC)) {
    printl2(0, "OK !\n");
    *calc_type = CALC_TI92;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }

  /* Test for a TI86 before a TI85 */
  printl2(0, _("Trying TI86... "));
  TRYF(cable->open());
  TRYF(send_packet(PC_TI86, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X/%02X> ", PC_TI86, data);
  TRYF(cable->close());

  if (!err && (data == TI86_PC)) {
    printl2(0, "OK !\n");
    *calc_type = CALC_TI86;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }

  /* Test for a TI85 */
  printl2(0, _("Trying TI85... "));
  TRYF(cable->open());
  TRYF(send_packet(PC_TI85, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X/%02X> ", PC_TI85, data);
  TRYF(cable->close());

  if (!err && (data == TI85_PC)) {
    printl2(0, "OK !\n");
    *calc_type = CALC_TI85;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }

  /* Test for a TI83 before a TI82 */
  printl2(0, _("Trying TI83... "));
  TRYF(cable->open());
  TRYF(send_packet(PC_TI83, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X/%02X> ", PC_TI82, data);
  TRYF(cable->close());

  if (!err && (data == TI83_PC)) {
    printl2(0, "OK !\n");
    *calc_type = CALC_TI83;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }

  /* Test for a TI82 */
  printl2(0, _("Trying TI82... "));
  TRYF(cable->open());
  TRYF(send_packet(PC_TI83, CMD_SCR, 2, NULL));
  err = tixx_recv_ACK(&data);

  printl2(0, "<%02X> ", data);
  TRYF(cable->close());

  if (!err && (data == TI82_PC)) {
    printl2(0, "OK !\n");
    *calc_type = CALC_TI82;

    return 0;
  } else {
    printl2(0, "NOK.\n");
  }
  /* Next calc */

  return 0;
}


/*
  Check if the calculator is ready and detect the type.
  Works only with TI73/83+/89/92+ calculators (FLASH). It could work with an 
  V200 but it returns the same ID as TI92+...
  Practically, call this function first, and call tixx_isready next.
  Return 0 if successful, 0 otherwise.
*/
TIEXPORT int TICALL ticalc_isready(TicalcType * calc_type)
{
  uint8_t host, cmd, st1, st2;
  uint16_t status;
  TicalcType ct;

  ticalc_get_calc(&ct);
  if ((ct != CALC_TI89) && (ct != CALC_TI92P) &&
      (ct != CALC_TI73) && (ct != CALC_TI83P) && (ct != CALC_V200))
    return 0;

  TRYF(cable->open());
  printl2(0, _("Is calculator ready (and check type) ?\n"));

  printl2(0, " PC->TI: RDY?\n");
  TRYF(send_packet(PC_TIXX, CMD_RDY, 2, NULL));

  printl2(0, " TI->PC: ACK");
  TRYF(cable->get(&host));
  TRYF(cable->get(&cmd));
  TRYF(cable->get(&st1));
  TRYF(cable->get(&st2));
  status = (st1 << 8) | st2;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;
  printl2(0, _("\nStatus = %04X\n"), status);

  // 0x98: TI89, 0x88: TI92+/V200, 0x73: TI83+, 0x74: TI73
  switch (host) {
    //case V200_PC:  *calc_type = CALC_V200; break;
  case TI92p_PC:
    *calc_type = CALC_TI92P;
    break;
  case TI89_PC:
    *calc_type = CALC_TI89;
    break;
  case TI83p_PC:
    *calc_type = CALC_TI83P;
    break;
  case TI73_PC:
    *calc_type = CALC_TI73;
    break;
  default:
    *calc_type = CALC_NONE;
    return ERR_INVALID_HOST;
    break;
  }

  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;
  if ((status & 1) != 0)
    return ERR_NOT_READY;

  printl2(0, _("The calculator is ready.\n"));
  printl2(0, _("Calculator type: %s\n"),
	  (*calc_type == CALC_TI83P) ? "TI83+" :
	  (*calc_type == CALC_TI89) ? "TI89" :
	  (*calc_type == CALC_TI92P) ? "TI92+" :
	  (*calc_type == CALC_V200) ? "V200" : "???");

  return 0;
}
