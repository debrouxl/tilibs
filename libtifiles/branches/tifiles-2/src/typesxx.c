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

#include <stdio.h>
#include "gettext.h"
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "typesxx.h"
#include "misc.h"

extern int tifiles_calc_type;

/********************************/
/* Calculator independant types */
/********************************/

TIEXPORT const char *TICALL tifiles_vartype2string(uint8_t data)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_byte2type(data);
    break;
  case CALC_TI82:
    return ti82_byte2type(data);
    break;
  case CALC_TI83:
    return ti83_byte2type(data);
    break;
  case CALC_TI83P:
	  return ti83p_byte2type(data);
	  break;
  case CALC_TI84P:
	return ti84p_byte2type(data);
    break;
  case CALC_TI85:
    return ti85_byte2type(data);
    break;
  case CALC_TI86:
    return ti86_byte2type(data);
    break;
  case CALC_TI89:
    return ti89_byte2type(data);
    break;
	case CALC_TI89T:
	return ti89t_byte2type(data);
    break;
  case CALC_TI92:
    return ti92_byte2type(data);
    break;
  case CALC_TI92P:
    return ti92p_byte2type(data);
    break;
  case CALC_V200:
    return v200_byte2type(data);
    break;
  default:
	  tifiles_error("tifiles_vartype2string: invalid calc_type argument.");
	  return "";
    break;
  }
}

TIEXPORT uint8_t TICALL tifiles_string2vartype(const char *s)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_type2byte(s);
    break;
  case CALC_TI82:
    return ti82_type2byte(s);
    break;
  case CALC_TI83:
    return ti83_type2byte(s);
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_type2byte(s);
    break;
  case CALC_TI85:
    return ti85_type2byte(s);
    break;
  case CALC_TI86:
    return ti86_type2byte(s);
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_type2byte(s);
    break;
  case CALC_TI92:
    return ti92_type2byte(s);
    break;
  case CALC_TI92P:
    return ti92p_type2byte(s);
    break;
  case CALC_V200:
    return v200_type2byte(s);
    break;
  default:
    tifiles_error("tifiles_string2vartype: invalid calc_type argument.");
    return 0;
    break;
  }
}

TIEXPORT const char *TICALL tifiles_vartype2file(uint8_t data)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_byte2fext(data);
    break;
  case CALC_TI82:
    return ti82_byte2fext(data);
    break;
  case CALC_TI83:
    return ti83_byte2fext(data);
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_byte2fext(data);
    break;
  case CALC_TI85:
    return ti85_byte2fext(data);
    break;
  case CALC_TI86:
    return ti86_byte2fext(data);
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_byte2fext(data);
    break;
  case CALC_TI92:
    return ti92_byte2fext(data);
    break;
  case CALC_TI92P:
    return ti92p_byte2fext(data);
    break;
  case CALC_V200:
    return v200_byte2fext(data);
    break;
  default:
    tifiles_error("tifiles_vartype2file: invalid calc_type argument.");
    return "";
    break;
  }
}

TIEXPORT uint8_t TICALL tifiles_file2vartype(const char *s)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_fext2byte(s);
    break;
  case CALC_TI82:
    return ti82_fext2byte(s);
    break;
  case CALC_TI83:
    return ti83_fext2byte(s);
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_fext2byte(s);
    break;
  case CALC_TI85:
    return ti85_fext2byte(s);
    break;
  case CALC_TI86:
    return ti86_fext2byte(s);
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_fext2byte(s);
    break;
  case CALC_TI92:
    return ti92_fext2byte(s);
    break;
  case CALC_TI92P:
    return ti92p_fext2byte(s);
    break;
  case CALC_V200:
    return v200_fext2byte(s);
    break;
  default:
    tifiles_error("tifiles_file2vartype: invalid calc_type argument.");
    return 0;
    break;
  }
}

