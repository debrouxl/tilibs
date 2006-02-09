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
  Calcs: 73/82/83/83+/85/86 & 89/92/92+
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_int.h"
#include "file_err.h"
#include "typesxx.h"
#include "filesxx.h"
#include "macros.h"

extern TicalcType tifiles_calc_type;	// current calculator type

/************************/
/* (Un)grouping content */
/************************/

/*
  Group TI files by concatenating an array of structures into a 
  single structure.
  - src [in]: a pointer on an array. The array is terminated by NULL.
  - dst [out]: the address of a pointer. This pointer will contain the group
  file. It's dynamically allocated.
  - int [out]: an error code.
*/
TIEXPORT int TICALL tifiles_group_contents(TiRegular ** srcs,
					   TiRegular ** dest)
{
  TiRegular *dst;
  int i;
  int n;

  for (n = 0; srcs[n] != NULL; n++);

  dst = *dest = (TiRegular *) calloc(1, sizeof(TiRegular));
  if (dst == NULL)
    return ERR_MALLOC;
  memcpy(dst, srcs[0], sizeof(TiRegular));

  dst->num_entries = n;
  dst->entries = (TiVarEntry *) calloc(n, sizeof(TiVarEntry));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < n; i++) {
    TiRegular *src = srcs[i];

    TRY(ti8x_dup_VarEntry(&(dst->entries[i]), &(src->entries[0])));
  }

  return 0;
}

/*
  Ungroup a TI file by exploding the structure into an array of structure.
  Each structure is a single file.
  - src [in]: the address of the group structure
  - dst [out]: the address of a pointer. This pointer points on an array of 
  structure. The array is terminated by NULL and dynamically allocated
  by the function.
  - int [out]: an error code.
 */
TIEXPORT int TICALL tifiles_ungroup_content(TiRegular * src,
					    TiRegular *** dest)
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
	tifiles_compute_checksum((uint8_t *) dst_entry, 15);
    dst[i]->checksum +=
	tifiles_compute_checksum(dst_entry->data, dst_entry->size);
  }
  dst[i] = NULL;

  return 0;
}

/*************************/
/* (Un)grouping of files */
/*************************/

/*
  Group some files
  - filenames [in]: an array of strings (list of files to read).
  Must be NULL terminated.
  - filename [in]: the filename where the group will be written
  - int [out]: an error code
 */
TIEXPORT int TICALL tifiles_group_files(char **filenames,
					const char *filename)
{
  int i, n;
  TiRegular **src = NULL;
  TiRegular *dst = NULL;
  char *unused;

  for (n = 0; filenames[n] != NULL; n++);

  src = (TiRegular **) calloc(n + 1, sizeof(TiRegular *));
  if (src == NULL)
    return ERR_MALLOC;

  for (i = 0; i < n; i++) {
    src[i] = (TiRegular *) calloc(1, sizeof(TiRegular));
    if (src[i] == NULL)
      return ERR_MALLOC;

    TRY(tifiles_read_regular_file(filenames[i], src[i]));
  }
  src[i] = NULL;

  TRY(tifiles_group_contents(src, &dst));

  for (i = 0; i < n; i++) {
    TRY(tifiles_free_regular_content(src[i]));
    free(src[i]);
  }
  free(src);

  TRY(tifiles_write_regular_file(filename, dst, &unused));

  return 0;
}

/*
  Ungroup a group file into several single files:
  - filename [in]: the name of a group file
  - int [out]: an error code
 */
TIEXPORT int TICALL tifiles_ungroup_file(const char *filename)
{
  TiRegular src;
  TiRegular **dst;
  TiRegular **ptr;
  char *real_name;

  TRY(tifiles_read_regular_file(filename, &src));

  TRY(tifiles_ungroup_content(&src, &dst));

  for (ptr = dst; *ptr != NULL; ptr++)
    TRY(tifiles_write_regular_file(NULL, *ptr, &real_name));

  return 0;
}
