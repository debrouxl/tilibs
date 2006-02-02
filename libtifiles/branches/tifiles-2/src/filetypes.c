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

#include <glib/gstdio.h>	// replace fopen by g_fopen which is locale independant
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gettext.h"
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "rwfile.h"
#include "typesxx.h"

/****************/
/* Global types */
/****************/

#define NCALCS FILES_NCALCS

static const char GROUP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73g", "82g", "83g", "8Xg", "8Xg", "85g", "86g", 
	"89g", "89g", "92g", "9Xg", "V2g",
};

static const char BACKUP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73b", "82b", "83b", "8Xb", "8Xb", "85b", "86b", 
	"89g", "89g", "92b", "9Xg", "V2g",
};

static const char FLASH_APP_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73k", "???", "???", "8Xk", "8Xk", "???", "???",
	"89k", "89k", "???", "9Xk", "V2k",
};

static const char FLASH_OS_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73u", "???", "???", "8Xu", "8Xu", "???", "???",
	"89u", "89u", "???", "9Xu", "V2u",
};

static const char CERTIF_FILE_EXT[NCALCS + 1][4] = 
{
	"XxX", 
	"73q", "???", "???", "8Xq", "8Xq", "???", "???",
	"89q", "89q", "???", "9Xq", "V2q",
};

/*******************/
/* File extensions */
/*******************/

/**
 * tifiles_fext_of_group:
 * @model: a calculator model.
 *
 * Returns file extension of a group file.
 *
 * Return value: a file extenstion as string (like "83g").
 **/
