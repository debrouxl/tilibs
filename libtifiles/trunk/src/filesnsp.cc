/* Hey EMACS -*- linux-c -*- */
/* $Id: files9x.c 3524 2007-06-26 13:31:26Z roms $ */

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
	TI File Format handling routines
	Calcs: TI-NSpire
*/

#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ticonv.h>
#include "tifiles.h"
#include "error.h"
#include "logging.h"
#include "macros.h"
#include "typesxx.h"
#include "filesnsp.h"
#include "rwfile.h"


/***********/
/* Reading */
/***********/

/**
 * tnsp_file_read_regular:
 * @filename: name of file to open.
 * @content: where to store the file content.
 *
 * Load the file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_file_read_regular(const char *filename, FileContent *content)
{
	FILE *f;
	int ret = ERR_FILE_IO;

	if (content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_regular(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrr2;
	}

	f = g_fopen(filename, "rb");
	if (f == NULL)
	{
		tifiles_info( "Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrr2;
	}

	content->model = CALC_NSPIRE;
	content->model_dst = content->model;

	content->entries = (VarEntry **)g_malloc0((content->num_entries + 1) * sizeof(VarEntry*));

	{
		long cur_pos;
		VarEntry *entry = content->entries[0] = (VarEntry *)g_malloc0(sizeof(VarEntry));

		gchar *basename = g_path_get_basename(filename);
		gchar *ext = tifiles_fext_get(basename);

		entry->type = tifiles_fext2vartype(content->model, ext);
		if (ext && ext[0]) *(ext-1) = '\0';

		entry->folder[0] = 0;
		strncpy(entry->name, basename, sizeof(entry->name) - 1);
		entry->name[sizeof(entry->name) - 1] = 0;
		g_free(basename);

		entry->attr = ATTRB_NONE;
		if (fseek(f, 0, SEEK_END) < 0) goto tfrr;
		cur_pos = ftell(f);
		if (cur_pos < 0) goto tfrr;
		if (fseek(f, 0, SEEK_SET) < 0) goto tfrr;

		// The Nspire series' members have at best 128 MB of Flash (TODO: modify this code if this no longer holds).
		// Regular files larger than that size (and even several MBs smaller than that size) are therefore insane.
		if (cur_pos >= (128L << 20))
		{
			ret = ERR_INVALID_FILE;
			goto tfrr;
		}
		entry->size = (uint32_t)cur_pos;

		entry->data = (uint8_t *)g_malloc0(entry->size);  
		if(fread(entry->data, 1, entry->size, f) < entry->size) goto tfrr;
	}

	content->num_entries++;

	fclose(f);
	return 0;

tfrr:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrr2:
	tifiles_content_delete_regular(content);
	return ret;
}

/**
 * tnsp_file_read_flash:
 * @filename: name of flash file to open.
 * @content: where to store the file content.
 *
 * Load the flash file into a #FlashContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_flash when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_file_read_flash(const char *filename, FlashContent *content)
{
	FILE *f;
	int c;
	long cur_pos;
	uint32_t file_size;
	int ret = ERR_FILE_IO;

	if (content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_tno(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrf2;
	}

	f = g_fopen(filename, "rb");
	if (f == NULL)
	{
		tifiles_info("Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrf2;
	}

	if (fseek(f, 0, SEEK_END) < 0) goto tfrf;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrf;
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrf;
	// The Nspire series' members have at best 128 MB of Flash (TODO: modify this code if this no longer holds).
	// Flash files larger than that size (and even several MBs smaller than that size) are insane.
	if (cur_pos >= (128L << 20))
	{
		ret = ERR_INVALID_FILE;
		goto tfrf;
	}
	file_size = (uint32_t)cur_pos;

	// FIXME this does no longer hold true now that there are multiple calcs for Nspire series members.
	content->model = CALC_NSPIRE;

	// Skip chars.
	c = 0;
	while (c != ' ')
	{
		c = fgetc(f);
		if (c == EOF)
		{
			goto tfrf;
		}
	}

	// Read revision major.
	c = fgetc(f);
	if (c == EOF)
	{
		goto tfrf;
	}
	content->revision_major = c;

	// Skip char.
	c = fgetc(f);
	if (c == EOF)
	{
		goto tfrf;
	}

	// Read revision minor.
	c = fgetc(f);
	if (c == EOF)
	{
		goto tfrf;
	}
	content->revision_minor = c;

	// Skip chars.
	c = fgetc(f);
	if (c == EOF)
	{
		goto tfrf;
	}

	c = 0;
	while (c != ' ')
	{
		c = fgetc(f);
		if (c == EOF)
		{
			goto tfrf;
		}
	}
	if (fscanf(f, "%i", &(content->data_length)) < 1)
	{
		goto tfrf;
	}
	if (content->data_length > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrf;
	}
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrf;

	content->data_part = (uint8_t *)g_malloc0(content->data_length);
	if (content->data_part == NULL) 
	{
		fclose(f);
		tifiles_content_delete_flash(content);
		return ERR_MALLOC;
	}

	content->next = NULL;
	if(fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;

	fclose(f);
	return 0;

tfrf:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrf2:
	tifiles_content_delete_flash(content);
	return ret;
}

/***********/
/* Writing */
/***********/

