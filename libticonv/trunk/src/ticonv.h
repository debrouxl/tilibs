/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libTICONV - charset library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TICONV_H__
#define __TICONV_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

// Need size_t declaration.
#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

// Need iconv_t declaration.
#include <iconv.h>

#include "export4.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Work around a Windows SDK issue that prevents us from just using "restrict" directly in both C/C++.
#if defined(__cplusplus) || defined(_MSC_VER)
#define _ticonv_restrict_backup restrict
#undef restrict
#define restrict __restrict
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCONV_VERSION "1.1.6"
#else
# define LIBCONV_VERSION VERSION
#endif

/**
 * CalcModel:
 *
 * An enumeration which contains several calculator models.
 *
 **/
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86, // 7,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200, // 12
	CALC_TI84P_USB, CALC_TI89T_USB, CALC_NSPIRE, CALC_TI80, // 16
	CALC_TI84PC, CALC_TI84PC_USB, CALC_TI83PCE_USB, CALC_TI84PCE_USB, CALC_TI82A_USB, CALC_TI84PT_USB, // 22
	CALC_NSPIRE_CRADLE, // 23
	CALC_NSPIRE_CLICKPAD, CALC_NSPIRE_CLICKPAD_CAS, CALC_NSPIRE_TOUCHPAD, CALC_NSPIRE_TOUCHPAD_CAS, // 27
	CALC_NSPIRE_CX, CALC_NSPIRE_CX_CAS, CALC_NSPIRE_CMC, CALC_NSPIRE_CMC_CAS, // 31
	CALC_NSPIRE_CXII, CALC_NSPIRE_CXII_CAS, CALC_NSPIRE_CXIIT, CALC_NSPIRE_CXIIT_CAS, // 35
	CALC_MAX
} CalcModel;

/**
 * CalcProductIDs:
 *
 * An enumeration which contains the product IDs used by TI graphing calculators and companion equipment.
 **/
typedef enum
{
        PRODUCT_ID_NONE = 0x00,
        PRODUCT_ID_TI92P = 0x01,
        PRODUCT_ID_TI73 = 0x02,
        PRODUCT_ID_TI89 = 0x03,
        PRODUCT_ID_TI83P = 0x04,
        PRODUCT_ID_CBL2 = 0x05,
        PRODUCT_ID_LABPRO = 0x06,
        PRODUCT_ID_TIPRESENTER = 0x07,
        PRODUCT_ID_TIV200 = 0x08,
        PRODUCT_ID_TI89T = 0x09,
        PRODUCT_ID_TI84P = 0x0A,
        PRODUCT_ID_TI82A = 0x0B,
        PRODUCT_ID_NSPIRE_CAS = 0x0C, // The abandoned Nspire CAS+ prototypes also use 0x0C, but libti*/tilp do not handle their unique communication protocol.
        PRODUCT_ID_LABCRADLE = 0x0D,
        PRODUCT_ID_NSPIRE = 0x0E,
        PRODUCT_ID_NSPIRE_CX_CAS = 0x0F, // Yes, two completely different models use ID 0x0F.
        PRODUCT_ID_TI84PCSE = 0x0F,
        PRODUCT_ID_NSPIRE_CX = 0x10,
        PRODUCT_ID_NSPIRE_CMC_CAS = 0x11,
        PRODUCT_ID_NSPIRE_CMC = 0x12,
        PRODUCT_ID_TI83PCE = 0x13, // These two similar models use the same ID as well.
        PRODUCT_ID_TI84PCE = 0x13,
        // No known calculators or lab equipments use 0x14-0x1A.
        PRODUCT_ID_TI84PT = 0x1B,
        PRODUCT_ID_NSPIRE_CXII_CAS = 0x1C,
        PRODUCT_ID_NSPIRE_CXII = 0x1D,
        PRODUCT_ID_NSPIRE_CXIIT = 0x1E
} CalcProductIDs;

