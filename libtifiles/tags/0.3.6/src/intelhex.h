/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifndef __TIFILES__INTELHEX__
#define __TIFILES__INTELHEX__

#include "macros.h"

#define MODE_APPS              (1<<11) // Send a (free) FLASH application
#define MODE_AMS               (1<<12) // Send an Operating System (AMS)

int read_data_block(FILE *f, 
		    uint16_t *flash_address, uint16_t *flash_page, 
		    uint8_t *data, int mode);

int write_data_block(FILE *f, 
		     uint16_t flash_address, uint16_t flash_page, 
		     uint8_t *data, int mode);

#endif




