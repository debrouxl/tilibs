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
#include "types83p.h"
#include "logging.h"

struct {
        const char * type;
        const char * fext83p;
        const char * fext84pc;
        const char * fext83pce;
        const char * fext84pce;
        const char * icon;
        const char * desc;
} TI83p_CONST[TI83p_MAXTYPES + 1] =
{
  {"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")}, // 0
  {"LIST",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "List",         N_("List")},
  {"MAT",    "8Xm",   "8Xm",   "8Xm",   "8Xm",   "Matrix",       N_("Matrix")},
  {"EQU",    "8Xe",   "8Xe",   "8Xe",   "8Xe",   "Equation",     N_("Equation")}, // Also 8Xy
  {"STR",    "8Xs",   "8Xs",   "8Xs",   "8Xs",   "String",       N_("String")},
  {"PRGM",   "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Program",      N_("Program")},  // 5
  {"PPRGM",  "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Program",      N_("Program")},
  {"PIC",    "8Xi",   "8Ci",   "8Ci",   "8Ci",   "Picture",      N_("Picture")},
  {"GDB",    "8Xd",   "8Xd",   "8Xd",   "8Xd",   "GDB",          N_("GDB")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 10, 0xA
  {"EQU",    "8Xe",   "8Xe",   "8Xe",   "8Xe",   "Equation",     N_("Equation")}, /* NewEqu */ // Also 8Xy
  {"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
  {"LIST",   "8Xl",   "8Xl",   "8Xl",   "8Xl",   "List",         N_("List")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
  {"WINDW",  "8Xw",   "8Xw",   "8Xw",   "8Xw",   "Window Setup", N_("Window Setup")}, // 15, 0xF
  {"ZSTO",   "8Xz",   "8Xz",   "8Xz",   "8Xz",   "Zoom",         N_("Zoom")},
  {"TABLE",  "8Xt",   "8Xt",   "8Xt",   "8Xt",   "Table Setup",  N_("Table Setup")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
  {"BKUP",   "8Xb",   "8Xb",   "8Xb",   "8Xb",   "Backup",       N_("Backup")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 20, 0x14
  {"APPV",   "8Xv",   "8Xv",   "8Xv",   "8Xv",   "App Var",      N_("App Var")},
  {"TPRGM",  "8Xp",   "8Xp",   "8Xp",   "8Xp",   "Program",      N_("Program")}, /* TempProg */
  {"GRP",    "8Xo",   "8Xo",   "8Xo",   "8Xo",   "Group Var",    N_("Group Var")}, // Also 8Xg
  {"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")}, /* Fraction */
  {"DIR",    "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")}, // 25, 0x19
  {"IMAGE",  "8Xa",   "8Ca",   "8Ca",   "8Ca",   "Image",        N_("Image")},
  {"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
  {"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
  {"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
  {"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")}, // 30, 0x1E
  {"CPLX",   "8Xc",   "8Xc",   "8Xc",   "8Xc",   "Complex",      N_("Complex")},
  {"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
  {"REAL",   "8Xn",   "8Xn",   "8Xn",   "8Xn",   "Real",         N_("Real")},
  {"",       "8X?",   "8X?",   "8X?",   "8X?",   "Unknown",      N_("Unknown")},
  {"AMS",    "8Xu",   "8Cu",   "8Pu",   "8Eu",   "OS upgrade",   N_("OS upgrade")}, // 35, 0x23
  {"APPL",   "8Xk", "8Ck", "8Pk", "8Ek", "Application", N_("Application")},
  {"CERT",   "8Xq", "8Cq", "8Pq", "8Eq", "Certificate", N_("Certificate")},
  {"IDLIST", "8Xidl", "8Cidl", "8Pidl", "8Eidl", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")}, // 40, 0x28
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")}, // 45, 0x2D
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},
  {"",       "8X?", "8X?", "8X?", "8X?", "Unknown", N_("Unknown")},

  {NULL, NULL, NULL},
};

// Return the type corresponding to the value
const char *ti83p_byte2type(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].type : "";
}
const char *ti84p_byte2type(uint8_t data)
{
	return ti83p_byte2type(data);
}
const char *ti84pc_byte2type(uint8_t data)
{
	return ti83p_byte2type(data);
}
const char *ti83pce_byte2type(uint8_t data)
{
	return ti83p_byte2type(data);
}
const char *ti84pce_byte2type(uint8_t data)
{
	return ti83p_byte2type(data);
}

// Return the value corresponding to the type
uint8_t ti83p_type2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83p_MAXTYPES; i++) 
	{
		if (!strcmp(TI83p_CONST[i].type, s))
			break;
	}

	//if (i == TI83p_MAXTYPES) tifiles_warning(_("ti83p_byte2type: unknown type.\n"));

	return i;
}
uint8_t ti84p_type2byte(const char *s)
{
	return ti83p_type2byte(s);
}
uint8_t ti84pc_type2byte(const char *s)
{
	return ti83p_type2byte(s);
}
uint8_t ti83pce_type2byte(const char *s)
{
	return ti83p_type2byte(s);
}
uint8_t ti84pce_type2byte(const char *s)
{
	return ti83p_type2byte(s);
}

// Return the file extension corresponding to the value
const char *ti83p_byte2fext(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].fext83p : "8X?";
}
const char *ti84p_byte2fext(uint8_t data)
{
	return ti83p_byte2fext(data);
}
const char *ti84pc_byte2fext(uint8_t data)
{
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].fext84pc : "8C?";
}
const char *ti83pce_byte2fext(uint8_t data)
{
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].fext83pce : "8C?";
}
const char *ti84pce_byte2fext(uint8_t data)
{
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].fext84pce : "8C?";
}

// Return the value corresponding to the file extension
uint8_t ti83p_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext83p, s))
			break;
	}

	//if (i == TI83p_MAXTYPES)	tifiles_warning(("ti83p_fext2byte: unknown type.\n"));
	return i;
}
uint8_t ti84p_fext2byte(const char *s)
{
	return ti83p_fext2byte(s);
}
uint8_t ti84pc_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext84pc, s))
			break;
	}

	return i;
}
uint8_t ti83pce_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext83pce, s))
			break;
	}

	return i;
}
uint8_t ti84pce_fext2byte(const char *s)
{
	int i;

	for (i = 0; i < TI83p_MAXTYPES; i++) 
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext84pce, s))
			break;
	}

	return i;
}


// Return the descriptive associated with the vartype
const char *ti83p_byte2desc(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83p_MAXTYPES) ? _(TI83p_CONST[data].desc) : _("Unknown");
}
const char *ti84p_byte2desc(uint8_t data)
{
	return ti83p_byte2desc(data);
}
const char *ti84pc_byte2desc(uint8_t data)
{
	return ti83p_byte2desc(data);
}
const char *ti83pce_byte2desc(uint8_t data)
{
	return ti83p_byte2desc(data);
}
const char *ti84pce_byte2desc(uint8_t data)
{
	return ti83p_byte2desc(data);
}

// Return the icon name associated with the vartype
const char *ti83p_byte2icon(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("typesxx: unknown type (%02x).\n"), data);
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].icon : "Unknown";
}
const char *ti84p_byte2icon(uint8_t data)
{
	return ti83p_byte2icon(data);
}
const char *ti84pc_byte2icon(uint8_t data)
{
	return ti83p_byte2icon(data);
}
const char *ti83pce_byte2icon(uint8_t data)
{
	return ti83p_byte2icon(data);
}
const char *ti84pce_byte2icon(uint8_t data)
{
	return ti83p_byte2icon(data);
}

#endif
