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
  TI File Format handling routines
  Calcs: 89/92/92+/V200
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "file_int.h"
#include "file_err.h"
#include "file_def.h"
#include "typesxx.h"
#include "filesxx.h"
#include "misc.h"
#include "macros.h"
#include "trans.h"
#include "printl.h"

static int fsignature[2] = { 1, 0 };

/**************/
/* Allocating */
/**************/

TIEXPORT Ti9xRegular *TICALL ti9x_create_regular_content(void)
{
  Ti9xRegular *content = (Ti9xRegular *) calloc(1, sizeof(Ti9xRegular));

  return content;
}

TIEXPORT Ti9xBackup *TICALL ti9x_create_backup_content(void)
{
  Ti9xBackup *content = (Ti9xBackup *) calloc(1, sizeof(Ti9xBackup));

  return content;
}

TIEXPORT Ti9xFlash *TICALL ti9x_create_flash_content(void)
{
  Ti9xFlash *content = (Ti9xFlash *) calloc(1, sizeof(Ti9xFlash));
  time_t tt;
  struct tm *lt;

  time(&tt);
  lt = localtime(&tt);
  content->revision_major = 1;
  content->revision_minor = 0;
  content->flags = 0;
  content->object_type = 0;
  content->revision_day = lt->tm_mday;
  content->revision_month = lt->tm_mon;
  content->revision_year = lt->tm_year + 1900;

  return content;
}

/*************************/
/* Copying (duplicating) */
/*************************/

/*
  Copy an Ti9xVarEntry structure (data included)
*/
int ti9x_dup_VarEntry(Ti9xVarEntry * dst, Ti9xVarEntry * src)
{
  memcpy(dst, src, sizeof(Ti9xVarEntry));

  dst->data = (uint8_t *) calloc(dst->size, 1);
  if (dst->data == NULL)
    return ERR_MALLOC;
  memcpy(dst->data, src->data, dst->size);

  return 0;
}


/*
  Copy an Ti9xRegular structure
*/
int ti9x_dup_Regular(Ti9xRegular * dst, Ti9xRegular * src)
{
  int i;

  memcpy(dst, src, sizeof(Ti9xRegular));

  dst->entries = (Ti9xVarEntry *) calloc(src->num_entries,
					 sizeof(Ti9xVarEntry));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < src->num_entries; i++)
    TRY(ti9x_dup_VarEntry(&(dst->entries[i]), &(src->entries[i])));

  return 0;
}

/*
  Copy an Ti9xBackup structure
*/
int ti9x_dup_Backup(Ti9xBackup * dst, Ti9xBackup * src)
{
  memcpy(dst, src, sizeof(Ti9xBackup));

  dst->data_part = (uint8_t *) calloc(dst->data_length, 1);
  if (dst->data_part == NULL)
    return ERR_MALLOC;

  memcpy(dst->data_part, src->data_part, dst->data_length);

  return 0;
}

/*
  Copy an Ti9xFlash structure
*/
int ti9x_dup_Flash(Ti9xFlash * dst, Ti9xFlash * src)
{
  // to do...

  return 0;
}

/***********/
/* Freeing */
/***********/

/*
  Free the content of a Ti9xRegular structure
*/
TIEXPORT int TICALL ti9x_free_regular_content(Ti9xRegular * content)
{
  int i;

  for (i = 0; i < content->num_entries; i++) {
    Ti9xVarEntry *entry = &(content->entries[i]);
    free(entry->data);
  }
  free(content->entries);

  return 0;
}

TIEXPORT int TICALL ti9x_free_backup_content(Ti9xBackup * content)
{
  free(content->data_part);
  return 0;
}

TIEXPORT int TICALL ti9x_free_flash_content(Ti9xFlash * content)
{
  Ti9xFlash *ptr;

  free(content->data_part);

  ptr = content->next;
  while (ptr != NULL) {
    Ti9xFlash *next = ptr->next;

    free(ptr->data_part);
    free(ptr);

    ptr = next;
  }

  return 0;
}

/***********/
/* Reading */
/***********/

/*
  Open a file and place its content in a Ti9xRegular structure
  - filename [in]: a file to read
  - content [out]: the address of a structure where the file content 
  will be stored
  - int [out]: an error code
 */
