/*  libtifiles - TI File Format library
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

#ifndef __TIFILES_ERRCODES__
#define __TIFILES_ERRCODES__

/* Error codes must begin at 512 up to 767 */

#define ERR_MALLOC           512	// Error with malloc
#define ERR_FILE_OPEN        513	// Unable to open file
#define ERR_FILE_CLOSE       514	// Unable to close file
#define ERR_GROUP_SIZE       515	// Group size exceeded (>64KB)
#define ERR_BAD_CALC	     516	// The function does not exist for this calc
#define ERR_INVALID_FILE     517	// Is not a TI file
#define ERR_BAD_FILE         518	// Same as above
#define ERR_FILE_CHECKSUM    519	// Checksum error

#endif
