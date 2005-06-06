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
#include "typesxx.h"

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBFILES_VERSION "0.0.3"
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
  ATTRB_NONE = 0, ATTRB_LOCKED = 1, ATTRB_PROTECTED, ATTRB_ARCHIVED = 3,
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
  ENCODING_ASCII = 1, ENCODING_LATIN1, ENCODING_UNICODE,
} FileEncoding;

/* Structures (common to all calcs) */

/**
 * VarEntry:
 * folder: name of folder (TI9x only) or ""
 * @name: name of variable (binary, on-calc)
 * @type: vartype ID
 * @attr: TI83+/89/92+ only (ATTRB_NONE or ARCHIVED)
 * @size: size of data (uint16_t for TI8x)
 * @data: pure data
 *
 * A generic structure used to store the content of a TI variable.
 **/
typedef struct 
{
  char		folder[9];
  char		name[9];
  uint8_t	type;
  uint8_t	attr;
  uint32_t	size;
  uint8_t*	data;

} VarEntry;

/**
 * FileContent:
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
  CalcModel		model;

  char			default_folder[9];	// TI9x only
  char			comment[43];		// Ti8x: 41 max

  int			num_entries;
  VarEntry*		entries;

  uint16_t		checksum;

} FileContent;

/**
 * BackupContent:
 * @model: calculator model
 * @comment: comment embedded in file (like "Backup file received by TiLP")
 * @checksum: checksum of file
 *
 * A generic structure used to store the content of a backup file.
 **/
typedef struct 
{
  CalcModel model;

  char		comment[43];	// 41 on TI9x

  char		rom_version[9];	// TI92 only
  uint16_t	mem_address;	// TI8x only

  uint8_t	type;

  uint32_t	data_length;	// TI9x only
  uint8_t*	data_part;

  uint16_t	data_length1;	// TI8x only
  uint8_t*	data_part1;

  uint16_t	data_length2;	// TI8x only
  uint8_t*	data_part2;

  uint16_t	data_length3;	// TI8x only
  uint8_t*	data_part3;

  uint16_t	data_length4;	// TI86 only
  uint8_t*	data_part4;

  uint16_t	checksum;

} BackupContent;

/**
 * FlashPage:
 * @offset: FLASH offset (see TI link guide).
 * @page: FLASH page (see TI link guide).
 * @flag: see link guide.
 * @length: length of pure data.
 * @data: pure FLASH data.
 *
 * A generic structure used to store the content of a TI8x memory page for FLASH.
 **/
typedef struct 
{
  uint16_t	addr;
  uint16_t	page;
  uint8_t	flag;
  uint16_t	size;
  uint8_t*	data;

} FlashPage;

/**
 * FlashContent:
 * @model: a calculator model.
 * @revision_major:
 * @revision_minor:
 * @flags:
 * @object_type:
 * @revision_day:
 * @revision_month:
 * @revision_year: 
 * @name: name of FLASH app or "basecode" for OS
 * @device_type: a device ID (TI89: 0x88, TI92+:0x98)
 * @data_type: var type ID (app, os, certificate, ...)
 * @data_length: length of data part
 * @data_part: pure FLASH data (TI9x only)
 * @num_pages: number of FLASH pages (TI8x only)
 * @pages: array of FLASH pages (TI8x only)
 * @next: pointer to next structure (linked list) (TI9x only).
 *
 * A generic structure used to store the content of a FLASH file (os or app).
 **/
typedef struct _FlashContent FlashContent;
struct _FlashContent
{
  CalcModel		model;

  uint8_t		revision_major;
  uint8_t		revision_minor;
  uint8_t		flags;
  uint8_t		object_type;
  uint8_t		revision_day;
  uint8_t		revision_month;
  uint16_t		revision_year;
  char			name[9];
  uint8_t		device_type;
  uint8_t		data_type;
  uint32_t		data_length;
  uint8_t*		data_part;	// TI9x only

  int			num_pages;	// TI8x only
  FlashPage*	pages;		// TI8x only

