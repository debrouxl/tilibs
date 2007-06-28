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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	TiGroup (*.tig) management
	A TiGroup file is in fact a ZIP archive with no compression (stored).

	Please note that I don't use USEWIN32IOAPI!
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBZ
#include "minizip/zip.h"
#include "minizip/unzip.h"
#endif
/*
#ifdef WIN32
# define USEWIN32IOAPI
# include "minizip/iowin32.h"
#endif
*/

#include <ticonv.h>
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "rwfile.h"

#define WRITEBUFFERSIZE (8192)

#ifdef HAVE_LIBZ
extern uLong filetime(char *f, tm_zip *tmzip, uLong *dt);
extern int do_list(unzFile uf);
#endif

#if !GLIB_CHECK_VERSION(2, 8, 0)
#include <errno.h>
// Code taken from Glib
int g_chdir (const gchar *path)
{
#ifdef __WIN32__
  if (G_WIN32_HAVE_WIDECHAR_API ())
    {
      wchar_t *wpath = g_utf8_to_utf16 (path, -1, NULL, NULL, NULL);
      int retval;
      int save_errno;

      if (wpath == NULL)
	{
	  errno = EINVAL;
	  return -1;
	}

      retval = _wchdir (wpath);
      save_errno = errno;

      g_free (wpath);
      
      errno = save_errno;
      return retval;
    }
  else
    {
      gchar *cp_path = g_locale_from_utf8 (path, -1, NULL, NULL, NULL);
      int retval;
      int save_errno;

      if (cp_path == NULL)
	{
	  errno = EINVAL;
	  return -1;
	}

      retval = chdir (cp_path);
      save_errno = errno;

      g_free (cp_path);

      errno = save_errno;
      return retval;
    }
#else
  return chdir (path);
#endif
}
#endif

// ---------------------------------------------------------------------------

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
TIEXPORT2 TigEntry* TICALL tifiles_te_create(const char *filename, FileClass type, CalcModel model)
{
	TigEntry *entry;

	entry = (TigEntry *)g_malloc0(sizeof(TigEntry));

	entry->filename = g_strdup(g_basename(filename));
	entry->type = type;

	if(type == TIFILE_FLASH)
		entry->content.flash = tifiles_content_create_flash(model);
	else if(type & TIFILE_REGULAR)
		entry->content.regular = tifiles_content_create_regular(model);

	return entry;
}

/**
 * tifiles_te_delete:
 * @entry: a #TigEntry structure.
 *
 * Destroy a #TigEntry structure as well as fields.
 *
 * Return value: none.
 **/
TIEXPORT2 int TICALL tifiles_te_delete(TigEntry* entry)
{
	g_free(entry->filename);

	if(entry->type == TIFILE_FLASH)
		tifiles_content_delete_flash(entry->content.flash);
	else if(entry->type & TIFILE_REGULAR)
		tifiles_content_delete_regular(entry->content.regular);

	g_free(entry);
	return 0;
}

/**
 * tifiles_te_create_array:
 * @nelts: size of NULL-terminated array (number of #TigEntry structures).
 *
 * Allocate a NULL-terminated array of #TigEntry structures. You have to allocate
 * each elements of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 TigEntry**	TICALL tifiles_te_create_array(int nelts)
{
	return g_malloc0((nelts + 1) * sizeof(TigEntry *));
}

/**
 * tifiles_te_resize_array:
 * @array: address of array
 * @nelts: size of NULL-terminated array (number of #TigEntry structures).
 *
 * Re-allocate a NULL-terminated array of #TigEntry structures. You have to allocate
 * each elements of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 TigEntry**	TICALL tifiles_te_resize_array(TigEntry** array, int nelts)
{
	return realloc(array, (nelts + 1) * sizeof(TigEntry *));
}

/**
 * tifiles_ve_delete_array:
 * @array: an NULL-terminated array of TigEntry structures.
 *
 * Free the whole array (data buffer, TigEntry structure and array itself).
 *
 * Return value: none.
 **/
