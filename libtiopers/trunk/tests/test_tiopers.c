/* $Id$ */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 2013-2016  Lionel Debroux
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

/*
 * This program shows how to use the libtiopers library. You can consider this
 * as an authoritative example. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ticables.h"
#include "tifiles.h"
#include "ticalcs.h"
#include "../src/tiopers.h"

#undef VERSION
#define VERSION "Test program"

int main(int argc, char **argv)
{
	// Force GLib 2.32+ to print info and debug messages like older versions did, unless this variable is already set.
	// No effect on earlier GLib versions.
	g_setenv("G_MESSAGES_DEBUG", "all", /* overwrite = */ FALSE);

	// init libs
	tifiles_library_init();
	ticables_library_init();
	ticalcs_library_init();
	tiopers_library_init();

// TODO !

	tiopers_library_exit();
	ticalcs_library_exit();
	ticables_library_exit();
	tifiles_library_exit();

	return 0;
}
