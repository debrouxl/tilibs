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
	Calcs: 73/82/83/83+/84+/85/86
*/

/*
	Thanks to Adrian Mettler <amettler@hmc.edu> for his patch which fixes
	some TI85/86 file issues (padded, not padded).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tifiles.h"
#include "error.h"
#include "logging.h"
#include "typesxx.h"
#include "files8x.h"
#include "rwfile.h"
#include "intelhex.h"
#include "transcode.h"

/********/
/* Misc */
/********/

static uint8_t fsignature85[3] = { 0x1A, 0x0C, 0x00 };	//TI85
static uint8_t fsignature8x[3] = { 0x1A, 0x0A, 0x00 };	//TI82, 83, 86


static int is_ti8586(TiCalcModel model)
{
  return ((model == CALC_TI85) || (model == CALC_TI86));
}

static int is_ti83p(TiCalcModel model)
{
  return (model == CALC_TI83P) || (model == CALC_TI84P);
}

/**************/
/* Allocating */
/**************/

/**
 * ti8x_create_regular_content:
 *
 * Allocates a #Ti8xRegular structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT Ti8xRegular *TICALL ti8x_create_regular_content(void)
{
	return (Ti8xRegular *) calloc(1, sizeof(Ti8xRegular));
}

/**
 * ti8x_create_backup_content:
 *
 * Allocates a #Ti8xBackup structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT Ti8xBackup *TICALL ti8x_create_backup_content(void)
{
	return (Ti8xBackup *) calloc(1, sizeof(Ti8xBackup));
}

/**
 * ti8x_create_flash_content:
 *
 * Allocates a #Ti8xFlashr structure.
 *
 * Return value: the allocated block.
 **/
TIEXPORT Ti8xFlash *TICALL ti8x_create_flash_content(void)
{
	return (Ti8xFlash *) calloc(1, sizeof(Ti8xFlash));
}

/*************************/
/* Copying (duplicating) */
/*************************/

/*
  Copy an Ti8xVarEntry structure (data included).
  Memory must be freed when no longer used.
*/
int ti8x_dup_VarEntry(Ti8xVarEntry *dst, Ti8xVarEntry *src)
{
  memcpy(dst, src, sizeof(Ti8xVarEntry));

  dst->data = (uint8_t *) calloc(dst->size, 1);
  if (dst->data == NULL)
    return ERR_MALLOC;
  memcpy(dst->data, src->data, dst->size);

  return 0;
}

/*
  Copy an Ti8xRegular structure.
  Memory must be freed when no longer used.
*/
int ti8x_dup_Regular(Ti8xRegular *dst, Ti8xRegular *src)
{
  int i;

  memcpy(dst, src, sizeof(Ti8xRegular));

  dst->entries = (Ti8xVarEntry *) calloc(src->num_entries,
					 sizeof(Ti8xVarEntry));
  if (dst->entries == NULL)
    return ERR_MALLOC;

  for (i = 0; i < src->num_entries; i++)
    TRY(ti8x_dup_VarEntry(&(dst->entries[i]), &(src->entries[i])));

  return 0;
}

/*
  Copy an Ti8xBackup structure.
  Memory must be freed when no longer used.
*/
int ti8x_dup_Backup(Ti8xBackup *dst, Ti8xBackup *src)
{
  memcpy(dst, src, sizeof(Ti8xBackup));

  dst->data_part1 = (uint8_t *) calloc(dst->data_length1, 1);
  dst->data_part2 = (uint8_t *) calloc(dst->data_length2, 1);
  dst->data_part3 = (uint8_t *) calloc(dst->data_length3, 1);
  dst->data_part4 = (uint8_t *) calloc(dst->data_length4, 1);

  if ((dst->data_part1 == NULL) ||
      (dst->data_part2 == NULL) ||
      (dst->data_part3 == NULL) || 
	  (dst->data_part4 == NULL))
    return ERR_MALLOC;

  memcpy(dst->data_part1, src->data_part1, dst->data_length1);
  memcpy(dst->data_part2, src->data_part2, dst->data_length2);
  memcpy(dst->data_part3, src->data_part3, dst->data_length3);
  memcpy(dst->data_part4, src->data_part4, dst->data_length4);

  return 0;
}

