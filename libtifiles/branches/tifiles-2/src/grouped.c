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
  Grouping/Ungrouping routines
  Calcs: 73/82/83/83+/84+/85/86 & 89/89tm/92/92+/V200
*/

#include "tifiles.h"
#include "error.h"
#include "macros.h"
#include "files8x.h"


/************************/
/* (Un)grouping content */
/************************/

int ti8x_dup_VarEntry(Ti8xVarEntry *dst, Ti8xVarEntry *src);

/**
 * tifiles_group_contents:
 * @src_contents: a pointer on an array of #TiRegular structures. The array must be terminated by NULL.
 * @dst_content: the address of a pointer. This pointer will contain the allocated group file.
 * Must be freed when no longer needed.
 *
 * Group several #TiRegular structures into a single one.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_group_contents(TiRegular **src_contents, TiRegular **dst_content)
{
  TiRegular *dst;
  int i, n;

  for (n = 0; src_contents[n] != NULL; n++);

  dst = *dst_content = (TiRegular *) calloc(1, sizeof(TiRegular));
  if (dst == NULL)
    return ERR_MALLOC;
  memcpy(dst, src_contents[0], sizeof(TiRegular));

  dst->num_entries = n;
  dst->entries = (TiVarEntry *) calloc(n, sizeof(TiVarEntry));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < n; i++) 
  {
    TiRegular *src = src_contents[i];

    TRY(ti8x_dup_VarEntry(&(dst->entries[i]), &(src->entries[0])));
  }

  return 0;
}

/**
 * tifiles_ungroup_content:
 * @src_content: a pointer on the structure to unpack.
 * @dst_contents: the address of your pointer. This pointers will point on a 
 * dynamically allocated array of structures. The array is terminated by NULL.
 * Must be freed when no longer needed.
 *
 * Ungroup a TI file by exploding the structure into an array of structures.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_ungroup_content(TiRegular *src, TiRegular ***dest)
{
  int i;
  TiRegular **dst;

  // allocate an array of Regular structures (NULL terminated)
  dst = *dest = (TiRegular **) calloc(src->num_entries + 1,
				      sizeof(TiRegular *));
  if (dst == NULL)
    return ERR_MALLOC;

  // parse each entry and duplicate it into a single content  
  for (i = 0; i < src->num_entries; i++) {
    TiVarEntry *src_entry = &(src->entries[i]);
    TiVarEntry *dst_entry = NULL;

    // allocate and duplicate content
    dst[i] = (TiRegular *) calloc(1, sizeof(TiRegular));
    if (dst[i] == NULL)
      return ERR_MALLOC;
    memcpy(dst[i], src, sizeof(TiRegular));

    // allocate and duplicate entry
    dst[i]->entries = (TiVarEntry *) calloc(1, sizeof(TiVarEntry));
    dst_entry = &(dst[i]->entries[0]);
    TRY(ti8x_dup_VarEntry(dst_entry, src_entry));

    // update some fields
    dst[i]->num_entries = 1;
    dst[i]->checksum +=
	tifiles_checksum((uint8_t *) dst_entry, 15);
    dst[i]->checksum +=
	tifiles_checksum(dst_entry->data, dst_entry->size);
  }
  dst[i] = NULL;

  return 0;
}

/*************************/
/* (Un)grouping of files */
/*************************/

/**
 * tifiles_group_files:
 * @src_filenames: a NULL-terminated array of strings (list of files to group).
 * @dst_filename: the filename where to store the group.
 *
 * Group several TI files into a single one (group file).
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_group_files(char **src_filenames, const char *dst_filename)
{
  int i, n;
  TiRegular **src = NULL;
  TiRegular *dst = NULL;
  char *unused;

  for (n = 0; src_filenames[n] != NULL; n++);

  src = (TiRegular **) calloc(n + 1, sizeof(TiRegular *));
  if (src == NULL)
    return ERR_MALLOC;

  for (i = 0; i < n; i++) {
    src[i] = (TiRegular *) calloc(1, sizeof(TiRegular));
    if (src[i] == NULL)
      return ERR_MALLOC;

    TRY(tifiles_file_read_regular(src_filenames[i], src[i]));
  }
  src[i] = NULL;

  TRY(tifiles_group_contents(src, &dst));

  for (i = 0; i < n; i++) {
    TRY(tifiles_content_free_regular(src[i]));
    free(src[i]);
  }
  free(src);

  TRY(tifiles_file_write_regular(dst_filename, dst, &unused));

  return 0;
}

/**
 * tifiles_ungroup_file:
 * @src_filename: full path of file to ungroup.
 *
 * Ungroup a TI 'group' file into several files. Resulting files have the
 * same name as the variable stored within group file.
 * Beware: there is no existence check; files may be overwritten !
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_ungroup_file(const char *src_filename)
{
  TiRegular src;
  TiRegular **dst;
  TiRegular **ptr;
  char *real_name;

  TRY(tifiles_file_read_regular(src_filename, &src));

  TRY(tifiles_ungroup_content(&src, &dst));

  for (ptr = dst; *ptr != NULL; ptr++)
    TRY(tifiles_file_write_regular(NULL, *ptr, &real_name));

  return 0;
}
