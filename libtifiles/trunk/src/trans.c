/*  libtifiles - TI File Format library
 *  Copyright (C) 2002-2003  Romain Lievin
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
  This unit is used to translate some varnames into a more readable name.
  Depends on the calculator type and the variable type.
*/

#include <stdio.h>
#include <string.h>
#include "stdints.h"

#include "typesxx.h"
#include "file_int.h"

// set to ISO8859-1 for compatibility with previous releases
static TifileEncoding encoding = ENCODING_LATIN1;

/*
  Set/Get encoding methods
*/

TIEXPORT void TICALL tifiles_translate_set_encoding(TifileEncoding enc)
{
  encoding = enc;
}

TIEXPORT TifileEncoding TICALL tifiles_translate_get_encoding(void)
{
  return encoding;
}


/*
  Convert string to pure ASCII.
  Note: src & dst will have the same length.
*/
TIEXPORT char* TICALL tifiles_convert_to_ascii(char* dst, const char *src)
{
  char *dest = dst;

  while(*src) {
    *dst++ = ((uint8_t)*src < 0x80) ? *src : '_';
    src++;
  }
  *dst = '\0';

  return dest;
}

/*
  Convert string to the ISO8859-1 charset (aka Latin1).
  Note: src & dst will have the same length.
*/
TIEXPORT char* TICALL tifiles_convert_to_latin1(char* dst, const char *src)
{
  char *dest = dst;

  while(*src) {
    *dst++ = (((uint8_t)*src >= 0x80) && ((uint8_t)*src < 0xA0)) ? '_' : *src;
    src++;
  }
  *dst = '\0';

  return dest;
}

/*
  Convert string to the UTF-8 charset (Unicode).
  See: www.unicode.org/charts & www.czyborra.com/utf
  Note: dst may be up to twice the length of src.
*/
TIEXPORT char* TICALL tifiles_convert_to_unicode(char* dst, const char *src)
{
  char *dest = dst;
  uint16_t wchar;

  while(*src) {
    if((uint8_t)*src < 0x80) // ASCII part
      wchar = *src;
    else if((uint8_t)*src >= 0xA0) // ISO8859-1 part
      wchar = *src & 0xff;
    else 
      { // greek characters
	switch((uint8_t)*src) {
	case 0x80: wchar = 0x03b1; break; // alpha
	case 0x81: wchar = 0x03b2; break; // beta
	case 0x82: wchar = 0x0393; break; // gamma (capital)
	case 0x83: wchar = 0x03b3; break; // gamma
	case 0x84: wchar = 0x0394; break; // delta (capital)
	case 0x85: wchar = 0x03b4; break; // delta
	case 0x86: wchar = 0x03b5; break; // epsilon
	case 0x87: wchar = 0x03b6; break; // dzeta
	case 0x88: wchar = 0x03b8; break; // theta
	case 0x89: wchar = 0x03bb; break; // lambda
	case 0x8a: wchar = 0x03be; break; // ksi
	case 0x8b: wchar = 0x03a0; break; // pi (capital)
	case 0x8c: wchar = 0x03c0; break; // pi
	case 0x8d: wchar = 0x03c1; break; // rho
	case 0x8e: wchar = 0x03a3; break; // sigma (capital)
	case 0x8f: wchar = 0x03c3; break; // sigma
	case 0x90: wchar = 0x03c4; break; // tau
	case 0x91: wchar = 0x03d5; break; // phi (capital)
	case 0x92: wchar = 0x03a8; break; // psi (capital)
	case 0x93: wchar = 0x03a9; break; // omega (capital)
	case 0x94: wchar = 0x03c9; break; // omega
	default: wchar = '_';
	}
      }

    // write our wide-char
    if ((uint16_t)wchar < 0x80)
      *dst++ = wchar;
    else if ((uint16_t)wchar < 0x0800) {
      *dst++ = (0xC0 | (wchar >> 6)) & 0xff;
      *dst++ = (0x80 | (wchar & 0x3f)) & 0xff;
    }
    src++;
  }
  *dst = '\0';

  return dest;
}


/* 
   Variable name translation functions.
   We have to process the varname for the following calcs:
   - 73/82/83/83+: binary-coded to ASCII charset translation
   - 85/86: no translation
   - 89/92/92+: TI's charset to user-defined charset (ASCII/Latin1/Unicode)

   See the protocol doc for more informations
*/

