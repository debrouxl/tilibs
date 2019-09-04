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

#ifndef DISABLE_TI8X

#include <string.h>
#include "gettext.h"
#include "logging.h"
#include "internal.h"

const TI83p_DATA TI83p_CONST[TI83p_MAXTYPES + 1] =
{
	{"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")}, // 0
	{"LIST",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "List",         N_("List")},
	{"MAT",    "8Xm",   "8Xm",   "8Xm",   "8Xm",   "8Xm",   "8Xm",   "Matrix",       N_("Matrix")},
	{"EQU",    "8Xe",   "8Xe",   "8Xe",   "8Xe",   "8Xe",   "8Xe",   "Equation",     N_("Equation")}, // Also 8Xy
	{"STR",    "8Xs",   "8Xs",   "8Xs",   "8Xs",   "8Xs",   "8Xs",   "String",       N_("String")},
	{"PRGM",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Program",      N_("Program")},  // 5
	{"PPRGM",  "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Asm Program",  N_("Asm Program")},
	{"PIC",    "8Xi",   "8Ci",   "8Ci",   "8Ci",   "8Xi",   "8Xi",   "Picture",      N_("Picture")},
	{"GDB",    "8Xd",   "8Xd",   "8Xd",   "8Xd",   "8Xd",   "8Xd",   "GDB",          N_("GDB")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 10, 0xA
	{"EQU",    "8Xe",   "8Xe",   "8Xe",   "8Xe",   "8Xe",   "8Xe",   "Equation",     N_("Equation")}, /* NewEqu */ // Also 8Xy
	{"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
	{"LIST",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "List",         N_("List")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"WINDW",  "8Xw",   "8Xw",   "8Xw",   "8Xw",   "8Xw",   "8Xw",   "Window Setup", N_("Window Setup")}, // 15, 0xF
	{"ZSTO",   "8Xz",   "8Xz",   "8Xz",   "8Xz",   "8Xz",   "8Xz",   "Zoom",         N_("Zoom")},
	{"TABLE",  "8Xt",   "8Xt",   "8Xt",   "8Xt",   "8Xt",   "8Xt",   "Table Setup",  N_("Table Setup")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"BKUP",   "8Xb",   "8Xb",   "8Xb",   "8Xb",   "8Xb",   "8Xb",   "Backup",       N_("Backup")},
	{"APPOBJ", "8Xv",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "Unknown",      N_("App Obj")}, /* AppObj */ // 20, 0x14
	{"APPV",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "App Var",      N_("App Var")},
	{"TPRGM",  "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Program",      N_("Program")}, /* TempProgObj */
	{"GRP",    "8Xo",   "8Xo",   "8Xo",   "8Xo",   "8Xo",   "8Xo",   "Group Var",    N_("Group Var")}, // Also 8Xg
	{"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")}, /* Fraction */
	{"DIR",    "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 25, 0x19
	{"IMAGE",  "8Xa",   "8Ca",   "8Ca",   "8Ca",   "8Xa",   "8Xa",   "Image",        N_("Image")},
	{"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
	{"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
	{"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
	{"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")}, // 30, 0x1E
	{"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
	{"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
	{"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"AMS",    "8Xu",   "8Cu",   "8Pu",   "8Eu",   "82u",   "8Xu",   "OS upgrade",   N_("OS upgrade")}, // 35, 0x23
	{"APPL",   "8Xk",   "8Ck",   "8Ek",   "8Ek",   "8X?",   "8X?",   "Application",  N_("Application")},
	{"CERT",   "8Xq",   "8Cq",   "8Pq",   "8Eq",   "8X?",   "8X?",   "Certificate",  N_("Certificate")},
	{"IDLIST", "8Xidl", "8Cidl", "8Pidl", "8Eidl", "8Xidl", "8Xidl", "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 40, 0x28
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 45, 0x2D
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{"",       "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
	{NULL,     NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,           NULL}
};

#endif
