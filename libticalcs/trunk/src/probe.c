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
	Probing support.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "dbus_pkt.h"
#include "error.h"
#include "gettext.h"
#include "pause.h"
#include "calc_xx.h"
#include "dusb_vpkt.h"
#include "dusb_cmd.h"

#define DEAD_TIME	250

/* 
	Get the first byte sent by the calc (Machine ID)
*/
static int tixx_recv_ACK(CalcHandle* handle, uint8_t* mid)
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
 * Beware: the call sequence is very important: 86, 85, 73, 83, 82 !!!
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
static int ticalcs_probe_calc_2(CalcHandle* handle, CalcModel* model)
{
	CalcHandle* h = handle;
	int err;
	uint8_t data;

	ticalcs_info(_("Probing calculator...\n"));
	*model = CALC_NONE;

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

	/* Test for a TI73 before a TI83 */
	ticalcs_info(_("Check for TI73... "));
	TRYF(dbus_send(h, PC_TI73, CMD_SCR, 2, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI73, data);

	if (!err && (data == TI73_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI73;

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

	/* Test for a TI80 */
#if 0
#warning TI-80 DETECTION FAILS
	ticalcs_info(_("Check for TI80... "));
	ticalcs_info("%d\n", dbus_send(h, PC_TI80, CMD_SCR, 0, NULL));
	err = tixx_recv_ACK(h, &data);

	ticalcs_info("<%02X-%02X> ", PC_TI80, data);

	if (!err && (data == TI80_PC)) 
	{
		ticalcs_info("OK !\n");
		*model = CALC_TI80;

		return 0;
	} 
	else 
	{
		ticalcs_info("NOK.\n");
		ticables_cable_reset(handle->cable);
		PAUSE(DEAD_TIME);
	}
#endif

	return (*model == CALC_NONE) ? ERR_NO_CALC : 0;
}

/**
 * ticalcs_probe_calc_1:
 * @handle: a previously allocated handle
 * @type: the calculator model
 *
 * Check if the calculator is ready and detect the type.
 * Works only on FLASH calculators with an AMS2.08 or OS2.00 by requesting the
 * version. A previous version was based on MID but TI83+/84+, TI89/TI89t, TI92+/V200 
 * could not be distinguished ;-(
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
static int ticalcs_probe_calc_1(CalcHandle* handle, CalcModel* model)
{
	uint8_t host, cmd;
	uint16_t status;
	uint8_t buffer[256];
	int i, err;
	CalcInfos infos;

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

	// test for TI73
	if(!err)
	{
		if(host == TI73_PC)
		{
			*model = CALC_TI73;
			return 0;
		}
		else if(host == TI92_PC)
		{
			*model = CALC_TI92;
			return 0;
		}
	}
	
	// test for TI92 (09 68 00 00 -> 89 56 00 00)
	else if(err)
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
		ticalcs_info(_("Check for TI9X... "));

		handle->model = CALC_TI89;
		handle->calc = (CalcFncts *)&calc_89;

		memset(&infos, 0, sizeof(CalcInfos));		
		TRYF(ticalcs_calc_get_version(handle, &infos));
		*model = infos.model;
	}
	else
	{
		ticalcs_info(_("Check for TI8X... "));

		handle->model = CALC_TI83P;
		handle->calc = (CalcFncts *)&calc_83p;

		memset(&infos, 0, sizeof(CalcInfos));
		TRYF(ticalcs_calc_get_version(handle, &infos));
		*model = infos.model;
	}

	ticalcs_info(_("Calculator type: %s"), tifiles_model_to_string(*model));

	return (*model == CALC_NONE) ? ERR_NO_CALC : 0;
}

extern const CalcUpdate default_update;

/**
 * ticalcs_probe_calc:
 * @cable: a valid (=opened/attached) link cable handle
 * @model: the calculator model which have been detected
 *
 * This function attempts to detect the calculator model plugged onto the cable.
 * It works in a heuristic fashion.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_probe_calc (CableHandle* cable, CalcModel* model)
{
	CalcHandle calc;
	int err = 0;

	if (cable == NULL)
	{
		return ERR_INVALID_HANDLE;
	}
	if (model == NULL)
	{
		ticalcs_critical("ticalcs_probe_calc: model is NULL");
		return -1;
	}

	// Hack: we construct the structure here because we don't really need it.
	// I want to use ticalcs functions with a non-fixed calculator
	memset(&calc, 0, sizeof(CalcHandle));
	calc.model = *model = CALC_NONE;
	calc.updat = (CalcUpdate *)&default_update;
	calc.priv2 = (uint8_t *)g_malloc(65536 + 4);
	calc.cable = cable;
	calc.open = !0;

	// first: search for FLASH hand-helds (fast)
	err = ticalcs_probe_calc_1(&calc, model);
	if(!err && (*model != CALC_NONE))
	{
		g_free(calc.priv2);
		return 0;
	}

	// second: search for other calcs (slow)
	err = ticalcs_probe_calc_2(&calc, model);
	if(err)
	{
		g_free(calc.priv2);
		return err;
	}

	g_free(calc.priv2);
	return (*model == CALC_NONE) ? ERR_NO_CALC : 0;
}

/**
 * ticalcs_probe_usb_calc:
 * @cable: a valid (=opened/attached) link cable handle
 * @model: the calculator model which have been detected
 *
 * This function attempts to detect the calculator model plugged onto the cable.
 * It works in a heuristic fashion and with FLASH hand-helds only.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_probe_usb_calc(CableHandle* cable, CalcModel* model)
{
	CalcHandle calc;
	int err = 0;
	int ret = ERR_NO_CALC;

	if (cable == NULL)
	{
		return ERR_INVALID_HANDLE;
	}
	if (model == NULL)
	{
		ticalcs_critical("ticalcs_probe_calc: model is NULL");
		return -1;
	}

	// Hack: we construct the structure here because we don't really need it.
	// I want to use ticalcs functions with a non-fixed calculator
	memset(&calc, 0, sizeof(CalcHandle));
	calc.model = *model = CALC_NONE;
	calc.updat = (CalcUpdate *)&default_update;
	calc.priv2 = (uint8_t *)g_malloc(65536 + 4);
	calc.cable = cable;
	calc.open = !0;

	if(cable->model == CABLE_SLV)
	{
		err = ticalcs_probe_calc_1(&calc, model);
		if(!err && (*model != CALC_NONE))
			ret = 0;
	}
	else if(cable->model == CABLE_USB)
	{
		int n, *list;
		int i;

		ticables_get_usb_devices(&list, &n);
		i = (int)cable->port-1 > n ? n-1 : (int)cable->port-1;
		switch(list[i])
		{
		case PID_TI89TM:   *model = CALC_TI89T_USB; ret = 0; break;
		case PID_TI84P:    *model = CALC_TI84P_USB; ret = 0; break;
		case PID_TI84P_SE: *model = CALC_TI84P_USB; ret = 0; break;
		case PID_NSPIRE:   *model = CALC_NSPIRE; ret = 0; break;
		default: break;
		}
	}

	g_free(calc.priv2);
	return ret;
}

/**
 * ticalcs_probe:
 * @c_model: link cable model
 * @c_port: link cable port
 * @model: hand-held model detected/found [out]
 * @all: which hand-helds to detect (FLASH only or all)
 *
 * This function attempts to detect the calculator model plugged onto a
 * given link cable model/port. It works in a heuristic fashion.
 * This function handles device opening/closing for you.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_probe(CableModel c_model, CablePort c_port, CalcModel* model, int all)
{
	CableHandle *handle;
	int err = 0;
	CalcHandle calc;

	if (model == NULL)
	{
		ticalcs_critical("ticalcs_probe_calc: model is NULL");
		return -1;
	}

	// get handle
	handle = ticables_handle_new(c_model, c_port);
	ticables_options_set_timeout(handle, 10);

	// hack: we construct the structure here because we don't really need it.
	// I want to use ticalcs functions with a non-fixed calculator
	memset(&calc, 0, sizeof(CalcHandle));
	calc.model = *model = CALC_NONE;
	calc.updat = (CalcUpdate *)&default_update;
	calc.priv2 = (uint8_t *)g_malloc(65536 + 4);
	calc.cable = handle;
	calc.open = !0;

	// open cable
	err = ticables_cable_open(handle);
	if(err)
	{
		ticables_handle_del(handle);
		return err;
	}

	// probe
	if(c_model == CABLE_USB)
		err = ticalcs_probe_usb_calc(handle, model);
	else
	{
		if(all)
			err = ticalcs_probe_calc(handle, model);
		else
			err = ticalcs_probe_calc_1(&calc, model);
	}

	if(err)
	{
		ticables_cable_close(handle);
		ticables_handle_del(handle);
		return err;
	}

	// close
	ticables_cable_close(handle);
	ticables_handle_del(handle);

	return err;
}
