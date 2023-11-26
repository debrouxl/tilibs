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
	TI File Format handling routines
	Calcs: 89/89tm/92/92+/V200
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
#include "files9x.h"
#include "rwfile.h"

#ifndef DISABLE_TI9X

/********/
/* Misc */
/********/

static const int fsignature[2] = { 1, 0 };

/***********/
/* Reading */
/***********/

/**
 * ti9x_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a Ti9xRegular structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_read_regular(const char *filename, Ti9xRegular *content)
{
	FILE *f;
	long cur_pos;
	char default_folder[FLDNAME_MAX];
	char current_folder[FLDNAME_MAX];
	uint32_t curr_offset = 0;
	uint32_t next_offset = 0;
	uint32_t file_size;
	uint16_t tmp;
	unsigned int i, j;
	char signature[9];
	char varname[VARNAME_MAX];
	int ret = ERR_FILE_IO;

	if (content == nullptr)
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
	if (f == nullptr) 
	{
		ret = ERR_FILE_OPEN;
		goto tfrr2;
	}

	// Get file size, then rewind.
	if (fseek(f, 0, SEEK_END) < 0) goto tfrr;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrr;
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrr;

	// The TI-68k series' members have at best 4 MB of Flash (TODO: modify this code if this no longer holds).
	// Regular / group files larger than that size are highly dubious, files larger than twice that size are insane.
	if (cur_pos >= (8L << 20))
	{
		ret = ERR_INVALID_FILE;
		goto tfrr;
	}
	file_size = (uint32_t)cur_pos;

	if (fread_8_chars(f, signature) < 0) goto tfrr; // Offset 0
	content->model = tifiles_signature2calctype(signature);
	if (content->model == CALC_NONE)
	{
		ret = ERR_INVALID_FILE;
		goto tfrr;
	}
	if (content->model_dst == CALC_NONE)
	{
		content->model_dst = content->model;
	}

	if (fread_word(f, nullptr) < 0) goto tfrr; // Offset 0x8
	if (fread_8_chars(f, default_folder) < 0) goto tfrr; // Offset 0xA
	ticonv_varname_from_tifile_sn(content->model_dst, default_folder, content->default_folder, sizeof(content->default_folder), -1);
	strncpy(current_folder, content->default_folder, sizeof(current_folder) - 1);
	current_folder[sizeof(current_folder) - 1] = 0;
	if (fread_n_chars(f, 40, content->comment) < 0) goto tfrr; // Offset 0x12
	if (fread_word(f, &tmp) < 0) goto tfrr; // Offset 0x3A
	content->num_entries = tmp;

	content->entries = (VarEntry **)g_malloc0((content->num_entries + 1) * sizeof(VarEntry*));
	if (content->entries == nullptr) 
	{
		ret = ERR_MALLOC;
		goto tfrr;
	}

	for (i = 0, j = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[j] = (VarEntry *)g_malloc0(sizeof(VarEntry));
		uint8_t attr;

		if (fread_long(f, &curr_offset) < 0) goto tfrr; // Offset N, 0x3C for the first entry
		if (curr_offset > file_size)
		{
			ret = ERR_INVALID_FILE;
			goto tfrr;
		}
		if (fread_8_chars(f, varname) < 0)  goto tfrr; // Offset N+4, 0x40 for the first entry
		ticonv_varname_from_tifile_sn(content->model_dst, varname, entry->name, sizeof(entry->name), entry->type);
		if (fread_byte(f, &(entry->type)) < 0) goto tfrr; // Offset N+12, 0x48 for the first entry
		if (fread_byte(f, &attr) < 0) goto tfrr; // Offset N+13, 0x49 for the first entry
		entry->attr = (attr == 2 || attr == 3) ? ATTRB_ARCHIVED : (FileAttr)attr;
		if (fread_word(f, nullptr) < 0) goto tfrr; // Offset N+14, 0x4A for the first entry

		if (entry->type == TI92_DIR) // same as TI89_DIR, TI89t_DIR, ...
		{
			strncpy(current_folder, entry->name,sizeof(current_folder) - 1);
			current_folder[sizeof(current_folder) - 1] = 0;
			g_free(entry);
			continue;			// folder: skip entry
		}
		else 
		{
			uint16_t checksum, sum = 0;

			j++;
			strncpy(entry->folder, current_folder, sizeof(entry->folder) - 1);
			current_folder[sizeof(entry->folder) - 1] = 0;
			cur_pos = ftell(f);
			if (cur_pos < 0) goto tfrr;
			if (fread_long(f, &next_offset) < 0) goto tfrr; // Offset N+16, 0x4C for the first entry
			if (next_offset > file_size)
			{
				ret = ERR_INVALID_FILE;
				goto tfrr;
			}
			entry->size = next_offset - curr_offset - 4 - 2;
			if (entry->size > file_size)
			{
				ret = ERR_INVALID_FILE;
				goto tfrr;
			}
			entry->data = (uint8_t *)g_malloc0(entry->size);
			if (entry->data == nullptr) 
			{
				ret = ERR_MALLOC;
				goto tfrr;
			}

			if (fseek(f, curr_offset, SEEK_SET)) goto tfrr;
			if (fread_long(f, nullptr) < 0) goto tfrr;	// Normally: offset N+22, 0x52 for the first entry
			if (fread(entry->data, 1, entry->size, f) < entry->size) goto tfrr; // Normally: offset N+26, 0x56 for the first entry

			if (fread_word(f, &checksum) < 0) goto tfrr;
			if (fseek(f, cur_pos, SEEK_SET)) goto tfrr;

			sum = tifiles_checksum(entry->data, entry->size);
			if (sum != checksum)
			{
				tifiles_warning("Checksum mismatch in entry #%u of %s: computed: %0x04X / stored: %0x04X", i, filename, sum, checksum);
			}
			content->checksum += sum;	// sum of all checksums but unused
			content->stored_checksum += checksum;	// sum of all checksums but unused
		}
	}
	content->num_entries = j;
	content->entries = (VarEntry **)g_realloc(content->entries, content->num_entries * sizeof(VarEntry*));
	//fread_long(f, &next_offset);
	//fseek(f, next_offset - 2, SEEK_SET);
	//fread_word(f, &(content->checksum));

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
 * ti9x_file_read_backup:
 * @filename: name of backup file to open.
 * @content: where to store the file content.
 *
 * Load the backup file into a Ti9xBackup structure.
 *
 * Structure content must be freed with #tifiles_content_delete_backup when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_read_backup(const char *filename, Ti9xBackup *content)
{
	FILE *f;
	long cur_pos = 0;
	uint32_t file_size;
	char signature[9];
	uint16_t sum;
	int ret = ERR_FILE_IO;

	if (content == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_backup(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrb2;
	}

	f = g_fopen(filename, "rb");
	if (f == nullptr) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrb2;
	}

	// Get file size, then rewind.
	if (fseek(f, 0, SEEK_END) < 0) goto tfrb;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrb;
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrb;

	// The TI-68k series' members have at best 4 MB of Flash (TODO: modify this code if this no longer holds).
	// Backup files larger than that size are highly dubious, files larger than twice that size are insane.
	if (cur_pos >= (8L << 20))
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}

	if (fread_8_chars(f, signature) < 0) goto tfrb;
	content->model = tifiles_signature2calctype(signature);
	if (content->model == CALC_NONE)
	{
		ret = ERR_INVALID_FILE;
	}

	if (fread_word(f, nullptr) < 0) goto tfrb;
	if (fread_8_chars(f, nullptr) < 0) goto tfrb;
	if (fread_n_chars(f, 40, content->comment) < 0) goto tfrb;
	if (fread_word(f, nullptr) < 0) goto tfrb;
	if (fread_long(f, nullptr) < 0) goto tfrb;
	if (fread_8_chars(f, content->rom_version) < 0) goto tfrb;
	if (fread_byte(f, &(content->type)) < 0) goto tfrb;
	if (fread_byte(f, nullptr) < 0) goto tfrb;
	if (fread_word(f, nullptr) < 0) goto tfrb;
	if (fread_long(f, &file_size) < 0) goto tfrb;
	if (file_size > (uint32_t)cur_pos)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	content->data_length = file_size - 0x52 - 2;
	if (fread_word(f, nullptr) < 0) goto tfrb;

	content->data_part = (uint8_t *)g_malloc0(content->data_length);
	if (content->data_part == nullptr) 
	{
		ret = ERR_MALLOC;
		goto tfrb;
	}

	if (fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrb;
	if (fread_word(f, &(content->stored_checksum)) < 0) goto tfrb;

	sum = tifiles_checksum(content->data_part, content->data_length);
	if (sum != content->stored_checksum)
	{
		tifiles_warning("Checksum mismatch in %s: computed: %0x04X / stored: %0x04X", filename, sum, content->stored_checksum);
	}
	content->checksum = sum;	// computed checksum.

	fclose(f);
	return 0;

tfrb:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrb2:
	tifiles_content_delete_backup(content);
	return ret;
}

static int check_device_type(uint8_t id)
{
	static const uint8_t types[] = { 0, DEVICE_TYPE_89, DEVICE_TYPE_92P };

	for (int i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
	{
		if (types[i] == id)
		{
			return i;
		}
	}

	return 0;
}

static int check_data_type(uint8_t id)
{
	static const uint8_t types[] = { 0, TI89_AMS, TI89_APPL, TI89_CERTIF, TI89_LICENSE };

	for (int i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
	{
		if (types[i] == id)
		{
			return i;
		}
	}

	return 0;
}

/**
 * ti9x_file_read_flash:
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
int ti9x_file_read_flash(const char *filename, Ti9xFlash *head)
{
	FILE *f;
	Ti9xFlash *content = head;
	long cur_pos = 0;
	int tib = 0;
	char signature[9];
	int ret = ERR_FILE_IO;

	if (head == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_flash(filename) && !tifiles_file_is_tib(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrf2;
	}

	// detect file type (old or new format)
	tib = tifiles_file_is_tib(filename);

	f = g_fopen(filename, "rb");
	if (f == nullptr) 
	{
		tifiles_info("Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrf2;
	}  

	if (fseek(f, 0, SEEK_END)) goto tfrf;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrf;
	if (fseek(f, 0, SEEK_SET)) goto tfrf;

	// The TI-68k series' members have at best 4 MB of Flash.
	// TIB files larger than that size are insane.
	if (cur_pos >= (4L << 20))
	{
		ret = ERR_INVALID_FILE;
		goto tfrf;
	}

	if (tib) 
	{
		// tib is an old format but mainly used by developers
		memset(content, 0, sizeof(Ti9xFlash));

		content->data_length = (uint32_t)cur_pos;

		strncpy(content->name, "basecode", sizeof(content->name) - 1);
		content->name[sizeof(content->name) - 1] = 0;
		content->data_type = 0x23;	// FLASH os

		content->data_part = (uint8_t *)g_malloc0(content->data_length);
		if (content->data_part == nullptr) 
		{
			ret = ERR_MALLOC;
			goto tfrf;
		}

		if (fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;
		switch(content->data_part[8])
		{
			case 1: content->device_type = DEVICE_TYPE_92P; break;	// TI92+
			case 3: content->device_type = DEVICE_TYPE_89; break;	// TI89
			// value added by the TI community according to HWID parameter
			// doesn't have any 'legal' existence.
			case 8: content->device_type = DEVICE_TYPE_92P; break;	// V200PLT
			case 9: content->device_type = DEVICE_TYPE_89; break;	// Titanium
		}

		content->next = nullptr;
	} 
	else 
	{
		for (content = head;; content = content->next) 
		{
			if (fread_8_chars(f, signature) < 0) goto tfrf;
			content->model = tifiles_file_get_model(filename);
			if (fread_byte(f, &(content->revision_major)) < 0) goto tfrf;
			if (fread_byte(f, &(content->revision_minor)) < 0) goto tfrf;
			if (fread_byte(f, &(content->flags)) < 0) goto tfrf;
			if (fread_byte(f, &(content->object_type)) < 0) goto tfrf;
			if (fread_byte(f, &(content->revision_day)) < 0) goto tfrf;
			if (fread_byte(f, &(content->revision_month)) < 0) goto tfrf;
			if (fread_word(f, &(content->revision_year)) < 0) goto tfrf;
			if (fskip(f, 1) < 0) goto tfrf;
			if (fread_8_chars(f, content->name) < 0) goto tfrf;
			if (fskip(f, 23) < 0) goto tfrf;
			if (fread_byte(f, &(content->device_type)) < 0) goto tfrf;
			if (fread_byte(f, &(content->data_type)) < 0) goto tfrf;
			if (fskip(f, 23) < 0) goto tfrf;
			if (fread_byte(f, &(content->hw_id)) < 0) goto tfrf;
			if (fread_long(f, &(content->data_length)) < 0) goto tfrf;

			if (content->data_type != TI89_LICENSE && !check_device_type(content->device_type))
			{
				ret = ERR_INVALID_FILE;
				goto tfrf;
			}
			if (!check_data_type(content->data_type))
			{
				ret = ERR_INVALID_FILE;
				goto tfrf;
			}
			// TODO: modify this code if TI ever makes a TI-eZ80 model with more than 4 MB of Flash memory...
			if (content->data_length > 4U * 1024 * 1024 - 65536U)
			{
				// Data length larger than Flash memory size - boot code sector size doesn't look right.
				ret = ERR_INVALID_FILE;
				goto tfrf;
			}

			content->data_part = (uint8_t *)g_malloc0(content->data_length);
			if (content->data_part == nullptr)
			{
				ret = ERR_MALLOC;
				goto tfrf;
			}

			if (fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;
			if (   (content->data_type == TI83p_AMS && content->data_part[0] != 0x80)
			    || (content->data_type == TI83p_APPL && content->data_part[0] != 0x81))
			{
				ret = ERR_INVALID_FILE;
				goto tfrf;
			}
			content->next = nullptr;

			// check for end of file
			if (fread_8_chars(f, signature) < 0)
			{
				break;
			}
			if (strcmp(signature, "**TIFL**") || feof(f))
			{
				break;
			}
			if (fseek(f, -8, SEEK_CUR)) goto tfrf;

			content->next = (Ti9xFlash *)g_malloc0(sizeof(Ti9xFlash));
			if (content->next == nullptr) 
			{
				ret = ERR_MALLOC;
				goto tfrf;
			}
		}
	}

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
 * ti9x_file_write_regular:
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
int ti9x_file_write_regular(const char *fname, Ti9xRegular *content, char **real_fname)
{
	unsigned int i;
	char *filename = nullptr;
	uint32_t offset = 0x52;
	unsigned int num_folders;
	char default_folder[FLDNAME_MAX];
	char fldname[FLDNAME_MAX], varname[VARNAME_MAX];

	if (content->entries == nullptr)
	{
		tifiles_warning("%s: skipping content with NULL content->entries", __FUNCTION__);
		return 0;
	}

	if (fname != nullptr)
	{
		filename = g_strdup(fname);
		if (filename == nullptr)
		{
			return ERR_MALLOC;
		}
	} 
	else 
	{
		if (content->entries[0])
		{
			filename = tifiles_build_filename(content->model_dst, content->entries[0]);
		}
		else
		{
			tifiles_warning("%s: asked to build a filename from null content->entries[0], bailing out", __FUNCTION__);
			if (real_fname != nullptr)
			{
				*real_fname = nullptr;
			}
			return 0;
		}
		if (real_fname != nullptr)
		{
			*real_fname = g_strdup(filename);
		}
	}

	// build the table of folder & variable entries
	int** table = tifiles_create_table_of_entries((FileContent*)content, &num_folders);
	if (table == nullptr)
	{
		g_free(filename);
		return ERR_MALLOC;
	}

	FILE* f = g_fopen(filename, "wb");
	if (f == nullptr) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		tifiles_free_table_of_entries(table);
		g_free(filename);
		return ERR_FILE_OPEN;
	}

	// write header
	if (fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwr;
	if (fwrite(fsignature, 1, 2, f) < 2) goto tfwr;
	if (content->num_entries == 1)	// folder entry for single var is placed here
	{
		strncpy(content->default_folder, content->entries[0]->folder, sizeof(content->default_folder) - 1);
		content->default_folder[sizeof(content->default_folder) - 1] = 0;
	}
	ticonv_varname_to_tifile_sn(content->model, content->default_folder, default_folder, sizeof(default_folder), -1);
	if (fwrite_8_chars(f, default_folder) < 0) goto tfwr;
	if (fwrite_n_bytes(f, 40, (uint8_t *)content->comment) < 0) goto tfwr;
	if (content->num_entries > 1) 
	{
		if (fwrite_word(f, (uint16_t) (content->num_entries + num_folders)) < 0) goto tfwr;
		offset += 16 * (content->num_entries + num_folders - 1);
	} 
	else
	{
		if (fwrite_word(f, 1) < 0) goto tfwr;
	}

	// write table of entries
	for (i = 0; table[i] != nullptr; i++) 
	{
		int j, idx = table[i][0];
		const VarEntry* fentry = content->entries[idx];

		if (fentry == nullptr)
		{
			tifiles_warning("%s: skipping null content entry %d", __FUNCTION__, i);
			continue;
		}

		if (content->num_entries > 1)	// single var does not have folder entry
		{
			if (fwrite_long(f, offset) < 0) goto tfwr;
			ticonv_varname_to_tifile_sn(content->model, fentry->folder, fldname, sizeof(fldname), -1);
			if (fwrite_8_chars(f, fldname) < 0) goto tfwr;
			if (fwrite_byte(f, (uint8_t)tifiles_folder_type(content->model)) < 0) goto tfwr;
			if (fwrite_byte(f, 0x00) < 0) goto tfwr;
			for (j = 0; table[i][j] != -1; j++);
			if (fwrite_word(f, (uint16_t) j) < 0) goto tfwr;
		}

		for (j = 0; table[i][j] != -1; j++) 
		{
			const int idx2 = table[i][j];
			const VarEntry *entry = content->entries[idx2];
			uint8_t attr = ATTRB_NONE;

			if (fwrite_long(f, offset) < 0) goto tfwr;
			ticonv_varname_to_tifile_sn(content->model, entry->name, varname, sizeof(varname), entry->type);
			if (fwrite_8_chars(f, varname) < 0) goto tfwr;
			if (fwrite_byte(f, entry->type) < 0) goto tfwr;
			attr = (entry->attr == ATTRB_ARCHIVED) ? 3 : entry->attr;
			if (fwrite_byte(f, attr) < 0) goto tfwr;
			if (fwrite_word(f, 0) < 0) goto tfwr;

			offset += entry->size + 4 + 2;
		}
	}

	if (fwrite_long(f, offset) < 0) goto tfwr;
	if (fwrite_word(f, 0x5aa5) < 0) goto tfwr;

	// write data
	for (i = 0; table[i] != nullptr; i++) 
	{
		for (int j = 0; table[i][j] != -1; j++) 
		{
			const int idx = table[i][j];
			const VarEntry *entry = content->entries[idx];

			if (fwrite_long(f, 0) < 0) goto tfwr;
			if (fwrite(entry->data, 1, entry->size, f) < entry->size) goto tfwr;
			const uint16_t sum = tifiles_checksum(entry->data, entry->size);
			if (fwrite_word(f, sum) < 0) goto tfwr;
		}
	}

	tifiles_free_table_of_entries(table);
	g_free(filename);
	fclose(f);
	return 0;

tfwr:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	tifiles_free_table_of_entries(table);
	g_free(filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**
 * ti9x_file_write_backup:
 * @filename: name of backup file where to write.
 * @content: the file content to write.
 *
 * Write content to a backup file.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_write_backup(const char *filename, Ti9xBackup *content)
{
	if (filename == nullptr || content == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	FILE* f = g_fopen(filename, "wb");
	if (f == nullptr) 
	{
		tifiles_info("Unable to open this file: %s", filename);
		return ERR_FILE_OPEN;
	}

	if (fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwb;
	if (fwrite(fsignature, 1, 2, f) < 2) goto tfwb;
	if (fwrite_8_chars(f, "") < 0) goto tfwb;
	if (fwrite_n_bytes(f, 40, (uint8_t *)content->comment) < 0) goto tfwb;
	if (fwrite_word(f, 1) < 0) goto tfwb;
	if (fwrite_long(f, 0x52) < 0) goto tfwb;
	if (fwrite_8_chars(f, content->rom_version) < 0) goto tfwb;
	if (fwrite_word(f, content->type) < 0) goto tfwb;
	if (fwrite_word(f, 0) < 0) goto tfwb;
	if (fwrite_long(f, content->data_length + 0x52 + 2) < 0) goto tfwb;
	if (fwrite_word(f, 0x5aa5) < 0) goto tfwb;
	if (fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwb;

	content->checksum = tifiles_checksum(content->data_part, content->data_length);
	if (fwrite_word(f, content->checksum) < 0) goto tfwb;

	fclose(f);
	return 0;

tfwb:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**
 * ti9x_file_write_flash:
 * @filename: name of flash file where to write.
 * @content: the file content to write.
 *
 * Write content to a flash file (os or app).
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_write_flash(const char *fname, Ti9xFlash *head, char **real_fname)
{
	const Ti9xFlash *content = head;
	char *filename;

	if (head == nullptr)
	{
		tifiles_critical("%s: head is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (fname)
	{
		filename = g_strdup(fname);
		if (filename == nullptr)
		{
			return ERR_MALLOC;
		}
	}
	else
	{
		VarEntry ve;

		for (content = head; content != nullptr; content = content->next)
		{
			if (content->data_type == TI89_AMS || content->data_type == TI89_APPL)
			{
				break;
			}
		}
		if (content == nullptr)
		{
			tifiles_critical("%s: content is NULL", __FUNCTION__);
			return ERR_BAD_FILE;
		}

		strncpy(ve.name, content->name, sizeof(ve.name) - 1);
		ve.name[sizeof(ve.name) - 1] = 0;
		ve.type = content->data_type;

		filename = tifiles_build_filename(content->model, &ve);
		if (real_fname != nullptr)
		{
			*real_fname = g_strdup(filename);
		}
	}

	FILE* f = g_fopen(filename, "wb");
	if (f == nullptr) 
	{
		tifiles_info("Unable to open this file: %s", filename);
		g_free(filename);
		return ERR_FILE_OPEN;
	}

	for (content = head; content != nullptr; content = content->next) 
	{
		if (fwrite_8_chars(f, "**TIFL**") < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_major) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_minor) < 0) goto tfwf;
		if (fwrite_byte(f, content->flags) < 0) goto tfwf;
		if (fwrite_byte(f, content->object_type) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_day) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_month) < 0) goto tfwf;
		if (fwrite_word(f, content->revision_year) < 0) goto tfwf;
		if (fwrite_byte(f, (uint8_t) strlen(content->name)) < 0) goto tfwf;
		if (fwrite_8_chars(f, content->name) < 0) goto tfwf;
		if (fwrite_n_chars(f, 23, "") < 0) goto tfwf;
		if (fwrite_byte(f, content->device_type) < 0) goto tfwf;
		if (fwrite_byte(f, content->data_type) < 0) goto tfwf;
		if (fwrite_n_chars(f, 23, "") < 0) goto tfwf;
		if (fwrite_byte(f, content->hw_id) < 0)  goto tfwf;
		if (fwrite_long(f, content->data_length) < 0) goto tfwf;
		if (fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwf;
	}

	g_free(filename);
	fclose(f);
	return 0;

tfwf:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	g_free(filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**************/
/* Displaying */
/**************/

