/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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

#ifndef __TICABLES_DATALOG__
#define __TICABLES_DATALOG__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdints.h"

// Constants

#define LOG_IN	0
#define LOG_OUT	1

// Functions

int log_start(void);
int log_1(int dir, uint8_t data);
int log_N(int dir, uint8_t *data, int len);
int log_stop(void);

// Wrappers

#ifdef ENABLE_LOGGING
# define START_LOGGING();		log_start();
# define LOG_1_DATA(w,d);		log_1(w,d);
# define LOG_N_DATA(w,d,n);		log_N(w,d,n);
# define STOP_LOGGING();		log_stop();
#else
# define START_LOGGING();
# define LOG_1_DATA(d);
# define LOG_N_DATA(d,n);
# define STOP_LOGGING();
#endif

#endif
