/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd73.c 1327 2005-07-05 15:42:00Z roms $ */

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
  This unit handles TI73 & TI83+ & TI84+ commands.
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

// Shares some commands between TI73 & 83+ & 84+
#define PC_TI7383 ((handle->model == CALC_TI73) ? PC_TI73 : PC_TI83p)
#define TI7383_PC ((handle->model == CALC_TI73) ? TI73_PC : TI83p_PC)
#define TI7383_BKUP ((handle->model == CALC_TI73) ? TI73_BKUP : TI83p_BKUP)
#define EXTRAS ((handle->model == CALC_TI83P) || (handle->model == CALC_TI84P) ? 2 : 0)

/* Variable (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16];

	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy(buffer + 3, varname, 8);
	buffer[11] = 0x00;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticalcs_info(" PC->TI: VAR (size=0x%04X, id=%02X, name=%s, attr=%i)", varsize, vartype, varname, varattr);

	if (vartype != TI7383_BKUP)
	{
		// backup: special header
		pad_buffer_to_8_chars(buffer + 3, '\0');
		TRYF(dbus_send(handle, PC_TI7383, CMD_VAR, 11 + EXTRAS, buffer));
	} 
	else 
	{
		TRYF(dbus_send(handle, PC_TI7383, CMD_VAR, 9, buffer));
	}

	return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
TIEXPORT3 int TICALL ti73_send_VAR2(CalcHandle* handle, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page)
{
	uint8_t buffer[11];

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

TIEXPORT3 int TICALL ti73_send_CTS(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: CTS");
	return dbus_send(handle, PC_TI7383, CMD_CTS, 0, NULL);
}

TIEXPORT3 int TICALL ti73_send_XDP(CalcHandle* handle, int length, uint8_t * data)
{
	ticalcs_info(" PC->TI: XDP (0x%04X bytes)", length);
	return dbus_send(handle, PC_TI7383, CMD_XDP, length, data);
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
TIEXPORT3 int TICALL ti73_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	TRYF(dbus_send(handle, PC_TI7383, CMD_SKP, 1, &rej_code));
	ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);

	return 0;
}

TIEXPORT3 int TICALL ti73_send_ACK(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: ACK");
	return dbus_send(handle, PC_TI7383, CMD_ACK, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_ERR(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: ERR");
	return dbus_send(handle, PC_TI7383, CMD_ERR, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_RDY(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: RDY?");
	return dbus_send(handle, PC_TI7383, CMD_RDY, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_SCR(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, PC_TI7383, CMD_SCR, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_KEY(CalcHandle* handle, uint16_t scancode)
{
	uint8_t buf[5];
  
	buf[0] = PC_TI7383;
	buf[1] = CMD_KEY;
	buf[2] = LSB(scancode);
	buf[3] = MSB(scancode);

	ticalcs_info(" PC->TI: KEY");
	return ticables_cable_send(handle->cable, buf, 4);
}

TIEXPORT3 int TICALL ti73_send_EOT(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: EOT");
	return dbus_send(handle, PC_TI7383, CMD_EOT, 2, NULL);
}

/* Request variable (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16] = { 0 };
	char trans[17];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy(buffer + 3, varname, 8);
	pad_buffer_to_8_chars(buffer + 3, '\0');
	buffer[11] = 0x00;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s, attr=%i)", varsize, vartype, trans, varattr);

	if (vartype != TI83p_IDLIST && vartype != TI83p_GETCERT) 
	{
		TRYF(dbus_send(handle, PC_TI7383, CMD_REQ, 11 + EXTRAS, buffer));
	}
	else if(vartype != TI83p_GETCERT && handle->model != CALC_TI73)
	{
		TRYF(dbus_send(handle, PC_TI7383, CMD_REQ, 11, buffer));
	}
	else
	{
		TRYF(dbus_send(handle, PC_TI73, CMD_REQ, 3, buffer));
	}

	return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
TIEXPORT3 int TICALL ti73_send_REQ2(CalcHandle* handle, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr)
{
	uint8_t buffer[16] = { 0 };

	if (appname == NULL)
	{
		ticalcs_critical("%s: appname is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer[0] = LSB(appsize);
	buffer[1] = MSB(appsize);
	buffer[2] = apptype;
	memcpy(buffer + 3, appname, 8);
	pad_buffer_to_8_chars(buffer + 3, '\0');

	ticalcs_info(" PC->TI: REQ (size=0x%04X, id=%02X, name=%s)", appsize, apptype, appname);
	return dbus_send(handle, PC_TI7383, CMD_REQ, 11, buffer);
}

/* Request to send (std var header: NUL padded, fixed length) */
TIEXPORT3 int TICALL ti73_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16];
	char trans[9];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy(buffer + 3, varname, 8);
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
		pad_buffer_to_8_chars(buffer + 3, '\0');
		TRYF(dbus_send(handle, PC_TI7383, CMD_RTS, 11 + EXTRAS, buffer));
	}
	else
	{
		TRYF(dbus_send(handle, PC_TI7383, CMD_RTS, 9, buffer));
	}

	return 0;
}

