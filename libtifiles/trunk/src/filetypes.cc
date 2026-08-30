/* Hey EMACS -*- linux-c -*- */
/* $Id: typesxx.c 912 2005-03-30 20:49:06Z roms $ */

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

#include <ctype.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#include "gettext.h"
#include "tifiles.h"
#include "logging.h"
#include "internal.h"
#include "typesxx.h"
#include "error.h"
#include "rwfile.h"
#include "memreader.h"

// Whether to enable strict file extension checking.
#define CHECK_FILE_EXTENSIONS


/****************/
/* Global types */
/****************/

static const char GROUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"",
	"73g", "82g", "83g", "8Xg", "8Xg", "85g", "86g", // 7
	"89g", "89g", "92g", "9Xg", "V2g", // 12
	"8Xg", "89g", "",    "", // 16
	"8Xg", "8Xg", "8Xg", "8Xg", "8Xg", "8xg", // 22
	"", // 23
	"",    "",    "",    "", // 27
	"",    "",    "",    "", // 31
	"",    "",    "",    "", // 35
	"8xg", // 36
	"",    "",    "",    "",    "",    "", // 42
	"",    "",    "",    "",    "", // 47
	""
};

static const char BACKUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"",
	"73b", "82b", "83b", "8Xb", "8Xb", "85b", "86b", // 7
	"89g", "89g", "92b", "9Xg", "V2g", // 12
	"8Xg", "89g", "",    "", // 16
	"8Cb", "8Cb", "",    "",    "8Xb", "8Xb", // 22
	"", // 23
	"",    "",    "",    "", // 27
	"",    "",    "",    "", // 31
	"",    "",    "",    "", // 35
	"8Xb", // 36
	"",    "",    "",    "",    "",    "", // 42
	"",    "",    "",    "",    "", // 47
	""
};

/*******************/
/* File extensions */
/*******************/

/**
 * tifiles_fext_of_group:
 * @model: a calculator model.
 *
 * Returns file extension of a group file.
 *
 * Return value: a file extension as string (like "83g").
 **/
const char * TICALL tifiles_fext_of_group (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return nullptr;
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return "73g";
		case CALC_TI80:
			return nullptr;
		case CALC_TI82:
			return "82g";
		case CALC_TI83:
			return "83g";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84PC:
		case CALC_TI84P_USB:
		case CALC_TI84PC_USB:
		case CALC_TI83PCE_USB:
		case CALC_TI84PCE_USB:
		case CALC_TI82A_USB:
		case CALC_TI84PT_USB:
		case CALC_TI82AEP_USB:
			return "8Xg"; // There's also 8Cg.
		case CALC_TI85:
			return "85g";
		case CALC_TI86:
			return "86g";
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89g";
		case CALC_TI92:
			return "92g";
		case CALC_TI92P:
			return "9Xg";
		case CALC_V200:
			return "V2g";
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
		case CALC_TIPRESENTER:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_LABPRO_USB:
		case CALC_EASYTEMP_GOTEMP_USB:
		case CALC_EASYLINK_GOLINK_USB:
		case CALC_CBR2_GOMOTION_USB:
		case CALC_GODIRECT_USB:
			return nullptr;
		case CALC_MAX:
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return nullptr;
}

/**
 * tifiles_fext_of_backup:
 * @model: a calculator model.
 *
 * Returns file extension of a backup file.
 *
 * Return value: a file extension as string (like "83b").
 **/
const char * TICALL tifiles_fext_of_backup (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??b";
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return "73b";
		case CALC_TI80:
			return nullptr;
		case CALC_TI82:
			return "82b";
		case CALC_TI83:
			return "83b";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
		case CALC_TI82A_USB:
		case CALC_TI84PT_USB:
		case CALC_TI82AEP_USB:
			return "8Xb";
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return "8Cb";
		case CALC_TI83PCE_USB:
		case CALC_TI84PCE_USB:
			return nullptr;
		case CALC_TI85:
			return "85b";
		case CALC_TI86:
			return "86b";
#endif
#ifndef DISABLE_TI89X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89g";
		case CALC_TI92:
			return "92b";
		case CALC_TI92P:
			return "9Xg";
		case CALC_V200:
			return "V2g";
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
		case CALC_TIPRESENTER:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_LABPRO_USB:
		case CALC_EASYTEMP_GOTEMP_USB:
		case CALC_EASYLINK_GOLINK_USB:
		case CALC_CBR2_GOMOTION_USB:
		case CALC_GODIRECT_USB:
			return nullptr;
		case CALC_MAX:
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return nullptr;
}

/**
 * tifiles_fext_of_flash_app:
 * @model: a calculator model.
 *
 * Returns file extension of a FLASH application file.
 *
 * Return value: a file extension as string (like "89k").
 **/
const char * TICALL tifiles_fext_of_flash_app (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??k";
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return "73k";
		case CALC_TI80:
			return nullptr;
		case CALC_TI82:
			return nullptr;
		case CALC_TI83:
			return nullptr;
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xk";
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return "8Ck";
		case CALC_TI83PCE_USB:
			return "8Ek";
		case CALC_TI84PCE_USB:
			return "8Ek";
		case CALC_TI82A_USB:
		case CALC_TI84PT_USB:
		case CALC_TI82AEP_USB:
			return nullptr;
		case CALC_TI85:
			return nullptr;
		case CALC_TI86:
			return nullptr;
#endif
#ifndef DISABLE_TI8X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89k";
		case CALC_TI92:
			return nullptr;
		case CALC_TI92P:
			return "9Xk";
		case CALC_V200:
			return "V2k";
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
		case CALC_TIPRESENTER:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_LABPRO_USB:
		case CALC_EASYTEMP_GOTEMP_USB:
		case CALC_EASYLINK_GOLINK_USB:
		case CALC_CBR2_GOMOTION_USB:
		case CALC_GODIRECT_USB:
			return nullptr;
		case CALC_MAX:
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return nullptr;
}

/**
 * tifiles_fext_of_flash_os:
 * @model: a calculator model.
 *
 * Returns file extension of a FLASH Operating System file.
 *
 * Return value: a file extension as string (like "89u").
 **/
const char * TICALL tifiles_fext_of_flash_os(CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return nullptr;
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return "73u";
		case CALC_TI80:
			return nullptr;
		case CALC_TI82:
			return nullptr;
		case CALC_TI83:
			return nullptr;
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
		case CALC_TI84PT_USB:
			return "8Xu";
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return "8Cu";
		case CALC_TI83PCE_USB:
			return "8Pu";
		case CALC_TI84PCE_USB:
			return "8Eu";
		case CALC_TI82A_USB:
			return "82u";
		case CALC_TI82AEP_USB:
			return "8Yu";
		case CALC_TI85:
			return nullptr;
		case CALC_TI86:
			return nullptr;
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89u";
		case CALC_TI92:
			return nullptr;
		case CALC_TI92P:
			return "9Xu";
		case CALC_V200:
			return "V2u";
#endif
		case CALC_NSPIRE:
			return "tno"; // Generic extension, it depends on the sub-model...
		case CALC_NSPIRE_CRADLE:
			return "tlo"; // Actually, it might depend on the sub-model...
		case CALC_NSPIRE_CLICKPAD:
			return "tno";
		case CALC_NSPIRE_CLICKPAD_CAS:
			return "tnc";
		case CALC_NSPIRE_TOUCHPAD:
			return "tno";
		case CALC_NSPIRE_TOUCHPAD_CAS:
			return "tnc";
		case CALC_NSPIRE_CX:
			return "tco";
		case CALC_NSPIRE_CX_CAS:
			return "tcc";
		case CALC_NSPIRE_CMC:
			return "tmo";
		case CALC_NSPIRE_CMC_CAS:
			return "tmc";
		case CALC_NSPIRE_CXII:
			return "tco2";
		case CALC_NSPIRE_CXII_CAS:
			return "tcc2";
		case CALC_NSPIRE_CXIIT:
			return "tct2";
		case CALC_NSPIRE_CXIIT_CAS:
			return "tcc2";
		case CALC_TIPRESENTER:
			return "hex"; // Sbase132.hex from the Windows TI-Presenter OS upgrader.
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBR2:
			return nullptr;
		case CALC_CBL2:
		case CALC_LABPRO:
		case CALC_LABPRO_USB:
			return "c2u"; // .hex is also seen in some OS upgraders.
		case CALC_EASYTEMP_GOTEMP_USB:
		case CALC_EASYLINK_GOLINK_USB:
		case CALC_CBR2_GOMOTION_USB:
		case CALC_GODIRECT_USB:
			return nullptr;
		case CALC_MAX:
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return nullptr;
}

