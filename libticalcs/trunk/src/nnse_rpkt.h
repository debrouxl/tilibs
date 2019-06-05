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

#ifndef __NNSE_RPKT__
#define __NNSE_RPKT__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

TIEXPORT3 const char* TICALL nnse_addr2name(uint8_t id);
TIEXPORT3 const char* TICALL nnse_sid2name(uint8_t id);
TIEXPORT3 int TICALL nnse_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len, uint8_t ep);

#ifdef __cplusplus
}
#endif

#endif