TIEXPORT3 int TICALL ti73_send_VER(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: VER");
	return dbus_send(handle, PC_TI7383, CMD_VER, 2, NULL);
}

TIEXPORT3 int TICALL ti73_send_DEL(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16] = { 0 };
	char trans[9];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varname == NULL)
	{
		ticalcs_critical("%s: varname is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype == TI83p_APPL ? 0x14 : vartype;
	memcpy(buffer + 3, varname, 8);
	pad_buffer_to_8_chars(buffer + 3, '\0');
	buffer[11] = 0x00;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, vartype);
	ticalcs_info(" PC->TI: DEL (name=%s)", trans);

	return dbus_send(handle, PC_TI7383, CMD_DEL, 11, buffer);
}

TIEXPORT3 int TICALL ti73_send_DUMP(CalcHandle* handle, uint16_t page)
{
	uint8_t buffer[] = {page, 0x00, 0x00, 0x40, 0x00, 0x40, 0x0C, 0x00};
	ticalcs_info(" PC->TI: DUMP (page=%02X)", page);
	return dbus_send(handle, PC_TI83p, CMD_DMP, 8, buffer);
}


TIEXPORT3 int TICALL ti73_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	char trans[9];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL || varattr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
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
	memcpy(varname, buffer + 3, 8);
	varname[8] = '\0';
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X, id=%02X, name=%s, attrb=%i)", *varsize, *vartype, trans, *varattr);

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
		return ERR_INVALID_PACKET;
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
	memcpy(name, buffer + 3, 3);
	name[3] = '\0';
	*offset = buffer[6] | (buffer[7] << 8);
	*page = buffer[8] | (buffer[9] << 8);
	*page &= 0xff;

	ticalcs_info(" TI->PC: VAR (size=0x%04X, type=%02X, name=%s, offset=%04X, page=%02X)", *length, *type, name, *offset, *page);

	return 0;
}

TIEXPORT3 int TICALL ti73_recv_CTS(CalcHandle* handle, uint16_t length)
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

TIEXPORT3 int TICALL ti73_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
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
		return ERR_INVALID_PACKET;
	}

	buffer = (uint8_t *)handle->priv2;
	*rej_code = 0;
	TRYF(dbus_recv(handle, &host, &cmd, &length, buffer));

	if (cmd == CMD_CTS) 
	{
		ticalcs_info("CTS");
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

TIEXPORT3 int TICALL ti73_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host, cmd;

	TRYF(dbus_recv(handle, &host, &cmd, length, data));

	if (cmd != CMD_XDP)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: XDP (%04X bytes)", *length);

	return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
TIEXPORT3 int TICALL ti73_recv_ACK(CalcHandle* handle, uint16_t * status)
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

TIEXPORT3 int TICALL ti73_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	char trans[9];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (varsize == NULL || vartype == NULL || varname == NULL || varattr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PACKET;
	}

	buffer = (uint8_t *)handle->priv2;
	TRYF(dbus_recv(handle, &host, &cmd, varsize, buffer));

	if (cmd != CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (buffer[1] << 8);
	*vartype = buffer[2];
	memcpy(varname, buffer + 3, 8);
	varname[8] = '\0';
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_s(handle->model, varname, trans, *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X, id=%02X, name=%s, attrb=%i)", *varsize, *vartype, trans, *varattr);

	return 0;
}
