/* Hey EMACS -*- linux-c -*- */
/* $Id: fileops.c 913 2005-03-31 09:28:09Z roms $ */

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

#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <ticonv.h>
#include "tifiles.h"
#include "rwfile.h"
#include "logging.h"

/**
 * tifiles_calc_is_ti8x:
 * @model: a calculator model.
 *
 * Check whether %model is a TI73..TI86 calculator.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_calc_is_ti8x(CalcModel model)
{
	return ticonv_model_is_tiz80(model) || ticonv_model_is_tiez80(model);
}

/**
 * tifiles_calc_is_ti9x:
 * @model: a calculator model.
 *
 * Check whether %model is a TI89...V200PLT calculator.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_calc_is_ti9x(CalcModel model)
{
	return ticonv_model_is_ti68k(model);
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
int TICALL tifiles_calc_are_compat(CalcModel model, CalcModel ref)
{
	if (tifiles_calc_is_ti8x(model) && tifiles_calc_is_ti8x(ref))
	{
		return !0;
	}
	else if (tifiles_calc_is_ti9x(model) && tifiles_calc_is_ti9x(ref))
	{
		return !0;
	}
	else if (ticonv_model_is_tinspire(model) && ticonv_model_is_tinspire(ref))
	{
		return !0;
	}

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
int TICALL tifiles_has_folder(CalcModel calc_type)
{
	return ticonv_model_is_ti68k(calc_type) || ticonv_model_is_tinspire(calc_type);
}

/**
 * tifiles_is_flash:
 * @model: a calculator model.
 *
 * Returns TRUE if the calculator model has FLASH technology.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_is_flash(CalcModel calc_type)
{
	return ticonv_model_has_flash_memory(calc_type);
}

/**
 * tifiles_has_backup:
 * @model: a calculator model.
 *
 * Returns TRUE if the calculator supports true backup.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_has_backup(CalcModel calc_type)
{
	return ((calc_type == CALC_TI73) || (calc_type == CALC_TI82) ||
	        (calc_type == CALC_TI83) || (calc_type == CALC_TI83P) ||
	        (calc_type == CALC_TI84P) || (calc_type == CALC_TI84PC) ||
	        (calc_type == CALC_TI84PC_USB) || (calc_type == CALC_TI82A_USB) ||
	        (calc_type == CALC_TI82AEP_USB) ||
	        (calc_type == CALC_TI84PT_USB) || (calc_type == CALC_TI85) ||
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
uint16_t TICALL tifiles_checksum(const uint8_t * buffer, unsigned int size)
{
	uint16_t c = 0;

	if (buffer == nullptr)
	{
		return 0;
	}

	for (unsigned int i = 0; i < size; i++)
	{
		c += buffer[i];
	}

	return c;
}

/**
 * tifiles_hexdump:
 * @ptr: a pointer on some data to dump
 * @len: the number of bytes to dump
 *
 * Dump the content of a buffer into hexadecimal format.
 * Return value: always 0
 **/
int TICALL tifiles_hexdump(const uint8_t * ptr, unsigned int len)
{
	char *str;
	if (ptr != nullptr)
	{
		const unsigned int alloc_len = (len < 1024) ? len : 1024;

		str = (char *)g_malloc(3 * alloc_len + 14);
		for (unsigned int i = 0; i < alloc_len; i++)
		{
			sprintf(&str[3 * i], "%02X ", ptr[i]);
		}
		if (alloc_len < len)
		{
			sprintf(&str[3 * alloc_len], "[...] (%u)", len);
		}
		else
		{
			sprintf(&str[3 * alloc_len], "(%u)", len);
		}

		tifiles_info("%s", str);
		g_free(str);
	}

	return 0;
}

/**
 * tifiles_get_varname:
 * @full_name: a calculator path such as 'fldname\varname'.
 *
 * Returns the name of the variable.
 *
 * Return value: varname as string. It should not be modified (static).
 **/
