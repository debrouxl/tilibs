/*  libtifiles - TI File Format library
 *  Copyright (C) 2002  Romain Lievin
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
  Calcs: 73/82/83/83+/85/86
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_int.h"
#include "file_err.h"
#include "file_def.h"
#include "typesxx.h"
#include "filesxx.h"
#include "misc.h"
#include "intelhex.h"
#include "trans.h"

static uint8_t fsignature[3] = { 0x1A, 0x0A, 0x00 };

static int is_ti8586(int calc_type)
{
  return ((calc_type == CALC_TI85) ||
	  (calc_type == CALC_TI86));
}

static int is_ti83p(int calc_type)
{
  return (calc_type == CALC_TI83P);
}

/**************/
/* Allocating */
/**************/

TIEXPORT Ti8xRegular* TICALL ti8x_create_regular_content(void)
{
  Ti8xRegular *content = (Ti8xRegular *)calloc(1, sizeof(Ti8xRegular));

  return content;
}

TIEXPORT Ti8xBackup*  TICALL ti8x_create_backup_content (void)
{
  Ti8xBackup *content = (Ti8xBackup *)calloc(1, sizeof(Ti8xBackup));

  return content;
}

TIEXPORT Ti8xFlash*   TICALL ti8x_create_flash_content  (void)
{
  Ti8xFlash *content = (Ti8xFlash *)calloc(1, sizeof(Ti8xFlash));

  return content;
}

/*************************/
/* Copying (duplicating) */
/*************************/

/*
  Copy an Ti8xVarEntry structure (data included)
*/
int ti8x_dup_VarEntry(Ti8xVarEntry *dst, Ti8xVarEntry *src)
{
  memcpy(dst, src, sizeof(Ti8xVarEntry));

  dst->data = (uint8_t *)calloc(dst->size, 1);
  if(dst->data == NULL)
    return ERR_MALLOC;    
  memcpy(dst->data, src->data, dst->size);

  return 0;
}

/*
  Copy an Ti8xRegular structure
*/
int ti8x_dup_Regular(Ti8xRegular *dst, Ti8xRegular *src)
{
  int i;

  memcpy(dst, src, sizeof(Ti8xRegular));

  dst->entries = (Ti8xVarEntry *)calloc(src->num_entries,
					sizeof(Ti8xVarEntry));
  if(dst->entries == NULL)
    return ERR_MALLOC;

  for(i=0; i<src->num_entries; i++)
    TRY(ti8x_dup_VarEntry(&(dst->entries[i]),
		       &(src->entries[i])));

  return 0;
}

