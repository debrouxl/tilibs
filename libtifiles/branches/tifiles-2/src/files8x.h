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

#ifndef __TIFILES_FILES8X_H__
#define __TIFILES_FILES8X_H__

#include "stdints.h"
#include "files8x.h"
#include "tifiles.h"

/* Structures */

typedef FileContent		Ti8xRegular;
typedef FlashContent	Ti8xFlash;
typedef FlashPage		Ti8xFlashPage;

/**
 * Ti8xBackup:
 * @model: calculator model.
 * @comment: comment embedded in file.
 * @type: a variable type ID for backup.
 * @mem_address: memory address for backup
 * @data_lengthX: length of data part #X
 * @data_partX: pure backup data #X.
 * @checksum: checksum of file.
 *
 * A generic structure used to store the content of a TI8x backup file.
 **/
typedef struct 
{
  CalcModel model;

  char		comment[43];
  uint8_t	type;
  uint16_t	mem_address;
  uint16_t	data_length1;
  uint8_t*	data_part1;
  uint16_t	data_length2;
  uint8_t*	data_part2;
  uint16_t	data_length3;
  uint8_t*	data_part3;
  uint16_t	data_length4;	// TI86 only
  uint8_t*	data_part4;
  uint16_t	checksum;

} Ti8xBackup;

#define DEVICE_TYPE_83P 0x73
#define DEVICE_TYPE_73  0x74

/* Functions */

// allocating
TIEXPORT Ti8xRegular* TICALL ti8x_content_create_regular(void);
TIEXPORT Ti8xBackup*  TICALL ti8x_content_create_backup(void);
TIEXPORT Ti8xFlash*   TICALL ti8x_content_create_flash(void);
// freeing
TIEXPORT void TICALL ti8x_content_free_regular(Ti8xRegular *content);
TIEXPORT void TICALL ti8x_content_free_backup(Ti8xBackup *content);
TIEXPORT void TICALL ti8x_content_free_flash(Ti8xFlash *content);
// displaying
TIEXPORT int TICALL ti8x_content_display_regular(Ti8xRegular *content);
TIEXPORT int TICALL ti8x_content_display_backup(Ti8xBackup *content);
TIEXPORT int TICALL ti8x_content_display_flash(Ti8xFlash *content);

// reading
TIEXPORT int TICALL ti8x_file_read_regular(const char *filename, Ti8xRegular *content);
TIEXPORT int TICALL ti8x_file_read_backup(const char *filename, Ti8xBackup *content);
TIEXPORT int TICALL ti8x_file_read_flash(const char *filename, Ti8xFlash *content);
// writing
TIEXPORT int TICALL ti8x_file_write_regular(const char *filename, Ti8xRegular *content, char **filename2);
TIEXPORT int TICALL ti8x_file_write_backup(const char *filename, Ti8xBackup *content);
TIEXPORT int TICALL ti8x_file_write_flash(const char *filename, Ti8xFlash *content);
// displaying
TIEXPORT int TICALL ti8x_file_display(const char *filename);

#endif
