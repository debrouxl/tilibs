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

#ifndef __TIFILES_FILES9X_H__
#define __TIFILES_FILES9X_H__

#include "stdints.h"
#include "tifiles.h"

/* Structures */

/**
 * Ti9xRegular:
 * @model: calculator model
 * @default_folder: name of the default folder (TI9x only)
 * @comment: comment aembedded in file (like "Single file received by TiLP")
 * @num_entries: number of variables stored after
 * @entries: an array of #TiVarEntry structures which contains data
 * @checksum: checksum of file
 *
 * A generic structure used to store the content of a single/grouped TI file.
 * This structure is the same as Ti9xRegular.
 **/
typedef struct 
{
  CalcModel		model;

  char			default_folder[9];
  char			comment[43];

  int			num_entries;
  VarEntry*		entries;

  uint16_t		checksum;

} Ti9xRegular;

/**
 * Ti9xBackup:
 * @model: calculator moel.
 * @comment: comment embedded in file.
 * @rom_version: ROM version  (such as "2.1").
 * @type: a variable type ID for backup.
 * @data_length: size of data part.
 * @data_part: pure backup data.
 * @checksum: checksum of file.
 *
 * A generic structure used to store the content of a TI92 backup file.
 **/
typedef struct 
{
  CalcModel model;

  char		comment[41];
  char		rom_version[9];
  uint8_t	type;
  uint32_t	data_length;
  uint8_t*	data_part;
  uint16_t	checksum;

} Ti9xBackup;

/**
 * Ti9xFlash:
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
 * @data_part: pure FLASH data
 * @next: pointer to next #Ti9xFlash structure (linked list).
 *
 * A generic structure used to store the content of a TI9x FLASH file (os or app).
 **/
typedef struct ti9x_flash Ti9xFlash;
struct ti9x_flash 
{
  CalcModel model;

  uint8_t	revision_major;
  uint8_t	revision_minor;
  uint8_t	flags;
  uint8_t	object_type;
  uint8_t	revision_day;
  uint8_t	revision_month;
  uint16_t	revision_year;
  char		name[9];
  uint8_t	device_type;
  uint8_t	data_type;
  uint32_t	data_length;
  uint8_t*	data_part;

  Ti9xFlash *next;
};

#define DEVICE_TYPE_89  0x98
#define DEVICE_TYPE_92P 0x88

/* Functions */

// allocating
TIEXPORT Ti9xRegular* TICALL ti9x_content_create_regular(void);
TIEXPORT Ti9xBackup*  TICALL ti9x_content_create_backup(void);
TIEXPORT Ti9xFlash*   TICALL ti9x_content_create_flash(void);
// freeing
TIEXPORT void TICALL ti9x_content_free_regular(Ti9xRegular *content);
TIEXPORT void TICALL ti9x_content_free_backup(Ti9xBackup *content);
TIEXPORT void TICALL ti9x_content_free_flash(Ti9xFlash *content);
//displaying
TIEXPORT int TICALL ti9x_content_display_regular(Ti9xRegular *content);
TIEXPORT int TICALL ti9x_content_display_backup(Ti9xBackup *content);
TIEXPORT int TICALL ti9x_content_display_flash(Ti9xFlash *content);

// reading
TIEXPORT int TICALL ti9x_file_read_regular(const char *filename, Ti9xRegular *content);
TIEXPORT int TICALL ti9x_file_read_backup(const char *filename, Ti9xBackup *content);
TIEXPORT int TICALL ti9x_file_read_flash(const char *filename, Ti9xFlash *content);
// writing
TIEXPORT int TICALL ti9x_file_write_regular(const char *filename, Ti9xRegular *content, char **filename2);
TIEXPORT int TICALL ti9x_file_write_backup(const char *filename, Ti9xBackup *content);
TIEXPORT int TICALL ti9x_file_write_flash(const char *filename, Ti9xFlash *content);
//displaying
TIEXPORT int TICALL ti9x_file_display(const char *filename);

#endif