/*
  Copy an Ti8xBackup structure
*/
int ti8x_dup_Backup(Ti8xBackup *dst, Ti8xBackup *src)
{
  memcpy(dst, src, sizeof(Ti8xBackup));
  
  dst->data_part1 = (uint8_t *)calloc(dst->data_length1, 1);
  dst->data_part2 = (uint8_t *)calloc(dst->data_length2, 1);
  dst->data_part3 = (uint8_t *)calloc(dst->data_length3, 1);
  dst->data_part4 = (uint8_t *)calloc(dst->data_length4, 1);

  if((dst->data_part1 == NULL) || 
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

/*
  Free the content of a Ti8xRegular structure
*/
TIEXPORT int TICALL ti8x_free_regular_content(Ti8xRegular *content)
{
  int i;

  for(i=0; i<content->num_entries; i++)
    {
      Ti8xVarEntry *entry = &(content->entries[i]);
      free(entry->data);
    }
  free(content->entries);
  
  return 0;
}

/*
  Same as above
*/
TIEXPORT int TICALL ti8x_free_backup_content(Ti8xBackup *content)
{
  free(content->data_part1);
  free(content->data_part2);
  free(content->data_part3);
  free(content->data_part4);

  return 0;
}

TIEXPORT int TICALL ti8x_free_flash_content(Ti8xFlash *content)
{
  free(content->pages);
  return 0;
}

/***********/
/* Reading */
/***********/

/*
  Open a file and place its content in a Ti8xRegular structure
  - filename [in]: a file to read
  - content [out]: the address of a structure where the file content 
  will be stored
  - int [out]: an error code
 */
TIEXPORT int TICALL ti8x_read_regular_file(const char *filename, 
					   Ti8xRegular *content)
{
  FILE *f;
  uint16_t tmp = 0x000B;
  long offset = 0;
  int i;
  int ti83p_flag = 0;
  uint8_t name_length = 8;	// ti85/86 only
  char signature[9];
  
  if(!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;
  if(!tifiles_is_a_regular_file(filename))
    return ERR_INVALID_FILE;
  
  f = fopen(filename, "rb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
      return ERR_FILE_OPEN;
    }

  fread_8_chars(f, signature);
  content->calc_type = tifiles_signature2calctype(signature);
  if(content->calc_type == CALC_NONE) return ERR_INVALID_FILE;
  fskip(f, 3);
  fread_n_chars(f, 42, content->comment);
  fread_word(f, NULL);

  // search for the number of entries by parsing the whole file
  offset = ftell(f);
  for(i=0; ; i++)
    {
      fread_word(f, &tmp);
      if(tmp == 0x0D)
	ti83p_flag = !0;	// true TI83+ file (2 extra bytes)
      if( (tmp != 0x0B) && (tmp != 0x0C) && (tmp != 0x0D) )
	break;
      if(is_ti8586(content->calc_type))
	fskip(f, 12);
      else if(content->calc_type == CALC_TI83P)
	fskip(f, 13);
      else
	fskip(f, 11);
      fread_word(f, &tmp);
      fskip(f, tmp);
    }
  fseek(f, offset, SEEK_SET);

  content->num_entries = i;
  content->entries = (Ti8xVarEntry *)calloc(content->num_entries,
					    sizeof(Ti8xVarEntry));
  if(content->entries == NULL)
    {
      fclose(f);
      return ERR_MALLOC;
    }

  for(i=0; i<content->num_entries; i++)
    {
      Ti8xVarEntry *entry = &((content->entries)[i]);
      
      fread_word(f, NULL);
      fread_word(f, (uint16_t *)&(entry->size));
      fread_byte(f, &(entry->type));
      if(is_ti8586(content->calc_type))
	fread_byte(f, &name_length);
      fread_n_chars(f, name_length, entry->name);
      tixx_translate_varname (entry->name, entry->trans, entry->type,
			      content->calc_type);
      if(is_ti8586(content->calc_type))
	fskip(f, 8-name_length);
      if(ti83p_flag)
	{
	  entry->attr = (fgetc(f) == 0x80 ? ATTRB_ARCHIVED : ATTRB_NONE);
	  fgetc(f);
	}
      fread_word(f, NULL);
      
      entry->data = (uint8_t *)calloc(entry->size, 1);
      if(entry->data == NULL)
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

/*
  Open a file and place its content in a structure
  - filename [in]: a file to read
  - content [out]: the address of a structure where the file content
  will be stored
  - int [out]: an error code
*/
TIEXPORT int TICALL ti8x_read_backup_file (const char *filename, 
					   Ti8xBackup  *content)
{
  FILE *f;
  char signature[9];

  if(!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;
  if(!tifiles_is_a_backup_file(filename))
    return ERR_INVALID_FILE;

  f = fopen(filename, "rb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
      return ERR_FILE_OPEN;
    }

  fread_8_chars(f, signature);
  content->calc_type = tifiles_signature2calctype(signature);
  if(content->calc_type == CALC_NONE) return ERR_INVALID_FILE;
  fskip(f, 3);
  fread_n_chars(f, 42, content->comment);
  fread_word(f, NULL);

  fread_word(f, NULL);
  fread_word(f, &(content->data_length1));
  fread_byte(f, &(content->type));
  fread_word(f, &(content->data_length2));
  fread_word(f, &(content->data_length3));
  content->data_length4 = 0;
  if(content->calc_type != CALC_TI86)
    fread_word(f, &(content->mem_address));
  else
    fread_word(f, &(content->data_length4));

  fread_word(f, NULL);
  content->data_part1 = (uint8_t *)calloc(content->data_length1, 1);
  if(content->data_part1 == NULL)
    {
      fclose(f);
      return ERR_MALLOC;
    }
  fread(content->data_part1, 1, content->data_length1, f);

  fread_word(f, NULL);
  content->data_part2 = (uint8_t *)calloc(content->data_length2, 1);
  if(content->data_part2 == NULL)
    {
      fclose(f);
      return ERR_MALLOC;
    }
  fread(content->data_part2, 1, content->data_length2, f);

  if(content->data_length3)  // can be 0000 on TI86
    {
      fread_word(f, NULL);
      content->data_part3 = (uint8_t *)calloc(content->data_length3, 1);
      if(content->data_part3 == NULL)
	{
	  fclose(f);
	  return ERR_MALLOC;
	}
      fread(content->data_part3, 1, content->data_length3, f);
    }

  if(content->calc_type == CALC_TI86)
    {
      fread_word(f, NULL);
      content->data_part4 = (uint8_t *)calloc(content->data_length4, 1);
      if(content->data_part4 == NULL)
	{
	  fclose(f);
	  return ERR_MALLOC;
	}
      fread(content->data_part4, 1, content->data_length4, f);
    }
  else {
    content->data_length4 = 0;
    content->data_part4 = NULL;
  } 

  fread_word(f, &(content->checksum));

  fclose(f);

  return 0;
}

TIEXPORT int TICALL ti8x_read_flash_file(const char *filename,
					 Ti8xFlash *content)
{
  FILE *f, *file;
  int i;
  uint32_t flash_size;
  uint32_t block_size;
  int num_blocks;
  int mask_mode;
  int ret;
  uint16_t flash_address;
  uint16_t flash_page;
  uint8_t flag = 0x80;
  uint8_t buf[256];
  char signature[9];

  if(!tifiles_is_a_ti_file(filename))
    return ERR_INVALID_FILE;

  if(!tifiles_is_a_flash_file(filename))
    return ERR_INVALID_FILE;

  content->calc_type = tifiles_which_calc_type(filename);
  f = fopen(filename, "rb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
      return ERR_FILE_OPEN;
    }
  file = f;

  fread_8_chars(f, signature);
  if(strcmp(signature, "**TIFL**")) return ERR_INVALID_FILE;
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
  fread_long(f, &flash_size);
  content->pages = NULL;

  // determine block size
  if(content->data_type == TI83p_AMS) {
    block_size = 256;
    mask_mode = MODE_AMS;  
  }
  else if(content->data_type == TI83p_APPL) {
    block_size = 128;
    mask_mode = MODE_APPS;  
  }
  else
    return ERR_INVALID_FILE;  
  
  // compute approximative number of pages
  num_blocks = (flash_size/77)/(block_size>>5);
  content->pages = (Ti8xFlashPage *)calloc(num_blocks+10,
					   sizeof(Ti8xFlashPage));
  if(content->pages == NULL)
    return ERR_MALLOC;

  // reset block reader by passing mode=0
  read_data_block(file, &flash_address, &flash_page, NULL, 0);
  flag = 0x80; // OS only
  for(i=0; ;i++)
    {
      ret = read_data_block(file, &flash_address, &flash_page, 
			    buf, mask_mode);
      
      if(mask_mode & MODE_AMS)
	{
	  if(i == 0)
	    { 
	      // first block is header
	      flag = 0x80;
	      flash_address = flash_page = 0;
	    }
	  if(i == 1)
	    {	
	      // other blocks are data
	      flag = 0x00;
	    }
	  if(ret == 3)
	    {	
	      // last block is signature
	      flag = 0x80;
	      flash_address = flash_page = 0; // fix quirk in IntelHex module
	    }
	}
      if(ret < 0)
	break;

      content->pages[i].offset  = flash_address;
      content->pages[i].page    = flash_page;
      content->pages[i].flag    = flag;
      content->pages[i].length  = block_size;
      
      content->pages[i].data = (uint8_t *)calloc(block_size, 1);
      if(content->pages[i].data == NULL)
	return ERR_MALLOC;

      memcpy(content->pages[i].data, buf, block_size);
    }
  content->num_pages = i;

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
TIEXPORT int TICALL ti8x_write_regular_file(const char *fname, 
					    Ti8xRegular *content,
					    char **real_fname)
{
  FILE *f;
  int i;
  uint16_t sum = 0;
  char *filename = NULL;
  char trans[9];
  uint32_t data_length;
  uint16_t packet_length = 0x0B;

  if(fname != NULL)
    {
      filename = strdup(fname);
      if(filename == NULL)
	return ERR_MALLOC;
    }
  else
    {
      tixx_translate_varname(content->entries[0].name, trans, 
			     content->entries[0].type, content->calc_type);
      
      filename = (char *)malloc(strlen(trans) + 1 + 5 + 1);     
      strcpy(filename, trans);
      strcat(filename, ".");
      strcat(filename, tifiles_vartype2file(content->entries[0].type));
      if(real_fname != NULL)
	*real_fname = strdup(filename);
    }

  f = fopen(filename, "wb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
      free(filename);
      return ERR_FILE_OPEN;
    }
  free(filename);

  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->calc_type));
  fwrite(fsignature, 1, 3, f);
  fwrite_n_chars(f, 42, content->comment);
  for(i=0, data_length=0; i<content->num_entries; i++)
    {
      Ti8xVarEntry *entry = &(content->entries[i]);
      data_length += entry->size + 15;
      if(is_ti8586(content->calc_type))
	data_length += 1;
      if(is_ti83p(content->calc_type))
	data_length += 2;
    }
  if(data_length > 65535)
    return ERR_GROUP_SIZE;
  fwrite_word(f, (uint16_t)data_length);

  switch(content->calc_type)
    {
    case CALC_TI85:
    case CALC_TI86: packet_length = 0x0C; break;
    case CALC_TI83P: packet_length = 0x0D; break;
    case CALC_TI82:
    case CALC_TI83: packet_length = 0x0B; break;
    default: break;
    }

  // write data section
  for(i=0, sum=0; i<content->num_entries; i++)
    {
      Ti8xVarEntry *entry = &(content->entries[i]);
      
      fwrite_word(f, packet_length);
      fwrite_word(f, entry->size);
      fwrite_byte(f, entry->type);
      if(is_ti8586(content->calc_type))
	fwrite_byte(f, (uint8_t)strlen(entry->name));
      fwrite_n_chars(f, 8, entry->name);
      if(is_ti83p(content->calc_type))
	fwrite_word(f, (entry->attr == ATTRB_ARCHIVED) ? 0x80 : 0x00);
      fwrite_word(f, entry->size);
      fwrite(entry->data, entry->size, 1, f);
      
      sum += packet_length;
      sum += tifiles_compute_checksum((uint8_t *)&(entry->size), 2);
      sum += entry->type;
      if(is_ti8586(content->calc_type))
	sum += strlen(entry->name);
      sum += tifiles_compute_checksum((uint8_t *)entry->name, 8);
      sum += tifiles_compute_checksum((uint8_t *)&(entry->size), 2);
      sum += tifiles_compute_checksum(entry->data, entry->size);
    }

  //checksum is the sum of all bytes in the data section
  content->checksum = sum;
  fwrite_word(f, content->checksum);
  
  fclose(f);

  return 0;
}

/*
  Write the content of the structure into the file
  - filename [in]: a file to write
  - content [in]: the address of a structure
  - int [out]: an error code
*/
TIEXPORT int TICALL ti8x_write_backup_file (const char *filename, 
					    Ti8xBackup  *content)
{
  FILE *f;
  uint16_t sum = 0;
  uint16_t data_length;

  f = fopen(filename, "wb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
      return ERR_FILE_OPEN;
    }

  // write header
  fwrite_8_chars(f, tifiles_calctype2signature(content->calc_type));
  fwrite(fsignature, 1, 3, f);
  fwrite_n_chars(f, 42, content->comment);
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
  if(content->calc_type != CALC_TI86)
    fwrite_word(f, content->mem_address);
  else
    fwrite_word(f, content->data_length4);

  // write data num_entries
  fwrite_word(f, content->data_length1);
  fwrite(content->data_part1, 1, content->data_length1, f);
  fwrite_word(f, content->data_length2);
  fwrite(content->data_part2, 1, content->data_length2, f);
  if(content->data_length3) // TI86: can be NULL
    fwrite_word(f, content->data_length3);
  fwrite(content->data_part3, 1, content->data_length3, f);
  if(content->calc_type == CALC_TI86) {
    fwrite_word(f, content->data_length4);
    fwrite(content->data_part4, 1, content->data_length4, f);
  }  

  // checksum = sum of all bytes in bachup headers and data num_entries
  sum = 0;
  sum += 9;
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length1), 2);
  sum+= content->type;
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length2), 2);
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length3), 2);
  if(content->calc_type != CALC_TI86)
    sum += tifiles_compute_checksum((uint8_t *)&(content->mem_address), 2);
  else
    sum += tifiles_compute_checksum((uint8_t *)&(content->data_length4), 2);
  
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length1), 2);
  sum += tifiles_compute_checksum(content->data_part1, content->data_length1);
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length2), 2);
  sum += tifiles_compute_checksum(content->data_part2, content->data_length2);
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length3), 2);
  sum += tifiles_compute_checksum(content->data_part3, content->data_length3); 
  sum += tifiles_compute_checksum((uint8_t *)&(content->data_length4), 2);
  sum += tifiles_compute_checksum(content->data_part4, content->data_length4);
 
  content->checksum = sum;
  fwrite_word(f, content->checksum);

  fclose(f);

  return 0;
}

