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

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "error.h"
#include "logging.h"
#include "cmdz80.h"

#ifdef _MSC_VER
#pragma warning( disable : 4761 )
#endif

// Share some commands between TI-82 & TI-83, TI-85 & TI-86, TI-73 & TI-83+ & TI-84+
#define TI7383p_PC ((handle != NULL) ? ((handle->model == CALC_TI73) ? TI73_PC : TI83p_PC) : 0)
#define TI7383p_BKUP ((handle->model == CALC_TI73) ? TI73_BKUP : TI83p_BKUP)
#define EXTRAS ((handle->model == CALC_TI73) ? 0 : 2)
#define TI8283_BKUP ((handle->model == CALC_TI82) ? TI82_BKUP : TI83_BKUP)
#define TI8586_BKUP ((handle->model == CALC_TI85) ? TI85_BKUP : TI86_BKUP)

uint8_t TICALL tiz80_handle_to_dbus_mid(CalcHandle * handle)
{
	if (ticalcs_validate_handle(handle))
	{
		switch (handle->model)
		{
		case CALC_TI73:
			return DBUS_MID_PC_TI73;
		case CALC_TI82:
			return DBUS_MID_PC_TI82;
		case CALC_TI83:
			return DBUS_MID_PC_TI83;
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84PC:
			return DBUS_MID_PC_TI83p;
		case CALC_TI85:
			return DBUS_MID_PC_TI85;
		case CALC_TI86:
			return DBUS_MID_PC_TI86;
		default:
			return DBUS_MID_PC_TIXX;
		}
	}
	return 0;
}

static inline int tiz80_send_simple_cmd(CalcHandle * handle, uint8_t target, uint8_t cmd, const char * cmdname, uint16_t length, uint8_t* data)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: %s", cmdname);
	return dbus_send(handle, target, cmd, length, data);
}

/* VAR: Variable (std var header: NUL padded, fixed length) */
static int tiz80_send_bkup_VAR(CalcHandle* handle, uint8_t *buffer, uint16_t varsize, uint8_t vartype, const char * varname, uint8_t target)
{
	memcpy((char *)buffer + 3, varname, 6);
	ticalcs_info(" PC->TI: VAR (size=0x%04X=%d, id=%02X, name=(<backup>))", varsize, varsize, vartype);
	return dbus_send(handle, target, DBUS_CMD_VAR, 9, buffer);
}

int TICALL ti73_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;

	if (vartype != TI7383p_BKUP || version != 0)
	{
		memcpy((char *)buffer + 3, varname, 8);
		buffer[11] = version;
		buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

		ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
		ticalcs_info(" PC->TI: VAR (size=0x%04X=%d, id=%02X, name=%s, attr=%d, version=%d)", varsize, varsize, vartype, trans, varattr, version);

		return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_VAR, 11 + EXTRAS, buffer);
	}
	else
	{
		return tiz80_send_bkup_VAR(handle, buffer, varsize, vartype, varname, tiz80_handle_to_dbus_mid_7383p(handle));
	}
}

int TICALL ti82_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;

	if (vartype != TI8283_BKUP)
	{
		memcpy((char *)buffer + 3, varname, 8);

		ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
		ticalcs_info(" PC->TI: VAR (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

		return dbus_send(handle, tiz80_handle_to_dbus_mid_8283(handle), DBUS_CMD_VAR, 11, buffer);
	}
	else
	{
		return tiz80_send_bkup_VAR(handle, buffer, varsize, vartype, varname, tiz80_handle_to_dbus_mid_8283(handle));
	}
}

