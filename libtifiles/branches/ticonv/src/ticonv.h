/* Hey EMACS -*- linux-c -*- */
/* $Id: ticonv.h 1825 2006-02-09 20:43:07Z roms $ */

/*  libTICONV - Ti File Format library, a part of the TiLP project
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

#ifndef __TICONV_H__
#define __TICONV_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "export4.h"

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCONV_VERSION "0.0.1"
#else
# define LIBCONV_VERSION VERSION
#endif

/* Functions */

// namespace scheme: library_class_function like ticonv_fext_get

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  
  TIEXPORT int TICALL ticonv_library_init(void);
  TIEXPORT int TICALL ticonv_library_exit(void);

  /*********************/
  /* General functions */
  /*********************/

  // ticonv.c
  TIEXPORT const char* TICALL ticonv_version_get (void);

  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