/***********/
/* Freeing */
/***********/

/**
 * ti8x_content_free_regular:
 *
 * Free the whole content of a #Ti8xRegular structure.
 *
 * Return value: none.
 **/
TIEXPORT void TICALL ti8x_content_free_regular(Ti8xRegular *content)
{
  int i;

  for (i = 0; i < content->num_entries; i++) 
  {
    Ti8xVarEntry *entry = &(content->entries[i]);
    free(entry->data);
  }
  free(content->entries);
}

/**
 * ti8x_content_free_backup:
 *
 * Free the whole content of a Ti8xBackup structure.
 *
 * Return value: none.
 **/
TIEXPORT void TICALL ti8x_content_free_backup(Ti8xBackup *content)
{
  free(content->data_part1);
  free(content->data_part2);
  free(content->data_part3);
  free(content->data_part4);
}

/**
 * ti8x_content_free_flash:
 *
 * Free the whole content of a Ti8xFlash structure.
 *
 * Return value: none.
 **/
TIEXPORT void TICALL ti8x_content_free_flash(Ti8xFlash *content)
{
    int i;

    for(i = 0; i < content->num_pages; i++)
	free(content->pages[i].data);
    free(content->pages);
}

/***********/
/* Reading */
/***********/

/**
 * ti8x_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a Ti8xRegular structure.
 *
 * Structure content must be freed with #ti8x_content_free_regular when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_read_regular(const char *filename, Ti8xRegular *content)
{
  FILE *f;
  uint16_t tmp = 0x000B;
  long offset = 0;
  int i;
  int ti83p_flag = 0;
  uint8_t name_length = 8;	// ti85/86 only
  uint16_t data_size;
  char signature[9];

  if (!tifiles_file_is_ti(filename))
    return ERR_INVALID_FILE;
  if (!tifiles_file_is_regular(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_warning( "Unable to open this file: <%s>\n", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;
  fskip(f, 3);
  fread_n_chars(f, 42, content->comment);
  fread_word(f, &data_size);

  // search for the number of entries by parsing the whole file
  offset = ftell(f);
  for (i = 0;; i++) 
  {
    int current_offset = (int)ftell(f);
    /* We are done finding entries once we reach the end of the data segment
     * as defined in the header.  This works better than magic numbers, as
     * as there exist files in the wild with incorrect magic numbers that
     * transmit correctly with TI's software and with this code.
     *   Adrian Mettler
     */
	  if (current_offset >= offset + data_size)
	    break;

    fread_word(f, &tmp);
    if (tmp == 0x0D)
      ti83p_flag = !0;		// true TI83+ file (2 extra bytes)
    if (is_ti8586(content->model)) 
	{
      /* name may follow one of three conventions: padded with SPC bytes
       * (most correct, generated by TI's software), padded with NULL bytes,
       * or unpadded.  TI's software accepts all three, so we should too.
       */
       fskip(f, 3);
       fread_byte(f, &name_length);
       fskip(f, content->model == CALC_TI85 ? name_length : 8);
    }
    else if ((content->model == CALC_TI83P) || (content->model == CALC_TI84P))
      fskip(f, 13);
    else
      fskip(f, 11);
    fread_word(f, &tmp);
    fskip(f, tmp);
  }
  fseek(f, offset, SEEK_SET);

  content->num_entries = i;
  content->entries = (Ti8xVarEntry *) calloc(content->num_entries,
					     sizeof(Ti8xVarEntry));
  if (content->entries == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }

  for (i = 0; i < content->num_entries; i++) 
  {
    Ti8xVarEntry *entry = &((content->entries)[i]);

    fread_word(f, NULL);
    fread_word(f, (uint16_t *) & (entry->size));
    fread_byte(f, &(entry->type));
    if (is_ti8586(content->model))
      fread_byte(f, &name_length);
    fread_n_chars(f, name_length, entry->var_name);
	if(content->model == CALC_TI86)
		fskip(f, 8 - name_length);
    tifiles_transcode_varname(content->model, entry->name, entry->var_name, 
			   entry->type);
    if (ti83p_flag) 
	{
      entry->attr = (fgetc(f) == 0x80 ? ATTRB_ARCHIVED : ATTRB_NONE);
      fgetc(f);
    }
    fread_word(f, NULL);

    entry->data = (uint8_t *) calloc(entry->size, 1);
    if (entry->data == NULL) 
	{
      fclose(f);
      return ERR_MALLOC;
    }

    fread(entry->data, entry->size, 1, f);
  }

  fread_word(f, &(content->checksum));

  fclose(f);

  return 0;
}

