/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.h 2074 2006-03-31 08:36:06Z roms $ */

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

#ifndef __NSP_CMDS__
#define __NSP_CMDS__

// Services IDs
#define SID_NULL                0x4001
#define SID_ECHO                0x4002

#define SID_DEV_INFOS           0x4020
#define SID_SCREENSHOT          0x4021
#define SID_SCREEN_RLE          0x4024

#define SID_KEYPRESSES          0x4042

#define SID_LOGIN               0x4050
#define SID_MESSAGES            0x4051

#define SID_FILE_MGMT           0x4060

#define SID_OS_INSTALL          0x4080

// Errors
#define ERR_OK					0x00
#define ERR_DIR_UNKNOWN			0x0a
#define ERR_DIRNAME_INVALID		0x0f
#define ERR_LIST_FAILED			0x10
#define ERR_NO_MORE_TO_LIST		0x11
#define ERR_FILENAME_INVALID	0x14
#define ERR_NO_FILE_EXTENSION	0x15

// Status command
#define CMD_STATUS		0xff

// Device Information command
#define CMD_DI_VERSION	0x01
#define CMD_DI_MODEL	0x02
#define CMD_DI_FEXT		0x03

// File Management commands
#define CMD_FM_PUT_FILE		0x03
#define CMD_FM_OK			0x04
#define CMD_FM_CONTENTS		0x05
#define CMD_FM_PUT_FILE_EOT	0x06
#define CMD_FM_GET_FILE		0x07
#define CMD_FM_DEL_FILE		0x09
#define CMD_FM_NEW_FOLDER	0x0a
#define CMD_FM_DEL_FOLDER	0x0b
#define CMD_FM_COPY_FILE	0x0c
#define CMD_FM_DIRLIST_INIT	0x0d
#define CMD_FM_DIRLIST_NEXT	0x0e
#define CMD_FM_DIRLIST_DONE	0x0f
#define CMD_FM_DIRLIST_ENT	0x10
#define CMD_FM_ATTRIBUTES	0x20
#define CMD_FM_RENAME_FILE	0x21

// OS Installation commands
#define CMD_OS_INSTALL		0x03
#define CMD_OS_OK			0x04
#define CMD_OS_CONTENTS		0x05
#define CMD_OS_PROGRESS		0x06

// Structures
// ...

// Command wrappers

TIEXPORT3 int TICALL nsp_cmd_r_login(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_status(CalcHandle *h, uint8_t status);
TIEXPORT3 int TICALL nsp_cmd_r_status(CalcHandle *h, uint8_t *status);

TIEXPORT3 int TICALL nsp_cmd_s_dev_infos(CalcHandle *h, uint8_t cmd);
TIEXPORT3 int TICALL nsp_cmd_r_dev_infos(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_screen_rle(CalcHandle *h, uint8_t cmd);
TIEXPORT3 int TICALL nsp_cmd_r_screen_rle(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_dir_attributes(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_dir_attributes(CalcHandle *h, uint32_t *size, uint8_t *type, uint32_t *date);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_init(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_init(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_next(CalcHandle *h);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_next(CalcHandle *h, char* name, uint32_t *size, uint8_t *type);

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_done(CalcHandle *h);
TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_done(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_put_file(CalcHandle *h, const char *name, uint32_t size);
TIEXPORT3 int TICALL nsp_cmd_r_put_file(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_put_file_eot(CalcHandle *h);
// No nsp_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

TIEXPORT3 int TICALL nsp_cmd_s_get_file(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_get_file(CalcHandle *h, uint32_t *size);

TIEXPORT3 int TICALL nsp_cmd_s_del_file(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_del_file(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_new_folder(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_new_folder(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_del_folder(CalcHandle *h, const char *name);
TIEXPORT3 int TICALL nsp_cmd_r_del_folder(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_copy_file(CalcHandle *h, const char *name, const char *name2);
TIEXPORT3 int TICALL nsp_cmd_r_copy_file(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_rename_file(CalcHandle *h, const char *name, const char *name2);
TIEXPORT3 int TICALL nsp_cmd_r_rename_file(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_file_ok(CalcHandle *h);
TIEXPORT3 int TICALL nsp_cmd_r_file_ok(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_file_contents(CalcHandle *h, uint32_t  size, uint8_t  *data);
TIEXPORT3 int TICALL nsp_cmd_r_file_contents(CalcHandle *h, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_os_install(CalcHandle *h, uint32_t size);
TIEXPORT3 int TICALL nsp_cmd_r_os_install(CalcHandle *h);

TIEXPORT3 int TICALL nsp_cmd_s_os_contents(CalcHandle *h, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nsp_cmd_r_progress(CalcHandle *h, uint8_t *value);

TIEXPORT3 int TICALL nsp_cmd_s_echo(CalcHandle *h, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL nsp_cmd_r_echo(CalcHandle *h, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL nsp_cmd_s_keypress_event(CalcHandle *h, const uint8_t keycode[3]);
// There doesn't seem to be a need for nsp_cmd_r_keypress_event.

#endif
