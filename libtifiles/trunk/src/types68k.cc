/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  Variable type ID and file extensions
*/

#ifndef DISABLE_TI9X

#include <string.h>
#include "gettext.h"
#include "logging.h"
#include "internal.h"

/*
  Missing ?
  - Lab Report (89r / 92r / 9xr / v2r)
*/
const TI68k_DATA TI68k_CONST[TI89_MAXTYPES + 1] =
{
	{"EXPR", "89e", "92e", "9xe", "v2e", "Expression", N_("Expression")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"LIST", "89l", "92l", "9xl", "v2l", "List", N_("List")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"MAT", "89m", "92m", "9xm", "v2m", "Matrix", N_("Matrix")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"DATA", "89c", "92c", "9xc", "v2c", "Data", N_("Data")},
	{"TEXT", "89t", "92t", "9xt", "v2t", "Text", N_("Text")},
	{"STR", "89s", "92s", "9xs", "v2s", "String", N_("String")},
	{"GDB", "89d", "92d", "9xd", "v2d", "GDB", N_("GDB")},
	{"FIG", "89a", "92a", "9xa", "v2a", "Figure", N_("Figure")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"PIC", "89i", "92i", "9xi", "v2i", "Picture", N_("Picture")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"PRGM", "89p", "92p", "9xp", "v2p", "Program", N_("Program")},
	{"FUNC", "89f", "92f", "9xf", "v2f", "Function", N_("Function")},
	{"MAC", "89x", "92x", "9xx", "v2x", "Macro", N_("Macro")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"CLOCK", "89clk", "92?", "9xclk", "v2clk", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"RDIR", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"LDIR", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"OTH", "89y", "92?", "9xy", "v2y", "Zipped", N_("Other (Zipped, etc.)")},
	{"BKUP", "89g", "92b", "9xg", "v2g", "Backup", N_("Backup")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"DIR", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"ASM", "89z", "92z", "9xz", "v2z", "Asm Program", N_("Asm Program")},
	{"IDLIST", "89idl", "92?", "9xidl", "v2idl", "ID-LIST", N_("ID-LIST")},
	{"AMS", "89u", "92?", "9xu", "v2u", "OS upgrade", N_("OS upgrade")},
	{"APPL", "89k", "92?", "9xk", "v2k", "Application", N_("Application")},
	{"CERT", "89q", "92?", "9xq", "v2q", "Certificate", N_("Certificate")},
	{"LOCKED", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"ARCHIV", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{"", "89?", "92?", "9x?", "v2?", "Unknown", N_("Unknown")},
	{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
};

#endif