/**
 * ti8x_file_read_backup:
 * @filename: name of backup file to open.
 * @content: where to store the file content.
 *
 * Load the backup file into a Ti8xBackup structure.
 *
 * Structure content must be freed with #ti8x_content_free_backup when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_read_backup(const char *filename, Ti8xBackup *content)
{
  FILE *f;
  char signature[9];

  if (!tifiles_file_is_ti(filename))
    return ERR_INVALID_FILE;
  if (!tifiles_file_is_backup(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }

  fread_8_chars(f, signature);
  content->model = tifiles_signature2calctype(signature);
  if (content->model == CALC_NONE)
    return ERR_INVALID_FILE;
  fskip(f, 3);
  fread_n_chars(f, 42, content->comment);
  fread_word(f, NULL);

  fread_word(f, NULL);
  fread_word(f, &(content->data_length1));
  fread_byte(f, &(content->type));
  fread_word(f, &(content->data_length2));
  fread_word(f, &(content->data_length3));
  content->data_length4 = 0;
  if (content->model != CALC_TI86)
    fread_word(f, &(content->mem_address));
  else
    fread_word(f, &(content->data_length4));

  fread_word(f, NULL);
  content->data_part1 = (uint8_t *) calloc(content->data_length1, 1);
  if (content->data_part1 == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }
  fread(content->data_part1, 1, content->data_length1, f);

  fread_word(f, NULL);
  content->data_part2 = (uint8_t *) calloc(content->data_length2, 1);
  if (content->data_part2 == NULL) 
  {
    fclose(f);
    return ERR_MALLOC;
  }
  fread(content->data_part2, 1, content->data_length2, f);

  if (content->data_length3)	// can be 0000 on TI86
  {
    fread_word(f, NULL);
    content->data_part3 = (uint8_t *) calloc(content->data_length3, 1);
    if (content->data_part3 == NULL) 
	{
      fclose(f);
      return ERR_MALLOC;
    }
    fread(content->data_part3, 1, content->data_length3, f);
  }

  if (content->model == CALC_TI86) 
  {
    fread_word(f, NULL);
    content->data_part4 = (uint8_t *) calloc(content->data_length4, 1);
    if (content->data_part4 == NULL) 
	{
      fclose(f);
      return ERR_MALLOC;
    }
    fread(content->data_part4, 1, content->data_length4, f);
  } 
  else 
  {
    content->data_length4 = 0;
    content->data_part4 = NULL;
  }

  fread_word(f, &(content->checksum));

  fclose(f);

  return 0;
}

/**
 * ti8x_file_read_flash:
 * @filename: name of flash file to open.
 * @content: where to store the file content.
 *
 * Load the flash file into a Ti8xFlash structure.
 *
 * Structure content must be freed with #ti8x_content_free_flash when
 * no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_read_flash(const char *filename, Ti8xFlash *content)
{
  FILE *f, *file;
  int i;
  uint32_t block_size;
  int num_blocks;
  int mask_mode;
  int ret;
  uint16_t flash_address;
  uint16_t flash_page;
  uint8_t flag = 0x80;
  uint8_t buf[256];
  char signature[9];

  if (!tifiles_file_is_ti(filename))
    return ERR_INVALID_FILE;

  if (!tifiles_file_is_flash(filename))
    return ERR_INVALID_FILE;

  content->model = tifiles_file_get_model(filename);
  f = fopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }
  file = f;

  fread_8_chars(f, signature);
  if (strcmp(signature, "**TIFL**"))
    return ERR_INVALID_FILE;
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
  fread_long(f, &content->data_length);
  content->pages = NULL;

  // determine block size
  if (content->data_type == TI83p_AMS) 
  {
    block_size = 256;
    mask_mode = MODE_AMS;
  } 
  else if (content->data_type == TI83p_APPL) 
  {
    block_size = 128;
    mask_mode = MODE_APPS;
  } 
  else
    return ERR_INVALID_FILE;

  // compute approximative number of pages
  num_blocks = (content->data_length / 77) / (block_size >> 5);
  content->pages = (Ti8xFlashPage *) calloc(num_blocks + 10,
					    sizeof(Ti8xFlashPage));
  if (content->pages == NULL)
    return ERR_MALLOC;

  // reset block reader by passing mode=0
  intelhex_read_data_block(file, &flash_address, &flash_page, NULL, 0);
  flag = 0x80;			// OS only
  for (i = 0;; i++) 
  {
    ret = intelhex_read_data_block(file, &flash_address, &flash_page,
			  buf, mask_mode);

    if (mask_mode & MODE_AMS) 
	{
      if (i == 0) 
	  {
		// first block is header
		flag = 0x80;
		flash_address = flash_page = 0;
      }
      if (i == 1) 
	  {
		// other blocks are data
		flag = 0x00;
      }
      if (ret == 3) 
	  {
		// last block is signature
		flag = 0x80;
		flash_address = flash_page = 0;	// fix quirk in IntelHex module
      }
    }
    if (ret < 0)
      break;

    content->pages[i].offset = flash_address;
    content->pages[i].page = flash_page;
    content->pages[i].flag = flag;
    content->pages[i].length = block_size;

    content->pages[i].data = (uint8_t *) calloc(block_size, 1);
    if (content->pages[i].data == NULL)
      return ERR_MALLOC;

    memcpy(content->pages[i].data, buf, block_size);
  }
  content->num_pages = i;

  fclose(f);

  return 0;
}

/***********/
/* Writing */
/***********/