/**
 * tifiles_fext_of_certif:
 * @model: a calculator model.
 *
 * Returns file extension of certificate file.
 *
 * Return value: a file extension as string (like "89q").
 **/
const char * TICALL tifiles_fext_of_certif(CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??q";
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return "73q";
		case CALC_TI80:
			return nullptr;
		case CALC_TI82:
			return nullptr;
		case CALC_TI83:
			return nullptr;
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xq";
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return "8Cq";
		case CALC_TI83PCE_USB:
			return "8Pq";
		case CALC_TI84PCE_USB:
			return "8Eq";
		case CALC_TI82A_USB:
		case CALC_TI84PT_USB:
		case CALC_TI82AEP_USB:
			return nullptr;
		case CALC_TI85:
			return nullptr;
		case CALC_TI86:
			return nullptr;
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89q";
		case CALC_TI92:
			return nullptr;
		case CALC_TI92P:
			return "9Xq";
		case CALC_V200:
			return "V2q";
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
		case CALC_TIPRESENTER:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_LABPRO_USB:
		case CALC_EASYTEMP_GOTEMP_USB:
		case CALC_EASYLINK_GOLINK_USB:
		case CALC_CBR2_GOMOTION_USB:
		case CALC_GODIRECT_USB:
			return nullptr;
		case CALC_MAX:
		default:
			tifiles_critical("%s: invalid calc_type argument", __FUNCTION__);
			break;
	}

	return nullptr;
}

/**
 * tifiles_fext_get:
 * @filename: a filename as string.
 *
 * Returns file extension part.
 *
 * Return value: a file extension without dot as string (like "89g").
 **/
