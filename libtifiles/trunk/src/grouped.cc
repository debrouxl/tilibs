/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
  Grouping/Ungrouping routines
  Calcs: 73/82/83/83+/84+/85/86 & 89/89tm/92/92+/V200
*/

#include <stdlib.h>
#include <string.h>

#include "tifiles.h"
#include "error.h"
#include "macros.h"
#include "files8x.h"
#include "files9x.h"
#include "logging.h"

/***********/
/* Freeing */
/***********/

/**
 * tifiles_content_create_group:
 * @n_entries: number of variables to allocate
 *
 * Convenient function which create a NULL-terminated array of #FileContent 
 * structures (typically used to store a group file).
 *
 * Return value: the array or NULL if failed.
 **/
FileContent** TICALL tifiles_content_create_group(unsigned int n_entries)
{
	return (FileContent **)g_malloc0((n_entries + 1) * sizeof(FileContent *));
}

/**
 * tifiles_content_delete_group:
 *
 * Convenient function which free a NULL-terminated array of #FileContent 
 * structures (typically used to store a group file) and the array itself.
 *
 * Return value: always 0.
 **/
int TICALL tifiles_content_delete_group(FileContent **array)
{
	int i, n;

	if (array != NULL)
	{
		// counter number of files to group
		for (n = 0; array[n] != NULL; n++);

		// release allocated memory in structures
		for (i = 0; i < n; i++)
		{
			tifiles_content_delete_regular(array[i]);
		}
		g_free(array);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/************************/
/* (Un)grouping content */
/************************/

/**
 * tifiles_group_contents:
 * @src_contents: a pointer on an array of #FileContent structures. The array must be NULL-terminated.
 * @dst_content: the address of a pointer. This pointer will see the allocated group file.
 *
 * Must be freed when no longer needed as well as the content of each #FileContent structure
 * (use #tifiles_content_delete_regular as usual).
 *
 * Group several #FileContent structures into a single one.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_group_contents(FileContent **src_contents, FileContent **dst_content)
{
	FileContent *dst;
	unsigned int i, j, n;

	if (src_contents == NULL || dst_content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	for (n = 0; src_contents[n] != NULL; n++)
	{
		if (ticonv_model_is_tinspire(src_contents[n]->model))
		{
			return ERR_BAD_CALC;
		}
	}

	dst = (FileContent *)g_malloc0(sizeof(FileContent));
	if (dst == NULL)
		return ERR_MALLOC;

	if (n > 0)
	{
		memcpy(dst, src_contents[0], sizeof(FileContent));
	}

	dst->num_entries = n;
	dst->entries = (VarEntry **)g_malloc0((n + 1) * sizeof(VarEntry*));
	if (dst->entries == NULL)
	{
		g_free(dst);
		return ERR_MALLOC;
	}

	for (i = 0; i < n; i++)
	{
		FileContent *src = src_contents[i];

		for (j = 0; j < src->num_entries; j++)
			dst->entries[i] = tifiles_ve_dup(src->entries[j]);
	}

	*dst_content = dst;

	return 0;
}

/**
 * tifiles_ungroup_content:
 * @src_content: a pointer on the structure to unpack.
 * @dst_contents: the address of your pointer. This pointers will point on a 
 * dynamically allocated array of structures. The array is terminated by NULL.
 *
 * Ungroup a TI file by exploding the structure into an array of structures.
 *
 * Array must be freed when no longer needed as well as the content of each #FileContent 
 * structure (use #tifiles_content_delete_regular as usual).
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_ungroup_content(FileContent *src, FileContent ***dest)
{
	unsigned int i;
	FileContent **dst;

	if (src == NULL || dest == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (ticonv_model_is_tinspire(src->model))
	{
		return ERR_BAD_CALC;
	}

	// allocate an array of FileContent structures (NULL terminated)
	dst = *dest = (FileContent **)g_malloc0((src->num_entries + 1) * sizeof(FileContent *));
	if (dst == NULL)
	{
		return ERR_MALLOC;
	}

	// parse each entry and duplicate it into a single content
	for (i = 0; i < src->num_entries; i++)
	{
		VarEntry *dst_entry = NULL;

		// allocate and duplicate content
		dst[i] = (FileContent *)g_malloc0(sizeof(FileContent));
		if (dst[i] == NULL)
		{
			return ERR_MALLOC;
		}
		memcpy(dst[i], src, sizeof(FileContent));

		// allocate and duplicate entry
		dst[i]->entries = (VarEntry **)g_malloc0((1+1) * sizeof(VarEntry*));
		dst_entry = dst[i]->entries[0] = tifiles_ve_dup(src->entries[i]);

		// update some fields
		dst[i]->num_entries = 1;
		dst[i]->checksum += tifiles_checksum((uint8_t *) dst_entry, 15);
		dst[i]->checksum += tifiles_checksum(dst_entry->data, dst_entry->size);
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
int TICALL tifiles_group_files(char **src_filenames, const char *dst_filename)
{
	int i, n;
	FileContent **src = NULL;
	FileContent *dst = NULL;
	int ret = 0;

	if (src_filenames == NULL || dst_filename == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	// count number of files to group, while performing several checks
	for (n = 0; src_filenames[n] != NULL; n++)
	{
		if (!tifiles_file_is_ti(src_filenames[n]))
		{
			return ERR_INVALID_FILE;
		}
		if (ticonv_model_is_tinspire(tifiles_file_get_model(src_filenames[n])))
		{
			return ERR_BAD_CALC;
		}
	}

	// allocate space for that
	src = (FileContent **)g_malloc0((n + 1) * sizeof(FileContent *));
	if (src == NULL)
	{
		return ERR_MALLOC;
	}

	// allocate each structure and load file content
	for (i = 0; i < n; i++)
	{
		src[i] = (FileContent *)g_malloc0(sizeof(FileContent));
		if (src[i] == NULL)
		{
			ret = ERR_MALLOC;
			goto tgf2;
		}

		ret = tifiles_file_read_regular(src_filenames[i], src[i]);
		if (ret)
		{
			goto tgf2;
		}
	}
	src[i] = NULL;

	// group the array of structures
	ret = tifiles_group_contents(src, &dst);
	if (ret)
	{
		goto tgf;
	}

	// write grouped file
	ret = tifiles_file_write_regular(dst_filename, dst, NULL);
	if (ret)
	{
		goto tgf;
	}

	ret = 0;
	// release allocated memory
tgf:
	tifiles_content_delete_regular(dst);
tgf2:
	tifiles_content_delete_group(src);

	return ret;
}

/**
 * tifiles_ungroup_file:
 * @src_filename: full path of file to ungroup.
 * @dst_filenames: NULL or the address of a pointer where to store a NULL-terminated 
 * array of strings which contain the list of ungrouped files.
 *
 * Ungroup a TI 'group' file into several files. Resulting files have the
 * same name as the variable stored within group file.
 * Beware: there is no existence check; files may be overwritten !
 *
 * %dst_filenames must be freed when no longer used.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_ungroup_file(const char *src_filename, char ***dst_filenames)
{
	FileContent *src = NULL;
	FileContent **ptr, **dst = NULL;
	char *real_name, **p;
	int i, n;
	int ret;

	if (src_filename == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (ticonv_model_is_tinspire(tifiles_file_get_model(src_filename)))
	{
		return ERR_BAD_CALC;
	}

	// read group file
	src = tifiles_content_create_regular(CALC_NONE);
	ret = tifiles_file_read_regular(src_filename, src);
	if (ret)
	{
		goto tuf3;
	}

	// ungroup structure
	ret = tifiles_ungroup_content(src, &dst);
	if (ret)
	{
		goto tuf2;
	}

	// count number of structures and allocates array of strings
	for (ptr = dst, n = 0; *ptr != NULL; ptr++, n++);
	if (dst_filenames != NULL)
	{
		*dst_filenames = (char **)g_malloc0((n + 1) * sizeof(char *));
	}

	// store each structure content to file
	for (ptr = dst, i = 0; *ptr != NULL; ptr++, i++)
	{
		ret = tifiles_file_write_regular(NULL, *ptr, &real_name);
		if (ret)
		{
			goto tuf;
		}

		if (dst_filenames != NULL)
		{
			(*dst_filenames)[i] = real_name;
		}
		else
		{
			g_free(real_name);
		}
	}

	ret = 0;
	goto tuf2;

tuf:
	if (dst_filenames != NULL)
	{
		for (p = *dst_filenames; *p; p++)
		{
			g_free(*p);
		}
		g_free(*dst_filenames);
		*dst_filenames = NULL;
	}

tuf2:
	// release allocated memory
	tifiles_content_delete_regular(src);
tuf3:
	tifiles_content_delete_group(dst);
	return ret;
}

/**
 * tifiles_content_add_entry:
 * @content: a file content (single/group only).
 * @ve: the entry to add
 *
 * Adds the entry to the file content and updates internal structures.
 * Beware: the entry is not duplicated.
 *
 * Return value: the number of entries.
 **/
int TICALL tifiles_content_add_entry(FileContent *content, VarEntry *ve)
{
	if (content != NULL)
	{
		content->entries = tifiles_ve_resize_array(content->entries, content->num_entries + 1);
		if (ve == NULL)
		{
			tifiles_critical("%s: adding NULL VarEntry ???", __FUNCTION__);
		}
		content->entries[content->num_entries] = ve;
		content->num_entries++;

		return content->num_entries;
	}
	else
	{
		tifiles_critical("%s: content is NULL", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_content_del_entry:
 * @content: a file content (single/group only).
 * @ve: the entry to remove
 *
 * Search for entry name and remove it from file content (not tested !).
 *
 * Return value: the number of entries or -1 if not found.
 **/
int TICALL tifiles_content_del_entry(FileContent *content, VarEntry *ve)
{
	unsigned int i, j;

	if (content != NULL)
	{
		if (ve == NULL)
		{
			tifiles_critical("%s: deleting NULL VarEntry ???", __FUNCTION__);
			return content->num_entries;
		}

		// Search for entry
		for (i = 0; i < content->num_entries; i++)
		{
			VarEntry *s = content->entries[i];

			if (!strcmp(s->folder, ve->folder) && !strcmp(s->name, ve->name))
			{
				break;
			}
		}

		// Not found ? Exit !
		if (i == content->num_entries)
		{
			return -1;
		}

		// Release
		tifiles_ve_delete(content->entries[i]);

		// And shift
		for (j = i; j < content->num_entries; j++)
		{
			content->entries[j] = content->entries[j+1];
		}
		content->entries[j] = NULL;

		// And resize
		content->entries = tifiles_ve_resize_array(content->entries, content->num_entries - 1);
		content->num_entries--;

		return content->num_entries;
	}
	else
	{
		tifiles_critical("%s: content is NULL", __FUNCTION__);
		return 0;
	}
}

/**
 * tifiles_group_add_file:
 * @src_filename: the file to add to group file
 * @dst_filename: the group file
 *
 * Add src_filename content to dst_filename content and write to dst_filename.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL tifiles_group_add_file(const char *src_filename, const char *dst_filename)
{
	CalcModel src_model;
	CalcModel dst_model;
	FileContent* src_content = NULL;
	FileContent* dst_content = NULL;
	unsigned int i;
	int ret = 0;

	if (src_filename == NULL || dst_filename == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	// src can be single/group file and dst must be group file
	if (!tifiles_file_is_group(dst_filename))
	{
		return -1;
	}

	src_model = tifiles_file_get_model(src_filename);
	dst_model = tifiles_file_get_model(dst_filename);

	src_content = tifiles_content_create_regular(src_model);
	dst_content = tifiles_content_create_regular(dst_model);

	ret = tifiles_file_read_regular(src_filename, src_content);
	if (!ret)
	{
		ret = tifiles_file_read_regular(dst_filename, dst_content);
		if (!ret)
		{
			for (i = 0; i < src_content->num_entries; i++)
			{
				tifiles_content_add_entry(dst_content, tifiles_ve_dup(src_content->entries[i]));
			}

			ret = tifiles_file_write_regular(dst_filename, dst_content, NULL);

			tifiles_content_delete_regular(dst_content);
		}

		tifiles_content_delete_regular(src_content);
	}
	else
	{
		tifiles_content_delete_regular(dst_content);
	}

	return ret;
}

/**
 * tifiles_group_del_file:
 * @src_filename: the file to remove from group file
 * @dst_filename: the group file
 *
 * Search for entry and remove it from file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL tifiles_group_del_file(VarEntry *entry, const char *dst_filename)
{
	CalcModel dst_model;
	FileContent* dst_content = NULL;
	int ret = 0;

	if (entry == NULL || dst_filename == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	// src can be single/group file and dst must be group file
	if (!tifiles_file_is_group(dst_filename))
	{
		return -1;
	}

	dst_model = tifiles_file_get_model(dst_filename);
	dst_content = tifiles_content_create_regular(dst_model);
	ret = tifiles_file_read_regular(dst_filename, dst_content);
	if (!ret)
	{
		tifiles_content_del_entry(dst_content, entry);
		tifiles_file_display_regular(dst_content);

		ret = tifiles_file_write_regular(dst_filename, dst_content, NULL);

		tifiles_content_delete_regular(dst_content);
	}

	return ret;
}
