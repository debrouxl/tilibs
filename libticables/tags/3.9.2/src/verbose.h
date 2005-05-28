/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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

#ifndef __CABLES_VERBOSE_H__
#define __CABLES_VERBOSE_H__

#include <stdio.h>
#include "export.h"
#include "cabl_def.h"

// Store in a file what is displayed in the console
#if defined(__LINUX__) || defined(__BSD__)
//# define DISP_FILE "/tmp/verbose.log"
# define DISP_FILE "/dev/null"
#else
# define DISP_FILE "C:\\verbose.log"
#endif

TIEXPORT int TICALL DISPLAY(const char *format, ...);
TIEXPORT int TICALL DISPLAY_ERROR(const char *format, ...);
TIEXPORT int TICALL ticable_DISPLAY_settings(TicableDisplay op);
TIEXPORT int TICALL ticable_verbose_set_file(const char *filename);
TIEXPORT int TICALL ticable_verbose_flush_file(void);

/* Deprecated */
TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_stream(FILE * stream);
TIEXPORT FILE *TICALL ticable_DISPLAY_set_output_to_file(char *filename);
TIEXPORT int TICALL ticable_DISPLAY_close_file();

#endif
