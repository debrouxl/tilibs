/* Hey EMACS -*- linux-c -*- */
/* $Id: fileops.c 913 2005-03-31 09:28:09Z roms $ */

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
#include <string.h>

#include "tifiles.h"
#include "rwfile.h"

/**
 * tifiles_calc_is_ti8x:
 * @model: a calculator model.
 *
 * Check whether %model is a TI73..TI86 calculator.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_calc_is_ti8x(CalcModel model)
{
  return ((model == CALC_TI73) || (model == CALC_TI82) ||
	  (model == CALC_TI82) || (model == CALC_TI83) ||
	  (model == CALC_TI83P) || (model == CALC_TI84P) ||
	  (model == CALC_TI85) || (model == CALC_TI86) ||
	  (model == CALC_TI84P_USB));
}

/**
 * tifiles_calc_is_ti9x:
 * @model: a calculator model.
 *
 * Check whether %model is a TI89...V200PLT calculator.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_calc_is_ti9x(CalcModel model)
{
  return ((model == CALC_TI89) || (model == CALC_TI89T) ||
	  (model == CALC_TI92) || (model == CALC_TI92P) || (model == CALC_V200) ||
	  (model == CALC_TI89T_USB));
}

/**
 * tifiles_calc_are_compat:
 * @model: a calculator model.
 * @ref: a calculator model.
 *
 * Check whether %model is compatible (in term of file types) with %ref.
 * Example: a .92t can be sent to a TI92 (of course) as well as a 
 * TI89, 92+, V200 and a Titanium.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_calc_are_compat(CalcModel model, CalcModel ref)
{
	if(tifiles_calc_is_ti8x(model) && tifiles_calc_is_ti8x(ref))
		return !0;
	else if(tifiles_calc_is_ti9x(model) && tifiles_calc_is_ti9x(ref))
		return !0;

	return 0;
}

/**
 * tifiles_has_folder:
 * @model: a calculator model.
 *
 * Returns TRUE if the calculator supports folders.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_has_folder(CalcModel calc_type)
{
  return ((calc_type == CALC_TI89) || (calc_type == CALC_TI89T) ||
	  (calc_type == CALC_TI92) || (calc_type == CALC_TI92P) || 
	  (calc_type == CALC_V200) || (calc_type == CALC_TI89T_USB));
}

/**
 * tifiles_is_flash:
 * @model: a calculator model.
 *
 * Returns TRUE if the calculator model has FLASH technology.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_is_flash(CalcModel calc_type)
{
  return ((calc_type == CALC_TI73) || (calc_type == CALC_TI83P) ||
	  (calc_type == CALC_TI84P) || (calc_type == CALC_TI84P_USB) || 
	  (calc_type == CALC_TI89T) || (calc_type == CALC_TI89) || 
	  (calc_type == CALC_TI92P) ||
	  (calc_type == CALC_V200) || (calc_type == CALC_TI89T_USB));
}

/**
 * tifiles_has_backup:
 * @model: a calculator model.
 *
 * Returns TRUE if the calculator supports true backup.
 *
 * Return value: a boolean value.
 **/
TIEXPORT int TICALL tifiles_has_backup(CalcModel calc_type)
{
  return ((calc_type == CALC_TI73) || (calc_type == CALC_TI82) ||
	  (calc_type == CALC_TI83) || (calc_type == CALC_TI83P) || 
	  (calc_type == CALC_TI84P) || (calc_type == CALC_TI85) ||
	  (calc_type == CALC_TI86) || (calc_type == CALC_TI92));
}

/**
 * tifiles_checksum:
 * @buffer: an array of bytes.
 * @size: the length of the array.
 *
 * Compute the checksum of the array on 'size' bytes.
 * Returns result as a word.
 *
 * Return value: the ckecksum.
 **/
TIEXPORT uint16_t TICALL tifiles_checksum(uint8_t * buffer, int size)
{
  int i;
  uint16_t c = 0;

  if (buffer == NULL)
    return 0;

  for (i = 0; i < size; i++)
    c += buffer[i];

  return c;
}

TIEXPORT int tifiles_hexdump(uint8_t* ptr, unsigned int length)
{
	return hexdump(ptr, length);
}

/**
 * tifiles_get_varname:
 * @full_name: a calculator path such as 'fldname\varname'.
 *
 * Returns the name of the variable.
 *
 * Return value: varname as string. It should not be modified.
 **/
char *TICALL tifiles_get_varname(const char *full_name)
{
  char *bs = strchr(full_name, '\\');

  if (bs == NULL)
    return (char *) full_name;
  else
    return (++bs);
}

/**
 * tifiles_get_fldname:
 * @full_name: a calculator path such as 'fldname\varname'.
 *
 * Returns the folder within the variable is located..
 *
 * Return value: folder name as string. It should not be modified.
 **/
char *TICALL tifiles_get_fldname(const char *full_name)
{
  static char folder[FLDNAME_MAX];
  char *bs = strchr(full_name, '\\');
  int i;

  if (bs == NULL)
    strcpy(folder, "");
  else 
  {
    i = strlen(full_name) - strlen(bs);
    strncpy(folder, full_name, i);
    folder[i] = '\0';
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
/**
 * tifiles_build_fullname:
 * @model: a calculator model.
 * @full_name: the buffer where to store the result.
 * @fldname: the name of folder or "".
 * @varname: the name of variable
 *
 * Build the complete path from folder name and variable name.
 * Not all of calculators supports folder.
 *
 * Return value: a full path as string like 'fldname\varname'.
 **/
char* TICALL tifiles_build_fullname(CalcModel model, char *full_name,
				  const char *fldname, const char *varname)
{
  if (tifiles_has_folder(model)) 
  {
    if (strcmp(fldname, "")) 
	{
      strcpy(full_name, fldname);
      strcat(full_name, "\\");
	  strcat(full_name, varname);
    }
	else
		strcpy(full_name, varname);
  } 
  else
  {
	  strcpy(full_name, varname);
	  //strncpy(full_name, varname, 8);
	  //full_name[8] = '\0';
  }

  return full_name;
}
