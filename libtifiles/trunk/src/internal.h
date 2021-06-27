/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2009 Romain Liévin
 *  Copyright (C) 2015      Lionel Debroux
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

#ifndef __TIFILES_INTERNAL__
#define __TIFILES_INTERNAL__

#include "typesxx.h"

typedef struct {
	const char * type;
	const char * fext;
	const char * icon;
	const char * desc;
} TIXX_DATA;

typedef struct {
	const char * type;
	const char * fext83p;
	const char * fext84pc;
	const char * fext83pce;
	const char * fext84pce;
	const char * fext82a;
	const char * fext84pt;
	const char * fext82aep;
	const char * icon;
	const char * desc;
} TI83p_DATA;

typedef struct {
	const char * type;
	const char * fext89;
	const char * fext92;
	const char * fext92p;
	const char * fextv200;
	const char * icon;
	const char * desc;
} TI68k_DATA;


extern const TIXX_DATA TI73_CONST[TI73_MAXTYPES + 1];
extern const TIXX_DATA TI82_CONST[TI82_MAXTYPES + 1];
extern const TIXX_DATA TI83_CONST[TI83_MAXTYPES + 1];
extern const TI83p_DATA TI83p_CONST[TI83p_MAXTYPES + 1];
extern const TIXX_DATA TI85_CONST[TI85_MAXTYPES + 1];
extern const TIXX_DATA TI86_CONST[TI86_MAXTYPES + 1];
extern const TI68k_DATA TI68k_CONST[TI89_MAXTYPES + 1];
extern const TIXX_DATA NSP_CONST[NSP_MAXTYPES + 1];


// Return the type corresponding to the value
static inline const char *tixx_byte2type(const TIXX_DATA * vardefs, uint8_t maxtypes, uint8_t data)
{
	//if(data >= maxtypes) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, maxtypes);
	return (data < maxtypes) ? ((vardefs[data].fext[2] != '?') ? vardefs[data].type : "") : "";
}

