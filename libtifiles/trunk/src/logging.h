/* Hey EMACS -*- linux-c -*- */
/* $Id: print.h 522 2004-04-08 10:12:55Z roms $ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifndef __TIFILES_LOGGING_H__
#define __TIFILES_LOGGING_H__

#include <glib.h>

#define LOG_DOMAIN	"tifiles"

void tifiles_debug(const gchar *format, ...);
void tifiles_info(const gchar *format, ...);
void tifiles_message(const gchar *format, ...);
void tifiles_warning(const gchar *format, ...);
void tifiles_critical(const gchar *format, ...);
void tifiles_error(const gchar *format, ...);

#define TRYC(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

#endif
