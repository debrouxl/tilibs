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
	TI File Format handling routines
	Calcs: 89/89tm/92/92+/V200
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <ticonv.h>
#include "tifiles.h"
#include "error.h"
#include "logging.h"
#include "macros.h"
#include "typesxx.h"
#include "files9x.h"
#include "rwfile.h"
#include "transcode.h"

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
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti9x_file_read_regular(const char *filename, Ti9xRegular *content)
{
  FILE *f;
  long cur_pos = 0;
  char current_folder[9];
  uint32_t curr_offset = 0;
  uint32_t next_offset = 0;
  uint16_t tmp;
  int i, j;
  char signature[9];

  if (!tifiles_file_is_regular(filename))
    return ERR_INVALID_FILE;

  f = gfopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;
  fread_word(f, NULL);
  fread_8_chars(f, content->default_folder);
  strcpy(current_folder, content->default_folder);
  fread_n_chars(f, 40, content->comment);
  fread_word(f, &tmp);
  content->num_entries = tmp;

  content->entries = calloc(content->num_entries + 1, sizeof(VarEntry*));
  if (content->entries == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }

  for (i = 0, j = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = content->entries[j] = calloc(1, sizeof(VarEntry));

    fread_long(f, &curr_offset);
    fread_8_chars(f, entry->name);
    fread_byte(f, &(entry->type));
    fread_byte(f, &(entry->attr));
    fread_word(f, NULL);

    if (entry->type == TI92_DIR) // same as TI89_DIR, TI89t_DIR, ...
	{
      strcpy(current_folder, entry->name);
      free(entry);
      continue;			// folder: skip entry
    } 
	else 
	{
	  uint16_t checksum, sum = 0;

      j++;
      strcpy(entry->folder, current_folder);
      cur_pos = ftell(f);
      fread_long(f, &next_offset);
      entry->size = next_offset - curr_offset - 4 - 2;
      entry->data = (uint8_t *) calloc(entry->size, 1);
      if (entry->data == NULL) 
	  {
		fclose(f);
		return ERR_MALLOC;
      }

      fseek(f, curr_offset, SEEK_SET);
      fread_long(f, NULL);	// 4 bytes (NULL)
      fread(entry->data, entry->size, 1, f);

      fread_word(f, &checksum);
      fseek(f, cur_pos, SEEK_SET);

	  sum = tifiles_checksum(entry->data, entry->size);
	  if(sum != checksum) 
	    return ERR_FILE_CHECKSUM;
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
}

/**
 * ti9x_file_read_backup:
 * @filename: name of backup file to open.
 * @content: where to store the file content.
 *
 * Load the backup file into a Ti9xBackup structure.
 *
 * Structure content must be freed with #tifiles_content_delete_backup when
 * no longer used.
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

  f = gfopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;
  fread_word(f, NULL);
  fread_8_chars(f, NULL);
  fread_n_chars(f, 40, content->comment);
  fread_word(f, NULL);
  fread_long(f, NULL);
  fread_8_chars(f, content->rom_version);
  fread_byte(f, &(content->type));
  fread_byte(f, NULL);
  fread_word(f, NULL);
  fread_long(f, &file_size);
  content->data_length = file_size - 0x52 - 2;
  fread_word(f, NULL);

  content->data_part = (uint8_t *) calloc(content->data_length, 1);
  if (content->data_part == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }
  fread(content->data_part, 1, content->data_length, f);

  fread_word(f, &(content->checksum));

  sum = tifiles_checksum(content->data_part, content->data_length);
  if(sum != content->checksum)
	  return ERR_FILE_CHECKSUM;

  fclose(f);

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
 * no longer used.
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

	// detect file type (old or new format)
	tib = tifiles_file_is_tib(filename);

	f = gfopen(filename, "rb");
	if (f == NULL) 
	{
	    tifiles_info("Unable to open this file: <%s>\n", filename);
		return ERR_FILE_OPEN;
	}  

	if (tib) 
	{	// tib is an old format but mainly used by developers
		memset(content, 0, sizeof(Ti9xFlash));
		fseek(f, 0, SEEK_END);
		content->data_length = (uint32_t) ftell(f);
		fseek(f, 0, SEEK_SET);

		strcpy(content->name, "basecode");
		content->data_type = 0x23;	// FLASH os

		content->data_part = (uint8_t *) calloc(content->data_length, 1);
		if (content->data_part == NULL) 
		{
			fclose(f);
			return ERR_MALLOC;
		}
		fread(content->data_part, content->data_length, 1, f);

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
		    fread_8_chars(f, signature);
		    content->model = tifiles_file_get_model(filename);
		    fread_byte(f, &(content->revision_major));
		    fread_byte(f, &(content->revision_minor));
		    fread_byte(f, &(content->flags));
		    fread_byte(f, &(content->object_type));
		    fread_byte(f, &(content->revision_day));
		    fread_byte(f, &(content->revision_month));
		    fread_word(f, &(content->revision_year));
		    fskip(f, 1);
		    fread_8_chars(f, content->name);
		    fskip(f, 23);
		    fread_byte(f, &(content->device_type));
		    fread_byte(f, &(content->data_type));
		    fskip(f, 24);
		    fread_long(f, &(content->data_length));

			content->data_part = (uint8_t *) calloc(content->data_length, 1);
			if (content->data_part == NULL) 
			{
				fclose(f);
				return ERR_MALLOC;
			}
			fread(content->data_part, content->data_length, 1, f);

			content->next = NULL;

			// check for end of file
			fread_8_chars(f, signature);
			if(strcmp(signature, "**TIFL**") || feof(f))
				break;
			fseek(f, -8, SEEK_CUR);

			content->next = (Ti9xFlash *) calloc(1, sizeof(Ti9xFlash));
			if (content->next == NULL) 
			{
				fclose(f);
				return ERR_MALLOC;
			}
		}
	}

	fclose(f);

	return 0;
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
  char basename[64];
  uint32_t offset = 0x52;
  int **table;
  int num_folders;

  if (fname != NULL) 
  {
    filename = strdup(fname);
    if (filename == NULL)
      return ERR_MALLOC;
  } 
  else 
  {
	ticonv_varname_to_filename_s(content->model, content->entries[0]->name, basename);

    filename = (char *) malloc(strlen(basename) + 1 + 5 + 1);
    strcpy(filename, basename);
    strcat(filename, ".");
    strcat(filename, tifiles_vartype2fext(content->model, content->entries[0]->type));
    if (real_fname != NULL)
      *real_fname = strdup(filename);
  }

  f = gfopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    free(filename);
    return ERR_FILE_OPEN;
  }
  free(filename);

  // build the table of folder & variable entries  
  table = tifiles_create_table_of_entries((FileContent *)content, &num_folders);
  if (table == NULL)
	  return ERR_MALLOC;

  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->model));
  fwrite(fsignature, 1, 2, f);
  if (content->num_entries == 1)	// folder entry for single var is placed here
    strcpy(content->default_folder, content->entries[0]->folder);
  fwrite_8_chars(f, content->default_folder);
  fwrite_n_bytes(f, 40, content->comment);
  if (content->num_entries > 1) 
  {
    fwrite_word(f, (uint16_t) (content->num_entries + num_folders));
    offset += 16 * (content->num_entries + num_folders - 1);
  } 
  else
    fwrite_word(f, 1);

  // write table of entries
  for (i = 0; table[i] != NULL; i++) 
  {
    VarEntry *fentry;
    int j, index = table[i][0];
    fentry = content->entries[index];

    if (content->num_entries > 1)	// single var does not have folder entry
    {
      fwrite_long(f, offset);
      fwrite_8_chars(f, fentry->folder);
      fwrite_byte(f, (uint8_t)tifiles_folder_type(content->model));
      fwrite_byte(f, 0x00);
      for (j = 0; table[i][j] != -1; j++);
      fwrite_word(f, (uint16_t) j);
    }

    for (j = 0; table[i][j] != -1; j++) 
	{
      int index = table[i][j];
      VarEntry *entry = content->entries[index];

      fwrite_long(f, offset);
      fwrite_8_chars(f, entry->name);
      fwrite_byte(f, entry->type);
      fwrite_byte(f, entry->attr);
      fwrite_word(f, 0);

      offset += entry->size + 4 + 2;
    }
  }

  fwrite_long(f, offset);
  fwrite_word(f, 0x5aa5);

  // write data
  for (i = 0; table[i] != NULL; i++) 
  {
    int j;

    for (j = 0; table[i][j] != -1; j++) 
	{
      int index = table[i][j];
      VarEntry *entry = content->entries[index];
      uint16_t sum;

      fwrite_long(f, 0);
      fwrite(entry->data, entry->size, 1, f);
      sum = tifiles_checksum(entry->data, entry->size);
      fwrite_word(f, sum);
    }
  }

  // free memory
  for (i = 0; i < num_folders; i++)
    free(table[i]);
  free(table);

  fclose(f);

  return 0;
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

  f = gfopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }

  fwrite_8_chars(f, tifiles_calctype2signature(content->model));
  fwrite(fsignature, 1, 2, f);
  fwrite_8_chars(f, "");
  fwrite_n_bytes(f, 40, content->comment);
  fwrite_word(f, 1);
  fwrite_long(f, 0x52);
  fwrite_8_chars(f, content->rom_version);
  fwrite_word(f, content->type);
  fwrite_word(f, 0);
  fwrite_long(f, content->data_length + 0x52 + 2);
  fwrite_word(f, 0x5aa5);
  fwrite(content->data_part, 1, content->data_length, f);

  content->checksum =
      tifiles_checksum(content->data_part, content->data_length);

  fwrite_word(f, content->checksum);

  fclose(f);

  return 0;
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
int ti9x_file_write_flash(const char *filename, Ti9xFlash *head)
{
  FILE *f;
  Ti9xFlash *content = head;

  f = gfopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }

  for (content = head; content != NULL; content = content->next) 
  {
    fwrite_8_chars(f, "**TIFL**");
    fwrite_byte(f, content->revision_major);
    fwrite_byte(f, content->revision_minor);
    fwrite_byte(f, content->flags);
    fwrite_byte(f, content->object_type);
    fwrite_byte(f, content->revision_day);
    fwrite_byte(f, content->revision_month);
    fwrite_word(f, content->revision_year);
    fwrite_byte(f, (uint8_t) strlen(content->name));
    fwrite_8_chars(f, content->name);
    fwrite_n_chars(f, 23, "");
    fwrite_byte(f, content->device_type);
    fwrite_byte(f, content->data_type);
    fwrite_n_chars(f, 24, "");
    fwrite_long(f, content->data_length);
    fwrite(content->data_part, content->data_length, 1, f);
  }

  fclose(f);

  return 0;
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

  tifiles_info("Signature:         <%s>",
	  tifiles_calctype2signature(content->model));
  tifiles_info("Comment:           <%s>", content->comment);
  tifiles_info("Default folder:    <%s>", content->default_folder);
  tifiles_info("Number of entries: %i", content->num_entries);

  for (i = 0; i < content->num_entries /*&& i<5 */ ; i++) 
  {
    tifiles_info("Entry #%i", i);
    tifiles_info("  folder:    <%s>", content->entries[i]->folder);
    tifiles_info("  name:      <%s>",
	    ticonv_varname_to_utf8_s(content->model, content->entries[i]->name, trans, content->entries[i]->type));
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
  tifiles_info("signature:      <%s>",
	  tifiles_calctype2signature(content->model));
  tifiles_info("comment:        <%s>", content->comment);
  tifiles_info("ROM version:    <%s>", content->rom_version);
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
    tifiles_info("Signature:      <%s>",
	    tifiles_calctype2signature(ptr->model));
    tifiles_info("Revision:       %i.%i",
	    ptr->revision_major, ptr->revision_minor);
    tifiles_info("Flags:          %02X", ptr->flags);
    tifiles_info("Object type:    %02X", ptr->object_type);
    tifiles_info("Date:           %02X/%02X/%02X%02X",
	    ptr->revision_day, ptr->revision_month,
	    ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
    tifiles_info("Name:           <%s>", ptr->name);
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
      tifiles_info("Unknown (mailto roms@lpg.ticalc.org)");
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