const char * TICALL tifiles_fext_get(const char *filename)
{
	if (filename != nullptr)
	{
		const char * d = strrchr(filename, '.');
		if (d != nullptr)
		{
			return (++d);
		}
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return "";
}

/**
 * tifiles_fext_dup:
 * @filename: a filename as string.
 *
 * Returns a copy of file extension part.
 *
 * Return value: a file extension without dot as string (like "89g").
 * Needs to be freed with tifiles_fext_free() when no longer needed.
 **/
char * TICALL tifiles_fext_dup(const char *filename)
{
	return g_strdup(tifiles_fext_get(filename));
}

/**
 * tifiles_fext_free:
 * @filename: a filename as string.
 *
 * Frees a file extension part previously allocated with tifiles_fext_dup().
 **/
void TICALL tifiles_fext_free(char *filename)
{
	g_free(filename);
}

/**********************/
/* Signature macros consumed by the buffer-based core helpers below. */
#define TIB_SIGNATURE	"Advanced Mathematics Software"
#define TIG_SIGNATURE	"PK\x03\x04"	// 0x04034b50
#define TIG_SIGNATURE2	"PK\x05\x06"	// 0x06054b50
#define OS_SIGNATURE_PREFIX            "TI-Nspire."
#define TNO_SIGNATURE_SUFFIX           "tno "
#define TNO_NOSAMPLES_SIGNATURE_SUFFIX "nosamples.tno "
#define TNC_SIGNATURE_SUFFIX           "tnc "
#define TCO_SIGNATURE_SUFFIX           "tco "
#define TCC_SIGNATURE_SUFFIX           "tcc "
#define TMO_SIGNATURE_SUFFIX           "tmo "
#define TMC_SIGNATURE_SUFFIX           "tmc "
#define TLO_SIGNATURE_SUFFIX           "tlo "
#define TLD_SIGNATURE_SUFFIX           "tld "
#define TCX2_SIGNATURE_PREFIX          "TI-Nspire.tc"
#define TCO2_SIGNATURE                 "TI-Nspire.tco2 "
#define TCC2_SIGNATURE                 "TI-Nspire.tcc2 "
#define TCT2_SIGNATURE                 "TI-Nspire.tct2 "
#define OSEXT1_SIGNATURE               "__OSEXT__1 "

static int buffer_has_ti_header(TiMemReader *r)
{
	char magic[9];
	size_t i;

	if (r->size - r->pos < 8)
	{
		return 0;
	}

	for (i = 0; i < 8; i++)
	{
		magic[i] = (char)toupper((unsigned char)r->data[r->pos + i]);
	}
	magic[8] = '\0';

	if (   !strcmp(magic, "**TI73**") || !strcmp(magic, "**TI82**")
	    || !strcmp(magic, "**TI83**") || !strcmp(magic, "**TI83F*")
	    || !strcmp(magic, "**TI85**") || !strcmp(magic, "**TI86**")
	    || !strcmp(magic, "**TI89**") || !strcmp(magic, "**TI92**")
	    || !strcmp(magic, "**TI92P*") || !strcmp(magic, "**V200**")
	    || !strcmp(magic, "**TIFL**") || !strcmp(magic, "**TICBL*")
	    || !strncmp(magic, "*TI", 3))
	{
		return !0;
	}

	return 0;
}

static int buffer_has_tib_header(TiMemReader *r)
{
	size_t need = 22 + sizeof(TIB_SIGNATURE) + 1;

	if (r->size - r->pos < need)
	{
		return 0;
	}

	if (!memcmp(r->data + r->pos + 22, TIB_SIGNATURE, sizeof(TIB_SIGNATURE) - 1))
	{
		return !0;
	}

	return 0;
}

static int buffer_has_tig_header(TiMemReader *r)
{
	size_t need = sizeof(TIG_SIGNATURE) - 1;

	if (r->size - r->pos < need)
	{
		return 0;
	}

	if (   !memcmp(r->data + r->pos, TIG_SIGNATURE, sizeof(TIG_SIGNATURE) - 1)
	    || !memcmp(r->data + r->pos, TIG_SIGNATURE2, sizeof(TIG_SIGNATURE2) - 1))
	{
		return !0;
	}

	return 0;
}

static int buffer_has_tifl_header(TiMemReader *r, uint8_t *dev_type, uint8_t *data_type)
{
	uint8_t buf[78];
	size_t  saved = r->pos;
	uint32_t len;
	int     ret = 0;

	if (r->size < 78)
	{
		r->pos = saved;
		return 0;
	}

	for (;;)
	{
		size_t got = tifiles_mem_reader_read(r, buf, 78);
		if (got != 78)
		{
			break;
		}
		if (strncmp((char *)buf, "**TIFL**", 8) != 0)
		{
			break;
		}

		ret = 1;

		/* if there are multiple entries (old OS files with license
		   agreement, old app files with certificate), return the type
		   of the last entry */
		if (dev_type != nullptr)
		{
			*dev_type = buf[48];
		}
		if (data_type != nullptr)
		{
			*data_type = buf[49];
		}

		len = (uint32_t)buf[74] | ((uint32_t)buf[75] << 8) | ((uint32_t)buf[76] << 16) | ((uint32_t)buf[77] << 24);
		/* Bounds-checked skip, replacing the previous unbounded fseek(). */
		if (len > (r->size - r->pos))
		{
			break;
		}
		r->pos += len;
	}

	r->pos = saved;
	return ret;
}

static int buffer_has_tno_header(TiMemReader *r)
{
	const unsigned char *p;

	if (r->size - r->pos < 1024)
	{
		return 0;
	}

	p = r->data + r->pos;

	if (!memcmp(p, OSEXT1_SIGNATURE, sizeof(OSEXT1_SIGNATURE) - 1))
	{
		return !0;
	}
	else if (!memcmp(p, OS_SIGNATURE_PREFIX, sizeof(OS_SIGNATURE_PREFIX) - 1))
	{
		if (   !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TNO_SIGNATURE_SUFFIX, sizeof(TNO_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TNC_SIGNATURE_SUFFIX, sizeof(TNC_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TNO_NOSAMPLES_SIGNATURE_SUFFIX, sizeof(TNO_NOSAMPLES_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TCO_SIGNATURE_SUFFIX, sizeof(TCO_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TCC_SIGNATURE_SUFFIX, sizeof(TCC_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TMO_SIGNATURE_SUFFIX, sizeof(TMO_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TMC_SIGNATURE_SUFFIX, sizeof(TMC_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TLO_SIGNATURE_SUFFIX, sizeof(TLO_SIGNATURE_SUFFIX) - 1)
		    || !memcmp(p + sizeof(OS_SIGNATURE_PREFIX) - 1, TLD_SIGNATURE_SUFFIX, sizeof(TLD_SIGNATURE_SUFFIX) - 1))
		{
			return !0;
		}
	}
	else
	{
		/* Look for a CX II signature, which isn't at the beginning of the
		   file anymore. Sadly, memmem() is not portable. */
		size_t remaining = 1024;
		const unsigned char *ptr2 = p;
		const unsigned char *ptr1 = (const unsigned char *)memchr(ptr2, 'T', remaining);

		while (ptr1 != nullptr)
		{
			if (   ((size_t)(ptr1 - p) + sizeof(TCX2_SIGNATURE_PREFIX) - 1 + 3) <= 1024
			    && !memcmp(ptr1, TCX2_SIGNATURE_PREFIX, sizeof(TCX2_SIGNATURE_PREFIX) - 1))
			{
				unsigned char c = *(ptr1 + sizeof(TCX2_SIGNATURE_PREFIX) - 1);
				if (   (c == 'o' || c == 'c' || c == 't')
				    && *(ptr1 + sizeof(TCX2_SIGNATURE_PREFIX)) == '2'
				    && *(ptr1 + sizeof(TCX2_SIGNATURE_PREFIX) + 1) == ' ')
				{
					return !0;
				}
			}
			remaining -= (size_t)(ptr1 - ptr2 + 1);
			ptr2 = ptr1 + 1;
			ptr1 = (const unsigned char *)memchr(ptr2, 'T', remaining);
		}
	}

	return 0;
}

static int buffer_is_ti(TiMemReader *r)
{
	if (   buffer_has_ti_header(r)
	    || buffer_has_tib_header(r)
	    || buffer_has_tig_header(r)
	    || buffer_has_tifl_header(r, nullptr, nullptr)
	    || buffer_has_tno_header(r))
	{
		return !0;
	}

	return 0;
}

static int buffer_is_flash(TiMemReader *r)
{
	return (   buffer_has_tib_header(r)
	        || buffer_has_tno_header(r)
	        || buffer_has_tifl_header(r, nullptr, nullptr));
}

static int buffer_is_os(TiMemReader *r)
{
	uint8_t type;

	if (!buffer_is_ti(r))
	{
		return 0;
	}
	if (   buffer_has_tib_header(r)
	    || buffer_has_tno_header(r)
	    || (buffer_has_tifl_header(r, nullptr, &type) && type == TI83p_AMS))
	{
		return !0;
	}

	return 0;
}

static int buffer_is_app(TiMemReader *r)
{
	uint8_t type;

	if (!buffer_is_ti(r))
	{
		return 0;
	}
	if (buffer_has_tifl_header(r, nullptr, &type) && type == TI83p_APPL)
	{
		return !0;
	}

	return 0;
}

static int buffer_test(TiMemReader *r, FileClass type, CalcModel target)
{
	uint8_t ctype = 0;
	uint8_t dtype = 0;

	if (target >= CALC_MAX)
	{
		return 0;
	}
	if (!buffer_is_ti(r))
	{
		return 0;
	}

	if (type & TIFILE_SINGLE)
	{
		/* Content-based best effort: a non-flash, non-tigroup TI file is a
		   single-variable candidate. Group/backup separation needs the file
		   extension, so we do not claim those classes here. */
		if (   !buffer_is_flash(r)
		    && !buffer_has_tig_header(r))
		{
			return !0;
		}
		return 0;
	}

	if (type & TIFILE_GROUP)
	{
		/* Reliable group detection from a bare buffer requires parsing the
		   variable-record count; not implemented (documented limitation). */
		return 0;
	}

	if (type & TIFILE_REGULAR)
	{
		return (buffer_test(r, TIFILE_SINGLE, target) || buffer_test(r, TIFILE_GROUP, target));
	}

	if (type & TIFILE_BACKUP)
	{
		/* Backup vs single is not content-distinguishable for most families
		   without the file extension (documented limitation). */
		return 0;
	}

	if (type & TIFILE_OS)
	{
		if (target && buffer_has_tifl_header(r, &ctype, &dtype))
		{
			return (ctype == tifiles_model_to_dev_type(target) && dtype == TI83p_AMS);
		}
		else if (target && buffer_has_tib_header(r))
		{
			uint8_t data[16];
			size_t  saved = r->pos;

			r->pos = 0;
			if (tifiles_mem_reader_read(r, data, 16) == 16)
			{
				r->pos = saved;
				switch (data[8])
				{
					case 1:  return (target == CALC_TI92P);
					case 3:  return (target == CALC_TI89);
					case 8:  return (target == CALC_V200);
					case 9:  return (target == CALC_TI89T);
					default: return 0;
				}
			}
			r->pos = saved;
			return 0;
		}
		else
		{
			return buffer_is_os(r);
		}
	}

	if (type & TIFILE_APP)
	{
		if (target && buffer_has_tifl_header(r, &ctype, &dtype))
		{
			return (ctype == tifiles_model_to_dev_type(target) && dtype == TI83p_APPL);
		}
		else
		{
			return buffer_is_app(r);
		}
	}

	if (type & TIFILE_FLASH)
	{
		return (buffer_test(r, TIFILE_OS, target) || buffer_test(r, TIFILE_APP, target));
	}

	if (type & TIFILE_TIGROUP)
	{
		/* Per-entry model compatibility needs the filename-based tigroup
		   reader; only the ZIP signature is checked here (documented
		   limitation). */
		return buffer_has_tig_header(r);
	}

	return 0;
}

/* Signature checking */
/**********************/

/**
 * tifiles_file_has_ti_header:
 * @filename: a filename as string.
 *
 * Check whether file has a TI magic number in the header.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_has_ti_header(const char *filename)
{
	TiMemReader r;
	int ret = 0;

	if (filename != nullptr && tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) == 0)
	{
		ret = buffer_has_ti_header(&r);
		tifiles_mem_reader_destroy(&r);
	}

	return ret;
}


/**
 * tifiles_file_has_tib_header:
 * @filename: a filename as string.
 *
 * Check whether file has a TIB magic number in the header.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_has_tib_header(const char *filename)
{
	TiMemReader r;
	int ret = 0;

	if (filename != nullptr)
	{
#ifdef CHECK_FILE_EXTENSIONS
		char *e = tifiles_fext_get(filename);

		if (   e[0] == 0
		    || g_ascii_strcasecmp(e, "tib"))
		{
			return 0;
		}
#endif

		if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) == 0)
		{
			ret = buffer_has_tib_header(&r);
			tifiles_mem_reader_destroy(&r);
		}
	}

	return ret;
}


/**
 * tifiles_file_has_tig_header:
 * @filename: a filename as string.
 *
 * Check whether file has a ZIP file header.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_has_tig_header(const char *filename)
{
	TiMemReader r;
	int ret = 0;

	if (filename != nullptr)
	{
#ifdef CHECK_FILE_EXTENSIONS
		char *e = tifiles_fext_get(filename);

		if (   e[0] == 0
		    || g_ascii_strcasecmp(e, "tig"))
		{
			return 0;
		}
#endif

		if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) == 0)
		{
			ret = buffer_has_tig_header(&r);
			tifiles_mem_reader_destroy(&r);
		}
	}

	return ret;
}

/* Read one page of the file; if it begins with a TIFL header, re-read up to
   TIFILES_FLASH_READ_CAP (flash OS/app for TI-Z80/68k/eZ80 are at most 4 MB of
   Flash). Non-flash files, including large Nspire OS files that carry a
   different magic and are classified from the page via buffer_has_tno_header(),
   stay at one page and never blow up memory. Returns 0 on success, -1 on
   failure (in which case *reader is untouched). */
static int open_for_tifl(TiMemReader *reader, const char *filename)
{
	TiMemReader r;

	if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) != 0)
	{
		return -1;
	}
	if (r.size >= 8 && memcmp(r.data, "**TIFL**", 8) == 0)
	{
		tifiles_mem_reader_destroy(&r);
		return tifiles_mem_reader_create_from_file(reader, filename, TIFILES_FLASH_READ_CAP);
	}
	*reader = r;
	return 0;
}

/**
 * tifiles_file_has_tifl_header:
 * @filename: a filename as string.
 *
 * Check whether file has a TI Flash file magic number in the header, and
 * fill device type and data type for the last entry in the file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_has_tifl_header(const char *filename, uint8_t *dev_type, uint8_t *data_type)
{
	TiMemReader r;
	int ret = 0;

	if (filename != nullptr && open_for_tifl(&r, filename) == 0)
	{
		ret = buffer_has_tifl_header(&r, dev_type, data_type);
		tifiles_mem_reader_destroy(&r);
	}

	return ret;
}


static int check_for_Nspire_OS_file_extension(const char * ext)
{
	if (nullptr != ext)
	{
		if (ext[0] == 't' || ext[0] == 'T')
		{
			char c = ext[1];
			if (c == 'n' || c == 'N')
			{
co:
				c = ext[2];
				if (c == 'o' || c == 'O' || c == 'c' || c == 'C')
				{
					return 1;
				}
			}
			else if (c == 'c' || c == 'C')
			{
				c = ext[2];
				if (c == 'o' || c == 'O' || c == 'c' || c == 'C')
				{
					return ext[3] == 0 || ext[3] == '2';
				}
				else if (c == 't' || c == 'T')
				{
					return ext[3] == '2';
				}
			}
			else if (c == 'm' || c == 'M')
			{
				goto co;
			}
			else if (c == 'l' || c == 'L')
			{
				return ext[2] == 'o' || ext[2] == 'O' || ext[2] == 'd' || ext[2] == 'D';
			}
		}
	}

	return 0;
}

/**
 * tifiles_file_has_tno_header:
 * @filename: a filename as string.
 *
 * Check whether file has a Nspire OS / OS extension file header.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_has_tno_header(const char *filename)
{
	TiMemReader r;
	int ret = 0;

	if (filename != nullptr)
	{
#ifdef CHECK_FILE_EXTENSIONS
		const char *e = tifiles_fext_get(filename);

		if (!check_for_Nspire_OS_file_extension(e))
		{
			return 0;
		}
#endif

		if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) == 0)
		{
			ret = buffer_has_tno_header(&r);
			tifiles_mem_reader_destroy(&r);
		}
	}

	return ret;
}

/**
 * tifiles_model_to_dev_type:
 * @model: a calculator model
 *
 * Converts the calculator model to FlashApp DeviceType.
 *
 * Return value: FlashApp DeviceType if that calculator model supports FlashApps, -1 otherwise.
 **/
int TICALL tifiles_model_to_dev_type(CalcModel model)
{
	switch (model) {
#ifndef DISABLE_TI8X
	case CALC_TI73:
		return DEVICE_TYPE_73;

	case CALC_TI83P:
	case CALC_TI84P:
	case CALC_TI84P_USB:
	case CALC_TI84PC:
	case CALC_TI84PC_USB:
	case CALC_TI83PCE_USB:
	case CALC_TI84PCE_USB:
	case CALC_TI82A_USB:
	case CALC_TI84PT_USB:
	case CALC_TI82AEP_USB:
		return DEVICE_TYPE_83P;
#endif
#ifndef DISABLE_TI9X
	case CALC_TI89:
	case CALC_TI89T:
	case CALC_TI89T_USB:
		return DEVICE_TYPE_89;

	case CALC_TI92P:
	case CALC_V200:
		return DEVICE_TYPE_92P;
#endif
	case CALC_TIPRESENTER:
		return DEVICE_TYPE_TIPRESENTER;
	case CALC_CBL2:
	case CALC_LABPRO:
	case CALC_LABPRO_USB: // The LabPro's USB side accepts the same OS upgrades as its legacy I/O side.
		return DEVICE_TYPE_CBL2;
	case CALC_NONE:
	case CALC_TI82:
	case CALC_TI83:
	case CALC_TI85:
	case CALC_TI86:
	case CALC_TI92:
	case CALC_NSPIRE:
	case CALC_TI80:
	case CALC_NSPIRE_CRADLE:
	case CALC_NSPIRE_CLICKPAD:
	case CALC_NSPIRE_CLICKPAD_CAS:
	case CALC_NSPIRE_TOUCHPAD:
	case CALC_NSPIRE_TOUCHPAD_CAS:
	case CALC_NSPIRE_CX:
	case CALC_NSPIRE_CX_CAS:
	case CALC_NSPIRE_CMC:
	case CALC_NSPIRE_CMC_CAS:
	case CALC_NSPIRE_CXII:
	case CALC_NSPIRE_CXII_CAS:
	case CALC_NSPIRE_CXIIT:
	case CALC_NSPIRE_CXIIT_CAS:
	case CALC_CBL:
	case CALC_CBR:
	case CALC_CBR2:
	case CALC_MAX:
	default:
		return -1;
	}
}


/**************/
/* File types */
/**************/

#ifndef __WIN32__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static int is_regfile(const char *filename)
{
#ifndef __WIN32__
	struct stat buf;

	if (stat(filename, &buf) < 0)
	{
		return 0;
	}

	if (S_ISREG(buf.st_mode))
	{
		return !0;
	}
	else
	{
		return 0;
	}
#else
	return !0;
#endif
}

/**
 * tifiles_file_is_ti:
 * @filename: a filename as string.
 *
 * Check whether file is a TI file by checking the signature.
 *
 * Return value: a boolean value.
 **/
/*****************************/
/* Memory-based core helpers */
/*****************************/

/* All helpers operate on a TiMemReader and restore its position on exit, so
   they can be composed freely (e.g. tifiles_buffer_is_ti tries every header
   check in turn). None of them performs any file-extension check. */


/******************************/
/* Public memory-based variants */
/******************************/

int TICALL tifiles_buffer_has_ti_header(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_has_ti_header(&r);
}

int TICALL tifiles_buffer_has_tib_header(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_has_tib_header(&r);
}

int TICALL tifiles_buffer_has_tig_header(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_has_tig_header(&r);
}

int TICALL tifiles_buffer_has_tifl_header(const uint8_t *data, size_t size, uint8_t *dev_type, uint8_t *data_type)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_has_tifl_header(&r, dev_type, data_type);
}

int TICALL tifiles_buffer_has_tno_header(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_has_tno_header(&r);
}

int TICALL tifiles_buffer_is_ti(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_is_ti(&r);
}

int TICALL tifiles_buffer_is_os(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_is_os(&r);
}

int TICALL tifiles_buffer_is_app(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_is_app(&r);
}

int TICALL tifiles_buffer_is_flash(const uint8_t *data, size_t size)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_is_flash(&r);
}

int TICALL tifiles_buffer_test(const uint8_t *data, size_t size, FileClass type, CalcModel target)
{
	TiMemReader r;

	if (data == nullptr || size == 0)
	{
		return 0;
	}
	tifiles_mem_reader_init(&r, data, size);
	return buffer_test(&r, type, target);
}

int TICALL tifiles_file_is_ti(const char *filename)
{
	TiMemReader r;

	if (filename != nullptr)
	{
		// bug: check that file is not a FIFO
		if (!is_regfile(filename))
		{
			return 0;
		}

		if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) != 0)
		{
			return 0;
		}
		if (buffer_is_ti(&r))
		{
			tifiles_mem_reader_destroy(&r);
			return !0;
		}
		tifiles_mem_reader_destroy(&r);

		{
			char *e = tifiles_fext_get(filename);

#ifdef CHECK_FILE_EXTENSIONS
			if (e[0] == 0)
			{
				return 0;
			}
#endif

			if (!g_ascii_strcasecmp(e, "tns"))
			{
				return !0;
			}
		}
	}
	else
	{
		tifiles_critical("%s(nullptr)", __FUNCTION__);
	}

	return 0;
}

/**
 * tifiles_file_is_single:
 * @filename: a filename as string.
 *
 * Check whether file is a single TI file (like program, function, ...).
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_single(const char *filename)
{
	TiMemReader r;
	int ret;

	if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) != 0)
	{
		return 0;
	}

	ret = buffer_is_ti(&r)
	    && !tifiles_file_is_group(filename)
	    && !tifiles_file_is_backup(filename)
	    && !buffer_is_flash(&r)
	    && !buffer_has_tig_header(&r);
	tifiles_mem_reader_destroy(&r);

	return ret;
}

/**
 * tifiles_file_is_group:
 * @filename: a filename as string.
 *
 * Check whether file is a group file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_group(const char *filename)
{
	TiMemReader r;
	int i;
	const char *e = tifiles_fext_get(filename);

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif
	if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) != 0)
	{
		return 0;
	}
	if (buffer_is_ti(&r))
	{
		for (i = 1; i < CALC_MAX; i++)
		{
			if (GROUP_FILE_EXT[i][0] != 0 && !g_ascii_strcasecmp(e, GROUP_FILE_EXT[i]))
			{
				tifiles_mem_reader_destroy(&r);
				return !0;
			}
		}
	}

	tifiles_mem_reader_destroy(&r);
	return 0;
}

/**
 * tifiles_file_is_regular:
 * @filename: a filename as string.
 *
 * Check whether file is a single or group file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_regular(const char *filename)
{
	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	return (tifiles_file_is_single(filename) || tifiles_file_is_group(filename));
}

/**
 * tifiles_file_is_backup:
 * @filename: a filename as string.
 *
 * Check whether file is a backup file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_backup(const char *filename)
{
	TiMemReader r;
	int i;
	const char *e = tifiles_fext_get(filename);

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif
	if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) != 0)
	{
		return 0;
	}
	if (buffer_is_ti(&r))
	{
		for (i = 1; i < CALC_MAX; i++)
		{
			if (BACKUP_FILE_EXT[i][0] != 0 && !g_ascii_strcasecmp(e, BACKUP_FILE_EXT[i]))
			{
				tifiles_mem_reader_destroy(&r);
				return !0;
			}
		}
	}

	tifiles_mem_reader_destroy(&r);
	return 0;
}

/**
 * tifiles_file_is_os:
 * @filename: a filename as string.
 *
 * Check whether file is a FLASH OS file (tib or XXu)
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_os(const char *filename)
{
	TiMemReader r;
	uint8_t type;
	int ret = 0;

	if (!open_for_tifl(&r, filename))
	{
		if (buffer_is_ti(&r))
		{
			ret = buffer_has_tib_header(&r)
			    || buffer_has_tno_header(&r)
			    || (buffer_has_tifl_header(&r, nullptr, &type) && type == TI83p_AMS);
		}
		tifiles_mem_reader_destroy(&r);
	}

	return ret;
}

/**
 * tifiles_file_is_app:
 * @filename: a filename as string.
 *
 * Check whether file is a FLASH app file
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_app(const char *filename)
{
	TiMemReader r;
	uint8_t type;
	int ret = 0;

	if (!open_for_tifl(&r, filename))
	{
		if (buffer_is_ti(&r))
		{
			ret = buffer_has_tifl_header(&r, nullptr, &type) && type == TI83p_APPL;
		}
		tifiles_mem_reader_destroy(&r);
	}

	return ret;
}

/**
 * tifiles_file_is_flash:
 * @filename: a filename as string.
 *
 * Check whether file is a FLASH file (os or app).
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_flash(const char *filename)
{
	TiMemReader r;
	int ret = 0;

	if (tifiles_mem_reader_create_from_file(&r, filename, TIFILES_HEAD_READ_CAP) == 0)
	{
		ret = buffer_has_tib_header(&r) || buffer_has_tno_header(&r) || buffer_has_tifl_header(&r, nullptr, nullptr);
		tifiles_mem_reader_destroy(&r);
	}

	return ret;
}

/**
 * tifiles_file_is_tib:
 * @filename: a filename as string.
 *
 * Check whether file is a TIB formatted file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_tib(const char *filename)
{
	return tifiles_file_has_tib_header(filename);
}

/**
 * tifiles_file_is_tigroup:
 * @filename: a filename as string.
 *
 * Check whether file is a TiGroup formatted file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_tigroup(const char *filename)
{
	return tifiles_file_has_tig_header(filename);
}

/**
 * tifiles_file_is_tno:
 * @filename: a filename as string.
 *
 * Check whether file is a TNO formatted file.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_is_tno(const char *filename)
{
	return tifiles_file_has_tno_header(filename);
}

/**
 * tifiles_file_test:
 * @filename: a filename as string.
 * @type: type to check
 * @target: hand-held model or CALC_NONE for no filtering
 *
 * Check whether #filename is a TI file of type #type useable on a #target model.
 * This function is a generic one which overwrap and extends the tifiles_file_is_*
 * functions.
 *
 * This is a powerful function which allows checking of a specific file type for
 * a given target.
 *
 * Return value: a boolean value.
 **/
int TICALL tifiles_file_test(const char *filename, FileClass type, CalcModel target)
{
	const char *e = tifiles_fext_get(filename);
	uint8_t ctype, dtype;

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif

	if (target >= CALC_MAX)
	{
		tifiles_critical("%s: invalid target argument", __FUNCTION__);
		return 0;
	}

	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	if (type & TIFILE_SINGLE)
	{
		if (GROUP_FILE_EXT[target][0] != 0 && !g_ascii_strncasecmp(e, GROUP_FILE_EXT[target], 2))
		{
			return !0;
		}
		else
		{
			return tifiles_file_is_single(filename);
		}
	}

	if (type & TIFILE_GROUP)
	{
		if (GROUP_FILE_EXT[target][0] != 0 && !g_ascii_strcasecmp(e, GROUP_FILE_EXT[target]))
		{
			return !0;
		}
		else
		{
			return tifiles_file_is_group(filename);
		}
	}

	if (type & TIFILE_REGULAR)
	{
		return tifiles_file_test(filename, TIFILE_SINGLE, target) || tifiles_file_test(filename, TIFILE_GROUP, target);
	}

	if (type & TIFILE_BACKUP)
	{
		if (BACKUP_FILE_EXT[target][0] != 0 && !g_ascii_strcasecmp(e, BACKUP_FILE_EXT[target]))
		{
			return !0;
		}
		else
		{
			return tifiles_file_is_backup(filename);
		}
	}

	if (type & TIFILE_OS)
	{
		if (target && tifiles_file_has_tifl_header(filename, &ctype, &dtype))
		{
			return (ctype == tifiles_model_to_dev_type(target) && dtype == TI83p_AMS);
		}
		else if (target && tifiles_file_is_tib(filename))
		{
			uint8_t data[16];

			FILE* f = g_fopen(filename, "rb");
			if (f == nullptr)
			{
				return 0;
			}

			fread_n_chars(f, 16, (char *)data);
			fclose(f);

			switch(data[8])
			{
				case 1:
				{
					if (target != CALC_TI92P)
					{
						return 0;
					}
					break;
				}
				case 3:
				{
					if (target != CALC_TI89)
					{
						return 0;
					}
					break;
				}
				case 8:
				{
					if (target != CALC_V200)
					{
						return 0;
					}
					break;
				}
				case 9:
				{
					if (target != CALC_TI89T)
					{
						return 0;
					}
					break;
				}
				default:
					return 0;
			}

			return !0;
		}
		else
		{
			return tifiles_file_is_os(filename);
		}
	}

	if (type & TIFILE_APP)
	{
		if(target && tifiles_file_has_tifl_header(filename, &ctype, &dtype))
		{
			return (ctype == tifiles_model_to_dev_type(target) && dtype == TI83p_APPL);
		}
		else
		{
			return tifiles_file_is_app(filename);
		}
	}

	if (type & TIFILE_FLASH)
	{
		return tifiles_file_test(filename, TIFILE_OS, target) || tifiles_file_test(filename, TIFILE_APP, target);
	}

	if (type & TIFILE_TIGROUP)
	{
		if (target)
		{
			// No easy/light way for this part: we have to load the whole file
			// and to parse the TigEntry structures.
			int ok=0;
			unsigned int k;

			if (!tifiles_file_has_tig_header(filename))
			{
				return 0;
			}

			TigContent* content = tifiles_content_create_tigroup(CALC_NONE, 0);
			const int ret = tifiles_file_read_tigroup(filename, content);
			if (ret)
			{
				tifiles_content_delete_tigroup(content);
				return 0;
			}

			for (k = 0; k < content->n_apps; k++)
			{
				const TigEntry *te = content->app_entries[k];

				if(tifiles_calc_are_compat(te->content.regular->model, target))
				{
					ok++;
				}
			}

			for (k = 0; k < content->n_vars; k++)
			{
				const TigEntry *te = content->var_entries[k];

				if(tifiles_calc_are_compat(te->content.regular->model, target))
				{
					ok++;
				}
			}

			tifiles_content_delete_tigroup(content);
			return ok;
		}
		else
		{
			return tifiles_file_is_tigroup(filename);
		}
	}

	return 0;
}

/********/
/* Misc */
/********/

/**
 * tifiles_fext_to_model:
 * @filename: a file extension.
 *
 * Returns the calculator model corresponding best to this file extension.
 *
 * Return value: a model taken in #CalcModel.
 **/
CalcModel TICALL tifiles_fext_to_model(const char *ext)
{
	CalcModel type = CALC_NONE;

	if (ext == nullptr)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return CALC_NONE;
	}

	if (ext[0] != 0 && ext[1] != 0 && ext[2] != 0)
	{
		const char c1 = g_ascii_tolower(ext[0]);
		const char c2 = g_ascii_tolower(ext[1]);
		const char c3 = g_ascii_tolower(ext[2]);

#ifndef DISABLE_TI8X
		if (c1 == '7' && c2 == '3')
		{
			type = CALC_TI73;
		}
		else
#endif
		     if (c1 == '8')
		{
#ifndef DISABLE_TI8X
			if (c2 == '2')
			{
				if (c3 == 'u')
				{
					type = CALC_TI82A_USB;
				}
				else
				{
					type = CALC_TI82;
				}
			}
			else if (c2 == '3')
			{
				type = CALC_TI83;
			}
			else if (c2 == 'x')
			{
				type = CALC_TI83P;
			}
			else if (c2 == 'c')
			{
				type = CALC_TI84PC;
			}
			else if (c2 == 'p')
			{
				type = CALC_TI83PCE_USB;
			}
			else if (c2 == 'e')
			{
				type = CALC_TI84PCE_USB;
			}
			else if (c2 == 'y')
			{
				type = CALC_TI82AEP_USB;
			}
			else if (c2 == '5')
			{
				type = CALC_TI85;
			}
			else if (c2 == '6')
			{
				type = CALC_TI86;
			}
#endif
#ifndef DISABLE_TI9X
			else if (c2 == '9')
			{
				type = CALC_TI89;
			}
#endif
			// else fall through.
		}
#ifndef DISABLE_TI9X
		else if (c1 == '9')
		{
			if (c2 == '2')
			{
				type = CALC_TI92;
			}
			else if (c2 == 'x')
			{
				type = CALC_TI92P;
			}
			// else fall through.
		}
		else if (c1 == 'v' && c2 == '2')
		{
			type = CALC_V200;
		}
#endif
		//else if (!g_ascii_strcasecmp(str, "tib"))
			//type = CALC_TI89;	// consider .tib as TI89
#ifndef DISABLE_NSPIRE
		else if (c1 == 't')
		{
			if (c2 == 'n' && c3 == 's')
			{
				type = CALC_NSPIRE;
			}
			else if (c2 == 'n' && c3 == 'c')
			{
				type = CALC_NSPIRE_TOUCHPAD_CAS;
			}
			else if (c2 == 'n' && c3 == 'o')
			{
				type = CALC_NSPIRE_TOUCHPAD;
			}
			else if (c2 == 'c' && c3 == 'c')
			{
				const char c4 = ext[3];
				if (c4 == 0)
				{
					type = CALC_NSPIRE_CX_CAS;
				}
				else if (c4 == '2')
				{
					type = CALC_NSPIRE_CXII_CAS; // CALC_NSPIRE_CXII_CAS and CALC_NSPIRE_CXIIT_CAS use the same OS.
				}
				// else fall through.
			}
			else if (c2 == 'c' && c3 == 'o')
			{
				const char c4 = ext[3];
				if (c4 == 0)
				{
					type = CALC_NSPIRE_CX;
				}
				else if (c4 == '2')
				{
					type = CALC_NSPIRE_CXII;
				}
				// else fall through.
			}
			else if (c2 == 'c' && c3 == 't')
			{
				const char c4 = ext[3];
				if (c4 == '2')
				{
					type = CALC_NSPIRE_CXIIT;
				}
				// else fall through.
			}
			else if (c2 == 'm' && c3 == 'c')
			{
				type = CALC_NSPIRE_CMC_CAS;
			}
			else if (c2 == 'm' && c3 == 'o')
			{
				type = CALC_NSPIRE_CMC;
			}
			else if (c2 == 'l' && c3 == 'o')
			{
				type = CALC_NSPIRE_CRADLE;
			}
			else if (c2 == 'l' && c3 == 'd')
			{
				type = CALC_NSPIRE_CRADLE;
			}
			// else fall through.
		}
#endif
		else if (c1 == 'h' && c2 == 'e' && c3 == 'x')
		{
			// Might be a file suitable for the TI-Presenter, but some versions of the CBL2 OS upgraders contain hex files as well.
			type = CALC_TIPRESENTER;
		}
		else if (c1 == 'c' && c2 == '2' && c3 == 'u')
		{
			// Or CALC_LABPRO, but we can't distinguish them at a file extension level, and the same OS upgrades work on both models.
			type = CALC_CBL2;
		}
		// else fall through.
	}

	return type;
}

