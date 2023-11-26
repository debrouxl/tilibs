/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - charset library, a part of the TiLP project
 *  Copyright (C) 2006-2006 Romain Lievin and Kevin Kofler
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

/*
	This unit contains charset conversion routines between TI and UTF-16.
	See: www.unicode.org/charts & www.czyborra.com/utf
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"
#include "charset.h"

///////////// Common functions /////////////
static unsigned short* ticonv_nonusb_to_utf16(const unsigned long * charset, const char *ti, unsigned short *utf16)
{
	const unsigned char *p = (const unsigned char *)ti;
	unsigned short *q = utf16;

	if (ti == nullptr || utf16 == nullptr)
	{
		return nullptr;
	}

	while (*p)
	{
		const unsigned long c = charset[*(p++)];
		if (c < 0x10000)
		{
			*(q++) = (unsigned short)c;
		}
		else
		{
			*(q++) = (unsigned short)(c >> 16);
			*(q++) = (unsigned short)(c & 0xffff);
		}
	}
	*q = 0;

	return utf16;
}

static unsigned short * ticonv_usb_to_utf16(const char *ti, unsigned short *utf16)
{
	if (ti == nullptr || utf16 == nullptr)
	{
		return nullptr;
	}

	unsigned short* tmp = ticonv_utf8_to_utf16(ti);
	if (tmp == nullptr)
	{
		return nullptr;
	}

	memcpy(utf16, tmp, 2 * ticonv_utf16_strlen(tmp));
	ticonv_utf16_free(tmp);

	return utf16;
}

static char * ticonv_utf16_to_nonusb(const unsigned long * charset, const unsigned short *utf16, char *ti)
{
	const unsigned short *p = utf16;
	unsigned char *q = (unsigned char *)ti;

	if (utf16 == nullptr || ti == nullptr)
	{
		return nullptr;
	}

	while (*p)
	{
		unsigned long c = *(p++);

		if ((c & 0xfc00) == 0xd800)
		{
			c = (c << 16) | *(p++);
		}

		if (c < 256 && charset[c] == c)
		{
			unsigned int i;

			if (c == 'x' && *p == 0x0305) // x followed by combining overline.
			{
				for (i = 0; i < 256; i++) {
					if (charset[i] == 0x00780305) {
						c = i; // xbar has that character code.
						break;
					}
				}
				p++;
			}
			else if (c == 'y' && *p == 0x0305) // y followed by combining overline.
			{
				for (i = 0; i < 256; i++) {
					if (charset[i] == 0x00790305) {
						c = i; // ybar has that character code.
						break;
					}
				}
				p++;
			}

			*(q++) = (unsigned char)c; // c < 256 if we come here...
		}
		else
		{
			*q = '?';
			for (unsigned int i = 0; i < 256; i++) {
				if (charset[i] == c) {
					*q = (unsigned char)i;
					break;
				}
			}
			q++;
		}
	}
	*q = 0;

	return ti;
}

static char* ticonv_utf16_to_usb(const unsigned short *utf16, char *ti)
{
	if (utf16 == nullptr || ti == nullptr)
	{
		return nullptr;
	}

	char* tmp = ticonv_utf16_to_utf8(utf16);
	if (tmp == nullptr)
	{
		return nullptr;
	}

	strcpy(ti, tmp);
	ticonv_utf8_free(tmp);

	return ti;
}

///////////// TI89,92,92+,V200,Titanium /////////////

extern const unsigned long TICALL ti9x_charset[256] =
{
// control chars
 0, // 0x2592 is prettier
 1, // 0x2401 is prettier
 2, // 0x2402 is prettier
 3, // 0x2403 is prettier
 4, // 0x2404 is prettier
 5, // 0x2405 is prettier
 6, // 0x2406 is prettier
 7, // 0x2407, 0xd83ddd14 are prettier
 8, // 0x232b is prettier
 9, // 0x21e5 is prettier
 10, // 0x21b4 is prettier
 0x2934,
 12, // 0x219f (upwards version of 0x21a1 form feed) is prettier
 13, // 0x21b5 is prettier
 0x2693, // For now, using anchor symbol U+2693 to represent locked, but should switch to 0xd83ddd12 (U+1F512) when Unicode 6.0+ is more widely implemented.
 0x2713,
 0x25fe,
 0x25c2,
 0x25b8,
 0x25b4,
 0x25be,
 0x2190,
 0x2192,
 0x2191,
 0x2193,
 0x25c0,
 0x25b6,
 0x2b06,
 0x222a,
 0x2229,
 0x2282,
 0x2208,
// ASCII
 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,

 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,

 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 0x25c6,

// Greek letters
 0x3b1, 0x3b2, 0x393, 0x3b3, 0x394, 0x3b4, 0x3b5, 0x3b6, 0x3b8, 0x3bb, 0x3be, 0x3a0, 0x3c0, 0x3c1, 0x3a3, 0x3c3,
 0x3c4,
 0x3c6,
 0x3c8,
 0x3a9,
 0x3c9,
// Math symbols
 0xd875dda4, //E (non-BMP character)
 0x212f, //e
 0xd875dc8a, //i (non-BMP character)
 0x2b3, //r
 0x22ba, //T - questionable.
 0x00780305, //x bar (requires composition)
 0x00790305, //y bar (requires composition)
 0x2264,
 0x2260,
 0x2265,
 0x2220,
// Latin1
 0x2026,
 161,
 162,
 163,
 164,
 165,
 166,
 167,
 0x221a,
 169,
 170, //^g in AMS 3.10, but there is no such character in Unicode
 171,
 172,
 0x2212,
 174,
 175,
 176,
 177,
 178,
 179,
 180, //^-1, but there is no such character in Unicode 0xB9 is superscript 1
 181,
 182,
 183,
 0x207a,
 185,
 186,
 187,
 0x2202,
 0x222b,
 0x221e,
 191,

 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,

 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

/**
 * ticonv_ti9x_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI89,92,92+,V200,Titanium charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
unsigned short* TICALL ticonv_ti9x_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti9x_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti9x:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI89,92,92+,V200,Titanium charset conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
char* TICALL ticonv_utf16_to_ti9x(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti9x_charset, utf16, ti);
}

///////////// TI73 /////////////

extern const unsigned long TICALL ti73_charset[256] = {
	'\0',   0x3b7,  'u',    'v',    'w',    0x25b6, 0x2191, 0x2193, // [0x01-0x05] != TI83
	0x222b, 'x',    176,    184,    183,    0x22ba, 179,    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080, 0x2081, 0x2082, 0x2083, 0x2084, 0x2085, 0x2086, 0x2087,
	0x2088, 0x2089, 192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  '[',    0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  'x',    'y',    '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   '_',
	'_',    169,    'L',    0x212f, 'L',   'N',     '_' /*))*/,0x2192,

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti73_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI73 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
unsigned short* TICALL ticonv_ti73_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti73_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti73:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI73 charset conversion.
 *
 * Return value: NULL (not implemented)
 **/
