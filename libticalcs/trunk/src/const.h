/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef CONST_H
#define CONST_H

#include "calc_ext.h"

const char *group_file_ext(int calc_type);
const char *backup_file_ext(int calc_type);
const int   tixx_dir(int calc_type);
const int   tixx_flash(int calc_type);
const char *pak_name(int calc_type);
const char *flash_app_file_ext(int calc_type);
const char *flash_os_file_ext(int calc_type);

const struct ti_key ti89_keys(unsigned char ascii_code);
const struct ti_key ti92_keys(unsigned char ascii_code);

#endif
