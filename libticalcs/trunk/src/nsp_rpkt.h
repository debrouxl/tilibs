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

#ifndef __NSP_RPKT__
#define __NSP_RPKT__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

TIEXPORT3 const char* TICALL nsp_addr2name(uint16_t id);
TIEXPORT3 const char* TICALL nsp_sid2name(uint16_t id);
TIEXPORT3 int TICALL nsp_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len, uint8_t ep);

#ifdef __cplusplus
}
#endif

#endif
