/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libTIFILES - file format library, a part of the TiLP project
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

#ifndef __TIFILES_H__
#define __TIFILES_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "export2.h"
#include "stdints2.h"
#include "typesxx.h"

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Conditionnals */

//#define CHECKSUM_ENABLED

/* Versioning */

#ifdef __WIN32__
# define LIBFILES_VERSION "1.1.7"
#else
# define LIBFILES_VERSION VERSION
#endif

/* Types */

// TI-XX: up to 4*8 chars + 1, NSpire, up to 4*255 chars + 1
#define FLDNAME_MAX		1024
#define VARNAME_MAX		1024
#define FULNAME_MAX		2048

/**
 * CalcModel:
 *
 * An enumeration which contains the following calculator types:
 **/
#if !defined(__TICONV_H__)
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
	CALC_TI84P_USB, CALC_TI89T_USB, CALC_NSPIRE, CALC_TI80,
	CALC_TI84PC, CALC_TI84PC_USB, CALC_MAX
} CalcModel;
#endif

#define FILES_NCALCS	CALC_MAX	// # of supported calcs

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
	TIFILE_SINGLE = 1, TIFILE_GROUP = 2, TIFILE_REGULAR = 3, TIFILE_BACKUP = 4,
	TIFILE_FLASH = 8, TIFILE_TIGROUP = 16, TIFILE_OS = 32, TIFILE_APP = 64,
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

/**
 * DeviceType:
 *
 * An enumeration which contains soem device IDs for FLASH apps:
 **/
typedef enum
{
	DEVICE_TYPE_83P = 0x73,
	DEVICE_TYPE_73	= 0x74,
	DEVICE_TYPE_89  = 0x98,
	DEVICE_TYPE_92P = 0x88,
} DeviceType;

/* Structures (common to all calcs) */

/**
 * VarEntry:
 * @folder: name of folder (TI9x only) or ""
 * @name: name of variable (raw on-calc encoding)
 * @type: vartype ID
 * @attr: TI83+/89/92+ only (ATTRB_NONE or ARCHIVED)
 * @version: version compatibility level (TI83+/84+ only)
 * @size: size of data (uint16_t for TI8x)
 * @data: pure data
 * @action: used by ticalcs library (must be set to 0)
 *
 * A generic structure used to store the content of a TI variable.
 **/
typedef struct 
{
	char		folder[FLDNAME_MAX];
	char		name[VARNAME_MAX];

	uint8_t		type;
	uint8_t		attr;
	uint8_t		version;
	uint32_t	size;
	uint8_t*	data;

	int			action;
} VarEntry;

/**
 * FileContent:
 * @model: calculator model
 * @default_folder: name of the default folder (TI9x only)
 * @comment: comment aembedded in file (like "Single file received by TiLP")
 * @num_entries: number of variables stored after
 * @entries: a NULL-terminated array of #TiVarEntry structures
 * @checksum: checksum of file
 *
 * A generic structure used to store the content of a single/grouped TI file.
 * Please note that:
 * - entries must be NULL-terminated. Parsing is based on this property.
 * - num_entries must be exact. Counting relies on this.
 *
 * Note: NSpire uses only one entry (no grouping support).
 *
 **/
