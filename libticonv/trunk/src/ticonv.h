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

/**
 * CalcModel:
 *
 * An enumeration which contains the following calculator types:
 **/
//typedef unsigned short	utf16c;
//typedef char			utf8c;

/**
 * ConvModel:
 *
 * An enumeration which contains the following calculator types:
 **/
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, /*CALC_TI82S, */ CALC_TI83, 
	CALC_TI83P, CALC_TI84P, 
	CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,

	CALC_TI84P_USB, CALC_TI89T_USB,
} ConvModel;

/* Functions */

// namespace scheme: library_class_function like ticonv_library_init

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

  TIEXPORT unsigned short* ticonv_utf8_to_utf16(const char* str);
  TIEXPORT const char*	   ticonv_utf16_to_utf8(const unsigned short* str);

  TIEXPORT char*		   TICALL ticonv_charset_utf16_to_ti(ConvModel model, const unsigned short *utf16, char *ti);
  TIEXPORT unsigned short* TICALL ticonv_charset_ti_to_utf16(ConvModel model, const char *ti, unsigned short *utf16);

  // charset.c
  TIEXPORT char*		   TICALL ticonv_utf16_to_ti9x(const unsigned short *utf16, char *ti);
  TIEXPORT unsigned short* TICALL ticonv_ti9x_to_utf16(const char *ti, unsigned short *utf16);

  // tokens.c
  // to write/import...

  // filenames.c
  // to write/import


  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