/**
 * tnsp_file_write_regular:
 * @filename: name of file where to write or NULL.
 * @content: the file content to write.
 * @real_filename: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write one variable into a single file. If filename is set to NULL,
 * the function build a filename from varname and allocates resulting filename in %real_fname.
 * %filename and %real_filename can be NULL but not both !
 *
 * %real_filename must be freed when no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_file_write_regular(const char *fname, FileContent *content, char **real_fname)
{
	FILE *f;
	char *filename = NULL;
	VarEntry *entry;

	if (content->entries == NULL || content->entries[0] == NULL)
	{
		tifiles_warning("%s: skipping content with NULL content->entries or content->entries[0]", __FUNCTION__);
		return ERR_FILE_IO;
	}

	if (fname != NULL) 
	{
		filename = g_strdup(fname);
		if (filename == NULL)
		{
			return ERR_MALLOC;
		}
	}
	else
	{
		filename = tifiles_build_filename(content->model_dst, content->entries[0]);
		if (real_fname != NULL)
		{
			*real_fname = g_strdup(filename);
		}
	}

	f = g_fopen(filename, "wb");
	if (f == NULL) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		g_free(filename);
		return ERR_FILE_OPEN;
	}

	entry = content->entries[0];
	if(fwrite(entry->data, 1, entry->size, f) < entry->size)
	{
		goto tfwr;
	}

	g_free(filename);
	fclose(f);
	return 0;

tfwr:  // release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	g_free(filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**************/
/* Displaying */
/**************/

/**
 * tnsp_content_display_flash:
 * @content: a FlashContent structure.
 *
 * Display fields of a FlashContent structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_content_display_flash(FlashContent *content)
{
	FlashContent *ptr = content;

	if (content == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		tifiles_info("FlashContent for TI-Nspire: %p", ptr);
		tifiles_info("Model:           %02X (%u)", ptr->model, ptr->model);
		tifiles_info("Signature:       %s", tifiles_calctype2signature(ptr->model));
		tifiles_info("model_dst:       %02X (%u)", ptr->model_dst, ptr->model_dst);
		tifiles_info("Revision:        %u.%u", ptr->revision_major, ptr->revision_minor);
		tifiles_info("Flags:           %02X", ptr->flags);
		tifiles_info("Object type:     %02X", ptr->object_type);
		tifiles_info("Date:            %02X/%02X/%02X%02X", ptr->revision_day, ptr->revision_month, ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
		tifiles_info("Name:            %s", ptr->name);
		tifiles_info("Data type:       OS data");
		tifiles_info("Length:          %08X (%i)", ptr->data_length, ptr->data_length);
		tifiles_info("Data part:       %p", ptr->data_part);
		tifiles_info("Next:            %p", ptr->next);
	}

	return 0;
}

/**
 * tnsp_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_file_display(const char *filename)
{
	FileContent *content1;
	FlashContent *content3;
	int ret;

	if (tifiles_file_is_os(filename)) 
	{
		content3 = tifiles_content_create_flash(CALC_NSPIRE);
		ret = tnsp_file_read_flash(filename, content3);
		if (!ret)
		{
			tnsp_content_display_flash(content3);
			tifiles_content_delete_flash(content3);
		}
	}
	else if (tifiles_file_is_regular(filename)) 
	{
		content1 = tifiles_content_create_regular(CALC_NSPIRE);
		ret = tnsp_file_read_regular(filename, content1);
		if (!ret)
		{
			tifiles_file_display_regular(content1);
			tifiles_content_delete_regular(content1);
		}
	}
	else
	{
		tifiles_info("Unknown file type !");
		return ERR_BAD_FILE;
	}

	return ret;
}
