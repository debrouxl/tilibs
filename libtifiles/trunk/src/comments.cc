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

// Helper function for factoring code.
static inline char * tifiles_comment_set_sn(const char * prefix, char * comment, uint32_t maxlen)
{
	if (nullptr != comment && maxlen > 0)
	{
		char * str;
		time_t t = time(nullptr);
#ifdef HAVE_CTIME_R
		char datetime[64];
		ctime_r(&t, datetime);
		str = datetime;
#else
		str = ctime(&t);
#endif
		maxlen = (maxlen >= 41) ? 41 : maxlen;
		// This truncation is intentional, and we want the compiler to shut up, so make sure to use the result of snprintf...
		if (maxlen <= (uint32_t)snprintf(comment, maxlen, "%s%s", prefix, str))
		{
			t = 0;
		}
		comment[maxlen - 1] = 0;
	}

	return comment;
}

/**
 * tifiles_comment_set_single:
 *
 * Use tifiles_comment_set_single_sn instead.
 *
 * Return value: a static string.
 **/
const char* TICALL tifiles_comment_set_single(void)
{
	static char comment[64];
	return tifiles_comment_set_single_sn(comment, 41);
}

/**
 * tifiles_comment_set_single_sn:
 * @comment: destination buffer for storing the comment, needs to be at least 41 chars long.
 * @maxlen: maximum length to be written.
 *
 * Stores a comment such as "Group file dated 12/31/99, 15:15" into the maxlen first bytes of the given buffer.
 *
 * Return value: the given comment buffer.
 **/
char* TICALL tifiles_comment_set_single_sn(char * comment, uint32_t maxlen)
{
	return tifiles_comment_set_sn("Single file dated ", comment, maxlen);
}

/**
 * tifiles_comment_set_group:
 *
 * Use tifiles_comment_set_group_sn instead.
 *
 * Return value: a static string.
 **/
const char* TICALL tifiles_comment_set_group(void)
{
	static char comment[64];
	return tifiles_comment_set_group_sn(comment, 41);
}

/**
 * tifiles_comment_set_group_sn:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: the given comment buffer.
 **/
char* TICALL tifiles_comment_set_group_sn(char * comment, uint32_t maxlen)
{
	return tifiles_comment_set_sn("Group file dated ", comment, maxlen);
}

/**
 * tifiles_comment_set_backup:
 *
 * Use tifiles_comment_set_backup_sn instead.
 *
 * Return value: a static string.
 **/
const char* TICALL tifiles_comment_set_backup(void)
{
	static char comment[64];
	return tifiles_comment_set_backup_sn(comment, 41);
}

/**
 * tifiles_comment_set_backup_sn:
 *
 * Returns a string which contains a comment such as "Group file dated 12/31/99, 15:15".
 *
 * Return value: the given comment buffer.
 **/
char* TICALL tifiles_comment_set_backup_sn(char * comment, uint32_t maxlen)
{
	return tifiles_comment_set_sn("Backup file dated ", comment, maxlen);
}

/**
 * tifiles_comment_set_tigroup:
 *
 * Use tifiles_comment_set_tigroup_sn instead.
 *
 * Return value: a static string.
 **/
const char* TICALL tifiles_comment_set_tigroup(void)
{
	static char comment[64];
	return tifiles_comment_set_tigroup_sn(comment, 41);
}

/**
 * tifiles_comment_set_tigroup_sn:
 *
 * Returns a string which contains a comment such as "TiGroup file dated 12/31/99, 15:15".
 *
 * Return value: the given comment buffer.
 **/
char* TICALL tifiles_comment_set_tigroup_sn(char * comment, uint32_t maxlen)
{
	return tifiles_comment_set_sn("TiGroup file dated ", comment, maxlen);
}
