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

#include "export.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* Versioning */

#ifdef __WIN32__
# define LIBTIFILES_VERSION "0.0.1"
#else
# define LIBTIFILES_VERSION VERSION
#endif

/* Definitions */

#define VARNAME_MAX		18	// group/name: 8 + 1 + 8 + 1
#define TIFILES_NCALCS	12	// # of supported calcs

typedef enum {
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} TiCalcModel;

typedef enum {
  ATTRB_NONE = 0, ATTRB_LOCKED = 1, ATTRB_PROTECTED, ATTRB_ARCHIVED = 3
} TiFileAttr;

typedef enum {
  TIFILE_SINGLE = 1, TIFILE_GROUP = 2, TIFILE_BACKUP = 4, TIFILE_FLASH = 8,
} TiFileClass;

typedef enum {
  ENCODING_ASCII = 1, ENCODING_LATIN1, ENCODING_UNICODE
} TiFileEncoding;

#include "typesxx.h"
#include "filesxx.h"

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
  TIEXPORT const char* TICALL tifiles_version_get(void);

  // error.c
  TIEXPORT int         TICALL tifiles_error_get(int number, char **message);

  // type2str.c
  TIEXPORT const char* TICALL tifiles_model_to_string(TiCalcModel type);
  TIEXPORT TiCalcModel TICALL tifiles_string_to_model(const char *str);
  
  TIEXPORT const char* TICALL tifiles_attribute_to_string(TiFileAttr atrb);
  TIEXPORT TiFileAttr  TICALL tifiles_string_to_attribute(const char *str);
  
  TIEXPORT const char* TICALL tifiles_class_to_string(TiFileClass type);
  TIEXPORT TiFileClass TICALL tifiles_string_to_class(const char *str);

  // transcode.c
  TIEXPORT char* TICALL tixx_transcode_detokenize(TiCalcModel model, const char *src, char *dst, uint8_t vartype);

  TIEXPORT char* TICALL tifiles_transcode_to_ascii(TiCalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_latin1(TiCalcModel model, char* dst, const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_unicode(TiCalcModel model, char* dst, const char *src);

  TIEXPORT void TICALL tifiles_transcoding_set(TiFileEncoding encoding);
  TIEXPORT TiFileEncoding TICALL tifiles_transcoding_get(void);

  TIEXPORT char *TICALL tifiles_transcode_varname(TiCalcModel model, char *dst, const char *src, uint8_t vartype);
  TIEXPORT char *TICALL tifiles_transcode_varname_static(TiCalcModel model, const char *src, uint8_t vartype);

  // filetypes.c
  TIEXPORT const char* TICALL tifiles_fext_of_group(TiCalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_backup(TiCalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_flash_app(TiCalcModel model);
  TIEXPORT const char* TICALL tifiles_fext_of_flash_os(TiCalcModel model);

  TIEXPORT char* TICALL tifiles_fext_get(const char *filename);
  TIEXPORT char* TICALL tifiles_fext_dup(const char *filename);

  TIEXPORT int TICALL tifiles_file_is_ti(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_single(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_group(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_regular(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_backup(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_flash(const char *filename);
  TIEXPORT int TICALL tifiles_file_is_tib(const char *filename);

  TIEXPORT TiCalcModel TICALL tifiles_file_get_model(const char *filename);
  TIEXPORT TiFileClass TICALL tifiles_file_get_class(const char *filename);

  TIEXPORT const char* TICALL tifiles_file_get_type(const char *filename);
  TIEXPORT const char* TICALL tifiles_file_get_icon(const char *filename);
  
  // typesXX.c
  TIEXPORT const char* TICALL tifiles_vartype2string(uint8_t data);
  TIEXPORT uint8_t     TICALL tifiles_string2vartype(const char *s);

  TIEXPORT const char* TICALL tifiles_vartype2fext(uint8_t data);
  TIEXPORT uint8_t     TICALL tifiles_fext2vartype(const char *s);

  TIEXPORT const char* TICALL tifiles_vartype2icon(uint8_t data);
  TIEXPORT const char* TICALL tifiles_vartype2type(uint8_t data);

  TIEXPORT const char* TICALL tifiles_calctype2signature(TiCalcModel calc_type);
  TIEXPORT TiCalcModel  TICALL tifiles_signature2calctype(const char *signature);
	
  TIEXPORT const uint8_t TICALL tifiles_folder_type(void);
  TIEXPORT const uint8_t TICALL tifiles_flash_type(void);
  TIEXPORT const uint8_t TICALL tifiles_idlist_type(void); 

  // misc.c
  TIEXPORT int TICALL tifiles_calc_is_ti8x(TiCalcModel model);
  TIEXPORT int TICALL tifiles_calc_is_ti9x(TiCalcModel model);

  TIEXPORT uint16_t TICALL tifiles_checksum(uint8_t * buffer, int size);

  TIEXPORT char* TICALL tifiles_get_varname(const char *full_name);
  TIEXPORT char* TICALL tifiles_get_fldname(const char *full_name);
  TIEXPORT int   TICALL tifiles_build_fullname(TiCalcModel model,
	                     char *full_name,
					     const char *fldname,
					     const char *varname);

  // filesXX.c: layer built on files8x/9x
  TIEXPORT TiRegular *TICALL tifiles_create_regular_content(void);
  TIEXPORT int TICALL tifiles_free_regular_content(TiRegular * content);
  TIEXPORT int TICALL tifiles_read_regular_file(const char *filename,
						TiRegular * content);
  TIEXPORT int TICALL tifiles_write_regular_file(const char *filename,
						 TiRegular * content,
						 char **filename2);
  TIEXPORT int TICALL tifiles_display_file(const char *filename);

  TIEXPORT int TICALL tifiles_create_table_of_entries(TiRegular *content,
						      int ***tabl,
						      int *nfolders);

  // grouped.c
  TIEXPORT int TICALL tifiles_group_contents(TiRegular ** srcs,
					     TiRegular ** dest);
  TIEXPORT int TICALL tifiles_ungroup_content(TiRegular * src,
					      TiRegular *** dest);

  TIEXPORT int TICALL tifiles_group_files(char **filenames,
					  const char *filename);
  TIEXPORT int TICALL tifiles_ungroup_file(const char *filename);

  

  // special for win32 (DLL partition -> memory violation)
#ifdef __WIN32__
  TIEXPORT void* TICALL tifiles_calloc(size_t nmemb, size_t size);
  TIEXPORT void* TICALL tifiles_malloc(size_t size);
  TIEXPORT void  TICALL tifiles_free(void *ptr);
  TIEXPORT void* TICALL tifiles_realloc(void *ptr, size_t size);
#else
# define tifiles_calloc  calloc
# define tifiles_malloc  malloc
# define tifiles_free    free
# define tifiles_realloc realloc
#endif

  // deprecated
  // nothing yet

#ifdef __cplusplus
}
#endif

#endif
