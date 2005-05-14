/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libTIFILES - Ti File Format library, a part of the TiLP project
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

#ifndef __TIFILES_H__
#define __TIFILES_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "export.h"
#include "stdints.h"

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBFILES_VERSION "0.0.2"
#else
# define LIBFILES_VERSION VERSION
#endif

/* Types */

#define VARNAME_MAX		18	// group/name: 8 + 1 + 8 + 1
#define FILES_NCALCS	12	// # of supported calcs

/**
 * CalcModel:
 *
 * An enumeration which contains the following calculator types:
 **/
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} CalcModel;

/**
 * FileAttr:
 *
 * An enumeration which contains the different variable attributes.
 **/
typedef enum 
{
  ATTRB_NONE = 0, ATTRB_LOCKED = 1, ATTRB_PROTECTED, ATTRB_ARCHIVED = 3
} FileAttr;

/**
 * FileClass:
 *
 * An enumeration which contains the following class of TI files:
 **/
typedef enum 
{
  TIFILE_SINGLE = 1, TIFILE_GROUP = 2, TIFILE_BACKUP = 4, TIFILE_FLASH = 8,
} FileClass;

/**
 * FileEncoding:
 *
 * An enumeration which contains the following encodings:
 **/
typedef enum 
{
  ENCODING_ASCII = 1, ENCODING_LATIN1, ENCODING_UNICODE
} FileEncoding;

/* Structures (common to all calcs) */

/**
 * TiVarEntry:
 * @fld_name: name of folder (TI9x only) or ""
 * @var_name: name of variable (binary, on-calc)
 * @name: name of variable (detokenized, human-readable)
 * @type: vartype ID
 * @attr: TI83+/89/92+ only (ATTRB_NONE or ARCHIVED)
 * @size: size of data (uint16_t for TI8x)
 * @data: pure data
 *
 * A generic structure used to store the content of a TI variable.
 **/
typedef struct 
{
  char		fld_name[9];
  char		var_name[9];

  char		name[18];
  uint8_t	type;
  uint8_t	attr;
  uint32_t	size;
  uint8_t*	data;
} VarEntry;

/**
 * Regular:
 * @model: calculator model
 * @default_folder: name of the default folder (TI9x only)
 * @comment: comment aembedded in file (like "Single file received by TiLP")
 * @num_entries: number of variables stored after
 * @entries: an array of #TiVarEntry structures which contains data
 * @checksum: checksum of file
 *
 * A generic structure used to store the content of a single/grouped TI file.
 **/
typedef struct 
{
  CalcModel	model;

  char			default_folder[9];	// TI9x only
  char			comment[43];		// Ti8x: 41 max

  int			num_entries;
  VarEntry*		entries;

  uint16_t		checksum;
} Regular;

/* Functions */

