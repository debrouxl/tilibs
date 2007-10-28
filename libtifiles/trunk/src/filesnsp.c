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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	TI File Format handling routines
	Calcs: TI-NSpire
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
#include "filesnsp.h"
#include "rwfile.h"


/***********/
/* Reading */
/***********/

/**
 * tnsp_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a FileContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_file_read_regular(const char *filename, FileContent *content)
{
  FILE *f;

  if (!tifiles_file_is_regular(filename))
    return ERR_INVALID_FILE;

  f = gfopen(filename, "rb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: %s", filename);
    return ERR_FILE_OPEN;
  }

  content->model = CALC_NSPIRE;
  content->model_dst = content->model;
      
  {
	  VarEntry *entry = content->entries[0] = g_malloc0(sizeof(VarEntry));
  
	  if(fread(entry->data, 1, entry->size, f) < entry->size) goto tffr;
  }

  fclose(f);
  return 0;

tffr:	// release on exit
    fclose(f);
	tifiles_content_delete_regular(content);
	return ERR_FILE_IO;
}

/***********/
/* Writing */
/***********/

/**
 * tnsp_file_write_regular:
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
int tnsp_file_write_regular(const char *fname, FileContent *content, char **real_fname)
{
  FILE *f;
  char *filename = NULL;

  {
    filename = g_strdup(fname);
    if (filename == NULL)
      return ERR_MALLOC;
  } 

  f = gfopen(filename, "wb");
  if (f == NULL) 
  {
    tifiles_info( "Unable to open this file: %s", filename);
    g_free(filename);
    return ERR_FILE_OPEN;
  }
  g_free(filename);

  {
	  VarEntry *entry = content->entries[0];
		
	  if(fwrite(entry->data, 1, entry->size, f) < entry->size) 
		  goto tfwr;
  }

  fclose(f);
  return 0;

tfwr:	// release on exit
    fclose(f);
	return ERR_FILE_IO;
}

/**************/
/* Displaying */
/**************/

/**
 * tnsp_content_display_regular:
 * @content: a FileContent structure.
 *
 * Display fields of a FileContent structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int tnsp_content_display_regular(FileContent *content)
{
  int i;
  char trans[17];

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

  // the testing order is important: regular before backup (due to TI89/92+)
  if (tifiles_file_is_os(filename)) 
  {
	  /*
	content3 = tifiles_content_create_flash(CALC_TI92);
    tnsp_file_read_flash(filename, content3);
    tnsp_content_display_flash(content3);
    tifiles_content_delete_flash(content3);
	*/
  } 
  else if (tifiles_file_is_regular(filename)) 
  {
	content1 = tifiles_content_create_regular(CALC_TI92);
    tnsp_file_read_regular(filename, content1);
    tnsp_content_display_regular(content1);
    tifiles_content_delete_regular(content1);
  }
  else
  {
      tifiles_info("Unknown file type !");
      return ERR_BAD_FILE;
  }

  return 0;
}
