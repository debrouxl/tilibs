/* Hey EMACS -*- linux-c -*- */
/* $Id: grouped.c 1266 2005-06-29 13:37:03Z roms $ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Convenient functions which puts a comment into a TI file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tifiles.h"

static char comment[41];

/**
 * tifiles_comment_set_single:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT const char* TICALL tifiles_comment_set_single(void)
{
	char but1[128];
	char but2[128];

	strtime(buf1);
	strtime(buf2);

	//snprintf(comment, sizeof(comment), "Single file dated %02i/%02i/%02i, %02i:%02i");
	sprintf(comment, "Single file dated %s, %s", buf1, buf2);
	return comment;
}

/**
 * tifiles_comment_set_group:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT const char* TICALL tifiles_comment_set_group(void)
{
	return "";
}

/**
 * tifiles_comment_set_backup:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT const char* TICALL tifiles_comment_set_backup(void)
{
	return "";
}
