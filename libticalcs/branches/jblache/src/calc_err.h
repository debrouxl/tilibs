/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef __CALC_ERRCODES__
#define __CALC_ERRCODES__

/* Error codes: ERR_...  */
#define ERR_ABORT              -1 // Operation aborted
#define ERR_NOT_REPLY        4    // The calculator do not reply
#define ERR_NOT_ACK          5    // Calc did not acknowledge to remote control
#define ERR_CHECKSUM         7    // Checksum error
#define ERR_VAR_NOTEXIST     12   // The requested var does not exit
#define ERR_DISCONTINUE      16   // The calculator does not want continue
#define ERR_INVALID_TI92_FILE 18  // Invalid TI92 file
#define ERR_NOT_READY         20  // Calculator is not ready
#define ERR_BACKUP            21  // Backup, not a var
#define ERR_INVALID_BYTE      22  // Invalid byte received
#define ERR_INVALID_TI92p_FILE 23 // Invalid TI92+ file
#define ERR_INVALID_TI89_FILE 24  // Invalid TI89 file
#define ERR_VOID_FUNCTION 25      // This function does not exist for the calc
#define ERR_INVALID_TI82_FILE 26  // Invalid TI82 file
#define ERR_PACKET 27             // Packet error
#define ERR_SIZE 28               // Size error
#define ERR_INVALID_TI83_FILE 29  // Invalid TI83 file
#define ERR_INVALID_TI85_FILE 30  // Invalid TI85 file
#define ERR_INVALID_TI86_FILE 31  // Invalid TI86 file
#define ERR_INVALID_TI83p_FILE 32 // Invalid TI83+ file
// New error codes must begin at 200
#define ERR_INVALID_FLASH_FILE 200 // Nothing !
#define ERR_VAR_REFUSED        201
#define ERR_OUT_OF_MEMORY      202
#define ERR_INVALID_TIXX_FILE  203 // Invalid TIxx file
#define ERR_GRP_SIZE_EXCEEDED  204
#define ERR_OPEN_FILE          300
#define ERR_CLOSE_FILE         301

#endif



