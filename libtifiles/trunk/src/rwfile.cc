/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
  This unit contains some miscellaneous but useful functions.
*/

#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>

#include "tifiles.h"
#include "logging.h"
#include "rwfile.h"

/**********************/
/* Read/Write strings */
/**********************/

/*
   Read a string of 'n' chars max from a file
   - s [out]: a buffer for storing the string
   - f [in]: a file descriptor
   - [out]: -1 if error, 0 otherwise.
*/
int fread_n_chars(FILE * f, unsigned int n, char *s)
{
	if (fread_n_bytes(f, n, (uint8_t *)s) < 0)
	{
		return -1;
	}

	if (s != nullptr)
	{
		// set NULL terminator
		s[n] = '\0';
		// and set unused bytes to 0
		for (unsigned int i = strlen(s); i < n; i++)
		{
			s[i] = '\0';
		}
	}

	return 0;
}

/*
  Write a string of 'n' chars max (NULL padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: -1 if error, 0 otherwise.
*/
int fwrite_n_chars(FILE * f, unsigned int n, const char *s)
{
	unsigned int i;

	const unsigned int l = strlen(s);
	if (l > n)
	{
		tifiles_critical("string passed to 'fwrite_n_chars' is too long (>n chars).\n");
		tifiles_critical( "s = %s, len(s) = %u\n", s, l);
		tifiles_hexdump((uint8_t *)s, (l < n) ? n : l);
		return -1;
	}

	for (i = 0; i < l; i++)
	{
		if (fputc(s[i], f) == EOF)
		{
			return -1;
		}
	}
	for (i = l; i < n; i++)
	{
		if (fputc(0x00, f) == EOF)
		{
			return -1;
		}
	}

	return 0;
}

/*
  Write a string of 'n' chars max (SPC padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: -1 if error, 0 otherwise.
*/
int fwrite_n_chars2(FILE * f, unsigned int n, const char *s)
{
	unsigned int i;

	const unsigned int l = strlen(s);
	if (l > n)
	{
		tifiles_critical("string passed to 'fwrite_n_chars2' is too long (>n chars).\n");
		tifiles_critical( "s = %s, len(s) = %u\n", s, l);
		tifiles_hexdump((uint8_t *)s, (l < n) ? n : l);
		return -1;
	}

	for (i = 0; i < l; i++)
	{
		if (fputc(s[i], f) == EOF)
		{
			return -1;
		}
	}
	for (i = l; i < n; i++)
	{
		if (fputc(0x20, f) == EOF)
		{
			return -1;
		}
	}

	return 0;
}
