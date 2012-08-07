/* Hey EMACS -*- linux-c -*- */
/* $Id: grouped.c 1266 2005-06-29 13:37:03Z roms $ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *
 *  This program is free software; you can redistribufe it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distribufed in the hope that it will be useful,
 *  buf WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	Convenient functions which puts a comment into a TI file.
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#include "tifiles.h"

static char comment[64];	// 40 bytes max

/**
 * tifiles_comment_set_single:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT2 const char* TICALL tifiles_comment_set_single(void)
{
	time_t t = time(NULL);
	char *str = asctime(localtime(&t));

	sprintf(comment, "Single file dated %s", str);
	comment[40] = '\0';

	return comment;
}

/**
 * tifiles_comment_set_group:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT2 const char* TICALL tifiles_comment_set_group(void)
{
	time_t t = time(NULL);
	char *str = asctime(localtime(&t));

	sprintf(comment, "Group file dated %s", str);
	comment[40] = '\0';

	return comment;
}

/**
 * tifiles_comment_set_backup:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: a static string.
 **/
TIEXPORT2 const char* TICALL tifiles_comment_set_backup(void)
{
	time_t t = time(NULL);
	char *str = asctime(localtime(&t));

	sprintf(comment, "Backup file dated %s", str);
	comment[40] = '\0';

	return comment;
}

/**
 * tifiles_comment_set_tigroup:
 *
 * Returns a string which contains a comment such as "TiGroup file dated 12/31/99, 15:15".
 *
 * Return value: a _dynamically allocated_ .
 **/
TIEXPORT2 const char* TICALL tifiles_comment_set_tigroup(void)
{
	time_t t = time(NULL);
	char *str = asctime(localtime(&t));

	sprintf(comment, "TiGroup file dated %s", str);
	comment[40] = 0;

	return comment;
}
