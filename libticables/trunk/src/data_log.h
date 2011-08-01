/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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

#ifndef __TICABLES_DATALOG__
#define __TICABLES_DATALOG__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ticables.h"

// Constants

#define LOG_IN	0
#define LOG_OUT	1

#define LOG_DIR		".ticables"

// Functions

int log_start(CableHandle *h);
int log_1(CableHandle *h, int dir, uint8_t data);
int log_N(CableHandle *h, int dir, uint8_t *data, int len);
int log_stop(CableHandle *h);

// Wrappers

#ifdef ENABLE_LOGGING
# define START_LOGGING(h);		log_start(h);
# define LOG_1_DATA(h,w,d);		log_1(h,w,d);
# define LOG_N_DATA(h,w,d,n);	log_N(h,w,d,n);
# define STOP_LOGGING(h);		log_stop(h);
#else
# define START_LOGGING(h);
# define LOG_1_DATA(h,w,d);
# define LOG_N_DATA(h,w,d,n);
# define STOP_LOGGING(h);
#endif

#endif
