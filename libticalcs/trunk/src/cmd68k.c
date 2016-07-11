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
  * TI-92 commands;
  * TI-89/92+/V200/89T commands.
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "error.h"
#include "logging.h"
#include "cmd68k.h"

#ifdef _MSC_VER
#pragma warning( disable : 4761 4244)
#endif

TIEXPORT3 uint8_t TICALL ti68k_handle_to_dbus_mid(CalcHandle * handle)
{
	if (ticalcs_validate_handle(handle))
	{
		switch (handle->model)
		{
		case CALC_TI89:
		case CALC_TI89T:
			return PC_TI89;
		case CALC_TI92:
			return PC_TI92;
		case CALC_TI92P:
			return PC_TI92p;
		case CALC_V200:
			return PC_V200;
		default:
			return PC_TIXX;
		}
	}
	return 0;
}

static const uint8_t dbus_errors[] = { 0x03, 0x25, 0x1e, 0x21, 0x07, 0x24, 0x08 };

static int err_code(uint8_t *data)
{
	int i;
	int code = data[2];

	ticalcs_info(" TI->PC: SKP (%02x)", data[0]);
	for (i = 0; i < (int)(sizeof(dbus_errors) / sizeof(dbus_errors[0])); i++)
	{
		if(dbus_errors[i] == code)
		{
			return i+1;
		}
	}

	ticalcs_warning("D-BUS error code not found in list. Please report it at <tilp-devel@lists.sf.net>.");

	return 0;
}

TIEXPORT3 int TICALL ti68k_send_VAR(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname, uint8_t target)
{
	uint8_t buffer[32];
	char trans[127];
	uint8_t extra = (target == PC_TI92) ? 0 : ((vartype == TI89_BKUP) ? 0 : 1);
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);

	buffer[0] = LSB(LSW(varsize));
	buffer[1] = MSB(LSW(varsize));
	buffer[2] = LSB(MSW(varsize));
	buffer[3] = MSB(MSW(varsize));
	buffer[4] = vartype;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);
	buffer[6 + len] = 0x03;

	ticalcs_info(" PC->TI: VAR (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, trans);
	return dbus_send(handle, target, CMD_VAR, 6 + len + extra, buffer);
}

TIEXPORT3 int TICALL ti68k_send_CTS(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: CTS");
	return dbus_send(handle, target, CMD_CTS, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_XDP(CalcHandle* handle, uint32_t length, const uint8_t * data, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: XDP (0x%04X = %i bytes)", length, length);
	return dbus_send(handle, target, CMD_XDP, length, data);
}

TIEXPORT3 int TICALL ti89_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	uint8_t buffer[4] = { rej_code, 0, 0, 0 };

	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_SKP, 3, buffer);
}

TIEXPORT3 int TICALL ti92_send_SKP(CalcHandle* handle, uint8_t rej_code)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: SKP (rejection code = %i)", rej_code);
	return dbus_send(handle, PC_TI92, CMD_SKP, 1, &rej_code);
}

