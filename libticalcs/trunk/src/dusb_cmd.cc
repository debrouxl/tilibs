/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.c 2077 2006-03-31 21:16:19Z roms $ */

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
	This unit handles virtual packet types (commands) thru DirectLink.
*/

// Some functions should be renamed or re-organized...

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "dusb_vpkt.h"
#include "dusb_cmd.h"

// Type to string

typedef struct
{
	uint16_t   id;
	const char *name;
} DUSBCmdParamInfo;

static const DUSBCmdParamInfo param_types[] =
{
	{ 0x0000, "" },
	{ DUSB_PID_PRODUCT_NUMBER, "Product number" },
	{ DUSB_PID_PRODUCT_NAME, "Product name" },
	{ DUSB_PID_MAIN_PART_ID, "Main part ID" },
	{ DUSB_PID_HW_VERSION, "Hardware version" },
	{ DUSB_PID_FULL_ID, "Full ID" },
	{ DUSB_PID_LANGUAGE_ID, "Language ID" },
	{ DUSB_PID_SUBLANG_ID, "Sub-language ID" },
	{ DUSB_PID_DEVICE_TYPE, "Device type" },
	{ DUSB_PID_BOOT_VERSION, "Boot version" },
	{ DUSB_PID_OS_MODE, "OS mode" },
	{ DUSB_PID_OS_VERSION, "OS version" },
	{ DUSB_PID_PHYS_RAM, "Physical RAM" },
	{ DUSB_PID_USER_RAM, "User RAM" },
	{ DUSB_PID_FREE_RAM, "Free RAM" },
	{ DUSB_PID_PHYS_FLASH, "Physical Flash" },
	{ DUSB_PID_USER_FLASH, "User Flash" },
	{ DUSB_PID_FREE_FLASH, "Free Flash" },
	{ DUSB_PID_USER_PAGES, "User pages" },
	{ DUSB_PID_FREE_PAGES, "Free pages" }, // 0x0013
	{ DUSB_PID_HAS_SCREEN, "Has screen" }, // 0x0019
	{ DUSB_PID_COLOR_AVAILABLE, "Color is available" }, // 0x001B
	{ DUSB_PID_BITS_PER_PIXEL, "Bits per pixel" }, // 0x001D
	{ DUSB_PID_LCD_WIDTH, "LCD width" },
	{ DUSB_PID_LCD_HEIGHT, "LCD height" }, // 0x001F
	{ DUSB_PID_SCREENSHOT, "Screenshot" }, // 0x0022
	{ DUSB_PID_CLASSIC_CLK_SUPPORT, "Classic clock supported" },
	{ DUSB_PID_CLK_ON, "Clock ON" },
	{ DUSB_PID_CLK_SEC_SINCE_1997, "Clock sec since 1997" }, // 0x0025
	{ DUSB_PID_CLK_DATE_FMT, "Clock date format" }, // 0x0027
	{ DUSB_PID_CLK_TIME_FMT, "Clock time format" }, // 0x0028
	{ DUSB_PID_BATTERY, "Battery level" }, // 0x002D
	{ DUSB_PID_USER_DATA_1, "User data area 1" }, // 0x0030
	{ DUSB_PID_FLASHAPPS, "FlashApps" }, // 0x0031
	{ DUSB_PID_USER_DATA_2, "User data area 2" }, // 0x0035
	{ DUSB_PID_MAIN_PART_ID_STRING, "Main part ID (as string)" }, // 0x0036
	{ DUSB_PID_HOMESCREEN, "Home screen" },
	{ DUSB_PID_BUSY, "Busy" },
	{ DUSB_PID_SCREEN_SPLIT, "Screen split mode" }, // 0x0039
	{ DUSB_PID_NEW_CLK_SUPPORT, "New clock supported" },
	{ DUSB_PID_CLK_SECONDS, "Clock seconds" },
	{ DUSB_PID_CLK_MINUTES, "Clock minutes" },
	{ DUSB_PID_CLK_HOURS, "Clock hours" },
	{ DUSB_PID_CLK_DAY, "Clock day" },
	{ DUSB_PID_CLK_MONTH, "Clock month" },
	{ DUSB_PID_CLK_YEAR, "Clock year" }, // 0x0040
	{ DUSB_PID_ANS, "Ans contents" }, // 0x0046
	{ DUSB_PID_OS_BUILD_NUMBER, "OS build number" }, // 0x0048
	{ DUSB_PID_BOOT_BUILD_NUMBER, "Boot build number" }, // 0x0049
	{ DUSB_PID_EXACT_MATH, "Exact math engine" }, // 0x004B
	{ DUSB_PID_BOOT_HASH, "Boot hash" }, // 0x004C
	{ DUSB_PID_OS_HASH, "OS hash" }, // 0x004D
	{ DUSB_PID_PTT_MODE_SET, "PTT mode set" }, // 0x004F
	{ DUSB_PID_OS_VERSION_STRING, "OS version (as string)" }, // 0x0052
	{ DUSB_PID_BOOT_VERSION_STRING, "Boot version (as string)" }, // 0x0053
	{ DUSB_PID_PTT_MODE_STATE, "PTT mode state" }, // 0x0054
	{ DUSB_PID_PTT_MODE_FEATURES, "PTT mode features" }, // 0x0055
	{ DUSB_PID_STOPWATCH_START, "Stopwatch start" }, // 0x0059
	{ DUSB_PID_STOPWATCH_VALUE1, "Stopwatch value 1" }, // 0x005B
	{ DUSB_PID_STOPWATCH_VALUE2, "Stopwatch value 2" }, // 0x005C
	{ 0xFFFF, NULL}
};

TIEXPORT3 const char* TICALL dusb_cmd_param_type2name(uint16_t id)
{
	const DUSBCmdParamInfo *p;

	for (p = param_types; p->name != NULL; p++)
	{
		if (p->id == id)
		{
			return p->name;
		}
	}

	return "unknown: not listed";
}

// Helpers

TIEXPORT3 DUSBCalcParam* TICALL dusb_cp_new(CalcHandle * handle, uint16_t id, uint16_t size)
{
	return dusb_cp_new_ex(handle, id, size, (uint8_t *)g_malloc0(size));
}

