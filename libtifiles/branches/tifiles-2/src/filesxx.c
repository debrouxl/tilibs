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

#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	FileContent* content = calloc(1, sizeof(FileContent));
	strcpy(content->comment, tifiles_comment_set_single());

	return content;
}

/**
 * tifiles_content_delete_regular:
 *
 * Free the whole content of a #FileContent structure.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_content_delete_regular(FileContent *content)
{
  int i;

  assert(content != NULL);

  for (i = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = content->entries[i];

	assert(entry != NULL);
    free(entry->data);
	free(entry);
  }

  free(content->entries);
#if 0
  free(content);
#endif

  return 0;
}

/**
 * tifiles_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
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
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
		return ti9x_file_read_regular(filename, (Ti9xRegular *)content);
	else
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
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_file_write_regular(filename, (Ti9xRegular *)content, real_fname);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_display_regular:
 * @content: the file content to show.
 *
 * Display file content informations.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_display_regular(FileContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_content_display_regular(content);
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_content_display_regular(content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_content_create_backup:
 *
 * Allocates a #FileContent structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT BackupContent* TICALL tifiles_content_create_backup(void)
{
	BackupContent* content = calloc(1, sizeof(BackupContent));
	strcpy(content->comment, tifiles_comment_set_backup());

	return content;
}

/**
 * tifiles_content_delete_regular:
 *
 * Free the whole content of a #FileContent structure.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_content_delete_backup(BackupContent *content)
{
  assert(content != NULL);

  if (tifiles_calc_is_ti9x(content->model))
		free(content->data_part);
  else if (tifiles_calc_is_ti8x(content->model))
  {
	  free(content->data_part1);
	  free(content->data_part2);
	  free(content->data_part3);
	  free(content->data_part4);
  }

#if 0
  free(content);
#endif

  return 0;
}

/**
 * tifiles_file_read_backup:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int tifiles_file_read_backup(const char *filename, BackupContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
		return ti8x_file_read_backup(filename, content);
	else
#endif 
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
		return ti9x_file_read_backup(filename, content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_write_backup:
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
TIEXPORT int tifiles_file_write_backup(const char *filename, BackupContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_file_write_backup(filename, content);
	else
#endif 
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_file_write_backup(filename, content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_display_regular:
 * @content: the file content to show.
 *
 * Display file content informations.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_display_backup(BackupContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_content_display_backup(content);
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_content_display_backup(content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_content_create_flash:
 *
 * Allocates a #FileContent structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT FlashContent* TICALL tifiles_content_create_flash(void)
{
	FlashContent* content = calloc(1, sizeof(FlashContent));

	if(tifiles_calc_is_ti9x(content->model))
	{
	  time_t tt;
	  struct tm *lt;

	  time(&tt);
	  lt = localtime(&tt);
	  content->revision_major = 1;
	  content->revision_minor = 0;
	  content->flags = 0;
	  content->object_type = 0;
	  content->revision_day = lt->tm_mday;
	  content->revision_month = lt->tm_mon;
	  content->revision_year = lt->tm_year + 1900;
	}

	return content;
}

/**
 * tifiles_content_delete_flash:
 *
 * Free the whole content of a #FileContent structure.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_content_delete_flash(FlashContent *content)
{
	int i;
	assert(content != NULL);

#if !defined(DISABLE_TI8X) && !defined(DISABLE_TI9X)
	{
		FlashContent *ptr;

		free(content->data_part);

		ptr = content->next;
		while (ptr != NULL) 
		{
			FlashContent *next = ptr->next;

			free(ptr->data_part);
			free(ptr);

			for(i = 0; i < content->num_pages; i++)
			{
				free(content->pages[i]->data);
				free(content->pages[i]);
			}
			free(content->pages);

			ptr = next;
		}
#if 0
		free(content);
#endif
	}
#else
    return ERR_BAD_CALC;
#endif

  return 0;
}

/**
 * tifiles_file_read_flash:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int tifiles_file_read_flash(const char *filename, FlashContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
		return ti8x_file_read_flash(filename, content);
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)) || tifiles_file_is_tib(filename))
		return ti9x_file_read_flash(filename, content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_write_flash:
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
TIEXPORT int tifiles_file_write_flash(const char *filename, FlashContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_file_write_flash(filename, content);
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_file_write_flash(filename, content);
	else
#endif
    return ERR_BAD_CALC;

	return 0;
}

/**
 * tifiles_file_display_flash:
 * @content: the file content to show.
 *
 * Display file content informations.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_display_flash(FlashContent *content)
{
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
		return ti8x_content_display_flash(content);
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
		return ti9x_content_display_flash(content);
	else
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
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
	    return ti9x_file_display(filename);
	else
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
    VarEntry *entry = content->entries[i];

    // scan for an existing folder entry
    for (ptr = folder_list; *ptr != NULL; ptr++) 
	{
      if (!strcmp(*ptr, entry->folder)) 
	  {
		//printf("break: <%s>\n", entry->folder);
		break;
      }
    }
    if (*ptr == NULL) 
	{		// add new folder entry
      folder_list[num_folders] = (char *) calloc(10, sizeof(char));
      //printf("%i: adding '%s'\n", num_folders, entry->folder);
      strcpy(folder_list[num_folders], entry->folder);
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
      VarEntry *entry = content->entries[i];

      if (!strcmp(folder_list[j], entry->folder)) 
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

// --- unused code

#if 0
/*************************/
/* Copying (duplicating) */
/*************************/

