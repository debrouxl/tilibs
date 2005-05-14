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
  This unit contains a TI file independant API
*/

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tifiles.h"
#include "error.h"
#include "files8x.h"
#include "files9x.h"

/**
 * tifiles_content_create_regular:
 *
 * Allocates a #FileContent structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT FileContent* TICALL tifiles_content_create_regular(void)
{
#if !defined(DISABLE_TI8X)
	return (FileContent*)ti8x_content_create_regular();
#elif !defined(DISABLE_TI9X)
    return (FileContent*)ti9x_content_create_regular();
#else
#error "You can't disable TI8x & TI9x support both.
#endif
}

/**
 * tifiles_content_free_regular:
 *
 * Free the whole content of a #FileContent structure.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_content_free_regular(FileContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		ti8x_content_free_regular((Ti8xRegular *)content);
	else 
#elif !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		ti9x_content_free_regular((Ti9xRegular *)content);
	else
#else
#error "You can't disable TI8x & TI9x support both.
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_free_regular when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int tifiles_file_read_regular(const char *filename, FileContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
		return ti8x_file_read_regular(filename, (Ti8xRegular *)content);
	else 
#elif !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
		return ti9x_file_read_regular(filename, (Ti9xRegular *)content);
	else
#else
#error "You can't disable TI8x & TI9x support both.
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_write_regular:
 * @filename: name of single/group file where to write or NULL.
 * @content: the file content to write.
 * @real_filename: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write one (or several) variable(s) into a single (group) file. If filename is set to NULL,
 * the function build a filename from varname and allocates resulting filename in %real_fname.
 * %filename and %real_filename can be NULL but not both !
 *
 * %real_filename must be freed when no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int tifiles_file_write_regular(const char *filename, FileContent *content, char **real_fname)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_file_write_regular(filename, (Ti8xRegular *)content, real_fname);
	else 
#elif !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_file_write_regular(filename, (Ti9xRegular *)content, real_fname);
	else
#else
#error "You can't disable TI8x & TI9x support both.
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_display(const char *filename)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
		return ti8x_file_display(filename);
	else 
#elif !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
		return ti9x_file_display(filename);
	else
#else
#error "You can't disable TI8x & TI9x support both.
#endif
    return ERR_BAD_CALC;

	return 0;
}

/*****************/
/* Miscellaneous */
/*****************/

/**
 * tifiles_create_table_of_entries:
 * @content: a TI file content structure.
 * @nfolders: returns the number of folders in the file.
 *
 * The goal of this function is to parse the file content structure in order to build
 * a table of entries so that it's easy to write it just after the header in a group
 * file. Mainly used as an helper.
 * The returned 'table' is an NULL-terminated array of int* pointers.
 * Each pointers points on an integer array. Each cell are an index on the 'VarEntry*  
 * entries' array.
 * 
 * In fact, this array represents a kind of tree. The array of pointer is the folder list
 * and each pointer is the variable list for each folder.
 * For accessing the entry, we use the index.
 *
 * This function may be difficult to understand but it avoids to use trees (and
 * linked list) which will require an implementation.
 *
 * Return value: a 2-dimensions allocated integer array. Must be freed when no
 * longer used.
 **/
TIEXPORT int** TICALL tifiles_create_table_of_entries(FileContent *content, int *nfolders)
{
  int num_folders = 0;
  int i, j;
  char **ptr, *folder_list[32768] = { 0 };
  int **table;

  // determine how many folders we have
  for (i = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = &(content->entries[i]);

    // scan for an existing folder entry
    for (ptr = folder_list; *ptr != NULL; ptr++) 
	{
      if (!strcmp(*ptr, entry->fld_name)) 
	  {
		//printf("break: <%s>\n", entry->fld_name);
		break;
      }
    }
    if (*ptr == NULL) 
	{		// add new folder entry
      folder_list[num_folders] = (char *) calloc(10, sizeof(char));
      //printf("%i: adding '%s'\n", num_folders, entry->fld_name);
      strcpy(folder_list[num_folders], entry->fld_name);
      folder_list[num_folders + 1] = NULL;
      num_folders++;
      g_assert(num_folders <= content->num_entries);
    }
  }
  if (tifiles_calc_is_ti8x(content->model))
    num_folders++;
  *nfolders = num_folders;

  // allocate the folder list
  table = (int **) calloc((num_folders + 1), sizeof(int *));
  table[num_folders] = NULL;

  // for each folder, determine how many variables we have
  // and allocate array with indexes
  for (j = 0; j < num_folders; j++) 
  {
    int k;

    for (i = 0, k = 0; i < content->num_entries; i++) 
	{
      VarEntry *entry = &(content->entries[i]);

      if (!strcmp(folder_list[j], entry->fld_name)) 
	  {
		table[j] = (int *) realloc(table[j], (k + 2) * sizeof(int));
		table[j][k] = i;
		//printf("%i %i: adding %i\n", j, k, i); 
		table[j][k + 1] = -1;
		k++;
      }
    }
  }

  // free memory
  for (j = 0; j < num_folders + 1; j++)
    free(folder_list[j]);

  return table;
}
