/*  libtifiles - TI File Format and Types library
 *  Copyright (C) 2002-2003  Romain Lievin
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

#ifndef __TIFILES_FILEINT__
#define __TIFILES_FILEINT__

#include <stdio.h>
#include "stdints.h"

#include "export.h"
#include "file_def.h"

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  
  TIEXPORT int TICALL tifiles_init(void);
  TIEXPORT int TICALL tifiles_exit(void);

  /*********************/
  /* General functions */
  /*********************/

  // intrface.c
  TIEXPORT const char *TICALL tifiles_get_version(void);
  TIEXPORT int TICALL tifiles_get_error(int err_num, char *error_msg);

  TIEXPORT void TICALL tifiles_set_calc(TicalcType type);
  TIEXPORT TicalcType TICALL tifiles_get_calc(void);

  TIEXPORT TIFILES_PRINTF tifiles_set_printf(TIFILES_PRINTF);

  // for win32 (DLL partition -> memory violation)
#ifdef __WIN32__
  TIEXPORT void *TICALL tifiles_calloc(size_t nmemb, size_t size);
  TIEXPORT void *TICALL tifiles_malloc(size_t size);
  TIEXPORT void TICALL tifiles_free(void *ptr);
  TIEXPORT void *TICALL tifiles_realloc(void *ptr, size_t size);
#else
# define tifiles_calloc  calloc
# define tifiles_malloc  malloc
# define tifiles_free    free
# define tifiles_realloc realloc
#endif

  // currently unused
  extern void (*tifiles_msgbox) (const char *t, char *s);
  extern int (*tifiles_choose) (char *cur_name, char *new_name);
  extern int (*tifiles_printf) (const char *format, ...);

  // trans.c
  TIEXPORT uint8_t TICALL *tixx_detokenize_varname(const char *varname, 
						   char *translate,
						   uint8_t vartype,
						   TicalcType calc_type);

  TIEXPORT char* TICALL tifiles_transcode_to_ascii(char* dst, 
						   const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_latin1(char* dst, 
						    const char *src);
  TIEXPORT char* TICALL tifiles_transcode_to_unicode(char* dst, 
						     const char *src);

  TIEXPORT void TICALL tifiles_translate_set_encoding(TifileEncoding encoding);
  TIEXPORT TifileEncoding TICALL tifiles_translate_get_encoding(void);

  TIEXPORT char *TICALL tifiles_translate_varname(const char *varname, 
						  char *translate, 
						  uint8_t vartype);
  TIEXPORT char *TICALL tifiles_translate_varname_static(const char
							 *varname,
							 uint8_t vartype);
  
  // typesXX.c
  TIEXPORT const char *TICALL tifiles_vartype2string(uint8_t data);
  TIEXPORT uint8_t TICALL tifiles_string2vartype(const char *s);
  TIEXPORT const char *TICALL tifiles_vartype2file(uint8_t data);
  TIEXPORT uint8_t TICALL tifiles_file2vartype(const char *s);

  TIEXPORT const char *TICALL tifiles_vartype2icon(uint8_t data);
  TIEXPORT const char *TICALL tifiles_vartype2desc(uint8_t data);

  TIEXPORT const char *TICALL tifiles_group_file_ext(void);
  TIEXPORT const char *TICALL tifiles_backup_file_ext(void);
  TIEXPORT const char *TICALL tifiles_flash_app_file_ext(void);
  TIEXPORT const char *TICALL tifiles_flash_os_file_ext(void);

  TIEXPORT int TICALL tifiles_is_a_ti_file(const char *filename);
  TIEXPORT int TICALL tifiles_is_a_single_file(const char *filename);
  TIEXPORT int TICALL tifiles_is_a_group_file(const char *filename);
  TIEXPORT int TICALL tifiles_is_a_regular_file(const char *filename);
  TIEXPORT int TICALL tifiles_is_a_backup_file(const char *filename);
  TIEXPORT int TICALL tifiles_is_a_flash_file(const char *filename);

  TIEXPORT int TICALL tifiles_which_calc_type(const char *filename);
  TIEXPORT int TICALL tifiles_which_file_type(const char *filename);

  TIEXPORT const char *TICALL tifiles_file_descriptive(const char *filename);
  TIEXPORT const char * TICALL tifiles_file_icon(const char *filename);

  TIEXPORT const char *TICALL tifiles_calctype2signature(TicalcType calc_type);
  TIEXPORT TicalcType TICALL tifiles_signature2calctype(const char *signat);
	
  TIEXPORT const int TICALL tifiles_folder_type(void);
  TIEXPORT const int TICALL tifiles_flash_type(void);
  TIEXPORT const int TICALL tifiles_idlist_type(void);
  
  TIEXPORT int TICALL tifiles_is_ti8x(TicalcType calc_type);
  TIEXPORT int TICALL tifiles_is_ti9x(TicalcType calc_type);

  // misc.c
  TIEXPORT char *TICALL tifiles_get_extension(const char *filename);
  TIEXPORT char *TICALL tifiles_dup_extension(const char *filename);

  TIEXPORT uint16_t TICALL tifiles_compute_checksum(uint8_t * buffer,
						    int size);

  TIEXPORT char *TICALL tifiles_get_varname(const char *full_name);
  TIEXPORT char *TICALL tifiles_get_fldname(const char *full_name);
  TIEXPORT int TICALL tifiles_build_fullname(char *full_name,
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

  // grp_ops.c
  TIEXPORT int TICALL tifiles_group_contents(TiRegular ** srcs,
					     TiRegular ** dest);
  TIEXPORT int TICALL tifiles_ungroup_content(TiRegular * src,
					      TiRegular *** dest);

  TIEXPORT int TICALL tifiles_group_files(char **filenames,
					  const char *filename);
  TIEXPORT int TICALL tifiles_ungroup_file(const char *filename);

  // type2str.c
  TIEXPORT const char *TICALL tifiles_calctype_to_string(TicalcType type);
  TIEXPORT const char *TICALL tifiles_attribute_to_string(TifileAttr atrb);
  TIEXPORT const char *TICALL tifiles_filetype_to_string(TifileType type);

  /************************/
  /* Deprecated functions */
  /************************/

  TIEXPORT const char *TICALL tifiles_calc_type_to_string(void);  
  TIEXPORT int TICALL tifiles_is_silent(TicalcType calc_type);
  TIEXPORT int TICALL tifiles_has_folder(TicalcType calc_type);
  TIEXPORT int TICALL tifiles_is_flash(TicalcType calc_type);
  TIEXPORT const char *TICALL tifiles_vartype_to_file_extension(int vartype);
  TIEXPORT char *TICALL tifiles_translate_varname_static(const char
                                                         *varname,
                                                         uint8_t vartype);

#ifdef __cplusplus
}
#endif
#endif