/*
  Copy an Ti8xRegular structure.
  Memory must be freed when no longer used.
*/
int ti8x_dup_Ti8xRegular(Ti8xRegular *dst, Ti8xRegular *src)
{
  int i;

  memcpy(dst, src, sizeof(Ti8xRegular));

  dst->entries = calloc(src->num_entries + 1, sizeof(VarEntry*));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < src->num_entries; i++)
	  dst->entries[i] = tifiles_ve_dup(src->entries[i]);

  return 0;
}

/*
  Copy an Ti8xBackup structure.
  Memory must be freed when no longer used.
*/
int ti8x_dup_Backup(Ti8xBackup *dst, Ti8xBackup *src)
{
  memcpy(dst, src, sizeof(Ti8xBackup));

  dst->data_part1 = (uint8_t *) calloc(dst->data_length1 , 1);
  dst->data_part2 = (uint8_t *) calloc(dst->data_length2, 1);
  dst->data_part3 = (uint8_t *) calloc(dst->data_length3, 1);
  dst->data_part4 = (uint8_t *) calloc(dst->data_length4, 1);

  if ((dst->data_part1 == NULL) ||
      (dst->data_part2 == NULL) ||
      (dst->data_part3 == NULL) || 
	  (dst->data_part4 == NULL))
    return ERR_MALLOC;

  memcpy(dst->data_part1, src->data_part1, dst->data_length1);
  memcpy(dst->data_part2, src->data_part2, dst->data_length2);
  memcpy(dst->data_part3, src->data_part3, dst->data_length3);
  memcpy(dst->data_part4, src->data_part4, dst->data_length4);

  return 0;
}

/*************************/
/* Copying (duplicating) */
/*************************/

/*
  Copy an Ti9xRegular structure.
  Memory must be freed when no longer used.
*/
int ti9x_dup_Ti9xRegular(Ti9xRegular * dst, Ti9xRegular * src)
{
  int i;

  memcpy(dst, src, sizeof(Ti9xRegular));

  dst->entries = calloc(src->num_entries + 1, sizeof(VarEntry*));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < src->num_entries; i++)
	  dst->entries[i] = tifiles_ve_dup(src->entries[i]);

  return 0;
}

/*
  Copy an Ti9xBackup structure.
  Memory must be freed when no longer used.
*/
int ti9x_dup_Backup(Ti9xBackup * dst, Ti9xBackup * src)
{
  memcpy(dst, src, sizeof(Ti9xBackup));

  dst->data_part = (uint8_t *) calloc(dst->data_length, 1);
  if (dst->data_part == NULL)
    return ERR_MALLOC;

  memcpy(dst->data_part, src->data_part, dst->data_length);

  return 0;
}

/*
  Copy an Ti9xFlash structure.
  Memory must be freed when no longer used.
*/
int ti9x_dup_Flash(Ti9xFlash * dst, Ti9xFlash * src)
{
  // to do...

  return 0;
}

#endif
