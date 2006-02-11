/* Hey EMACS -*- linux-c -*- */
/* $Id: ticonv.c 1721 2006-01-21 13:58:14Z roms $ */

/*  libticonv - Ti File Format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  This unit contains the interface of the libticonv library.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "ticonv.h"

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticonv_instance = 0;	// counts # of instances

/**
 * ticonv_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the handle count.
 **/
TIEXPORT int TICALL ticonv_library_init()
{
	if (ticonv_instance)
		return (++ticonv_instance);
	printf("ticonv library version %s", LIBCONV_VERSION);

  	return (++ticonv_instance);
}

/**
 * ticonv_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the handle count.
 **/
TIEXPORT int TICALL ticonv_library_exit()
{
  	return (--ticonv_instance);
}

/***********/
/* Methods */
/***********/

/**
 * ticonv_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT const char *TICALL ticonv_version_get(void)
{
	return LIBCONV_VERSION;
}

/**
 * ticonv_utf8_to_utf16:
 * @src: null terminated UTF-8 string
 *
 * UTF-8 to UTF-16 conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT unsigned short* ticonv_utf8_to_utf16(const char* str)
{
	gunichar2*  dst;
	const gchar* src = str;
	
	dst = g_utf8_to_utf16(str, -1, NULL, NULL, NULL);

	return dst;
}

/**
 * ticonv_utf16_to_utf8:
 * @src: null terminated UTF-16 string
 *
 * UTF-16 to UTF-8 conversion.
 *
 * Return value: a newly allocated string, NULL otherwise (error).
 **/
TIEXPORT const char*	   ticonv_utf16_to_utf8(const unsigned short* str)
{
	const gunichar2*  src = str;
	gchar* dst;

	dst = g_utf16_to_utf8(src, -1, NULL, NULL, NULL);

	return dst;
}

///////////// TI89,92,92+,V200,Titanium
static unsigned long ti9x_utf16pair_code[256]=
{
// control chars
 0,
 1,
 2,
 3,
 4,
 5,
 6,
 7,
 8,
 9,
 10,
 0x2934,
 12,
 13,
 0x2693, // "locked" symbol (doesn't exist in Unicode) <-> anchor
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
 32,
 33,
 34,
 35,
 36,
 37,
 38,
 39,
 40,
 41,
 42,
 43,
 44,
 45,
 46,
 47,
 48,
 49,
 50,
 51,
 52,
 53,
 44,
 55,
 56,
 57,
 58,
 59,
 60,
 61,
 62,
 63,
 64,
 65,
 66,
 67,
 68,
 69,
 70,
 71,
 72,
 73,
 74,
 75,
 76,
 77,
 78,
 79,
 80,
 81,
 82,
 83,
 84,
 85,
 86,
 87,
 88,
 89,
 90,
 91,
 92,
 93,
 94,
 95,
 96,
 97,
 98,
 99,
 100,
 101,
 102,
 103,
 104,
 105,
 106,
 107,
 108,
 109,
 110,
 111,
 112,
 113,
 114,
 115,
 116,
 117,
 118,
 119,
 120,
 121,
 122,
 123,
 124,
 125,
 126,
 0x25c6,
// Greek letters
 0x3b1,
 0x3b2,
 0x393,
 0x3b3,
 0x394,
 0x3b4,
 0x3b5,
 0x3b6,
 0x3b8,
 0x3bb,
 0x3be,
 0x3a0,
 0x3c0,
 0x3c1,
 0x3a3,
 0x3c3,
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
 0x22ba, //T
 0x03050078, //x bar (requires composing)
 0x03050079, //y bar (requires composing)
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
 180, //^-1, but there is no such character in Unicode
 181,
 182,
 183,
 184,
 185,
 186,
 0x207a,
 0x2202,
 0x222b,
 0x221e,
 191,
 192,
 193,
 194,
 195,
 196,
 197,
 198,
 199,
 200,
 201,
 202,
 203,
 204,
 205,
 206,
 207,
 208,
 209,
 210,
 211,
 212,
 213,
 214,
 215,
 216,
 217,
 218,
 219,
 220,
 221,
 222,
 223,
 224,
 225,
 226,
 227,
 228,
 229,
 230,
 231,
 232,
 233,
 234,
 235,
 236,
 237,
 238,
 239,
 240,
 241,
 242,
 243,
 244,
 245,
 246,
 247,
 248,
 249,
 250,
 251,
 252,
 253,
 254,
 255 
};

