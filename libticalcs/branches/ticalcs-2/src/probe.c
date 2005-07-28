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
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "dbus_pkt.h"
#include "error.h"
#include "gettext.h"
#include "pause.h"

#define DEAD_TIME	250

/* 
	Get the first byte sent by the calc (Machine ID)
*/
int tixx_recv_ACK(CalcHandle* handle, uint8_t* mid)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t buffer[5];

	TRYF(dbus_recv_2(handle, &host, &cmd, &length, buffer));
	ticalcs_info(" TI->PC: ACK");

	*mid = host;

	if (cmd == CMD_SKP)
		return ERR_VAR_REJECTED;

	return 0;
}

/**
 * ticalcs_probe_calc_2:
 * @handle: a previously allocated handle
 * @type: the calculator model
 *
 * This function tries and detect the calculator type for non-silent models
 * by requesting a screedump and analyzing the Machine ID. 
 * It supposes your calc is on and plugged.
 * 
 * PC: 08  6D 00 00		PC request a screen dump
 * TI: MId 56 00 00		TI reply OK
 *
 * Beware: the call sequence is very important: 86, 85, 83, 82 !!!
 *
 * Return value: 0 if ready else an error code.
 **/
TIEXPORT int TICALL ticalcs_probe_calc_2(CalcHandle* handle, CalcModel* model)
{
	CalcHandle* h = handle;
	int err;
	uint8_t data;

	ticalcs_info(_("Probing calculator...\n"));

	/* Test for a TI86 before a TI85 */
	ticalcs_info(_("Check for TI86... "));
	TRYF(dbus_send(h, PC_TI86, CMD_SCR, 2, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI86, data);

	if (!err && (data == TI86_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI86;

		return 0;
	} 
	else 
	{
		ticalcs_info("NOK.\n");
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);
	}

	/* Test for a TI85 */
	ticalcs_info(_("Check for TI85... "));
	TRYF(dbus_send(h, PC_TI85, CMD_SCR, 2, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI85, data);

	if (!err && (data == TI85_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI85;

		return 0;
	} 
	else 
	{
		ticalcs_info("NOK.\n");
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);
	}

	/* Test for a TI83 before a TI82 */
	ticalcs_info(_("Check for TI83... "));
	TRYF(dbus_send(h, PC_TI83, CMD_SCR, 2, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI83, data);

	if (!err && (data == TI83_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI83;

		return 0;
	} 
	else 
	{
		ticalcs_info("NOK.\n");
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);
	}

	/* Test for a TI82 */
	ticalcs_info(_("Check for TI82... "));
	TRYF(dbus_send(h, PC_TI83, CMD_SCR, 2, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI82, data);

	if (!err && (data == TI82_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI82;

		return 0;
	} 
	else 
	{
		ticalcs_info("NOK.\n");
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);
	}

	return 0;
}

/**
 * ticalcs_probe_calc_1:
 * @handle: a previously allocated handle
 * @type: the calculator model
 *
 * Check if the calculator is ready and detect the type.
 * Works only with FLASH calculators and a AMS2.08 (?!) mini by requesting
 * version. A previous version was based on MID but TI83+/84+, TI89/TI89t, TI92+/V200 
 * could not be distinguished ;-(
 *
 * Return value: 0 if ready else an error code.
 **/
TIEXPORT int TICALL ticalcs_probe_calc_1(CalcHandle* handle, CalcModel* model)
{
	uint8_t host, cmd;
	uint16_t status;
	uint8_t buffer[256];
	int i, err;

	// init value
	*model = CALC_NONE;

	// test for FLASH hand-helds (00 68 00 00 -> XX 56 00 00)
	// where XX is 0x98: TI89/89t, 0x88: TI92+/V200, 0x73: TI83+/84+, 0x74: TI73
	ticalcs_info(_("Check for TIXX... "));
	for(i = 0; i < 2; i++)
	{		
		ticalcs_info(" PC->TI: RDY?");
		err = dbus_send(handle, PC_TIXX, CMD_RDY, 2, NULL);
		if(err) continue;

		err = dbus_recv_2(handle, &host, &cmd, &status, buffer);
		ticalcs_info(" TI->PC: ACK");
		if(err) continue;

		break;
	}
	
	// test for TI92 (09 68 00 00 -> 89 56 00 00)
	if(err)
	{
		ticalcs_info(_("Check for TI92... "));
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);	// needed !

		for(i = 0; i < 2; i++)
		{
			ticalcs_info(" PC->TI: RDY?");
			err = dbus_send(handle, PC_TI92, CMD_RDY, 2, NULL);
			if(err) continue;

			err = dbus_recv_2(handle, &host, &cmd, &status, buffer);
			ticalcs_info(" TI->PC: ACK");
			if(err) continue;

			break;
		}

		if(!err)
			*model = CALC_TI92;
	}

	if (cmd != CMD_ACK)
		return ERR_INVALID_CMD;

	if ((status & 1) != 0)
		return ERR_NOT_READY;
	
	// test for TI9x FLASH hand-helds again (request version and analyze HW_ID)
	if(!err && (host != TI73_PC) && (host != TI83p_PC))
	{
		CalcInfos infos;

		ticalcs_info(_("Check for TI9X... "));
		handle->model = CALC_TI89;
		memset(&infos, 0, sizeof(CalcInfos));
		TRYF(ticalcs_calc_get_version(handle, &infos));

		switch(infos.hw_id)
		{
		case 1: *model = CALC_TI92P; break;
		case 3: *model = CALC_TI89;  break;
		case 8: *model = CALC_V200; break;
		case 9: *model = CALC_TI89T; break;
		}
	}
	else
	{
		ticalcs_info(_("Check for TI8X... "));
		switch (host) 
		{
		case TI83p_PC: *model = CALC_TI83P; break;
		case TI73_PC:  *model = CALC_TI73;  break;
		default: break;
		}
	}

	ticalcs_info(_("Calculator type: %s"), tifiles_model_to_string(*model));

	return 0;
}

extern const CalcUpdate default_update;

/**
 * ticalcs_probe_calc:
 * @cable: a link cable handle
 * @model: the calculator model which have been detected
 *
 * This function attempts to detect the calculator model plugged onto the cable.
 * It works in a heuristic fashion.
 *
 * Return value: 0 if ready else an error code.
 **/
TIEXPORT int TICALL ticalcs_probe_calc  (CableHandle* cable, CalcModel* model)
{
	CalcHandle calc;
	int err = 0;

	// we construct the structure here because we don't really need it
	calc.model = *model = CALC_NONE;
	calc.updat = (CalcUpdate *)&default_update;
	calc.priv2 = (uint8_t *)malloc(65536 + 4);
	calc.cable = cable;

	// first: search for FLASH hand-helds (fast)
	err = ticalcs_probe_calc_1(&calc, model);
	if(!err && (*model != CALC_NONE))
	{
		free(calc.priv2);
		return 0;
	}

	// second: search for other calcs (slow)
	err = ticalcs_probe_calc_2(&calc, model);
	if(err)
	{
		free(calc.priv2);
		return err;
	}

	free(calc.priv2);
	return 0;
}
