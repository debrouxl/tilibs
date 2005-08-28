/* Hey EMACS -*- linux-c -*- */
/* $Id: slv_link.c 370 2004-03-22 18:47:32Z roms $ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

/* TI-GRAPH LINK USB support */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#if defined(__LINUX__)
static int max_ps = 32; // max packet size (32 or 64)
#include "linux/slv_link.c"
#if defined(HAVE_LIBUSB)
#include "linux/slv_link2.c"
#endif

#elif defined(__BSD__)
static int max_ps = 32; // max packet size (32 or 64)
#include "linux/slv_link2.c"

#elif defined(__WIN32__)
#include "win32/slv_link.c"

#elif defined(__MACOSX__)
#include "macos/slv_link.c"

#else
#endif