int TICALL ti85_send_VAR(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;

	if (vartype != TI8586_BKUP)
	{
		int len = strlen(varname);
		if (len > 8)
		{
			ticalcs_critical("Oversized variable name has length %d, clamping to 8", len);
			len = 8;
		}
		buffer[3] = len;
		memcpy((char *)buffer + 4, varname, len);

		ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
		ticalcs_info(" PC->TI: VAR (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

		return dbus_send(handle, tiz80_handle_to_dbus_mid_8586(handle), DBUS_CMD_VAR, 4 + len, buffer);
	}
	else
	{
		return tiz80_send_bkup_VAR(handle, buffer, varsize, vartype, varname, tiz80_handle_to_dbus_mid_8586(handle));
	}
}

/* FLASH (special var header: size, id, flag, offset, page) */
int TICALL ti73_send_VAR2(CalcHandle* handle, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page)
{
	uint8_t buffer[11];

	VALIDATE_HANDLE(handle);

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

	ticalcs_info(" PC->TI: VAR (size=0x%08X=%d, id=%02X, flag=%02X, offset=%04X, page=%02X)", length, length, type, flag, offset, page);

	return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_VAR, 10, buffer);
}

int TICALL tiz80_send_CTS(CalcHandle* handle, uint8_t target)
{
	return tiz80_send_simple_cmd(handle, target, DBUS_CMD_CTS, "CTS", 0, NULL);
}

int TICALL tiz80_send_XDP(CalcHandle* handle, uint16_t length, uint8_t * data, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: XDP (0x%04X = %d bytes)", length, length);
	return dbus_send(handle, target, DBUS_CMD_XDP, length, data);
}

int TICALL tiz80_send_SKP(CalcHandle* handle, uint8_t rej_code, uint8_t target)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: SKP (rejection code = %d)", rej_code);
	return dbus_send(handle, target, DBUS_CMD_SKP, 1, &rej_code);
}

int TICALL tiz80_send_ACK(CalcHandle* handle, uint8_t target)
{
	return tiz80_send_simple_cmd(handle, target, DBUS_CMD_ACK, "ACK", 2, NULL);
}

int TICALL tiz80_send_ERR(CalcHandle* handle, uint8_t target)
{
	return tiz80_send_simple_cmd(handle, target, DBUS_CMD_ERR, "ERR", 2, NULL);
}

int TICALL ti73_send_RDY(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_RDY, "RDY", 2, NULL);
}

int TICALL tiz80_send_SCR(CalcHandle* handle, uint8_t target)
{
	return tiz80_send_simple_cmd(handle, target, DBUS_CMD_SCR, "SCR", 2, NULL);
}

int TICALL ti80_send_SCR(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, DBUS_MID_PC_TI80, DBUS_CMD_SCR, "SCR", 0, NULL);
}

int TICALL tiz80_send_KEY(CalcHandle* handle, uint16_t scancode, uint8_t target)
{
	int ret;
	uint8_t buf[4] = { target, DBUS_CMD_KEY, LSB(scancode), MSB(scancode) };
	CalcEventData event;

	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: KEY");

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_DBUS_PKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ scancode, /* id */ target, /* cmd */ DBUS_CMD_KEY, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		ret = ticables_cable_send(handle->cable, buf, 4);
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_DBUS_PKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ scancode, /* id */ target, /* cmd */ DBUS_CMD_KEY, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL tiz80_send_EOT(CalcHandle* handle, uint8_t target)
{
	return tiz80_send_simple_cmd(handle, target, DBUS_CMD_EOT, "EOT", 2, NULL);
}

/* REQ: request variable (std var header: NUL padded, fixed length) */
int TICALL ti73_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy((char *)buffer + 3, varname, 8);
	buffer[11] = version;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X=%d, id=%02X, name=%s, attr=%d)", varsize, varsize, vartype, trans, varattr);

	if (vartype != TI83p_IDLIST && vartype != TI83p_GETCERT)
	{
		return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_REQ, 11 + EXTRAS, buffer);
	}
	else if (vartype != TI83p_GETCERT && handle->model != CALC_TI73)
	{
		return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_REQ, 11, buffer);
	}
	else
	{
		return dbus_send(handle, DBUS_MID_PC_TI73, DBUS_CMD_REQ, 3, buffer);
	}
}