TIEXPORT const char *TICALL tifiles_vartype2desc(uint8_t d)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_byte2desc(d);
    break;
  case CALC_TI82:
    return ti82_byte2desc(d);
    break;
  case CALC_TI83:
    return ti83_byte2desc(d);
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_byte2desc(d);
    break;
  case CALC_TI85:
    return ti85_byte2desc(d);
    break;
  case CALC_TI86:
    return ti86_byte2desc(d);
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_byte2desc(d);
    break;
  case CALC_TI92:
    return ti92_byte2desc(d);
    break;
  case CALC_TI92P:
    return ti92p_byte2desc(d);
    break;
  case CALC_V200:
    return v200_byte2desc(d);
    break;
  default:
    tifiles_error("tifiles_vartype2desc: invalid calc_type argument.");
    return "";
    break;
  }
}

TIEXPORT const char *TICALL tifiles_vartype2icon(uint8_t d)
{
  switch (tifiles_calc_type) 
  {
  case CALC_TI73:
    return ti73_byte2icon(d);
    break;
  case CALC_TI82:
    return ti82_byte2icon(d);
    break;
  case CALC_TI83:
    return ti83_byte2icon(d);
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_byte2icon(d);
    break;
  case CALC_TI85:
    return ti85_byte2icon(d);
    break;
  case CALC_TI86:
    return ti86_byte2icon(d);
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_byte2icon(d);
    break;
  case CALC_TI92:
    return ti92_byte2icon(d);
    break;
  case CALC_TI92P:
    return ti92p_byte2icon(d);
    break;
  case CALC_V200:
    return v200_byte2icon(d);
    break;
  default:
    tifiles_error("tifiles_vartype2icon: invalid calc_type argument.");
    return "";
    break;
  }
}

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

static const int TIXX_DIR[NCALCS + 1] = 
{
	-1, 
	TI73_DIR, -1, TI83_DIR, TI83p_DIR, TI84p_DIR, -1, TI86_DIR,
	TI89_DIR, TI89_DIR, TI92_DIR, V200_DIR,
};

static const int TIXX_FLASH[NCALCS + 1] = 
{
	-1, 
	TI73_APPL, -1, -1, TI83p_APPL, TI84p_APPL, -1, -1,
	TI89_APPL, TI89t_APPL, -1, TI92p_APPL, V200_APPL,
};

static const int TIXX_IDLIST[NCALCS + 1] = 
{
	-1, 
	TI73_IDLIST, -1, -1, TI83p_IDLIST, TI84p_IDLIST, -1, -1,
	TI89_IDLIST, TI89t_IDLIST, -1, TI92p_IDLIST, V200_IDLIST,
};

/**********************************/
/* Global types: access functions */
/**********************************/

