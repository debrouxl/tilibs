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
#include "rwfile.h"

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

TIEXPORT const uint8_t TICALL tifiles_folder_type(void)
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

TIEXPORT const uint8_t TICALL tifiles_flash_type(void)
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

TIEXPORT const uint8_t TICALL tifiles_idlist_type(void)
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