int TICALL ti82_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	return dbus_send(handle, tiz80_handle_to_dbus_mid_8283(handle), DBUS_CMD_REQ, 11, buffer);
}

int TICALL ti85_send_REQ(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];
	int len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: REQ (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	if ((handle->model == CALC_TI86) && (vartype >= TI86_DIR) && (vartype <= TI86_ZRCL))
	{
		memset(buffer, 0, 6);
		buffer[2] = vartype;
		return dbus_send(handle, DBUS_MID_PC_TI86, DBUS_CMD_REQ, 5, buffer);
	}
	else if ((handle->model == CALC_TI86) && (vartype == TI86_BKUP))
	{
		memset(buffer, 0, 12);
		buffer[2] = vartype;
		return dbus_send(handle, DBUS_MID_PC_TI86, DBUS_CMD_REQ, 11, buffer);
	}
	else
	{
		buffer[0] = LSB(varsize);
		buffer[1] = MSB(varsize);
		buffer[2] = vartype;
		len = strlen(varname);
		if (len > 8)
		{
			ticalcs_critical("Oversized variable name has length %d, clamping to 8", len);
			len = 8;
		}
		buffer[3] = len;
		memcpy((char *)buffer + 4, varname, len);

		return dbus_send(handle, tiz80_handle_to_dbus_mid_8586(handle), DBUS_CMD_REQ, 4 + len, buffer);
	}
}

/* FLASH (special var header: size, id, flag, offset, page) */
int TICALL ti73_send_REQ2(CalcHandle* handle, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr)
{
	uint8_t buffer[16];

	/* Note: attribute/version bytes are not used (and will be ignored
	   by the calculator if included in the packet.)  The 'appattr'
	   parameter has no effect. */

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(appname);

	buffer[0] = LSB(appsize);
	buffer[1] = MSB(appsize);
	buffer[2] = apptype;
	memcpy((char *)buffer + 3, appname, 8);

	ticalcs_info(" PC->TI: REQ (size=0x%04X=%d, id=%02X, name=%s)", appsize, appsize, apptype, appname);
	return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_REQ, 11, buffer);
}

/* Request to send (std var header: NUL padded, fixed length) */
int TICALL ti73_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy((char *)buffer + 3, varname, 8);
	buffer[11] = version;
	buffer[12] = (varattr == ATTRB_ARCHIVED) ? 0x80 : 0x00;

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X=%d, id=%02X, name=%s, attr=%d)", varsize, varsize, vartype, trans, varattr);

	if (vartype != TI7383p_BKUP || version != 0)
	{
		// backup: special header
		return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_RTS, 11 + EXTRAS, buffer);
	}
	else
	{
		return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_RTS, 9, buffer);
	}
}

/* Request to send (std var header: NUL padded, fixed length) */
int TICALL ti82_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	memcpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	if (vartype != TI8283_BKUP)
	{
		// backup: special header
		return dbus_send(handle, tiz80_handle_to_dbus_mid_8283(handle), DBUS_CMD_RTS, 11, buffer);
	}
	else
	{
		return dbus_send(handle, tiz80_handle_to_dbus_mid_8283(handle), DBUS_CMD_RTS, 9, buffer);
	}
}