char* TICALL ticonv_utf16_to_ti73(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti73_charset, utf16, ti);
}

///////////// TI80 /////////////

extern const unsigned long TICALL ti80_charset[256] = {
	' ',    0x2588, '_',    0x2191, 'A',    0x25b6, '%',    '(', // 0x00-0x7F
	')',    '\"',   ',',    '!',    176,    '\'',   0x2b3,  180,

	178,    'a',    'b',    'c',    'd',    'e',    'n',    'r', 
	0x2423, 'x',    'y',    0x2081, 0x2080, 0x3c0,  0xffff, '=',

	'X',    'Y',    'T',    'R',    0x3b8,  0x2025, 0x25a1, 0x207a,
	0x2d9,  '{',    '}',    179,    '.',    0x1d07, 0x2044, ':',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    '=',    0x2260, '>',    0x2265, '<',    0x2264,

	'?',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '+',    '-',    0x00d7, '/',

	'^',    0x207b, 0x221a, 0x3a3,  0x3c3,  0x1e8b, 0x1e8f, 0x394,
	0x1d1b, 0x2191, 0x2193, 0x2e3,  247,    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    0x2592, 0x2af0, 179,
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',

	' ',    0x2588, '_',    0x2191, 'A',    0x25b6, '%',    '(', // 0x80-0xFF onwards: same as 0x00-0x7F
	')',    '\"',   ',',    '!',    176,    '\'',   0x2b3,  180,

	178,    'a',    'b',    'c',    'd',    'e',    'n',    'r', 
	0x2423, 'x',    'y',    0x2081, 0x2080, 0x3c0,  0xffff, '=',

	'X',    'Y',    'T',    'R',    0x3b8,  0x2025, 0x25a1, 0x207a,
	0x2d9,  '{',    '}',    179,    '.',    0x1d07, 0x2044, ':',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    '=',    0x2260, '>',    0x2265, '<',    0x2264,

	'?',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '+',    '-',    0x00d7, '/',

	'^',    0x207b, 0x221a, 0x3a3,  0x3c3,  0x1e8b, 0x1e8f, 0x394,
	0x1d1b, 0x2191, 0x2193, 0x2e3,  247,    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    0x2592, 0x2af0, 179,
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti80_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI80 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
unsigned short* TICALL ticonv_ti80_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti80_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti80:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI80 charset conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
char* TICALL ticonv_utf16_to_ti80(const unsigned short *utf16, char *ti)
{
// TODO investigate reimplementation based on ticonv_utf16_to_nonusb().
	const unsigned short *p = utf16;
	unsigned char *q = (unsigned char *)ti;

	if (utf16 == nullptr || ti == nullptr)
	{
		return nullptr;
	}

	while (*p) 
	{
		if (   (*p >= 48 && *p <= 57)
		    || (*p >= 65 && *p <= 90)
		   )
		{
			*(q++)=(unsigned char)*(p++);
		} 
		else
		{
			switch (*(p++)) 
			{
				case 0x03b8: *(q++) = 91; break;	// theta
				case 0x3c0: *(q++) = 29; *(q++) = 30; break;	// pi
				case 0x03a3: *(q++) = 99; break;	// capital sigma
				case 0x03c3: *(q++) = 100; break;	// sigma
#pragma message("Warning: FINISH UTF-16-TO-TI-80 CONVERSION")

				case 0x2080: *(q++) = 28; break;	// subscript 10
				case 0x2081: *(q++) = 27; break;

				default:
					if (p[-1] >= 0xd800 && p[-1] <= 0xdbff)
					{
						p++;
					}
					*(q++) = 64;
				break;
			}
		}
	}
	*q=0;

	return ti;
}

///////////// TI82 /////////////

extern const unsigned long TICALL ti82_charset[256] = {
	'\0',    'b',    'o',    'd',    'h',   0x25b6, 0x2191, 0x2193, 
	0x222b, 'x',    176,    184,    183,    0x22ba, 179,    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  '[',    0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  'x',    'y',    '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0x3b7,
	0x25b6, '_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
}; 

/**
 * ticonv_ti82_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI82 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
unsigned short* TICALL ticonv_ti82_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti82_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti82:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI82 charset conversion.
 *
 * Return value: NULL (not implemented)
 **/
char* TICALL ticonv_utf16_to_ti82(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti82_charset, utf16, ti);
}

