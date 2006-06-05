/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
  This unit contains some miscellaneous but useful functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdints2.h"
#include <sys/stat.h>

#include "tifiles.h"
#include "logging.h"

// cut'ed from GLib 2.6.6 because this code work but the g_fopen call doesn't ?!
#define G_WIN32_IS_NT_BASED() (g_win32_get_windows_version () < 0x80000000)
#define G_WIN32_HAVE_WIDECHAR_API() (G_WIN32_IS_NT_BASED ())

FILE *
gfopen (const gchar *filename, const gchar *mode)
{
#ifdef __WIN32__
  if (G_WIN32_HAVE_WIDECHAR_API ())
    {
      wchar_t *wfilename = g_utf8_to_utf16 (filename, -1, NULL, NULL, NULL);
      wchar_t *wmode;
      FILE *retval;
      int save_errno;

      if (wfilename == NULL)
	{
	  errno = -1;
	  return NULL;
	}

      wmode = g_utf8_to_utf16 (mode, -1, NULL, NULL, NULL);

      if (wmode == NULL)
	{
	  g_free (wfilename);
	  errno = -1;
	  return NULL;
	}
	
      retval = _wfopen (wfilename, wmode);
      save_errno = errno;

      g_free (wfilename);
      g_free (wmode);

      errno = save_errno;
      return retval;
    }
  else
    {
      gchar *cp_filename = g_locale_from_utf8 (filename, -1, NULL, NULL, NULL);
      FILE *retval;
      int save_errno;

      if (cp_filename == NULL)
	{
	  errno = -1;
	  return NULL;
	}

      retval = fopen (cp_filename, mode);
      save_errno = errno;

      g_free (cp_filename);

      errno = save_errno;
      return retval;
    }
#else
  return fopen (filename, mode);
#endif
}

/*
  Dump into hexadecimal format the content of a buffer
  - ptr [in]: a pointer on some data to dump
  - len [in]: the number of bytes to dump
  - [out]: always 0
 */
int hexdump(uint8_t * ptr, int len)
{
	char *str = (char *)malloc(3*len + 8);
	int i;
  
	for (i = 0; i < len; i++)
		sprintf(&str[3*i], "%02X ", ptr[i]);
	sprintf(&str[3*i], "(%i)", len);
	tifiles_info(str);

  return 0;
}

/**********************/
/* Read/Write strings */
/**********************/

/*
   Read a block of 'n' n bytes from a file
   - s [out]: a buffer for storing the data
   - f [in]: a file descriptor
   - [out]: the result of the operation (0 if failed)
*/
int fread_n_bytes(FILE * f, int n, char *s)
{
  int i;

  if (s == NULL) 
  {
    for (i = 0; i < n; i++)
      fgetc(f);
  } 
  else 
  {
    for (i = 0; i < n; i++)
      s[i] = 0xff & fgetc(f);
    s[i] = '\0';
  }

  return 0;
}

/*
  Write a string of 'n' chars max (NULL padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: always different of 0
*/
int fwrite_n_bytes(FILE * f, int n, const char *s)
{
  int i;

  for (i = 0; i < n; i++)
    fputc(s[i], f);

  return 0;
}


/*
   Read a string of 'n' chars max from a file
   - s [out]: a buffer for storing the string
   - f [in]: a file descriptor
   - [out]: the result of the operation (0 if failed)
*/
int fread_n_chars(FILE * f, int n, char *s)
{
	int i;
	int ret;
	
	ret = fread_n_bytes(f, n, s);

	if(s != NULL)
	{	
		// not compulsory but I prefer set unused bytes to 0
		for(i = strlen(s); i < n; i++)
			s[i] = '\0';
	}

	return ret;
}

/*
  Write a string of 'n' chars max (NULL padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: always different of 0
*/
int fwrite_n_chars(FILE * f, int n, const char *s)
{
  int i;
  int l = n;

  l = strlen(s);
  if (l > n) 
  {
    tifiles_error("string passed in 'write_string8' is too long (>n chars).\n");
    tifiles_error( "s = <%s>, len(s) = %i\n", s, strlen(s));
    hexdump((uint8_t *) s, (strlen(s) < 9) ? 9 : strlen(s));
    abort();
  }

  for (i = 0; i < l; i++)
    fputc(s[i], f);
  for (i = l; i < n; i++) 
    fputc(0x00, f);

  return 0;
}

/*
  Write a string of 'n' chars max (SPC padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: always different of 0
*/
int fwrite_n_chars2(FILE * f, int n, const char *s)
{
  int i;
  int l = n;

  l = strlen(s);
  if (l > n) 
  {
    tifiles_error("string passed in 'write_string8' is too long (>n chars).\n");
    tifiles_error( "s = <%s>, len(s) = %i\n", s, strlen(s));
    hexdump((uint8_t *) s, (strlen(s) < 9) ? 9 : strlen(s));
    abort();
  }

  for (i = 0; i < l; i++)
    fputc(s[i], f);
  for (i = l; i < n; i++) 
    fputc(0x20, f);

  return 0;
}


int fread_8_chars(FILE * f, char *s)
{
  return fread_n_chars(f, 8, s);
}

int fwrite_8_chars(FILE * f, const char *s)
{
  return fwrite_n_chars(f, 8, s);
}

int fskip(FILE * f, int n)
{
  return fseek(f, n, SEEK_CUR);
}

/***************************/
/* Read byte/word/longword */
/***************************/

int fread_byte(FILE * f, uint8_t * data)
{
  if (data != NULL)
    return fread((void *) data, sizeof(uint8_t), 1, f);
  else
    fskip(f, 1);

  return 0;
}

int fread_word(FILE * f, uint16_t * data)
{
  int ret = 0;
  if (data != NULL)
  {
    ret = fread((void *) data, sizeof(uint16_t), 1, f);
	*data = GUINT16_FROM_LE(*data);
  }
  else
    fskip(f, 2);

  return ret;
}

int fread_long(FILE * f, uint32_t * data)
{
  int ret = 0;
  if (data != NULL)
  {
    ret = fread((void *) data, sizeof(uint32_t), 1, f);
	*data = GUINT32_FROM_LE(*data);
  }
  else
    fskip(f, 4);

  return ret;
}

/****************************/
/* Write byte/word/longword */
/****************************/

int fwrite_byte(FILE * f, uint8_t data)
{
  return fwrite(&data, sizeof(uint8_t), 1, f);
}

int fwrite_word(FILE * f, uint16_t data)
{
	data = GUINT16_TO_LE(data);
  return fwrite(&data, sizeof(uint16_t), 1, f);
}

int fwrite_long(FILE * f, uint32_t data)
{
	data = GUINT32_TO_LE(data);
  return fwrite(&data, sizeof(uint32_t), 1, f);
}