/* Request to send (var header: SPC padded, fixed length) */
int TICALL ti85_send_RTS(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname)
{
	uint8_t buffer[16];
	char trans[127];
	int len;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype;
	len = strlen(varname);
	if (len > 8)
	{
		ticalcs_critical("Oversized variable name has length %d, clamping to 8", len);
		len = 8;
	}
	buffer[3] = len;
	memset(buffer + 4, ' ', 8);
	memcpy((char *)buffer + 4, varname, len);

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: RTS (size=0x%04X=%d, id=%02X, name=%s)", varsize, varsize, vartype, trans);

	return dbus_send(handle, tiz80_handle_to_dbus_mid_8586(handle), DBUS_CMD_RTS, 12, buffer);

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
	int ret;
	uint16_t ioData;
	uint16_t errSP;
	uint16_t onSP;
	uint16_t tempMem;
	uint16_t fpBase;
	uint8_t buffer[50];
	uint16_t length, offset, endptr, es, sum;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARENTRY(var);

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

	buffer[0] = (handle->model == CALC_TI82 ? DBUS_MID_PC_TI82 : DBUS_MID_PC_TI85);
	buffer[1] = DBUS_CMD_VAR;

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
#pragma warning(push)
#pragma warning(disable:4146)
	offset = -(var->size - 2);
#pragma warning(pop)
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

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_DBUS_PKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ length, /* id */ buffer[0], /* cmd */ DBUS_CMD_VAR, /* data */ buffer + 4);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		ret = ticables_cable_send(handle->cable, buffer, length + 6);
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_DBUS_PKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ length, /* id */ buffer[0], /* cmd */ DBUS_CMD_VAR, /* data */ buffer + 4);
	ret = ticalcs_event_send(handle, &event);

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL ti73_send_VER(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_VER, "VER", 2, NULL);
}

int TICALL ti73_send_DEL(CalcHandle* handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr)
{
	uint8_t buffer[16];
	char trans[127];

	/* Note: attribute/version bytes are not used (and will be ignored
	   by the calculator if included in the packet.)  The 'varattr'
	   parameter has no effect. */

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varname);

	buffer[0] = LSB(varsize);
	buffer[1] = MSB(varsize);
	buffer[2] = vartype == TI83p_APPL ? 0x14 : vartype;
	memcpy((char *)buffer + 3, varname, 8);

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), vartype);
	ticalcs_info(" PC->TI: DEL (name=%s)", trans);

	return dbus_send(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_DEL, 11, buffer);
}

int TICALL ti73_send_DUMP(CalcHandle* handle, uint16_t page)
{
	uint8_t buffer[8] = { (uint8_t)page, 0x00, 0x00, 0x40, 0x00, 0x40, 0x0C, 0x00 };

	VALIDATE_HANDLE(handle);

	ticalcs_info(" PC->TI: DUMP (page=%02X)", page);
	return dbus_send(handle, DBUS_MID_PC_TI83p, DBUS_CMD_DMP, 8, buffer);
}

int TICALL ti73_send_EKE(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_EKE, "EKE", 2, NULL);
}

int TICALL ti73_send_DKE(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_DKE, "DKE", 2, NULL);
}

int TICALL ti73_send_ELD(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_ELD, "ELD", 2, NULL);
}

int TICALL ti73_send_DLD(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_DLD, "DLD", 2, NULL);
}

int TICALL ti73_send_GID(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_GID, "GID", 2, NULL);
}

int TICALL ti73_send_RID(CalcHandle* handle)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_RID, "RID", 2, NULL);
}

int TICALL ti73_send_SID(CalcHandle* handle, uint8_t * data)
{
	return tiz80_send_simple_cmd(handle, tiz80_handle_to_dbus_mid_7383p(handle), DBUS_CMD_SID, "SID", 32, data);
}

int TICALL ti73_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr, uint8_t * version)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	uint16_t length = 0;
	char trans[127];
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);
	VALIDATE_NONNULL(varattr);
	VALIDATE_NONNULL(version);

	buffer = (uint8_t *)handle->buffer;
	memset(buffer, 0, 13);
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == DBUS_CMD_EOT)
	{
		return ERR_EOT; // not really an error
	}

	if (cmd == DBUS_CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != DBUS_CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if(length < 9 || length > 13) //if ((length != (11 + EXTRAS)) && (length != 9))
	{
		return ERR_INVALID_PACKET;
	}

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	memcpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';
	*version = buffer[11];
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%d, id=%02X, name=%s, attr=%d)", *varsize, *varsize, *vartype, trans, *varattr);

	return 0;
}