TIEXPORT2 void			TICALL tifiles_te_delete_array(TigEntry** array)
{
	TigEntry** ptr;

	assert(array != NULL);

	for(ptr = array; ptr; ptr++)
		tifiles_te_delete(*ptr);
	g_free(array);
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
TIEXPORT2 int TICALL tifiles_te_sizeof_array(TigEntry** array)
{
	int i;
	TigEntry **p;

	for(i = 0, p = array; *p; *p++, i++);

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
TIEXPORT2 int TICALL tifiles_content_add_te(TigContent *content, TigEntry *te)
{
	if(te->type == TIFILE_FLASH)
	{
		int n = content->n_apps;

		content->app_entries = tifiles_te_resize_array(content->app_entries, n + 1);

		content->app_entries[n++] = te;
		content->app_entries[n] = NULL;
		content->n_apps = n;

		return n;
	}
	else if(te->type & TIFILE_REGULAR)
	{
		int n = content->n_vars;

		content->var_entries = tifiles_te_resize_array(content->var_entries, n + 1);

		content->var_entries[n++] = te;
		content->var_entries[n] = NULL;
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
TIEXPORT2 int TICALL tifiles_content_del_te(TigContent *content, TigEntry *te)
{
	int i, j, k;

	// Search for entry
	for(i = 0; i < content->n_vars && (te->type & TIFILE_REGULAR); i++)
	{
		TigEntry *s = content->var_entries[i];

		if(!strcmp(s->filename, te->filename))
			break;
	}

	for(j = 0; j < content->n_apps && (te->type & TIFILE_FLASH); j++)
	{
		TigEntry *s = content->app_entries[i];

		if(!strcmp(s->filename, te->filename))
			break;
	}

	// Not found ? Exit !
	if((i == content->n_vars) && (j == content->n_apps))
		return -1;

	// Release
	if(i < content->n_vars)
	{
		// Delete
		tifiles_te_delete(content->var_entries[i]);

		// And shift
		for(k = i; k < content->n_vars; k++)
			content->var_entries[k] = content->var_entries[k+1];
		content->var_entries[k] = NULL;

		// And resize
		content->var_entries = tifiles_te_resize_array(content->var_entries, content->n_vars - 1);
		content->n_vars--;

		return content->n_vars;
	}

	if(j < content->n_apps)
	{
		// Delete
		tifiles_te_delete(content->app_entries[j]);

		// And shift
		for(k = j; k < content->n_apps; k++)
			content->app_entries[k] = content->app_entries[k+1];
		content->app_entries[k] = NULL;

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
TIEXPORT2 int TICALL tifiles_tigroup_add_file(const char *src_filename, const char *dst_filename)
{
	CalcModel model;
	FileClass type;
	TigEntry *te;
	TigContent *content = NULL;
	int ret = 0;

	// group file is created if non existent
	if(!stricmp(tifiles_fext_get(dst_filename), "tig"))
	{
		if(!g_file_test(dst_filename, G_FILE_TEST_EXISTS))
		{
			content = tifiles_content_create_tigroup(CALC_NONE, 0);
			tifiles_file_write_tigroup(dst_filename, content);
			tifiles_content_delete_tigroup(content);
		}
	}

	// src can't be a TiGroup file but dst should be
	if(!(tifiles_file_is_ti(src_filename) && !tifiles_file_is_tigroup(src_filename) &&
		tifiles_file_is_tigroup(dst_filename)))
		return -1;

	// load src file
	model = tifiles_file_get_model(src_filename);
	type = tifiles_file_get_class(src_filename);
	
	te = tifiles_te_create(src_filename, type, model);
	if(type == TIFILE_FLASH)
	{ 
		ret = tifiles_file_read_flash(src_filename, te->content.flash);
		if(ret) goto ttaf;
	}
	else if(type & TIFILE_REGULAR)
	{ 
		ret = tifiles_file_read_regular(src_filename, te->content.regular);
		if(ret) goto ttaf;
	}

	// load dst file
	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	ret = tifiles_file_read_tigroup(dst_filename, content);
	if(ret) goto ttaf;

	tifiles_content_add_te(content, te);
	ret = tifiles_file_write_tigroup(dst_filename, content);
	if(ret) goto ttaf;

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
TIEXPORT2 int TICALL tifiles_tigroup_del_file(TigEntry *entry,          const char *filename)
{
	TigContent* content = NULL;
	int err = 0;

	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	err = tifiles_file_read_tigroup(filename, content);
	if(err) goto tfdf;

	tifiles_content_del_te(content, entry);
	err = tifiles_file_write_tigroup(filename, content);
	if(err) goto tfdf;

tfdf:
	tifiles_content_delete_tigroup(content);
	return 0;
}

// ---------------------------------------------------------------------------

TIEXPORT2 FileContent* TICALL tifiles_content_dup_regular(FileContent *content);
TIEXPORT2 FlashContent* TICALL tifiles_content_dup_flash(FlashContent *content);

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
TIEXPORT2 int TICALL tifiles_tigroup_contents(FileContent **src_contents1, FlashContent **src_contents2, TigContent **dst_content)
{
	TigContent *content;
	int i, m=0, n=0;
	CalcModel model = CALC_NONE;

	if(src_contents1 == NULL && src_contents2 == NULL)
		return -1;

	if(src_contents1)
		for (m = 0; src_contents1[m] != NULL; m++);
	if(src_contents2)
		for (n = 0; src_contents2[n] != NULL; n++);

	if(src_contents2)
		if(*src_contents2)
			model = src_contents2[0]->model;
	if(src_contents1)
		if(*src_contents1)
			model = src_contents1[0]->model;	// FileContent is more precise than FlashContent

	content = tifiles_content_create_tigroup(model, m+n);

	if(src_contents1)
	{
		for(i = 0; i < m; i++)
		{
			TigEntry *te = (TigEntry *)g_malloc0(sizeof(TigEntry));

			te->filename = tifiles_build_filename(model, src_contents1[i]->entries[0]);
			te->type = TIFILE_GROUP;
			te->content.regular = tifiles_content_dup_regular(src_contents1[i]);
			tifiles_content_add_te(content, te);
		}
	}

	if(src_contents2)
	{
		for(i = 0; i < n; i++)
		{
			TigEntry *te = (TigEntry *)g_malloc0(sizeof(TigEntry));
			VarEntry ve;
			FlashContent *ptr;

			for (ptr = src_contents2[i]; ptr; ptr = ptr->next)
				if(ptr->data_type == tifiles_flash_type(model))
					break;
			
			strcpy(ve.folder, "");
			strcpy(ve.name, ptr->name);
			ve.type = ptr->data_type;
			te->filename = tifiles_build_filename(model, &ve);
			te->type = TIFILE_FLASH;
			te->content.flash = tifiles_content_dup_flash(src_contents2[i]);
			tifiles_content_add_te(content, te);
		}
	}

	*dst_content = content;

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
TIEXPORT2 int TICALL tifiles_untigroup_content(TigContent *src_content, FileContent ***dst_contents1, FlashContent ***dst_contents2)
{
	TigContent *src = src_content;
	FileContent **dst1 = NULL;
	FlashContent **dst2 = NULL;
	int i, j;

	// allocate an array of FileContent/FlashContent structures (NULL terminated)
	{
		dst1 = (FileContent **)g_malloc0((src->n_vars+1) * sizeof(FileContent *));
		if (dst1 == NULL)
			return ERR_MALLOC;
	}
	{
		dst2 = (FlashContent **)g_malloc0((src->n_apps+1) * sizeof(FlashContent *));
		if (dst2 == NULL)
			return ERR_MALLOC;
	}

	// parse each entry and duplicate it into a single content
	for(i = 0; i < src->n_vars; i++)
	{
		TigEntry *te = src->var_entries[i];

		dst1[i] = tifiles_content_dup_regular(te->content.regular);
	}

	for(j = 0; j < src->n_apps; j++)
	{
		TigEntry *te = src->app_entries[j];

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
TIEXPORT2 int TICALL tifiles_tigroup_files(char **src_filenames, const char *dst_filename)
{
	FileContent **src1 = NULL;
	FlashContent **src2 = NULL;
	TigContent *dst = NULL;
	CalcModel model;
	int i, j, k, m, n;
	int ret = 0;

	// counts number of files to group and allocate space for that
	for(k = m = n = 0; src_filenames[k]; k++)
	{
		if(tifiles_file_is_regular(src_filenames[k]))
			m++;
		else if(tifiles_file_is_flash(src_filenames[k]))
			n++;
	}
	model = tifiles_file_get_model(src_filenames[0]);

	// allocate space for that
	src1 = (FileContent **)g_malloc0((m + 1) * sizeof(FileContent *));
	if (src1 == NULL)
		return ERR_MALLOC;

	src2 = (FlashContent **)g_malloc0((n + 1) * sizeof(FlashContent *));
	if (src2 == NULL)
		return ERR_MALLOC;

	for(i = j = k = 0; k < m+n; k++)
	{
		if(tifiles_file_is_regular(src_filenames[k]))
		{
			src1[i] = tifiles_content_create_regular(model);
			ret = tifiles_file_read_regular(src_filenames[k], src1[i]);
			if(ret) goto tgf;
			i++;
		}
		else if(tifiles_file_is_flash(src_filenames[k]))
		{
			src2[j] = tifiles_content_create_flash(model);
			ret = tifiles_file_read_flash(src_filenames[k], src2[j]);
			if(ret) goto tgf;
			j++;
		}
	}

	ret = tifiles_tigroup_contents(src1, src2, &dst);
	if(ret) goto tgf;

	ret = tifiles_file_write_tigroup(dst_filename, dst);
	if(ret) goto tgf;

tgf:
	for(i = 0; i < m; i++)
		g_free(src1[i]);
	g_free(src1);
	for(i = 0; i < n; i++)
		g_free(src2[i]);
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
TIEXPORT2 int TICALL tifiles_untigroup_file(const char *src_filename, char ***dst_filenames)
{
	TigContent *src = NULL;
	FileContent **ptr1, **dst1 = NULL;
	FlashContent **ptr2, **dst2 = NULL;
	char *real_name;
	int i, j;
	int ret = 0;

	// read TiGroup file
	src = tifiles_content_create_tigroup(CALC_NONE, 0);
	ret = tifiles_file_read_tigroup(src_filename, src);
	if(ret) goto tuf;

	// ungroup structure
	ret = tifiles_untigroup_content(src, &dst1, &dst2);
	if(ret) goto tuf;

	// count number of structures and allocates array of strings
	if(dst_filenames != NULL)
		*dst_filenames = (char **)g_malloc((src->n_vars + src->n_apps + 1) * sizeof(char *));

	// store each structure content to file
	for (ptr1 = dst1, i = 0; *ptr1 != NULL || i < src->n_vars; ptr1++, i++)
	{
		ret = tifiles_file_write_regular(NULL, *ptr1, &real_name);
		if(ret) goto tuf;

		if(dst_filenames != NULL)
			*dst_filenames[i] = real_name;
		else
			g_free(real_name);
	}

	for (ptr2 = dst2, j = 0; *ptr2 != NULL || j < src->n_apps; ptr2++, j++)
	{
		ret = tifiles_file_write_flash2(NULL, *ptr2, &real_name);
		if(ret) goto tuf;

		if(dst_filenames != NULL)
			*dst_filenames[i+j] = real_name;
		else
			g_free(real_name);
	}

	// release allocated memory
tuf:
	if(dst1)
	{
		for(ptr1 = dst1; *ptr1; ptr1++)
			tifiles_content_delete_regular(*ptr1);
	}
	if(dst2)
	{
		for(ptr2 = dst2; *ptr2; ptr2++)
			tifiles_content_delete_flash(*ptr2);
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
TIEXPORT2 TigContent* TICALL tifiles_content_create_tigroup(CalcModel model, int n)
{
	TigContent* content = g_malloc0(sizeof(FileContent));

	content->model = content->model_dst = model;
	content->comment = g_strdup(tifiles_comment_set_tigroup());
	content->comp_level = 0;
	content->var_entries = (TigEntry **)g_malloc0(sizeof(TigEntry *));
	content->app_entries = (TigEntry **)g_malloc0(sizeof(TigEntry *));

	return content;
}

/**
 * tifiles_content_delete_tigroup:
 *
 * Free the whole content of a @TigContent structure and the content itself.
 *
 * Return value: none.
 **/
TIEXPORT2 int TICALL tifiles_content_delete_tigroup(TigContent *content)
{
	int i;

	for(i = 0; i < content->n_vars; i++)
	{
		TigEntry* entry = content->var_entries[i];
		tifiles_te_delete(entry);
	}

	for(i = 0; i < content->n_apps; i++)
	{
		TigEntry* entry = content->app_entries[i];
		tifiles_te_delete(entry);
	}
	g_free(content);

	return 0;
}

/**
 * tifiles_file_read_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content (may be re-allocated).
 *
 * This function load & TiGroup and place its content into content.
 *
 * The temporary folder is used by this function to store temporary files.
 * 
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT2 int TICALL tifiles_file_read_tigroup(const char *filename, TigContent *content)
{
#ifdef HAVE_LIBZ
	unzFile uf = NULL;
	unz_global_info gi;
	unz_file_info file_info;
	int cnt, err = UNZ_OK;
	char filename_inzip[256];
	unsigned i;
	void* buf = NULL;
	const char *password = NULL;
	int vi = 0, ai = 0;

	// Open ZIP archive
	uf = unzOpen(filename);
	if (uf == NULL)
	{
		printf("Can't open this file: %s\n", filename);
		return ERR_FILE_ZIP;
	}

	// Allocate
	buf = (void*)malloc(WRITEBUFFERSIZE);
    if (buf==NULL)
    {
        printf("Error allocating memory\n");
		goto tfrt_exit;
    }
	
	// Size of comment and number of files in archive
	err = unzGetGlobalInfo (uf,&gi);
    if (err!=UNZ_OK)
	{
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);
		goto tfrt_exit;
	}        
	//printf("# entries: %lu\n", gi.number_entry);

	g_free(content->var_entries);
	content->var_entries = (TigEntry **)g_malloc0((gi.number_entry + 1) * sizeof(TigEntry *));
	content->n_vars = 0;

	g_free(content->app_entries);
	content->app_entries = (TigEntry **)g_malloc0((gi.number_entry + 1) * sizeof(TigEntry *));
	content->n_apps = 0;

	// Get comment
	g_free(content->comment);
	content->comment = (char *)g_malloc((gi.size_comment+1) * sizeof(char));
	err = unzGetGlobalComment(uf, content->comment, gi.size_comment);

	// Parse archive for files
	for (i = 0; i < gi.number_entry; i++)
    {
		FILE *f;
		gchar *fname;
		gchar *utf8;
		gchar *gfe;

		// get infos
		err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			goto tfrt_exit;
		}
		//printf("Extracting %s with %lu bytes\n", filename_inzip, file_info.uncompressed_size);

		err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
            printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
			goto tfrt_exit;
        }

		// extract/uncompress into temporary file
		utf8 = g_locale_to_utf8(filename_inzip, -1, NULL, NULL, NULL);
		gfe = g_filename_from_utf8(utf8, -1, NULL, NULL, NULL);		
		fname = g_strconcat(g_get_tmp_dir(), G_DIR_SEPARATOR_S, gfe, NULL);
		g_free(utf8);
		g_free(gfe);

		f = gfopen(fname, "wb");
		if(f == NULL)
		{
			err = ERR_FILE_OPEN;
			goto tfrt_exit;
		}

		do
        {
            err = unzReadCurrentFile(uf,buf,WRITEBUFFERSIZE);
            if (err<0)
            {
                printf("error %d with zipfile in unzReadCurrentFile\n",err);
				fclose(f);
                goto tfrt_exit;
            }
            if (err>0)
			{
				cnt = fwrite(buf, 1, err, f);
				if(cnt == -1)
                {
                    printf("error in writing extracted file\n");
                    err=UNZ_ERRNO;
					fclose(f);
                    goto tfrt_exit;
                }
            }
        }
		while (err>0);
		fclose(f);

		// add to TigContent
		{
			int model = tifiles_file_get_model(fname);

			if(content->model == CALC_NONE)
				content->model = model;

			if(tifiles_file_is_regular(fname))
			{
				TigEntry *entry = tifiles_te_create(filename_inzip, tifiles_file_get_class(fname), content->model);
				int ret;

				ret = tifiles_file_read_regular(fname, entry->content.regular);
				if(ret) { g_free(entry); unlink(fname); g_free(fname); err = ret; goto tfrt_exit; }

				content->var_entries[vi++] = entry;
				content->n_vars++;
			}
			else if(tifiles_file_is_flash(fname))
			{
				TigEntry *entry = tifiles_te_create(filename_inzip, tifiles_file_get_class(fname), content->model);
				int ret;

				ret = tifiles_file_read_flash(fname, entry->content.flash);
				if(ret) { g_free(entry); unlink(fname); g_free(fname); err = ret; goto tfrt_exit; }

				content->app_entries[ai++] = entry;
				content->n_apps++;
			}
			else
			{
				// skip
			}
		}
		unlink(fname);
		g_free(fname);

		// next file
		if ((i+1) < gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				goto tfrt_exit;
			}
		}
    }	

	// Close
tfrt_exit:
	g_free(buf);
	unzCloseCurrentFile(uf);
	return err ? ERR_FILE_ZIP : 0;
#else
	return ERR_UNSUPPORTED;
#endif
}

#ifdef HAVE_LIBZ
static int zip_write(zipFile *zf, const char *fname, int comp_level)
{
		int err = ZIP_OK;
		FILE *f = NULL;
		zip_fileinfo zi;
		char filenameinzip[256];
		unsigned long crcFile=0;
		int size_read;
		void* buf=NULL;

		// missing tmp file !
		f = gfopen(fname, "rb");
		if(f == NULL)
		{
		  printf("error in opening tmp file %s\n", fname);
			err = ERR_FILE_OPEN;
			goto zw_exit;
		}
		strcpy(filenameinzip, fname);

		// Allocate buffer
		buf = (void*)g_malloc(WRITEBUFFERSIZE);
		if (buf==NULL)
		{
			printf("Error allocating memory\n");
			goto zw_exit;
		}

		// update time stamp (to do ?)
		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
        filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

		err = zipOpenNewFileInZip3(*zf,filenameinzip,&zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 comp_level ? Z_DEFLATED : 0 /* comp method */,
                                 1 /*comp level */,0,
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 NULL /* no pwd*/,crcFile);
        if (err != ZIP_OK)
		{
            printf("error in opening %s in zipfile\n",filenameinzip);
			return ERR_FILE_ZIP;
		}		

		do
        {
			// feed with our data
            err = ZIP_OK;
			size_read = fread(buf, 1, WRITEBUFFERSIZE, f);

            if (size_read < WRITEBUFFERSIZE)
			{
				if (!feof(f))
				{
					printf("error in reading %s\n",filenameinzip);
					err = ZIP_ERRNO;
					goto zw_exit;
				}
			}
            if (size_read > 0)
            {
                err = zipWriteInFileInZip (*zf,buf,size_read);
                if (err<0)
                {
                    printf("error in writing %s in the zipfile\n", filenameinzip);
					goto zw_exit;
                }

            }
        } while ((err == ZIP_OK) && (size_read>0));

		// close file
		err = zipCloseFileInZip(*zf);
        if (err!=ZIP_OK)
		{
            printf("error in closing %s in the zipfile\n", filenameinzip);
			goto zw_exit;
		}

zw_exit:
		g_free(buf);
		fclose(f);
		return err;
}
#endif

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
TIEXPORT2 int TICALL tifiles_file_write_tigroup(const char *filename, TigContent *content)
{
#ifdef HAVE_LIBZ
	zipFile zf;
	int err = ZIP_OK;
	gchar *old_dir = g_get_current_dir();
	TigEntry **ptr;

	// Open ZIP archive (and set comment)
#ifdef USEWIN32IOAPI
        zlib_filefunc_def ffunc;
        fill_win32_filefunc(&ffunc);

        zf = zipOpen2(filename, APPEND_STATUS_CREATE, &(content->comment), &ffunc);
#else
        zf = zipOpen(filename, APPEND_STATUS_CREATE);
#endif
	if (zf == NULL)
	{
		printf("Can't open this file: %s\n", filename);
		return ERR_FILE_ZIP;
	}
	g_chdir(g_get_tmp_dir());

	// Parse entries and store
	for(ptr = content->var_entries; *ptr; ptr++)
	{
		TigEntry* entry = *ptr;
		char *fname = NULL;

		// ZIP archives don't like greek chars
		fname = ticonv_gfe_to_zfe(content->model, entry->filename);

		// write TI file into tmp folder
		TRYC(tifiles_file_write_regular(fname, entry->content.regular, NULL));

		err = zip_write(&zf, fname, content->comp_level);
		g_free(fname);

		if(err)
			break;
	}

	for(ptr = content->app_entries; *ptr; ptr++)
	{
		TigEntry* entry = *ptr;
		char *fname = NULL;

		// ZIP archives don't like greek chars
		fname = ticonv_gfe_to_zfe(content->model, entry->filename);

		// write TI file into tmp folder
		TRYC(tifiles_file_write_flash(fname, entry->content.flash));

		err = zip_write(&zf, fname, content->comp_level);
		g_free(fname);

		if(err)
			break;
	}

	// close archive
	err = zipClose(zf,NULL);
    if (err != ZIP_OK)
    {
        printf("error in closing %s\n",filename);
		unlink(filename);
    }
	
	g_chdir(old_dir);
	return err;
#else
	return ERR_UNSUPPORTED;
#endif
}

/**
 * tifiles_file_display_tigroup:
 * @filename: the name of file to load.
 *
 * This function shows file conten ( = "unzip -l filename.tig").
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT2 int TICALL tifiles_file_display_tigroup(const char *filename)
{
#ifdef HAVE_LIBZ
	unzFile uf = NULL;

	uf = unzOpen(filename);
	if (uf==NULL)
    {
		tifiles_warning("Can't open this file: %s", filename);
		return -1;
	}

	do_list(uf);
	unzCloseCurrentFile(uf);

	return 0;
#else
	return ERR_UNSUPPORTED;
#endif
}
