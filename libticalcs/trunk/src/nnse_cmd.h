/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2019  Lionel Debroux
 *  Copyright (C) 2019  Fabian Vogt
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

#ifndef __NNSE_CMDS__
#define __NNSE_CMDS__

#ifdef __cplusplus
extern "C" {
#endif

// Service IDs
#define NNSE_SID_ADDR_REQUEST  0x01
#define NNSE_SID_TIME          0x02
#define NNSE_SID_ECHO          0x03
#define NNSE_SID_STREAM        0x04
#define NNSE_SID_TRANSMIT      0x05
#define NNSE_SID_LOOPBACK      0x06
#define NNSE_SID_STATS         0x07
#define NNSE_SID_UNKNOWN       0x08
#define NNSE_SID_ACK_FLAG      0x80

// Structures
// ...

// Command wrappers

// FIXME these will need an overhaul, but most functions should exist - NNSE is mostly wrapping NN.
#if 0
TIEXPORT3 int TICALL nnse_cmd_r_login(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_status(CalcHandle *handle, uint8_t status);
TIEXPORT3 int TICALL nnse_cmd_r_status(CalcHandle *handle, uint8_t *status);

TIEXPORT3 int TICALL nnse_cmd_s_generic_data(CalcHandle *handle, uint32_t size, uint8_t *data, uint16_t sid, uint8_t cmd);
TIEXPORT3 int TICALL nnse_cmd_r_generic_data(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nnse_cmd_s_echo(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nnse_cmd_r_echo(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nnse_cmd_s_dev_infos(CalcHandle *handle, uint8_t cmd);
TIEXPORT3 int TICALL nnse_cmd_r_dev_infos(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nnse_cmd_s_screen_rle(CalcHandle *handle, uint8_t cmd);
TIEXPORT3 int TICALL nnse_cmd_r_screen_rle(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nnse_cmd_s_dir_attributes(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_dir_attributes(CalcHandle *handle, uint32_t *size, uint8_t *type, uint32_t *date);

TIEXPORT3 int TICALL nnse_cmd_s_dir_enum_init(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_dir_enum_init(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_dir_enum_next(CalcHandle *handle);
TIEXPORT3 int TICALL nnse_cmd_r_dir_enum_next(CalcHandle *handle, char* name, uint32_t *size, uint8_t *type);

TIEXPORT3 int TICALL nnse_cmd_s_dir_enum_done(CalcHandle *handle);
TIEXPORT3 int TICALL nnse_cmd_r_dir_enum_done(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_put_file(CalcHandle *handle, const char *name, uint32_t size);
TIEXPORT3 int TICALL nnse_cmd_r_put_file(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_put_file_eot(CalcHandle *handle);
// No nnse_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

TIEXPORT3 int TICALL nnse_cmd_s_get_file(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_get_file(CalcHandle *handle, uint32_t *size);

TIEXPORT3 int TICALL nnse_cmd_s_del_file(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_del_file(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_new_folder(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_new_folder(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_del_folder(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nnse_cmd_r_del_folder(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_copy_file(CalcHandle *handle, const char *name, const char *name2);
TIEXPORT3 int TICALL nnse_cmd_r_copy_file(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_rename_file(CalcHandle *handle, const char *name, const char *name2);
TIEXPORT3 int TICALL nnse_cmd_r_rename_file(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_file_ok(CalcHandle *handle);
TIEXPORT3 int TICALL nnse_cmd_r_file_ok(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_file_contents(CalcHandle *handle, uint32_t  size, uint8_t  *data);
TIEXPORT3 int TICALL nnse_cmd_r_file_contents(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nnse_cmd_s_os_install(CalcHandle *handle, uint32_t size);
TIEXPORT3 int TICALL nnse_cmd_r_os_install(CalcHandle *handle);

TIEXPORT3 int TICALL nnse_cmd_s_os_contents(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nnse_cmd_r_progress(CalcHandle *handle, uint8_t *value);

TIEXPORT3 int TICALL nnse_cmd_s_key(CalcHandle *handle, uint32_t code);
TIEXPORT3 int TICALL nnse_cmd_s_keypress_event(CalcHandle *handle, const uint8_t keycode[3]);
// There doesn't seem to be a need for nnse_cmd_r_key / nnse_cmd_r_keypress_event.
#endif

#ifdef __cplusplus
}
#endif

#endif
