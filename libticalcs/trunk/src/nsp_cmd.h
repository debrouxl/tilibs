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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __NSP_CMDS__
#define __NSP_CMDS__

// Services IDs
#define SID_NULL			0x4001
#define SID_ECHO			0x4002
#define SID_DEV_INFOS		0x4020
#define SID_SCREENSHOT		0x4021
#define SID_SCREEN_RLE		0x4024
#define SID_LOGIN			0x4050
#define SID_FILE_MGMT		0x4060
#define SID_OS_INSTALL		0x4080

// Device Information IDs
#define DI_VERSION	1
#define DI_MODEL	2
#define DI_FEXT		3

// DirList command
#define DL_INIT		0x0d
#define DL_NEXT		0x0e
#define DL_DONE		0x0f

// Structures
// ...

// Command wrappers

int cmd_r_status(CalcHandle *h, uint16_t *status);

int cmd_s_dev_infos(CalcHandle *h, uint8_t cmd);
int cmd_r_dev_infos(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data);

int cmd_s_screen_rle(CalcHandle *h, uint8_t cmd);
int cmd_r_screen_rle(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data);

int cmd_s_dir_enum_init(CalcHandle *h, const char *name);
int cmd_r_dir_enum_init(CalcHandle *h);

int cmd_s_dir_enum_next(CalcHandle *h);
int cmd_r_dir_enum_next(CalcHandle *h, char* name, uint32_t *size, uint8_t *type);

int cmd_s_dir_enum_done(CalcHandle *h);
int cmd_r_dir_enum_done(CalcHandle *h);

#endif