char *ti73_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  int i;

  switch (vartype) {
  case TI73_WDW:
    strcpy(translate, "Window");
    break;
  case TI73_ZSTO:
    strcpy(translate, "RclWin");
    break;
  case TI73_TAB:
    strcpy(translate, "TblSet");
    break;
  default:
    switch (varname[0] & 0xFF) {
    case 0x5D:
      if (varname[1] > 0x09) {
	for (i = 0; i < 7; i++) {
	  (translate)[i] = varname[i + 1];
	}
	break;
      }
      sprintf(translate, "L%c", varname[1] + '0');	//TI73 != TI83 here
      break;
    case 0x5C:
      sprintf(translate, "[%c]", varname[1] + 'A');
      break;
    case 0x5E:
      switch ((varname[1] & 0xF0) >> 4) {
      case 1:
	if (varname[1] != 0x19)
	  sprintf(translate, "Y%c", (varname[1] & 0xFF) + '1' - 0x10);
	else
	  strcpy(translate, "Y0");
	break;
      case 2:
	if (varname[1] & 0x01) {
	  sprintf(translate, "X%ct", ((varname[1] - 0x20) >> 1) + '1');
	} else {
	  sprintf(translate, "Y%ct", ((varname[1] - 0x20) >> 1) + '1');
	}
	break;
      case 4:
	sprintf(translate, "r%c", (varname[1] & 0xFF) - 0x40 + '1');
	break;
      case 8:
	sprintf(translate, "%c", (varname[1] & 0xFF) - 0x80 + 'u');
	break;
      }
      break;
    case 0xAA:
      if (varname[1] != 0x09)
	sprintf(translate, "Str%c", varname[1] + '1');
      else
	strcpy(translate, "Str0");
      break;
    case 0x60:
      if (varname[1] != 0x09)
	sprintf(translate, "Pic%c", varname[1] + '1');
      else
	strcpy(translate, "Pic0");
      break;
    case 0x61:
      if (varname[1] != 0x09)
	sprintf(translate, "GDB%c", varname[1] + '1');
      else
	strcpy(translate, "GDB0");
      break;
    default:
      strcpy(translate, varname);
    }
    break;
  }

  return translate;
}

char *ti82_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  int i;

  /*
     printf("-> <%s> ", varname);
     for(i=0; i<8; i++) { printf("%02X ", 0xFF & varname[i]); }
     printf("\n");
   */
  switch (vartype) {
  case TI82_WDW:
    strcpy(translate, "Window");
    break;
  case TI82_ZSTO:
    strcpy(translate, "RclWin");
    break;
  case TI82_TAB:
    strcpy(translate, "TblSet");
    break;
  default:
    switch (varname[0] & 0xFF) {
    case 0x5D:			/* List */
      if (varname[1] > 0x09) {
	for (i = 0; i < 7; i++) {
	  translate[i] = varname[i + 1];
	}
	break;
      }
      sprintf(translate, "L%c", varname[1] + '1');
      break;
    case 0x5C:			/* Matrix */
      sprintf(translate, "[%c]", varname[1] + 'A');
      break;
    case 0x5E:			/* Equations */
      switch ((varname[1] & 0xF0) >> 4) {
      case 1:
	if (varname[1] != 0x19)
	  sprintf(translate, "Y%c", (varname[1] & 0xFF) + '1' - 0x10);
	else
	  strcpy(translate, "Y0");
	break;
      case 2:
	if (varname[1] & 0x01) {
	  sprintf(translate, "Y%ct", ((varname[1] - 0x20) >> 1) + '1');
	} else {
	  sprintf(translate, "X%ct", ((varname[1] - 0x20) >> 1) + '1');
	}
	break;
      case 4:
	sprintf(translate, "r%c", (varname[1] & 0xFF) - 0x40 + '1');
	break;
      case 8:
	sprintf(translate, "%c", (varname[1] & 0xFF) - 0x80 + 'u');
	break;
      }
      break;
    case 0x60:			/* Pictures */
      sprintf(translate, "Pic%c", varname[1] + '1');
      break;
    case 0x61:			/* Gdb */
      sprintf(translate, "GDB%c", varname[1] + '1');
      break;
    default:
      strcpy(translate, varname);
    }
    break;
  }

  return translate;
}

