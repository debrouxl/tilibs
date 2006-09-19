/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libTICONV - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2006  Romain Lievin and Kevin Kofler
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

#undef TICONV_ICONV_INTERFACE

// Need size_t declaration.
#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

// Need iconv_t declaration.
#include <iconv.h>

#include "export4.h"

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCONV_VERSION "0.0.6"
#else
# define LIBCONV_VERSION VERSION
#endif

/**
 * CalcModel:
 *
 * An enumeration which contains the following calculator types:
 **/
#if !defined(__TIFILES_H__)
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
	CALC_TI84P_USB, CALC_TI89T_USB,
} CalcModel;
#endif

/* Identifier for conversion method from one codeset to another. Exported if 
	ICONV_INTERFACE is enabled. */
#if defined(TICONV_ICONV_INTERFACE)
typedef struct
{
	CalcModel src_calc;
	iconv_t iconv_desc;
	CalcModel dest_calc;
	size_t lossy_count;
	size_t lookahead_result;
	int lookahead_errno;
	unsigned short lookahead;
} ticonv_iconv_t;
#endif

/* Functions */

// namespace scheme: library_class_function like ticonv_library_init
// functions suffixed with _s means 'static', no memory allocation

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  
  //TIEXPORT int TICALL ticonv_library_init(void);
  //TIEXPORT int TICALL ticonv_library_exit(void);

  /*********************/
  /* General functions */
  /*********************/

  // ticonv.c
  TIEXPORT const char* TICALL ticonv_version_get (void);

  TIEXPORT size_t TICALL ticonv_utf16_strlen(const unsigned short *str);

  TIEXPORT unsigned short* ticonv_utf8_to_utf16(const char *str);
  TIEXPORT char*	       ticonv_utf16_to_utf8(const unsigned short *str);

  TIEXPORT char*		   TICALL ticonv_charset_utf16_to_ti_s(CalcModel model, const unsigned short *utf16, char *ti);
  TIEXPORT unsigned short* TICALL ticonv_charset_ti_to_utf16_s(CalcModel model, const char *ti, unsigned short *utf16);

  TIEXPORT char*		   TICALL ticonv_charset_utf16_to_ti(CalcModel model, const unsigned short *utf16);
  TIEXPORT unsigned short* TICALL ticonv_charset_ti_to_utf16(CalcModel model, const char *ti);

  TIEXPORT unsigned short* TICALL ticonv_varname_to_utf16_s(CalcModel model, const char *src, unsigned short *dst);
  TIEXPORT unsigned short* TICALL ticonv_varname_to_utf16  (CalcModel model, const char *src);

  TIEXPORT char* TICALL ticonv_varname_to_utf8_s(CalcModel model, const char *src, char *dst);
  TIEXPORT char* TICALL ticonv_varname_to_utf8  (CalcModel model, const char *src           );

  TIEXPORT char* TICALL ticonv_varname_to_filename_s(CalcModel model, const char *src, char *dst);
  TIEXPORT char* TICALL ticonv_varname_to_filename  (CalcModel model, const char *src);

  TIEXPORT char* TICALL ticonv_varname_to_tifile_s(CalcModel model, const char *src, char *dst);
  TIEXPORT char* TICALL ticonv_varname_to_tifile  (CalcModel model, const char *src);
  TIEXPORT char* TICALL ticonv_varname_from_tifile_s(CalcModel model, const char *src, char *dst);
  TIEXPORT char* TICALL ticonv_varname_from_tifile  (CalcModel model, const char *src);

  // charset.c
#ifndef __cplusplus
  TIEXPORT const unsigned long TICALL ti73_charset[256];
  TIEXPORT const unsigned long TICALL ti82_charset[256];
  TIEXPORT const unsigned long TICALL ti83_charset[256];
  TIEXPORT const unsigned long TICALL ti83p_charset[256];
  TIEXPORT const unsigned long TICALL ti85_charset[256];
  TIEXPORT const unsigned long TICALL ti86_charset[256];
  TIEXPORT const unsigned long TICALL ti9x_charset[256];
#endif

  // tokens.c
  TIEXPORT char* TICALL ticonv_varname_detokenize(CalcModel model, const char *src);
  TIEXPORT char* TICALL ticonv_varname_tokenize  (CalcModel model, const char *src);

  // filename.c
  TIEXPORT char* TICALL ticonv_utf16_to_gfe(CalcModel model, const unsigned short *src);
  TIEXPORT char* TICALL ticonv_gfe_to_zfe(CalcModel model, const char *src);

  // iconv.c: exported if ICONV_INTERFACE has been enabled from configure.
#if defined(TICONV_ICONV_INTERFACE)
  /* Allocate descriptor for code conversion from codeset FROMCODE to
     codeset TOCODE.  */
  TIEXPORT ticonv_iconv_t TICALL ticonv_iconv_open (const char *tocode, const char *fromcode);
  /* Convert at most *INBYTESLEFT bytes from *INBUF according to the
     code conversion algorithm specified by CD and place up to
     *OUTBYTESLEFT bytes in buffer at *OUTBUF.  */
  TIEXPORT size_t TICALL ticonv_iconv (ticonv_iconv_t cd, char **__restrict inbuf,
                                       size_t *__restrict inbytesleft,
                                       char **__restrict outbuf,
                                       size_t *__restrict outbytesleft);
  /* Free resources allocated for descriptor CD for code conversion.  */
  TIEXPORT int TICALL ticonv_iconv_close (ticonv_iconv_t cd);
#endif

  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
