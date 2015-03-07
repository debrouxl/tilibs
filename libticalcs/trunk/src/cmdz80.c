/* Hey EMACS -*- linux-c -*- */

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
  This unit handles:
  * TI-80 commands;
  * TI-82 & TI-83 commands;
  * TI-85 & TI-86 commands;
  * TI-73 & TI-83+ & TI-84+ commands.
*/

#include <string.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "error.h"
#include "logging.h"
#include "macros.h"

#ifdef _MSC_VER
#pragma warning( disable : 4761 )
#endif

// Share some commands between TI73 & 83+ & 84+
#define PC_TI7383 ((handle->model == CALC_TI73) ? PC_TI73 : PC_TI83p)
#define TI7383_PC ((handle->model == CALC_TI73) ? TI73_PC : TI83p_PC)
#define TI7383_BKUP ((handle->model == CALC_TI73) ? TI73_BKUP : TI83p_BKUP)
#define EXTRAS ((handle->model == CALC_TI83P) || (handle->model == CALC_TI84P) ? 2 : 0)

// Share some commands between TI82 & 83
#define PC_TI8283 ((handle->model == CALC_TI82) ? PC_TI82 : PC_TI83)
#define TI8283_BKUP ((handle->model == CALC_TI82) ? TI82_BKUP : TI83_BKUP)

// Share some commands between TI85 & 86
#define PC_TI8586 ((handle->model == CALC_TI85) ? PC_TI85 : PC_TI86)
#define TI8586_BKUP ((handle->model == CALC_TI85) ? TI85_BKUP : TI86_BKUP)

/* VAR: Variable (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);
	buffer[11] = 0x00;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticalcs_info(" PC->TI: VAR (size=0x%04X, id=%02X, name=%s, attr=%i)", varsize, vartype, varname, varattr);

	if (vartype != TI7383_BKUP)
	{
		// backup: special header
		return dbus_send(handle, PC_TI7383, CMD_VAR, 11 + EXTRAS, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI7383, CMD_VAR, 9, buffer);
	}
}

TIEXPORT3 int TICALL ti82_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: VAR (size=0x%04X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	if (vartype != TI8283_BKUP)
	{
		// backup: special header
		return dbus_send(handle, PC_TI8283, CMD_VAR, 11, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI8283, CMD_VAR, 9, buffer);
	}
}

TIEXPORT3 int TICALL ti85_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: VAR (size=0x%04X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	if (vartype != TI8586_BKUP)
	{
		// backup: special header
		int len = strlen(varname);
		if (len > 8)
		{
			len = 8;
		}
		buffer[3] = len;
		strncpy((char *)buffer + 4, varname, len);
		return dbus_send(handle, PC_TI8586, CMD_VAR, 4 + len, buffer);
	}
	else
	{
		strncpy((char *)buffer + 3, varname, 6);
		return dbus_send(handle, PC_TI8586, CMD_VAR, 9, buffer);
	}
}

/* FLASH (special var header: size, id, flag, offset, page) */
TIEXPORT3 int TICALL ti73_send_VAR2(CalcHandle* handle, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page)
{
	uint8_t buffer[11];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

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

	ticalcs_info(" PC->TI: VAR (size=0x%04X, id=%02X, flag=%02X, offset=%04X, page=%02X)", length, type, flag, offset, page);

	return dbus_send(handle, PC_TI7383, CMD_VAR, 10, buffer);
}

/* CTS */
static int tiz80_send_CTS(CalcHandle* handle, uint8_t target)
{
	ticalcs_info(" PC->TI: CTS");
	return dbus_send(handle, target, CMD_CTS, 0, NULL);
}

