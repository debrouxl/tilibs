/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#ifndef __TIFILES_FILESXX__
#define __TIFILES_FILESXX__

#ifdef __cplusplus
extern "C" {
#endif

/* Structures (common to all calcs) */

typedef struct {

  char folder[9];		// TI9x only
  char name[9];			// binary name
  char trans[18];		// translated name (human readable)
  uint8_t type;
  uint8_t attr;			// TI83+/89/92+ only (ATTRB_NONE or ARCHIVED)
  uint32_t size;		// uint16_t for TI8x
  uint8_t *data;

} TiVarEntry;

typedef struct {
  TicalcType calc_type;

  char default_folder[9];	// TI9x only
  char comment[43];		// Ti8x: 41 max
  int num_entries;
  TiVarEntry *entries;
  uint16_t checksum;

} TiRegular;


#include "files8x.h"
#include "files9x.h"

#ifdef __cplusplus
}
#endif

#endif
