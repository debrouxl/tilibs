/* Hey EMACS -*- linux-c -*- */
/* $Id: typesxx.c 912 2005-03-30 20:49:06Z roms $ */

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

#include <stdio.h>
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "fileops.h"

/****************/
/* Global types */
/****************/

#define NCALCS TIFILES_NCALCS

static const char GROUP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73g", "82g", "83g", "8Xg", "8Xg", "85g", "86g", 
	"89g", "89g", "92g", "9Xg", "v2g",
};

static const char BACKUP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73b", "82b", "83b", "8Xb", "8Xb", "85b", "86b", 
	"89g", "89g", "92b", "9Xg", "v2g",
};

static const char FLASH_APP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73k", "???", "???", "8Xk", "8Xk", "???", "???",
	"89k", "89k", "???", "9Xk", "v2k",
};

static const char FLASH_OS_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73u", "???", "???", "8Xu", "8Xu", "???", "???",
	"89u", "89u", "???", "9Xu", "v2u",
};

/*******************/
/* File extensions */
/*******************/

TIEXPORT const char *TICALL tifiles_group_file_ext(TiCalcType model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "??g";
  case CALC_TI73:
    return "73g";
  case CALC_TI82:
    return "82g";
  case CALC_TI83:
    return "83g";
  case CALC_TI83P:
  case CALC_TI84P:
    return "8Xg";
  case CALC_TI85:
    return "85g";
  case CALC_TI86:
    return "86g";
  case CALC_TI89:
  case CALC_TI89T:
    return "89g";
  case CALC_TI92:
    return "92g";
  case CALC_TI92P:
    return "9Xg";
  case CALC_V200:
    return "v2g";
  default:
    tifiles_error("tifiles_group_file_ext: invalid calc_type argument.");
    break;
  }

  return NULL;
}

TIEXPORT const char *TICALL tifiles_backup_file_ext(TiCalcType model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "??b";
  case CALC_TI73:
    return "73b";
  case CALC_TI82:
    return "82b";
  case CALC_TI83:
    return "83b";
  case CALC_TI83P:
  case CALC_TI84P:
    return "8Xb";
  case CALC_TI85:
    return "85b";
  case CALC_TI86:
    return "86b";
  case CALC_TI89:
  case CALC_TI89T:
    return "89g";
  case CALC_TI92:
    return "92b";
  case CALC_TI92P:
    return "9Xg";
  case CALC_V200:
    return "v2g";
  default:
    tifiles_error("tifiles_backup_file_ext: invalid calc_type argument.");
    break;
  }

  return NULL;
}

TIEXPORT const char *TICALL tifiles_flash_app_file_ext(TiCalcType model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "??k";
  case CALC_TI73:
    return "73k";
  case CALC_TI82:
    return "???";
  case CALC_TI83:
    return "???";
  case CALC_TI83P:
  case CALC_TI84P:
    return "8Xk";
  case CALC_TI85:
    return "???";
  case CALC_TI86:
    return "???";
  case CALC_TI89:
  case CALC_TI89T:
    return "89k";
  case CALC_TI92:
    return "???";
  case CALC_TI92P:
    return "9Xk";
  case CALC_V200:
    return "v2k";
  default:
    tifiles_error("tifiles_flash_app_file_ext: invalid calc_type argument.");
    break;
  }

  return NULL;
}

