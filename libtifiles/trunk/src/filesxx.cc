/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2006  Romain Lievin
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
  This unit contains a TI file independent API
*/

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tifiles.h"
#include "error.h"
#include "files8x.h"
#include "files9x.h"
#include "filesnsp.h"
#include "logging.h"

// Whether to print detailed information about FileContent, BackupContent, FlashContent instances throughout their lifecycle.
//#define TRACE_CONTENT_INSTANCES

/**
 * tifiles_content_create_regular:
 * @model: a calculator model (required).
 *
 * Allocates a #FileContent structure.
 *
 * Return value: the allocated block.
 **/
FileContent* TICALL tifiles_content_create_regular(CalcModel model)
{
	FileContent* content = (FileContent *)g_malloc0(sizeof(FileContent));

	if (content != NULL)
	{
		if ((unsigned int)model >= CALC_MAX)
		{
			tifiles_warning("Invalid calculator model");
		}
		content->model = content->model_dst = model;
		tifiles_comment_set_single_sn(content->comment, sizeof(content->comment));
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_create_regular: %p", content);
	tifiles_file_display_regular(content);
#endif

	return content;
}

/**
 * tifiles_content_delete_regular:
 *
 * Free the whole content of a #FileContent structure.
 *
 * Return value: 0.
 **/
int TICALL tifiles_content_delete_regular(FileContent *content)
{
	unsigned int i;

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_delete_regular: %p", content);
	tifiles_file_display_regular(content);
#endif

	if (content != NULL)
	{
		for (i = 0; i < content->num_entries; i++) 
		{
			VarEntry *entry = content->entries[i];

			if (entry != NULL)
			{
				g_free(entry->data);
				g_free(entry);
			}
			else
			{
				tifiles_critical("tifiles_content_delete_regular(content with NULL entry)");
			}
		}

		g_free(content->entries);
		g_free(content);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_content_dup_regular:
 *
 * Allocates and copies a new #FileContent structure.
 *
 * Return value: none.
 **/
FileContent* TICALL tifiles_content_dup_regular(FileContent *content)
{
	FileContent *dup = NULL;
	unsigned int i;

	if (content != NULL)
	{
		dup = tifiles_content_create_regular(content->model);
		if (dup != NULL)
		{
			memcpy(dup, content, sizeof(FileContent));
			dup->entries = tifiles_ve_create_array(content->num_entries);

			if (dup->entries != NULL)
			{
				for (i = 0; i < content->num_entries; i++)
				{
					dup->entries[i] = tifiles_ve_dup(content->entries[i]);
					if (dup->entries[i] == NULL)
					{
						tifiles_content_delete_regular(dup);
						dup = NULL;
						break;
					}
				}
			}
			else
			{
				dup->num_entries = 0;
				tifiles_content_delete_regular(dup);
				dup = NULL;
			}
		}
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_dup_regular: %p", dup);
	tifiles_file_display_regular(dup);
#endif

	return dup;
}

/**
 * tifiles_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used. If an error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_read_regular(const char *filename, FileContent *content)
{
	if (filename == NULL || content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
	{
		return ti8x_file_read_regular(filename, (Ti8xRegular *)content);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
	{
		return ti9x_file_read_regular(filename, (Ti9xRegular *)content);
	}
	else
#endif
	if (ticonv_model_is_tinspire(content->model))
	{
		return tnsp_file_read_regular(filename, (FileContent *)content);
	}
	else
	{
		return ERR_BAD_CALC;
	}
}

/**
 * tifiles_file_write_regular:
 * @filename: name of single/group file where to write or NULL.
 * @content: the file content to write.
 * @real_fname: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write one (or several) variable(s) into a single (group) file. If filename is set to NULL,
 * the function build a filename from varname and allocates resulting filename in %real_fname.
 * %filename and %real_filename can be NULL but not both !
 *
 * %real_filename must be freed when no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_write_regular(const char *filename, FileContent *content, char **real_fname)
{
	if (content == NULL || (filename == NULL && real_fname == NULL))
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		return ti8x_file_write_regular(filename, (Ti8xRegular *)content, real_fname);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		return ti9x_file_write_regular(filename, (Ti9xRegular *)content, real_fname);
	}
	else
#endif
	if (ticonv_model_is_tinspire(content->model))
	{
		return tnsp_file_write_regular(filename, (FileContent *)content, real_fname);
	}
	else
	{
		return ERR_BAD_CALC;
	}
}

/**
 * tifiles_file_display_regular:
 * @content: the file content to show, a FileContent pointer.
 *
 * Display file content information contained in a FileContent structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_file_display_regular(FileContent *content)
{
	unsigned int i;
	char trans[257];
	int model_supports_folder = 1;

	if (content == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		tifiles_info("FileContent for TI-8x: %p", content);
		model_supports_folder = 0;
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		tifiles_info("FileContent for TI-9x: %p", content);
	}
	else
#endif
	if (ticonv_model_is_tinspire(content->model))
	{
		tifiles_info("FileContent for TI-Nspire: %p", content);
	}
	else
	{
		return ERR_BAD_CALC;
	}

	tifiles_info("Model:             %02X (%u)", content->model, content->model);
	tifiles_info("Signature:         %s", tifiles_calctype2signature(content->model));
	tifiles_info("Comment:           %s", content->comment);
	if (model_supports_folder)
	{
		tifiles_info("Default folder:    %s", content->default_folder);
	}
	tifiles_info("Number of entries: %u", content->num_entries);
	tifiles_info("Entries:           %p", content->entries);

	if (content->entries != NULL)
	{
		for (i = 0; i < content->num_entries; i++)
		{
			VarEntry * ve = content->entries[i];
			if (ve != NULL)
			{
				tifiles_info("Entry #%u %p", i, ve);
				if (model_supports_folder)
				{
					tifiles_info("  folder:      %s", ve->folder);
				}
				tifiles_info("  name:        %s", ticonv_varname_to_utf8_sn(content->model, ve->name, trans, sizeof(trans), ve->type));
				tifiles_info("  type:        %02X (%s)", ve->type, tifiles_vartype2string(content->model, ve->type));
				tifiles_info("  attr:        %s", tifiles_attribute_to_string((FileAttr)ve->attr));
				tifiles_info("  version:     %02X (%u)", ve->version, ve->version);
				tifiles_info("  length:      %04X (%u)", ve->size, ve->size);
				tifiles_info("  data:        %p", ve->data);
			}
			else
			{
				tifiles_critical("%s: an entry in content is NULL", __FUNCTION__);
			}
		}
	}

	tifiles_info("Checksum:      %04X (%u)", content->checksum, content->checksum);
	tifiles_info("Dest model:    %02X (%u)", content->model_dst, content->model_dst);

	return 0;
}

/**
 * tifiles_content_create_backup:
 * @model: a calculator model or CALC_NONE.
 *
 * Allocates a BackupContent structure.
 *
 * Return value: the allocated block.
 **/
BackupContent* TICALL tifiles_content_create_backup(CalcModel model)
{
	BackupContent* content = (BackupContent *)g_malloc0(sizeof(BackupContent));

	if (content != NULL)
	{
		if ((unsigned int)model >= CALC_MAX)
		{
			tifiles_warning("Invalid calculator model");
		}
		content->model = model;
		tifiles_comment_set_backup_sn(content->comment, sizeof(content->comment));
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_create_backup: %p", content);
	tifiles_file_display_backup(content);
#endif

	return content;
}

/**
 * tifiles_content_delete_backup:
 *
 * Free the whole content of a BackupContent structure.
 *
 * Return value: none.
 **/
int TICALL tifiles_content_delete_backup(BackupContent *content)
{
#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_delete_backup: %p", content);
	tifiles_file_display_backup(content);
#endif

	if (content != NULL)
	{
		if (tifiles_calc_is_ti9x(content->model))
		{
			g_free(content->data_part);
		}
		else if (tifiles_calc_is_ti8x(content->model))
		{
			g_free(content->data_part1);
			g_free(content->data_part2);
			g_free(content->data_part3);
			g_free(content->data_part4);
		}

		g_free(content);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_file_read_backup:
 * @filename: name of backup file to open.
 * @content: where to store the file content.
 *
 * Load the backup file into a BackupContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_backup when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_read_backup(const char *filename, BackupContent *content)
{
	if (filename == NULL || content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
	{
		return ti8x_file_read_backup(filename, content);
	}
	else
#endif 
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
	{
		return ti9x_file_read_backup(filename, content);
	}
	else
#endif
	return ERR_BAD_CALC;
}

/**
 * tifiles_file_write_backup:
 * @filename: name of backup file where to write.
 * @content: the file content to write.
 *
 * Write backup into file.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_write_backup(const char *filename, BackupContent *content)
{
	if (filename == NULL || content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		return ti8x_file_write_backup(filename, content);
	}
	else
#endif 
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		return ti9x_file_write_backup(filename, content);
	}
	else
#endif
	return ERR_BAD_CALC;
}

/**
 * tifiles_file_display_backup:
 * @content: the file content to show.
 *
 * Display file content information.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_file_display_backup(BackupContent *content)
{
	if (content == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		return ti8x_content_display_backup(content);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		return ti9x_content_display_backup(content);
	}
	else
#endif
	return ERR_BAD_CALC;
}

/**
 * tifiles_content_create_flash:
 * @model: a calculator model (compulsory).
 *
 * Allocates a #FlashContent structure.
 *
 * Return value: the allocated block.
 **/
FlashContent* TICALL tifiles_content_create_flash(CalcModel model)
{
	FlashContent* content = (FlashContent *)g_malloc0(sizeof(FlashContent));

	if (content != NULL)
	{
		if ((unsigned int)model >= CALC_MAX)
		{
			tifiles_warning("Invalid calculator model");
		}
		content->model = content->model_dst = model;
		if (tifiles_calc_is_ti9x(content->model))
		{
			time_t tt;
			struct tm lt;

			time(&tt);
#ifdef HAVE_LOCALTIME_R
			localtime_r(&tt, &lt);
#else
			memcpy(&lt, localtime(&tt), sizeof(struct tm));
#endif
			content->revision_major = 1;
			content->revision_minor = 0;
			content->flags = 0;
			content->object_type = 0;
			content->revision_day = lt.tm_mday;
			content->revision_month = lt.tm_mon;
			content->revision_year = lt.tm_year + 1900;
		}
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_create_flash: %p", content);
	tifiles_file_display_flash(content);
#endif

	return content;
}

/**
 * tifiles_content_delete_flash:
 *
 * Free the whole content of a #FlashContent structure.
 *
 * Return value: always 0.
 **/
int TICALL tifiles_content_delete_flash(FlashContent *content)
{
#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_delete_flash: %p", content);
	tifiles_file_display_flash(content);
#endif

	if (content != NULL)
	{
#if !defined(DISABLE_TI8X) && !defined(DISABLE_TI9X)
		FlashContent *ptr;
		unsigned int i;

		g_free(content->data_part);

		ptr = content->next;
		while (ptr != NULL)
		{
			FlashContent *next = ptr->next;

			g_free(ptr->data_part);
			g_free(ptr);

			for (i = 0; i < content->num_pages; i++)
			{
				g_free(content->pages[i]->data);
				g_free(content->pages[i]);
			}
			g_free(content->pages);

			ptr = next;
		}

		g_free(content);
#else
		return ERR_BAD_CALC;
#endif
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_content_dup_flash:
 *
 * Allocates and copies a new FlashContent structure.
 *
 * Return value: none.
 **/
FlashContent* TICALL tifiles_content_dup_flash(FlashContent *content)
{
	FlashContent *dup = NULL;
	FlashContent *p, *q;

	if (content != NULL)
	{
		dup = tifiles_content_create_flash(content->model);
		if (dup != NULL)
		{
			for (p = content, q = dup; p; p = p->next, q = q->next)
			{
				memcpy(q, p, sizeof(FlashContent));

				// TI-68k or TI-eZ80 part
				if (tifiles_calc_is_ti9x(content->model) || ticonv_model_is_tiez80(content->model))
				{
					if (p->data_part)
					{
						q->data_part = (uint8_t *)g_malloc0(p->data_length+1);
						memcpy(q->data_part, p->data_part, p->data_length+1);
					}
				}

				// TI-Z80 part
				if (tifiles_calc_is_ti8x(content->model))
				{
					unsigned int i;

					// copy pages
					q->pages = tifiles_fp_create_array(p->num_pages);
					for (i = 0; i < content->num_pages; i++)
					{
						q->pages[i] = (FlashPage *)g_malloc0(sizeof(FlashPage));
						memcpy(q->pages[i], p->pages[i], sizeof(FlashPage));

						q->pages[i]->data = (uint8_t *) g_malloc0(p->pages[i]->size);
						memcpy(q->pages[i]->data, p->pages[i]->data, p->pages[i]->size);
					}
				}

				if (p->next)
				{
					q->next = tifiles_content_create_flash(p->model);
				}
			}
		}
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

#ifdef TRACE_CONTENT_INSTANCES
	tifiles_info("tifiles_content_dup_flash: %p", dup);
	tifiles_file_display_flash(dup);
#endif

	return dup;
}

/**
 * tifiles_file_read_flash:
 * @filename: name of FLASH file to open.
 * @content: where to store the file content.
 *
 * Load the FLASH file into a FlashContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_flash when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_read_flash(const char *filename, FlashContent *content)
{
	if (filename == NULL || content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
	{
		return ti8x_file_read_flash(filename, content);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)) || tifiles_file_is_tib(filename))
	{
		return ti9x_file_read_flash(filename, content);
	}
	else
#endif
	if (ticonv_model_is_tinspire(content->model))
	{
		return tnsp_file_read_flash(filename, content);
	}
	else
	{
		return ERR_BAD_CALC;
	}
}

/**
 * tifiles_file_write_flash2:
 * @filename: name of flash file where to write or NULL.
 * @content: the file content to write.
 * @real_fname: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write a FLASH content to a file. If filename is set to NULL, the function build a filename 
 * from appname and allocates resulting filename in %real_fname.
 * %filename and %real_fname can be NULL but not both !
 *
 * %real_fname must be freed when no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_write_flash2(const char *filename, FlashContent *content, char **real_fname)
{
	if (content == NULL || (filename == NULL && real_fname == NULL))
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		return ti8x_file_write_flash(filename, content, real_fname);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		return ti9x_file_write_flash(filename, content, real_fname);
	}
	else
#endif
	return ERR_BAD_CALC;
}

/**
 * tifiles_file_write_flash:
 * @filename: name of flash file where to write or NULL.
 * @content: the file content to write.
 *
 * Write a FLASH content to a file. If filename is set to NULL, the function build a filename 
 * from appname.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tifiles_file_write_flash(const char *filename, FlashContent *content)
{
	return tifiles_file_write_flash2(filename, content, NULL);
}

/**
 * tifiles_file_display_flash:
 * @content: the file content to show.
 *
 * Display file content information.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_file_display_flash(FlashContent *content)
{
	if (content == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(content->model))
	{
		return ti8x_content_display_flash(content);
	}
	else 
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(content->model))
	{
		return ti9x_content_display_flash(content);
	}
	else
#endif
	return ERR_BAD_CALC;
}

/**
 * tifiles_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
int TICALL tifiles_file_display(const char *filename)
{
	if (tifiles_file_is_tigroup(filename))
	{
		return tifiles_file_display_tigroup(filename);
	}
#if !defined(DISABLE_TI8X)
	if (tifiles_calc_is_ti8x(tifiles_file_get_model(filename)))
	{
		return ti8x_file_display(filename);
	}
	else
#endif
#if !defined(DISABLE_TI9X)
	if (tifiles_calc_is_ti9x(tifiles_file_get_model(filename)))
	{
		return ti9x_file_display(filename);
	}
	else
#endif
	return ERR_BAD_CALC;
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
 * Return value: a 2-dimensions allocated integer array. Must be freed with tifiles_free_table_of_entries when
 * no longer used.
 **/
int** tifiles_create_table_of_entries(FileContent *content, unsigned int *nfolders)
{
	unsigned int num_folders = 0;
	unsigned int i, j;
	char **ptr, *folder_list[32768] = { 0 };
	int **table;

	if (content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return NULL;
	}

	if (content->entries != NULL)
	{
		// determine how many folders we have
		for (i = 0; i < content->num_entries; i++)
		{
			VarEntry *entry = content->entries[i];

			// scan for an existing folder entry
			for (ptr = folder_list; *ptr != NULL; ptr++)
			{
				if (!strcmp(*ptr, entry->folder))
				{
					//printf("break: %s\n", entry->folder);
					break;
				}
			}
			if (*ptr == NULL)
			{		// add new folder entry
				folder_list[num_folders] = (char *) g_malloc0(257);
				//printf("%i: adding '%s'\n", num_folders, entry->folder);
				strncpy(folder_list[num_folders], entry->folder, 257 - 1);
				folder_list[num_folders][257 - 1] = 0;
				folder_list[num_folders + 1] = NULL;
				num_folders++;
			}
		}
		if (tifiles_calc_is_ti8x(content->model))
		{
			num_folders++;
		}
	}
	if (nfolders != NULL)
	{
		*nfolders = num_folders;
	}

	// allocate the folder list
	table = (int **) g_malloc0((num_folders + 1) * sizeof(int *));
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
				table[j] = (int *) g_realloc(table[j], (k + 2) * sizeof(int));
				table[j][k] = i;
				//printf("%i %i: adding %i\n", j, k, i); 
				table[j][k + 1] = -1;
				k++;
			}
		}
	}

	// g_free( memory
	for (j = 0; j < num_folders + 1; j++)
	{
		g_free(folder_list[j]);
	}

	return table;
}

void tifiles_free_table_of_entries(int ** table)
{
	if (table != NULL)
	{
		int ** ptr = table;
		while (*ptr != NULL)
		{
			g_free(*ptr);
			ptr++;
		}
		g_free(table);
	}
}