/**
 * ticonv_iconv_t:
 * @src_calc:
 * @iconv_desc:
 * @dest_calc:
 * @lossy_count:
 * @lookahead_result:
 * @lookahead_errno:
 * @lookahead:
 *
 * A structure which is an identifier for conversion method from one codeset to another. 
 *
 **/
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

/* Functions */

// namespace scheme: library_class_function like ticonv_library_init
// functions suffixed with _s means 'static', no memory allocation
// TODO functions suffixed with _sn means 'static with length limit', no memory allocation

	/****************/
	/* Entry points */
	/****************/

	//TIEXPORT4 int TICALL ticonv_library_init(void);
	//TIEXPORT4 int TICALL ticonv_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// ticonv.c
	TIEXPORT4 const char* TICALL ticonv_version_get (void);

	TIEXPORT4 size_t TICALL ticonv_utf16_strlen(const unsigned short *str);

	TIEXPORT4 unsigned short* TICALL ticonv_utf8_to_utf16(const char *str);
	TIEXPORT4 void TICALL ticonv_utf16_free(unsigned short *str);
	TIEXPORT4 char* TICALL ticonv_utf16_to_utf8(const unsigned short *str);
	TIEXPORT4 void TICALL ticonv_utf8_free(char *str);

	// TODO TIEXPORT4 char* TICALL ticonv_charset_utf16_to_ti_sn(CalcModel model, const unsigned short *utf16, char *ti, uint32_t maxlen);
	/* TODO TILIBS_DEPRECATED */ TIEXPORT4 char* TICALL ticonv_charset_utf16_to_ti_s(CalcModel model, const unsigned short *utf16, char *ti);
	TIEXPORT4 char* TICALL ticonv_charset_utf16_to_ti(CalcModel model, const unsigned short *utf16);
	TIEXPORT4 void  TICALL ticonv_ti_free(char * ti);

	// TODO TIEXPORT4 unsigned short* TICALL ticonv_charset_ti_to_utf16_sn(CalcModel model, const char *ti, unsigned short *utf16, uint32_t maxlen);
	/* TODO TILIBS_DEPRECATED */ TIEXPORT4 unsigned short* TICALL ticonv_charset_ti_to_utf16_s(CalcModel model, const char *ti, unsigned short *utf16);
	TIEXPORT4 unsigned short* TICALL ticonv_charset_ti_to_utf16(CalcModel model, const char *ti);
	// The free function corresponding to ticonv_charset_ti_to_utf16() is ticonv_utf16_free().

	TIEXPORT4 int TICALL ticonv_model_uses_utf8(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_is_tiz80(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_is_tiez80(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_is_ti68k(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_is_tinspire(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_legacy_ioport(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_usb_ioport(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_flash_memory(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_real_screen(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_monochrome_screen(CalcModel model);
	TIEXPORT4 int TICALL ticonv_model_has_color_screen(CalcModel model);
	TIEXPORT4 CalcProductIDs TICALL ticonv_model_to_product_id(CalcModel model);

	// TODO TIEXPORT4 unsigned short* TICALL ticonv_varname_to_utf16_sn(CalcModel model, const char *src, unsigned short *dst, uint32_t maxlen, unsigned char type);
	/* TODO TILIBS_DEPRECATED */ TIEXPORT4 unsigned short* TICALL ticonv_varname_to_utf16_s(CalcModel model, const char *src, unsigned short *dst, unsigned char type);
	TIEXPORT4 unsigned short* TICALL ticonv_varname_to_utf16  (CalcModel model, const char *src, unsigned char type);
	// The free function corresponding to ticonv_varname_to_utf16() is ticonv_utf16_free().

	TIEXPORT4 char* TICALL ticonv_varname_to_utf8_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TILIBS_DEPRECATED TIEXPORT4 char* TICALL ticonv_varname_to_utf8_s(CalcModel model, const char *src, char *dst, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_to_utf8  (CalcModel model, const char *src, unsigned char type);
	// The free function corresponding to ticonv_varname_to_utf8() is ticonv_utf8_free().

	TIEXPORT4 char* TICALL ticonv_varname_to_filename_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TILIBS_DEPRECATED TIEXPORT4 char* TICALL ticonv_varname_to_filename_s(CalcModel model, const char *src, char *dst, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_to_filename  (CalcModel model, const char *src, unsigned char type);
	// The free function corresponding to ticonv_varname_to_filename() is ticonv_gfe_free().

	TIEXPORT4 char* TICALL ticonv_varname_to_tifile_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TILIBS_DEPRECATED TIEXPORT4 char* TICALL ticonv_varname_to_tifile_s(CalcModel model, const char *src, char *dst, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_to_tifile  (CalcModel model, const char *src, unsigned char type);
	// The free function corresponding to ticonv_varname_to_tifile() is ticonv_varname_free().
	TIEXPORT4 char* TICALL ticonv_varname_from_tifile_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TILIBS_DEPRECATED TIEXPORT4 char* TICALL ticonv_varname_from_tifile_s(CalcModel model, const char *src, char *dst, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_from_tifile  (CalcModel model, const char *src, unsigned char type);
	// The free function corresponding to ticonv_varname_from_tifile() is ticonv_ti_free().

	// type2str.c
	TIEXPORT4 const char* TICALL ticonv_model_to_string(CalcModel type);
	TIEXPORT4 CalcModel   TICALL ticonv_string_to_model(const char *str);

	// charset.c
	extern TIEXPORT4 const unsigned long TICALL ti73_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti80_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti82_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti83_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti83p_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti85_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti86_charset[256];
	extern TIEXPORT4 const unsigned long TICALL ti9x_charset[256];

	// tokens.c
	// TODO TIEXPORT4 char* TICALL ticonv_varname_detokenize_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_detokenize(CalcModel model, const char *src, unsigned char type);
	// TODO TIEXPORT4 char* TICALL ticonv_varname_tokenize_sn(CalcModel model, const char *src, char *dst, uint32_t maxlen, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_tokenize  (CalcModel model, const char *src, unsigned char type);
	TIEXPORT4 char* TICALL ticonv_varname_strdup    (char *varname);
	TIEXPORT4 void  TICALL ticonv_varname_free      (char *varname);

	// filename.c
	TIEXPORT4 char* TICALL ticonv_utf16_to_gfe(CalcModel model, const unsigned short *src);
	TIEXPORT4 void  TICALL ticonv_gfe_free(char *src);
	TIEXPORT4 char* TICALL ticonv_gfe_to_zfe(CalcModel model, const char *src);
	TIEXPORT4 void  TICALL ticonv_zfe_free(char *src);
	TIEXPORT4 int   TICALL ticonv_environment_is_utf8(void);
	TIEXPORT4 int   TICALL ticonv_environment_has_utf8_filenames(void);

	// iconv.c
	/* Allocate descriptor for code conversion from codeset FROMCODE to codeset TOCODE.  */
	TIEXPORT4 ticonv_iconv_t TICALL ticonv_iconv_open (const char *tocode, const char *fromcode);
	/* Convert at most *INBYTESLEFT bytes from *INBUF according to the
	   code conversion algorithm specified by CD and place up to
	   *OUTBYTESLEFT bytes in buffer at *OUTBUF.  */
	TIEXPORT4 size_t TICALL ticonv_iconv (ticonv_iconv_t cd, char ** restrict inbuf,
	                                      size_t * restrict inbytesleft,
	                                      char ** restrict outbuf,
	                                      size_t * restrict outbytesleft);
	/* Free resources allocated for descriptor CD for code conversion.  */
	TIEXPORT4 int TICALL ticonv_iconv_close (ticonv_iconv_t cd);

	/************************/
	/* Deprecated functions */
	/************************/

#if defined(__cplusplus) || defined(_MSC_VER)
#undef restrict
#define restrict _ticonv_restrict_backup
#endif

#ifdef __cplusplus
}
#endif

#endif
