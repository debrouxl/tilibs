/*  libticonv - charset library, a part of the TILP project
 *  Copyright (c) 2006 Kevin Kofler
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
 *  Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __CHARSET_H__
#define __CHARSET_H__

#include "export4.h"

TIEXPORT4 unsigned short* TICALL ticonv_ti73_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti82_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti83_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti83p_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti85_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti86_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti9x_to_utf16(const char *ti, unsigned short *utf16);

TIEXPORT4 char* TICALL ticonv_utf16_to_ti73(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti82(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti83(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti83p(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti85(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti86(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti9x(const unsigned short *utf16, char *ti);

TIEXPORT4 unsigned short* TICALL ticonv_ti84pusb_to_utf16(const char *ti, unsigned short *utf16);
TIEXPORT4 unsigned short* TICALL ticonv_ti89tusb_to_utf16(const char *ti, unsigned short *utf16);

TIEXPORT4 char* TICALL ticonv_utf16_to_ti84pusb(const unsigned short *utf16, char *ti);
TIEXPORT4 char* TICALL ticonv_utf16_to_ti89tusb(const unsigned short *utf16, char *ti);

#endif
