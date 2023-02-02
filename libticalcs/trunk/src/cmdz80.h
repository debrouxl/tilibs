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

// /!\ NOTE: for this file, backwards compatibility will not necessarily be maintained as strongly as it is for ticalcs.h !

#ifndef __TICALCS_CMDZ80__
#define __TICALCS_CMDZ80__

#include "dbus_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Helper functions and macros */
TIEXPORT3 uint8_t TICALL tiz80_model_to_dbus_mid(CalcModel model);
TIEXPORT3 uint8_t TICALL tiz80_handle_to_dbus_mid(CalcHandle * handle);

static inline uint8_t tiz80_handle_to_dbus_mid_7383p(CalcHandle * handle)
{
	return (handle != NULL) ? ((handle->model == CALC_TI73) ? DBUS_MID_PC_TI73 : DBUS_MID_PC_TI83p) : 0;
}

static inline uint8_t tiz80_handle_to_dbus_mid_8283(CalcHandle * handle)
{
	return (handle != NULL) ? ((handle->model == CALC_TI82) ? DBUS_MID_PC_TI82 : DBUS_MID_PC_TI83) : 0;
}

static inline uint8_t tiz80_handle_to_dbus_mid_8586(CalcHandle * handle)
{
	return (handle != NULL) ? ((handle->model == CALC_TI85) ? DBUS_MID_PC_TI85 : DBUS_MID_PC_TI86) : 0;
}


