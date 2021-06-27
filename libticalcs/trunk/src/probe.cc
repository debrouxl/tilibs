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
#include "calc_xx.h"
#include "dusb_vpkt.h"
#include "dusb_cmd.h"

#define DEAD_TIME	250

/* 
	Get the first byte sent by the calc (Machine ID)
*/
static int tixx_recv_ACK(CalcHandle* handle, uint8_t* mid)
{
	uint8_t host = 0, cmd = 0;
	uint16_t length = 0;
	uint8_t buffer[5];
	int ret;

	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (!ret)
	{
		ticalcs_info(" TI->PC: ACK");

		*mid = host;

		if (cmd == DBUS_CMD_SKP)
		{
			ret = ERR_VAR_REJECTED;
		}
	}

	return ret;
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
	int err;
	uint8_t data;

	do
	{
		ticalcs_info("%s", _("Probing calculator...\n"));
		*model = CALC_NONE;

		/* Test for a TI86 before a TI85 */
		ticalcs_info("%s", _("Check for TI86... "));
		err = dbus_send(handle, DBUS_MID_PC_TI86, DBUS_CMD_SCR, 2, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI86, data);

		if (!err && (data == DBUS_MID_TI86_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI86;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}

		/* Test for a TI85 */
		ticalcs_info("%s", _("Check for TI85... "));
		err = dbus_send(handle, DBUS_MID_PC_TI85, DBUS_CMD_SCR, 2, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI85, data);

		if (!err && (data == DBUS_MID_TI85_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI85;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}

		/* Test for a TI73 before a TI83 */
		ticalcs_info("%s", _("Check for TI73... "));
		err = dbus_send(handle, DBUS_MID_PC_TI73, DBUS_CMD_SCR, 2, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI73, data);

		if (!err && (data == DBUS_MID_TI73_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI73;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}

		/* Test for a TI83 before a TI82 */
		ticalcs_info("%s", _("Check for TI83... "));
		err = dbus_send(handle, DBUS_MID_PC_TI83, DBUS_CMD_SCR, 2, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI83, data);

		if (!err && (data == DBUS_MID_TI83_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI83;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}

		/* Test for a TI82 */
		ticalcs_info("%s", _("Check for TI82... "));
		err = dbus_send(handle, DBUS_MID_PC_TI83, DBUS_CMD_SCR, 2, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI82, data);

		if (!err && (data == DBUS_MID_TI82_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI82;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}

#if 0
		/* Test for a TI80 */
#pragma message("Warning: TI-80 DETECTION FAILS")
		ticalcs_info("%s", _("Check for TI80... "));
		err = dbus_send(handle, DBUS_MID_PC_TI80, DBUS_CMD_SCR, 0, NULL);
		if (err)
		{
			break;
		}
		err = tixx_recv_ACK(handle, &data);

		ticalcs_info("<%02X-%02X> ", DBUS_MID_PC_TI80, data);

		if (!err && (data == DBUS_MID_TI80_PC))
		{
			ticalcs_info("OK !\n");
			*model = CALC_TI80;
			break;
		}
		else
		{
			ticalcs_info("NOK.\n");
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);
		}
#endif
	} while(0);

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
	uint8_t host = 0, cmd = 0;
	uint16_t status = 0;
	uint8_t buffer[256];
	int i, ret;

	// init value
	*model = CALC_NONE;

	do
	{
		CalcInfos infos;

		// test for FLASH hand-helds (00 68 00 00 -> XX 56 00 00)
		// where XX is 0x98: TI89/89t, 0x88: TI92+/V200, 0x73: TI83+/84+, 0x74: TI73
		ticalcs_info("%s", _("Check for TIXX... "));
		for (i = 0; i < 2; i++)
		{
			ticalcs_info(" PC->TI: RDY?");
			ret = dbus_send(handle, DBUS_MID_PC_TIXX, DBUS_CMD_RDY, 2, NULL);
			if (ret)
			{
				continue;
			}

			ret = dbus_recv(handle, &host, &cmd, &status, buffer);
			ticalcs_info(" TI->PC: ACK");
			if (ret)
			{
				continue;
			}

			break;
		}

		// test for TI73
		if (!ret)
		{
			if (host == DBUS_MID_TI73_PC)
			{
				*model = CALC_TI73;
				break;
			}
			else if (host == DBUS_MID_TI92_PC)
			{
				*model = CALC_TI92;
				break;
			}
		}

		// test for TI92 (09 68 00 00 -> 89 56 00 00)
		else if (ret)
		{
			ticalcs_info("%s", _("Check for TI92... "));
			ticables_cable_reset(handle->cable);
			PAUSE(DEAD_TIME);	// needed !

			for (i = 0; i < 2; i++)
			{
				ticalcs_info(" PC->TI: RDY?");
				ret = dbus_send(handle, DBUS_MID_PC_TI92, DBUS_CMD_RDY, 2, NULL);
				if (ret)
				{
					continue;
				}

				ret = dbus_recv(handle, &host, &cmd, &status, buffer);
				ticalcs_info(" TI->PC: ACK");
				if (ret)
				{
					continue;
				}

				break;
			}

			if (!ret)
			{
				*model = CALC_TI92;
			}
		}

		if (cmd != DBUS_CMD_ACK)
		{
			ret = ERR_INVALID_CMD;
			break;
		}

		if ((status & 1) != 0)
		{
			ret = ERR_NOT_READY;
			break;
		}

		// test for TI9x FLASH hand-helds again (request version and analyze HW_ID)
		if(!ret && (host != DBUS_MID_TI73_PC) && (host != DBUS_MID_TI83p_PC))
		{
			ticalcs_info("%s", _("Check for TI9X... "));

			handle->model = CALC_TI89;
			handle->calc = (CalcFncts *)&calc_89;

			memset(&infos, 0, sizeof(CalcInfos));
			ret = ticalcs_calc_get_version(handle, &infos);
			if (ret)
			{
				break;
			}
			*model = infos.model;
		}
		else
		{
			ticalcs_info("%s", _("Check for TI8X... "));

			handle->model = CALC_TI83P;
			handle->calc = (CalcFncts *)&calc_83p;

			memset(&infos, 0, sizeof(CalcInfos));
			ret = ticalcs_calc_get_version(handle, &infos);
			if (ret)
			{
				break;
			}
			*model = infos.model;
		}
	} while(0);

	ticalcs_info(_("Calculator type: %s"), tifiles_model_to_string(*model));

	if (!ret)
	{
		ret = (*model == CALC_NONE) ? ERR_NO_CALC : 0;
	}

	return ret;
}

extern CalcUpdate default_update;

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
int TICALL ticalcs_probe_calc(CableHandle* cable, CalcModel* model)
{
	CalcHandle calc;
	int ret = 0;

	VALIDATE_NONNULL(cable);
	VALIDATE_NONNULL(model);

	do
	{
		// Hack: we construct the structure here because we don't really need it.
		// I want to use ticalcs functions with a non-fixed calculator
		memset(&calc, 0, sizeof(CalcHandle));
		calc.model = *model = CALC_NONE;
		calc.updat = (CalcUpdate *)&default_update;
		calc.buffer = (uint8_t *)g_malloc(65536 + 6);
		calc.buffer2 = (uint8_t *)g_malloc(65536 + 6);
		calc.cable = cable;
		calc.open = !0;

		// first: search for FLASH hand-helds (fast)
		ret = ticalcs_probe_calc_1(&calc, model);
		if (!ret && (*model != CALC_NONE))
		{
			g_free(calc.buffer2);
			g_free(calc.buffer);
			break;
		}

		// second: search for other calcs (slow)
		ret = ticalcs_probe_calc_2(&calc, model);
		g_free(calc.buffer2);
		g_free(calc.buffer);
	} while(0);

	if (!ret)
	{
		ret = (*model == CALC_NONE) ? ERR_NO_CALC : 0;
	}

	return ret;
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
int TICALL ticalcs_probe_usb_calc(CableHandle* cable, CalcModel* model)
{
	CalcHandle calc;
	int ret = ERR_NO_CALC;

	VALIDATE_NONNULL(cable);
	VALIDATE_NONNULL(model);

	do
	{
		// Hack: we construct the structure here because we don't really need it.
		// I want to use ticalcs functions with a non-fixed calculator
		memset(&calc, 0, sizeof(CalcHandle));
		calc.model = *model = CALC_NONE;
		calc.updat = (CalcUpdate *)&default_update;
		calc.buffer = (uint8_t *)g_malloc(65536 + 6);
		calc.buffer2 = (uint8_t *)g_malloc(65536 + 6);
		calc.cable = cable;
		calc.open = !0;

		if (cable->model == CABLE_SLV)
		{
			ret = ticalcs_probe_calc_1(&calc, model);
		}
		else if (cable->model == CABLE_USB)
		{
			CableDeviceInfo info;

			ret = ticables_cable_get_device_info(cable, &info);
			if (!ret)
			{
				*model = ticalcs_device_info_to_model(&info);
			}
		}

		g_free(calc.buffer2);
		g_free(calc.buffer);
	} while(0);

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
int TICALL ticalcs_probe(CableModel c_model, CablePort c_port, CalcModel* model, int all)
{
	CableHandle *handle;
	int ret = 0;

	VALIDATE_NONNULL(model);

	// get handle
	handle = ticables_handle_new(c_model, c_port);
	ticables_options_set_timeout(handle, 10);

	do
	{
		// open cable
		ret = ticables_cable_open(handle);
		if (ret)
		{
			ticables_handle_del(handle);
			break;
		}


		// probe
		if (c_model == CABLE_USB)
		{
			ret = ticalcs_probe_usb_calc(handle, model);
		}
		else
		{
			if (all)
			{
				ret = ticalcs_probe_calc(handle, model);
			}
			else
			{
				CalcHandle calc;
				// hack: we construct the structure here because we don't really need it.
				// I want to use ticalcs functions with a non-fixed calculator
				memset(&calc, 0, sizeof(CalcHandle));
				calc.model = *model = CALC_NONE;
				calc.updat = (CalcUpdate *)&default_update;
				calc.buffer = (uint8_t *)g_malloc(65536 + 6);
				calc.buffer2 = (uint8_t *)g_malloc(65536 + 6);
				calc.cable = handle;
				calc.open = !0;

				ret = ticalcs_probe_calc_1(&calc, model);

				g_free(calc.buffer2);
				g_free(calc.buffer);
			}
		}

		// close
		ticables_cable_close(handle);
		ticables_handle_del(handle);
	} while(0);

	return ret;
}

/**
 * ticalcs_device_info_to_model:
 * @info: pointer to device info
 *
 * Converts a cable device info to a CalcModel, if possible.
 *
 * Return value: != CALC_NONE if a precise calculator model can be determined through the cable+device info, CALC_NONE if invalid argument or SilverLink cable.
 */
CalcModel TICALL ticalcs_device_info_to_model(const CableDeviceInfo *info)
{
	CalcModel model = CALC_NONE;

	if (info != NULL)
	{
		if (info->family == CABLE_FAMILY_DBUS)
		{
			// Can't determine calc model through the sole cable model. That's a job for the upper layers.
			// Fall through.
		}
		else if (info->family == CABLE_FAMILY_USB_TI8X)
		{
			if (info->variant == CABLE_VARIANT_TI84P || info->variant == CABLE_VARIANT_TI84PSE)
			{
				model = CALC_TI84P_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI84PCSE)
			{
				model = CALC_TI84PC_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI84PCE)
			{
				model = CALC_TI84PCE_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI83PCE)
			{
				model = CALC_TI83PCE_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI82A)
			{
				model = CALC_TI82A_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI84PT)
			{
				model = CALC_TI84PT_USB;
			}
			else if (info->variant == CABLE_VARIANT_TI82AEP)
			{
				model = CALC_TI82AEP_USB;
			}
			else
			{
				ticalcs_warning("Unexpected variant %d for TI-(e)Z80 USB cable, assuming 84+CE", info->variant);
				model = CALC_TI84PCE_USB;
			}
		}
		else if (info->family == CABLE_FAMILY_USB_TI9X)
		{
			// This cable family has a single corresponding calc model.
			if (info->variant != CABLE_VARIANT_TI89TM)
			{
				ticalcs_warning("Unexpected variant %d for TI-68k USB cable", info->variant);
			}
			model = CALC_TI89T_USB;
		}
		else if (info->family == CABLE_FAMILY_USB_NSPIRE)
		{
			// This cable family has multiple corresponding calc models, but they can't be accurately distinguished at the USB level...
			if (info->variant == CABLE_VARIANT_NSPIRE)
			{
				model = CALC_NSPIRE_CX;
			}
			else if (info->variant == CABLE_VARIANT_NSPIRE_CAS)
			{
				model = CALC_NSPIRE_CX_CAS;
			}
			else
			{
				ticalcs_warning("Unexpected variant %d for Nspire USB cable", info->variant);
			}
		}
		else if (info->family == CABLE_FAMILY_USB_NSPIRE_CRADLE)
		{
			if (info->variant == CABLE_VARIANT_NSPIRE_CRADLE)
			{
				model = CALC_NSPIRE_CRADLE;
			}
			else
			{
				ticalcs_warning("Unexpected variant %d for Nspire Cradle USB cable", info->variant);
			}
		}
		else if (info->family == CABLE_FAMILY_USB_NSPIRE_CXII)
		{
			// This cable family has multiple corresponding calc models, but they can't be distinguished at the USB level.
			if (info->variant != CABLE_VARIANT_NSPIRE_CXII)
			{
				ticalcs_warning("Unexpected variant %d for Nspire CX II USB cable", info->variant);
			}
			model = CALC_NSPIRE_CXII;
		}
		else
		{
			ticalcs_critical("Unexpected cable family %d", info->family);
			// Fall through.
		}
	}
	// else fall through.

	return model;
}

/**
 * ticalcs_remap_model_from_usb:
 * @cable: a cable model model taken in #CableModel.
 * @cable: a calc model model taken in #CalcModel.
 *
 * This function remaps a CALC_*_USB model to the corresponding CALC_* model, if said CALC_* model exists and the selected cable is USB.
 *
 * Return value: the new calculator model, which can be the same as the passed one.
 **/
CalcModel TICALL ticalcs_remap_model_from_usb(CableModel cable, CalcModel calc)
{
	if (cable == CABLE_USB)
	{
		if (calc == CALC_TI84P_USB)
		{
			return CALC_TI84P;
		}
		else if (calc == CALC_TI84PC_USB)
		{
			return CALC_TI84PC;
		}
		else if (calc == CALC_TI89T_USB)
		{
			return CALC_TI89T;
		}
		// else fall through.
	}
	// else fall through.

	return calc;
}

/**
 * ticalcs_remap_model_from_usb:
 * @cable: a cable model model taken in #CableModel.
 * @cable: a calc model model taken in #CalcModel.
 *
 * This function remaps a CALC_* model to the corresponding CALC_*_USB model, if said CALC_*_USB model exists and the selected cable is USB.
 *
 * Return value: the new calculator model, which can be the same as the passed one.
 **/
CalcModel TICALL ticalcs_remap_model_to_usb(CableModel cable, CalcModel calc)
{
	if (cable == CABLE_USB)
	{
		if (calc == CALC_TI84P)
		{
			return CALC_TI84P_USB;
		}
		else if (calc == CALC_TI84PC)
		{
			return CALC_TI84PC_USB;
		}
		else if (calc == CALC_TI89T)
		{
			return CALC_TI89T_USB;
		}
		// else fall through.
	}
	// else fall through.

	return calc;
}
