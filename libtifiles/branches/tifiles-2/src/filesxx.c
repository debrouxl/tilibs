/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "file_int.h"
#include "file_err.h"

// allocating
TIEXPORT TiRegular *TICALL tifiles_create_regular_content(void)
{
  return ti9x_create_regular_content();
}

// freeing
TIEXPORT int TICALL tifiles_free_regular_content(TiRegular * content)
{
  if (tifiles_is_ti8x(content->calc_type))
    return ti8x_free_regular_content(content);
  else if (tifiles_is_ti9x(content->calc_type))
    return ti9x_free_regular_content(content);
  else
    return ERR_BAD_CALC;
}

// reading
TIEXPORT int tifiles_read_regular_file(const char *filename,
                                       TiRegular *content)
{
  if (tifiles_is_ti8x(tifiles_which_calc_type(filename)))
    return ti8x_read_regular_file(filename, content);
  else if (tifiles_is_ti9x(tifiles_which_calc_type(filename)))
    return ti9x_read_regular_file(filename, content);
  else
    return ERR_BAD_CALC;

  return 0;
}

// writing
TIEXPORT int tifiles_write_regular_file(const char *filename,
                                        TiRegular * content,
                                        char **real_fname)
{
  if (tifiles_is_ti8x(content->calc_type))
    return ti8x_write_regular_file(filename, content, real_fname);
  else if (tifiles_is_ti9x(content->calc_type))
    return ti9x_write_regular_file(filename, content, real_fname);
  else
    return ERR_BAD_CALC;

  return 0;
}

// displaying
TIEXPORT int TICALL tifiles_display_file(const char *filename)
{
  if (tifiles_is_ti8x(tifiles_which_calc_type(filename)))
    return ti8x_display_file(filename);
  else if (tifiles_is_ti9x(tifiles_which_calc_type(filename)))
    return ti9x_display_file(filename);
  else
    return ERR_BAD_CALC;

  return 0;
}

/*****************/
/* Miscellaneous */
/*****************/

/*
  This function needs some explanations...
  Its goal is to parse the file content in order to build a table of
  entries so that it's easy to write it after the header in a group file.
  Our 'table' is an array of pointers terminated by NULL.
  Each pointer points on an array of integer. Theses integers are an index
  in the 'Ti9xVarEntry*  entries' array.
  This array represents a kind of tree. The array of pointer is the folder list
  and each pointer is the var list for each folder.
  For accessing the entry, we use the index.

  This function may be difficult to understand but it avoids to use trees (and
  linked list) which will require an implementation.
 */
TIEXPORT int TICALL tifiles_create_table_of_entries(TiRegular * content,
						    int ***tabl,
						    int *nfolders)
{
  int num_folders = 0;
  int i, j;
  char **ptr, *folder_list[32768] = { 0 };
  int **table;

  folder_list[0] = (char *) calloc(9, sizeof(char));
  strcpy(folder_list[0], "");
  folder_list[1] = NULL;

  // determine how many folders we have
  for (i = 0; i < content->num_entries; i++) {
    TiVarEntry *entry = &(content->entries[i]);

    // scan for an existing folder entry
    for (ptr = folder_list; *ptr != NULL; ptr++) {
      if (!strcmp(*ptr, entry->folder)) {
	//printf("break: <%s>\n", entry->folder);
	break;
      }
    }
    if (*ptr == NULL) {		// add new folder entry
      folder_list[num_folders] = (char *) calloc(9, sizeof(char));
      //printf("%i: adding '%s'\n", num_folders, entry->folder);
      strcpy(folder_list[num_folders], entry->folder);
      folder_list[num_folders + 1] = NULL;
      num_folders++;
      assert(num_folders <= content->num_entries);
    }
  }
  if (tifiles_is_ti8x(content->calc_type))
    num_folders++;
  *nfolders = num_folders;

  // allocate the folder list
  table = *tabl = (int **) calloc((num_folders + 1), sizeof(int *));
  table[num_folders] = NULL;

  // for each folder, determine how many variables we have
  // and allocate array with indexes
  for (j = 0; j < num_folders; j++) {
    int k;

    for (i = 0, k = 0; i < content->num_entries; i++) {
      Ti9xVarEntry *entry = &(content->entries[i]);

      if (!strcmp(folder_list[j], entry->folder)) {
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

  return 0;
}
