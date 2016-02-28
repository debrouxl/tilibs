/* Hey EMACS -*- linux-c -*- */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 2015  Lionel Debroux
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

/* TCP client virtual link */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef NO_CABLE_TCPC

#if defined(__LINUX__)
#include "linux/link_tcpc.c"

#elif defined(__BSD__)
#include "linux/link_tcpc.c"

#elif defined(__WIN32__)
/* Not ported yet */

#elif defined(__MACOSX__)
#include "linux/link_tcpc.c"

#else
#endif

#endif