TIEXPORT int TICALL ti8x_write_flash_file(const char *filename,
					  Ti8xFlash *content)
{
  FILE *f, *file;
  int i, j;

  f = fopen(filename, "wb");
  if(f == NULL)
    {
      printf("Unable to open this file: <%s>\n", filename);
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
  fwrite_byte(f, strlen(content->name));
  fwrite_8_chars(f, content->name);
  for(j=0; j<23; j++) fputc(0, f);
  fwrite_byte(f, content->device_type);
  fwrite_byte(f, content->data_type);
  for(j=0; j<24; j++) fputc(0, f);
  fwrite_long(f, 0); //????
  
  for(i=0; i<content->num_pages; i++)
    write_data_block(file, content->pages[i].offset, content->pages[i].page, 
		     content->pages[i].data, 0);
  
  return 0;
}


/**************/
/* Displaying */
/**************/

#define tprintf tifiles_printf

/* 
   Display the characteristics of a TI regular content
   - content [in]: the content to show
   - int [out]: an error code
*/
int ti8x_display_regular_content(Ti8xRegular *content)
{
  int i;
  char trans[9];

  tprintf("Signature:     <%s>\n", 
	  tifiles_calctype2signature(content->calc_type));
  tprintf("Comment:       <%s>\n", content->comment);
  tprintf("# of entries:  %i\n", content->num_entries);
  
  for(i=0; i<content->num_entries /*&& i<5*/; i++)
    {
      tprintf("Entry #%i\n", i);
      tprintf("  name:        <%s>\n", 
	      tixx_translate_varname(content->entries[i].name, trans, 
				     content->entries[i].type, 
				     content->calc_type));
      tprintf("  type:        %02X (%s)\n", content->entries[i].type,
	      tifiles_vartype2string(content->entries[i].type));
      tprintf("  attr:        %s\n", 
	     tifiles_attribute_to_string(content->entries[i].attr));
      tprintf("  length:      %04X (%i)\n", 
	     content->entries[i].size, 
	     content->entries[i].size);
    }
  
  tprintf("Checksum:      %04X (%i) \n", content->checksum, 
	 content->checksum);

  return 0;
}

/*
  Display the characteristics of a TI backup content
  - content [in]: the content to show
  - int [out]: an error code
*/
int ti8x_display_backup_content(Ti8xBackup *content)
{
  tprintf("Signature:      <%s>\n", 
	  tifiles_calctype2signature(content->calc_type));
  tprintf("Comment:        <%s>\n", content->comment);
  tprintf("Type:           %02X (%s)\n", content->type, 
    tifiles_vartype2string(content->type));
  tprintf("Mem address:    %04X (%i)\n", 
	 content->mem_address, content->mem_address);
  
  tprintf("\n");
  
  tprintf("data_length1:   %04X (%i)\n", 
	 content->data_length1, content->data_length1);
  tprintf("data_length2:   %04X (%i)\n", 
	 content->data_length2, content->data_length2);
  tprintf("data_length3:   %04X (%i)\n", 
	 content->data_length3, content->data_length3);
  if(content->calc_type == CALC_TI86)
    tprintf("data_length4:   %04X (%i)\n",
	    content->data_length4, content->data_length4);
  
  tprintf("Checksum:       %04X (%i) \n", content->checksum, content->checksum);
  
  return 0;
}

/*
  Display the characteristics of a TI flash content
  - content [in]: the content to show
  - int [out]: an error code
*/
int ti8x_display_flash_content(Ti8xFlash *content)
{
  Ti8xFlash *ptr=content;
  
  tprintf("Signature:       <%s>\n", 
	  tifiles_calctype2signature(ptr->calc_type));
  tprintf("Revision:        %i.%i\n", ptr->revision_major, ptr->revision_minor);
  tprintf("Flags:           %02X\n", ptr->flags);
  tprintf("Object type:     %02X\n", ptr->object_type);
  tprintf("Date:            %02X/%02X/%02X%02X\n",
	 ptr->revision_day, ptr->revision_month,
	 ptr->revision_year & 0xff,
	 (ptr->revision_year & 0xff00) >> 8);
  tprintf("Name:            <%s>\n", ptr->name);
  tprintf("Device type:     %s\n", ptr->device_type == DEVICE_TYPE_83P ?
	 "ti83+" : "ti73");
  tprintf("Data type:       ");
  switch(ptr->data_type)
    {
    case 0x23: tprintf("OS data\n"); break;
    case 0x24: tprintf("APP data\n"); break;
    case 0x25: tprintf("certificate\n"); break;
    case 0x3E: tprintf("license\n"); break;
    default: tprintf("Unknown (mailto roms@lpg.ticalc.org)\n"); break;
    }
  tprintf("Number of pages: %i\n", ptr->num_pages);

  return 0;
}

/*
  Display the internal characteristics of a TI file:
  - filename [in]: the file to stat
  - int [out]: an error code
*/
TIEXPORT int TICALL ti8x_display_file(const char *filename)
{
  Ti8xRegular content1;
  Ti8xBackup content2;
  Ti8xFlash content3;
  
  if(tifiles_is_a_flash_file(filename))
    {
      ti8x_read_flash_file(filename, &content3);
      ti8x_display_flash_content(&content3);
      ti8x_free_flash_content(&content3);
    }
  else if(tifiles_is_a_backup_file(filename))
    {
      ti8x_read_backup_file(filename, &content2);
      ti8x_display_backup_content(&content2);
      ti8x_free_backup_content(&content2);
    }
  else if(tifiles_is_a_regular_file(filename))
    { 
      ti8x_read_regular_file(filename, &content1);
      ti8x_display_regular_content(&content1);
      ti8x_free_regular_content(&content1);
    }
  else
    {
      tprintf("Unknwon file type !\n");
      return ERR_BAD_FILE;
    }  

  return 0;
}

