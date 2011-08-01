/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#include "ticalcs.h"

#include "keys73.h"
#include "keys83.h"
#include "keys83p.h"
#include "keys86.h"
#include "keys89.h"
#include "keys92p.h"

extern const CalcKey TI73_KEYS[];
extern const CalcKey TI83_KEYS[];
extern const CalcKey TI83P_KEYS[];
extern const CalcKey TI86_KEYS[];
extern const CalcKey TI89_KEYS[];
extern const CalcKey TI92P_KEYS[];

/**
 * ticalcs_keys_73:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey* TICALL ticalcs_keys_73(uint8_t ascii_code)
{
  return &TI73_KEYS[ascii_code];
}

/**
 * ticalcs_keys_83:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey*  TICALL ticalcs_keys_83(uint8_t ascii_code)
{
  return &TI83_KEYS[ascii_code];
}

/**
 * ticalcs_keys_83p:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey*  TICALL ticalcs_keys_83p(uint8_t ascii_code)
{
  return &TI83P_KEYS[ascii_code];
}

/**
 * ticalcs_keys_86:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey*  TICALL ticalcs_keys_86(uint8_t ascii_code)
{
  return &TI86_KEYS[ascii_code];
}

/**
 * ticalcs_keys_89:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey*  TICALL ticalcs_keys_89(uint8_t ascii_code)
{
  return &TI89_KEYS[ascii_code];
}

/**
 * ticalcs_keys_92p:
 * @code: an ASCII character
 *
 * Conversion of an ASCII code into a TI key.
 *
 * Return value: a TI key.
 **/
TIEXPORT3 const CalcKey*  TICALL ticalcs_keys_92p(uint8_t ascii_code)
{
  return &TI92P_KEYS[ascii_code];
}
