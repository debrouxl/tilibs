/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2009  Romain Lievin
 *  Copyright (C) 2009-2018  Lionel Debroux
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

#ifndef __TICONV_LOGGING_H__
#define __TICONV_LOGGING_H__

#include <glib.h>

#define ticonv_debug(format, ...) g_log("ticonv", G_LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define ticonv_info(format, ...) g_log("ticonv", G_LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define ticonv_warning(format, ...) g_log("ticonv", G_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define ticonv_critical(format, ...) g_log("ticonv", G_LOG_LEVEL_CRITICAL, format, ##__VA_ARGS__)

#endif