TIEXPORT3 int TICALL ti68k_send_ACK(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: ACK");
	return dbus_send(handle, target, CMD_ACK, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_ERR(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: ERR");
	return dbus_send(handle, target, CMD_ERR, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_RDY(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: RDY?");
	return dbus_send(handle, target, CMD_RDY, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_SCR(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: SCR");
	return dbus_send(handle, target, CMD_SCR, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_CNT(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: CNT");
	return dbus_send(handle, target, CMD_CNT, 2, NULL);
}

TIEXPORT3 int TICALL ti68k_send_KEY(CalcHandle* handle, uint16_t scancode, uint8_t target)
{
	uint8_t buf[4] = { target, CMD_KEY, LSB(scancode), MSB(scancode) };

	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: KEY");
	return ticables_cable_send(handle->cable, buf, 4);
}

TIEXPORT3 int TICALL ti68k_send_EOT(CalcHandle* handle, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: EOT");
	return dbus_send(handle, target, CMD_EOT, 2, NULL);
}

TIEXPORT3 int TICALL ti89_send_REQ(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[32];
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);
	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	buffer[0] = LSB(LSW(varsize));
	buffer[1] = MSB(LSW(varsize));
	buffer[2] = LSB(MSW(varsize));
	buffer[3] = MSB(MSW(varsize));
	buffer[4] = vartype;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);
	buffer[6 + len] = 0x00;

	len += 6 + 1;
	if (vartype != TI89_CLK) {
		len--;
	}

	ticalcs_info(" PC->TI: REQ (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_REQ, len, buffer);
}

TIEXPORT3 int TICALL ti92_send_REQ(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[32];
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);
	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = vartype;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);

	ticalcs_info(" PC->TI: REQ (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
	return dbus_send(handle, PC_TI92, CMD_REQ, 6 + len, buffer);
}

TIEXPORT3 int TICALL ti89_send_RTS(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[32];
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);
	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	buffer[0] = LSB(LSW(varsize));
	buffer[1] = MSB(LSW(varsize));
	buffer[2] = LSB(MSW(varsize));
	buffer[3] = MSB(MSW(varsize));
	buffer[4] = vartype;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);
	buffer[6 + len] = 0x00;

	len += 6 + 1;
	// used by AMS <= 2.09 ?
	//if ((vartype == TI89_AMS) || (vartype == TI89_APPL)) len--;

	ticalcs_info(" PC->TI: RTS (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_RTS, len, buffer);
}

TIEXPORT3 int TICALL ti92_send_RTS(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[32];
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);
	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	buffer[0] = LSB(LSW(varsize));
	buffer[1] = MSB(LSW(varsize));
	buffer[2] = LSB(MSW(varsize));
	buffer[3] = MSB(MSW(varsize));
	buffer[4] = vartype;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);

	ticalcs_info(" PC->TI: RTS (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
	return dbus_send(handle, PC_TI92, CMD_RTS, 6 + len, buffer);
}

TIEXPORT3 int TICALL ti89_send_RTS2(CalcHandle* handle, uint32_t varsize, uint8_t vartype, uint8_t hw_id)
{
	uint8_t buffer[9];

	VALIDATE_HANDLE(handle);

	buffer[0] = LSB(LSW(varsize));
	buffer[1] = MSB(LSW(varsize));
	buffer[2] = LSB(MSW(varsize));
	buffer[3] = MSB(MSW(varsize));
	buffer[4] = vartype;
	buffer[5] = 0x00;
	buffer[6] = 0x08;
	buffer[7] = 0x00;
	buffer[8] = hw_id;	// 0x08 -> V200, 0x09 -> Titanium (Hardware ID)

	ticalcs_info(" PC->TI: RTS (size=0x%08X=%i, id=%02X, hw_id=%02x)", varsize, varsize, vartype, hw_id);
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_RTS, 9, buffer);
}

TIEXPORT3 int TICALL ti89_send_VER(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: VER");
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_VER, 2, NULL);
}

TIEXPORT3 int TICALL ti89_send_DEL(CalcHandle* handle, uint32_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[32];
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);
	len = (uint16_t)strlen(varname);
	if (len > 17)
	{
		ticalcs_critical("Oversized variable name has length %i, clamping to 17", len);
		len = 17;
	}

	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = len;
	memcpy(buffer + 6, varname, len);

	ticalcs_info(" PC->TI: DEL (size=0x%08X=%i, id=%02X, name=%s)", varsize, varsize, vartype, varname);
	return dbus_send(handle, ti68k_handle_to_dbus_mid(handle), CMD_DEL, 6 + len, buffer);
}

TIEXPORT3 int TICALL ti89_recv_VAR(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	uint8_t strl;
	uint8_t flag;
	char * varname_nofldname;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;		// not really an error
	}
	if (cmd == CMD_SKP)
	{
		return ERR_CALC_ERROR1 + err_code(buffer);
	}
	if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (((uint32_t)buffer[1]) << 8) | (((uint32_t)buffer[2]) << 16) | (((uint32_t)buffer[3]) << 24);
	*vartype = buffer[4];
	strl = buffer[5];
	memcpy(varname, buffer + 6, strl);
	varname[strl] = '\0';
	flag = buffer[6 + strl];

	if ((length != (6 + strlen(varname))) && (length != (7 + strlen(varname))))
	{
		return ERR_INVALID_PACKET;
	}

	ticalcs_info(" TI->PC: VAR (size=0x%08X=%i, id=%02X, name=%s, flag=%i)", *varsize, *varsize, *vartype, varname, flag);
	varname_nofldname = tifiles_get_varname(varname);
	if (varname_nofldname != varname)
	{
		// This variable name contains a folder name. Erase it.
		ticalcs_info(" TI->PC: VAR: the variable name contains a folder name, stripping it.");
		memmove(varname, varname_nofldname, strlen(varname_nofldname)+1);
	}

	return 0;
}

