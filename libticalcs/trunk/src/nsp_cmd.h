/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2007-2009  Romain Liévin
 *  Copyright (C) 2009-2019  Lionel Debroux
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

#ifndef __NSP_CMDS__
#define __NSP_CMDS__

#ifdef __cplusplus
extern "C" {
#endif

// Service IDs
#define NSP_SID_NULL                    0x4001
#define NSP_SID_ECHO                    0x4002
#define NSP_SID_ADDR_REQUEST            0x4003
#define NSP_SID_ADDR_ASSIGN             0x4003
#define NSP_SID_DEV_INFOS               0x4020
#define NSP_SID_SCREENSHOT              0x4021
#define NSP_SID_EVENT                   0x4022
#define NSP_SID_SHUTDOWN                0x4023
#define NSP_SID_SCREEN_RLE              0x4024
#define NSP_SID_ACTIVITY                0x4041
#define NSP_SID_KEYPRESSES              0x4042
#define NSP_SID_RPC                     0x4043
#define NSP_SID_LOGIN                   0x4050
#define NSP_SID_MESSAGES                0x4051
#define NSP_SID_HUB_CONNECTION          0x4054
#define NSP_SID_FILE_MGMT               0x4060
#define NSP_SID_TIROBOT                 0x4070
#define NSP_SID_OS_INSTALL              0x4080
#define NSP_SID_REMOTE_MGMT             0x4090
#define NSP_SID_DISCONNECT              0x40DE
#define NSP_SID_EXTECHO                 0x5000

// Errors
#define NSP_ERR_OK                      0x00
#define NSP_ERR_DIR_UNKNOWN             0x0a
#define NSP_ERR_DIRNAME_INVALID         0x0f
#define NSP_ERR_LIST_FAILED             0x10
#define NSP_ERR_NO_MORE_TO_LIST         0x11
#define NSP_ERR_FILENAME_INVALID        0x14
#define NSP_ERR_NO_FILE_EXTENSION       0x15

// Status command
#define NSP_CMD_STATUS                  0xff

// Device Information command
#define NSP_CMD_DI_VERSION              0x01
#define NSP_CMD_DI_MODEL                0x02
#define NSP_CMD_DI_FEXT                 0x03

// File Management commands
#define NSP_CMD_FM_PUT_FILE             0x03
#define NSP_CMD_FM_OK                   0x04
#define NSP_CMD_FM_CONTENTS             0x05
#define NSP_CMD_FM_PUT_FILE_EOT         0x06
#define NSP_CMD_FM_GET_FILE             0x07
#define NSP_CMD_FM_DEL_FILE             0x09
#define NSP_CMD_FM_NEW_FOLDER           0x0a
#define NSP_CMD_FM_DEL_FOLDER           0x0b
#define NSP_CMD_FM_COPY_FILE            0x0c
#define NSP_CMD_FM_DIRLIST_INIT         0x0d
#define NSP_CMD_FM_DIRLIST_NEXT         0x0e
#define NSP_CMD_FM_DIRLIST_DONE         0x0f
#define NSP_CMD_FM_DIRLIST_ENT          0x10
#define NSP_CMD_FM_ATTRIBUTES           0x20
#define NSP_CMD_FM_RENAME_FILE          0x21

// OS Installation commands
#define NSP_CMD_OS_INSTALL              0x03
#define NSP_CMD_OS_OK                   0x04
#define NSP_CMD_OS_CONTENTS             0x05
#define NSP_CMD_OS_PROGRESS             0x06

// Structures
// ...

// Command wrappers

TIEXPORT3 int TICALL nsp_cmd_r_login(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_status(CalcHandle *handle, uint8_t status);
TIEXPORT3 int TICALL nsp_cmd_r_status(CalcHandle *handle, uint8_t *status);

TIEXPORT3 int TICALL nsp_cmd_s_generic_data(CalcHandle *handle, uint32_t size, uint8_t *data, uint16_t sid, uint8_t cmd);
TIEXPORT3 int TICALL nsp_cmd_r_generic_data(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_echo(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nsp_cmd_r_echo(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_dev_infos(CalcHandle *handle, uint8_t cmd);
TIEXPORT3 int TICALL nsp_cmd_r_dev_infos(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_screen_rle(CalcHandle *handle, uint8_t cmd);
TIEXPORT3 int TICALL nsp_cmd_r_screen_rle(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_dir_attributes(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_dir_attributes(CalcHandle *handle, uint32_t *size, uint8_t *type, uint32_t *date);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_init(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_init(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_next(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_next(CalcHandle *handle, char* name, uint32_t *size, uint8_t *type);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_done(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_done(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_put_file(CalcHandle *handle, const char *name, uint32_t size);
TIEXPORT3 int TICALL nsp_cmd_r_put_file(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_put_file_eot(CalcHandle *handle);
// No nsp_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

TIEXPORT3 int TICALL nsp_cmd_s_get_file(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_get_file(CalcHandle *handle, uint32_t *size);

TIEXPORT3 int TICALL nsp_cmd_s_del_file(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_del_file(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_new_folder(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_new_folder(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_del_folder(CalcHandle *handle, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_del_folder(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_copy_file(CalcHandle *handle, const char *name, const char *name2);
TIEXPORT3 int TICALL nsp_cmd_r_copy_file(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_rename_file(CalcHandle *handle, const char *name, const char *name2);
TIEXPORT3 int TICALL nsp_cmd_r_rename_file(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_file_ok(CalcHandle *handle);
TIEXPORT3 int TICALL nsp_cmd_r_file_ok(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_file_contents(CalcHandle *handle, uint32_t  size, uint8_t  *data);
TIEXPORT3 int TICALL nsp_cmd_r_file_contents(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_os_install(CalcHandle *handle, uint32_t size);
TIEXPORT3 int TICALL nsp_cmd_r_os_install(CalcHandle *handle);

TIEXPORT3 int TICALL nsp_cmd_s_os_contents(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nsp_cmd_r_progress(CalcHandle *handle, uint8_t *value);

TIEXPORT3 int TICALL nsp_cmd_s_key(CalcHandle *handle, uint32_t code);
TIEXPORT3 int TICALL nsp_cmd_s_keypress_event(CalcHandle *handle, const uint8_t keycode[3]);
// There doesn't seem to be a need for nsp_cmd_r_key / nsp_cmd_r_keypress_event.

#ifdef __cplusplus
}
#endif

#endif
