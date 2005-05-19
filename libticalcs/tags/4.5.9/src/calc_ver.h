/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

#ifndef __CALC_VERSION__
#define __CALC_VERSION__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define LIBCALCS_REQUIRES_LIBFILES_VERSION  "0.5.8"	// useless with pkg-config
#define LIBCALCS_REQUIRES_LIBCABLES_VERSION "3.8.1"	// useless with pkg-config

#ifdef __WIN32__
# define LIBTICALCS_VERSION "4.5.9"
#else
# define LIBTICALCS_VERSION VERSION
#endif

#endif
