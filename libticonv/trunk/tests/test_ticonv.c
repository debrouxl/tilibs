/* Hey EMACS -*- linux-c -*- */
/* $Id: test_tifiles_2.c 1798 2006-02-03 08:27:54Z roms $ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
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

/*
  This program contains a lot of routines for testing various part of the
  library as well as checking file support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#define _DEBUG
#include "../src/ticonv.h"

unsigned long* charsets[] = {
    ti73_charset,
    ti82_charset,
    ti83_charset,
    ti83p_charset,
    ti85_charset,
    ti86_charset,
    ti9x_charset,
};

/*
  The main function
*/
int main(int argc, char **argv)
{
	int i, j;
	int n = 0;
	int m = sizeof(charsets) / sizeof(unsigned long*);

	printf("m = %i\n", m);

	// test ticonv.c
	printf("Library version : <%s>\n", ticonv_version_get());
	printf("--\n");

	printf("Choose your charset: ");
	if(!scanf("%i", &n))
	    n = 0;
	if(n >= m)
	    n = m-1;

	printf("  0 1 2 3 4 5 6 7 8 9 A B C D E F\n");

	for(i = 0; i < 16; i++)
	{
		printf("%i ", i);
		for(j = 0; j < 16; j++)
		{
		    unsigned long wc = charsets[n][16*i+j];
		    gchar *str = NULL;

		    if(wc && wc != '\n')
		    {
			gunichar2 buf[4] = { 0 };

			buf[0] = wc;
			str = ticonv_utf16_to_utf8(buf);
		    }
		    else
		    {
			str = NULL;
		    }

		    if(str)
			printf("%s ", str);
		    else
			printf("");
		    
		    g_free(str);
		}
		printf("\n");
	}

	return 0;
}