TIEXPORT const char *TICALL tifiles_group_file_ext(void)
{
  switch (tifiles_calc_type) 
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

TIEXPORT const char *TICALL tifiles_backup_file_ext(void)
{
  switch (tifiles_calc_type) 
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

TIEXPORT const char *TICALL tifiles_flash_app_file_ext(void)
{
  switch (tifiles_calc_type) 
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

TIEXPORT const char *TICALL tifiles_flash_os_file_ext()
{
  switch (tifiles_calc_type) 
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

TIEXPORT const int TICALL tifiles_folder_type(void)
{
  switch (tifiles_calc_type) 
  {
  case CALC_NONE:
    return -1;
  case CALC_TI73:
    return TI73_DIR;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
    return TI83_DIR;
  case CALC_TI83P:
  case CALC_TI84P:
    return TI83p_DIR;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return TI86_DIR;
  case CALC_TI89:
  case CALC_TI89T:
    return TI89_DIR;
  case CALC_TI92:
    return TI92_DIR;
  case CALC_TI92P:
    return TI92p_DIR;
  case CALC_V200:
    return V200_DIR;
  default:
    tifiles_error("tifiles_folder_type: invalid calc_type argument.");
    break;
  }

  return -1;
}

TIEXPORT const int TICALL tifiles_flash_type(void)
{
  switch (tifiles_calc_type) 
  {
  case CALC_NONE:
    return -1;
  case CALC_TI73:
    return TI73_APPL;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
    return -1;
  case CALC_TI83P:
  case CALC_TI84P:
    return TI83p_APPL;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return -1;
  case CALC_TI89:
  case CALC_TI89T:
    return TI89_APPL;
  case CALC_TI92:
    return -1;
  case CALC_TI92P:
    return TI92p_APPL;
  case CALC_V200:
    return V200_APPL;
  default:
    tifiles_error("tifiles_flash_type: invalid calc_type argument.");
    break;
  }

  return -1;
}

TIEXPORT const int TICALL tifiles_idlist_type(void)
{
  switch (tifiles_calc_type) 
  {
  case CALC_NONE:
    return -1;
  case CALC_TI73:
    return TI73_IDLIST;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
    return -1;
  case CALC_TI83P:
  case CALC_TI84P:
    return TI83p_IDLIST;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return -1;
  case CALC_TI89:
  case CALC_TI89T:
    return TI89_IDLIST;
  case CALC_TI92:
    return -1;
  case CALC_TI92P:
    return TI92p_IDLIST;
  case CALC_V200:
    return V200_IDLIST;
  default:
    tifiles_error("tifiles_idlist_type: invalid calc_type argument.");
    break;
  }

  return -1;
}

/****************/
/* Miscelaneous */
/****************/

/* 
   Return a string describing the calculator type or NULL
   - const char* [out]: the string
*/
TIEXPORT const char *TICALL tifiles_calc_type_to_string(void)
{
  switch (tifiles_calc_type) 
  {
  case CALC_NONE:
    return "none";
  case CALC_V200:
    return "v200";
  case CALC_TI92P:
    return "92+";
  case CALC_TI92:
    return "92";
  case CALC_TI89T:
	  return "89t";
  case CALC_TI89:
    return "89";
  case CALC_TI86:
    return "86";
  case CALC_TI85:
    return "85";
  case CALC_TI84P:
    return "84+";
  case CALC_TI83P:
    return "83+";
  case CALC_TI83:
    return "83";
  case CALC_TI82:
    return "82";
  case CALC_TI73:
    return "73";
  default:
	  tifiles_error("tifiles_calc_type_to_string: invalid calc_type argument.");
    return NULL;
  }

  return NULL;
}


/*
   Return a string describing the file type (.82s for instance) or NULL
   - type [in]: a variable type (independant format)
   - const char* [out]: the string
*/
TIEXPORT const char *TICALL tifiles_vartype_to_file_extension(int type)
{
	return tifiles_vartype2file((uint8_t)type);
}

TIEXPORT const char *TICALL tifiles_calctype2signature(TiCalcType calc_type)
{
  switch (calc_type) 
  {
  case CALC_NONE:
    return "**TI??**";
  case CALC_TI73:
    return "**TI73**";
  case CALC_TI82:
    return "**TI82**";
  case CALC_TI83:
    return "**TI83**";
  case CALC_TI83P:
	  return "**TI83F*";
  case CALC_TI84P:
    return "**TI83F*";
  case CALC_TI85:
    return "**TI85**";
  case CALC_TI86:
    return "**TI86**";
  case CALC_TI89:
	  return "**TI89**";
  case CALC_TI89T:
    return "**TI89**";
  case CALC_TI92:
    return "**TI92**";
  case CALC_TI92P:
    return "**TI92P*";
  case CALC_V200:
    return "**TI92P*";
  default:
    tifiles_error("tifiles_calctype2signature: invalid calc_type argument.");
    break;
  }

  return NULL;
}

TIEXPORT TiCalcType TICALL tifiles_signature2calctype(const char *s)
{

  if (!g_ascii_strcasecmp(s, "**TI73**"))
    return CALC_TI73;
  else if (!g_ascii_strcasecmp(s, "**TI82**"))
    return CALC_TI82;
  else if (!g_ascii_strcasecmp(s, "**TI83**"))
    return CALC_TI83;
  else if (!g_ascii_strcasecmp(s, "**TI83F*"))
    return CALC_TI83P;
  else if (!g_ascii_strcasecmp(s, "**TI85**"))
    return CALC_TI85;
  else if (!g_ascii_strcasecmp(s, "**TI86**"))
    return CALC_TI86;
  else if (!g_ascii_strcasecmp(s, "**TI89**"))
    return CALC_TI89;
  else if (!g_ascii_strcasecmp(s, "**TI92**"))
    return CALC_TI92;
  else if (!g_ascii_strcasecmp(s, "**TI92P*"))
    return CALC_TI92P;
  else if (!g_ascii_strcasecmp(s, "**V200**"))
    return CALC_V200;
  else
    return CALC_NONE;
}


/*******************/
/* Query functions */
/*******************/

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

/* Note: a better way should be to open the file and read the signature */
/* 
   Return the calc type corresponding to the file
   - filename [in]: a filename
   - int [out]: the calculator type
*/
TIEXPORT int TICALL tifiles_which_calc_type(const char *filename)
{
  char *ext;
  int type = CALC_NONE;

  ext = tifiles_dup_extension(filename);
  if (ext == NULL)
    return CALC_NONE;

  ext[2] = '\0';

  if (!g_ascii_strcasecmp(ext, "73"))
    type = CALC_TI73;
  else if (!g_ascii_strcasecmp(ext, "82"))
    type = CALC_TI82;
  else if (!g_ascii_strcasecmp(ext, "83"))
    type = CALC_TI83;
  else if (!g_ascii_strcasecmp(ext, "8x"))
    type = CALC_TI83P;
  else if (!g_ascii_strcasecmp(ext, "85"))
    type = CALC_TI85;
  else if (!g_ascii_strcasecmp(ext, "86"))
    type = CALC_TI86;
  else if (!g_ascii_strcasecmp(ext, "89"))
    type = CALC_TI89;
  else if (!g_ascii_strcasecmp(ext, "92"))
    type = CALC_TI92;
  else if (!g_ascii_strcasecmp(ext, "9x"))
    type = CALC_TI92P;
  else if (!g_ascii_strcasecmp(ext, "v2"))
    type = CALC_V200;
  else
    type = CALC_NONE;

  g_free(ext);

  return type;
}

/*
   Return the file type corresponding to the file
   - filename [in]: a filename
   - int [out]: the file type
*/
TIEXPORT int TICALL tifiles_which_file_type(const char *filename)
{
  if (tifiles_is_a_single_file(filename))
    return TIFILE_SINGLE;
  else if (tifiles_is_a_group_file(filename))
    return TIFILE_GROUP;
  else if (tifiles_is_a_backup_file(filename))
    return TIFILE_BACKUP;
  else if (tifiles_is_a_flash_file(filename))
    return TIFILE_FLASH;
  else
    return 0;
}

/*
   Return the descriptive of the file such as 'Vector' or 'String'.
   This function is localized (i18n).
   - filename [in]: a filename
   - char* [out]: the descriptive
*/
TIEXPORT const char *TICALL tifiles_file_descriptive(const char *filename)
{
  char *ext;

  ext = tifiles_get_extension(filename);
  if (ext == NULL)
    return "";

  if (!g_ascii_strcasecmp(ext, "tib"))
    return _("OS upgrade");

  if (!tifiles_is_a_ti_file(filename))
    return "";

  if (tifiles_is_a_group_file(filename)) 
  {
    switch (tifiles_which_calc_type(filename)) 
	{
    case CALC_TI89:
	case CALC_TI89T:
    case CALC_TI92P:
    case CALC_V200:
      return _("Group/Backup");
    default:
      return _("Group");
    }
  }

  switch (tifiles_which_calc_type(filename)) 
  {
  case CALC_TI73:
    return ti73_byte2desc(ti73_fext2byte(ext));
  case CALC_TI82:
    return ti82_byte2desc(ti82_fext2byte(ext));
  case CALC_TI83:
    return ti83_byte2desc(ti83_fext2byte(ext));
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_byte2desc(ti83p_fext2byte(ext));
  case CALC_TI85:
    return ti85_byte2desc(ti85_fext2byte(ext));
  case CALC_TI86:
    return ti86_byte2desc(ti86_fext2byte(ext));
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_byte2desc(ti89_fext2byte(ext));
  case CALC_TI92:
    return ti92_byte2desc(ti92_fext2byte(ext));
  case CALC_TI92P:
    return ti92p_byte2desc(ti92p_fext2byte(ext));
  case CALC_V200:
    return v200_byte2desc(v200_fext2byte(ext));
  case CALC_NONE:
  default:
    return "";
    break;
  }

  return "";
}

/*
   Return an icon name associated with the file type.
   This function is the same than 'tifiles_file_descriptive' but it is
   not localized (i18n).
   - filename [in]: a filename
   - char* [out]: the icon name, such as 'Vector'.
*/
TIEXPORT const char *TICALL tifiles_file_icon(const char *filename)
{
  char *ext;

  ext = tifiles_get_extension(filename);
  if (ext == NULL)
    return "";

  if (!g_ascii_strcasecmp(ext, "tib"))
    return "OS upgrade";

  if (!tifiles_is_a_ti_file(filename))
    return "";

  if (tifiles_is_a_group_file(filename)) 
  {
    switch (tifiles_which_calc_type(filename)) 
	{
    case CALC_TI89:
	case CALC_TI89T:
    case CALC_TI92P:
    case CALC_V200:
      return "Group/Backup";
    default:
      return "Group";
    }
  }

  switch (tifiles_which_calc_type(filename)) 
  {
  case CALC_TI73:
    return ti73_byte2icon(ti73_fext2byte(ext));
  case CALC_TI82:
    return ti82_byte2icon(ti82_fext2byte(ext));
  case CALC_TI83:
    return ti83_byte2icon(ti83_fext2byte(ext));
  case CALC_TI83P:
  case CALC_TI84P:
    return ti83p_byte2icon(ti83p_fext2byte(ext));
  case CALC_TI85:
    return ti85_byte2icon(ti85_fext2byte(ext));
  case CALC_TI86:
    return ti86_byte2icon(ti86_fext2byte(ext));
  case CALC_TI89:
  case CALC_TI89T:
    return ti89_byte2icon(ti89_fext2byte(ext));
  case CALC_TI92:
    return ti92_byte2icon(ti92_fext2byte(ext));
  case CALC_TI92P:
    return ti92p_byte2icon(ti92p_fext2byte(ext));
  case CALC_V200:
    return v200_byte2icon(v200_fext2byte(ext));
  case CALC_NONE:
  default:
    return "";
    break;
  }

  return "";
}

/*
  Return TRUE for calcs in TI73..TI86
*/
TIEXPORT int TICALL tifiles_is_ti8x(TiCalcType calc_type)
{
  return ((calc_type == CALC_TI73) || (calc_type == CALC_TI82) ||
	  (calc_type == CALC_TI82) || (calc_type == CALC_TI83) ||
	  (calc_type == CALC_TI83P) || (calc_type == CALC_TI84P) ||
	  (calc_type == CALC_TI85) || (calc_type == CALC_TI86));
}

/*
  Return TRUE for calcs in TI89..TI92+,V200
*/
TIEXPORT int TICALL tifiles_is_ti9x(TiCalcType calc_type)
{
  return ((calc_type == CALC_TI89) || (calc_type == CALC_TI89T) ||
	  (calc_type == CALC_TI92) ||
	  (calc_type == CALC_TI92P) || (calc_type == CALC_V200));
}

/*
  Return silent calcs
*/
TIEXPORT int TICALL tifiles_is_silent(TiCalcType calc_type)
{
  return ((calc_type == CALC_TI82) || (calc_type == CALC_TI83) ||
	  (calc_type == CALC_TI85));
}

/*
  Returns calcs which have folders
*/
TIEXPORT int TICALL tifiles_has_folder(TiCalcType calc_type)
{
  return ((calc_type == CALC_TI89) || (calc_type == CALC_TI89T) ||
	  (calc_type == CALC_TI92) || (calc_type == CALC_TI92P) || 
	  (calc_type == CALC_V200));
}

/*
  Return FLASH calcs
*/
TIEXPORT int TICALL tifiles_is_flash(TiCalcType calc_type)
{
  return ((calc_type == CALC_TI73) || (calc_type == CALC_TI83P) ||
	  (calc_type == CALC_TI84P) || (calc_type == CALC_TI89T) ||
	  (calc_type == CALC_TI89) || (calc_type == CALC_TI92P) ||
	  (calc_type == CALC_V200));
}
