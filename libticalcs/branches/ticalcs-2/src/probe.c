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
	Probing support.
*/

#include <stdio.h>

#include "ticalcs.h"
#include "logging.h"
#include "packets.h"
#include "error.h"
#include "gettext.h"

/* 
	Get the first byte sent by the calc (Machine ID)
*/
#if 0
int tixx_recv_ACK(uint8_t* mid)
{
	uint8_t host, cmd;
	uint16_t status;
	uint8_t buffer[256];

	TRYF(recv_packet(handle, &host, &cmd, &status, buffer));
	ticalcs_info(" TI->PC: ACK");

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
#endif

/**
 * ticalcs_calc_isready_with_model:
 * @handle: a previously allocated handle
 * @type: the calculator model
 *
 * Check if the calculator is ready and detect the type.
 * Works only with TI73/83+/89/92+ calculators (FLASH). It could work with an 
 * V200 but it returns the same ID as TI92+...
 * Practically, call this function first, and call ticalcs_calc_isready next.
 *
 * Return value: 0 if ready else an error code.
 **/
TIEXPORT int TICALL ticalcs_calc_isready_with_model(CalcHandle* handle, CalcModel* model)
{
	uint8_t host, cmd;
	uint16_t status;
	uint8_t buffer[256];

	if(!tifiles_is_flash(handle->model))
	{
		*model = CALC_NONE;
		return 0;
	}

	ticalcs_info(" PC->TI: RDY?");
	TRYF(send_packet(handle, PC_TIXX, CMD_RDY, 2, NULL));

	TRYF(recv_packet(handle, &host, &cmd, &status, buffer));
	ticalcs_info(" TI->PC: ACK");

	// 0x98: TI89, 0x88: TI92+/V200, 0x73: TI83+, 0x74: TI73
	switch (host) 
	{
		/*
    case V200_PC:  
		*model = CALC_V200; 
		break;*/
	case TI92p_PC:
		*model = CALC_TI92P;
    break;
	case TI89_PC:
		*model = CALC_TI89;
    break;
	case TI83p_PC:
		*model = CALC_TI83P;
    break;
	case TI73_PC:
		*model = CALC_TI73;
    break;
	default:
		*model = CALC_NONE;
    return ERR_INVALID_HOST;
    break;
	}

	if (cmd != CMD_ACK)
		return ERR_INVALID_CMD;

	if ((status & 1) != 0)
		return ERR_NOT_READY;

	ticalcs_info(_("Calculator type: %s"),
	  (*model == CALC_TI83P) ? "TI83+" :
	  (*model == CALC_TI84P) ? "TI84+" :
	  (*model == CALC_TI89) ? "TI89" :
	  (*model == CALC_TI89T) ? "TI89t" :
	  (*model == CALC_TI92P) ? "TI92+" :
	  (*model == CALC_V200) ? "V200" : "???");

  return 0;
}