TIEXPORT3 int TICALL ti73_send_CTS(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_CTS(handle, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_CTS(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_CTS(handle, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_CTS(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_CTS(handle, PC_TI8586);
}

/* XDP */
static int tiz80_send_XDP(CalcHandle* handle, uint16_t length, const uint8_t * data, uint8_t target)
{
	ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
	return dbus_send(handle, target, CMD_XDP, length, data);
}

TIEXPORT3 int TICALL ti73_send_XDP(CalcHandle* handle, uint16_t length, const uint8_t * data)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_XDP(handle, length, data, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_XDP(CalcHandle* handle, uint16_t length, const uint8_t * data)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_XDP(handle, length, data, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_XDP(CalcHandle* handle, uint16_t length, const uint8_t * data)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_XDP(handle, length, data, PC_TI8586);
}

/* SKP: skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
static int tiz80_send_SKP(CalcHandle* handle, uint8_t rej_code, uint8_t target)
{
	ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);
	return dbus_send(handle, target, CMD_SKP, 1, &rej_code);
}

TIEXPORT3 int TICALL ti73_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SKP(handle, rej_code, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SKP(handle, rej_code, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SKP(handle, rej_code, PC_TI8586);
}

/* ACK */
static int tiz80_send_ACK(CalcHandle* handle, uint8_t target)
{
	ticalcs_info(" PC->TI: ACK");
	return dbus_send(handle, target, CMD_ACK, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_ACK(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ACK(handle, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_ACK(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ACK(handle, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_ACK(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ACK(handle, PC_TI8586);
}

/* ERR */
static int tiz80_send_ERR(CalcHandle* handle, uint8_t target)
{
	ticalcs_info(" PC->TI: ERR");
	return dbus_send(handle, target, CMD_ERR, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_ERR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ERR(handle, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_ERR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ERR(handle, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_ERR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_ERR(handle, PC_TI8586);
}

/* RDY */
TIEXPORT3 int TICALL ti73_send_RDY(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: RDY?");
	return dbus_send(handle, PC_TI7383, CMD_RDY, 2, NULL);
}

/* SCR */
static int tiz80_send_SCR(CalcHandle* handle, uint8_t target)
{
	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, target, CMD_SCR, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_SCR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SCR(handle, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_SCR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SCR(handle, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_SCR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_SCR(handle, PC_TI8586);
}

TIEXPORT3 int TICALL ti80_send_SCR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, PC_TI80, CMD_SCR, 0, NULL);
}

/* KEY */
static int tiz80_send_KEY(CalcHandle* handle, uint16_t scancode, uint8_t target)
{
	uint8_t buf[5];

	buf[0] = target;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	return ticables_cable_send(handle->cable, buf, 4);
}

TIEXPORT3 int TICALL ti73_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_KEY(handle, scancode, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_KEY(handle, scancode, PC_TI83);
}

TIEXPORT3 int TICALL ti85_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_KEY(handle, scancode, PC_TI8586);
}

/* EOT */
static int tiz80_send_EOT(CalcHandle* handle, uint8_t target)
{
	ticalcs_info(" PC->TI: EOT");
	return dbus_send(handle, target, CMD_EOT, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_EOT(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_EOT(handle, PC_TI7383);
}

TIEXPORT3 int TICALL ti82_send_EOT(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_EOT(handle, PC_TI8283);
}

TIEXPORT3 int TICALL ti85_send_EOT(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	return tiz80_send_EOT(handle, PC_TI8586);
}

/* REQ: request variable (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16] = { 0 };
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);
	buffer[11] = 0x00;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s, attr=%i)", varsize, vartype, trans, varattr);

	if (vartype != TI83p_IDLIST && vartype != TI83p_GETCERT)
	{
		return dbus_send(handle, PC_TI7383, CMD_REQ, 11 + EXTRAS, buffer);
	}
	else if(vartype != TI83p_GETCERT && handle->model != CALC_TI73)
	{
		return dbus_send(handle, PC_TI7383, CMD_REQ, 11, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI73, CMD_REQ, 3, buffer);
	}
}

TIEXPORT3 int TICALL ti82_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16] = { 0 };
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	return dbus_send(handle, PC_TI8283, CMD_REQ, 11, buffer);
}

TIEXPORT3 int TICALL ti85_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16] = { 0 };
	char trans[127];
	int len;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	len = strlen(varname);
	if (len > 8)
	{
		len = 8;
	}
	buffer[3] = len;
	strncpy((char *)buffer + 4, varname, len);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s)", varsize, vartype, trans);
	if ((handle->model == CALC_TI86) && (vartype >= TI86_DIR) && (vartype <= TI86_ZRCL))
	{
		memset(buffer, 0, 9);
		buffer[2] = vartype;
		return dbus_send(handle, PC_TI86, CMD_REQ, 5, buffer);
	}
	else if((handle->model == CALC_TI86) && (vartype == TI86_BKUP))
	{
		memset(buffer, 0, 12);
		buffer[2] = vartype;
		return dbus_send(handle, PC_TI86, CMD_REQ, 11, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI8586, CMD_REQ, 4 + len, buffer);
	}
}

/* FLASH (special var header: size, id, flag, offset, page) */
TIEXPORT3 int TICALL ti73_send_REQ2(CalcHandle* handle, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr)
{
	uint8_t buffer[16] = { 0 };

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (appname == NULL)
	{
		ticalcs_critical("%s: appname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(appsize);
	buffer[1] = MSB(appsize);
	buffer[2] = apptype;
	strncpy((char *)buffer + 3, appname, 8);

	ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s)", appsize, apptype, appname);
	return dbus_send(handle, PC_TI7383, CMD_REQ, 11, buffer);
}

/* RTS */
/* Request to send (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16];
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);
	buffer[11] = 0x00;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	/* Kludge to support 84+CSE Pic files.  Please do not rely on this
	   behavior; it will go away in the future. */
	if (vartype == 0x07 && varsize == 0x55bb)
		buffer[11] = 0x0a;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X, id=%02X, name=%s, attr=%i)", varsize, vartype, trans, varattr);

	if (vartype != TI7383_BKUP)
	{
		// backup: special header
		return dbus_send(handle, PC_TI7383, CMD_RTS, 11 + EXTRAS, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI7383, CMD_RTS, 9, buffer);
	}
}

/* Request to send (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti82_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	strncpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	if (vartype != TI8283_BKUP)
	{
		// backup: special header
		return dbus_send(handle, PC_TI8283, CMD_RTS, 11, buffer);
	}
	else
	{
		return dbus_send(handle, PC_TI8283, CMD_RTS, 9, buffer);
	}
}

/* Request to send (var header: SPC padded, fixed length) */
TIEXPORT3 int TICALL ti85_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];
	int len;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	len = strlen(varname);
	if (len > 8)
	{
		len = 8;
	}
	buffer[3] = len;
	memset(buffer + 4, ' ', 8);
	strncpy((char *)buffer + 4, varname, len);

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X, id=%02X, name=%s)", varsize, vartype, trans);

	return dbus_send(handle, PC_TI8586, CMD_RTS, 12, buffer);

	return 0;
}

/* Send an invalid packet that causes the calc to execute assembly
   code stored in the most recently transferred variable.

   The program must perform whatever cleanup is necessary, including
   restoring (FPS), (OPS), and (errSP).  You can do so by calling
   ResetStacks, or by jumping to JForceCmdNoChar when you exit.  For
   ROM-independent methods, see romdump.asm.
*/
int ti82_send_asm_exec(CalcHandle* handle, VarEntry * var)
{
	uint16_t ioData;
	uint16_t errSP;
	uint16_t onSP;
	uint16_t tempMem;
	uint16_t fpBase;
	uint8_t buffer[50];
	uint16_t length, offset, endptr, es, sum;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (var == NULL)
	{
		ticalcs_critical("%s: var is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if (handle->model != CALC_TI82 && handle->model != CALC_TI85)
	{
		ticalcs_critical("asm_exec not supported for this model");
		return ERR_UNSUPPORTED;
	}

	ioData  = (handle->model == CALC_TI82 ? 0x81fd : 0x831e);
	errSP   = (handle->model == CALC_TI82 ? 0x821a : 0x8338);
	onSP    = (handle->model == CALC_TI82 ? 0x8143 : 0x81bc);
	tempMem = (handle->model == CALC_TI82 ? 0x8d0a : 0x8bdd);
	fpBase  = (handle->model == CALC_TI82 ? 0x8d0c : 0x8bdf);

	buffer[0] = (handle->model == CALC_TI82 ? PC_TI82 : PC_TI85);
	buffer[1] = CMD_VAR;

	/* Warning: Heavy wizardry begins here. ;) */

	length = errSP + 2 - ioData;
	buffer[2] = LSB(length);
	buffer[3] = MSB(length);

	memset(buffer + 4, 0, length);

	/* ld sp, (onSP) */
	buffer[4] = 0xed; buffer[5] = 0x7b; buffer[6] = LSB(onSP); buffer[7] = MSB(onSP);
	/* ld hl, (endptr) */
	endptr = (var->name[0] == 0x24 ? fpBase : tempMem);
	buffer[8] = 0x2a; buffer[9] = LSB(endptr); buffer[10] = MSB(endptr);
	/* ld de, -program_size */
	offset = -(var->size - 2);
	buffer[11] = 0x11; buffer[12] = LSB(offset); buffer[13] = MSB(offset);
	/* add hl, de */
	buffer[14] = 0x19;
	/* jp (hl) */
	buffer[15] = 0xe9;

	es = 4 + errSP - ioData;
	buffer[es] = LSB(errSP - 11); buffer[es + 1] = MSB(errSP - 11);

	buffer[es - 4] = (handle->model == CALC_TI82 ? 0x88 : 0);
	buffer[es - 3] = LSB(ioData); buffer[es - 2] = MSB(ioData);

	sum = tifiles_checksum(buffer + 4, length) + 0x5555;
	buffer[4 + length] = LSB(sum);
	buffer[4 + length + 1] = MSB(sum);

	ticalcs_info(" PC->TI: VAR (exec assembly; program size = 0x%04X)", var->size);

	return ticables_cable_send(handle->cable, buffer, length + 6);
}

TIEXPORT3 int TICALL ti73_send_VER(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: VER");
	return dbus_send(handle, PC_TI7383, CMD_VER, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_DEL(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16] = { 0 };
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype == TI83p_APPL ? 0x14 : vartype;
	strncpy((char *)buffer + 3, varname, 8);
	buffer[11] = 0x00;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: DEL (name=%s)", trans);

	return dbus_send(handle, PC_TI7383, CMD_DEL, 11, buffer);
}

TIEXPORT3 int TICALL ti73_send_DUMP(CalcHandle* handle, uint16_t page)
{
	uint8_t buffer[] = {page, 0x00, 0x00, 0x40, 0x00, 0x40, 0x0C, 0x00};

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: DUMP (page=%02X)", page);
	return dbus_send(handle, PC_TI83p, CMD_DMP, 8, buffer);
}

TIEXPORT3 int TICALL ti73_send_EKE(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: EKE");
	return dbus_send(handle, PC_TI7383, CMD_EKE, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_DKE(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: DKE");
	return dbus_send(handle, PC_TI7383, CMD_DKE, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_ELD(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: ELD");
	return dbus_send(handle, PC_TI7383, CMD_ELD, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_DLD(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: DLD");
	return dbus_send(handle, PC_TI7383, CMD_DLD, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_GID(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: GID");
	return dbus_send(handle, PC_TI7383, CMD_GID, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_RID(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: RID");
	return dbus_send(handle, PC_TI7383, CMD_RID, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_SID(CalcHandle* handle, uint8_t * data)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: SID");
	return dbus_send(handle, PC_TI7383, CMD_SID, 32, data);
}

TIEXPORT3 int TICALL ti73_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL || varattr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (cmd == CMD_EOT)
	{
		return ERR_EOT; // not really an error
	}

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if(length < 9 || length > 13) //if ((length != (11 + EXTRAS)) && (length != 9))
	{
		return ERR_INVALID_PACKET;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	strncpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%i, id=%02X, name=%s, attrb=%i)", *varsize, *varsize, *vartype, trans, *varattr);

	return 0;
}

TIEXPORT3 int TICALL ti82_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (cmd == CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if ((length != 11) && (length != 9))
	{
		return ERR_INVALID_PACKET;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	strncpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

TIEXPORT3 int TICALL ti85_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (cmd == CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	//if((length != (4+strlen(varname))) && (length != 9))
	//return ERR_INVALID_PACKET;

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	if (*vartype != TI8586_BKUP)
	{
		uint8_t len = buffer[3];
		if (len > 8)
		{
			len = 8;
		}
		strncpy(varname, (char *)buffer + 4, len);
		varname[8] = '\0';
	}
	else
	{
		strncpy(varname, (char *)buffer + 3, 8);
	}

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
TIEXPORT3 int TICALL ti73_recv_VAR2(CalcHandle* handle, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t len;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (length == NULL || type == NULL || name == NULL || offset == NULL || page == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &len, buffer));

	if (cmd == CMD_EOT)
	{
		return ERR_EOT; // not really an error
	}

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if (len != 10)
	{
		return ERR_INVALID_PACKET;
	}

	*length = buffer[0] | (buffer[1] << 8);
	*type = buffer[2];
	strncpy(name, (char *)buffer + 3, 3);
	name[3] = '\0';
	*offset = buffer[6] | (buffer[7] << 8);
	*page = buffer[8] | (buffer[9] << 8);
	*page &= 0xff;

	ticalcs_info(" TI->PC: VAR (size=0x%04X, type=%02X, name=%s, offset=%04X, page=%02X)", *length, *type, name, *offset, *page);

	return 0;
}

/* CTS */
static int tiz80_recv_CTS(CalcHandle* handle, uint16_t length)
{
	uint8_t host, cmd;
	uint16_t len;
	uint8_t *buffer;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &len, buffer));

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}
	else if (cmd != CMD_CTS)
	{
		return ERR_INVALID_CMD;
	}

	if (length != len)
	{
		return ERR_CTS_ERROR;
	}

	ticalcs_info(" TI->PC: CTS");

	return 0;
}

TIEXPORT3 int TICALL ti73_recv_CTS(CalcHandle* handle, uint16_t length)
{
	return tiz80_recv_CTS(handle, length);
}

TIEXPORT3 int TICALL ti82_recv_CTS(CalcHandle* handle)
{
	return tiz80_recv_CTS(handle, 0);
}

TIEXPORT3 int TICALL ti85_recv_CTS(CalcHandle* handle)
{
	return tiz80_recv_CTS(handle, 0);
}

/* SKP */
static int tiz80_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t *buffer;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (rej_code == NULL)
	{
		ticalcs_critical("%s: rej_code is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	*rej_code = 0;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (cmd == CMD_CTS)
	{
		ticalcs_info(" TI->PC: CTS");
		return 0;
	}

	if (cmd != CMD_SKP)
	{
		return ERR_INVALID_CMD;
	}

	*rej_code = buffer[0];
	ticalcs_info(" TI->PC: SKP (rejection code = %i)", *rej_code);

	return 0;
}

TIEXPORT3 int TICALL ti73_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	return tiz80_recv_SKP(handle, rej_code);
}

TIEXPORT3 int TICALL ti82_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	return tiz80_recv_SKP(handle, rej_code);
}

TIEXPORT3 int TICALL ti85_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	return tiz80_recv_SKP(handle, rej_code);
}

/* XDP */
TIEXPORT3 int TICALL tiz80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data, uint8_t is_73)
{
	uint8_t host, cmd;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	TRYF(dbus_recv(handle, &host, &cmd, length, data));

	if (is_73 && cmd == CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;
	}
	if (cmd != CMD_XDP)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);

	return 0;
}

TIEXPORT3 int TICALL ti73_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 1);
}

TIEXPORT3 int TICALL ti82_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

TIEXPORT3 int TICALL ti85_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

TIEXPORT3 int TICALL ti80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

TIEXPORT3 int TICALL ti73_recv_SID(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host, cmd;

	TRYF(dbus_recv(handle, &host, &cmd, length, data));

	if (cmd == CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;
	}
	else if (cmd != CMD_SID)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: SID (%04X bytes)", *length);

	return 0;
}

/* ACK: receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
static int tiz80_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t *buffer;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (status != NULL)
	{
		*status = length;
	}
	else if (length != 0x0000) // is an error code ? (=5 when app is rejected)
	{
		return ERR_NACK;
	}

	if (cmd != CMD_ACK)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: ACK");

	return 0;
}

TIEXPORT3 int TICALL ti73_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return tiz80_recv_ACK(handle, status);
}

TIEXPORT3 int TICALL ti82_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return tiz80_recv_ACK(handle, status);
}

TIEXPORT3 int TICALL ti85_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return tiz80_recv_ACK(handle, status);
}

TIEXPORT3 int TICALL ti80_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return tiz80_recv_ACK(handle, status);
}

/* ERR */
TIEXPORT3 int TICALL ti82_recv_ERR(CalcHandle* handle, uint16_t * status)
{
	uint8_t host, cmd;
	uint16_t sts;
	int ret;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ret = dbus_recv(handle, &host, &cmd, &sts, NULL);
	if (ret && ret != ERR_CHECKSUM)
	{
		return ret;
	}

	if (status != NULL)
	{
		*status = sts;
	}

	if (cmd != CMD_ERR)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: ERR");

	return 0;
}

/* RTS */
TIEXPORT3 int TICALL ti73_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL || varattr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

	if (cmd != CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	strncpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%i, id=%02X, name=%s, attrb=%i)", *varsize, *varsize, *vartype, trans, *varattr);

	return 0;
}

TIEXPORT3 int TICALL ti82_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	char trans[127];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

	if (cmd != CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	strncpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

TIEXPORT3 int TICALL ti85_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	char trans[127];
	uint8_t strl;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

	if (cmd != CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	strl = buffer[3];
	if (strl > 8)
	{
		strl = 8;
	}
	strncpy(varname, (char *)buffer + 4, strl);
	varname[8] = '\0';

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}
