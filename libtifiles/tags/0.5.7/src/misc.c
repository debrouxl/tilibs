/* Hey EMACS -*- linux-c -*- */
/*  libtifiles - TI File Format library
 *  Copyright (C) 2002-2003  Romain Lievin
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
#include "stdints.h"
#include <sys/stat.h>

#include "export.h"
#include "file_int.h"

#define bswap_16(a) (a >> 8) | (a << 8)

#define bswap_32(x) (x >> 24) | (x & 0xff0000) >> 8 | (x & 0xff00) << 8 | (x & 0xff) << 24

/*
  Dump into hexadecimal format the content of a buffer
  - ptr [in]: a pointer on some data to dump
  - len [in]: the number of bytes to dump
  - [out]: always 0
 */
TIEXPORT int TICALL hexdump(uint8_t * ptr, int len)
{
  int i;

  for (i = 0; i < len; i++)
    fprintf(stdout, "%02X ", ptr[i]);
  fprintf(stdout, "\n");

  return 0;
}

/**********************/
/* Read/Write strings */
/**********************/

/*
   Read a string of 'n' chars from a file
   - s [out]: a buffer for storing the string
   - f [in]: a file descriptor
   - [out]: the result of the operation (0 if failed)
*/
int fread_n_chars(FILE * f, int n, char *s)
{
  int i;

  if (s == NULL) {
    for (i = 0; i < n; i++)
      fgetc(f);
  } else {
    for (i = 0; i < n; i++)
      s[i] = 0xff & fgetc(f);
    s[i] = '\0';
  }

  return 0;
}

/*
  Write a string of 'n' chars (NULL padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: always different of 0
*/
#define FOO
int fwrite_n_chars(FILE * f, int n, const char *s)
{
#ifdef FOO
  int i;
  int l = n;

  l = strlen(s);
  if (l > n) {
    fprintf(stderr,
	    "libtifiles error: string passed in 'write_string8' is too long (>n chars).\n");
    printf("s = <%s>, len(s) = %i\n", s, strlen(s));
    hexdump((uint8_t *) s, (strlen(s) < 9) ? 9 : strlen(s));
    abort();
  }

  for (i = 0; i < l; i++)
    fputc(s[i], f);
  for (i = l; i < n; i++) {
    fputc(0x00, f);
  }
#else
  int i;

  for (i = 0; i < n; i++)
    fputc((int) s[i], f);
#endif
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
  /*
     int i;
     for(i=0; i<n; i++)
     fgetc(f);
     return 0;
   */
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
#ifdef WORDS_BIGENDIAN
    *data = bswap_16(*data);
#endif /* WORDS_BIGENDIAN */
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
#ifdef WORDS_BIGENDIAN
    *data = bswap_32(*data);
#endif /* WORDS_BIGENDIAN */
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
#ifdef WORDS_BIGENDIAN
  data = bswap_16(data);
#endif /* WORDS_BIGENDIAN */
  return fwrite(&data, sizeof(uint16_t), 1, f);
}

int fwrite_long(FILE * f, uint32_t data)
{
#ifdef WORDS_BIGENDIAN
  data = bswap_32(data);
#endif /* WORDS_BIGENDIAN */
  return fwrite(&data, sizeof(uint32_t), 1, f);
}

/****************/
/* Miscelaneous */
/****************/

/*
  Retrieve the extension of a file
  - filename [in]: a filename
  - ext [out]: the extension
  - [out]: the extension
*/
TIEXPORT char *TICALL tifiles_get_extension(const char *filename)
{
  char *d = NULL;

  d = strrchr(filename, '.');
  if (d == NULL)
    return NULL;

  return (++d);
}

TIEXPORT char *TICALL tifiles_dup_extension(const char *filename)
{
  char *ext = tifiles_get_extension(filename);

  if (ext != NULL)
    return strdup(tifiles_get_extension(filename));
  else
    return strdup("");
}

/* 
   Compute the checksum of a uint8_t array. Returns a uint16_t value.
   - buffer [in]: an array of uint8_t values
   - size [in]: the array size
   - chk [out]: the computed checksum
   - [out]: the computed checksum
*/
TIEXPORT uint16_t TICALL tifiles_compute_checksum(uint8_t * buffer,
						  int size)
{
  int i;
  uint16_t c = 0;

  if (buffer == NULL)
    return 0;

  for (i = 0; i < size; i++)
    c += buffer[i];

  return c;
}


/*
  Retrieve the varname component of a full path
   - full_name [in]: a stringsuch as 'fldname\varname'
   - [out]: the varname
*/
char *TICALL tifiles_get_varname(const char *full_name)
{
  char *bs = strchr(full_name, '\\');

  if (bs == NULL)
    return (char *) full_name;
  else
    return (++bs);
}


/*
  Retrieve the folder component of a full path (fldname\varname).
   - full_name [in]: a string such as 'fldname\varname'
   - [out]: the folder name (don't need to be freed)
*/
char *TICALL tifiles_get_fldname(const char *full_name)
{
  static char folder[9];
  char *bs = strchr(full_name, '\\');
  int i;

  if (bs == NULL)
    strcpy(folder, "");
  else {
    i = strlen(full_name) - strlen(bs);
    strncpy(folder, full_name, i);
    folder[i + 1] = '\0';
  }
  return folder;
}


/*
  Build the complete path starting at varname & folder name.
  This function is calculator independant.
   - full_name [out]: a string such as 'fldname\varname'
   - fldname [in]: the folder name or "" (local -> no path)
   - varname [in]: the variable name
   - [out]: aalways 0.
*/
extern TicalcType tifiles_calc_type;
int TICALL tifiles_build_fullname(char *full_name,
				  const char *fldname, const char *varname)
{
  if (tifiles_has_folder(tifiles_calc_type)) {
    if (strcmp(fldname, "")) {
      strcpy(full_name, fldname);
      strcat(full_name, "\\");
    }
    strcat(full_name, varname);
  } else
    strcpy(full_name, varname);

  return 0;
}

int is_regfile(const char *filename)
{
#ifndef __WIN32__
  struct stat buf;

  if (stat(filename, &buf) < 0)
    return 0;

  if (S_ISREG(buf.st_mode))
    return !0;
  else
    return 0;
#else
  return !0;
#endif
}
