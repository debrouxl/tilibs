/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - charset library, a part of the TiLP project
 *  Copyright (C) 1999-2009 Romain Lievin and Kevin Kofler
 *  Copyright (C) 2009-2016 Lionel Debroux
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
	This unit contains generic functions for converting between a model
	string and enum CalcModel.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>

#include "ticonv.h"

TIEXPORT4 const char * TICALL ticonv_model_to_string(CalcModel model)
{
	switch (model)
	{
	case CALC_NONE:  return "none";
	case CALC_TI73:  return "TI73";
	case CALC_TI82:  return "TI82";
	case CALC_TI83:  return "TI83";
	case CALC_TI83P: return "TI83+";
	case CALC_TI84P: return "TI84+";
	case CALC_TI85:  return "TI85";
	case CALC_TI86:  return "TI86";
	case CALC_TI89:  return "TI89";
	case CALC_TI89T: return "TI89T";
	case CALC_TI92:  return "TI92";
	case CALC_TI92P: return "TI92+";
	case CALC_V200:  return "V200";
	case CALC_TI84P_USB: return "TI84+ USB";
	case CALC_TI89T_USB: return "TI89T USB";
	case CALC_NSPIRE: return "Nspire";
	case CALC_TI80: return "TI80";
	case CALC_TI84PC: return "TI84+CSE";
	case CALC_TI84PC_USB: return "TI84+CSE USB";
	case CALC_TI83PCE_USB: return "TI83PCE USB";
	case CALC_TI84PCE_USB: return "TI84+CE USB";
	case CALC_TI82A_USB: return "TI82A USB";
	case CALC_TI84PT_USB: return "TI84+T USB";
	default: return "unknown";
	}
}