// namespace scheme: library_class_function like tifiles_fext_get

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  
  TIEXPORT int TICALL tifiles_library_init(void);
  TIEXPORT int TICALL tifiles_library_exit(void);

  /*********************/
  /* General functions */
  /*********************/

  // tifiles.c
  TIEXPORT const char* TICALL tifiles_version_get (void);

  // error.c
  TIEXPORT int         TICALL tifiles_error_get (int number, char **message);

  // type2str.c
  TIEXPORT const char* TICALL tifiles_model_to_string (CalcModel type);
  TIEXPORT CalcModel TICALL tifiles_string_to_model (const char *str);
  
  TIEXPORT const char* TICALL tifiles_attribute_to_string (FileAttr atrb);
  TIEXPORT FileAttr  TICALL tifiles_string_to_attribute (const char *str);
  
  TIEXPORT const char* TICALL tifiles_class_to_string (FileClass type);
  TIEXPORT FileClass TICALL tifiles_string_to_class (const char *str);

  // transcode.c
  TIEXPORT char* TICALL tifiles_transcode_detokenize (CalcModel model, char *dst, const char *src, uint8_t vartype);

  TIEXPORT char* TICALL tifiles_transcode_to_ascii (CalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_latin1 (CalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_unicode (CalcModel model, char* dst, const char *src);

  TIEXPORT void TICALL tifiles_transcoding_set (FileEncoding encoding);
  TIEXPORT FileEncoding TICALL tifiles_transcoding_get (void);

  TIEXPORT char *TICALL tifiles_transcode_varname (CalcModel model, char *dst, const char *src, uint8_t vartype);
  TIEXPORT char *TICALL tifiles_transcode_varname_static (CalcModel model, const char *src, uint8_t vartype);

  // filetypes.c
  TIEXPORT const char* TICALL tifiles_fext_of_group (CalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_backup (CalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_flash_app (CalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_flash_os (CalcModel model);

  TIEXPORT char* TICALL tifiles_fext_get (const char *filename);
  TIEXPORT char* TICALL tifiles_fext_dup (const char *filename);

  TIEXPORT int TICALL tifiles_file_is_ti (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_single (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_group (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_regular (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_backup (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_flash (const char *filename);
  TIEXPORT int TICALL tifiles_file_is_tib (const char *filename);

  TIEXPORT CalcModel TICALL tifiles_file_get_model (const char *filename);
  TIEXPORT FileClass TICALL tifiles_file_get_class (const char *filename);

  TIEXPORT const char* TICALL tifiles_file_get_type (const char *filename);
  TIEXPORT const char* TICALL tifiles_file_get_icon (const char *filename);
  
  // typesXX.c
  TIEXPORT const char* TICALL tifiles_vartype2string (CalcModel model, uint8_t data);
  TIEXPORT uint8_t     TICALL tifiles_string2vartype (CalcModel model, const char *s);

  TIEXPORT const char* TICALL tifiles_vartype2fext (CalcModel model, uint8_t data);
  TIEXPORT uint8_t     TICALL tifiles_fext2vartype (CalcModel model, const char *s);

  TIEXPORT const char* TICALL tifiles_vartype2type (CalcModel model, uint8_t id);
  TIEXPORT const char* TICALL tifiles_vartype2icon (CalcModel model, uint8_t id);

  TIEXPORT const char*  TICALL tifiles_calctype2signature (CalcModel model);
  TIEXPORT CalcModel  TICALL tifiles_signature2calctype (const char *signature);

  TIEXPORT const uint8_t TICALL tifiles_folder_type (CalcModel model);
  TIEXPORT const uint8_t TICALL tifiles_flash_type (CalcModel model);
  TIEXPORT const uint8_t TICALL tifiles_idlist_type (CalcModel model);

  // misc.c
  TIEXPORT int TICALL tifiles_calc_is_ti8x (CalcModel model);
  TIEXPORT int TICALL tifiles_calc_is_ti9x (CalcModel model);

  TIEXPORT int TICALL tifiles_has_folder (CalcModel model);
  TIEXPORT int TICALL tifiles_is_flash (CalcModel model);

  TIEXPORT uint16_t TICALL tifiles_checksum(uint8_t * buffer, int size);

  TIEXPORT char* TICALL tifiles_get_varname(const char *full_name);
  TIEXPORT char* TICALL tifiles_get_fldname(const char *full_name);
  TIEXPORT char* TICALL tifiles_build_fullname(CalcModel model,
	                     char *full_name,
					     const char *fldname,
					     const char *varname);

  // filesXX.c: layer built on files8x/9x
  TIEXPORT Regular* TICALL tifiles_content_create_regular(void);
  TIEXPORT int      TICALL tifiles_content_free_regular(Regular *content);

  TIEXPORT int TICALL tifiles_file_read_regular(const char *filename, Regular *content);
  TIEXPORT int TICALL tifiles_file_write_regular(const char *filename, Regular *content, char **filename2);
  TIEXPORT int TICALL tifiles_file_display(const char *filename);

  TIEXPORT int** TICALL tifiles_create_table_of_entries(Regular *content, int *nfolders);

  // grouped.c
  TIEXPORT int TICALL tifiles_group_contents(Regular **src_contents, Regular **dst_content);
  TIEXPORT int TICALL tifiles_ungroup_content(Regular *src_content, Regular ***dst_contents);

  TIEXPORT int TICALL tifiles_group_files(char **src_filenames, const char *dst_filename);
  TIEXPORT int TICALL tifiles_ungroup_file(const char *src_filename, char ***dst_filenames);

  TIEXPORT int TICALL tifiles_content_free_group(Regular **array);

  

  // special for MSVC (DLL partition -> memory violation, why ?!)
#if defined(__WIN32__) && !defined(__MINGW32__)
  TIEXPORT void* TICALL tifiles_calloc(unsigned int nmemb, unsigned int size);
  TIEXPORT void* TICALL tifiles_malloc(unsigned int size);
  TIEXPORT void  TICALL tifiles_free(void *ptr);
  TIEXPORT void* TICALL tifiles_realloc(void *ptr, unsigned int size);
#else
# define tifiles_calloc  calloc
# define tifiles_malloc  malloc
# define tifiles_free    free
# define tifiles_realloc realloc
#endif

  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