TIEXPORT int TICALL ti9x_read_regular_file(const char *filename,
					   Ti9xRegular * content)
{
  FILE *f;
  long cur_pos = 0;
  char current_folder[9];
  uint32_t curr_offset = 0;
  uint32_t next_offset = 0;
  uint16_t tmp;
  int i, j;
  char signature[9];

  if (!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;

  if (!tifiles_is_a_regular_file(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->calc_type = tifiles_signature2calctype(signature);
  if (content->calc_type == CALC_NONE)
    return ERR_INVALID_FILE;
  fread_word(f, NULL);
  fread_8_chars(f, content->default_folder);
  strcpy(current_folder, content->default_folder);
  fread_n_chars(f, 40, content->comment);
  fread_word(f, &tmp);
  content->num_entries = tmp;

  content->entries = (Ti9xVarEntry *) calloc(content->num_entries,
					     sizeof(Ti9xVarEntry));
  if (content->entries == NULL) {
    fclose(f);
    return ERR_MALLOC;
  }

  for (i = 0, j = 0; i < content->num_entries; i++) {
    Ti9xVarEntry *entry = &((content->entries)[j]);

    fread_long(f, &curr_offset);
    fread_8_chars(f, entry->name);
    tixx_translate_varname(entry->name, entry->trans,
			   entry->type, content->calc_type);
    fread_byte(f, &(entry->type));
    fread_byte(f, &(entry->attr));
    fread_word(f, NULL);

    if (entry->type == tifiles_folder_type()) {
      strcpy(current_folder, entry->name);
      continue;			// folder: skip entry
    } else {
      j++;
      strcpy(entry->folder, current_folder);
      cur_pos = ftell(f);
      fread_long(f, &next_offset);
      entry->size = next_offset - curr_offset - 4 - 2;
      entry->data = (uint8_t *) calloc(entry->size, 1);
      if (entry->data == NULL) {
	fclose(f);
	return ERR_MALLOC;
      }

      fseek(f, curr_offset, SEEK_SET);
      fread_long(f, NULL);	// 4 bytes (NULL)
      fread(entry->data, entry->size, 1, f);
      fread_word(f, NULL);	//checksum
      fseek(f, cur_pos, SEEK_SET);
    }
  }
  content->num_entries = j;
  content->entries = (Ti9xVarEntry *) realloc(content->entries,
					      content->num_entries *
					      sizeof(Ti9xVarEntry));
  fread_long(f, &next_offset);
  fseek(f, next_offset - 2, SEEK_SET);
  fread_word(f, &(content->checksum));

  fclose(f);

  return 0;
}

/*
  Open a file and place its content in a structure
  - filename [in]: a file to read
  - content [out]: the address of a structure where the file content
  will be stored
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_read_backup_file(const char *filename,
					  Ti9xBackup * content)
{
  FILE *f;
  uint32_t file_size;
  char signature[9];

  if (!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;
  if (!tifiles_is_a_backup_file(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->calc_type = tifiles_signature2calctype(signature);
  if (content->calc_type == CALC_NONE)
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
  if (content->data_part == NULL) {
    fclose(f);
    return ERR_MALLOC;
  }
  fread(content->data_part, 1, content->data_length, f);

  fread_word(f, &(content->checksum));

  fclose(f);

  return 0;
}

// tib support not tested yet
TIEXPORT int TICALL ti9x_read_flash_file(const char *filename,
					 Ti9xFlash * head)
{
  int tib = 0;
  FILE *f;
  int j;
  char buf[128];
  Ti9xFlash *content = head;
  char signature[9];

  if (!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;
  if (!tifiles_is_a_flash_file(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  if (strcmp(signature, "**TIFL**"))
    return ERR_INVALID_FILE;

  // detect file type (old or new format)
  fgets(buf, 128, f);
  tib = (int) strstr(buf, "Advanced Mathematics Software");
  rewind(f);

  if (tib) {			// tib is an old format
    fseek(f, 0, SEEK_END);
    content->data_length = (uint32_t) ftell(f);
    fseek(f, 0, SEEK_SET);
    strcpy(content->name, "basecode");
  } else {
    long file_size;

    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    for (content = head;; content = content->next) {
      fread_8_chars(f, signature);
      content->calc_type = tifiles_which_calc_type(filename);
      fread_byte(f, &(content->revision_major));
      fread_byte(f, &(content->revision_minor));
      fread_byte(f, &(content->flags));
      fread_byte(f, &(content->object_type));
      fread_byte(f, &(content->revision_day));
      fread_byte(f, &(content->revision_month));
      fread_word(f, &(content->revision_year));
      fread_byte(f, NULL);
      fread_8_chars(f, content->name);
      for (j = 0; j < 23; j++)
	fgetc(f);
      fread_byte(f, &(content->device_type));
      fread_byte(f, &(content->data_type));
      for (j = 0; j < 24; j++)
	fgetc(f);
      fread_long(f, &(content->data_length));
      content->data_part = (uint8_t *) calloc(content->data_length, 1);
      if (content->data_part == NULL) {
	fclose(f);
	return ERR_MALLOC;
      }
      fread(content->data_part, content->data_length, 1, f);

      content->next = NULL;
      if (file_size == ftell(f))
	break;

      content->next = (Ti9xFlash *) calloc(1, sizeof(Ti9xFlash));
      if (content->next == NULL) {
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

/*
  Write the content of the structure into the file.
  If the filename is NULL, the function uses the name as filename.
  - filename [in]: a file to write
  - content [in]: the address of a structure
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_write_regular_file(const char *fname,
					    Ti9xRegular * content,
					    char **real_fname)
{
  FILE *f;
  int i;
  char *filename = NULL;
  char trans[17];
  uint32_t offset = 0x52;
  int **table;
  int num_folders;

  if (fname != NULL) {
    filename = strdup(fname);
    if (filename == NULL)
      return ERR_MALLOC;
  } else {
    tixx_translate_varname(content->entries[0].name, trans,
			   content->entries[0].type, content->calc_type);

    filename = (char *) malloc(strlen(trans) + 1 + 5 + 1);
    strcpy(filename, trans);
    strcat(filename, ".");
    strcat(filename, tifiles_vartype2file(content->entries[0].type));
    if (real_fname != NULL)
      *real_fname = strdup(filename);
  }

  f = fopen(filename, "wb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    free(filename);
    return ERR_FILE_OPEN;
  }
  free(filename);

  // build the table of folder & variable entries  
  TRY(tifiles_create_table_of_entries(content, &table, &num_folders));

  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->calc_type));
  fwrite(fsignature, 1, 2, f);
  if (content->num_entries == 1)	// folder entry for single var is placed here
    strcpy(content->default_folder, content->entries[0].folder);
  fwrite_8_chars(f, content->default_folder);
  fwrite_n_chars(f, 40, content->comment);
  if (content->num_entries > 1) {
    fwrite_word(f, (uint16_t) (content->num_entries + num_folders));
    offset += 16 * (content->num_entries + num_folders - 1);
  } else
    fwrite_word(f, 1);

  // write table of entries
  for (i = 0; table[i] != NULL; i++) {
    Ti9xVarEntry *fentry;
    int j, index = table[i][0];
    fentry = &(content->entries[index]);

    if (content->num_entries > 1)	// single var does not have folder entry
    {
      fwrite_long(f, offset);
      fwrite_8_chars(f, fentry->folder);
      fwrite_byte(f, (uint8_t)tifiles_folder_type());
      fwrite_byte(f, 0x00);
      for (j = 0; table[i][j] != -1; j++);
      fwrite_word(f, (uint16_t) j);
    }

    for (j = 0; table[i][j] != -1; j++) {
      int index = table[i][j];
      Ti9xVarEntry *entry = &(content->entries[index]);

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
  for (i = 0; table[i] != NULL; i++) {
    int j;

    for (j = 0; table[i][j] != -1; j++) {
      int index = table[i][j];
      Ti9xVarEntry *entry = &(content->entries[index]);
      uint16_t sum;

      fwrite_long(f, 0);
      fwrite(entry->data, entry->size, 1, f);
      sum = tifiles_compute_checksum(entry->data, entry->size);
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

/*
  Write the content of the structure into the file
  - filename [in]: a file to write
  - content [in]: the address of a structure
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_write_backup_file(const char *filename,
					   Ti9xBackup * content)
{
  FILE *f;

  f = fopen(filename, "wb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  fwrite_8_chars(f, tifiles_calctype2signature(content->calc_type));
  fwrite(fsignature, 1, 2, f);
  fwrite_8_chars(f, "");
  fwrite_n_chars(f, 40, content->comment);
  fwrite_word(f, 1);
  fwrite_long(f, 0x52);
  fwrite_8_chars(f, content->rom_version);
  fwrite_word(f, content->type);
  fwrite_word(f, 0);
  fwrite_long(f, content->data_length + 0x52 + 2);
  fwrite_word(f, 0x5aa5);
  fwrite(content->data_part, 1, content->data_length, f);

  content->checksum =
      tifiles_compute_checksum(content->data_part, content->data_length);

  fwrite_word(f, content->checksum);

  fclose(f);

  return 0;
}

TIEXPORT int TICALL ti9x_write_flash_file(const char *filename,
					  Ti9xFlash * head)
{
  FILE *f;
  Ti9xFlash *content = head;

  f = fopen(filename, "wb");
  if (f == NULL) {
    printl3(0, "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  for (content = head; content != NULL; content = content->next) {
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

  return 0;
}

/**************/
/* Displaying */
/**************/

/* 
   Display the characteristics of a TI regular content
   - content [in]: the content to show
   - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_display_regular_content(Ti9xRegular * content)
{
  int i;
  char trans[17];

  printl3(0, "Signature:         <%s>\n",
	  tifiles_calctype2signature(content->calc_type));
  printl3(0, "Comment:           <%s>\n", content->comment);
  printl3(0, "Default folder:    <%s>\n", content->default_folder);
  printl3(0, "Number of entries: %i\n", content->num_entries);

  for (i = 0; i < content->num_entries /*&& i<5 */ ; i++) {
    printl3(0, "Entry #%i\n", i);
    printl3(0, "  folder:    <%s>\n", content->entries[i].folder);
    printl3(0, "  name:      <%s>\n",
	    tixx_translate_varname(content->entries[i].name,
				   trans,
				   content->entries[i].type,
				   content->calc_type));
    printl3(0, "  type:      %02X (%s)\n",
	    content->entries[i].type,
	    tifiles_vartype2string(content->entries[i].type));
    printl3(0, "  attr:      %s\n",
	    tifiles_attribute_to_string(content->entries[i].attr));
    printl3(0, "  length:    %04X (%i)\n",
	    content->entries[i].size, content->entries[i].size);
  }

  printl3(0, "Checksum:    %04X (%i) \n", content->checksum,
	  content->checksum);

  return 0;
}

/*
  Display the characteristics of a TI backup content
  - content [in]: the content to show
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_display_backup_content(Ti9xBackup * content)
{
  printl3(0, "signature:      <%s>\n",
	  tifiles_calctype2signature(content->calc_type));
  printl3(0, "comment:        <%s>\n", content->comment);
  printl3(0, "ROM version:    <%s>\n", content->rom_version);
  printl3(0, "type:           %02X (%s)\n",
	  content->type, tifiles_vartype2string(content->type));
  printl3(0, "data length:    %08X (%i)\n",
	  content->data_length, content->data_length);

  printl3(0, "checksum:       %04X (%i) \n", content->checksum,
	  content->checksum);

  return 0;
}

/*
  Display the characteristics of a TI flash content
  - content [in]: the content to show
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_display_flash_content(Ti9xFlash * content)
{
  Ti9xFlash *ptr;

  for (ptr = content; ptr != NULL; ptr = ptr->next) {
    printl3(0, "Signature:      <%s>\n",
	    tifiles_calctype2signature(ptr->calc_type));
    printl3(0, "Revision:       %i.%i\n",
	    ptr->revision_major, ptr->revision_minor);
    printl3(0, "Flags:          %02X\n", ptr->flags);
    printl3(0, "Object type:    %02X\n", ptr->object_type);
    printl3(0, "Date:           %02X/%02X/%02X%02X\n",
	    ptr->revision_day, ptr->revision_month,
	    ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
    printl3(0, "Name:           <%s>\n", ptr->name);
    printl3(0, "Device type:    %s\n",
	    ptr->device_type == DEVICE_TYPE_89 ? "ti89" : "ti92+");
    printl3(0, "Data type:      ");
    switch (ptr->data_type) {
    case 0x23:
      printl3(0, "OS data\n");
      break;
    case 0x24:
      printl3(0, "APP data\n");
      break;
    case 0x25:
      printl3(0, "certificate\n");
      break;
    case 0x3E:
      printl3(0, "license\n");
      break;
    default:
      printl3(0, "Unknown (mailto roms@lpg.ticalc.org)\n");
      break;
    }
    printl3(0, "Length:         %08X (%i)\n", ptr->data_length,
	    ptr->data_length);
    printl3(0, "\n");
  }

  return 0;
}

/*
  Display the internal characteristics of a TI file:
  - filename [in]: the file to stat
  - int [out]: an error code
*/
TIEXPORT int TICALL ti9x_display_file(const char *filename)
{
  Ti9xRegular content1;
  Ti9xBackup content2;
  Ti9xFlash content3;

  // the testing order is important: regular before backup (due to TI89/92+)
  if (tifiles_is_a_flash_file(filename)) {
    ti9x_read_flash_file(filename, &content3);
    ti9x_display_flash_content(&content3);
    ti9x_free_flash_content(&content3);
  } else if (tifiles_is_a_regular_file(filename)) {
    ti9x_read_regular_file(filename, &content1);
    ti9x_display_regular_content(&content1);
    ti9x_free_regular_content(&content1);
  } else if (tifiles_is_a_backup_file(filename)) {
    ti9x_read_backup_file(filename, &content2);
    ti9x_display_backup_content(&content2);
    ti9x_free_backup_content(&content2);
  } else
    printl3(0, "Unknwon file type !\n");

  return 0;
}
