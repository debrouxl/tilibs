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
#include "typesnsp.h"
#include "logging.h"

const char *NSP_CONST[NSP_MAXTYPES + 1][4] = {
	{"TNS", "tns", "Document", N_("TIicon1")},
	{"DIR", "???", "Directory", N_("Directory")},
	{NULL, NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *nsp_byte2type(uint8_t data)
{
	return (data < NSP_MAXTYPES) ? NSP_CONST[data][0] : "";
}

// Return the value corresponding to the type
uint8_t nsp_type2byte(const char *s)
{
	int i;

	for (i = 0; i < NSP_MAXTYPES; i++) 
	{
		if (!strcmp(NSP_CONST[i][0], s))
			break;
	}

	return i;
}

// Return the file extension corresponding to the value
const char *nsp_byte2fext(uint8_t data)
{
	return (data < NSP_MAXTYPES) ? NSP_CONST[data][1] : "";
}

// Return the value corresponding to the file extension
uint8_t nsp_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < NSP_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(NSP_CONST[i][1], s))
			break;
	}

	return i;
}

// Return the descriptive associated with the vartype
const char *nsp_byte2desc(uint8_t data)
{
	return (data < NSP_MAXTYPES) ? NSP_CONST[data][2] : _("Unknown");
}

// Return the icon name associated with the vartype
const char *nsp_byte2icon(uint8_t data)
{
	return (data < NSP_MAXTYPES) ? NSP_CONST[data][3] : "Unknown";
}