static inline const char *ti83p_byte2type(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83p_MAXTYPES);
	return (data < TI83p_MAXTYPES) ? ((TI83p_CONST[data].fext83p[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

#define ti84p_byte2type ti83p_byte2type

static inline const char *ti84pc_byte2type(uint8_t data)
{
	//if(data >= TI84pc_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pc_MAXTYPES);
	return (data < TI84pc_MAXTYPES) ? ((TI83p_CONST[data].fext84pc[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti83pce_byte2type(uint8_t data)
{
	//if(data >= TI83pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83pce_MAXTYPES);
	return (data < TI83pce_MAXTYPES) ? ((TI83p_CONST[data].fext83pce[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti84pce_byte2type(uint8_t data)
{
	//if(data >= TI84pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pce_MAXTYPES);
	return (data < TI84pce_MAXTYPES) ? ((TI83p_CONST[data].fext84pce[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti82a_byte2type(uint8_t data)
{
	//if(data >= TI82a_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82a_MAXTYPES);
	return (data < TI82a_MAXTYPES) ? ((TI83p_CONST[data].fext82a[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti84pt_byte2type(uint8_t data)
{
	//if(data >= TI84pt_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pt_MAXTYPES);
	return (data < TI84pt_MAXTYPES) ? ((TI83p_CONST[data].fext84pt[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti82aep_byte2type(uint8_t data)
{
	//if(data >= TI82aep_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82aep_MAXTYPES);
	return (data < TI82aep_MAXTYPES) ? ((TI83p_CONST[data].fext82aep[2] != '?') ? TI83p_CONST[data].type : "") : "";
}

static inline const char *ti89_byte2type(uint8_t data)
{
	//if(data >= TI89_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI89_MAXTYPES);
	return (data < TI89_MAXTYPES) ? ((TI68k_CONST[data].fext89[2] != '?') ? TI68k_CONST[data].type : "") : "";
}

#define ti89t_byte2type ti89_byte2type

static inline const char *ti92_byte2type(uint8_t data)
{
	//if(data >= TI92_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92_MAXTYPES);
	return (data < TI92_MAXTYPES) ? ((TI68k_CONST[data].fext92[2] != '?') ? TI68k_CONST[data].type : "") : "";
}

static inline const char *ti92p_byte2type(uint8_t data)
{
	//if(data >= TI92p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92p_MAXTYPES);
	return (data < TI92p_MAXTYPES) ? ((TI68k_CONST[data].fext92p[2] != '?') ? TI68k_CONST[data].type : "") : "";
}

static inline const char *v200_byte2type(uint8_t data)
{
	//if(data >= V200_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, V200_MAXTYPES);
	return (data < V200_MAXTYPES) ? ((TI68k_CONST[data].fextv200[2] != '?') ? TI68k_CONST[data].type : "") : "";
}



// Return the value corresponding to the type
static inline uint8_t tixx_type2byte(const TIXX_DATA * vardefs, uint8_t maxtypes, const char *s)
{
	uint8_t i;

	for (i = 0; i < maxtypes; i++)
	{
		if (!strcmp(vardefs[i].type, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti83p_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI83p_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext83p[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

#define ti84p_type2byte ti83p_type2byte

static inline uint8_t ti84pc_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pc_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext84pc[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti83pce_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI83pce_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext83pce[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti84pce_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pce_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext84pce[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti82a_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI82a_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext82a[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti84pt_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pt_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext84pt[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti82aep_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI82aep_MAXTYPES; i++)
	{
		if (!strcmp(TI83p_CONST[i].type, s) && TI83p_CONST[i].fext82aep[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti89_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI89_MAXTYPES; i++)
	{
		if (!strcmp(TI68k_CONST[i].type, s) && TI68k_CONST[i].fext89[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

#define ti89t_type2byte ti89_type2byte

static inline uint8_t ti92_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI92_MAXTYPES; i++)
	{
		if (!strcmp(TI68k_CONST[i].type, s) && TI68k_CONST[i].fext92[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti92p_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI92p_MAXTYPES; i++)
	{
		if (!strcmp(TI68k_CONST[i].type, s) && TI68k_CONST[i].fext92p[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t v200_type2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < V200_MAXTYPES; i++)
	{
		if (!strcmp(TI68k_CONST[i].type, s) && TI68k_CONST[i].fextv200[2] != '?')
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown type %s."), __FUNCTION__, s);
	return i;
}



// Return the file extension corresponding to the value
static inline const char *tixx_byte2fext(const TIXX_DATA * vardefs, uint8_t maxtypes, uint8_t data, const char * fallback)
{
	//if(data >= maxtypes) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, maxtypes);
	return (data < maxtypes) ? vardefs[data].fext : fallback;
}

static inline const char *ti83p_byte2fext(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83p_MAXTYPES);
	return (data < TI83p_MAXTYPES) ? TI83p_CONST[data].fext83p : "8x?";
}

#define ti84p_byte2fext ti83p_byte2fext

static inline const char *ti84pc_byte2fext(uint8_t data)
{
	//if(data >= TI84pc_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pc_MAXTYPES);
	return (data < TI84pc_MAXTYPES) ? TI83p_CONST[data].fext84pc : "8C?";
}

static inline const char *ti83pce_byte2fext(uint8_t data)
{
	//if(data >= TI83pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83pce_MAXTYPES);
	return (data < TI83pce_MAXTYPES) ? TI83p_CONST[data].fext83pce : "8C?";
}

static inline const char *ti84pce_byte2fext(uint8_t data)
{
	//if(data >= TI84pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pce_MAXTYPES);
	return (data < TI84pce_MAXTYPES) ? TI83p_CONST[data].fext84pce : "8C?";
}

static inline const char *ti82a_byte2fext(uint8_t data)
{
	//if(data >= TI82a_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82a_MAXTYPES);
	return (data < TI82a_MAXTYPES) ? TI83p_CONST[data].fext82a : "8X?";
}

static inline const char *ti84pt_byte2fext(uint8_t data)
{
	//if(data >= TI84pt_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pt_MAXTYPES);
	return (data < TI84pt_MAXTYPES) ? TI83p_CONST[data].fext84pt : "8X?";
}

static inline const char *ti82aep_byte2fext(uint8_t data)
{
	//if(data >= TI82aep_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82aep_MAXTYPES);
	return (data < TI82aep_MAXTYPES) ? TI83p_CONST[data].fext82aep : "8X?";
}

static inline const char *ti89_byte2fext(uint8_t data)
{
	//if(data >= TI89_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI89_MAXTYPES);
	return (data < TI89_MAXTYPES) ? TI68k_CONST[data].fext89 : "89?";
}

#define ti89t_byte2fext ti89_byte2fext

static inline const char *ti92_byte2fext(uint8_t data)
{
	//if(data >= TI92_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92_MAXTYPES);
	return (data < TI92_MAXTYPES) ? TI68k_CONST[data].fext92 : "92?";
}

static inline const char *ti92p_byte2fext(uint8_t data)
{
	//if(data >= TI92p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92p_MAXTYPES);
	return (data < TI92p_MAXTYPES) ? TI68k_CONST[data].fext92p : "9x?";
}

static inline const char *v200_byte2fext(uint8_t data)
{
	//if(data >= V200_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, V200_MAXTYPES);
	return (data < V200_MAXTYPES) ? TI68k_CONST[data].fextv200 : "v2?";
}



// Return the value corresponding to the file extension
static inline uint8_t tixx_fext2byte(const TIXX_DATA * vardefs, uint8_t maxtypes, const char *s)
{
	uint8_t i;

	for (i = 0; i < maxtypes; i++)
	{
		if (!g_ascii_strcasecmp(vardefs[i].fext, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti83p_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI83p_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext83p, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

#define ti84p_fext2byte ti83p_fext2byte

static inline uint8_t ti84pc_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pc_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext84pc, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti83pce_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI83pce_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext83pce, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti84pce_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pce_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext84pce, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti82a_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI82a_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext82a, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti84pt_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI84pt_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext84pt, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti82aep_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI82aep_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI83p_CONST[i].fext82aep, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti89_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI89_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI68k_CONST[i].fext89, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

#define ti89t_fext2byte ti89_fext2byte

static inline uint8_t ti92_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI92_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI68k_CONST[i].fext92, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t ti92p_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < TI92p_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI68k_CONST[i].fext92p, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}

static inline uint8_t v200_fext2byte(const char *s)
{
	uint8_t i;

	for (i = 0; i < V200_MAXTYPES; i++)
	{
		if (!g_ascii_strcasecmp(TI68k_CONST[i].fextv200, s))
		{
			break;
		}
	}

	//if (i == maxtypes) tifiles_warning(_("%s: unknown fext %s."), __FUNCTION__, s);
	return i;
}



// Return the description associated with the vartype
static inline const char *tixx_byte2desc(const TIXX_DATA * vardefs, uint8_t maxtypes, uint8_t data)
{
	//if(data >= maxtypes) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, maxtypes);
	return (data < maxtypes) ? ((vardefs[data].fext[2] != '?') ? vardefs[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti83p_byte2desc(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83p_MAXTYPES);
	return (data < TI83p_MAXTYPES) ? ((TI83p_CONST[data].fext83p[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

#define ti84p_byte2desc ti83p_byte2desc

static inline const char *ti84pc_byte2desc(uint8_t data)
{
	//if(data >= TI84pc_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pc_MAXTYPES);
	return (data < TI84pc_MAXTYPES) ? ((TI83p_CONST[data].fext84pc[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti83pce_byte2desc(uint8_t data)
{
	//if(data >= TI83pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83pce_MAXTYPES);
	return (data < TI83pce_MAXTYPES) ? ((TI83p_CONST[data].fext83pce[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti84pce_byte2desc(uint8_t data)
{
	//if(data >= TI84pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pce_MAXTYPES);
	return (data < TI84pce_MAXTYPES) ? ((TI83p_CONST[data].fext84pce[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti82a_byte2desc(uint8_t data)
{
	//if(data >= TI82a_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82a_MAXTYPES);
	return (data < TI82a_MAXTYPES) ? ((TI83p_CONST[data].fext82a[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti84pt_byte2desc(uint8_t data)
{
	//if(data >= TI84pt_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pt_MAXTYPES);
	return (data < TI84pt_MAXTYPES) ? ((TI83p_CONST[data].fext84pt[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti82aep_byte2desc(uint8_t data)
{
	//if(data >= TI82aep_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82aep_MAXTYPES);
	return (data < TI82aep_MAXTYPES) ? ((TI83p_CONST[data].fext82aep[2] != '?') ? TI83p_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti89_byte2desc(uint8_t data)
{
	//if(data >= TI89_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI89_MAXTYPES);
	return (data < TI89_MAXTYPES) ? ((TI68k_CONST[data].fext89[2] != '?') ? TI68k_CONST[data].desc : _("Unknown")) : _("Unknown");
}

#define ti89t_byte2desc ti89_byte2desc

static inline const char *ti92_byte2desc(uint8_t data)
{
	//if(data >= TI92_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92_MAXTYPES);
	return (data < TI92_MAXTYPES) ? ((TI68k_CONST[data].fext92[2] != '?') ? TI68k_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *ti92p_byte2desc(uint8_t data)
{
	//if(data >= TI92p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92p_MAXTYPES);
	return (data < TI92p_MAXTYPES) ? ((TI68k_CONST[data].fext92p[2] != '?') ? TI68k_CONST[data].desc : _("Unknown")) : _("Unknown");
}

static inline const char *v200_byte2desc(uint8_t data)
{
	//if(data >= V200_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, V200_MAXTYPES);
	return (data < V200_MAXTYPES) ? ((TI68k_CONST[data].fextv200[2] != '?') ? TI68k_CONST[data].desc : _("Unknown")) : _("Unknown");
}



// Return the icon name associated with the vartype
static inline const char *tixx_byte2icon(const TIXX_DATA * vardefs, uint8_t maxtypes, uint8_t data)
{
	//if(data >= maxtypes) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, maxtypes);
	return (data < maxtypes) ? ((vardefs[data].fext[2] != '?') ? vardefs[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti83p_byte2icon(uint8_t data)
{
	//if(data >= TI83p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83p_MAXTYPES);
	return (data < TI83p_MAXTYPES) ? ((TI83p_CONST[data].fext83p[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

#define ti84p_byte2icon ti83p_byte2icon

static inline const char *ti84pc_byte2icon(uint8_t data)
{
	//if(data >= TI84pc_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pc_MAXTYPES);
	return (data < TI84pc_MAXTYPES) ? ((TI83p_CONST[data].fext84pc[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti83pce_byte2icon(uint8_t data)
{
	//if(data >= TI83pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI83pce_MAXTYPES);
	return (data < TI83pce_MAXTYPES) ? ((TI83p_CONST[data].fext83pce[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti84pce_byte2icon(uint8_t data)
{
	//if(data >= TI84pce_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pce_MAXTYPES);
	return (data < TI84pce_MAXTYPES) ? ((TI83p_CONST[data].fext84pce[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti82a_byte2icon(uint8_t data)
{
	//if(data >= TI82a_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82a_MAXTYPES);
	return (data < TI82a_MAXTYPES) ? ((TI83p_CONST[data].fext82a[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti84pt_byte2icon(uint8_t data)
{
	//if(data >= TI84pt_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI84pt_MAXTYPES);
	return (data < TI84pt_MAXTYPES) ? ((TI83p_CONST[data].fext84pt[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti82aep_byte2icon(uint8_t data)
{
	//if(data >= TI82aep_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI82aep_MAXTYPES);
	return (data < TI82aep_MAXTYPES) ? ((TI83p_CONST[data].fext82aep[2] != '?') ? TI83p_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti89_byte2icon(uint8_t data)
{
	//if(data >= TI89_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI89_MAXTYPES);
	return (data < TI89_MAXTYPES) ? ((TI68k_CONST[data].fext89[2] != '?') ? TI68k_CONST[data].icon : "Unknown") : "Unknown";
}

#define ti89t_byte2icon ti89_byte2icon

static inline const char *ti92_byte2icon(uint8_t data)
{
	//if(data >= TI92_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92_MAXTYPES);
	return (data < TI92_MAXTYPES) ? ((TI68k_CONST[data].fext92[2] != '?') ? TI68k_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *ti92p_byte2icon(uint8_t data)
{
	//if(data >= TI92p_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, TI92p_MAXTYPES);
	return (data < TI92p_MAXTYPES) ? ((TI68k_CONST[data].fext92p[2] != '?') ? TI68k_CONST[data].icon : "Unknown") : "Unknown";
}

static inline const char *v200_byte2icon(uint8_t data)
{
	//if(data >= V200_MAXTYPES) tifiles_warning(_("%s: unknown type %02X >= %02X."), __FUNCTION__, data, V200_MAXTYPES);
	return (data < V200_MAXTYPES) ? ((TI68k_CONST[data].fextv200[2] != '?') ? TI68k_CONST[data].icon : "Unknown") : "Unknown";
}

#endif
