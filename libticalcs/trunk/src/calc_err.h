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

#ifndef __CALC_ERRCODES__
#define __CALC_ERRCODES__

/* Error codes: ERR_...  */
/* Error codes must begin at 256 up to 512 */
#define ERR_ABORT              -1  // Operation aborted
#define ERR_NOT_REPLY          304 // The calculator do not reply
#define ERR_NOT_ACK            305 // Calc did not acknowledge
#define ERR_CHECKSUM           307 // Checksum error
#define ERR_VAR_NOTEXIST       312 // The requested var does not exit
#define ERR_DISCONTINUE        316 // The calculator does not want continue
#define ERR_INVALID_TI92_FILE  318 // Invalid TI92 file
#define ERR_NOT_READY          320 // Calculator is not ready
#define ERR_BACKUP             321 // Backup, not a var
#define ERR_INVALID_BYTE       322 // Invalid byte received
#define ERR_INVALID_TI92p_FILE 323 // Invalid TI92+ file
#define ERR_INVALID_TI89_FILE  324 // Invalid TI89 file
#define ERR_VOID_FUNCTION      325 // This function does not exist for the calc
#define ERR_INVALID_TI82_FILE  326 // Invalid TI82 file
#define ERR_PACKET             327 // Packet error
#define ERR_SIZE               328 // Size error
#define ERR_INVALID_TI83_FILE  329 // Invalid TI83 file
#define ERR_INVALID_TI85_FILE  330 // Invalid TI85 file
#define ERR_INVALID_TI86_FILE  331 // Invalid TI86 file
#define ERR_INVALID_TI83p_FILE 332 // Invalid TI83+ file

#define ERR_INVALID_FLASH_FILE 256 // FLASH file is invalid
#define ERR_VAR_REFUSED        257 // Var has been refused by calc
#define ERR_OUT_OF_MEMORY      258 // Calc reply out of memory
#define ERR_INVALID_TIXX_FILE  259 // Invalid TI file
#define ERR_GRP_SIZE_EXCEEDED  260 // Group size exceeds 64KB
#define ERR_OPEN_FILE          261 // Can not open file
#define ERR_CLOSE_FILE         262 // Can not close file
#define ERR_NO_IDLIST          263 // Calc has not an IDlist

#define ERR_PENDING_TRANSFER   333 // A transfer is in progress

#endif


