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

static int fsignature[2] = { 1, 0 };

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
  long cur_pos = 0;
  char default_folder[FLDNAME_MAX];
  char current_folder[FLDNAME_MAX];
  uint32_t curr_offset = 0;
  uint32_t next_offset = 0;
  uint16_t tmp;
  int i, j;
  char signature[9];
  char varname[VARNAME_MAX];

  if (!tifiles_file_is_regular(filename))
    return ERR_INVALID_FILE;

  if (content == NULL)
  {
    tifiles_critical("%s: an argument is NULL", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  f = g_fopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: %s", filename);
    return ERR_FILE_OPEN;
  }

  if(fread_8_chars(f, signature) < 0) goto tffr;
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;
  if(content->model_dst == CALC_NONE) content->model_dst = content->model;

  if(fread_word(f, NULL) < 0) goto tffr;
  if(fread_8_chars(f, default_folder) < 0) goto tffr;
  ticonv_varname_from_tifile_s(content->model_dst, default_folder, content->default_folder, -1);
  strcpy(current_folder, content->default_folder);
  if(fread_n_chars(f, 40, content->comment) < 0) goto tffr;
  if(fread_word(f, &tmp) < 0) goto tffr;
  content->num_entries = tmp;

  content->entries = g_malloc0((content->num_entries + 1) * sizeof(VarEntry*));
  if (content->entries == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }

  for (i = 0, j = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = content->entries[j] = g_malloc0(sizeof(VarEntry));

    if(fread_long(f, &curr_offset) < 0) goto tffr;
    if(fread_8_chars(f, varname) < 0)  goto tffr;
	ticonv_varname_from_tifile_s(content->model_dst, varname, entry->name, entry->type);
    if(fread_byte(f, &(entry->type)) < 0) goto tffr;
    if(fread_byte(f, &(entry->attr)) < 0) goto tffr;
    entry->attr = (entry->attr == 2 || entry->attr == 3) ? ATTRB_ARCHIVED : entry->attr;
    if(fread_word(f, NULL) < 0) goto tffr;

    if (entry->type == TI92_DIR) // same as TI89_DIR, TI89t_DIR, ...
	{
      strcpy(current_folder, entry->name);
      g_free(entry);
      continue;			// folder: skip entry
    } 
	else 
	{
	  uint16_t checksum, sum = 0;

      j++;
      strcpy(entry->folder, current_folder);
      cur_pos = ftell(f);
	  if(cur_pos == -1L) goto tffr;
      if(fread_long(f, &next_offset) < 0) goto tffr;
      entry->size = next_offset - curr_offset - 4 - 2;
      entry->data = (uint8_t *)g_malloc0(entry->size);
      if (entry->data == NULL) 
	  {
		fclose(f);
		tifiles_content_delete_regular(content);
		return ERR_MALLOC;
      }

      if(fseek(f, curr_offset, SEEK_SET)) goto tffr;
      if(fread_long(f, NULL) < 0) goto tffr;	// 4 bytes (NULL)
      if(fread(entry->data, 1, entry->size, f) < entry->size) goto tffr;

      if(fread_word(f, &checksum) < 0) goto tffr;
      if(fseek(f, cur_pos, SEEK_SET)) goto tffr;

	  sum = tifiles_checksum(entry->data, entry->size);
	  if(sum != checksum)
	  {
		  fclose(f);
		  tifiles_content_delete_regular(content);
	      return ERR_FILE_CHECKSUM;
	  }
	  content->checksum += sum;	// sum of all checksums but unused
    }
  }
  content->num_entries = j;
  content->entries = realloc(content->entries, content->num_entries * sizeof(VarEntry*));
  //fread_long(f, &next_offset);
  //fseek(f, next_offset - 2, SEEK_SET);
  //fread_word(f, &(content->checksum));

  fclose(f);
  return 0;

tffr:	// release on exit
    fclose(f);
	tifiles_content_delete_regular(content);
	return ERR_FILE_IO;
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
  uint32_t file_size;
  char signature[9];
  uint16_t sum;

  if (!tifiles_file_is_backup(filename))
    return ERR_INVALID_FILE;

  f = g_fopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: %s", filename);
    return ERR_FILE_OPEN;
  }

  if (content == NULL)
  {
    tifiles_critical("%s: an argument is NULL", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  if(fread_8_chars(f, signature) < 0) goto tfrb;
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;

  if(fread_word(f, NULL) < 0) goto tfrb;
  if(fread_8_chars(f, NULL) < 0) goto tfrb;
  if(fread_n_chars(f, 40, content->comment) < 0) goto tfrb;
  if(fread_word(f, NULL) < 0) goto tfrb;
  if(fread_long(f, NULL) < 0) goto tfrb;
  if(fread_8_chars(f, content->rom_version) < 0) goto tfrb;
  if(fread_byte(f, &(content->type)) < 0) goto tfrb;
  if(fread_byte(f, NULL) < 0) goto tfrb;
  if(fread_word(f, NULL) < 0) goto tfrb;
  if(fread_long(f, &file_size) < 0) goto tfrb;
  content->data_length = file_size - 0x52 - 2;
  if(fread_word(f, NULL) < 0) goto tfrb;

  content->data_part = (uint8_t *)g_malloc0(content->data_length);
  if (content->data_part == NULL) 
  {
    fclose(f);
	tifiles_content_delete_backup(content);
    return ERR_MALLOC;
  }

  if(fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrb;
  if(fread_word(f, &(content->checksum)) < 0) goto tfrb;

  sum = tifiles_checksum(content->data_part, content->data_length);
#if defined(CHECKSUM_ENABLED)
  if(sum != content->checksum)
  {
	  fclose(f);
	  tifiles_content_delete_backup(content);
	  return ERR_FILE_CHECKSUM;
  }
#endif

  fclose(f);
  return 0;

tfrb:	// release on exit
    fclose(f);
	tifiles_content_delete_backup(content);
	return ERR_FILE_IO;
}

static int check_device_type(uint8_t id)
{
	static const uint8_t types[] = { 0, DEVICE_TYPE_89, DEVICE_TYPE_92P };
	int i;

	for(i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
		if(types[i] == id)
			return i;

	return 0;
}

static int check_data_type(uint8_t id)
{
	static const uint8_t types[] = { 0, TI89_AMS, TI89_APPL, TI89_CERTIF, TI89_LICENSE };
	int i;

	for(i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
		if(types[i] == id)
			return i;

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
	int tib = 0;
	char signature[9];

	if (!tifiles_file_is_flash(filename) && !tifiles_file_is_tib(filename))
		return ERR_INVALID_FILE;

	if (head == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	// detect file type (old or new format)
	tib = tifiles_file_is_tib(filename);

	f = g_fopen(filename, "rb");
	if (f == NULL) 
	{
	    tifiles_info("Unable to open this file: %s\n", filename);
		return ERR_FILE_OPEN;
	}  

	if (tib) 
	{	// tib is an old format but mainly used by developers
		memset(content, 0, sizeof(Ti9xFlash));
		if(fseek(f, 0, SEEK_END)) goto tfrf;
		content->data_length = (uint32_t) ftell(f);
		if(fseek(f, 0, SEEK_SET)) goto tfrf;

		strcpy(content->name, "basecode");
		content->data_type = 0x23;	// FLASH os

		content->data_part = (uint8_t *)g_malloc0(content->data_length);
		if (content->data_part == NULL) 
		{
			fclose(f);
			return ERR_MALLOC;
		}

		if(fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;
		switch(content->data_part[8])
		{
		case 1: content->device_type = DEVICE_TYPE_92P; break;	// TI92+
		case 3: content->device_type = DEVICE_TYPE_89; break;	// TI89
		// value added by the TI community according to HWID parameter
		// doesn't have any 'legal' existence.
		case 8: content->device_type = DEVICE_TYPE_92P; break;	// V200PLT
		case 9: content->device_type = DEVICE_TYPE_89; break;	// Titanium
		}

		content->next = NULL;
	} 
	else 
	{
		for (content = head;; content = content->next) 
		{
		    if(fread_8_chars(f, signature) < 0) goto tfrf;
		    content->model = tifiles_file_get_model(filename);
		    if(fread_byte(f, &(content->revision_major)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->revision_minor)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->flags)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->object_type)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->revision_day)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->revision_month)) < 0) goto tfrf;
		    if(fread_word(f, &(content->revision_year)) < 0) goto tfrf;
		    if(fskip(f, 1) < 0) goto tfrf;
		    if(fread_8_chars(f, content->name) < 0) goto tfrf;
		    if(fskip(f, 23) < 0) goto tfrf;
		    if(fread_byte(f, &(content->device_type)) < 0) goto tfrf;
		    if(fread_byte(f, &(content->data_type)) < 0) goto tfrf;
		    if(fskip(f, 23) < 0) goto tfrf;
			if(fread_byte(f, &(content->hw_id)) < 0) goto tfrf;
		    if(fread_long(f, &(content->data_length)) < 0) goto tfrf;

			if(content->data_type != TI89_LICENSE && !check_device_type(content->device_type))
				return ERR_INVALID_FILE;
			if(!check_data_type(content->data_type))
				return ERR_INVALID_FILE;

			content->data_part = (uint8_t *)g_malloc0(content->data_length);
			if (content->data_part == NULL) 
			{
				fclose(f);
				tifiles_content_delete_flash(content);
				return ERR_MALLOC;
			}

			if(fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;
			content->next = NULL;

			// check for end of file
			if(fread_8_chars(f, signature) < 0)
				break;
			if(strcmp(signature, "**TIFL**") || feof(f))
				break;
			if(fseek(f, -8, SEEK_CUR)) goto tfrf;

			content->next = (Ti9xFlash *)g_malloc0(sizeof(Ti9xFlash));
			if (content->next == NULL) 
			{
				fclose(f);
				tifiles_content_delete_flash(content);
				return ERR_MALLOC;
			}
		}
	}

	fclose(f);
	return 0;

tfrf:	// release on exit
    fclose(f);
	tifiles_content_delete_flash(content);
	return ERR_FILE_IO;
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
  FILE *f;
  int i;
  char *filename = NULL;
  uint32_t offset = 0x52;
  int **table;
  int num_folders;
  char default_folder[FLDNAME_MAX];
  char fldname[FLDNAME_MAX], varname[VARNAME_MAX];

  if (content->entries == NULL)
  {
    tifiles_warning("%s: skipping content with NULL content->entries", __FUNCTION__);
    return 0;
  }

  if (fname != NULL)
  {
    filename = g_strdup(fname);
    if (filename == NULL)
      return ERR_MALLOC;
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
      if (real_fname != NULL)
      {
        *real_fname = NULL;
      }
      return 0;
    }
    if (real_fname != NULL)
      *real_fname = g_strdup(filename);
  }

  f = g_fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: %s", filename);
    g_free(filename);
    return ERR_FILE_OPEN;
  }
  g_free(filename);

  // build the table of folder & variable entries  
  table = tifiles_create_table_of_entries((FileContent *)content, &num_folders);
  if (table == NULL)
	  return ERR_MALLOC;

  // write header
  if(fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwr;
  if(fwrite(fsignature, 1, 2, f) < 2) goto tfwr;
  if (content->num_entries == 1)	// folder entry for single var is placed here
    strcpy(content->default_folder, content->entries[0]->folder);
  ticonv_varname_to_tifile_s(content->model, content->default_folder, default_folder, -1);
  if(fwrite_8_chars(f, default_folder) < 0) goto tfwr;
  if(fwrite_n_bytes(f, 40, (uint8_t *)content->comment) < 0) goto tfwr;
  if (content->num_entries > 1) 
  {
    if(fwrite_word(f, (uint16_t) (content->num_entries + num_folders)) < 0) goto tfwr;
    offset += 16 * (content->num_entries + num_folders - 1);
  } 
  else
    if(fwrite_word(f, 1) < 0) goto tfwr;

  // write table of entries
  for (i = 0; table[i] != NULL; i++) 
  {
    VarEntry *fentry;
    int j, idx = table[i][0];
    fentry = content->entries[idx];

    if (fentry == NULL)
    {
      tifiles_warning("%s: skipping null content entry %d", __FUNCTION__, i);
      continue;
    }

    if (content->num_entries > 1)	// single var does not have folder entry
    {
      if(fwrite_long(f, offset) < 0) goto tfwr;
	  ticonv_varname_to_tifile_s(content->model, fentry->folder, fldname, -1);
      if(fwrite_8_chars(f, fldname) < 0) goto tfwr;
      if(fwrite_byte(f, (uint8_t)tifiles_folder_type(content->model)) < 0) goto tfwr;
      if(fwrite_byte(f, 0x00) < 0) goto tfwr;
      for (j = 0; table[i][j] != -1; j++);
      if(fwrite_word(f, (uint16_t) j) < 0) goto tfwr;
    }

    for (j = 0; table[i][j] != -1; j++) 
	{
      int idx2 = table[i][j];
      VarEntry *entry = content->entries[idx2];
	  uint8_t attr = ATTRB_NONE;

      if(fwrite_long(f, offset) < 0) goto tfwr;
	  ticonv_varname_to_tifile_s(content->model, entry->name, varname, entry->type);
      if(fwrite_8_chars(f, varname) < 0) goto tfwr;
      if(fwrite_byte(f, entry->type) < 0) goto tfwr;
      attr = (entry->attr == ATTRB_ARCHIVED) ? 3 : entry->attr;
      if(fwrite_byte(f, attr) < 0) goto tfwr;
      if(fwrite_word(f, 0) < 0) goto tfwr;

      offset += entry->size + 4 + 2;
    }
  }

  if(fwrite_long(f, offset) < 0) goto tfwr;
  if(fwrite_word(f, 0x5aa5) < 0) goto tfwr;

  // write data
  for (i = 0; table[i] != NULL; i++) 
  {
    int j;

    for (j = 0; table[i][j] != -1; j++) 
	{
      int idx = table[i][j];
      VarEntry *entry = content->entries[idx];
      uint16_t sum;

      if(fwrite_long(f, 0) < 0) goto tfwr;
      if(fwrite(entry->data, 1, entry->size, f) < entry->size) goto tfwr;
      sum = tifiles_checksum(entry->data, entry->size);
      if(fwrite_word(f, sum) < 0) goto tfwr;
    }
  }

  // g_free( memory
  for (i = 0; i < num_folders; i++)
    g_free(table[i]);
  g_free(table);

  fclose(f);
  return 0;

tfwr:	// release on exit
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
  FILE *f;

  if (filename == NULL || content == NULL)
  {
    tifiles_critical("%s: an argument is NULL", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  f = g_fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: %s", filename);
    return ERR_FILE_OPEN;
  }

  if(fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwb;
  if(fwrite(fsignature, 1, 2, f) < 2) goto tfwb;
  if(fwrite_8_chars(f, "") < 0) goto tfwb;
  if(fwrite_n_bytes(f, 40, (uint8_t *)content->comment) < 0) goto tfwb;
  if(fwrite_word(f, 1) < 0) goto tfwb;
  if(fwrite_long(f, 0x52) < 0) goto tfwb;
  if(fwrite_8_chars(f, content->rom_version) < 0) goto tfwb;
  if(fwrite_word(f, content->type) < 0) goto tfwb;
  if(fwrite_word(f, 0) < 0) goto tfwb;
  if(fwrite_long(f, content->data_length + 0x52 + 2) < 0) goto tfwb;
  if(fwrite_word(f, 0x5aa5) < 0) goto tfwb;
  if(fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwb;

  content->checksum =
      tifiles_checksum(content->data_part, content->data_length);
  if(fwrite_word(f, content->checksum) < 0) goto tfwb;

  fclose(f);
  return 0;

tfwb:	// release on exit
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
  FILE *f;
  Ti9xFlash *content = head;
  char *filename;

  if (head == NULL)
  {
    tifiles_critical("%s: head is NULL", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  if (fname)
  {
	  filename = g_strdup(fname);
	  if(filename == NULL)
		  return ERR_MALLOC;
  }
  else
  {
	  VarEntry ve;

	  for (content = head; content != NULL; content = content->next)
		if(content->data_type == TI89_AMS || content->data_type == TI89_APPL)
			break;

	  strcpy(ve.name, content->name);
	  ve.type = content->data_type;

	  filename = tifiles_build_filename(content->model, &ve);
	  if (real_fname != NULL)
		*real_fname = g_strdup(filename);
  }

  f = g_fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: %s", filename);
    return ERR_FILE_OPEN;
  }

  for (content = head; content != NULL; content = content->next) 
  {
    if(fwrite_8_chars(f, "**TIFL**") < 0) goto tfwf;
    if(fwrite_byte(f, content->revision_major) < 0) goto tfwf;
    if(fwrite_byte(f, content->revision_minor) < 0) goto tfwf;
    if(fwrite_byte(f, content->flags) < 0) goto tfwf;
    if(fwrite_byte(f, content->object_type) < 0) goto tfwf;
    if(fwrite_byte(f, content->revision_day) < 0) goto tfwf;
    if(fwrite_byte(f, content->revision_month) < 0) goto tfwf;
    if(fwrite_word(f, content->revision_year) < 0) goto tfwf;
    if(fwrite_byte(f, (uint8_t) strlen(content->name)) < 0) goto tfwf;
    if(fwrite_8_chars(f, content->name) < 0) goto tfwf;
    if(fwrite_n_chars(f, 23, "") < 0) goto tfwf;
    if(fwrite_byte(f, content->device_type) < 0) goto tfwf;
    if(fwrite_byte(f, content->data_type) < 0) goto tfwf;
    if(fwrite_n_chars(f, 23, "") < 0) goto tfwf;
	if(fwrite_byte(f, content->hw_id) < 0)  goto tfwf;
    if(fwrite_long(f, content->data_length) < 0) goto tfwf;
    if(fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwf;
  }

  fclose(f);
  return 0;

tfwf:	// release on exit
    fclose(f);
	return ERR_FILE_IO;
}

/**************/
/* Displaying */
/**************/

/**
 * ti9x_content_display_regular:
 * @content: a Ti9xRegular structure.
 *
 * Display fields of a Ti9xRegular structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_content_display_regular(Ti9xRegular *content)
{
  int i;
  char trans[17];

  if (content == NULL)
  {
    tifiles_critical("%s(NULL)", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  tifiles_info("Signature:         %s",
	  tifiles_calctype2signature(content->model));
  tifiles_info("Comment:           %s", content->comment);
  tifiles_info("Default folder:    %s", content->default_folder);
  tifiles_info("Number of entries: %i", content->num_entries);

  for (i = 0; i < content->num_entries /*&& i<5 */ ; i++) 
  {
    tifiles_info("Entry #%i", i);
    tifiles_info("  folder:    %s", content->entries[i]->folder);
    tifiles_info("  name:      %s",
	    ticonv_varname_to_utf8_s(content->model, content->entries[i]->name, 
			trans, content->entries[i]->type));
    tifiles_info("  type:      %02X (%s)",
	    content->entries[i]->type,
	    tifiles_vartype2string(content->model, content->entries[i]->type));
    tifiles_info("  attr:      %s",
	    tifiles_attribute_to_string(content->entries[i]->attr));
    tifiles_info("  length:    %04X (%i)",
	    content->entries[i]->size, content->entries[i]->size);
  }

  tifiles_info("Checksum:    %04X (%i) ", content->checksum,
	  content->checksum);

  return 0;
}

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
  if (content == NULL)
  {
    tifiles_critical("%s(NULL)", __FUNCTION__);
    return ERR_INVALID_FILE;
  }

  tifiles_info("signature:      %s",
	  tifiles_calctype2signature(content->model));
  tifiles_info("comment:        %s", content->comment);
  tifiles_info("ROM version:    %s", content->rom_version);
  tifiles_info("type:           %02X (%s)",
	  content->type, tifiles_vartype2string(content->model, content->type));
  tifiles_info("data length:    %08X (%i)",
	  content->data_length, content->data_length);

  tifiles_info("checksum:       %04X (%i) ", content->checksum,
	  content->checksum);

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
  Ti9xFlash *ptr;

  for (ptr = content; ptr != NULL; ptr = ptr->next) 
  {
    tifiles_info("Signature:      %s",
	    tifiles_calctype2signature(ptr->model));
    tifiles_info("Revision:       %i.%i",
	    ptr->revision_major, ptr->revision_minor);
    tifiles_info("Flags:          %02X", ptr->flags);
    tifiles_info("Object type:    %02X", ptr->object_type);
    tifiles_info("Date:           %02X/%02X/%02X%02X",
	    ptr->revision_day, ptr->revision_month,
	    ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
    tifiles_info("Name:           %s", ptr->name);
    tifiles_info("Device type:    %s",
	    ptr->device_type == DEVICE_TYPE_89 ? "ti89" : "ti92+");
    switch (ptr->data_type) 
	{
    case 0x23:
      tifiles_info("Data type:      OS data");
      break;
    case 0x24:
      tifiles_info("Data type:      APP data");
      break;
	case 0x20:
    case 0x25:
      tifiles_info("Data type:      certificate");
      break;
    case 0x3E:
      tifiles_info("Data type:      license");
      break;
    default:
      tifiles_info("Unknown (mailto tilp-users@lists.sf.net)");
      break;
    }
    tifiles_info("Length:         %08X (%i)", ptr->data_length,
	    ptr->data_length);
    tifiles_info("");
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
  Ti9xRegular *content1;
  Ti9xBackup *content2;
  Ti9xFlash *content3;

  // the testing order is important: regular before backup (due to TI89/92+)
  if (tifiles_file_is_flash(filename) || tifiles_file_is_tib(filename)) 
  {
	content3 = tifiles_content_create_flash(CALC_TI92);
    ti9x_file_read_flash(filename, content3);
    ti9x_content_display_flash(content3);
    tifiles_content_delete_flash(content3);
  } 
  else if (tifiles_file_is_regular(filename)) 
  {
	content1 = tifiles_content_create_regular(CALC_TI92);
    ti9x_file_read_regular(filename, content1);
    ti9x_content_display_regular(content1);
    tifiles_content_delete_regular(content1);
  } 
  else if (tifiles_file_is_backup(filename)) 
  {
	content2 = tifiles_content_create_backup(CALC_TI92);
    ti9x_file_read_backup(filename, content2);
    ti9x_content_display_backup(content2);
    tifiles_content_delete_backup(content2);
  } 
  else
  {
      tifiles_info("Unknown file type !");
      return ERR_BAD_FILE;
  }

  return 0;
}

#endif