///////////// TI83 /////////////

extern const unsigned long TICALL ti83_charset[256] = {
	'\0',   0x3b7,  'u',    'v',    'w',    0x25b6, 0x2191, 0x2193, // [0x01-0x05] != TI83
	0x222b, 'x',    176,    184,    183,    0x22ba, 179,    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  '[',    0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  'x',    'y',    '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0xd875dc8a,
	'?',    0x3c7,  'F',    0x212f, 'L',   'N',     '_' /*))*/,0x2192,

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti83_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI82 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer.
 **/
unsigned short* TICALL ticonv_ti83_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti83_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti83:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI83 charset conversion.
 *
 * Return value: NULL (not implemented)
 **/
char* TICALL ticonv_utf16_to_ti83(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti83_charset, utf16, ti);
}

///////////// TI83+/84+ /////////////

extern const unsigned long TICALL ti83p_charset[256] = {
	'\0',   0x3b7,  'u',    'v',    'w',    0x25b6, 0x2191, 0x2193, // [0x01-0x05] != TI83
	0x222b, 215,    176,    184,    183,    0x22ba, 179,    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    0x3b8,  '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+3,  232+1,  232+0,
	232+2,  232+3,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    241,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  '[',    0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  0x00780305, 0x00790305,    0xa4,   0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0xd875dc8a,
	'?',    0x3c7,  'F',    0x212f, 'L',   'N',     '_',    0x2192,

	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',    '_',    '$',    '_',    223,    '_',    '_',    '_',
	'_',    '_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti83p_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI83+ charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
unsigned short* TICALL ticonv_ti83p_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti83p_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti83p:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI83+ charset conversion.
 *
 * Return value: returns the destination pointer or NULL if error.
 **/
char* TICALL ticonv_utf16_to_ti83p(const unsigned short *utf16, char *ti)
{
// TODO investigate reimplementation based on ticonv_utf16_to_nonusb().
	const unsigned short *p = utf16;
	unsigned char *q = (unsigned char *)ti;

	if (utf16 == nullptr || ti == nullptr)
	{
		return nullptr;
	}

	while (*p)
	{
		if (   (*p >= 32 && *p <= 35)	// 36 out
		    || (*p >= 37 && *p <= 90)	// 91 out
		    || (*p >= 92 && *p <= 126)
		   ) 
		{
			*(q++)=(unsigned char)*(p++);
		} 
		else
		{
			switch (*(p++)) 
			{
				case 0x03b7: *(q++) = 1; break;		// eta
				case 0x2074: *(q++) = 36; break;
				case 0x03b8: *(q++) = 91; break;	// theta

				case 0x2080: *(q++) = 128; break;	// 0 to 9 in underscript
				case 0x2081: *(q++) = 129; break;
				case 0x2082: *(q++) = 130; break;
				case 0x2083: *(q++) = 131; break;
				case 0x2084: *(q++) = 132; break;
				case 0x2085: *(q++) = 133; break;
				case 0x2086: *(q++) = 134; break;
				case 0x2087: *(q++) = 135; break;
				case 0x2088: *(q++) = 136; break;
				case 0x2089: *(q++) = 137; break;

				case 192+1: *(q++) = 138; break;	// i18n characters
				case 192+0: *(q++) = 139; break;
				case 192+2: *(q++) = 140; break;
				case 192+4: *(q++) = 141; break;
				case 224+1: *(q++) = 142; break;
				case 224+0: *(q++) = 143; break;
				case 224+2: *(q++) = 144; break;
				case 224+4: *(q++) = 145; break;
				case 200+1: *(q++) = 146; break;
				case 200+0: *(q++) = 147; break;
				case 200+2: *(q++) = 148; break;
				case 200+3: *(q++) = 149; break;
				case 232+1: *(q++) = 150; break;
				case 232+0: *(q++) = 151; break;
				case 232+2: *(q++) = 152; break;
				case 232+3: *(q++) = 153; break;
				case 204+1: *(q++) = 154; break;
				case 204+0: *(q++) = 155; break;
				case 204+2: *(q++) = 156; break;
				case 204+3: *(q++) = 157; break;
				case 236+1: *(q++) = 158; break;
				case 236+0: *(q++) = 159; break;
				case 236+2: *(q++) = 160; break;
				case 236+3: *(q++) = 161; break;
				case 210+1: *(q++) = 162; break;
				case 210+0: *(q++) = 163; break;
				case 210+2: *(q++) = 164; break;
				case 210+4: *(q++) = 165; break;
				case 242+1: *(q++) = 166; break;
				case 242+0: *(q++) = 167; break;
				case 242+2: *(q++) = 168; break;
				case 242+4: *(q++) = 169; break;
				case 217+1: *(q++) = 170; break;
				case 217+0: *(q++) = 171; break;
				case 217+2: *(q++) = 172; break;
				case 217+3: *(q++) = 173; break;
				case 249+1: *(q++) = 174; break;
				case 249+0: *(q++) = 175; break;
				case 249+2: *(q++) = 176; break;
				case 249+3: *(q++) = 177; break;
				case 199 :	*(q++) = 178; break;
				case 231:	*(q++) = 179; break;
				case 209:	*(q++) = 180; break;
				case 241:	*(q++) = 181; break;

				case 0x3b1: *(q++) = 187; break;	// greek characters
				case 0x3b2: *(q++) = 188; break;
				case 0x3b3: *(q++) = 189; break;
				case 0x394: *(q++) = 190; break;
				case 0x3b4: *(q++) = 191; break;
				case 0x3b5: *(q++) = 192; break;
				case '[':	*(q++) = 193; break;
				case 0x3bb: *(q++) = 194; break;
				case 0x3bc: *(q++) = 195; break;
				case 0x3c0: *(q++) = 196; break;
				case 0x3c1: *(q++) = 197; break;
				case 0x3a3: *(q++) = 198; break;
				case 0x3c3: *(q++) = 199; break;
				case 0x3c4: *(q++) = 200; break;
				case 0x3d5: *(q++) = 201; break;
				case 0x3a9: *(q++) = 202; break;

				default:
					if (p[-1] >= 0xd800 && p[-1] <= 0xdbff)
					  p++;
					*(q++)='?';
				break;
			}
		}
	}
	*q=0;

	return ti;
}

///////////// TI85 /////////////

// I'm not sure this is the right TI85 charset, my doc is incomplete
extern const unsigned long TICALL ti85_charset[256] = {
	'\0',   'b',   'o',    'd',    'h',   0x25b6, 0x2191, 0x2193, 
	0x222b, 'x',   'A',     'B',    'C',   'D',    'E',    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  0x3b8,  0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  'x',    'y',    '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0x26b6,
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti85_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI85 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer.
 **/
unsigned short* TICALL ticonv_ti85_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti85_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti85:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI85 charset conversion.
 *
 * Return value: NULL (not implemented)
 **/
char* TICALL ticonv_utf16_to_ti85(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti85_charset, utf16, ti);
}

///////////// TI86 /////////////

extern const unsigned long TICALL ti86_charset[256] = {
	'\0',    'b',  'o',     'd',    'h',   0x25b6, 0x2191, 0x2193, 
	0x222b, 'x',   'A',     'B',    'C',   'D',    'E',    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264, 
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7', 
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_', // TI82 != TI85: theta <-> [

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  0x3b8,  0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  0x00780305, 0x00790305,   '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0x26b6,
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
};

/**
 * ticonv_ti86_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI86 charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer.
 **/
unsigned short* TICALL ticonv_ti86_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_nonusb_to_utf16(ti86_charset, ti, utf16);
}

