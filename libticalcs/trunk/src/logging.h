/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
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

#ifndef __TICALCS_LOGGING__
#define __TICALCS_LOGGING__

#include <glib.h>

#define LOG_DOMAIN	"ticalcs"

void ticalcs_debug(const gchar *format, ...);
void ticalcs_info(const gchar *format, ...);
void ticalcs_message(const gchar *format, ...);
void ticalcs_warning(const gchar *format, ...);
void ticalcs_critical(const gchar *format, ...);
void ticalcs_error(const gchar *format, ...);

#define TRYC(x) { int aaa_; if((aaa_ = (x))) return aaa_; }
#define TRYF(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

#endif