/* Note: a better way should be to open the file and read the signature */
/**
 * tifiles_file_get_model:
 * @filename: a filename as string.
 *
 * Returns the calculator model targeted by this file.
 *
 * Return value: a model taken in #CalcModel.
 **/
CalcModel TICALL tifiles_file_get_model(const char *filename)
{
	const char *e = tifiles_fext_get(filename);
	return tifiles_fext_to_model(e);
}

/**
 * tifiles_file_get_class:
 * @filename: a filename as string.
 *
 * Returns the file class (single, group, backup, flash, tigroup).
 *
 * Return value: a value in #FileClass.
 **/
FileClass TICALL tifiles_file_get_class(const char *filename)
{
	if (tifiles_file_is_single(filename))
	{
		return TIFILE_SINGLE;
	}
	else if (tifiles_file_is_group(filename))
	{
		return TIFILE_GROUP;
	}
	else if (tifiles_file_is_backup(filename))
	{
		return TIFILE_BACKUP;
	}
	else if (tifiles_file_is_os(filename))
	{
		return TIFILE_OS;
	}
	else if (tifiles_file_is_app(filename))
	{
		return TIFILE_APP;
	}
	else if (tifiles_file_is_flash(filename))
	{
		return TIFILE_FLASH;
	}
	else if (tifiles_file_is_tigroup(filename))
	{
		return TIFILE_TIGROUP;
	}
	else
	{
		return TIFILE_NONE;
	}
}