TIEXPORT char*		   TICALL ticonv_utf16_to_ti9x(const unsigned short *utf16, char *ti)
{
	const unsigned short *p = utf16;
	unsigned char *q = (unsigned char *)ti;

  while (*p) {
    if (*p<=10
        || *p==12 || *p==13
        || (*p>=32 && *p<=126)
        || (*p>=161 && *p<=167)
        || (*p>=169 && *p<=172)
        || (*p>=174 && *p<=186)
        || (*p>=191 && *p<=255)) {
      *(q++)=(unsigned char)*(p++);
    } else switch (*(p++)) {
      case 0x2934:
        *(q++)=11;
        break;
      case 0x2693:
        *(q++)=14;
        break;
      case 0x2713:
        *(q++)=15;
        break;
      case 0x25fe:
        *(q++)=16;
        break;
      case 0x25c2:
        *(q++)=17;
        break;
      case 0x25b8:
        *(q++)=18;
        break;
      case 0x25b4:
        *(q++)=19;
        break;
      case 0x25be:
        *(q++)=20;
        break;
      case 0x2190:
        *(q++)=21;
        break;
      case 0x2192:
        *(q++)=22;
        break;
      case 0x2191:
        *(q++)=23;
        break;
      case 0x2193:
        *(q++)=24;
        break;
      case 0x25c0:
        *(q++)=25;
        break;
      case 0x25b6:
        *(q++)=26;
        break;
      case 0x2b06:
        *(q++)=27;
        break;
      case 0x222a:
        *(q++)=28;
        break;
      case 0x2229:
        *(q++)=29;
        break;
      case 0x2282:
        *(q++)=30;
        break;
      case 0x2208:
        *(q++)=31;
        break;
      case 0x25c6:
        *(q++)=127;
        break;
      case 0x3b1:
        *(q++)=128;
        break;
      case 0x3b2:
        *(q++)=129;
        break;
      case 0x393:
        *(q++)=130;
        break;
      case 0x3b3:
        *(q++)=131;
        break;
      case 0x394:
        *(q++)=132;
        break;
      case 0x3b4:
        *(q++)=133;
        break;
      case 0x3b5:
        *(q++)=134;
        break;
      case 0x3b6:
        *(q++)=135;
        break;
      case 0x3b8:
        *(q++)=136;
        break;
      case 0x3bb:
        *(q++)=137;
        break;
      case 0x3be:
        *(q++)=138;
        break;
      case 0x3a0:
        *(q++)=139;
        break;
      case 0x3c0:
        *(q++)=140;
        break;
      case 0x3c1:
        *(q++)=141;
        break;
      case 0x3a3:
        *(q++)=142;
        break;
      case 0x3c3:
        *(q++)=143;
        break;
      case 0x3c4:
        *(q++)=144;
        break;
      case 0x3c6:
        *(q++)=145;
        break;
      case 0x3c8:
        *(q++)=146;
        break;
      case 0x3a9:
        *(q++)=147;
        break;
      case 0x3c9:
        *(q++)=148;
        break;
      case 0x212f:
        *(q++)=150;
        break;
      case 0x2b3:
        *(q++)=152;
        break;
      case 0x22ba:
        *(q++)=153;
        break;
      case 0x2264:
        *(q++)=156;
        break;
      case 0x2260:
        *(q++)=157;
        break;
      case 0x2265:
        *(q++)=158;
        break;
      case 0x2220:
        *(q++)=159;
        break;
      case 0x2026:
        *(q++)=160;
        break;
      case 0x221a:
        *(q++)=168;
        break;
      case 0x2212:
        *(q++)=173;
        break;
      case 0x207a:
        *(q++)=187;
        break;
      case 0x2202:
        *(q++)=188;
        break;
      case 0x222b:
        *(q++)=189;
        break;
      case 0x221e:
        *(q++)=190;
        break;
      case 0x305:
        if (*p==0x78) {
          *(q++)=154;
          p++;
        } else if (*p==0x79) {
          *(q++)=155;
          p++;
        } else {
          *(q++)='?';
        }
        break;
      case 0xd875:
        if (*p==0xdda4) {
          *(q++)=149;
          p++;
          break;
        } else if (*p==0xdc8a) {
          *(q++)=151;
          p++;
          break;
        }
      default:
        if (p[-1] >= 0xd800 && p[-1] <= 0xdbff)
          p++;
        *(q++)='?';
        break;
    }
  }
  *q=0;

	return ti;
}

TIEXPORT unsigned short* TICALL ticonv_ti9x_to_utf16(const char *ti, unsigned short *utf16)
{
	const unsigned char *p = (const unsigned char *)ti;
	unsigned short *q = utf16;
	unsigned long c;

	while (*p) 
	{
		c=ti9x_utf16pair_code[*(p++)];
		if (c<0x10000) 
		{
			*(q++)=(unsigned short)c;
		} 
		else 
		{
			*(q++)=(unsigned short)(c>>16);
			*(q++)=(unsigned short)(c&0xffff);
		}
	}
	*q=0;

	return utf16;
}