TIEXPORT4 CalcModel TICALL ticonv_string_to_model(const char *str)
{
	if (str != NULL)
	{
		if (   !g_ascii_strcasecmp(str, "TI-73")
		    || !g_ascii_strcasecmp(str, "TI73")
		    || !g_ascii_strcasecmp(str, "73")
		   )
			return CALC_TI73;
		else if (   !g_ascii_strcasecmp(str, "TI-82")
		         || !g_ascii_strcasecmp(str, "TI82")
		         || !g_ascii_strcasecmp(str, "82")
		        )
			return CALC_TI82;
		else if (   !g_ascii_strcasecmp(str, "TI-83")
		         || !g_ascii_strcasecmp(str, "TI83")
		         || !g_ascii_strcasecmp(str, "83")
		        )
			return CALC_TI83;
		else if (   !g_ascii_strcasecmp(str, "TI-83+")
		         || !g_ascii_strcasecmp(str, "TI-83p")
		         || !g_ascii_strcasecmp(str, "TI83+")
		         || !g_ascii_strcasecmp(str, "TI83p")
		         || !g_ascii_strcasecmp(str, "83+")
		         || !g_ascii_strcasecmp(str, "83p")
		        )
			return CALC_TI83P;
		else if (   !g_ascii_strcasecmp(str, "TI-84+")
		         || !g_ascii_strcasecmp(str, "TI-84p")
		         || !g_ascii_strcasecmp(str, "TI84+")
		         || !g_ascii_strcasecmp(str, "TI84p")
		         || !g_ascii_strcasecmp(str, "84+")
		         || !g_ascii_strcasecmp(str, "84p")
		        )
			return CALC_TI84P;
		else if (   !g_ascii_strcasecmp(str, "TI-85")
		         || !g_ascii_strcasecmp(str, "TI85")
		         || !g_ascii_strcasecmp(str, "85")
		        )
			return CALC_TI85;
		else if (   !g_ascii_strcasecmp(str, "TI-86")
		         || !g_ascii_strcasecmp(str, "TI86")
		         || !g_ascii_strcasecmp(str, "86")
		        )
			return CALC_TI86;
		else if (   !g_ascii_strcasecmp(str, "TI-89")
		         || !g_ascii_strcasecmp(str, "TI89")
		         || !g_ascii_strcasecmp(str, "89")
		        )
			return CALC_TI89;
		else if (   !g_ascii_strcasecmp(str, "TI-89T")
		         || !g_ascii_strcasecmp(str, "TI89T")
		         || !g_ascii_strcasecmp(str, "89T")
		        )
			return CALC_TI89T;
		else if (   !g_ascii_strcasecmp(str, "TI-92")
		         || !g_ascii_strcasecmp(str, "TI-92II")
		         || !g_ascii_strcasecmp(str, "TI92")
		         || !g_ascii_strcasecmp(str, "TI92II")
		         || !g_ascii_strcasecmp(str, "92")
		         || !g_ascii_strcasecmp(str, "92II")
		        )
			return CALC_TI92;
		else if (   !g_ascii_strcasecmp(str, "TI-92+")
		         || !g_ascii_strcasecmp(str, "TI-92P")
		         || !g_ascii_strcasecmp(str, "TI92+")
		         || !g_ascii_strcasecmp(str, "TI92P")
		         || !g_ascii_strcasecmp(str, "92+")
		         || !g_ascii_strcasecmp(str, "92P")
		        )
			return CALC_TI92P;
		else if (   !g_ascii_strcasecmp(str, "TI-V200")
		         || !g_ascii_strcasecmp(str, "TIV200")
		         || !g_ascii_strcasecmp(str, "V200")
		        )
			return CALC_V200;
		else if (   !g_ascii_strcasecmp(str, "TI-84+ USB")
		         || !g_ascii_strcasecmp(str, "TI-84p USB")
		         || !g_ascii_strcasecmp(str, "TI84+ USB")
		         || !g_ascii_strcasecmp(str, "TI84p USB")
		         || !g_ascii_strcasecmp(str, "84+ USB")
		         || !g_ascii_strcasecmp(str, "84p USB")
		        )
			return CALC_TI84P_USB;
		else if (   !g_ascii_strcasecmp(str, "TI-89T USB")
		         || !g_ascii_strcasecmp(str, "TI89T USB")
		         || !g_ascii_strcasecmp(str, "89T USB")
		        )
			return CALC_TI89T_USB;
		else if (   !g_ascii_strncasecmp(str, "TI-Nspire", 9)
		         || !g_ascii_strncasecmp(str, "TI Nspire", 9)
		         || !g_ascii_strncasecmp(str, "Nspire", 6)
		         || !g_ascii_strncasecmp(str, "Nsp", 3)
		        )
			return CALC_NSPIRE;
		else if (   !g_ascii_strcasecmp(str, "TI-80")
		         || !g_ascii_strcasecmp(str, "TI80")
		         || !g_ascii_strcasecmp(str, "80")
		        )
			return CALC_TI80;
		else if (   !g_ascii_strcasecmp(str, "TI-84+CSE")
		         || !g_ascii_strcasecmp(str, "TI-84PCSE")
		         || !g_ascii_strcasecmp(str, "TI84+CSE")
		         || !g_ascii_strcasecmp(str, "TI84PCSE")
		         || !g_ascii_strcasecmp(str, "84+CSE")
		         || !g_ascii_strcasecmp(str, "84PCSE")
		        )
			return CALC_TI84PC;
		else if (   !g_ascii_strcasecmp(str, "TI-84+CSE USB")
		         || !g_ascii_strcasecmp(str, "TI-84PCSE USB")
		         || !g_ascii_strcasecmp(str, "TI84+CSE USB")
		         || !g_ascii_strcasecmp(str, "TI84PCSE USB")
		         || !g_ascii_strcasecmp(str, "84+CSE USB")
		         || !g_ascii_strcasecmp(str, "84PCSE USB")
		        )
			return CALC_TI84PC_USB;
		else if (   !g_ascii_strcasecmp(str, "TI-83PCE USB")
		         || !g_ascii_strcasecmp(str, "TI83PCE USB")
		         || !g_ascii_strcasecmp(str, "TI-83PCE")
		         || !g_ascii_strcasecmp(str, "TI83PCE")
		         || !g_ascii_strcasecmp(str, "83PCE USB")
		         || !g_ascii_strcasecmp(str, "83PCE")
		        )
			return CALC_TI83PCE_USB;
		else if (   !g_ascii_strcasecmp(str, "TI-84+CE USB")
		         || !g_ascii_strcasecmp(str, "TI-84PCE USB")
		         || !g_ascii_strcasecmp(str, "TI84+CE USB")
		         || !g_ascii_strcasecmp(str, "TI84PCE USB")
		         || !g_ascii_strcasecmp(str, "TI-84+CE")
		         || !g_ascii_strcasecmp(str, "TI-84PCE")
		         || !g_ascii_strcasecmp(str, "TI84+CE")
		         || !g_ascii_strcasecmp(str, "TI84PCE")
		         || !g_ascii_strcasecmp(str, "84+CE USB")
		         || !g_ascii_strcasecmp(str, "84PCE USB")
		         || !g_ascii_strcasecmp(str, "84+CE")
		         || !g_ascii_strcasecmp(str, "84PCE")
		        )
			return CALC_TI84PCE_USB;
		else if (   !g_ascii_strcasecmp(str, "TI-82A USB")
		         || !g_ascii_strcasecmp(str, "TI82A USB")
		         || !g_ascii_strcasecmp(str, "TI-82A")
		         || !g_ascii_strcasecmp(str, "TI82A")
		         || !g_ascii_strcasecmp(str, "82A USB")
		         || !g_ascii_strcasecmp(str, "82A")
		        )
			return CALC_TI82A_USB;
		else if (   !g_ascii_strcasecmp(str, "TI-84+T USB")
		         || !g_ascii_strcasecmp(str, "TI-84PT USB")
		         || !g_ascii_strcasecmp(str, "TI84+T USB")
		         || !g_ascii_strcasecmp(str, "TI84PT USB")
		         || !g_ascii_strcasecmp(str, "TI-84+T")
		         || !g_ascii_strcasecmp(str, "TI-84PT")
		         || !g_ascii_strcasecmp(str, "TI84+T")
		         || !g_ascii_strcasecmp(str, "TI84PT")
		         || !g_ascii_strcasecmp(str, "84+T USB")
		         || !g_ascii_strcasecmp(str, "84PT USB")
		         || !g_ascii_strcasecmp(str, "84+T")
		         || !g_ascii_strcasecmp(str, "84PT")
		        )
			return CALC_TI84PT_USB;
	}

	return CALC_NONE;
}