  FlashContent*	next;		// TI9x only
};

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
  TIEXPORT CalcModel   TICALL tifiles_string_to_model (const char *str);
  
  TIEXPORT const char* TICALL tifiles_attribute_to_string (FileAttr atrb);
  TIEXPORT FileAttr    TICALL tifiles_string_to_attribute (const char *str);
  
  TIEXPORT const char* TICALL tifiles_class_to_string (FileClass type);
  TIEXPORT FileClass   TICALL tifiles_string_to_class (const char *str);

  // transcode.c
  TIEXPORT char* TICALL tifiles_transcode_tokenize   (CalcModel model, char *dst, const char *src, uint8_t vartype);
  TIEXPORT char* TICALL tifiles_transcode_detokenize (CalcModel model, char *dst, const char *src, uint8_t vartype);

  TIEXPORT char* TICALL tifiles_transcode_to_ascii   (CalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_latin1  (CalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_unicode (CalcModel model, char* dst, const char *src);

  TIEXPORT void         TICALL tifiles_transcoding_set (FileEncoding encoding);
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
  TIEXPORT CalcModel    TICALL tifiles_signature2calctype (const char *signature);

  TIEXPORT const uint8_t TICALL tifiles_folder_type (CalcModel model);
  TIEXPORT const uint8_t TICALL tifiles_flash_type (CalcModel model);
  TIEXPORT const uint8_t TICALL tifiles_idlist_type (CalcModel model);

  // misc.c
  TIEXPORT int TICALL tifiles_calc_is_ti8x (CalcModel model);
  TIEXPORT int TICALL tifiles_calc_is_ti9x (CalcModel model);

  TIEXPORT int TICALL tifiles_has_folder (CalcModel model);
  TIEXPORT int TICALL tifiles_is_flash (CalcModel model);

  TIEXPORT uint16_t TICALL tifiles_checksum(uint8_t * buffer, int size);
  TIEXPORT int             tifiles_hexdump(uint8_t* ptr, unsigned int length);

  TIEXPORT char* TICALL tifiles_get_varname(const char *full_name);
  TIEXPORT char* TICALL tifiles_get_fldname(const char *full_name);
  TIEXPORT char* TICALL tifiles_build_fullname(CalcModel model,
	                     char *full_name,
					     const char *fldname,
					     const char *varname);

  // filesXX.c
  TIEXPORT FileContent* TICALL tifiles_content_create_regular(void);
  TIEXPORT int          TICALL tifiles_content_free_regular(FileContent *content);
  TIEXPORT int TICALL tifiles_file_read_regular(const char *filename, FileContent *content);
  TIEXPORT int TICALL tifiles_file_write_regular(const char *filename, FileContent *content, char **filename2);
  TIEXPORT int TICALL tifiles_file_display_regular(FileContent *content);

  TIEXPORT BackupContent* TICALL tifiles_content_create_backup(void);
  TIEXPORT int            TICALL tifiles_content_free_backup(BackupContent *content);
  TIEXPORT int TICALL tifiles_file_read_backup(const char *filename, BackupContent *content);
  TIEXPORT int TICALL tifiles_file_write_backup(const char *filename, BackupContent *content);
  TIEXPORT int TICALL tifiles_file_display_backup(BackupContent *content);

  TIEXPORT FlashContent* TICALL tifiles_content_create_flash(void);
  TIEXPORT int           TICALL tifiles_content_free_flash(FlashContent *content);
  TIEXPORT int TICALL tifiles_file_read_flash(const char *filename, FlashContent *content);
  TIEXPORT int TICALL tifiles_file_write_flash(const char *filename, FlashContent *content);
  TIEXPORT int TICALL tifiles_file_display_flash(FlashContent *content);

  TIEXPORT int** TICALL tifiles_create_table_of_entries(FileContent *content, int *nfolders);
  TIEXPORT int TICALL tifiles_file_display(const char *filename);

  // grouped.c
  TIEXPORT int TICALL tifiles_group_contents(FileContent **src_contents, FileContent **dst_content);
  TIEXPORT int TICALL tifiles_ungroup_content(FileContent *src_content, FileContent ***dst_contents);

  TIEXPORT int TICALL tifiles_group_files(char **src_filenames, const char *dst_filename);
  TIEXPORT int TICALL tifiles_ungroup_file(const char *src_filename, char ***dst_filenames);

  TIEXPORT int TICALL tifiles_content_free_group(FileContent **array);

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