TIEXPORT const char *TICALL tifiles_fext_of_group (CalcModel model)
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
  case CALC_TI84P_USB:
    return "8Xg";
  case CALC_TI85:
    return "85g";
  case CALC_TI86:
    return "86g";
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return "89g";
  case CALC_TI92:
    return "92g";
  case CALC_TI92P:
    return "9Xg";
  case CALC_V200:
    return "V2g";
  default:
    tifiles_error("tifiles_fext_of_group: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**
 * tifiles_fext_of_backup:
 * @model: a calculator model.
 *
 * Returns file extension of a backup file.
 *
 * Return value: a file extenstion as string (like "83b").
 **/
TIEXPORT const char *TICALL tifiles_fext_of_backup (CalcModel model)
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
	  case CALC_TI84P_USB:
    return "8Xb";
  case CALC_TI85:
    return "85b";
  case CALC_TI86:
    return "86b";
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return "89g";
  case CALC_TI92:
    return "92b";
  case CALC_TI92P:
    return "9Xg";
  case CALC_V200:
    return "V2g";
  default:
    tifiles_error("tifiles_fext_of_backup: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**
 * tifiles_fext_of_flash_app:
 * @model: a calculator model.
 *
 * Returns file extension of a FLASH application file.
 *
 * Return value: a file extenstion as string (like "89k").
 **/
TIEXPORT const char *TICALL tifiles_fext_of_flash_app (CalcModel model)
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
	  case CALC_TI84P_USB:
    return "8Xk";
  case CALC_TI85:
    return "???";
  case CALC_TI86:
    return "???";
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return "89k";
  case CALC_TI92:
    return "???";
  case CALC_TI92P:
    return "9Xk";
  case CALC_V200:
    return "V2k";
  default:
    tifiles_error("tifiles_fext_of_flash_app: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**
 * tifiles_fext_of_flash_os:
 * @model: a calculator model.
 *
 * Returns file extension of a FLASH Operating System file.
 *
 * Return value: a file extenstion as string (like "89u").
 **/
TIEXPORT const char *TICALL tifiles_fext_of_flash_os(CalcModel model)
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
	  case CALC_TI84P_USB:
    return "8Xu";
  case CALC_TI85:
    return "???";
  case CALC_TI86:
    return "???";
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return "89u";
  case CALC_TI92:
    return "???";
  case CALC_TI92P:
    return "9Xu";
  case CALC_V200:
    return "V2u";
  default:
    tifiles_error("tifiles_fext_of_flash_os: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**
 * tifiles_fext_of_certif:
 * @model: a calculator model.
 *
 * Returns file extension of certificate file.
 *
 * Return value: a file extenstion as string (like "89q").
 **/
TIEXPORT const char *TICALL tifiles_fext_of_certif(CalcModel model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "??q";
  case CALC_TI73:
    return "73q";
  case CALC_TI82:
    return "???";
  case CALC_TI83:
    return "???";
  case CALC_TI83P:
  case CALC_TI84P:
	  case CALC_TI84P_USB:
    return "8Xq";
  case CALC_TI85:
    return "???";
  case CALC_TI86:
    return "???";
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return "89q";
  case CALC_TI92:
    return "???";
  case CALC_TI92P:
    return "9Xq";
  case CALC_V200:
    return "V2q";
  default:
    tifiles_error("tifiles_fext_of_flash_os: invalid calc_type argument.");
    break;
  }

  return NULL;
}

/**
 * tifiles_fext_get:
 * @filename: a filename as string.
 *
 * Returns file extension part.
 *
 * Return value: a file extension without dot as string (like "89g").
 **/
TIEXPORT char *TICALL tifiles_fext_get(const char *filename)
{
  char *d = NULL;

  d = strrchr(filename, '.');
  if (d == NULL)
    return "";

  return (++d);
}

/**
 * tifiles_fext_dup:
 * @filename: a filename as string.
 *
 * Returns a copy of file extension part.
 *
 * Return value: a file extension without dot as string (like "89g").
 * Need to be freed when no longer needed.
 **/
TIEXPORT char *TICALL tifiles_fext_dup(const char *filename)
{
    return strdup(tifiles_fext_get(filename));
}

/**************/
/* File types */
/**************/

#ifndef __WIN32__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

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
#define TIG_SIGNATURE	"PK\x04\x03"	// 0x04034b50

/**
 * tifiles_file_is_ti:
 * @filename: a filename as string.
 *
 * Check whether file is a TI file by checking the signature.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_ti(const char *filename)
{
  FILE *f;
  char buf[9];
  char str[64];
  char *p;

  // bug: check that file is not a FIFO
  if (!is_regfile(filename))
    return 0;

  f = fopen(filename, "rb");
  if (f == NULL)
	  return 0;
  // read header
  fread_8_chars(f, buf);

  for(p = buf; *p != '\0'; p++)
      *p = toupper(*p);

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
  rewind(f);
  fread_n_chars(f, 0x16, NULL);
  fread_n_chars(f, strlen(TIB_SIGNATURE), str);
  str[strlen(TIB_SIGNATURE)] = '\0';
  if(!strcmp(str, TIB_SIGNATURE)) 
  {
	fclose(f);
	return !0;
  }

	// check for TIG file
	rewind(f);
	fread_n_chars(f, strlen(TIG_SIGNATURE), str);
	str[strlen(TIG_SIGNATURE)] = '\0';
	if(!strcmp(str, TIG_SIGNATURE)) 
	{
		fclose(f);
		return !0;
	}

  fclose(f);
  return 0;
}

/**
 * tifiles_file_is_single:
 * @filename: a filename as string.
 *
 * Check whether file is a single TI file (like program, function, ...).
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_single(const char *filename)
{
  if (!tifiles_file_is_ti(filename))
    return 0;

  if (tifiles_file_is_group(filename) ||
      tifiles_file_is_backup(filename) ||
      tifiles_file_is_flash(filename))
    return 0;
  else
    return !0;
}

/**
 * tifiles_file_is_group:
 * @filename: a filename as string.
 *
 * Check whether file is a group file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_group(const char *filename)
{
  int i;
  char *e = tifiles_fext_get(filename);

  if (!strcmp(e, ""))
    return 0;

  if (!tifiles_file_is_ti(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if (!g_ascii_strcasecmp(e, GROUP_FILE_EXT[i]))
      return !0;
  }

  return 0;
}

/**
 * tifiles_file_is_regular:
 * @filename: a filename as string.
 *
 * Check whether file is a single or group file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_regular(const char *filename)
{
  if (!tifiles_file_is_ti(filename))
    return 0;

  return (tifiles_file_is_single(filename) ||
	  tifiles_file_is_group(filename));
}

/**
 * tifiles_file_is_backup:
 * @filename: a filename as string.
 *
 * Check whether file is a backup file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_backup(const char *filename)
{
  int i;
  char *e = tifiles_fext_get(filename);

  if (!strcmp(e, ""))
    return 0;

  if (!tifiles_file_is_ti(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if (!g_ascii_strcasecmp(e, BACKUP_FILE_EXT[i]))
      return !0;
  }

  return 0;
}

/**
 * tifiles_file_is_flash:
 * @filename: a filename as string.
 *
 * Check whether file is a FLASH file (os or app).
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_flash(const char *filename)
{
  int i;
  char *e = tifiles_fext_get(filename);

  if (!strcmp(e, ""))
    return 0;

  if (!tifiles_file_is_ti(filename))
    return 0;

  for (i = 1; i < NCALCS + 1; i++) 
  {
    if ((!g_ascii_strcasecmp(e, FLASH_APP_FILE_EXT[i])) ||
	(!g_ascii_strcasecmp(e, FLASH_OS_FILE_EXT[i])) ||
	(!g_ascii_strcasecmp(e, CERTIF_FILE_EXT[i])))
      return !0;
  }

  return 0;
}

/**
 * tifiles_file_is_tib:
 * @filename: a filename as string.
 *
 * Check whether file is a TIB formatted file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_tib(const char *filename)
{
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
	  return 0;

	if (!tifiles_file_is_ti(filename))
		return 0;

	if(!g_ascii_strcasecmp(e, "tib"))
		return !0;

	// no need to do more test, TIB signature has already been checked 
	// by tifiles_file_is_ti()

	return 0;
}

/**
 * tifiles_file_is_tig:
 * @filename: a filename as string.
 *
 * Check whether file is a TiGroup formatted file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_file_is_tig(const char *filename)
{
	FILE *f;
	char str[4];
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
	  return 0;

	if(g_ascii_strcasecmp(e, "tig"))
		return 0;

	f = fopen(filename, "rb");
	if(f == NULL)
		return 0;

	fread_n_chars(f, strlen(TIG_SIGNATURE), str);
	str[strlen(TIG_SIGNATURE)] = '\0';
	if(!strcmp(str, TIG_SIGNATURE)) 
	{
		fclose(f);
		return !0;
	}

	fclose(f);
	return 0;
}

/********/
/* Misc */
/********/

/* Note: a better way should be to open the file and read the signature */
/**
 * tifiles_file_get_model:
 * @filename: a filename as string.
 *
 * Returns the calculator model targetted for this file.
 *
 * Return value: a model taken in #CalcModel.
 **/
TIEXPORT CalcModel TICALL tifiles_file_get_model(const char *filename)
{
  char *ext = tifiles_fext_get(filename);
  int type = CALC_NONE;
  char str[3];

  if (!strcmp(ext, ""))
    return CALC_NONE;

  strncpy(str, ext, 2);
  str[2] = '\0';

  if (!g_ascii_strcasecmp(str, "73"))
    type = CALC_TI73;
  else if (!g_ascii_strcasecmp(str, "82"))
    type = CALC_TI82;
  else if (!g_ascii_strcasecmp(str, "83"))
    type = CALC_TI83;
  else if (!g_ascii_strcasecmp(str, "8x"))
    type = CALC_TI83P;
  else if (!g_ascii_strcasecmp(str, "85"))
    type = CALC_TI85;
  else if (!g_ascii_strcasecmp(str, "86"))
    type = CALC_TI86;
  else if (!g_ascii_strcasecmp(str, "89"))
    type = CALC_TI89;
  else if (!g_ascii_strcasecmp(str, "92"))
    type = CALC_TI92;
  else if (!g_ascii_strcasecmp(str, "9X"))
    type = CALC_TI92P;
  else if (!g_ascii_strcasecmp(str, "V2"))
    type = CALC_V200;
  //else if (!g_ascii_strcasecmp(str, "tib"))
    //type = CALC_TI89;	// consider .tib as TI89
  else
    type = CALC_NONE;

  return type;
}

/**
 * tifiles_file_get_class:
 * @filename: a filename as string.
 *
 * Returns the file class (single, group, backup, flash).
 *
 * Return value: a value in #FileClass.
 **/
TIEXPORT FileClass TICALL tifiles_file_get_class(const char *filename)
{
  if (tifiles_file_is_single(filename))
    return TIFILE_SINGLE;
  else if (tifiles_file_is_group(filename))
    return TIFILE_GROUP;
  else if (tifiles_file_is_backup(filename))
    return TIFILE_BACKUP;
  else if (tifiles_file_is_flash(filename))
    return TIFILE_FLASH;
  else
    return 0;
}

/**
 * tifiles_file_get_type:
 * @filename: a filename as string.
 *
 * Returns the type of file (function, program, ...).
 *
 * Return value: a string like "Assembly Program" (localized).
 **/
TIEXPORT const char *TICALL tifiles_file_get_type(const char *filename)
{
  char *ext;

  ext = tifiles_fext_get(filename);
  if (!strcmp(ext, ""))
    return "";

  if (!g_ascii_strcasecmp(ext, "tib"))
    return _("OS upgrade");

  if (!tifiles_file_is_ti(filename))
    return "";

  if (tifiles_file_is_group(filename)) 
  {
    switch (tifiles_file_get_model(filename)) 
	{
    case CALC_TI89:
	case CALC_TI89T:
		case CALC_TI89T_USB:
    case CALC_TI92P:
    case CALC_V200:
      return _("Group/Backup");
    default:
      return _("Group");
    }
  }

  switch (tifiles_file_get_model(filename)) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return ti73_byte2desc(ti73_fext2byte(ext));
  case CALC_TI82:
    return ti82_byte2desc(ti82_fext2byte(ext));
  case CALC_TI83:
    return ti83_byte2desc(ti83_fext2byte(ext));
  case CALC_TI83P:
  case CALC_TI84P:
	  case CALC_TI84P_USB:
    return ti83p_byte2desc(ti83p_fext2byte(ext));
  case CALC_TI85:
    return ti85_byte2desc(ti85_fext2byte(ext));
  case CALC_TI86:
    return ti86_byte2desc(ti86_fext2byte(ext));
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return ti89_byte2desc(ti89_fext2byte(ext));
  case CALC_TI92:
    return ti92_byte2desc(ti92_fext2byte(ext));
  case CALC_TI92P:
    return ti92p_byte2desc(ti92p_fext2byte(ext));
  case CALC_V200:
    return v200_byte2desc(v200_fext2byte(ext));
#endif
  case CALC_NONE:
  default:
    return "";
    break;
  }

  return "";
}

/**
 * tifiles_file_get_icon:
 * @filename: a filename as string.
 *
 * Returns the type of file (function, program, ...).
 *
 * Return value: a string like "Assembly Program" (non localized).
 **/
TIEXPORT const char *TICALL tifiles_file_get_icon(const char *filename)
{
  char *ext;

  ext = tifiles_fext_get(filename);
  if (!strcmp(ext, ""))
    return "";

  if (!g_ascii_strcasecmp(ext, "tib"))
    return "OS upgrade";

  if (!tifiles_file_is_ti(filename))
    return "";

  if(tifiles_file_is_tig(filename))
	  return "TiGroup";

  if (tifiles_file_is_group(filename)) 
  {
    switch (tifiles_file_get_model(filename)) 
	{
    case CALC_TI89:
	case CALC_TI89T:
		case CALC_TI89T_USB:
    case CALC_TI92P:
    case CALC_V200:
      return "Group/Backup";
    default:
      return "Group";
    }
  }

  switch (tifiles_file_get_model(filename)) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return ti73_byte2icon(ti73_fext2byte(ext));
  case CALC_TI82:
    return ti82_byte2icon(ti82_fext2byte(ext));
  case CALC_TI83:
    return ti83_byte2icon(ti83_fext2byte(ext));
  case CALC_TI83P:
  case CALC_TI84P:
	  case CALC_TI84P_USB:
    return ti83p_byte2icon(ti83p_fext2byte(ext));
  case CALC_TI85:
    return ti85_byte2icon(ti85_fext2byte(ext));
  case CALC_TI86:
    return ti86_byte2icon(ti86_fext2byte(ext));
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
  case CALC_TI89T:
	  case CALC_TI89T_USB:
    return ti89_byte2icon(ti89_fext2byte(ext));
  case CALC_TI92:
    return ti92_byte2icon(ti92_fext2byte(ext));
  case CALC_TI92P:
    return ti92p_byte2icon(ti92p_fext2byte(ext));
  case CALC_V200:
    return v200_byte2icon(v200_fext2byte(ext));
#endif
  case CALC_NONE:
  default:
    return "";
    break;
  }

  return "";
}