/**
 * ticonv_utf16_to_ti86:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI86 charset conversion.
 *
 * Return value: NULL (not implemented)
 **/
char* TICALL ticonv_utf16_to_ti86(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_nonusb(ti86_charset, utf16, ti);
}

///////////// TI84+ USB & Titanium USB /////////////

/**
 * ticonv_ti84pusb_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * TI84+/USB charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer.
 **/
unsigned short* TICALL ticonv_ti84pusb_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_usb_to_utf16(ti, utf16);
}

/**
 * ticonv_utf16_to_ti84pusb:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to TI84+/USB charset conversion.
 *
 * Return value: returns the destination pointer.
 **/
char* TICALL ticonv_utf16_to_ti84pusb(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_usb(utf16, ti);
}

/**
 * ticonv_ti89tpusb_to_utf16:
 * @ti: null terminated string (input)
 * @utf16: null terminated string (output)
 *
 * Titanium/USB charset to UTF-16 conversion.
 *
 * Return value: returns the destination pointer.
 **/
unsigned short* TICALL ticonv_ti89tusb_to_utf16(const char *ti, unsigned short *utf16)
{
	return ticonv_usb_to_utf16(ti, utf16);
}

/**
 * ticonv_utf16_to_ti89tpusb:
 * @utf16: null terminated string (input)
 * @ti: null terminated string (output)
 *
 * UTF-16 to Titanium/USB charset conversion.
 *
 * Return value: returns the destination pointer.
 **/
char* TICALL ticonv_utf16_to_ti89tusb(const unsigned short *utf16, char *ti)
{
	return ticonv_utf16_to_usb(utf16, ti);
}

////////////////////////////////