char *ti83_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  int i;

  switch (vartype) {
  case TI82_WDW:
    strcpy(translate, "Window");
    break;
  case TI82_ZSTO:
    strcpy(translate, "RclWin");
    break;
  case TI82_TAB:
    strcpy(translate, "TblSet");
    break;
  default:
    switch (varname[0] & 0xFF) {
    case 0x5D:
      if (varname[1] > 0x09) {
	for (i = 0; i < 7; i++) {
	  (translate)[i] = varname[i + 1];
	}
	break;
      }
      if (varname[1] != 0x09)
	sprintf(translate, "L%c", varname[1] + '1');
      else
	strcpy(translate, "L9");
      break;
    case 0x5C:
      sprintf(translate, "[%c]", varname[1] + 'A');
      break;
    case 0x5E:
      switch ((varname[1] & 0xF0) >> 4) {
      case 1:
	if (varname[1] != 0x19)
	  sprintf(translate, "Y%c", (varname[1] & 0xFF) + '1' - 0x10);
	else
	  strcpy(translate, "Y0");
	break;
      case 2:
	if (varname[1] & 0x01) {
	  sprintf(translate, "X%ct", ((varname[1] - 0x20) >> 1) + '1');
	} else {
	  sprintf(translate, "Y%ct", ((varname[1] - 0x20) >> 1) + '1');
	}
	break;
      case 4:
	sprintf(translate, "r%c", (varname[1] & 0xFF) - 0x40 + '1');
	break;
      case 8:
	sprintf(translate, "%c", (varname[1] & 0xFF) - 0x80 + 'u');
	break;
      }
      break;
    case 0xAA:
      if (varname[1] != 0x09)
	sprintf(translate, "Str%c", varname[1] + '1');
      else
	strcpy(translate, "Str0");
      break;
    case 0x60:
      if (varname[1] != 0x09)
	sprintf(translate, "Pic%c", varname[1] + '1');
      else
	strcpy(translate, "Pic0");
      break;
    case 0x61:
      if (varname[1] != 0x09)
	sprintf(translate, "GDB%c", varname[1] + '1');
      else
	strcpy(translate, "GDB0");
      break;
    default:
      strcpy(translate, varname);
    }
    break;
  }

  return translate;
}

/* TI83+ is not supported yet */
char *ti83p_translate_varname(const char *varname, char *translate,
			      uint8_t vartype)
{
  return ti83_translate_varname(varname, translate, vartype);
}

/*
  Other calc do not need varname translation but these functions are provided
  as general functions
*/

char *ti85_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  strcpy(translate, varname);
  return translate;
}

char *ti86_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  strcpy(translate, varname);
  return translate;
}

char *ti89_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  switch(encoding) {
  case ENCODING_ASCII: return tifiles_convert_to_ascii(translate, varname);
  case ENCODING_LATIN1: return tifiles_convert_to_latin1(translate, varname);
  case ENCODING_UNICODE: return tifiles_convert_to_unicode(translate, varname);
  }

  return translate;
}

char *ti92_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  strcpy(translate, varname);
  return translate;
}

char *ti92p_translate_varname(const char *varname, char *translate,
			      uint8_t vartype)
{
  strcpy(translate, varname);
  return translate;
}

char *v200_translate_varname(const char *varname, char *translate,
			     uint8_t vartype)
{
  strcpy(translate, varname);
  return translate;
}

char *tixx_translate_varname(const char *varname, char *translate,
			     uint8_t vartype, TicalcType calc_type)
{
  switch (calc_type) {
  case CALC_TI73:
    return ti73_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI82:
    return ti82_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI83:
    return ti83_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI83P:
    return ti83p_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI85:
    return ti85_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI86:
    return ti86_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI89:
    return ti89_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI92:
    return ti92_translate_varname(varname, translate, vartype);
    break;
  case CALC_TI92P:
    return ti92p_translate_varname(varname, translate, vartype);
    break;
  case CALC_V200:
    return v200_translate_varname(varname, translate, vartype);
    break;
  default:
    return "invalid calc type !";
    break;
  }
}

extern int tifiles_calc_type;

TIEXPORT char *TICALL tifiles_translate_varname(const char *varname,
						char *translate,
						uint8_t vartype)
{
  return tixx_translate_varname(varname, translate, vartype, 
				tifiles_calc_type);
}

TIEXPORT char *TICALL tifiles_translate_varname_static(const char *varname,
						       uint8_t vartype)
{
  static char trans[9];
  
  return tixx_translate_varname(varname, trans, vartype,
                                tifiles_calc_type);
}

/* obsolete, replaced by the func below */
TIEXPORT char *TICALL tifiles_translate_varname2(const char *varname,
                                                 uint8_t vartype)
{
  return tifiles_translate_varname_static(varname, vartype);
}