int TICALL ti82_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	uint16_t length = 0;
	char trans[127];
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

	if (cmd == DBUS_CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}

	if (cmd == DBUS_CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != DBUS_CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if ((length != 11) && (length != 9))
	{
		return ERR_INVALID_PACKET;
	}

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	memcpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%d, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

int TICALL ti85_recv_VAR(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	uint16_t length = 0;
	char trans[127];
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

	if (cmd == DBUS_CMD_EOT)
	{
		return ERR_EOT;		// not really an error
	}

	if (cmd == DBUS_CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != DBUS_CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	//if((length != (4+strlen(varname))) && (length != 9))
	//return ERR_INVALID_PACKET;

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	if (*vartype != TI8586_BKUP)
	{
		uint8_t len = buffer[3];
		if (len > 8)
		{
			len = 8;
		}
		memcpy(varname, (char *)buffer + 4, len);
		varname[8] = '\0';
	}
	else
	{
		memcpy(varname, (char *)buffer + 3, 8);
	}

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: VAR (size=0x%04X=%d, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

/* FLASH (special var header: size, id, flag, offset, page) */
int TICALL ti73_recv_VAR2(CalcHandle* handle, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	uint16_t len = 0;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(length);
	VALIDATE_NONNULL(type);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(offset);
	VALIDATE_NONNULL(page);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &len, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == DBUS_CMD_EOT)
	{
		return ERR_EOT; // not really an error
	}

	if (cmd == DBUS_CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}

	if (cmd != DBUS_CMD_VAR)
	{
		return ERR_INVALID_CMD;
	}

	if (len != 10)
	{
		return ERR_INVALID_PACKET;
	}

	*length = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*type = buffer[2];
	memcpy(name, (char *)buffer + 3, 3);
	name[3] = '\0';
	*offset = buffer[6] | (((uint16_t)buffer[7]) << 8);
	*page = buffer[8] | (((uint16_t)buffer[9]) << 8);
	*page &= 0xff;

	ticalcs_info(" TI->PC: VAR (size=0x%04X=%d, type=%02X, name=%s, offset=%04X, page=%02X)", *length, *length, *type, name, *offset, *page);

	return 0;
}

int TICALL tiz80_recv_CTS(CalcHandle* handle, uint16_t length)
{
	uint8_t host = 0, cmd = 0;
	uint16_t len = 0;
	uint8_t *buffer;
	int ret;

	VALIDATE_HANDLE(handle);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &len, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == DBUS_CMD_SKP)
	{
		return ERR_VAR_REJECTED;
	}
	else if (cmd != DBUS_CMD_CTS)
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

int TICALL tiz80_recv_SKP(CalcHandle* handle, uint8_t * rej_code)
{
	uint8_t host = 0, cmd = 0;
	uint16_t length = 0;
	uint8_t *buffer;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(rej_code);

	buffer = (uint8_t *)handle->buffer;
	*rej_code = 0;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd == DBUS_CMD_CTS)
	{
		ticalcs_info(" TI->PC: CTS");
		return 0;
	}

	if (cmd != DBUS_CMD_SKP)
	{
		return ERR_INVALID_CMD;
	}

	*rej_code = buffer[0];
	ticalcs_info(" TI->PC: SKP (rejection code = %d)", *rej_code);

	return 0;
}

static int tiz80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data, uint8_t is_73)
{
	uint8_t host = 0, cmd = 0;
	int ret;

	VALIDATE_HANDLE(handle);

	ret = dbus_recv(handle, &host, &cmd, length, data);
	if (ret)
	{
		return ret;
	}

	if (is_73 && cmd == DBUS_CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;
	}
	if (cmd != DBUS_CMD_XDP)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: XDP (%04X=%d bytes)", *length, *length);

	return 0;
}

int TICALL ti73_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 1);
}

int TICALL ti82_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

int TICALL ti85_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

int TICALL ti80_recv_XDP(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	return tiz80_recv_XDP(handle, length, data, 0);
}