TIEXPORT3 int TICALL ti92_recv_VAR(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	uint8_t strl;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
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

	*varsize = buffer[0] | (((uint32_t)buffer[1]) << 8) | (((uint32_t)buffer[2]) << 16) | (((uint32_t)buffer[3]) << 24);
	*vartype = buffer[4];
	strl = buffer[5];
	memcpy(varname, buffer + 6, strl);
	varname[strl] = '\0';

	if (length != (6 + strlen(varname)))
	{
		return ERR_INVALID_PACKET;
	}

	ticalcs_info(" TI->PC: VAR (size=0x%08X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, varname);

	return 0;
}

static int ti68k_recv_CTS(CalcHandle* handle, uint8_t is_92)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t *buffer;
	int ret;

	VALIDATE_HANDLE(handle);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == CMD_SKP)
	{
		return is_92 ? ERR_VAR_REJECTED : ERR_CALC_ERROR1 + err_code(buffer);
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

TIEXPORT3 int TICALL ti89_recv_CTS(CalcHandle* handle)
{
	return ti68k_recv_CTS(handle, 0);
}

TIEXPORT3 int TICALL ti92_recv_CTS(CalcHandle* handle)
{
	return ti68k_recv_CTS(handle, 1);
}

TIEXPORT3 int TICALL ti68k_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t *buffer;
	int retval;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(rej_code);

	buffer = (uint8_t *)handle->buffer;
	*rej_code = 0;

	retval = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (retval == 0)
	{
		if (cmd == CMD_CTS)
		{
			ticalcs_info("CTS");
		}
		else
		{
			if (cmd != CMD_SKP)
			{
				retval = ERR_INVALID_CMD;
			}
			else
			{
				ticalcs_info(" TI->PC: SKP (rejection code = %i)", buffer[0]);
				if (rej_code != NULL)
				{
					*rej_code = buffer[0];
				}
			}
		}
	}

	return retval;
}

TIEXPORT3 int TICALL ti68k_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host, cmd;
	int err;
	uint16_t len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(length);

	err = dbus_recv(handle, &host, &cmd, &len, data);
	*length = len;

	if (cmd != CMD_XDP)
	{
		return ERR_INVALID_CMD;
	}

	if (!err)
	{
		ticalcs_info(" TI->PC: XDP (%04X=%i bytes)", *length, *length);
	}

	return err;
}

/* ACK: receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
static int ti68k_recv_ACK(CalcHandle* handle, uint16_t * status, uint8_t is_92)
{
	uint8_t host, cmd;
	uint16_t length;
	uint8_t *buffer;
	int ret;

	VALIDATE_HANDLE(handle);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (!is_92 && cmd == CMD_SKP)
	{
		return ERR_CALC_ERROR1 + err_code(buffer);
	}

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

TIEXPORT3 int TICALL ti89_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return ti68k_recv_ACK(handle, status, 0);
}

TIEXPORT3 int TICALL ti92_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	return ti68k_recv_ACK(handle, status, 1);
}

TIEXPORT3 int TICALL ti68k_recv_CNT(CalcHandle* handle)
{
	uint8_t host, cmd;
	uint16_t sts;
	int ret;

	VALIDATE_HANDLE(handle);

	ret = dbus_recv(handle, &host, &cmd, &sts, NULL);
	if (ret)
	{
		return ret;
	}

	if (cmd == CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}

	if (cmd != CMD_CNT)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: CNT");

	return 0;
}

TIEXPORT3 int TICALL ti68k_recv_EOT(CalcHandle* handle)
{
	uint8_t host, cmd;
	uint16_t length;
	int ret;

	VALIDATE_HANDLE(handle);

	ret = dbus_recv(handle, &host, &cmd, &length, NULL);
	if (ret)
	{
		return ret;
	}

	if (cmd != CMD_EOT)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: EOT");

	return 0;
}

static int ti68k_recv_RTS(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname, uint8_t is_92)
{
	uint8_t host, cmd;
	uint8_t *buffer;
	uint16_t length;
	uint8_t strl;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}
	else if (cmd == CMD_SKP)
	{
		return is_92 ? ERR_VAR_REJECTED : ERR_CALC_ERROR1 + err_code(buffer);
	}
	else if (cmd != CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (((uint32_t)buffer[1]) << 8) | (((uint32_t)buffer[2]) << 16) | (((uint32_t)buffer[3]) << 24);
	*vartype = buffer[4];
	strl = buffer[5];
	memcpy(varname, buffer + 6, strl);
	varname[strl] = '\0';

	if (length != (6 + strlen(varname)))
	{
		return ERR_INVALID_PACKET;
	}

	ticalcs_info(" TI->PC: RTS (size=0x%08X=%i, id=%02X, name=%s)", *varsize, *varsize, *vartype, varname);

	return 0;
}

TIEXPORT3 int TICALL ti89_recv_RTS(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
	return ti68k_recv_RTS(handle, varsize, vartype, varname, 0);
}

TIEXPORT3 int TICALL ti92_recv_RTS(CalcHandle* handle, uint32_t * varsize, uint8_t * vartype, char *varname)
{
	return ti68k_recv_RTS(handle, varsize, vartype, varname, 1);
}
