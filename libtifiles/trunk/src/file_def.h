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

#ifndef __TIFILES_FILEDEF__
#define __TIFILES_FILEDEF__

#define TIFILES_NCALCS 10	// # of supported calcs

typedef enum {
  CALC_NONE = 0,
  CALC_TI92P, CALC_TI92, CALC_TI89,
  CALC_TI86, CALC_TI85, CALC_TI83P, CALC_TI83, CALC_TI82, CALC_TI73,
  CALC_V200,
} TicalcType;

typedef enum {
  ATTRB_NONE = 0, ATTRB_LOCKED = 1, ATTRB_PROTECTED, ATTRB_ARCHIVED = 3
} TifileAttr;

typedef enum {
  TIFILE_SINGLE = 1, TIFILE_GROUP = 2, TIFILE_BACKUP = 4, TIFILE_FLASH = 8,
} TifileType;

typedef enum {
  ENCODING_ASCII = 1, ENCODING_LATIN1, ENCODING_UNICODE
} TifileEncoding;

// headers

#include "typesxx.h"
#include "filesxx.h"

// callbacks

typedef void (*TIFILES_MSGBOX) (const char *, char *);
typedef int (*TIFILES_CHOOSE) (char *, char *);

typedef int (*TIFILES_PRINTF) (const char *format, ...);

#endif
