/* Hey EMACS -*- linux-c -*- */
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

#ifndef __TIFILES_FILES8X_H__
#define __TIFILES_FILES8X_H__

#include "export.h"
#include "stdints.h"
#include "files8x.h"

/* Structures */

// defined in filesxx.h (common to all calcs)
typedef TiVarEntry Ti8xVarEntry;
typedef TiRegular Ti8xRegular;

typedef struct {
  TiCalcModel calc_type;

  char comment[43];
  uint8_t type;
  uint16_t mem_address;
  uint16_t data_length1;
  uint8_t *data_part1;
  uint16_t data_length2;
  uint8_t *data_part2;
  uint16_t data_length3;
  uint8_t *data_part3;
  uint16_t data_length4;	// TI86 only
  uint8_t *data_part4;
  uint16_t checksum;

} Ti8xBackup;


typedef struct {

  uint16_t offset;
  uint16_t page;
  uint8_t flag;
  uint32_t length;
  uint8_t *data;

} Ti8xFlashPage;

typedef struct ti8x_flash Ti8xFlash;
struct ti8x_flash {
  TiCalcModel calc_type;

  uint8_t revision_major;
  uint8_t revision_minor;
  uint8_t flags;
  uint8_t object_type;
  uint8_t revision_day;
  uint8_t revision_month;
  uint16_t revision_year;
  char name[9];
  uint8_t device_type;
  uint8_t data_type;

  int num_pages;
  Ti8xFlashPage *pages;
};

#define DEVICE_TYPE_83P 0x73
#define DEVICE_TYPE_73  0x74

/* Functions */

// allocating
TIEXPORT Ti8xRegular *TICALL ti8x_create_regular_content(void);
TIEXPORT Ti8xBackup *TICALL ti8x_create_backup_content(void);
TIEXPORT Ti8xFlash *TICALL ti8x_create_flash_content(void);
// freeing
TIEXPORT int TICALL ti8x_free_regular_content(Ti8xRegular * content);
TIEXPORT int TICALL ti8x_free_backup_content(Ti8xBackup * content);
TIEXPORT int TICALL ti8x_free_flash_content(Ti8xFlash * content);
// reading
TIEXPORT int TICALL ti8x_read_regular_file(const char *filename,
					   Ti8xRegular * content);
TIEXPORT int TICALL ti8x_read_backup_file(const char *filename,
					  Ti8xBackup * content);
TIEXPORT int TICALL ti8x_read_flash_file(const char *filename,
					 Ti8xFlash * content);
// writing
TIEXPORT int TICALL ti8x_write_regular_file(const char *filename,
					    Ti8xRegular * content,
					    char **filename2);
TIEXPORT int TICALL ti8x_write_backup_file(const char *filename,
					   Ti8xBackup * content);
TIEXPORT int TICALL ti8x_write_flash_file(const char *filename,
					  Ti8xFlash * content);
// displaying
TIEXPORT int TICALL ti8x_display_regular_content(Ti8xRegular * content);
TIEXPORT int TICALL ti8x_display_backup_content(Ti8xBackup * content);
TIEXPORT int TICALL ti8x_display_flash_content(Ti8xFlash * content);
TIEXPORT int TICALL ti8x_display_file(const char *filename);

int ti8x_dup_VarEntry(Ti8xVarEntry *dst, Ti8xVarEntry *src);

#endif