int TICALL ti73_recv_SID(CalcHandle* handle, uint16_t * length, uint8_t * data)
{
	uint8_t host = 0, cmd = 0;
	int ret;

	ret = dbus_recv(handle, &host, &cmd, length, data);
	if (ret)
	{
		return ret;
	}

	if (cmd == DBUS_CMD_EOT)
	{
		ticalcs_info(" TI->PC: EOT");
		return ERR_EOT;
	}
	else if (cmd != DBUS_CMD_SID)
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
int TICALL tiz80_recv_ACK(CalcHandle* handle, uint16_t * status)
{
	uint8_t host = 0, cmd = 0;
	uint16_t length = 0;
	uint8_t *buffer;
	int ret;

	VALIDATE_HANDLE(handle);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, &length, buffer);
	if (ret)
	{
		return ret;
	}

	if (status != NULL)
	{
		*status = length;
	}
	else if (length != 0x0000) // is an error code ? (=5 when app is rejected)
	{
		return ERR_NACK;
	}

	if (cmd != DBUS_CMD_ACK)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: ACK");

	return 0;
}

int TICALL ti82_recv_ERR(CalcHandle* handle, uint16_t * status)
{
	uint8_t host = 0, cmd = 0;
	uint16_t sts = 0;
	int ret;

	VALIDATE_HANDLE(handle);

	ret = dbus_recv(handle, &host, &cmd, &sts, NULL);
	if (ret && ret != ERR_CHECKSUM)
	{
		return ret;
	}

	if (status != NULL)
	{
		*status = sts;
	}

	if (cmd != DBUS_CMD_ERR)
	{
		return ERR_INVALID_CMD;
	}

	ticalcs_info(" TI->PC: ERR");

	return 0;
}

int TICALL ti73_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr, uint8_t * version)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	char trans[127];
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);
	VALIDATE_NONNULL(varattr);
	VALIDATE_NONNULL(version);

	buffer = (uint8_t *)handle->buffer;
	memset(buffer, 0, 13);
	ret = dbus_recv(handle, &host, &cmd, varsize, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd != DBUS_CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	if (*varsize < 13)
	{
		return ERR_INVALID_PACKET;
	}

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	memcpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';
	*version = buffer[11];
	*varattr = (buffer[12] & 0x80) ? ATTRB_ARCHIVED : ATTRB_NONE;

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%d, id=%02X, name=%s, attr=%d)", *varsize, *varsize, *vartype, trans, *varattr);

	return 0;
}

int TICALL ti82_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	char trans[127];
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, varsize, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd != DBUS_CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	memcpy(varname, (char *)buffer + 3, 8);
	varname[8] = '\0';

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%d, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}

int TICALL ti85_recv_RTS(CalcHandle* handle, uint16_t * varsize, uint8_t * vartype, char *varname)
{
	uint8_t host = 0, cmd = 0;
	uint8_t *buffer;
	char trans[127];
	uint8_t strl;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(varsize);
	VALIDATE_NONNULL(vartype);
	VALIDATE_NONNULL(varname);

	buffer = (uint8_t *)handle->buffer;
	ret = dbus_recv(handle, &host, &cmd, varsize, buffer);
	if (ret)
	{
		return ret;
	}

	if (cmd != DBUS_CMD_RTS)
	{
		return ERR_INVALID_CMD;
	}

	*varsize = buffer[0] | (((uint16_t)buffer[1]) << 8);
	*vartype = buffer[2];
	strl = buffer[3];
	if (strl > 8)
	{
		strl = 8;
	}
	memcpy(varname, (char *)buffer + 4, strl);
	varname[8] = '\0';

	ticonv_varname_to_utf8_sn(handle->model, varname, trans, sizeof(trans), *vartype);
	ticalcs_info(" TI->PC: RTS (size=0x%04X=%d, id=%02X, name=%s)", *varsize, *varsize, *vartype, trans);

	return 0;
}
