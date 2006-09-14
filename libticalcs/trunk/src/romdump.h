/* Hey EMACS -*- linux-c -*- */
/* $Id: logging.h 1015 2005-05-02 14:00:00Z roms $ */

/*  libticables - Ti File Format library, a part of the TiLP project
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

#ifndef __ROMDUMP_H__
#define __ROMDUMP_H__

int rd_dump(CalcHandle* h, const char *filename);
int rd_is_ready(CalcHandle* h);
int rd_send(CalcHandle *h, const char *prgname, unsigned int size, unsigned char *data);

#endif