TIEXPORT const char *TICALL tifiles_flash_os_file_ext(TiCalcType model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "??u";
  case CALC_TI73:
    return "73u";
  case CALC_TI82:
    return "???";
  case CALC_TI83:
    return "???";
  case CALC_TI83P:
  case CALC_TI84P:
    return "8Xu";
  case CALC_TI85:
    return "???";
  case CALC_TI86:
    return "???";
  case CALC_TI89:
  case CALC_TI89T:
    return "89u";
  case CALC_TI92:
    return "???";
  case CALC_TI92P:
    return "9Xu";
  case CALC_V200:
    return "v2u";
  default:
    tifiles_error("tifiles_flash_os_file_ext: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**************/
/* File types */
/**************/

static int is_regfile(const char *filename)
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

#define TIB_SIGNATURE	"Advanced Mathematics Software"

/*
  Check if the file is a valid TI file by
  reading its signature
  - filename [in]: a file name
  - int [out]: TRUE if valid file, FALSE otherwise
 */
TIEXPORT int TICALL tifiles_is_a_ti_file(const char *filename)
{
  FILE *f;
  char buf[9];
  char str[64];

  // bug: check the file is not an fifo
  if (!is_regfile(filename))
    return 0;

  f = fopen(filename, "rb");
  if (f == NULL)
	  return ERR_FILE_OPEN;

  // read header
  fread_8_chars(f, buf);
  if (!strcmp(buf, "**TI73**") || !strcmp(buf, "**TI82**") ||
      !strcmp(buf, "**TI83**") || !strcmp(buf, "**TI83F*") ||
      !strcmp(buf, "**TI85**") || !strcmp(buf, "**TI86**") ||
      !strcmp(buf, "**TI89**") || !strcmp(buf, "**TI92**") ||
      !strcmp(buf, "**TI92P*") || !strcmp(buf, "**V200**") ||
      !strcmp(buf, "**TIFL**")) {
    fclose(f);
    return !0;
  }

  // check for TIB file
  fread_n_chars(f, 14, str);
  fread_n_chars(f, strlen(TIB_SIGNATURE), str);
  str[strlen(TIB_SIGNATURE)] = '\0';
  if(!strcmp(str, TIB_SIGNATURE)) 
  {
	fclose(f);
	return !0;
  }

  fclose(f);
  return 0;
}

/*
  Check whether it is a single file
  - filename [in]: a file name
  - int [out]: TRUE if group file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_single_file(const char *filename)
{
  if (!tifiles_is_a_ti_file(filename))
    return 0;

  if (tifiles_is_a_group_file(filename) ||
      tifiles_is_a_backup_file(filename) ||
      tifiles_is_a_flash_file(filename))
    return 0;
  else
    return !0;
}

/* 
   Check whether it is a group file
   - filename [in]: a file name
   - int [out]: TRUE if group file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_group_file(const char *filename)
{
  int i;
  char *e = tifiles_get_extension(filename);

  if (e == NULL)
    return 0;

  if (!tifiles_is_a_ti_file(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if (!g_ascii_strcasecmp(e, GROUP_FILE_EXT[i]))
      return !0;
  }

  return 0;
}

/*
   Check whether it is a regular file (single | group)
   - filename [in]: a file name
   - int [out]: TRUE if group file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_regular_file(const char *filename)
{
  if (!tifiles_is_a_ti_file(filename))
    return 0;

  return (tifiles_is_a_single_file(filename) ||
	  tifiles_is_a_group_file(filename));
}

/* 
   Check whether it is a backup file
   - filename [in]: a file name
   - int [out]: TRUE if backup file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_backup_file(const char *filename)
{
  int i;
  char *e = tifiles_get_extension(filename);

  if (e == NULL)
    return 0;

  if (!tifiles_is_a_ti_file(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if (!g_ascii_strcasecmp(e, BACKUP_FILE_EXT[i]))
      return !0;
  }

  return 0;
}

/* 
   Check whether it is a flash file
   - filename [in]: a file name
   - int [out]: TRUE if flash file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_flash_file(const char *filename)
{
  int i;
  char *e = tifiles_get_extension(filename);

  if (e == NULL)
    return 0;

  if (!tifiles_is_a_ti_file(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if ((!g_ascii_strcasecmp(e, FLASH_APP_FILE_EXT[i])) ||
	(!g_ascii_strcasecmp(e, FLASH_OS_FILE_EXT[i])))
      return !0;
  }

  return 0;
}

/* 
   Check whether it is a tib file
   - filename [in]: a file name
   - int [out]: TRUE if tib file, FALSE otherwise
*/
TIEXPORT int TICALL tifiles_is_a_tib_file(const char *filename)
{
	char *e = tifiles_get_extension(filename);

	if (e == NULL)
	  return 0;

	if (!tifiles_is_a_ti_file(filename))
		return 0;

	if(!g_ascii_strcasecmp(e, "tib"))
		return !0;

	// no need to do more test, TIB signature has already been checked 
	// by is_a_ti_file()

	return 0;
}