/**
 * tifiles_file_get_type:
 * @filename: a filename as string.
 *
 * Returns the type of file (function, program, ...).
 *
 * Return value: a string like "Assembly Program" (localized).
 **/
const char *TICALL tifiles_file_get_type(const char *filename)
{
	const char *e = tifiles_fext_get(filename);
#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return "";
	}

	if (check_for_Nspire_OS_file_extension(e) || !g_ascii_strcasecmp(e, "tib") || !g_ascii_strcasecmp(e, "c2u"))
	{
		return _("OS upgrade");
	}
#endif

	if (!tifiles_file_is_ti(filename))
	{
		return "";
	}

	if (tifiles_file_is_tigroup(filename))
	{
		return _("TIGroup");
	}

	if (tifiles_file_is_group(filename))
	{
		switch (tifiles_file_get_model(filename))
		{
#ifndef DISABLE_TI9X
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI89T_USB:
			case CALC_TI92P:
			case CALC_V200:
				return _("Group/Backup");
#endif
			case CALC_NONE:
			case CALC_TI73:
			case CALC_TI82:
			case CALC_TI83:
			case CALC_TI83P:
			case CALC_TI84P:
			case CALC_TI85:
			case CALC_TI86:
			case CALC_TI92:
			case CALC_TI84P_USB:
			case CALC_NSPIRE:
			case CALC_TI80:
			case CALC_TI84PC:
			case CALC_TI84PC_USB:
			case CALC_TI83PCE_USB:
			case CALC_TI84PCE_USB:
			case CALC_TI82A_USB:
			case CALC_TI84PT_USB:
			case CALC_NSPIRE_CRADLE:
			case CALC_NSPIRE_CLICKPAD:
			case CALC_NSPIRE_CLICKPAD_CAS:
			case CALC_NSPIRE_TOUCHPAD:
			case CALC_NSPIRE_TOUCHPAD_CAS:
			case CALC_NSPIRE_CX:
			case CALC_NSPIRE_CX_CAS:
			case CALC_NSPIRE_CMC:
			case CALC_NSPIRE_CMC_CAS:
			case CALC_NSPIRE_CXII:
			case CALC_NSPIRE_CXII_CAS:
			case CALC_NSPIRE_CXIIT:
			case CALC_NSPIRE_CXIIT_CAS:
			case CALC_TI82AEP_USB:
			case CALC_CBL:
			case CALC_CBR:
			case CALC_CBL2:
			case CALC_CBR2:
			case CALC_LABPRO:
			case CALC_TIPRESENTER:
			case CALC_MAX:
			default:
				return _("Group");
		}
	}

	switch (tifiles_file_get_model(filename))
	{
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return tixx_byte2desc(TI73_CONST, TI73_MAXTYPES, tixx_fext2byte(TI73_CONST, TI73_MAXTYPES, e));
		case CALC_TI82:
			return tixx_byte2desc(TI82_CONST, TI82_MAXTYPES, tixx_fext2byte(TI82_CONST, TI82_MAXTYPES, e));
		case CALC_TI83:
			return tixx_byte2desc(TI83_CONST, TI83_MAXTYPES, tixx_fext2byte(TI83_CONST, TI83_MAXTYPES, e));
		case CALC_TI83P:
			return ti83p_byte2desc(ti83p_fext2byte(e));
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return ti84p_byte2desc(ti84p_fext2byte(e));
		case CALC_TI82A_USB:
			return ti82a_byte2desc(ti82a_fext2byte(e));
		case CALC_TI84PT_USB:
			return ti84pt_byte2desc(ti84pt_fext2byte(e));
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return ti84pc_byte2desc(ti84pc_fext2byte(e));
		case CALC_TI83PCE_USB:
			return ti83pce_byte2desc(ti83pce_fext2byte(e));
		case CALC_TI84PCE_USB:
			return ti84pce_byte2desc(ti84pce_fext2byte(e));
		case CALC_TI82AEP_USB:
			return ti82aep_byte2desc(ti82a_fext2byte(e));
		case CALC_TI85:
			return tixx_byte2desc(TI85_CONST, TI85_MAXTYPES, tixx_fext2byte(TI85_CONST, TI85_MAXTYPES, e));
		case CALC_TI86:
			return tixx_byte2desc(TI86_CONST, TI86_MAXTYPES, tixx_fext2byte(TI86_CONST, TI86_MAXTYPES, e));
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
			return ti89_byte2desc(ti89_fext2byte(e));
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return ti89t_byte2desc(ti89_fext2byte(e));
		case CALC_TI92:
			return ti92_byte2desc(ti92_fext2byte(e));
		case CALC_TI92P:
			return ti92p_byte2desc(ti92p_fext2byte(e));
		case CALC_V200:
			return v200_byte2desc(v200_fext2byte(e));
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
			return tixx_byte2desc(NSP_CONST, NSP_MAXTYPES, tixx_fext2byte(NSP_CONST, NSP_MAXTYPES, e));
		case CALC_NONE:
		case CALC_TI80:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_TIPRESENTER:
		case CALC_MAX:
		default:
			return "";
	}

	return "";
}