TIEXPORT3 DUSBCalcParam* TICALL dusb_cp_new_ex(CalcHandle * handle, uint16_t id, uint16_t size, uint8_t * data)
{
	DUSBCalcParam* cp = NULL;

	if (ticalcs_validate_handle(handle))
	{
		cp = (DUSBCalcParam *)g_malloc0(sizeof(DUSBCalcParam));

		if (NULL != cp)
		{
			//GList * cpca_list;

			cp->id = id;
			cp->size = size;
			cp->data = data;

			//cpca_list = g_list_append((GList *)(handle->priv.dusb_cpca_list), cp);
			//handle->priv.dusb_cpca_list = (void *)cpca_list;
		}
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return cp;
}

TIEXPORT3 void TICALL dusb_cp_fill(DUSBCalcParam * cp, uint16_t id, uint16_t size, uint8_t * data)
{
	if (cp != NULL)
	{
		cp->id = id;
		cp->ok = 0;
		cp->size = size;
		cp->data = data;
	}
	else
	{
		ticalcs_critical("%s: cp is NULL", __FUNCTION__);
	}
}

TIEXPORT3 void TICALL dusb_cp_del(CalcHandle * handle, DUSBCalcParam* cp)
{
	//GList *cpca_list;

	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	if (cp == NULL)
	{
		ticalcs_critical("%s: cp is NULL", __FUNCTION__);
		return;
	}

	//cpca_list = g_list_remove((GList *)(handle->priv.dusb_cpca_list), cp);
	//handle->priv.dusb_cpca_list = (void *)cpca_list;

	g_free(cp->data);
	g_free(cp);
}

TIEXPORT3 DUSBCalcParam ** TICALL dusb_cp_new_array(CalcHandle * handle, unsigned int size)
{
	DUSBCalcParam ** array = NULL;

	if (ticalcs_validate_handle(handle))
	{
		array = (DUSBCalcParam **)g_malloc0((size+1) * sizeof(DUSBCalcParam *));
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return array;
}

TIEXPORT3 void TICALL dusb_cp_del_array(CalcHandle * handle, unsigned int size, DUSBCalcParam **params)
{
	unsigned int i;

	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	if (params == NULL)
	{
		ticalcs_critical("%s: params is NULL", __FUNCTION__);
		return;
	}

	for (i = 0; i < size && params[i]; i++)
	{
		dusb_cp_del(handle, params[i]);
	}
	g_free(params);
}

TIEXPORT3 void * TICALL dusb_cp_alloc_data(uint16_t size)
{
	return g_malloc0(size);
}

TIEXPORT3 DUSBCalcParam * TICALL dusb_cp_realloc_data(DUSBCalcParam* cp, uint16_t size)
{
	if (cp != NULL)
	{
		uint8_t * data = (uint8_t *)g_realloc(cp->data, size);
		if (NULL != data)
		{
			if (size > cp->size)
			{
				memset(data + cp->size, 0x00, size - cp->size);
			}
			cp->data = data;
		}
		else
		{
			return NULL;
		}
	}

	return cp;
}

TIEXPORT3 void TICALL dusb_cp_free_data(void * data)
{
	return g_free(data);
}

/////////////----------------

TIEXPORT3 DUSBCalcAttr* TICALL dusb_ca_new(CalcHandle * handle, uint16_t id, uint16_t size)
{
	return dusb_ca_new_ex(handle, id, size, (uint8_t *)g_malloc0(size));
}

TIEXPORT3 DUSBCalcAttr* TICALL dusb_ca_new_ex(CalcHandle * handle, uint16_t id, uint16_t size, uint8_t * data)
{
	DUSBCalcAttr* ca = NULL;

	if (ticalcs_validate_handle(handle))
	{
		ca = (DUSBCalcAttr *)g_malloc0(sizeof(DUSBCalcAttr));

		if (NULL != ca)
		{
			//GList * cpca_list;

			ca->id = id;
			ca->size = size;
			ca->data = data;

			//cpca_list = g_list_append((GList *)(handle->priv.dusb_cpca_list), ca);
			//handle->priv.dusb_cpca_list = (void *)cpca_list;
		}
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return ca;
}

TIEXPORT3 void TICALL dusb_ca_fill(DUSBCalcAttr * ca, uint16_t id, uint16_t size, uint8_t * data)
{
	if (ca != NULL)
	{
		ca->id = id;
		ca->ok = 0;
		ca->size = size;
		ca->data = data;
	}
	else
	{
		ticalcs_critical("%s: ca is NULL", __FUNCTION__);
	}
}

TIEXPORT3 void TICALL dusb_ca_del(CalcHandle * handle, DUSBCalcAttr* ca)
{
	//GList *cpca_list;

	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	if (ca == NULL)
	{
		ticalcs_critical("%s: ca is NULL", __FUNCTION__);
		return;
	}

	//cpca_list = g_list_remove((GList *)(handle->priv.dusb_cpca_list), ca);
	//handle->priv.dusb_cpca_list = (void *)cpca_list;

	g_free(ca->data);
	g_free(ca);
}

TIEXPORT3 DUSBCalcAttr ** TICALL dusb_ca_new_array(CalcHandle * handle, unsigned int size)
{
	DUSBCalcAttr ** array = NULL;

	if (ticalcs_validate_handle(handle))
	{
		array = (DUSBCalcAttr **)g_malloc0((size+1) * sizeof(DUSBCalcAttr *));
	}
	else
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
	}

	return array;
}

TIEXPORT3 void TICALL dusb_ca_del_array(CalcHandle * handle, unsigned int size, DUSBCalcAttr **attrs)
{
	unsigned int i;

	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return;
	}

	if (attrs == NULL)
	{
		ticalcs_critical("%s: attrs is NULL", __FUNCTION__);
		return;
	}

	for (i = 0; i < size && attrs[i]; i++)
	{
		dusb_ca_del(handle, attrs[i]);
	}
	g_free(attrs);
}

TIEXPORT3 void * TICALL dusb_ca_alloc_data(uint16_t size)
{
	return g_malloc0(size);
}

TIEXPORT3 DUSBCalcParam * TICALL dusb_ca_realloc_data(DUSBCalcParam* ca, uint16_t size)
{
	if (ca != NULL)
	{
		uint8_t * data = (uint8_t *)g_realloc(ca->data, size);
		if (NULL != data)
		{
			if (size > ca->size)
			{
				memset(data + ca->size, 0x00, size - ca->size);
			}
			ca->data = data;
		}
		else
		{
			return NULL;
		}
	}

	return ca;
}

TIEXPORT3 void TICALL dusb_ca_free_data(void * data)
{
	return g_free(data);
}

/////////////----------------

static void byteswap(uint8_t *data, uint32_t len)
{
	(void)data, (void)len;
	/*
	if(len == 2)
	{
		uint8_t tmp;

		tmp = data[0];
		data[0] = data[1];
		data[1] = tmp;
	}
	else if(len == 4)
	{
		uint8_t tmp;

		tmp = data[0];
		data[0] = data[3];
		data[3] = tmp;

		tmp = data[1];
		data[1] = data[2];
		data[2] = tmp;
	}
	*/
}

/////////////----------------

static const uint16_t usb_errors[] = {
	0x0004, 0x0006, 0x0008, 0x0009, 0x000c, 0x000d, 0x000e,
	0x0011, 0x0012, 0x001c, 0x001d, 0x0021, 0x0022, 0x0023,
	0x0027, 0x0029, 0x002b, 0x002e, 0x0034
};

static int err_code(uint16_t code)
{
	unsigned int i;

	for (i = 0; i < sizeof(usb_errors) / sizeof(usb_errors[0]); i++)
	{
		if (usb_errors[i] == code)
		{
			return i + 1;
		}
	}

	ticalcs_warning("USB error code 0x%02x not found in list. Please report it at <tilp-devel@lists.sf.net>.", code);

	return 0;
}

static int err_code_pkt(DUSBVirtualPacket *pkt)
{
	return err_code((((uint16_t)pkt->data[0]) << 8) | pkt->data[1]);
}

/////////////----------------

int dusb_check_cmd_data(CalcModel model, const uint8_t * data, uint32_t len, uint32_t vtl_size, uint16_t vtl_type)
{
	int ret = ERR_INVALID_PACKET;

	(void)vtl_size;

	switch (vtl_type)
	{
		case DUSB_VPKT_PING:
		{
			if (len == 10U)
			{
				ret = 0;
			}
		}
		break;

		case DUSB_VPKT_PARM_REQ:
		{
			if (len >= 2U)
			{
				uint16_t npids = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
				if (len == 2U + npids * 2)
				{
					ret = 0;
				}
				// else do nothing.
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_PARM_DATA:
		{
			if (len >= 2U)
			{
				uint16_t nparams = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
				data += 2;
				if (len >= 2U + 3 * nparams)
				{
					uint16_t i;
					uint32_t additional_size = 0;
					int overrun = 0;

					for (i = 0; i < nparams; i++)
					{
						uint8_t ok = !(data[2]);
						data += 3;
						if (ok)
						{
							uint16_t size = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
							data += size + 2;
							additional_size += size + 2;
						}
						if (len < 2U + 3 * nparams + additional_size)
						{
							overrun = 1;
							break;
						}
						// else do nothing.
					}
					if (!overrun && len == 2U + 3 * nparams + additional_size)
					{
						ret = 0;
					}
					// else do nothing.
				}
				// else do nothing.
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_PARM_SET:
		{
			if (len > 4U)
			{
				uint16_t size = (((uint16_t)(data[2])) << 8) | ((uint16_t)(data[3]));
				if (len == 4U + size)
				{
					ret = 0;
				}
				// else do nothing.
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_OS_BEGIN:
		{
			if (len == 11U)
			{
				ret = 0;
			}
		}
		break;

		case DUSB_VPKT_OS_ACK:
		{
			if (len >= 4U)
			{
				ret = 0;
			}
		}
		break;

		case DUSB_VPKT_OS_HEADER:
		case DUSB_VPKT_OS_DATA:
		{
			if (model == CALC_TI89T_USB)
			{
				if (len > 0U)
				{
					ret = 0;
				}
			}
			else
			{
				if (len > 4U)
				{
					ret = 0;
				}
			}
		}
		break;

		case DUSB_VPKT_DELAY_ACK:
		{
			if (len == 4U)
			{
				ret = 0;
			}
		}
		break;

		case DUSB_VPKT_ERROR:
		{
			if (len == 2U)
			{
				ret = 0;
			}
		}
		break;

		// Nothing to do.
		case DUSB_VPKT_VAR_CNTS:
		case DUSB_VPKT_MODE_SET:
		case DUSB_VPKT_EOT_ACK:
		case DUSB_VPKT_DATA_ACK:
		case DUSB_VPKT_EOT:
		{
			ret = 0;
		}
		break;

		// TODO
		case DUSB_VPKT_DIR_REQ:
		case DUSB_VPKT_VAR_HDR:
		case DUSB_VPKT_RTS:
		case DUSB_VPKT_VAR_REQ:
		case DUSB_VPKT_MODIF_VAR:
		case DUSB_VPKT_EXECUTE:
		{
			ret = 0;
		}

		// Fall through for unknown vpkts, they're already marked invalid anyway.
		default:
		break;
	}

	if (ret)
	{
		ticalcs_critical("Validation failed for DUSB packet data of len=%lu, type=%04X", (unsigned long)len, vtl_type);
	}

	return ret;
}

int dusb_dissect_cmd_data(CalcModel model, FILE *f, const uint8_t * data, uint32_t len, uint32_t vtl_size, uint16_t vtl_type)
{
	int ret = dusb_check_cmd_data(model, data, len, vtl_size, vtl_type);
	(void)vtl_size;

	if (ret)
	{
		return ret;
	}

	switch (vtl_type)
	{
		case DUSB_VPKT_PING:
		{
			uint16_t arg1 = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
			uint16_t arg2 = (((uint16_t)(data[2])) << 8) | ((uint16_t)(data[3]));
			uint16_t arg3 = (((uint16_t)(data[4])) << 8) | ((uint16_t)(data[5]));
			uint16_t arg4 = (((uint16_t)(data[6])) << 8) | ((uint16_t)(data[7]));
			uint16_t arg5 = (((uint16_t)(data[8])) << 8) | ((uint16_t)(data[9]));
			fprintf(f, "Set mode: { %u, %u, %u, %u, 0x%04X }\n", arg1, arg2, arg3, arg4, arg5);
		}
		break;

		case DUSB_VPKT_PARM_REQ:
		{
			uint16_t npids = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
			uint16_t i;

			if (len == 2U + npids * 2)
			{
				data += 2;
				fprintf(f, "Requested %u (%X) parameter IDs:\n", npids, npids);
				for (i = 0; i < npids; i++)
				{
					uint16_t pid = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
					data += 2;
					fprintf(f, "\t%04X (%s)\n", pid, dusb_cmd_param_type2name(pid));
				}
				fputc('\n', f);
			}
		}
		break;

		case DUSB_VPKT_PARM_DATA:
		{
			uint16_t nparams = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
			uint16_t i;
			uint32_t additional_size = 0;

			if (len >= 2U + 3 * nparams)
			{
				data += 2;
				fprintf(f, "Received %u (%X) parameter values:\n", nparams, nparams);
				for (i = 0; i < nparams; i++)
				{
					uint16_t pid = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
					uint8_t ok;
					data += 2;
					ok = !(*data++);
					fprintf(f, "\t%04X (%s): ", pid, dusb_cmd_param_type2name(pid));
					if (ok)
					{
						uint16_t j;
						uint16_t size = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));

						data += 2;
						additional_size += size + 2;
						if (len < 2U + 3 * nparams + additional_size)
						{
							break;
						}
						fprintf(f, "OK, size %04X\n\t\t", size);
						for (j = 0; j < size;)
						{
							fprintf(f, "%02X ", *data++);
							if (!(++j & 15))
							{
								fprintf(f, "\n\t\t");
							}
						}
						fputc('\n', f);
					}
					else
					{
						fputs("NOK !\n", f);
					}
				}
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_PARM_SET:
		{
			uint16_t id = (((uint16_t)(data[0])) << 8) | ((uint16_t)(data[1]));
			uint16_t size = (((uint16_t)(data[2])) << 8) | ((uint16_t)(data[3]));
			uint16_t i;

			data += 4;
			if (len == 4U + size)
			{
				fprintf(f, "Sending value of size %04X for parameter %04X\n\t", size, id);
				for (i = 0; i < size; i++)
				{
					fprintf(f, "%02X ", *data++);
					if (!(++i & 15))
					{
						fprintf(f, "\n\t");
					}
				}
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_OS_BEGIN:
		{
			uint32_t size = (((uint32_t)data[7]) << 24) | (((uint32_t)data[8]) << 16) | (((uint32_t)data[9]) << 8) | (((uint32_t)data[10]) << 0);
			fprintf(f, "Size: %lu / %08lX\n", (unsigned long)size, (unsigned long)size);
		}
		break;

		case DUSB_VPKT_OS_ACK:
		{
			uint32_t size = (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[2]) << 8) | (((uint32_t)data[3]) << 0);
			fprintf(f, "Chunk size: %lu / %08lX\n", (unsigned long)size, (unsigned long)size);
		}
		break;

		case DUSB_VPKT_OS_HEADER:
		case DUSB_VPKT_OS_DATA:
		{
			if (model == CALC_TI83PCE_USB || model == CALC_TI84PCE_USB)
			{
				uint32_t addr = (((uint32_t)data[3]) << 24) | (((uint32_t)data[2]) << 16) | (((uint32_t)data[1]) << 8) | (((uint32_t)data[0]) << 0);
				fprintf(f, "Address: %08lX\n", (unsigned long)addr);
			}
			else if (model != CALC_TI89T_USB)
			{
				uint16_t addr = (((uint16_t)data[0]) << 8) | (((uint32_t)data[1]) << 0);
				fprintf(f, "Address: %04X\tPage: %02X\tFlag: %02X\n", addr, data[2], data[3]);
			}
			// else do nothing.
		}
		break;

		case DUSB_VPKT_DELAY_ACK:
		{
			uint32_t delay = (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[2]) << 8) | (data[3] << 0);
			fprintf(f, "Delay: %lu\n", (unsigned long)delay);
		}
		break;

		case DUSB_VPKT_ERROR:
		{
			int err = err_code((((uint16_t)data[0]) << 8) | (((uint32_t)data[1]) << 0));
			fprintf(f, "Error code: %u (%04X)\n", err, err);
		}
		break;

		// Nothing to do.
		case DUSB_VPKT_VAR_CNTS:
		case DUSB_VPKT_MODE_SET:
		case DUSB_VPKT_EOT_ACK:
		case DUSB_VPKT_DATA_ACK:
		case DUSB_VPKT_EOT:
		break;

		// TODO
		case DUSB_VPKT_DIR_REQ:
		case DUSB_VPKT_VAR_HDR:
		case DUSB_VPKT_RTS:
		case DUSB_VPKT_VAR_REQ:
		case DUSB_VPKT_MODIF_VAR:
		case DUSB_VPKT_EXECUTE:
		{
			fputs("(no extra dissection performed for now)\n", f);
		}
		break;

		default:
		{
			fputs("(not performing extra dissection on unknown vpkt type)\n", f);
		}
		break;

	}

	return ret;
}

/////////////----------------

#define CATCH_DELAY() CATCH_DELAY_VARSIZE(NULL, 0)

#define CATCH_DELAY_VARSIZE(ds, es)                   \
	if (pkt->type == DUSB_VPKT_DELAY_ACK) \
	{ \
		uint32_t delay = (((uint32_t)pkt->data[0]) << 24) | (((uint32_t)pkt->data[1]) << 16) | (((uint32_t)pkt->data[2]) << 8) | (pkt->data[3] << 0); \
		ticalcs_info("    delay = %u", delay); \
		if (delay > 400000) \
		{ \
			delay = 400000; \
			ticalcs_info("    (absurdly high delay, clamping to a more reasonable value)"); \
		} \
\
		PAUSE(delay/1000); \
\
		dusb_vtl_pkt_del(handle, pkt); \
		pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL); \
\
		retval = (ds == NULL ? dusb_recv_data(handle, pkt) : dusb_recv_data_varsize(handle, pkt, ds, es)); \
		if (retval) \
		{ \
			goto end; \
		} \
	}

// 0x0001: set mode or ping
TIEXPORT3 int TICALL dusb_cmd_s_mode_set(CalcHandle *handle, const DUSBModeSet mode)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	retval = dusb_send_buf_size_request(handle, DUSB_DFL_BUF_SIZE);
	if (!retval)
	{
		retval = dusb_recv_buf_size_alloc(handle, NULL);
		if (!retval)
		{
			pkt = dusb_vtl_pkt_new_ex(handle, sizeof(mode), DUSB_VPKT_PING, (uint8_t *)dusb_vtl_pkt_alloc_data(sizeof(mode)));

			pkt->data[0] = MSB(mode.arg1);
			pkt->data[1] = LSB(mode.arg1);
			pkt->data[2] = MSB(mode.arg2);
			pkt->data[3] = LSB(mode.arg2);
			pkt->data[4] = MSB(mode.arg3);
			pkt->data[5] = LSB(mode.arg3);
			pkt->data[6] = MSB(mode.arg4);
			pkt->data[7] = LSB(mode.arg4);
			pkt->data[8] = MSB(mode.arg5);
			pkt->data[9] = LSB(mode.arg5);

			retval = dusb_send_data(handle, pkt);

			dusb_vtl_pkt_del(handle, pkt);

			ticalcs_info("   %04x %04x %04x %04x %04x", mode.arg1, mode.arg2, mode.arg3, mode.arg4, mode.arg5);
		}
	}

	return retval;
}

// 0x0002: begin OS transfer
TIEXPORT3 int TICALL dusb_cmd_s_os_begin(CalcHandle *handle, uint32_t size)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 11, DUSB_VPKT_OS_BEGIN, (uint8_t *)dusb_vtl_pkt_alloc_data(11));

	pkt->data[7] = MSB(MSW(size));
	pkt->data[8] = LSB(MSW(size));
	pkt->data[9] = MSB(LSW(size));
	pkt->data[10]= LSB(LSW(size));
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);

	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0003: acknowledgement of OS transfer
TIEXPORT3 int TICALL dusb_cmd_r_os_ack(CalcHandle *handle, uint32_t *size)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_OS_ACK)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (size != NULL)
		{
			*size = (((uint32_t)pkt->data[0]) << 24) | (((uint32_t)pkt->data[1]) << 16) | (((uint32_t)pkt->data[2]) << 8) | (((uint32_t)pkt->data[3]) << 0);
			ticalcs_info("   chunk size = %08x (%i)", *size, *size);
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

static int s_os(uint8_t type, CalcHandle *handle, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, 4 + size, type, (uint8_t *)dusb_vtl_pkt_alloc_data(4 + size));

	pkt->data[0] = MSB(addr);
	pkt->data[1] = LSB(addr);
	pkt->data[2] = page;
	pkt->data[3] = flag;
	memcpy(pkt->data+4, data, size);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   addr=%04x, page=%02x, flag=%02x, size=%04x", addr, page, flag, size);

	return retval;
}

// 0x0004: OS header
TIEXPORT3 int TICALL dusb_cmd_s_os_header(CalcHandle *handle, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(DUSB_VPKT_OS_HEADER, handle, addr, page, flag, size, data);
}

// 0x0005: OS data
TIEXPORT3 int TICALL dusb_cmd_s_os_data(CalcHandle *handle, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(DUSB_VPKT_OS_DATA, handle, addr, page, flag, size, data);
}

// 0x0004: OS header
TIEXPORT3 int TICALL dusb_cmd_s_os_header_89(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, size, DUSB_VPKT_OS_HEADER, (uint8_t *)dusb_vtl_pkt_alloc_data(size));

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0005: OS data
TIEXPORT3 int TICALL dusb_cmd_s_os_data_89(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, size, DUSB_VPKT_OS_DATA, (uint8_t *)dusb_vtl_pkt_alloc_data(size));

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0005: OS data
TIEXPORT3 int TICALL dusb_cmd_s_os_data_834pce(CalcHandle *handle, uint32_t addr, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, 4 + size, DUSB_VPKT_OS_DATA, (uint8_t *)dusb_vtl_pkt_alloc_data(4 + size));

	pkt->data[0] = (addr      ) & 0xFF;
	pkt->data[1] = (addr >>  8) & 0xFF;
	pkt->data[2] = (addr >> 16) & 0xFF;
	pkt->data[3] = (addr >> 24) & 0xFF;
	memcpy(pkt->data+4, data, size);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   addr=%08x, size=%04x", addr, size);

	return retval;
}

// 0x0006: acknowledgement of EOT
TIEXPORT3 int TICALL dusb_cmd_r_eot_ack(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
		}
		else if (pkt->type != DUSB_VPKT_EOT_ACK)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0x0007: parameter request
TIEXPORT3 int TICALL dusb_cmd_s_param_request(CalcHandle *handle, unsigned int npids, const uint16_t *pids)
{
	DUSBVirtualPacket* pkt;
	unsigned int i;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_ATTRS(npids, pids);

	pkt = dusb_vtl_pkt_new_ex(handle, 2 + npids * sizeof(uint16_t), DUSB_VPKT_PARM_REQ, (uint8_t *)dusb_vtl_pkt_alloc_data(2 + npids * sizeof(uint16_t)));

	pkt->data[0] = MSB(npids);
	pkt->data[1] = LSB(npids);

	for (i = 0; i < npids; i++)
	{
		pkt->data[2*(i+1) + 0] = MSB(pids[i]);
		pkt->data[2*(i+1) + 1] = LSB(pids[i]);
	}

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   npids=%i", npids);

	return retval;
}

// 0x0008: parameter data
TIEXPORT3 int TICALL dusb_cmd_r_param_data(CalcHandle *handle, unsigned int nparams, DUSBCalcParam **params)
{
	DUSBVirtualPacket* pkt;
	unsigned int i, j;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(params);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_PARM_DATA)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (((((unsigned int)pkt->data[0]) << 8) | pkt->data[1]) != nparams)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		for (i = 0, j = 2; i < nparams; i++)
		{
			DUSBCalcParam *s = dusb_cp_new(handle, 0, 0);

			if (s != NULL)
			{
				s->id = ((uint16_t)pkt->data[j++]) << 8; s->id |= pkt->data[j++];
				s->ok = !pkt->data[j++];
				if (s->ok)
				{
					s->size = ((uint16_t)pkt->data[j++]) << 8; s->size |= pkt->data[j++];
					if (s->size > 0)
					{
						s->data = (uint8_t *)g_malloc0(s->size);
						if (s->data != NULL)
						{
							memcpy(s->data, &pkt->data[j], s->size);
						}
						else
						{
							retval = ERR_MALLOC;
							break;
						}
						j += s->size;
					}
				}

				params[i] = s;
			}
			else
			{
				retval = ERR_MALLOC;
				break;
			}
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   nparams=%i", nparams);

	return retval;
}

// 0x0008 (variant): screenshot data (for TI-84 Plus C support)
TIEXPORT3 int TICALL dusb_cmd_r_screenshot(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	DUSBVirtualPacket* pkt;
	uint32_t declared_size;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data_varsize(handle, pkt, &declared_size, 153600);

	if (!retval)
	{
		CATCH_DELAY_VARSIZE(&declared_size, 153600);

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_PARM_DATA)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (   ((((uint16_t)pkt->data[0]) << 8) | pkt->data[1]) != 1
		    || ((((uint16_t)pkt->data[2]) << 8) | pkt->data[3]) != DUSB_PID_SCREENSHOT
		    || pkt->data[4] != 0)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		*size = pkt->size - 7;
		*data = (uint8_t *)g_memdup(pkt->data + 7, pkt->size - 7);
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0x0009: request directory listing
TIEXPORT3 int TICALL dusb_cmd_s_dirlist_request(CalcHandle *handle, unsigned int naids, const uint16_t *aids)
{
	DUSBVirtualPacket* pkt;
	unsigned int i;
	unsigned int j = 0;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_ATTRS(naids, aids);

	pkt = dusb_vtl_pkt_new_ex(handle, 4 + 2 * naids + 7, DUSB_VPKT_DIR_REQ, (uint8_t *)dusb_vtl_pkt_alloc_data(4 + 2 * naids + 7));

	pkt->data[j++] = MSB(MSW(naids));
	pkt->data[j++] = LSB(MSW(naids));
	pkt->data[j++] = MSB(LSW(naids));
	pkt->data[j++] = LSB(LSW(naids));

	for (i = 0; i < naids; i++)
	{
		pkt->data[j++] = MSB(aids[i]);
		pkt->data[j++] = LSB(aids[i]);
	}

	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x01;

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   naids=%i", naids);

	return retval;
}

// 0x000A: variable header (name is utf-8)
// beware: attr array contents is allocated by function
TIEXPORT3 int TICALL dusb_cmd_r_var_header(CalcHandle *handle, char *folder, char *name, DUSBCalcAttr **attr)
{
	DUSBVirtualPacket* pkt;
	uint8_t fld_len;
	uint8_t var_len;
	int nattr;
	int i, j;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(folder);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(attr);

	folder[0] = 0;
	name[0] = 0;
	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_EOT)
		{
			retval = ERR_EOT;
			goto end;
		}
		else if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_VAR_HDR)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		j = 0;
		fld_len = pkt->data[j++];
		if (fld_len)
		{
			memcpy(folder, &pkt->data[j], fld_len + 1);
			j += fld_len+1;
		}
		var_len = pkt->data[j++];
		if (var_len)
		{
			memcpy(name, &pkt->data[j], var_len + 1);
			j += var_len+1;
		}

		nattr = (((int)pkt->data[j + 0]) << 8) | pkt->data[j + 1];
		j += 2;

		for (i = 0; i < nattr; i++)
		{
			DUSBCalcAttr *s = attr[i] = dusb_ca_new(handle, 0, 0);

			s->id = ((uint16_t)pkt->data[j++]) << 8; s->id |= pkt->data[j++];
			s->ok = !pkt->data[j++];
			if (s->ok)
			{
				s->size = ((uint16_t)pkt->data[j++]) << 8; s->size |= pkt->data[j++];
				s->data = (uint8_t *)g_malloc0(s->size);
				memcpy(s->data, &pkt->data[j], s->size);
				j += s->size;
			}
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   folder=%s, name=%s", folder, name);

	return retval;
}

// 0x000B: request to send
static int dusb_cmd_s_rts2(CalcHandle *handle, const char *folder, const char *name, uint32_t size, unsigned int nattrs, const DUSBCalcAttr **attrs, int modeflag)
{
	DUSBVirtualPacket* pkt;
	int pks;
	unsigned int i;
	unsigned int j = 0;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(folder);
	VALIDATE_NONNULL(name);
	VALIDATE_ATTRS(nattrs, attrs);

	pks = 2 + strlen(name) + 1 + 5 + 2;
	if (strlen(folder))
	{
		pks += strlen(folder)+1;
	}
	for (i = 0; i < nattrs; i++)
	{
		pks += 4 + attrs[i]->size;
	}

	pkt = dusb_vtl_pkt_new_ex(handle, pks, DUSB_VPKT_RTS, (uint8_t *)dusb_vtl_pkt_alloc_data(pks));

	if (strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	pkt->data[j++] = strlen(name);
	memcpy(pkt->data + j, name, strlen(name)+1);
	j += strlen(name)+1;

	pkt->data[j++] = MSB(MSW(size));
	pkt->data[j++] = LSB(MSW(size));
	pkt->data[j++] = MSB(LSW(size));
	pkt->data[j++] = LSB(LSW(size));
	pkt->data[j++] = modeflag;

	pkt->data[j++] = MSB(nattrs);
	pkt->data[j++] = LSB(nattrs);
	for (i = 0; i < nattrs; i++)
	{
		pkt->data[j++] = MSB(attrs[i]->id);
		pkt->data[j++] = LSB(attrs[i]->id);
		pkt->data[j++] = MSB(attrs[i]->size);
		pkt->data[j++] = LSB(attrs[i]->size);
		memcpy(pkt->data + j, attrs[i]->data, attrs[i]->size);
		byteswap(pkt->data + j, attrs[i]->size);
		j += attrs[i]->size;
	}

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   folder=%s, name=%s, size=%i, nattrs=%i", folder, name, size, nattrs);

	return retval;
}

// 0x000B: request to send ("silent")
TIEXPORT3 int TICALL dusb_cmd_s_rts(CalcHandle *handle, const char *folder, const char *name, uint32_t size, unsigned int nattrs, const DUSBCalcAttr **attrs)
{
	return dusb_cmd_s_rts2(handle, folder, name, size, nattrs, attrs, 0x01);
}

// 0x000B: request to send ("non-silent")
TIEXPORT3 int TICALL dusb_cmd_s_rts_ns(CalcHandle *handle, const char *folder, const char *name, uint32_t size, unsigned int nattrs, const DUSBCalcAttr **attrs)
{
	return dusb_cmd_s_rts2(handle, folder, name, size, nattrs, attrs, 0x02);
}

// 0x000C: variable request
TIEXPORT3 int TICALL dusb_cmd_s_var_request(CalcHandle *handle, const char *folder, const char *name, unsigned int naids, const uint16_t *aids, unsigned int nattrs, const DUSBCalcAttr **attrs)
{
	DUSBVirtualPacket* pkt;
	int pks;
	unsigned int i;
	unsigned int j = 0;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(folder);
	VALIDATE_NONNULL(name);
	VALIDATE_ATTRS(naids, aids);
	VALIDATE_ATTRS(nattrs, attrs);

	pks = 2 + strlen(name) + 1 + 5 + 2 + 2 * naids + 2;
	if (strlen(folder))
	{
		pks += strlen(folder)+1;
	}
	for (i = 0; i < nattrs; i++)
	{
		pks += 4 + attrs[i]->size;
	}
	pks += 2;

	pkt = dusb_vtl_pkt_new_ex(handle, pks, DUSB_VPKT_VAR_REQ, (uint8_t *)dusb_vtl_pkt_alloc_data(pks));

	if (strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	pkt->data[j++] = strlen(name);
	memcpy(pkt->data + j, name, strlen(name)+1);
	j += strlen(name)+1;

	pkt->data[j++] = 0x01;
	pkt->data[j++] = 0xFF; pkt->data[j++] = 0xFF;
	pkt->data[j++] = 0xFF; pkt->data[j++] = 0xFF;

	pkt->data[j++] = MSB(naids);
	pkt->data[j++] = LSB(naids);
	for (i = 0; i < naids; i++)
	{
		pkt->data[j++] = MSB(aids[i]);
		pkt->data[j++] = LSB(aids[i]);
	}

	pkt->data[j++] = MSB(nattrs);
	pkt->data[j++] = LSB(nattrs);
	for (i = 0; i < nattrs; i++)
	{
		pkt->data[j++] = MSB(attrs[i]->id);
		pkt->data[j++] = LSB(attrs[i]->id);
		pkt->data[j++] = MSB(attrs[i]->size);
		pkt->data[j++] = LSB(attrs[i]->size);
		memcpy(pkt->data + j, attrs[i]->data, attrs[i]->size);
		byteswap(pkt->data + j, attrs[i]->size);
		j += attrs[i]->size;
	}
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x00;

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   folder=%s, name=%s, naids=%i, nattrs=%i", folder, name, naids, nattrs);

	return retval;
}

// 0x000D: variable contents (recv)
TIEXPORT3 int TICALL dusb_cmd_r_var_content(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_VAR_CNTS)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (size != NULL)
		{
			*size = pkt->size;
		}

		*data = (uint8_t *)g_malloc0(pkt->size);
		if (*data != NULL)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
		ticalcs_info("   size=%i", pkt->size);
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0x000D: variable contents (send)
TIEXPORT3 int TICALL dusb_cmd_s_var_content(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = dusb_vtl_pkt_new_ex(handle, size, DUSB_VPKT_VAR_CNTS, (uint8_t *)dusb_vtl_pkt_alloc_data(size));

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   size=%i", size);

	return retval;
}

// 0x000E: parameter set
TIEXPORT3 int TICALL dusb_cmd_s_param_set(CalcHandle *handle, const DUSBCalcParam *param)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(param);

	pkt = dusb_vtl_pkt_new_ex(handle, 2 + 2 + param->size, DUSB_VPKT_PARM_SET, (uint8_t *)dusb_vtl_pkt_alloc_data(2 + 2 + param->size));

	pkt->data[0] = MSB(param->id);
	pkt->data[1] = LSB(param->id);
	pkt->data[2] = MSB(param->size);
	pkt->data[3] = LSB(param->size);
	memcpy(pkt->data + 4, param->data, param->size);

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   pid=%04x, size=%04x", param->id, param->size);

	return retval;
}

// 0x0010: modify/rename/delete variable
TIEXPORT3 int TICALL dusb_cmd_s_var_modify(CalcHandle *handle,
                     const char *src_folder, const char *src_name,
                     unsigned int n_src_attrs, const DUSBCalcAttr **src_attrs,
                     const char *dst_folder, const char *dst_name,
                     unsigned int n_dst_attrs, const DUSBCalcAttr **dst_attrs)
{
	DUSBVirtualPacket* pkt;
	unsigned int i;
	unsigned int j = 0;
	unsigned int pks;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(src_folder);
	VALIDATE_NONNULL(src_name);
	VALIDATE_NONNULL(src_attrs);
	VALIDATE_NONNULL(dst_folder);
	VALIDATE_NONNULL(dst_name);
	VALIDATE_ATTRS(n_dst_attrs, dst_attrs);

	pks = 2 + strlen(src_name)+1 + 2;
	if (strlen(src_folder))
	{
		pks += strlen(src_folder)+1;
	}
	for (i = 0; i < n_src_attrs; i++)
	{
		pks += 4 + src_attrs[i]->size;
	}

	pks += 5;

	if (strlen(dst_folder))
	{
		pks += strlen(dst_folder)+1;
	}
	if (strlen(dst_name))
	{
		pks += strlen(dst_name)+1;
	}
	for (i = 0; i < n_dst_attrs; i++)
	{
		pks += 4 + dst_attrs[i]->size;
	}

	pkt = dusb_vtl_pkt_new_ex(handle, pks, DUSB_VPKT_MODIF_VAR, (uint8_t *)dusb_vtl_pkt_alloc_data(pks));

	if (strlen(src_folder))
	{
		pkt->data[j++] = strlen(src_folder);
		memcpy(pkt->data + j, src_folder, strlen(src_folder)+1);
		j += strlen(src_folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	pkt->data[j++] = strlen(src_name);
	memcpy(pkt->data + j, src_name, strlen(src_name)+1);
	j += strlen(src_name)+1;

	pkt->data[j++] = MSB(n_src_attrs);
	pkt->data[j++] = LSB(n_src_attrs);
	for (i = 0; i < n_src_attrs; i++)
	{
		pkt->data[j++] = MSB(src_attrs[i]->id);
		pkt->data[j++] = LSB(src_attrs[i]->id);
		pkt->data[j++] = MSB(src_attrs[i]->size);
		pkt->data[j++] = LSB(src_attrs[i]->size);
		memcpy(pkt->data + j, src_attrs[i]->data, src_attrs[i]->size);
		j += src_attrs[i]->size;
	}

	pkt->data[j++] = 0x01; /* ??? */

	if (strlen(dst_folder))
	{
		pkt->data[j++] = strlen(dst_folder);
		memcpy(pkt->data + j, dst_folder, strlen(dst_folder)+1);
		j += strlen(dst_folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	if (strlen(dst_name))
	{
		pkt->data[j++] = strlen(dst_name);
		memcpy(pkt->data + j, dst_name, strlen(dst_name)+1);
		j += strlen(dst_name)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	pkt->data[j++] = MSB(n_dst_attrs);
	pkt->data[j++] = LSB(n_dst_attrs);
	for (i = 0; i < n_dst_attrs; i++)
	{
		pkt->data[j++] = MSB(dst_attrs[i]->id);
		pkt->data[j++] = LSB(dst_attrs[i]->id);
		pkt->data[j++] = MSB(dst_attrs[i]->size);
		pkt->data[j++] = LSB(dst_attrs[i]->size);
		memcpy(pkt->data + j, dst_attrs[i]->data, dst_attrs[i]->size);
		j += dst_attrs[i]->size;
	}

	if (j == pks)
	{
		retval = dusb_send_data(handle, pkt);
	}
	else
	{
		// Really shouldn't occur.
		ticalcs_warning("Discrepancy in packet generation, not sending it");
		retval = ERR_INVALID_PACKET;
	}

	ticalcs_info("   src_folder=%s, name=%s, nattrs=%i", src_folder, src_name, n_src_attrs);
	ticalcs_info("   dst_folder=%s, name=%s, nattrs=%i", dst_folder, dst_name, n_dst_attrs);

	dusb_vtl_pkt_del(handle, pkt);
	return retval;
}

TIEXPORT3 int TICALL dusb_cmd_s_var_delete(CalcHandle *handle, const char *folder, const char *name, unsigned int nattrs, const DUSBCalcAttr **attrs)
{
	return dusb_cmd_s_var_modify(handle, folder, name, nattrs, attrs, "", "", 0, NULL);
}

// 0x0011: remote control
TIEXPORT3 int TICALL dusb_cmd_s_execute(CalcHandle *handle, const char *folder, const char *name, uint8_t action, const char *args, uint16_t code)
{
	DUSBVirtualPacket* pkt = NULL;
	int pks;
	int j = 0;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(folder);
	VALIDATE_NONNULL(name);

	pks = 3;
	if (handle->model == CALC_TI89T_USB && folder[0] != 0)
	{
		pks += strlen(folder) + 1;
	}
	if (name[0] != 0)
	{
		pks += strlen(name) + 1;
	}
	if (args)
	{
		pks += strlen(args);
	}
	if (action == DUSB_EID_KEY)
	{
		pks += 2;
	}
	pkt = dusb_vtl_pkt_new_ex(handle, pks, DUSB_VPKT_EXECUTE, (uint8_t *)dusb_vtl_pkt_alloc_data(pks));

	pkt->data[j++] = strlen(folder);
	if (handle->model == CALC_TI89T_USB && folder[0] != 0)
	{
		memcpy(pkt->data + j, folder, strlen(folder) + 1);
		j += strlen(folder) + 1;
	}

	pkt->data[j++] = strlen(name);
	if (name[0] != 0)
	{
		memcpy(pkt->data + j, name, strlen(name) + 1);
		j += strlen(name) + 1;
	}

	pkt->data[j++] = action;

	if (args)
	{
		memcpy(pkt->data + j, args, strlen(args));
	}
	if (action == DUSB_EID_KEY)
	{
		// TI-89T: big-endian
		// TI-84+, two-byte keycode: little-endian
		//  (keyExtend first, kbdKey second)
		// TI-84+, one-byte keycode: either order works for Z80 OSes,
		//  but big-endian is required by eZ80

		if (handle->model == CALC_TI89T_USB || MSB(code) == 0)
		{
			pkt->data[j++] = MSB(code);
			pkt->data[j++] = LSB(code);
		}
		else
		{
			pkt->data[j++] = LSB(code);
			pkt->data[j++] = MSB(code);
		}
	}

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	if (action == DUSB_EID_KEY)
	{
		ticalcs_info("   action=%i, keycode=%04x", action, code);
	}
	else
	{
		ticalcs_info("   action=%i, folder=%s, name=%s, args=%s", action, folder, name, args ? args : "NULL");
	}

	return retval;
}

// 0x0012: acknowledgement of mode setting
TIEXPORT3 int TICALL dusb_cmd_r_mode_ack(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
		}
		else if (pkt->type != DUSB_VPKT_MODE_SET)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0xAA00: acknowledgement of data
TIEXPORT3 int TICALL dusb_cmd_r_data_ack(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
		}
		else if (pkt->type != DUSB_VPKT_DATA_ACK)
		{
			ticalcs_info("cmd_r_data_ack: expected type 0x%4X, received type 0x%4X", DUSB_VPKT_DATA_ACK, pkt->type);
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0xBB00: delay acknowledgement
TIEXPORT3 int TICALL dusb_cmd_r_delay_ack(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
		}
		else if (pkt->type != DUSB_VPKT_DELAY_ACK)
		{
			ticalcs_info("cmd_r_data_ack: expected type 0x%4X, received type 0x%4X", DUSB_VPKT_DELAY_ACK, pkt->type);
			retval = ERR_INVALID_PACKET;
		}
	}

	PAUSE(100);

	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0xDD00: end of transmission (send)
TIEXPORT3 int TICALL dusb_cmd_s_eot(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, DUSB_VPKT_EOT, NULL);

	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0xDD00: end of transmission (recv)
TIEXPORT3 int TICALL dusb_cmd_r_eot(CalcHandle *handle)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 0, 0, NULL);

	retval = dusb_recv_data(handle, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if (pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code_pkt(pkt);
			goto end;
		}
		else if (pkt->type != DUSB_VPKT_EOT)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}
	}

end:
	dusb_vtl_pkt_del(handle, pkt);

	return retval;
}

// 0xEE00: error
TIEXPORT3 int TICALL dusb_cmd_s_error(CalcHandle *handle, uint16_t code)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = dusb_vtl_pkt_new_ex(handle, 2, DUSB_VPKT_ERROR, (uint8_t *)dusb_vtl_pkt_alloc_data(2));

	pkt->data[0] = MSB(code);
	pkt->data[1] = LSB(code);
	retval = dusb_send_data(handle, pkt);

	dusb_vtl_pkt_del(handle, pkt);
	ticalcs_info("   code = %04x", code);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL dusb_cmd_s_param_set_r_data_ack(CalcHandle *handle, uint16_t id, uint16_t size, const uint8_t * data)
{
	DUSBCalcParam *param;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	if (size > 0 && NULL == data)
	{
		return ERR_INVALID_PARAMETER;
	}

	param = dusb_cp_new(handle, id, size);
	if (NULL != data)
	{
		memcpy(param->data, data, size);
	}
	retval = dusb_cmd_s_param_set(handle, param);
	dusb_cp_del(handle, param);
	if (!retval)
	{
		retval = dusb_cmd_r_data_ack(handle);
	}

	return retval;
}
