/* Hey EMACS -*- linux-c -*- */
/* $Id: grouped.c 1737 2006-01-23 12:54:47Z roms $ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2006  Romain Lievin
 *  Copyright (C) 2006  Kevin Kofler
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
	TiGroup (*.tig) management
	A TiGroup file is in fact a ZIP archive with no compression (stored).

	Please note that I don't use USEWIN32IOAPI!
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#include <archive.h>
#include <archive_entry.h>

#include <ticonv.h>
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "rwfile.h"

#ifdef __WIN32__
#include <io.h>
#endif

// Whether to print detailed information about TigEntry, TigContent instances throughout their lifecycle.
//#define TRACE_CONTENT_INSTANCES

#define WRITEBUFFERSIZE (8192)

/**
 * tifiles_te_create:
 * @filename: internal filename in archive.
 * @type: file type (regular or flash)
 * @model: calculator model
 *
 * Allocates a TigEntry structure and allocates fields (aka call #tifiles_content_create_flash/regular for you).
 *
 * Return value: the allocated block.
 **/
TigEntry* TICALL tifiles_te_create(const char *filename, FileClass type, CalcModel model)
{
	TigEntry *entry = nullptr;

	if (filename != nullptr && strcmp(filename, ""))
	{
		entry = (TigEntry *)g_malloc0(sizeof(TigEntry));
		if (entry != nullptr)
		{
			entry->filename = g_path_get_basename(filename);
			entry->type = type;

			if (type == TIFILE_FLASH)
			{
				entry->content.flash = tifiles_content_create_flash(model);
			}
			else if (type & TIFILE_REGULAR)
			{
				entry->content.regular = tifiles_content_create_regular(model);
			}
		}
	}
	else
	{
		tifiles_critical("%s: invalid filename", __FUNCTION__);
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_te_create: %p", entry);
	tifiles_te_display(entry);
#endif

	return entry;
}

/**
 * tifiles_te_delete:
 * @entry: a #TigEntry structure.
 *
 * Destroy a #TigEntry structure as well as fields.
 *
 * Return value: always 0.
 **/
int TICALL tifiles_te_delete(TigEntry* entry)
{
#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_te_delete: %p", entry);
	tifiles_te_display(entry);
#endif

	if (entry != nullptr)
	{
		g_free(entry->filename);

		if (entry->type == TIFILE_FLASH)
		{
			tifiles_content_delete_flash(entry->content.flash);
		}
		else if (entry->type & TIFILE_REGULAR)
		{
			tifiles_content_delete_regular(entry->content.regular);
		}

		g_free(entry);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_te_display:
 * @entry: a #TigEntry structure pointer.
 *
 * Display a #TigEntry structure's contents.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_te_display(TigEntry* entry)
{
	if (entry == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	tifiles_info("Filename:          %s", entry->filename);
	tifiles_info("File class:        %04X (%u)", entry->type, entry->type);

	if (entry->type == TIFILE_FLASH)
	{
		tifiles_file_display_flash(entry->content.flash);
	}
	else if (entry->type & TIFILE_REGULAR)
	{
		tifiles_file_display_regular(entry->content.regular);
	}
	else
	{
		tifiles_info("Data:              %p", entry->content.data);
	}

	return 0;
}

/**
 * tifiles_te_create_array:
 * @nelts: size of NULL-terminated array (number of #TigEntry structures).
 *
 * Allocate a NULL-terminated array of #TigEntry structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TigEntry**	TICALL tifiles_te_create_array(unsigned int nelts)
{
	return (TigEntry **)g_malloc0((nelts + 1) * sizeof(TigEntry *));
}

/**
 * tifiles_te_resize_array:
 * @array: address of array
 * @nelts: size of NULL-terminated array (number of #TigEntry structures).
 *
 * Re-allocate a NULL-terminated array of #TigEntry structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TigEntry**	TICALL tifiles_te_resize_array(TigEntry** array, unsigned int nelts)
{
	TigEntry ** ptr = (TigEntry **)g_realloc(array, (nelts + 1) * sizeof(TigEntry *));
	if (ptr != nullptr)
	{
		ptr[nelts] = nullptr;
	}
	return ptr;
}

/**
 * tifiles_ve_delete_array:
 * @array: an NULL-terminated array of TigEntry structures.
 *
 * Free the whole array (data buffer, TigEntry structure and array itself).
 *
 * Return value: none.
 **/
void			TICALL tifiles_te_delete_array(TigEntry** array)
{
#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_te_delete_array: %p", array);
#endif

	if (array != nullptr)
	{
		for (TigEntry** ptr = array; *ptr; ptr++)
		{
			tifiles_te_delete(*ptr);
		}
		g_free(array);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}
}

/**
 * tifiles_te_sizeof_array:
 * @array: an NULL-terminated array of TigEntry structures.
 * @r: number of FileContent entries
 * @f: number of FlashContent entries
 *
 * Returns the size of a #TigEntry array.
 *
 * Return value: none.
 **/
int TICALL tifiles_te_sizeof_array(TigEntry** array)
{
	int i = 0;

	if (array != nullptr)
	{
		for (TigEntry** p = array; *p; p++, i++);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return i;
}

// ---------------------------------------------------------------------------

/**
 * tifiles_content_add_te:
 * @content: a file content (TiGroup).
 * @te: the entry to add
 *
 * Adds the entry to the file content and updates internal structures.
 * Beware: the entry is not duplicated.
 *
 * Return value: the number of entries.
 **/
int TICALL tifiles_content_add_te(TigContent *content, TigEntry *te)
{
	if (content == nullptr || te == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return 0;
	}

	if (te->type == TIFILE_FLASH)
	{
		int n = content->n_apps;

		content->app_entries = tifiles_te_resize_array(content->app_entries, n + 1);

		content->app_entries[n++] = te;
		content->app_entries[n] = nullptr;
		content->n_apps = n;

		return n;
	}
	else if (te->type & TIFILE_REGULAR)
	{
		int n = content->n_vars;

		content->var_entries = tifiles_te_resize_array(content->var_entries, n + 1);

		content->var_entries[n++] = te;
		content->var_entries[n] = nullptr;
		content->n_vars = n;

		return n;
	}

	return 0;
}

/**
 * tifiles_content_del_te:
 * @content: a file content (TiGroup).
 * @te: the entry to remove
 *
 * Search for entry name and remove it from file content.
 *
 * Return value: the number of entries or -1 if not found.
 **/
int TICALL tifiles_content_del_te(TigContent *content, TigEntry *te)
{
	unsigned int i, j, k;

	if (content == nullptr || te == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_del_te: %p %p", content, te);
	tifiles_file_display_tigcontent(content);
	tifiles_te_display(te);
#endif

	// Search for entry
	for (i = 0; i < content->n_vars && (te->type & TIFILE_REGULAR); i++)
	{
		const TigEntry *s = content->var_entries[i];

		if (!strcmp(s->filename, te->filename))
		{
			break;
		}
	}

	for (j = 0; j < content->n_apps && (te->type & TIFILE_FLASH); j++)
	{
		const TigEntry *s = content->app_entries[i];

		if (!strcmp(s->filename, te->filename))
		{
			break;
		}
	}

	// Not found ? Exit !
	if ((i == content->n_vars) && (j == content->n_apps))
	{
		return -1;
	}

	// Release
	if (i < content->n_vars)
	{
		// Delete
		tifiles_te_delete(content->var_entries[i]);

		// And shift
		for (k = i; k < content->n_vars; k++)
		{
			content->var_entries[k] = content->var_entries[k+1];
		}
		content->var_entries[k] = nullptr;

		// And resize
		content->var_entries = tifiles_te_resize_array(content->var_entries, content->n_vars - 1);
		content->n_vars--;

		return content->n_vars;
	}

	if (j < content->n_apps)
	{
		// Delete
		tifiles_te_delete(content->app_entries[j]);

		// And shift
		for (k = j; k < content->n_apps; k++)
		{
			content->app_entries[k] = content->app_entries[k+1];
		}
		content->app_entries[k] = nullptr;

		// And resize
		content->app_entries = tifiles_te_resize_array(content->app_entries, content->n_apps - 1);
		content->n_apps--;

		return content->n_apps;
	}

	return 0;
}

#ifndef __WIN32__
# define stricmp strcasecmp
#endif

/**
 * tifiles_tigroup_add_file:
 * @src_filename: the file to add to TiGroup file
 * @dst_filename: the TiGroup file (must exist!)
 *
 * Add src_filename content to dst_filename content and write to dst_filename.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL tifiles_tigroup_add_file(const char *src_filename, const char *dst_filename)
{
	TigContent *content = nullptr;
	int ret = 0;

	if (src_filename == nullptr || dst_filename == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

	// group file is created if non existent
	if (!stricmp(tifiles_fext_get(dst_filename), "tig"))
	{
		if (!g_file_test(dst_filename, G_FILE_TEST_EXISTS))
		{
			content = tifiles_content_create_tigroup(CALC_NONE, 0);
			tifiles_file_write_tigroup(dst_filename, content);
			tifiles_content_delete_tigroup(content);
			content = nullptr;
		}
	}

	// src can't be a TiGroup file but dst should be
	if (!(tifiles_file_is_ti(src_filename) && !tifiles_file_is_tigroup(src_filename) &&
		tifiles_file_is_tigroup(dst_filename)))
	{
		return -1;
	}

	// load src file
	const CalcModel model = tifiles_file_get_model(src_filename);
	const FileClass type = tifiles_file_get_class(src_filename);

	TigEntry* te = tifiles_te_create(src_filename, type, model);
	if (te == nullptr)
	{
		ret = ERR_BAD_FILE;
		goto ttaf;
	}
	if (type == TIFILE_FLASH)
	{ 
		ret = tifiles_file_read_flash(src_filename, te->content.flash);
		if (ret)
		{
			goto ttaf;
		}
	}
	else if (type & TIFILE_REGULAR)
	{ 
		ret = tifiles_file_read_regular(src_filename, te->content.regular);
		if (ret)
		{
			goto ttaf;
		}
	}

	// load dst file
	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	ret = tifiles_file_read_tigroup(dst_filename, content);
	if (ret)
	{
		goto ttaf;
	}

	tifiles_content_add_te(content, te);
	ret = tifiles_file_write_tigroup(dst_filename, content);
	if (ret)
	{
		goto ttaf;
	}

	tifiles_content_delete_tigroup(content);

	return 0;

ttaf:	// release on exit
	tifiles_te_delete(te);
	tifiles_content_delete_tigroup(content);
	return ret;
}

/**
 * tifiles_tigroup_del_file:
 * @src_filename: the file to remove from TiGroup file
 * @dst_filename: the TiGroup file
 *
 * Search for entry and remove it from file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL tifiles_tigroup_del_file(TigEntry *entry, const char *filename)
{
	TigContent* content = nullptr;
	int ret = 0;

	if (entry == nullptr || filename == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_tigroup_del_file: %p", entry);
	tifiles_te_display(entry);
#endif

	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	ret = tifiles_file_read_tigroup(filename, content);
	if (!ret)
	{
		(void)tifiles_content_del_te(content, entry);
		ret = tifiles_file_write_tigroup(filename, content);
	}

	tifiles_content_delete_tigroup(content);
	return ret;
}

// ---------------------------------------------------------------------------

/**
 * tifiles_tigroup_contents:
 * @src_contents1: a pointer on an array of #FileContent structures or NULL. The array must be NULL-terminated.
 * @src_contents2: a pointer on an array of #FlashContent structures or NULL. The array must be NULL-terminated.
 * @dst_content: the address of a pointer. This pointer will see the allocated TiGroup file.
 *
 * Group several #FileContent/#FlashContent structures into a single one.
 * Must be freed when no longer used by a call to #tifiles_content_delete_tigroup.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_tigroup_contents(FileContent **src_contents1, FlashContent **src_contents2, TigContent **dst_content)
{
	int i, m=0, n=0;
	CalcModel model = CALC_NONE;

	if (src_contents1 == nullptr && src_contents2 == nullptr)
	{
		return -1;
	}

	if (dst_content == nullptr)
	{
		tifiles_critical("%s: dst_content is NULL", __FUNCTION__);
		return -1;
	}

	if (src_contents1)
	{
		for (m = 0; src_contents1[m] != nullptr; m++);
	}
	if (src_contents2)
	{
		for (n = 0; src_contents2[n] != nullptr; n++);
	}

	if (src_contents2)
	{
		if (*src_contents2)
		{
			model = src_contents2[0]->model;
		}
	}
	if (src_contents1)
	{
		if (*src_contents1)
		{
			model = src_contents1[0]->model;	// FileContent is more precise than FlashContent
		}
	}

	TigContent* content = tifiles_content_create_tigroup(model, m + n);

	if (src_contents1)
	{
		for (i = 0; i < m; i++)
		{
			TigEntry *te = (TigEntry *)g_malloc0(sizeof(TigEntry));

			te->filename = tifiles_build_filename(model, src_contents1[i]->entries[0]);
			te->type = TIFILE_GROUP;
			te->content.regular = tifiles_content_dup_regular(src_contents1[i]);
			tifiles_content_add_te(content, te);
		}
	}

	if (src_contents2)
	{
		for (i = 0; i < n; i++)
		{
			VarEntry ve;
			FlashContent *ptr;

			for (ptr = src_contents2[i]; ptr; ptr = ptr->next)
			{
				if (ptr->data_type == tifiles_flash_type(model))
				{
					break;
				}
			}
			if (ptr == nullptr)
			{
				tifiles_critical("%s: ptr is NULL, skipping", __FUNCTION__);
				continue;
			}

			TigEntry* te = (TigEntry*)g_malloc0(sizeof(TigEntry));
			ve.folder[0] = 0;
			strncpy(ve.name, ptr->name, sizeof(ve.name) - 1);
			ve.name[sizeof(ve.name) - 1] = 0;
			ve.type = ptr->data_type;
			te->filename = tifiles_build_filename(model, &ve);
			te->type = TIFILE_FLASH;
			te->content.flash = tifiles_content_dup_flash(src_contents2[i]);
			tifiles_content_add_te(content, te);
		}
	}

	*dst_content = content;

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_tigroup_contents: %p", content);
	tifiles_file_display_tigcontent(content);
#endif

	return 0;
}

/**
 * tifiles_untigroup_content:
 * @src_content: a pointer on the structure to unpack.
 * @dst_contents1: the address of your pointer. This pointers will point on a 
 * @dst_contents2: the address of your pointer. This pointers will point on a 
 * dynamically allocated array of structures. The array is terminated by NULL.
 *
 * Ungroup a TiGroup file by exploding the structure into an array of structures.
 * Must be freed when no longer used by a call to #tifiles_content_delete_tigroup.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_untigroup_content(TigContent *src_content, FileContent ***dst_contents1, FlashContent ***dst_contents2)
{
	const TigContent *src = src_content;
	FileContent **dst1 = nullptr;
	FlashContent **dst2 = nullptr;

	if (src_content == nullptr || dst_contents1 == nullptr || dst_contents2 == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_untigroup_content: %p", src_content);
	tifiles_file_display_tigcontent(src_content);
#endif

	// allocate an array of FileContent/FlashContent structures (NULL terminated)
	dst1 = (FileContent **)g_malloc0((src->n_vars+1) * sizeof(FileContent *));
	if (dst1 == nullptr)
	{
		return ERR_MALLOC;
	}
	dst2 = (FlashContent **)g_malloc0((src->n_apps+1) * sizeof(FlashContent *));
	if (dst2 == nullptr)
	{
		g_free(dst1);
		return ERR_MALLOC;
	}

	// parse each entry and duplicate it into a single content
	for (unsigned int i = 0; i < src->n_vars; i++)
	{
		const TigEntry *te = src->var_entries[i];

		dst1[i] = tifiles_content_dup_regular(te->content.regular);
	}

	for (unsigned int j = 0; j < src->n_apps; j++)
	{
		const TigEntry *te = src->app_entries[j];

		dst2[j] = tifiles_content_dup_flash(te->content.flash);
	}

	*dst_contents1 = dst1;
	*dst_contents2 = dst2;

	return 0;
}

/**
 * tifiles_group_files:
 * @src_filenames: a NULL-terminated array of strings (list of files to group).
 * @dst_filename: the filename where to store the TiGroup.
 *
 * Group several TI files (regular/flash) into a single one (TiGroup file).
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_tigroup_files(char **src_filenames, const char *dst_filename)
{
	FileContent **src1 = nullptr;
	FlashContent **src2 = nullptr;
	TigContent *dst = nullptr;
	int i, j, k, m, n;
	int ret = 0;

	if (src_filenames == nullptr || dst_filename == nullptr)
	{
		tifiles_critical("%s: an argument is NULL !", __FUNCTION__);
		return -1;
	}

	// counts number of files to group and allocate space for that
	for (k = m = n = 0; src_filenames[k]; k++)
	{
		if (tifiles_file_is_regular(src_filenames[k]))
		{
			m++;
		}
		else if (tifiles_file_is_flash(src_filenames[k]))
		{
			n++;
		}
	}
	const CalcModel model = tifiles_file_get_model(src_filenames[0]);

	// allocate space for that
	src1 = (FileContent **)g_malloc0((m + 1) * sizeof(FileContent *));
	if (src1 == nullptr)
	{
		return ERR_MALLOC;
	}

	src2 = (FlashContent **)g_malloc0((n + 1) * sizeof(FlashContent *));
	if (src2 == nullptr)
	{
		g_free(src1);
		return ERR_MALLOC;
	}

	for (i = j = k = 0; k < m+n; k++)
	{
		if (tifiles_file_is_regular(src_filenames[k]))
		{
			src1[i] = tifiles_content_create_regular(model);
			ret = tifiles_file_read_regular(src_filenames[k], src1[i]);
			if (ret)
			{
				goto tgf;
			}
			i++;
		}
		else if (tifiles_file_is_flash(src_filenames[k]))
		{
			src2[j] = tifiles_content_create_flash(model);
			ret = tifiles_file_read_flash(src_filenames[k], src2[j]);
			if (ret)
			{
				goto tgf;
			}
			j++;
		}
	}

	ret = tifiles_tigroup_contents(src1, src2, &dst);
	if (ret)
	{
		goto tgf;
	}

	ret = tifiles_file_write_tigroup(dst_filename, dst);

tgf:
	for (i = 0; i < m; i++)
	{
		g_free(src1[i]);
	}
	g_free(src1);
	for (i = 0; i < n; i++)
	{
		g_free(src2[i]);
	}
	g_free(src2);
	tifiles_content_delete_tigroup(dst);

	return ret;
}

/**
 * tifiles_ungroup_file:
 * @src_filename: full path of file to ungroup.
 * @dst_filenames: NULL or the address of a pointer where to store a NULL-terminated 
 * array of strings which contain the list of ungrouped files (regular/flash).
 *
 * Ungroup a TiGroup file into several files. Resulting files have the
 * same name as the variable stored within group file.
 * Beware: there is no existence check; files may be overwritten !
 *
 * %dst_filenames must be freed when no longer used.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_untigroup_file(const char *src_filename, char ***dst_filenames)
{
	TigContent *src = nullptr;
	FileContent **ptr1, **dst1 = nullptr;
	FlashContent **ptr2, **dst2 = nullptr;
	char *real_name;
	unsigned int i, j;
	int ret = 0;

	if (src_filename == nullptr)
	{
		tifiles_critical("%s: src_filename is NULL !", __FUNCTION__);
		return -1;
	}

	// read TiGroup file
	src = tifiles_content_create_tigroup(CALC_NONE, 0);
	ret = tifiles_file_read_tigroup(src_filename, src);
	if (ret)
	{
		goto tuf;
	}

	// ungroup structure
	ret = tifiles_untigroup_content(src, &dst1, &dst2);
	if (ret)
	{
		goto tuf;
	}

	// count number of structures and allocates array of strings
	if (dst_filenames != nullptr)
	{
		*dst_filenames = (char **)g_malloc((src->n_vars + src->n_apps + 1) * sizeof(char *));
	}

	// store each structure content to file
	for (ptr1 = dst1, i = 0; *ptr1 != nullptr || i < src->n_vars; ptr1++, i++)
	{
		ret = tifiles_file_write_regular(nullptr, *ptr1, &real_name);
		if (ret)
		{
			goto tuf;
		}

		if (dst_filenames != nullptr)
		{
			*dst_filenames[i] = real_name;
		}
		else
		{
			g_free(real_name);
		}
	}

	for (ptr2 = dst2, j = 0; *ptr2 != nullptr || j < src->n_apps; ptr2++, j++)
	{
		ret = tifiles_file_write_flash2(nullptr, *ptr2, &real_name);
		if (ret)
		{
			goto tuf;
		}

		if (dst_filenames != nullptr)
		{
			*dst_filenames[i+j] = real_name;
		}
		else
		{
			g_free(real_name);
		}
	}

	// release allocated memory
tuf:
	if (dst1)
	{
		for (ptr1 = dst1; *ptr1; ptr1++)
		{
			tifiles_content_delete_regular(*ptr1);
		}
	}
	if (dst2)
	{
		for (ptr2 = dst2; *ptr2; ptr2++)
		{
			tifiles_content_delete_flash(*ptr2);
		}
	}
	tifiles_content_delete_tigroup(src);

	return ret;
}

// ---------------------------------------------------------------------------

/**
 * tifiles_content_create_tigroup:
 * @model: a calculator model or CALC_NONE.
 * @n: number of #tigEntry entries
 *
 * Allocates a TigContent structure. Note: the calculator model is not required
 * if the content is used for file reading but is compulsory for file writing.
 *
 * Return value: the allocated block.
 **/
TigContent* TICALL tifiles_content_create_tigroup(CalcModel model, unsigned int n)
{
	TigContent* content = (TigContent *)g_malloc0(sizeof(*content));
	if (content != nullptr)
	{
		char comment[43];
		content->model = content->model_dst = model;
		tifiles_comment_set_tigroup_sn(comment, sizeof(comment));
		content->comment = g_strdup(comment);
		content->comp_level = 4;
		content->var_entries = (TigEntry **)g_malloc0(sizeof(TigEntry *));
		content->app_entries = (TigEntry **)g_malloc0(sizeof(TigEntry *));
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_create_tigroup: %p", content);
	tifiles_file_display_tigcontent(content);
#endif

	return content;
}

/**
 * tifiles_content_delete_tigroup:
 *
 * Free the whole content of a @TigContent structure and the content itself.
 *
 * Return value: none.
 **/
int TICALL tifiles_content_delete_tigroup(TigContent *content)
{
	unsigned int i;

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_delete_tigroup: %p", content);
	tifiles_file_display_tigcontent(content);
#endif

	if (content != nullptr)
	{
		if (content->var_entries != nullptr)
		{
			for (i = 0; i < content->n_vars; i++)
			{
				TigEntry* entry = content->var_entries[i];
				tifiles_te_delete(entry);
			}
			g_free(content->var_entries);
		}

		if (content->app_entries != nullptr)
		{
			for (i = 0; i < content->n_apps; i++)
			{
				TigEntry* entry = content->app_entries[i];
				tifiles_te_delete(entry);
			}
			g_free(content->app_entries);
		}

		g_free(content->comment);

		g_free(content);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/* Open a temporary file */
static int open_temp_file(const char *orig_name, char **temp_name)
{
	*temp_name = nullptr;
	const char* suffix = strrchr(orig_name, '.');
	if (suffix && (strchr(suffix, '/') || strchr(suffix, '\\')))
	{
		suffix = nullptr;
	}
	char* templ = g_strconcat("tigXXXXXX", suffix, NULL);

	const int fd = g_file_open_tmp(templ, temp_name, nullptr);

	g_free(templ);
	if (fd == -1) {
		g_free(*temp_name);
		*temp_name = nullptr;
	}

	return fd;
}

/**
 * tifiles_file_read_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content (may be re-allocated).
 *
 * This function loads a TiGroup from \a filename and places its content into \a content.
 * If an error occurs, the structure content is *NOT* released for you.
 *
 * The temporary folder is used by this function to store temporary files.
 * 
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_file_read_tigroup(const char *filename, TigContent *content)
{
	struct archive *arc;
	struct archive_entry *entry;
	int ret = 0;

	if (filename == nullptr || content == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

	// Open ZIP archive
	FILE* tigf = g_fopen(filename, "rb");
	if (tigf == nullptr)
	{
		return ERR_FILE_OPEN;
	}

	if (!(arc = archive_read_new())
	    || archive_read_support_format_zip(arc) != ARCHIVE_OK
	    || archive_read_open_FILE(arc, tigf) != ARCHIVE_OK)
	{
		if (arc)
		{
			archive_read_free(arc);
		}
		fclose(tigf);
		return ERR_FILE_ZIP;
	}

	g_free(content->var_entries);
	content->var_entries = (TigEntry **)g_malloc0(1 * sizeof(TigEntry *));
	content->n_vars = 0;

	g_free(content->app_entries);
	content->app_entries = (TigEntry **)g_malloc0(1 * sizeof(TigEntry *));
	content->n_apps = 0;

	// Get comment
	g_free(content->comment);
	// FIXME: any way to get this from libarchive?
	content->comment = g_strdup("");

	// Parse archive for files
	while (archive_read_next_header(arc, &entry) == ARCHIVE_OK)
	{
		gchar *fname;

		const char* filename_inzip = archive_entry_pathname(entry);
		if (!filename_inzip)
		{
			tifiles_warning("archive contains a file with no name");
			archive_read_data_skip(arc);
			continue;
		}

		// create a temporary file
		const int fd = open_temp_file(filename_inzip, &fname);
		if (fd == -1)
		{
			ret = ERR_FILE_IO;
			goto tfrt_exit;
		}

		// extract data into temporary file
		if (archive_read_data_into_fd(arc, fd) != ARCHIVE_OK)
		{
			close(fd);
			g_unlink(fname);
			g_free(fname);
			ret = ERR_FILE_IO;
			goto tfrt_exit;
		}
		close(fd);

		// add to TigContent
		{
			const CalcModel model = tifiles_file_get_model(fname);

			if (content->model == CALC_NONE)
			{
				content->model = model;
			}

			if (tifiles_file_is_regular(fname))
			{
				TigEntry *tigentry = tifiles_te_create(filename_inzip, tifiles_file_get_class(fname), content->model);

				if (tigentry != nullptr)
				{
					ret = tifiles_file_read_regular(fname, tigentry->content.regular);
					if (ret)
					{
						g_free(tigentry);
						g_unlink(fname);
						g_free(fname);
						break;
					}

					tifiles_content_add_te(content, tigentry);
				}
			}
			else if (tifiles_file_is_flash(fname))
			{
				TigEntry *tigentry = tifiles_te_create(filename_inzip, tifiles_file_get_class(fname), content->model);

				if (tigentry != nullptr)
				{
					ret = tifiles_file_read_flash(fname, tigentry->content.flash);
					if (ret)
					{
						g_free(tigentry);
						g_unlink(fname);
						g_free(fname);
						break;
					}

					tifiles_content_add_te(content, tigentry);
				}
			}
			else
			{
				// skip
			}
		}
		g_unlink(fname);
		g_free(fname);
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_file_read_tigroup: %p", content);
	tifiles_file_display_tigcontent(content);
#endif

	// Close
tfrt_exit:
	archive_read_free(arc);
	fclose(tigf);
	return ret;
}

static int zip_write(struct archive *arc, CalcModel model, const char *origfname, const char *tempfname)
{
	struct stat st;
	int err = 0;
	FILE *f = nullptr;
	int size_read;
	void* buf= nullptr;

	if (arc == nullptr)
	{
		tifiles_critical("zip_write: arc is NULL !");
		return ERR_FILE_ZIP;
	}

	// Set metadata
	struct archive_entry* entry = archive_entry_new();
	if (entry == nullptr)
	{
		tifiles_critical("zip_write: cannot allocate archive entry");
		return ERR_FILE_ZIP;
	}

	if (g_stat(tempfname, (GStatBuf*)&st))
	{
		tifiles_critical("zip_write: cannot stat temporary file");
		archive_entry_free(entry);
		return ERR_FILE_IO;
	}
	archive_entry_copy_stat(entry, &st);

	// ZIP archives don't like greek chars
	char* filenameinzip = ticonv_gfe_to_zfe(model, origfname);
	archive_entry_set_pathname(entry, filenameinzip);
	g_free(filenameinzip);

	// missing tmp file !
	f = g_fopen(tempfname, "rb");
	if (f == nullptr)
	{
		tifiles_critical("zip_write: cannot read temporary file");
		archive_entry_free(entry);
		err = ERR_FILE_IO;
		goto end2;
	}

	if (archive_write_header(arc, entry) != ARCHIVE_OK)
	{
		archive_entry_free(entry);
		err = ERR_FILE_IO;
		goto end;
	}
	archive_entry_free(entry);

	// Allocate buffer
	buf = (void*)g_malloc(WRITEBUFFERSIZE);

	do
	{
		// feed with our data
		size_read = fread(buf, 1, WRITEBUFFERSIZE, f);

		if (size_read < WRITEBUFFERSIZE)
		{
			if (!feof(f))
			{
				tifiles_critical("error in reading %s", tempfname);
				err = ERR_FILE_IO;
			}
		}
		if (size_read > 0)
		{
			if (archive_write_data(arc, buf, size_read) != size_read)
			{
				tifiles_critical("error in writing %s in the zipfile\n", origfname);
				err = ERR_FILE_IO;
			}
		}
	} while (!err && (size_read>0));

	g_free(buf);
end:
	fclose(f);
end2:
	return err;
}

/**
 * tifiles_file_write_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content.
 *
 * This function store TiGroup contents to file. Please note that contents 
 * can contains no data. In this case, the file is void but created.
 *
 * The temporary folder is used by this function to store temporary files.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_file_write_tigroup(const char *filename, TigContent *content)
{
	FILE *tigf;
	struct archive *arc;
	int err = 0;
	TigEntry **ptr;

	if (filename == nullptr || content == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return -1;
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_file_write_tigroup: %p", content);
	tifiles_file_display_tigcontent(content);
#endif

	// Open ZIP archive
	tigf = g_fopen(filename, "wb");
	if (tigf == nullptr)
	{
		return ERR_FILE_OPEN;
	}

	if (!(arc = archive_write_new()) || archive_write_set_format_zip(arc) != ARCHIVE_OK)
	{
		if (arc)
		{
			archive_write_close(arc);
			archive_write_free(arc);
		}
		fclose(tigf);
		return ERR_FILE_OPEN;
	}

	// tell libarchive not to pad output to 10240-byte blocks (why
	// this is not the default for zip format, I have no idea)
	archive_write_set_bytes_per_block(arc, 0);

	if (content->comp_level > 0)
	{
		archive_write_set_options(arc, "compression=deflate");
	}
	else
	{
		archive_write_set_options(arc, "compression=store");
	}

	if (archive_write_open_FILE(arc, tigf) != ARCHIVE_OK)
	{
		err = ERR_FILE_OPEN;
	}

	// Parse entries and store
	for (ptr = content->var_entries; *ptr && !err; ptr++)
	{
		const TigEntry* entry = *ptr;
		char *fname = nullptr;

		// write TI file into tmp folder
		const int fd = open_temp_file(entry->filename, &fname);
		if (fd == -1)
		{
			g_free(fname);
			err = ERR_FILE_OPEN;
			break;
		}
		close(fd);

		err = tifiles_file_write_regular(fname, entry->content.regular, nullptr);
		if (!err)
		{
			err = zip_write(arc, content->model, entry->filename, fname);
		}

		g_unlink(fname);
		g_free(fname);
	}

	for (ptr = content->app_entries; *ptr && !err; ptr++)
	{
		const TigEntry* entry = *ptr;
		char *fname = nullptr;

		// write TI file into tmp folder
		const int fd = open_temp_file(entry->filename, &fname);
		if (fd == -1)
		{
			g_free(fname);
			err = ERR_FILE_OPEN;
			break;
		}
		close(fd);

		err = tifiles_file_write_flash(fname, entry->content.flash);
		if (!err)
		{
			err = zip_write(arc, content->model, entry->filename, fname);
		}

		g_unlink(fname);
		g_free(fname);
	}

	// close archive
	if (archive_write_close(arc) != ARCHIVE_OK)
	{
		err = ERR_FILE_IO;
	}
	archive_write_free(arc);
	fclose(tigf);
	return err;
}

/**
 * tifiles_file_display_tigroup:
 * @filename: the name of file to load.
 *
 * This function shows file contents (similar to "unzip -l filename").
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
int TICALL tifiles_file_display_tigroup(const char *filename)
{
	struct archive *arc;
	struct archive_entry *entry;

	if (filename == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return -1;
	}

	FILE* tigf = g_fopen(filename, "rb");
	if (tigf == nullptr)
	{
		return ERR_FILE_OPEN;
	}

	if (!(arc = archive_read_new())
	    || archive_read_support_format_zip(arc) != ARCHIVE_OK
	    || archive_read_open_FILE(arc, tigf) != ARCHIVE_OK)
	{
		if (arc)
		{
			archive_read_free(arc);
		}
		fclose(tigf);
		return ERR_FILE_ZIP;
	}

	tifiles_info("TIGroup file contents:");
	tifiles_info(" Size    Name");
	tifiles_info(" ------  ------");

	while (archive_read_next_header(arc, &entry) == ARCHIVE_OK)
	{
		const char *name = archive_entry_pathname(entry);
		char *dispname = g_filename_display_name(name);
		const unsigned long size = (unsigned long) archive_entry_size(entry);
		tifiles_info(" %-7lu %s", size, dispname);
		archive_read_data_skip(arc);
		g_free(dispname);
	}

	archive_read_free(arc);
	fclose(tigf);
	return 0;
}

/**
 * tifiles_file_display_tigcontent:
 * @content: the tigroup content to show, TigContent pointer.
 *
 * Display tigroup information contained in a TigContent structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_file_display_tigcontent(TigContent *content)
{
	unsigned int i;

	if (content == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	tifiles_info("Model:             %02X (%u)", content->model, content->model);
	tifiles_info("Signature:         %s", tifiles_calctype2signature(content->model));
	tifiles_info("model_dst:         %02X (%u)", content->model_dst, content->model_dst);
	tifiles_info("Comment:           %s", content->comment);
	tifiles_info("Compression level: %d", content->comp_level);

	tifiles_info("Number of vars:    %u", content->n_vars);
	tifiles_info("Var entries:       %p", content->var_entries);

	if (content->var_entries != nullptr)
	{
		for (i = 0; i < content->n_vars; i++)
		{
			tifiles_te_display(content->var_entries[i]);
		}
	}

	tifiles_info("Number of apps:    %u", content->n_apps);
	tifiles_info("Apps entries:      %p", content->app_entries);

	if (content->app_entries != nullptr)
	{
		for (i = 0; i < content->n_apps; i++)
		{
			tifiles_te_display(content->app_entries[i]);
		}
	}

	return 0;
}
