/*  libtifiles - TI File Format library
 *  Copyright (C) 2002  Romain Lievin
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

#define TIFILES_NCALCS 10 // # of supported calcs

#define CALC_NONE  0
#define CALC_TI92P 1
#define CALC_TI92  2
#define CALC_TI89  3
#define CALC_TI86  4
#define CALC_TI85  5
#define CALC_TI83P 6
#define CALC_TI83  7
#define CALC_TI82  8
#define CALC_TI73  9
#define CALC_V200 10

#define ATTRB_NONE      0
#define ATTRB_LOCKED    1 // don't change this value !
#define ATTRB_ARCHIVED  3 // don't change this value !
#define ATTRB_PROTECTED 2

#define TIFILE_SINGLE 1
#define TIFILE_GROUP  2
#define TIFILE_BACKUP 4
#define TIFILE_FLASH  8

// headers

#include "typesxx.h"
#include "filesxx.h"

// callbacks

typedef void (*TIFILES_MSGBOX) (const char *, char *);
typedef int  (*TIFILES_CHOOSE) (char *, char *);

typedef int (*TIFILES_PRINTF) (const char *format, ...);

#endif