/**
 * ti8x_file_write_regular:
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
TIEXPORT int TICALL ti8x_file_write_regular(const char *fname, Ti8xRegular *content, char **real_fname)
{
  FILE *f;
  int i;
  uint16_t sum = 0;
  char *filename = NULL;
  char trans[17];
  uint32_t data_length;
  uint16_t packet_length = 0x0B;

  if (fname != NULL) 
  {
    filename = strdup(fname);
    if (filename == NULL)
      return ERR_MALLOC;
  } 
  else 
  {
    tifiles_transcode_varname(content->model, trans, content->entries[0].var_name, 
			   content->entries[0].type );

    filename = (char *) malloc(strlen(trans) + 1 + 5 + 1);
    strcpy(filename, trans);
    strcat(filename, ".");
    strcat(filename, tifiles_vartype2fext(content->model, content->entries[0].type));
    if (real_fname != NULL)
      *real_fname = strdup(filename);
  }

  f = fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    free(filename);
    return ERR_FILE_OPEN;
  }
  free(filename);

  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->model));
  fwrite(content->model == CALC_TI85 ? fsignature85 : fsignature8x, 1, 3, f);
  fwrite_n_bytes(f, 42, content->comment);
  for (i = 0, data_length = 0; i < content->num_entries; i++) 
  {
    Ti8xVarEntry *entry = &(content->entries[i]);
    data_length += entry->size + 15;
    if (is_ti8586(content->model))
      data_length += 1;
    if (is_ti83p(content->model))
      data_length += 2;
  }
  if (data_length > 65535)
    return ERR_GROUP_SIZE;
  fwrite_word(f, (uint16_t) data_length);

  switch (content->model) 
  {
  case CALC_TI85:
  case CALC_TI86:
    packet_length = 0x0C;
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    packet_length = 0x0D;
    break;
  case CALC_TI82:
  case CALC_TI83:
    packet_length = 0x0B;
    break;
  default:
    break;
  }

  // write data section
  for (i = 0, sum = 0; i < content->num_entries; i++) 
  {
    Ti8xVarEntry *entry = &(content->entries[i]);

    fwrite_word(f, packet_length);
    fwrite_word(f, (uint16_t)entry->size);
    fwrite_byte(f, entry->type);
    if (is_ti8586(content->model)) 
	{
      size_t name_length = strlen(entry->var_name);
      fwrite_byte(f, (uint8_t)name_length);
	  if(content->model == CALC_TI85)
		fwrite_n_chars(f, name_length, entry->var_name);
	  else
		fwrite_n_chars2(f, 8, entry->var_name);
    }
    else
    	fwrite_n_chars(f, 8, entry->var_name);
    if (is_ti83p(content->model))
      fwrite_word(f, (uint16_t)((entry->attr == ATTRB_ARCHIVED) ? 0x80 : 0x00));
    fwrite_word(f, (uint16_t)entry->size);
    fwrite(entry->data, entry->size, 1, f);

    sum += packet_length;
    sum += tifiles_checksum((uint8_t *) & (entry->size), 2);
    sum += entry->type;
    if (is_ti8586(content->model))
      sum += strlen(entry->var_name);
    sum += tifiles_checksum((uint8_t *) entry->var_name, 8);
    sum += tifiles_checksum((uint8_t *) & (entry->size), 2);
    sum += tifiles_checksum(entry->data, entry->size);
  }

  //checksum is the sum of all bytes in the data section
  content->checksum = sum;
  fwrite_word(f, content->checksum);

  fclose(f);

  return 0;
}

/**
 * ti8x_file_write_backup:
 * @filename: name of backup file where to write.
 * @content: the file content to write.
 *
 * Write content to a backup file.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_write_backup(const char *filename, Ti8xBackup *content)
{
  FILE *f;
  uint16_t sum = 0;
  uint16_t data_length;

  f = fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }
  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->model));
  fwrite(content->model == CALC_TI85 ? fsignature85 : fsignature8x, 1, 3, f);
  fwrite_n_bytes(f, 42, content->comment);
  data_length =
      content->data_length1 + content->data_length2 +
      content->data_length3 + 17;
  data_length += content->data_length4;
  fwrite_word(f, data_length);

  // write backup header
  fwrite_word(f, 0x09);
  fwrite_word(f, content->data_length1);
  fwrite_byte(f, content->type);
  fwrite_word(f, content->data_length2);
  fwrite_word(f, content->data_length3);
  if (content->model != CALC_TI86)
    fwrite_word(f, content->mem_address);
  else
    fwrite_word(f, content->data_length4);

  // write data num_entries
  fwrite_word(f, content->data_length1);
  fwrite(content->data_part1, 1, content->data_length1, f);
  fwrite_word(f, content->data_length2);
  fwrite(content->data_part2, 1, content->data_length2, f);
  if (content->data_length3)	// TI86: can be NULL
    fwrite_word(f, content->data_length3);
  fwrite(content->data_part3, 1, content->data_length3, f);
  if (content->model == CALC_TI86) {
    fwrite_word(f, content->data_length4);
    fwrite(content->data_part4, 1, content->data_length4, f);
  }
  // checksum = sum of all bytes in bachup headers and data num_entries
  sum = 0;
  sum += 9;
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length1), 2);
  sum += content->type;
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length2), 2);
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length3), 2);
  if (content->model != CALC_TI86)
    sum +=
	tifiles_checksum((uint8_t *) & (content->mem_address), 2);
  else
    sum +=
	tifiles_checksum((uint8_t *) & (content->data_length4), 2);

  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length1), 2);
  sum +=
      tifiles_checksum(content->data_part1, content->data_length1);
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length2), 2);
  sum +=
      tifiles_checksum(content->data_part2, content->data_length2);
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length3), 2);
  sum +=
      tifiles_checksum(content->data_part3, content->data_length3);
  sum +=
      tifiles_checksum((uint8_t *) & (content->data_length4), 2);
  sum +=
      tifiles_checksum(content->data_part4, content->data_length4);

  content->checksum = sum;
  fwrite_word(f, content->checksum);

  fclose(f);

  return 0;
}

/**
 * ti8x_file_write_flash:
 * @filename: name of flash file where to write.
 * @content: the file content to write.
 *
 * Write content to a flash file (os or app).
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_write_flash(const char *filename, Ti8xFlash *content)
{
  FILE *f, *file;
  int i, j;
  int bytes_written = 0;

  f = fopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info("Unable to open this file: <%s>", filename);
    return ERR_FILE_OPEN;
  }
  file = f;

  fwrite_8_chars(f, "**TIFL**");
  fwrite_byte(f, content->revision_major);
  fwrite_byte(f, content->revision_minor);
  fwrite_byte(f, content->flags);
  fwrite_byte(f, content->object_type);
  fwrite_byte(f, content->revision_day);
  fwrite_byte(f, content->revision_month);
  fwrite_word(f, content->revision_year);
  fwrite_byte(f, (uint8_t)strlen(content->name));
  fwrite_8_chars(f, content->name);
  for (j = 0; j < 23; j++)
    fputc(0, f);
  fwrite_byte(f, content->device_type);
  fwrite_byte(f, content->data_type);
  for (j = 0; j < 24; j++)
    fputc(0, f);

  // approximative value, need to be fixed !
  //content->data_length = content->num_pages * (content->pages[0].length >> 5) * 77;
  fwrite_long(f, content->data_length);

  // data
  for (i = 0; i < content->num_pages; i++)
    bytes_written += intelhex_write_data_block(file, content->pages[i].offset,
		     content->pages[i].page, content->pages[i].data, 0);

  // final block
  bytes_written += intelhex_write_data_block(file, 0, 0, 0, !0);  
  printf("bytes_written = %06x (%i) \n", bytes_written, bytes_written);

  fclose(f);

  return 0;
}


/**************/
/* Displaying */
/**************/