char *TICALL tifiles_get_varname(const char *full_name)
{
	if (full_name != nullptr)
	{
		char *bs = (char *)strchr(full_name, '\\');

		if (bs == nullptr)
		{
			return (char *) full_name;
		}
		else
		{
			return (++bs);
		}
	}
	tifiles_critical("%s(NULL)", __FUNCTION__);
	return nullptr;
}

/**
 * tifiles_get_fldname:
 *
 * Use tifiles_get_fldname_s instead.
 *
 * Return value: a static string.
 **/
char *TICALL tifiles_get_fldname(const char *full_name)
{
	static char folder[FLDNAME_MAX];
	return tifiles_get_fldname_s(full_name, folder);
}

/**
 * tifiles_get_fldname_s:
 * @full_name: a calculator path such as 'fldname\varname'.
 * @dest_fldname: a destination buffer for storing the variable name, assumed to be at least FLDNAME_MAX characters large.
 *
 * Returns the folder part of the given calculator path.
 *
 * Return value: the given buffer, dest_fldname.
 **/
char *TICALL tifiles_get_fldname_s(const char *full_name, char * dest_fldname)
{
	if (full_name != nullptr && dest_fldname != nullptr)
	{
		const char *bs = (char *)strchr(full_name, '\\');
		if (bs == nullptr)
		{
			dest_fldname[0] = 0;
		}
		else
		{
			const int i = strlen(full_name) - strlen(bs);
			strncpy(dest_fldname, full_name, i);
			dest_fldname[i] = '\0';
		}
		return dest_fldname;
	}
	tifiles_critical("%s(NULL)", __FUNCTION__);
	return nullptr;
}

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
char* TICALL tifiles_build_fullname(CalcModel model, char *full_name, const char *fldname, const char *varname)
{
	if (full_name == nullptr || fldname == nullptr || varname == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return nullptr;
	}

	if (tifiles_has_folder(model)) 
	{
		if (fldname[0] != 0)
		{
			sprintf(full_name, "%s\\%s", fldname, varname);
		}
		else
		{
			strcpy(full_name, varname);
		}
	}
	else
	{
		strcpy(full_name, varname);
	}

	return full_name;
}

/**
 * tifiles_build_filename:
 * @model: a calculator model.
 * @ve: a #VarEntry structure.
 *
 * Build a valid filename from folder name, variable name and variable type.
 * Example: real number x on TI89 in the 'main' folder will give 'main.x.89e'.
 * Note: this function is useable with FLASH apps, too (but you have to fill the #VarEntry structure yourself).
 *
 * Return value: a newly allocated string which must be freed with tifiles_filename_free() when no longer used.
 **/
char* TICALL tifiles_build_filename(CalcModel model, const VarEntry *ve)
{
	char *filename;

	if (ve == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return nullptr;
	}

	if (tifiles_calc_is_ti8x(model) || !strcmp(ve->folder, "") || (ve->type == tifiles_flash_type(model)))
	{
		char* part2 = ticonv_varname_to_filename(model, ve->name, ve->type);
		const char* part3 = tifiles_vartype2fext(model, ve->type);

		filename = g_strconcat(part2, ".", part3, NULL);
		g_free(part2);
	}
	else
	{
		char* part1 = ticonv_varname_to_filename(model, ve->folder, -1);
		char* part2 = ticonv_varname_to_filename(model, ve->name, ve->type);
		const char* part3 = tifiles_vartype2fext(model, ve->type);

		filename = g_strconcat(part1, ".", part2, ".", part3, NULL);
		g_free(part1);
		g_free(part2);
	}

	return filename;
}

/**
 * tifiles_filename_free:
 * @filename: a previously allocated file name.
 *
 * Free a file name previously allocated by tifiles_build_filename().
 **/
void TICALL tifiles_filename_free(char * filename)
{
	g_free(filename);
}
