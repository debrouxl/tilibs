/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TIFILES__INTELHEX__
#define __TIFILES__INTELHEX__

#include "macros.h"

#define PAGE_SIZE	16384	//(= FLASH_PAGE_SIZE)

int hex_block_read(FILE *f, uint16_t *size, uint16_t *addr, uint8_t *type, uint8_t *data, uint16_t *page);
int hex_block_write(FILE *f, uint16_t size, uint16_t  addr, uint8_t  type, uint8_t *data, uint16_t  page);

#endif