/**
 * ti8x_content_display_regular:
 * @content: a Ti8xRegular structure.
 *
 * Display fields of a Ti8xRegular structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_content_display_regular(Ti8xRegular *content)
{
  int i;
  char trans[17];

  tifiles_info("Signature:     <%s>",
	  tifiles_calctype2signature(content->model));
  tifiles_info("Comment:       <%s>", content->comment);
  tifiles_info("# of entries:  %i", content->num_entries);

  for (i = 0; i < content->num_entries /*&& i<5 */ ; i++) 
  {
    tifiles_info("Entry #%i", i);
    tifiles_info("  name:        <%s>",
	    tifiles_transcode_varname(content->model, trans,
					content->entries[i].var_name,				   
				   content->entries[i].type
				   ));
    tifiles_info("  type:        %02X (%s)",
	    content->entries[i].type,
	    tifiles_vartype2string(content->model, content->entries[i].type));
    tifiles_info("  attr:        %s",
	    tifiles_attribute_to_string(content->entries[i].attr));
    tifiles_info("  length:      %04X (%i)",
	    content->entries[i].size, content->entries[i].size);
  }

  tifiles_info("Checksum:      %04X (%i) ", content->checksum,
	  content->checksum);

  return 0;
}

/**
 * ti8x_content_display_backup:
 * @content: a Ti8xBackup structure.
 *
 * Display fields of a Ti8xBackup structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_content_display_backup(Ti8xBackup *content)
{
  tifiles_info("Signature:      <%s>",
	  tifiles_calctype2signature(content->model));
  tifiles_info("Comment:        <%s>", content->comment);
  tifiles_info("Type:           %02X (%s)", content->type,
	  tifiles_vartype2string(content->model, content->type));
  tifiles_info("Mem address:    %04X (%i)",
	  content->mem_address, content->mem_address);

  tifiles_info("\n");

  tifiles_info("data_length1:   %04X (%i)",
	  content->data_length1, content->data_length1);
  tifiles_info("data_length2:   %04X (%i)",
	  content->data_length2, content->data_length2);
  tifiles_info("data_length3:   %04X (%i)",
	  content->data_length3, content->data_length3);
  if (content->model == CALC_TI86)
    tifiles_info("data_length4:   %04X (%i)",
	    content->data_length4, content->data_length4);

  tifiles_info("Checksum:       %04X (%i) ", content->checksum,
	  content->checksum);

  return 0;
}

/**
 * ti8x_content_display_flash:
 * @content: a Ti8xFlash structure.
 *
 * Display fields of a Ti8xFlash structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_content_display_flash(Ti8xFlash *content)
{
  Ti8xFlash *ptr = content;

  tifiles_info("Signature:       <%s>",
	  tifiles_calctype2signature(ptr->model));
  tifiles_info("Revision:        %i.%i", ptr->revision_major,
	  ptr->revision_minor);
  tifiles_info("Flags:           %02X", ptr->flags);
  tifiles_info("Object type:     %02X", ptr->object_type);
  tifiles_info("Date:            %02X/%02X/%02X%02X",
	  ptr->revision_day, ptr->revision_month,
	  ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
  tifiles_info("Name:            <%s>", ptr->name);
  tifiles_info("Device type:     %s",
	  ptr->device_type == DEVICE_TYPE_83P ? "ti83+" : "ti73");
  tifiles_info("Data type:       ");
  switch (ptr->data_type) 
  {
  case 0x23:
    tifiles_info("OS data");
    break;
  case 0x24:
    tifiles_info("APP data");
    break;
  case 0x25:
    tifiles_info("certificate");
    break;
  case 0x3E:
    tifiles_info("license");
    break;
  default:
    tifiles_info("Unknown (mailto roms@lpg.ticalc.org)\n");
    break;
  }
  tifiles_info("Length:          %08X (%i)", ptr->data_length,
	    ptr->data_length);
  tifiles_info("Number of pages: %i", ptr->num_pages);

  return 0;
}

/**
 * ti8x_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
TIEXPORT int TICALL ti8x_file_display(const char *filename)
{
  Ti8xRegular content1;
  Ti8xBackup content2;
  Ti8xFlash content3;

  if (tifiles_file_is_flash(filename)) 
  {
    ti8x_file_read_flash(filename, &content3);
    ti8x_content_display_flash(&content3);
    ti8x_content_free_flash(&content3);
  } 
  else if (tifiles_file_is_backup(filename)) 
  {
    ti8x_file_read_backup(filename, &content2);
    ti8x_content_display_backup(&content2);
    ti8x_content_free_backup(&content2);
  } 
  else if (tifiles_file_is_regular(filename)) 
  {
    ti8x_file_read_regular(filename, &content1);
    ti8x_content_display_regular(&content1);
    ti8x_content_free_regular(&content1);
  } 
  else 
  {
    tifiles_info("Unknwon file type !");
    return ERR_BAD_FILE;
  }

  return 0;
}
