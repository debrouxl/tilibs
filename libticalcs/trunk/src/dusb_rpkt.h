/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2004-2009  Romain Liévin
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

#ifndef __DUSB_RPKT__
#define __DUSB_RPKT__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Raw packet types

#define DUSB_RPKT_BUF_SIZE_REQ   1
#define DUSB_RPKT_BUF_SIZE_ALLOC 2

#define DUSB_RPKT_VIRT_DATA      3
#define DUSB_RPKT_VIRT_DATA_LAST 4
#define DUSB_RPKT_VIRT_DATA_ACK  5

TIEXPORT3 const char* TICALL dusb_rpkt_type2name(uint8_t id);
TIEXPORT3 int TICALL dusb_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len, uint8_t ep, uint8_t * first);

#ifdef __cplusplus
}
#endif

#endif
