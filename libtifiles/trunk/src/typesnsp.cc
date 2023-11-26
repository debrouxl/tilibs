/* Hey EMACS -*- linux-c -*- */
/* $Id: typesv2.c 3056 2006-11-06 17:12:50Z roms $ */

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

/*
  Variable type ID and file extensions
*/

#include <string.h>
#include "gettext.h"
#include "logging.h"
#include "internal.h"

const TIXX_DATA NSP_CONST[NSP_MAXTYPES + 1] =
{
	{"TNS", "tns", "TIicon1", N_("Document")},
	{"DIR", "???", "Directory", N_("Directory")},
	{ nullptr, nullptr, nullptr, nullptr }
};
