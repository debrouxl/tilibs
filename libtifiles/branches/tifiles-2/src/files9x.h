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

#ifndef __TIFILES_FILES9X_H__
#define __TIFILES_FILES9X_H__

#include "stdints.h"
#include "tifiles.h"

/* Structures */

// defined in filesxx.h (common to all calcs)
typedef TiVarEntry Ti9xVarEntry;
typedef TiRegular Ti9xRegular;

typedef struct {
  TiCalcModel calc_type;

  char comment[41];
  char rom_version[9];
  uint8_t type;
  uint32_t data_length;
  uint8_t *data_part;
  uint16_t checksum;

} Ti9xBackup;

typedef struct ti9x_flash Ti9xFlash;
struct ti9x_flash {
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
  uint32_t data_length;
  uint8_t *data_part;

  Ti9xFlash *next;
};

#define DEVICE_TYPE_89  0x98
#define DEVICE_TYPE_92P 0x88
// no legal existence, internal use
//#define DEVICE_TYPE_

/* Functions */

// allocating
TIEXPORT Ti9xRegular *TICALL ti9x_create_regular_content(void);
TIEXPORT Ti9xBackup *TICALL ti9x_create_backup_content(void);
TIEXPORT Ti9xFlash *TICALL ti9x_create_flash_content(void);
// freeing
TIEXPORT int TICALL ti9x_free_regular_content(Ti9xRegular * content);
TIEXPORT int TICALL ti9x_free_backup_content(Ti9xBackup * content);
TIEXPORT int TICALL ti9x_free_flash_content(Ti9xFlash * content);
// reading
TIEXPORT int TICALL ti9x_read_regular_file(const char *filename,
					   Ti9xRegular * content);
TIEXPORT int TICALL ti9x_read_backup_file(const char *filename,
					  Ti9xBackup * content);
TIEXPORT int TICALL ti9x_read_flash_file(const char *filename,
					 Ti9xFlash * content);
// writing
TIEXPORT int TICALL ti9x_write_regular_file(const char *filename,
					    Ti9xRegular * content,
					    char **filename2);
TIEXPORT int TICALL ti9x_write_backup_file(const char *filename,
					   Ti9xBackup * content);
TIEXPORT int TICALL ti9x_write_flash_file(const char *filename,
					  Ti9xFlash * content);
//displaying
TIEXPORT int TICALL ti9x_display_regular_content(Ti9xRegular * content);
TIEXPORT int TICALL ti9x_display_backup_content(Ti9xBackup * content);
TIEXPORT int TICALL ti9x_display_flash_content(Ti9xFlash * content);
TIEXPORT int TICALL ti9x_display_file(const char *filename);

#endif