typedef struct 
{
	CalcModel		model;

	char			default_folder[FLDNAME_MAX];	// TI9x only
	char			comment[43];					// Ti8x: 42 max, Ti9x: 40 max

	int			num_entries;
	VarEntry**	entries;

	uint16_t		checksum;

	CalcModel		model_dst;
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

#define FLASH_PAGE_SIZE	16384

/**
 * FlashHeader:
 * @revision_major:
 * @revision_minor:
 * @flags:
 * @object_type:
 * @revision_day:
 * @revision_month:
 * @revision_year: 
 * @name: name of FLASH app or "basecode" for OS or '' for certificate
 * @device_type: a device ID (TI89: 0x88, TI92+:0x98, ...)
 * @data_type: var type ID (app, os, certificate, ...)
 * @data_length: length of data part
 *
 * A generic structure used to store the header before data of a FLASH file.
 **/
typedef struct 
{
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
} FlashHeader;

/**
 * FlashPage:
 * @offset: FLASH offset (see TI link guide).
 * @page: FLASH page (see TI link guide).
 * @flag: see link guide.
 * @size: length of pure data (up to 16384 bytes)
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
 * @name: name of FLASH app or OS
 * @device_type: a device ID
 * @data_type: a type ID
 * @hw_id: hardware ID (used on TI9x only, 0 otherwise)
 * @data_length: length of pure data
 * @data_part: pure FLASH data (TI9x only) or license or certificate
 * @num_pages: number of FLASH pages (TI8x only)
 * @pages: NULL-terminated array of FLASH pages (TI8x only)
 * @next: pointer to next structure (linked list of contents)
 *
 * A generic structure used to store the content of a FLASH file (os or app).
 **/
typedef struct _FlashContent FlashContent;
struct _FlashContent
{
	CalcModel		model;

	//FlashHeader	header;
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
	uint8_t		hw_id;
	uint32_t		data_length;

	uint8_t*		data_part;	// TI9x only
	int			num_pages;	// TI8x only
	FlashPage**	pages;		// TI8x only

	FlashContent*	next;		// TI9x only
};

typedef struct
{
	char*		filename;
	FileClass	type;
	union {
		FileContent*	regular;
		FlashContent*	flash;
		void*			data;
	} content;
} TigEntry;

/**
 * TigContent:
 * @model: a calculator model
 * @comment: a global comment for archive
 * @comp_level: compression level (0: store, 1 to 4: slow to fast)
 * @num_entries: the number of entries
 * @entries: a NULL-terminated array of #TigEntry structures
 *
 * A generic structure used to store the content of a TiGroup file.
 **/
typedef struct 
{
	CalcModel			model;
	char*				comment;
	int				comp_level;

	TigEntry**		var_entries;
	int				n_vars;

	TigEntry**		app_entries;
	int				n_apps;

	CalcModel			model_dst;
} TigContent;

/* Functions */

// namespace scheme: library_class_function like tifiles_fext_get

#ifdef __cplusplus
extern "C" {
#endif

	/****************/
	/* Entry points */
	/****************/

	TIEXPORT2 int TICALL tifiles_library_init(void);
	TIEXPORT2 int TICALL tifiles_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// tifiles.c
	TIEXPORT2 const char* TICALL tifiles_version_get (void);

	// error.c
	TIEXPORT2 int         TICALL tifiles_error_get (int number, char **message);

	// type2str.c
	TIEXPORT2 const char* TICALL tifiles_model_to_string (CalcModel type);
	TIEXPORT2 CalcModel   TICALL tifiles_string_to_model (const char *str);

	TIEXPORT2 const char* TICALL tifiles_attribute_to_string (FileAttr atrb);
	TIEXPORT2 FileAttr    TICALL tifiles_string_to_attribute (const char *str);

	TIEXPORT2 const char* TICALL tifiles_class_to_string (FileClass type);
	TIEXPORT2 FileClass   TICALL tifiles_string_to_class (const char *str);

	// filetypes.c
	TIEXPORT2 const char* TICALL tifiles_fext_of_group    (CalcModel model);
	TIEXPORT2 const char* TICALL tifiles_fext_of_backup   (CalcModel model);
	TIEXPORT2 const char* TICALL tifiles_fext_of_flash_app(CalcModel model);
	TIEXPORT2 const char* TICALL tifiles_fext_of_flash_os (CalcModel model);
	TIEXPORT2 const char* TICALL tifiles_fext_of_certif   (CalcModel model);

	TIEXPORT2 char* TICALL tifiles_fext_get (const char *filename);
	TIEXPORT2 char* TICALL tifiles_fext_dup (const char *filename);

	TIEXPORT2 int TICALL tifiles_file_is_ti (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_single (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_group (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_regular (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_backup (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_os(const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_app(const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_tib (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_flash (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_tigroup (const char *filename);
	TIEXPORT2 int TICALL tifiles_file_is_tno(const char *filename);

	TIEXPORT2 int TICALL tifiles_file_test(const char *filename, FileClass type, CalcModel target);

	TIEXPORT2 CalcModel TICALL tifiles_file_get_model (const char *filename);
	TIEXPORT2 FileClass TICALL tifiles_file_get_class (const char *filename);

	TIEXPORT2 const char* TICALL tifiles_file_get_type (const char *filename);
	TIEXPORT2 const char* TICALL tifiles_file_get_icon (const char *filename);

	// typesXX.c
	TIEXPORT2 const char* TICALL tifiles_vartype2string (CalcModel model, uint8_t data);
	TIEXPORT2 uint8_t     TICALL tifiles_string2vartype (CalcModel model, const char *s);

	TIEXPORT2 const char* TICALL tifiles_vartype2fext (CalcModel model, uint8_t data);
	TIEXPORT2 uint8_t     TICALL tifiles_fext2vartype (CalcModel model, const char *s);

	TIEXPORT2 const char* TICALL tifiles_vartype2type (CalcModel model, uint8_t id);
	TIEXPORT2 const char* TICALL tifiles_vartype2icon (CalcModel model, uint8_t id);

	TIEXPORT2 const char*  TICALL tifiles_calctype2signature (CalcModel model);
	TIEXPORT2 CalcModel    TICALL tifiles_signature2calctype (const char *signature);

	TIEXPORT2 uint8_t TICALL tifiles_folder_type (CalcModel model);
	TIEXPORT2 uint8_t TICALL tifiles_flash_type (CalcModel model);
	TIEXPORT2 uint8_t TICALL tifiles_idlist_type (CalcModel model);

	// misc.c
	TIEXPORT2 int TICALL tifiles_calc_is_ti8x (CalcModel model);
	TIEXPORT2 int TICALL tifiles_calc_is_ti9x (CalcModel model);

	TIEXPORT2 int TICALL tifiles_calc_are_compat(CalcModel model1, CalcModel model2);

	TIEXPORT2 int TICALL tifiles_has_folder (CalcModel model);
	TIEXPORT2 int TICALL tifiles_is_flash (CalcModel model);
	TIEXPORT2 int TICALL tifiles_has_backup(CalcModel model);

	TIEXPORT2 uint16_t TICALL tifiles_checksum(uint8_t * buffer, int size);
	TIEXPORT2 int             tifiles_hexdump(uint8_t* ptr, unsigned int length);

	TIEXPORT2 char* TICALL tifiles_get_varname(const char *full_name);
	TIEXPORT2 char* TICALL tifiles_get_fldname(const char *full_name);
	TIEXPORT2 char* TICALL tifiles_build_fullname(CalcModel model, char *full_name, const char *fldname, const char *varname);
	TIEXPORT2 char* TICALL tifiles_build_filename(CalcModel model, const VarEntry *ve);

	// filesXX.c
	TIEXPORT2 FileContent* TICALL tifiles_content_create_regular(CalcModel model);
	TIEXPORT2 int          TICALL tifiles_content_delete_regular(FileContent *content);
	TIEXPORT2 int TICALL tifiles_file_read_regular(const char *filename, FileContent *content);
	TIEXPORT2 int TICALL tifiles_file_write_regular(const char *filename, FileContent *content, char **filename2);
	TIEXPORT2 int TICALL tifiles_file_display_regular(FileContent *content);

	TIEXPORT2 BackupContent* TICALL tifiles_content_create_backup(CalcModel model);
	TIEXPORT2 int            TICALL tifiles_content_delete_backup(BackupContent *content);
	TIEXPORT2 int TICALL tifiles_file_read_backup(const char *filename, BackupContent *content);
	TIEXPORT2 int TICALL tifiles_file_write_backup(const char *filename, BackupContent *content);
	TIEXPORT2 int TICALL tifiles_file_display_backup(BackupContent *content);

	TIEXPORT2 FlashContent* TICALL tifiles_content_create_flash(CalcModel model);
	TIEXPORT2 int           TICALL tifiles_content_delete_flash(FlashContent *content);
	TIEXPORT2 int TICALL tifiles_file_read_flash(const char *filename, FlashContent *content);
	TIEXPORT2 int TICALL tifiles_file_write_flash (const char *filename, FlashContent *content);
	TIEXPORT2 int TICALL tifiles_file_write_flash2(const char *filename, FlashContent *content, char **filename2);
	TIEXPORT2 int TICALL tifiles_file_display_flash(FlashContent *content);

	TIEXPORT2 FileContent*  TICALL tifiles_content_dup_regular(FileContent *content);
	TIEXPORT2 FlashContent* TICALL tifiles_content_dup_flash(FlashContent *content);

	TIEXPORT2 int TICALL tifiles_file_display(const char *filename);

	// grouped.c
	TIEXPORT2 FileContent** TICALL tifiles_content_create_group(int n_entries);
	TIEXPORT2 int           TICALL tifiles_content_delete_group(FileContent **array);

	TIEXPORT2 int TICALL tifiles_group_contents(FileContent **src_contents, FileContent **dst_content);
	TIEXPORT2 int TICALL tifiles_ungroup_content(FileContent *src_content, FileContent ***dst_contents);

	TIEXPORT2 int TICALL tifiles_group_files(char **src_filenames, const char *dst_filename);
	TIEXPORT2 int TICALL tifiles_ungroup_file(const char *src_filename, char ***dst_filenames);


	TIEXPORT2 int TICALL tifiles_content_add_entry(FileContent *content, VarEntry *ve);
	TIEXPORT2 int TICALL tifiles_content_del_entry(FileContent *content, VarEntry *ve);

	TIEXPORT2 int TICALL tifiles_group_add_file(const char *src_filename, const char *dst_filename);
	TIEXPORT2 int TICALL tifiles_group_del_file(VarEntry *entry,          const char *dst_filename);

	// tigroup.c -> filesXX.c
	TIEXPORT2 TigContent* TICALL tifiles_content_create_tigroup(CalcModel model, int);
	TIEXPORT2 int         TICALL tifiles_content_delete_tigroup(TigContent *content);

	TIEXPORT2 int TICALL tifiles_file_read_tigroup(const char *filename, TigContent *content);
	TIEXPORT2 int TICALL tifiles_file_write_tigroup(const char *filename, TigContent *content);
	TIEXPORT2 int TICALL tifiles_file_display_tigroup(const char *filename);

	// tigroup.c -> grouped.c
	TIEXPORT2 int TICALL tifiles_tigroup_contents(FileContent **src_contents1, FlashContent **src_contents2, TigContent **dst_content);
	TIEXPORT2 int TICALL tifiles_untigroup_content(TigContent *src_content, FileContent ***dst_contents1, FlashContent ***dst_contents2);

	TIEXPORT2 int TICALL tifiles_tigroup_files(char **src_filenames, const char *dst_filename);
	TIEXPORT2 int TICALL tifiles_untigroup_file(const char *src_filename, char ***dst_filenames);


	TIEXPORT2 int TICALL tifiles_content_add_te(TigContent *content, TigEntry *te);
	TIEXPORT2 int TICALL tifiles_content_del_te(TigContent *content, TigEntry *te);

	TIEXPORT2 int TICALL tifiles_tigroup_add_file(const char *src_filename, const char *dst_filename);
	TIEXPORT2 int TICALL tifiles_tigroup_del_file(TigEntry *entry,          const char *dst_filename);

	// tigroup.c -> ve_fp.c
	TIEXPORT2 TigEntry* TICALL tifiles_te_create(const char *filename, FileClass type, CalcModel model);
	TIEXPORT2 int		  TICALL tifiles_te_delete(TigEntry* entry);

	TIEXPORT2 TigEntry**	TICALL tifiles_te_create_array(int nelts);
	TIEXPORT2 TigEntry**	TICALL tifiles_te_resize_array(TigEntry**, int nelts);
	TIEXPORT2 void		TICALL tifiles_te_delete_array(TigEntry**);
	TIEXPORT2 int			TICALL tifiles_te_sizeof_array(TigEntry**);

	// comment.c
	TIEXPORT2 const char* TICALL tifiles_comment_set_single(void);
	TIEXPORT2 const char* TICALL tifiles_comment_set_group(void);
	TIEXPORT2 const char* TICALL tifiles_comment_set_backup(void);
	TIEXPORT2 const char* TICALL tifiles_comment_set_tigroup(void);

	// ve_fp.c
	TIEXPORT2 VarEntry*	TICALL tifiles_ve_create(void);
	TIEXPORT2 VarEntry*	TICALL tifiles_ve_create_with_data(uint32_t size);
	TIEXPORT2 void		TICALL tifiles_ve_delete(VarEntry*);

	TIEXPORT2 void*		tifiles_ve_alloc_data(size_t size);
	TIEXPORT2 VarEntry*	TICALL tifiles_ve_copy(VarEntry* dst, VarEntry* src);
	TIEXPORT2 VarEntry*	TICALL tifiles_ve_dup(VarEntry* src);

	TIEXPORT2 VarEntry**	TICALL tifiles_ve_create_array(int nelts);
	TIEXPORT2 VarEntry**	TICALL tifiles_ve_resize_array(VarEntry**, int nelts);
	TIEXPORT2 void		TICALL tifiles_ve_delete_array(VarEntry**);

	TIEXPORT2 FlashPage*	TICALL tifiles_fp_create(void);
	TIEXPORT2 FlashPage*	TICALL tifiles_fp_create_with_data(uint32_t size);
	TIEXPORT2 void		TICALL tifiles_fp_delete(FlashPage*);

	TIEXPORT2 void*		TICALL tifiles_fp_alloc_data(size_t size);

	TIEXPORT2 FlashPage**	TICALL tifiles_fp_create_array(int nelts);
	TIEXPORT2 void		TICALL tifiles_fp_delete_array(FlashPage**);

	// undocumented
	TIEXPORT2 int** tifiles_create_table_of_entries(FileContent *content, int *nfolders);
	TIEXPORT2 void tifiles_free_table_of_entries(int ** table);

	/************************/
	/* Deprecated functions */
	/************************/

#ifdef __cplusplus
}
#endif

#endif
