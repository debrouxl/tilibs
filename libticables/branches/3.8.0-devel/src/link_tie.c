/* Hey EMACS -*- linux-c -*- */
/* $Id: tie_link.c 370 2004-03-22 18:47:32Z roms $ */

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

/* TiEmu virtual link */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#if defined(__LINUX__)
#include "linux/tie_link.c"

#elif defined(__BSD__)
#include "linux/tie_link.c"

#elif defined(__WIN32__)
#include "win32/tie_link.c"

#elif defined(__MACOSX__)
#include "linux/tie_link.c"

#else
#endif