/**
 * tifiles_file_get_icon:
 * @filename: a filename as string.
 *
 * Returns the type of file (function, program, ...).
 *
 * Return value: a string like "Assembly Program" (non localized).
 **/
const char *TICALL tifiles_file_get_icon(const char *filename)
{
	const char *e = tifiles_fext_get(filename);
#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return "";
	}

	if (check_for_Nspire_OS_file_extension(e) || !g_ascii_strcasecmp(e, "tib"))
	{
		return _("OS upgrade");
	}
#endif

	if (!tifiles_file_is_ti(filename))
	{
		return "";
	}

	if (tifiles_file_is_tigroup(filename))
	{
		return _("TIGroup");
	}

	if (tifiles_file_is_group(filename))
	{
		switch (tifiles_file_get_model(filename))
		{
#ifndef DISABLE_TI9X
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI89T_USB:
			case CALC_TI92P:
			case CALC_V200:
				return _("Group/Backup");
#endif
			case CALC_NONE:
			case CALC_TI73:
			case CALC_TI82:
			case CALC_TI83:
			case CALC_TI83P:
			case CALC_TI84P:
			case CALC_TI85:
			case CALC_TI86:
			case CALC_TI92:
			case CALC_TI84P_USB:
			case CALC_NSPIRE:
			case CALC_TI80:
			case CALC_TI84PC:
			case CALC_TI84PC_USB:
			case CALC_TI83PCE_USB:
			case CALC_TI84PCE_USB:
			case CALC_TI82A_USB:
			case CALC_TI84PT_USB:
			case CALC_NSPIRE_CRADLE:
			case CALC_NSPIRE_CLICKPAD:
			case CALC_NSPIRE_CLICKPAD_CAS:
			case CALC_NSPIRE_TOUCHPAD:
			case CALC_NSPIRE_TOUCHPAD_CAS:
			case CALC_NSPIRE_CX:
			case CALC_NSPIRE_CX_CAS:
			case CALC_NSPIRE_CMC:
			case CALC_NSPIRE_CMC_CAS:
			case CALC_NSPIRE_CXII:
			case CALC_NSPIRE_CXII_CAS:
			case CALC_NSPIRE_CXIIT:
			case CALC_NSPIRE_CXIIT_CAS:
			case CALC_TI82AEP_USB:
			case CALC_CBL:
			case CALC_CBR:
			case CALC_CBL2:
			case CALC_CBR2:
			case CALC_LABPRO:
			case CALC_TIPRESENTER:
			case CALC_MAX:
			default:
				return _("Group");
		}
	}

	switch (tifiles_file_get_model(filename))
	{
#ifndef DISABLE_TI8X
		case CALC_TI73:
			return tixx_byte2icon(TI73_CONST, TI73_MAXTYPES, tixx_fext2byte(TI73_CONST, TI73_MAXTYPES, e));
		case CALC_TI82:
			return tixx_byte2icon(TI82_CONST, TI82_MAXTYPES, tixx_fext2byte(TI82_CONST, TI82_MAXTYPES, e));
		case CALC_TI83:
			return tixx_byte2icon(TI83_CONST, TI83_MAXTYPES, tixx_fext2byte(TI83_CONST, TI83_MAXTYPES, e));
		case CALC_TI83P:
			return ti83p_byte2icon(ti83p_fext2byte(e));
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return ti84p_byte2icon(ti83p_fext2byte(e));
		case CALC_TI82A_USB:
			return ti82a_byte2icon(ti82a_fext2byte(e));
		case CALC_TI84PT_USB:
			return ti84pt_byte2icon(ti84pt_fext2byte(e));
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return ti84pc_byte2icon(ti84pc_fext2byte(e));
		case CALC_TI83PCE_USB:
			return ti83pce_byte2icon(ti83pce_fext2byte(e));
		case CALC_TI84PCE_USB:
			return ti84pce_byte2icon(ti84pce_fext2byte(e));
		case CALC_TI82AEP_USB:
			return ti82aep_byte2icon(ti82a_fext2byte(e));
		case CALC_TI85:
			return tixx_byte2icon(TI85_CONST, TI85_MAXTYPES, tixx_fext2byte(TI85_CONST, TI85_MAXTYPES, e));
		case CALC_TI86:
			return tixx_byte2icon(TI86_CONST, TI86_MAXTYPES, tixx_fext2byte(TI86_CONST, TI86_MAXTYPES, e));
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
			return ti89_byte2icon(ti89_fext2byte(e));
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return ti89t_byte2icon(ti89_fext2byte(e));
		case CALC_TI92:
			return ti92_byte2icon(ti92_fext2byte(e));
		case CALC_TI92P:
			return ti92p_byte2icon(ti92p_fext2byte(e));
		case CALC_V200:
			return v200_byte2icon(v200_fext2byte(e));
#endif
		case CALC_NSPIRE:
		case CALC_NSPIRE_CRADLE:
		case CALC_NSPIRE_CLICKPAD:
		case CALC_NSPIRE_CLICKPAD_CAS:
		case CALC_NSPIRE_TOUCHPAD:
		case CALC_NSPIRE_TOUCHPAD_CAS:
		case CALC_NSPIRE_CX:
		case CALC_NSPIRE_CX_CAS:
		case CALC_NSPIRE_CMC:
		case CALC_NSPIRE_CMC_CAS:
		case CALC_NSPIRE_CXII:
		case CALC_NSPIRE_CXII_CAS:
		case CALC_NSPIRE_CXIIT:
		case CALC_NSPIRE_CXIIT_CAS:
			return tixx_byte2icon(NSP_CONST, NSP_MAXTYPES, tixx_fext2byte(NSP_CONST, NSP_MAXTYPES, e));
		case CALC_NONE:
		case CALC_TI80:
		case CALC_CBL:
		case CALC_CBR:
		case CALC_CBL2:
		case CALC_CBR2:
		case CALC_LABPRO:
		case CALC_TIPRESENTER:
		case CALC_MAX:
		default:
			return "";
	}

	return "";
}