/**
 * ti9x_content_display_backup:
 * @content: a Ti9xBackup structure.
 *
 * Display fields of a Ti9xBackup structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_content_display_backup(Ti9xBackup *content)
{
	if (content == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	tifiles_info("BackupContent for TI-9x: %p", content);
	tifiles_info("Model:          %02X (%u)", content->model, content->model);
	tifiles_info("Signature:      %s", tifiles_calctype2signature(content->model));
	tifiles_info("Comment:        %s", content->comment);
	tifiles_info("ROM version:    %s", content->rom_version);
	tifiles_info("Type:           %02X (%s)", content->type, tifiles_vartype2string(content->model, content->type));

	tifiles_info("data_length:    %08X (%u)", content->data_length, content->data_length);
	tifiles_info("data_part:      %p", content->data_part);

	tifiles_info("Checksum:       %04X (%u)", content->checksum, content->checksum);

	return 0;
}

/**
 * ti9x_content_display_flash:
 * @content: a Ti9xFlash structure.
 *
 * Display fields of a Ti9xFlash structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_content_display_flash(Ti9xFlash *content)
{
	for (Ti9xFlash* ptr = content; ptr != nullptr; ptr = ptr->next)
	{
		tifiles_info("FlashContent for TI-9x: %p", ptr);
		tifiles_info("Model:           %02X (%u)", ptr->model, ptr->model);
		tifiles_info("Signature:       %s", tifiles_calctype2signature(ptr->model));
		tifiles_info("model_dst:       %02X (%u)", ptr->model_dst, ptr->model_dst);
		tifiles_info("Revision:        %u.%u", ptr->revision_major, ptr->revision_minor);
		tifiles_info("Flags:           %02X", ptr->flags);
		tifiles_info("Object type:     %02X", ptr->object_type);
		tifiles_info("Date:            %02X/%02X/%02X%02X", ptr->revision_day, ptr->revision_month, ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
		tifiles_info("Name:            %s", ptr->name);
		tifiles_info("Device type:     %s", ptr->device_type == DEVICE_TYPE_89 ? "ti89" : "ti92+");
		switch (ptr->data_type) 
		{
			case 0x23:
				tifiles_info("Data type:       OS data");
				break;
			case 0x24:
				tifiles_info("Data type:       APP data");
				break;
			case 0x20:
			case 0x25:
				tifiles_info("Data type:       certificate");
				break;
			case 0x3E:
				tifiles_info("Data type:       license");
				break;
			default:
				tifiles_info("Data type:       Unknown (send mail to tilp-users@lists.sf.net)");
				break;
		}
		tifiles_info("Hardware ID:     %02X (%u)", ptr->hw_id, ptr->hw_id);
		tifiles_info("Length:          %08X (%u)", ptr->data_length, ptr->data_length);
		tifiles_info("Data part:       %p", ptr->data_part);
		tifiles_info("Next:            %p", ptr->next);
	}

	return 0;
}

/**
 * ti9x_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_display(const char *filename)
{
	int ret;

	// the testing order is important: regular before backup (due to TI89/92+)
	if (tifiles_file_is_flash(filename) || tifiles_file_is_tib(filename)) 
	{
		Ti9xFlash* content3 = tifiles_content_create_flash(CALC_TI92);
		ret = ti9x_file_read_flash(filename, content3);
		if (!ret)
		{
			ti9x_content_display_flash(content3);
			tifiles_content_delete_flash(content3);
		}
	} 
	else if (tifiles_file_is_regular(filename)) 
	{
		Ti9xRegular* content1 = tifiles_content_create_regular(CALC_TI92);
		ret = ti9x_file_read_regular(filename, content1);
		if (!ret)
		{
			tifiles_file_display_regular(content1);
			tifiles_content_delete_regular(content1);
		}
	} 
	else if (tifiles_file_is_backup(filename)) 
	{
		Ti9xBackup* content2 = tifiles_content_create_backup(CALC_TI92);
		ret = ti9x_file_read_backup(filename, content2);
		if (!ret)
		{
			ti9x_content_display_backup(content2);
			tifiles_content_delete_backup(content2);
		}
	} 
	else
	{
		tifiles_info("Unknown file type !");
		return ERR_BAD_FILE;
	}

	return ret;
}

#endif
