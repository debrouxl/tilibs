/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
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

#ifndef __TICALCS_ROMDUMP_H__
#define __TICALCS_ROMDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

TIEXPORT3 int TICALL rd_send_dumper(CalcHandle *handle, const char *prgname, uint16_t size, uint8_t *data);
TIEXPORT3 int TICALL rd_send_dumper2(CalcHandle *handle, const char *filename);
TIEXPORT3 int TICALL rd_is_ready(CalcHandle* handle);
TIEXPORT3 int TICALL rd_read_dump(CalcHandle* handle, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