/* TI-Z80 family, send functions */
TIEXPORT3 int TICALL tiz80_send_CTS(CalcHandle* handle, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_XDP(CalcHandle* handle, uint16_t length, uint8_t * data, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_SKP(CalcHandle* handle, uint8_t rej_code, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_ACK(CalcHandle* handle, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_ERR(CalcHandle* handle, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_SCR(CalcHandle* handle, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_KEY(CalcHandle* handle, uint16_t scancode, uint8_t target);
TIEXPORT3 int TICALL tiz80_send_EOT(CalcHandle* handle, uint8_t target);

/* TI-Z80 family, receive functions */
TIEXPORT3 int TICALL tiz80_recv_CTS(CalcHandle* handle, uint16_t length);
TIEXPORT3 int TICALL tiz80_recv_SKP(CalcHandle* handle, uint8_t * rej_code);
TIEXPORT3 int TICALL tiz80_recv_ACK(CalcHandle* handle, uint16_t * status);


/* TI-80 (not Z80-based, in fact), send functions */
TIEXPORT3 int TICALL ti80_send_SCR(CalcHandle *handle);

/* TI-80 (not Z80-based, in fact), receive functions */
TIEXPORT3 int TICALL ti80_recv_XDP(CalcHandle *handle, uint16_t * length, uint8_t * data);
static inline int ti80_recv_ACK(CalcHandle* handle, uint16_t* status) { return tiz80_recv_ACK(handle, status); }


/* TI-73 family, send functions */
TIEXPORT3 int TICALL ti73_send_VAR(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version);
TIEXPORT3 int TICALL ti73_send_VAR2(CalcHandle *handle, uint32_t length, uint8_t type, uint8_t flag, uint16_t offset, uint16_t page);
static inline int ti73_send_CTS(CalcHandle* handle) { return tiz80_send_CTS(handle, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_XDP(CalcHandle* handle, uint16_t length, uint8_t* data) { return tiz80_send_XDP(handle, length, data, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_SKP(CalcHandle* handle, uint8_t rej_code) { return tiz80_send_SKP(handle, rej_code, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_ACK(CalcHandle* handle) { return tiz80_send_ACK(handle, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_ERR(CalcHandle* handle) { return tiz80_send_ERR(handle, tiz80_handle_to_dbus_mid_7383p(handle)); }
TIEXPORT3 int TICALL ti73_send_RDY(CalcHandle *handle);
static inline int ti73_send_SCR(CalcHandle* handle) { return tiz80_send_SCR(handle, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_KEY(CalcHandle* handle, uint16_t scancode) { return tiz80_send_KEY(handle, scancode, tiz80_handle_to_dbus_mid_7383p(handle)); }
static inline int ti73_send_EOT(CalcHandle* handle) { return tiz80_send_EOT(handle, tiz80_handle_to_dbus_mid_7383p(handle)); }
TIEXPORT3 int TICALL ti73_send_REQ(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version);
TIEXPORT3 int TICALL ti73_send_REQ2(CalcHandle *handle, uint16_t appsize, uint8_t apptype, const char *appname, uint8_t appattr);
TIEXPORT3 int TICALL ti73_send_RTS(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr, uint8_t version);
TIEXPORT3 int TICALL ti73_send_VER(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_DEL(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname, uint8_t varattr);
TIEXPORT3 int TICALL ti73_send_DUMP(CalcHandle *handle, uint16_t page);
TIEXPORT3 int TICALL ti73_send_EKE(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_DKE(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_ELD(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_DLD(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_GID(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_RID(CalcHandle *handle);
TIEXPORT3 int TICALL ti73_send_SID(CalcHandle *handle, uint8_t * data);

/* TI-73 family, receive functions */
TIEXPORT3 int TICALL ti73_recv_VAR(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr, uint8_t * version);
TIEXPORT3 int TICALL ti73_recv_VAR2(CalcHandle *handle, uint16_t * length, uint8_t * type, char *name, uint16_t * offset, uint16_t * page);
static inline int ti73_recv_CTS(CalcHandle* handle, uint16_t length) { return tiz80_recv_CTS(handle, length); }
static inline int ti73_recv_SKP(CalcHandle* handle, uint8_t* rej_code) { return tiz80_recv_SKP(handle, rej_code); }
TIEXPORT3 int TICALL ti73_recv_XDP(CalcHandle *handle, uint16_t * length, uint8_t * data);
TIEXPORT3 int TICALL ti73_recv_SID(CalcHandle *handle, uint16_t * length, uint8_t * data);
static inline int ti73_recv_ACK(CalcHandle* handle, uint16_t* status) { return tiz80_recv_ACK(handle, status); }
TIEXPORT3 int TICALL ti73_recv_RTS(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname, uint8_t * varattr, uint8_t * version);


/* TI-82 & TI-83, send functions */
TIEXPORT3 int TICALL ti82_send_VAR(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);
static inline int ti82_send_CTS(CalcHandle* handle) { return tiz80_send_CTS(handle, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_XDP(CalcHandle* handle, uint16_t length, uint8_t* data) { return tiz80_send_XDP(handle, length, data, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_SKP(CalcHandle* handle, uint8_t rej_code) { return tiz80_send_SKP(handle, rej_code, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_ACK(CalcHandle* handle) { return tiz80_send_ACK(handle, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_ERR(CalcHandle* handle) { return tiz80_send_ERR(handle, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_SCR(CalcHandle* handle) { return tiz80_send_SCR(handle, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_KEY(CalcHandle* handle, uint16_t scancode) { return tiz80_send_KEY(handle, scancode, tiz80_handle_to_dbus_mid_8283(handle)); }
static inline int ti82_send_EOT(CalcHandle* handle) { return tiz80_send_EOT(handle, tiz80_handle_to_dbus_mid_8283(handle)); }
TIEXPORT3 int TICALL ti82_send_REQ(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti82_send_RTS(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);

/* TI-82 & TI-83, receive functions */
TIEXPORT3 int TICALL ti82_recv_VAR(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname);
static inline int ti82_recv_CTS(CalcHandle* handle) { return tiz80_recv_CTS(handle, 0); }
static inline int ti82_recv_SKP(CalcHandle* handle, uint8_t* rej_code) { return tiz80_recv_SKP(handle, rej_code); }
TIEXPORT3 int TICALL ti82_recv_XDP(CalcHandle *handle, uint16_t * length, uint8_t * data);
static inline int ti82_recv_ACK(CalcHandle* handle, uint16_t* status) { return tiz80_recv_ACK(handle, status); }
TIEXPORT3 int TICALL ti82_recv_ERR(CalcHandle *handle, uint16_t * status);
TIEXPORT3 int TICALL ti82_recv_RTS(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname);


/* TI-85 & TI-86, send functions */
TIEXPORT3 int TICALL ti85_send_VAR(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);
static inline int ti85_send_CTS(CalcHandle* handle) { return tiz80_send_CTS(handle, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_XDP(CalcHandle* handle, uint16_t length, uint8_t* data) { return tiz80_send_XDP(handle, length, data, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_SKP(CalcHandle* handle, uint8_t rej_code) { return tiz80_send_SKP(handle, rej_code, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_ACK(CalcHandle* handle) { return tiz80_send_ACK(handle, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_ERR(CalcHandle* handle) { return tiz80_send_ERR(handle, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_SCR(CalcHandle* handle) { return tiz80_send_SCR(handle, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_KEY(CalcHandle* handle, uint16_t scancode) { return tiz80_send_KEY(handle, scancode, tiz80_handle_to_dbus_mid_8586(handle)); }
static inline int ti85_send_EOT(CalcHandle* handle) { return tiz80_send_EOT(handle, tiz80_handle_to_dbus_mid_8586(handle)); }
TIEXPORT3 int TICALL ti85_send_REQ(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);
TIEXPORT3 int TICALL ti85_send_RTS(CalcHandle *handle, uint16_t varsize, uint8_t vartype, const char *varname);

/* TI-85 & TI-86, receive functions */
TIEXPORT3 int TICALL ti85_recv_VAR(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname);
static inline int ti85_recv_CTS(CalcHandle* handle) { return tiz80_recv_CTS(handle, 0); }
static inline int ti85_recv_SKP(CalcHandle* handle, uint8_t* rej_code) { return tiz80_recv_SKP(handle, rej_code); }
TIEXPORT3 int TICALL ti85_recv_XDP(CalcHandle *handle, uint16_t * length, uint8_t * data);
static inline int ti85_recv_ACK(CalcHandle* handle, uint16_t* status) { return tiz80_recv_ACK(handle, status); }
TIEXPORT3 int TICALL ti85_recv_RTS(CalcHandle *handle, uint16_t * varsize, uint8_t * vartype, char *varname);

#ifdef __cplusplus
}
#endif

#endif
