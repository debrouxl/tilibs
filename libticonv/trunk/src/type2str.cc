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

const char * TICALL ticonv_model_to_string(CalcModel model)
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
	case CALC_NSPIRE_CRADLE: return "Nspire Cradle";
	case CALC_NSPIRE_CLICKPAD: return "Nspire Clickpad";
	case CALC_NSPIRE_CLICKPAD_CAS: return "Nspire Clickpad CAS";
	case CALC_NSPIRE_TOUCHPAD: return "Nspire Touchpad";
	case CALC_NSPIRE_TOUCHPAD_CAS: return "Nspire Touchpad CAS";
	case CALC_NSPIRE_CX: return "Nspire CX";
	case CALC_NSPIRE_CX_CAS: return "Nspire CX CAS";
	case CALC_NSPIRE_CMC: return "Nspire CM-C";
	case CALC_NSPIRE_CMC_CAS: return "Nspire CM-C CAS";
	case CALC_NSPIRE_CXII: return "Nspire CX II";
	case CALC_NSPIRE_CXII_CAS: return "Nspire CX II CAS";
	case CALC_NSPIRE_CXIIT: return "Nspire CX II-T";
	case CALC_NSPIRE_CXIIT_CAS: return "Nspire CX II-T CAS";
	default: return "unknown";
	}
}

CalcModel TICALL ticonv_string_to_model(const char *str)
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
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire")
		         || !g_ascii_strcasecmp(str, "TI Nspire")
		         || !g_ascii_strcasecmp(str, "Nspire")
		         || !g_ascii_strcasecmp(str, "Nsp")
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
		else if (   !g_ascii_strcasecmp(str, "Nspire Cradle")
		         || !g_ascii_strcasecmp(str, "NspireCradle")
		         || !g_ascii_strcasecmp(str, "NspCradle")
		        )
			return CALC_NSPIRE_CRADLE;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire Clickpad")
		         || !g_ascii_strcasecmp(str, "TI Nspire Clickpad")
		         || !g_ascii_strcasecmp(str, "Nspire Clickpad")
		         || !g_ascii_strcasecmp(str, "NspireClickpad")
		         || !g_ascii_strcasecmp(str, "Clickpad")
		        )
			return CALC_NSPIRE_CLICKPAD;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire Clickpad CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire Clickpad CAS")
		         || !g_ascii_strcasecmp(str, "Nspire Clickpad CAS")
		         || !g_ascii_strcasecmp(str, "NspireClickpadCAS")
		         || !g_ascii_strcasecmp(str, "Clickpad CAS")
		         || !g_ascii_strcasecmp(str, "ClickpadCAS")
		        )
			return CALC_NSPIRE_CLICKPAD_CAS;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire Touchpad")
		         || !g_ascii_strcasecmp(str, "TI Nspire Touchpad")
		         || !g_ascii_strcasecmp(str, "Nspire Touchpad")
		         || !g_ascii_strcasecmp(str, "NspireTouchpad")
		         || !g_ascii_strcasecmp(str, "Touchpad")
		        )
			return CALC_NSPIRE_TOUCHPAD;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire Touchpad CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire Touchpad CAS")
		         || !g_ascii_strcasecmp(str, "Nspire Touchpad CAS")
		         || !g_ascii_strcasecmp(str, "NspireTouchpadCAS")
		         || !g_ascii_strcasecmp(str, "Touchpad CAS")
		         || !g_ascii_strcasecmp(str, "TouchpadCAS")
		        )
			return CALC_NSPIRE_TOUCHPAD_CAS;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX")
		         || !g_ascii_strcasecmp(str, "Nspire CX")
		         || !g_ascii_strcasecmp(str, "NspireCX")
		         || !g_ascii_strcasecmp(str, "CX")
		        )
			return CALC_NSPIRE_CX;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CX CAS")
		         || !g_ascii_strcasecmp(str, "NspireCXCAS")
		         || !g_ascii_strcasecmp(str, "CX CAS")
		         || !g_ascii_strcasecmp(str, "CXCAS")
		        )
			return CALC_NSPIRE_CX_CAS;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CM-C")
		         || !g_ascii_strcasecmp(str, "TI Nspire CM-C")
		         || !g_ascii_strcasecmp(str, "Nspire CM-C")
		         || !g_ascii_strcasecmp(str, "NspireCMC")
		         || !g_ascii_strcasecmp(str, "CM-C")
		         || !g_ascii_strcasecmp(str, "CMC")
		        )
			return CALC_NSPIRE_CMC;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CM-C CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CM-C CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CM-C CAS")
		         || !g_ascii_strcasecmp(str, "NspireCMCCAS")
		         || !g_ascii_strcasecmp(str, "CM-C CAS")
		         || !g_ascii_strcasecmp(str, "CM-CCAS")
		         || !g_ascii_strcasecmp(str, "CMCCAS")
		        )
			return CALC_NSPIRE_CMC_CAS;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX II")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX II")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXII")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXII")
		         || !g_ascii_strcasecmp(str, "Nspire CX II")
		         || !g_ascii_strcasecmp(str, "Nspire CXII")
		         || !g_ascii_strcasecmp(str, "NspireCXII")
		         || !g_ascii_strcasecmp(str, "CX II")
		         || !g_ascii_strcasecmp(str, "CXII")
		        )
			return CALC_NSPIRE_CXII;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX II CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX II CAS")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXII CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXII CAS")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXIICAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXIICAS")
		         || !g_ascii_strcasecmp(str, "Nspire CX II CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CXII CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CXIICAS")
		         || !g_ascii_strcasecmp(str, "NspireCXIICAS")
		         || !g_ascii_strcasecmp(str, "CX II CAS")
		         || !g_ascii_strcasecmp(str, "CXII CAS")
		         || !g_ascii_strcasecmp(str, "CXIICAS")
		        )
			return CALC_NSPIRE_CXII_CAS;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX II-T")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX II-T")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXII-T")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXII-T")
		         || !g_ascii_strcasecmp(str, "Nspire CX II-T")
		         || !g_ascii_strcasecmp(str, "Nspire CXII-T")
		         || !g_ascii_strcasecmp(str, "NspireCXIIT")
		         || !g_ascii_strcasecmp(str, "CX II-T")
		         || !g_ascii_strcasecmp(str, "CXIIT")
		        )
			return CALC_NSPIRE_CXIIT;
		else if (   !g_ascii_strcasecmp(str, "TI-Nspire CX II-T CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CX II-T CAS")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXIIT CAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXIIT CAS")
		         || !g_ascii_strcasecmp(str, "TI-Nspire CXIITCAS")
		         || !g_ascii_strcasecmp(str, "TI Nspire CXIITCAS")
		         || !g_ascii_strcasecmp(str, "Nspire CX II-T CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CXIIT CAS")
		         || !g_ascii_strcasecmp(str, "Nspire CXIITCAS")
		         || !g_ascii_strcasecmp(str, "NspireCXIITCAS")
		         || !g_ascii_strcasecmp(str, "CX II-T CAS")
		         || !g_ascii_strcasecmp(str, "CXIIT CAS")
		         || !g_ascii_strcasecmp(str, "CXIITCAS")
		        )
			return CALC_NSPIRE_CXIIT_CAS;
	}

	return CALC_NONE;
}
