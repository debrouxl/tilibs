/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include "defsxx.h"
#include "calc_ext.h"

/****************/
/* Global types */
/****************/

static const char GROUP_FILE_EXT[9][4]=
{  
  "XxX", "9Xg", "92g", "89g", "86g", "85g", "8xg", "83g", "82g"
};

static const char BACKUP_FILE_EXT[9][4]=
{ 
  "XxX", "9xg", "92b", "89g", "86b", "85b", "8xb", "83b", "82b" 
};

static const char FLASH_APP_FILE_EXT[9][4]=
{
  "XxX", "9xk", "???", "89k", "???", "???", "8xk", "???", "???" 
};

static const char FLASH_OS_FILE_EXT[9][4]=
{
  "XxX", "9xu", "???", "89u", "???", "???", "8xu", "???", "???" 
};


static const int TIXX_DIR[9]=
{ 
  -1, TI92p_DIR, TI92_DIR, TI89_DIR, TI86_DIR, -1, TI83p_DIR, TI83_DIR, -1
};

static const int TIXX_FLASH[9]=
{
  -1, TI92p_FLASH, -1, TI89_FLASH, -1, -1, -1, -1, -1
};


static const char PAK_NAME[9][10]=
{ 
  "", "TI92+_PAK", "TI92_PAK", "TI89_PAK", "TI86_PAK", "TI85_PAK", 
  "TI83+_PAK", "TI83_PAK", "TI82_PAK" 
};

extern const struct ti_key TI89_KEYS[];
extern const struct ti_key TI92_KEYS[];

const char *group_file_ext(int calc_type)
{
  return GROUP_FILE_EXT[calc_type];
}

const char *backup_file_ext(int calc_type)
{
  return BACKUP_FILE_EXT[calc_type];
}

const char *flash_app_file_ext(int calc_type)
{
	return FLASH_APP_FILE_EXT[calc_type];
}

const char *flash_os_file_ext(int calc_type)
{
	return FLASH_OS_FILE_EXT[calc_type];
}


const int tixx_dir(int calc_type)
{
  return TIXX_DIR[calc_type];
}

const int tixx_flash(int calc_type)
{
  return TIXX_FLASH[calc_type];
}

const char *pak_name(int calc_type)
{
  return PAK_NAME[calc_type];
}

const struct ti_key ti89_keys(unsigned char ascii_code)
{
  return TI89_KEYS[ascii_code];
}

const struct ti_key ti92_keys(unsigned char ascii_code)
{
  return TI92_KEYS[ascii_code];
}
