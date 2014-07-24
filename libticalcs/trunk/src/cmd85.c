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
  This unit handles TI85 & TI86 commands.
  This unit shares its indentation with cmd82.c
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

// Shares some commands between TI85 & 86
#define PC_TI8586 ((handle->model == CALC_TI85) ? PC_TI85 : PC_TI86)
#define TI8586_BKUP ((handle->model == CALC_TI85) ? TI85_BKUP : TI86_BKUP)

/* Variable (std var header: NUL padded, variable length) */
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
	ticalcs_info(" PC->TI: VAR (size=0x%04X, id=%02X, name=%s)", varsize, vartype, trans);

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

TIEXPORT3 int TICALL ti85_send_CTS(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: CTS");
	return dbus_send(handle, PC_TI8586, CMD_CTS, 0, NULL);
}

TIEXPORT3 int TICALL ti85_send_XDP(CalcHandle* handle, int length, uint8_t * data)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
	return dbus_send(handle, PC_TI8586, CMD_XDP, length, data);
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
TIEXPORT3 int TICALL ti85_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);
	return dbus_send(handle, PC_TI8586, CMD_SKP, 1, &rej_code);
}

TIEXPORT3 int TICALL ti85_send_ACK(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: ACK");
	return dbus_send(handle, PC_TI8586, CMD_ACK, 2, NULL);
}

TIEXPORT3 int TICALL ti85_send_ERR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: ERR");
	return dbus_send(handle, PC_TI8586, CMD_ERR, 2, NULL);
}

TIEXPORT3 int TICALL ti85_send_SCR(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, PC_TI8586, CMD_SCR, 2, NULL);
}

TIEXPORT3 int TICALL ti85_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	buf[0] = PC_TI8586;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	return ticables_cable_send(handle->cable, buf, 4);
}

TIEXPORT3 int TICALL ti85_send_EOT(CalcHandle* handle)
{
	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info(" PC->TI: EOT");
	return dbus_send(handle, PC_TI8586, CMD_EOT, 2, NULL);
}

/* Variable request (var header: NUL padded, fixed length) */
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

/* Variable (std var header: NUL padded, variable length) */
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

TIEXPORT3 int TICALL ti85_recv_CTS(CalcHandle* handle)
{
	uint8_t host, cmd;
	uint16_t length;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	TRYF(dbus_recv(handle, &host, &cmd, &length, NULL));

	if (cmd == CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}
	else if (cmd != CMD_CTS)
	{
		return ERR_INVALID_CMD;
	}

	if (length != 0x0000)
	{
		return ERR_CTS_ERROR;
	}

	ticalcs_info(" TI->PC: CTS");

	return 0;
}

TIEXPORT3 int TICALL ti85_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
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

TIEXPORT3 int TICALL ti85_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host, cmd;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	TRYF(dbus_recv(handle, &host, &cmd, length, data));

	if (cmd != CMD_XDP)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);

	return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
TIEXPORT3 int TICALL ti85_recv_ACK(CalcHandle* handle, uint16_t * status)
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
	else if (length != 0x0000)
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
