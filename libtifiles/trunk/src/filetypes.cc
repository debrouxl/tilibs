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

// Whether to enable strict file extension checking.
#define CHECK_FILE_EXTENSIONS

/****************/
/* Global types */
/****************/

static const char GROUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"",
	"73g", "82g", "83g", "8Xg", "8Xg", "85g", "86g",
	"89g", "89g", "92g", "9Xg", "V2g", "8Xg", "89g",
	"",    "",    "8Xg", "8Xg", "8Xg", "8Xg", "8Xg",
	"8xg",
	""
};

static const char BACKUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"",
	"73b", "82b", "83b", "8Xb", "8Xb", "85b", "86b",
	"89g", "89g", "92b", "9Xg", "V2g", "8Xg", "89g",
	"",    "",    "8Cb", "8Cb", "",    "",    "8Xb",
	"8Xb",
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
			return NULL;
		case CALC_TI73:
			return "73g";
		case CALC_TI80:
			return NULL;
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
			return "8Xg"; // There's also 8Cg.
		case CALC_TI85:
			return "85g";
		case CALC_TI86:
			return "86g";
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
		case CALC_NSPIRE:
			return NULL;
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return NULL;
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
		case CALC_TI73:
			return "73b";
		case CALC_TI80:
			return NULL;
		case CALC_TI82:
			return "82b";
		case CALC_TI83:
			return "83b";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
		case CALC_TI82A_USB:
		case CALC_TI84PT_USB:
			return "8Xb";
		case CALC_TI84PC:
		case CALC_TI84PC_USB:
			return "8Cb";
		case CALC_TI83PCE_USB:
		case CALC_TI84PCE_USB:
			return NULL;
		case CALC_TI85:
			return "85b";
		case CALC_TI86:
			return "86b";
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
		case CALC_NSPIRE:
			return NULL;
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return NULL;
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
		case CALC_TI73:
			return "73k";
		case CALC_TI80:
			return NULL;
		case CALC_TI82:
			return NULL;
		case CALC_TI83:
			return NULL;
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
			return NULL;
		case CALC_TI85:
			return NULL;
		case CALC_TI86:
			return NULL;
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89k";
		case CALC_TI92:
			return NULL;
		case CALC_TI92P:
			return "9Xk";
		case CALC_V200:
			return "V2k";
		case CALC_NSPIRE:
			return NULL;
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return NULL;
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
			return NULL;
		case CALC_TI73:
			return "73u";
		case CALC_TI80:
			return NULL;
		case CALC_TI82:
			return NULL;
		case CALC_TI83:
			return NULL;
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
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
		case CALC_TI84PT_USB:
			return "8Xu";
		case CALC_TI85:
			return NULL;
		case CALC_TI86:
			return NULL;
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89u";
		case CALC_TI92:
			return NULL;
		case CALC_TI92P:
			return "9Xu";
		case CALC_V200:
			return "V2u";
		case CALC_NSPIRE:
			return "tno"; // Actually, it depends on the sub-model...
		default:
			tifiles_critical("%s: invalid model argument", __FUNCTION__);
			break;
	}

	return NULL;
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
		case CALC_TI73:
			return "73q";
		case CALC_TI80:
			return NULL;
		case CALC_TI82:
			return NULL;
		case CALC_TI83:
			return NULL;
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
			return NULL;
		case CALC_TI85:
			return NULL;
		case CALC_TI86:
			return NULL;
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89q";
		case CALC_TI92:
			return NULL;
		case CALC_TI92P:
			return "9Xq";
		case CALC_V200:
			return "V2q";
		case CALC_NSPIRE:
			return NULL;
		default:
			tifiles_critical("%s: invalid calc_type argument", __FUNCTION__);
			break;
	}

	return NULL;
}

/**
 * tifiles_fext_get:
 * @filename: a filename as string.
 *
 * Returns file extension part.
 *
 * Return value: a file extension without dot as string (like "89g").
 **/
char * TICALL tifiles_fext_get(const char *filename)
{
	if (filename != NULL)
	{
		char * d = (char *)strrchr(filename, '.');
		if (d != NULL)
		{
			return (++d);
		}
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return (char *)"";
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
	FILE *f;
	char buf[9];
	char *p;
	int ret = 0;

	if (filename != NULL)
	{
		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			memset(buf, 0, sizeof(buf));
			if (fread_8_chars(f, buf) == 0)
			{
				for (p = buf; *p != '\0'; p++)
				{
					*p = toupper(*p);
				}

				if (!strcmp(buf, "**TI73**") || !strcmp(buf, "**TI82**") ||
				    !strcmp(buf, "**TI83**") || !strcmp(buf, "**TI83F*") ||
				    !strcmp(buf, "**TI85**") || !strcmp(buf, "**TI86**") ||
				    !strcmp(buf, "**TI89**") || !strcmp(buf, "**TI92**") ||
				    !strcmp(buf, "**TI92P*") || !strcmp(buf, "**V200**") ||
				    !strcmp(buf, "**TIFL**") ||
				    !strncmp(buf, "*TI", 3))
				{
					ret = !0;
				}
			}
			fclose(f);
		}
	}

	return ret;
}

#define TIB_SIGNATURE	"Advanced Mathematics Software"

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
	FILE *f;
	char str[64];
	int ret = 0;

	if (filename != NULL)
	{
#ifdef CHECK_FILE_EXTENSIONS
		char *e = tifiles_fext_get(filename);

		if (   e[0] == 0
		    || g_ascii_strcasecmp(e, "tib"))
		{
			return 0;
		}
#endif

		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			if (fread_n_chars(f, 22 + sizeof(TIB_SIGNATURE) + 1, str) == 0)
			{
				str[22 + sizeof(TIB_SIGNATURE) + 1] = '\0';
				if (!strcmp(str + 22, TIB_SIGNATURE))
				{
					ret = !0;
				}
			}
			fclose(f);
		}
	}

	return ret;
}

#define TIG_SIGNATURE	"PK\x03\x04"	// 0x04034b50
#define TIG_SIGNATURE2	"PK\x05\x06"	// 0x06054b50

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
	FILE *f;
	char str[5];
	int ret = 0;

	if (filename != NULL)
	{
#ifdef CHECK_FILE_EXTENSIONS
		char *e = tifiles_fext_get(filename);

		if (   e[0] == 0
		    || g_ascii_strcasecmp(e, "tig"))
		{
			return 0;
		}
#endif

		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			if (fread_n_chars(f, strlen(TIG_SIGNATURE), str) == 0)
			{
				str[strlen(TIG_SIGNATURE)] = '\0';
				if (!strcmp(str, TIG_SIGNATURE) || !strcmp(str, TIG_SIGNATURE2))
				{
					ret = !0;
				}
			}
			fclose(f);
		}
	}

	return ret;
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
	FILE *f;
	uint8_t buf[78];
	uint32_t len;
	int ret = 0;

	if (filename != NULL)
	{
		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			while (fread(buf, 1, 78, f) == 78)
			{
				if (strncmp((char *) buf, "**TIFL**", 8))
				{
					break;
				}

				ret = 1;

				/* if there are multiple entries (old OS files with license
				   agreement, old app files with certificate), return the type
				   of the last entry */
				if (dev_type != NULL)
				{
					*dev_type = buf[48];
				}
				if (data_type != NULL)
				{
					*data_type = buf[49];
				}

				len = buf[74] | (((uint32_t)buf[75]) << 8) | (((uint32_t)buf[76]) << 16) | (((uint32_t)buf[77]) << 24);
				if (fseek(f, len, SEEK_CUR))
				{
					break;
				}
			}

			fclose(f);
		}
	}

	return ret;
}

#define TNO_SIGNATURE           "TI-Nspire.tno "
#define TNO_NOSAMPLES_SIGNATURE "TI-Nspire.nosamples.tno "
#define TNC_SIGNATURE           "TI-Nspire.tnc "
#define TCO_SIGNATURE           "TI-Nspire.tco "
#define TCC_SIGNATURE           "TI-Nspire.tcc "
#define TMO_SIGNATURE           "TI-Nspire.tmo "
#define TMC_SIGNATURE           "TI-Nspire.tmc "
#define OSEXT1_SIGNATURE        "__OSEXT__1 "

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
	FILE *f;
	char str[128];
	int ret = 0;

	if (filename != NULL)
	{
#ifdef CHECK_FILE_EXTENSIONS
		char *e = tifiles_fext_get(filename);

		if (   e[0] == 0
		    || (   g_ascii_strcasecmp(e, "tno") && g_ascii_strcasecmp(e, "tnc")
			&& g_ascii_strcasecmp(e, "tco") && g_ascii_strcasecmp(e, "tcc")
			&& g_ascii_strcasecmp(e, "tmo") && g_ascii_strcasecmp(e, "tmc")
		       )
		   )
		{
			return 0;
		}
#endif

		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			if (fread_n_chars(f, 63, str) == 0)
			{
				if (   !strncmp(str, TNO_SIGNATURE, 14)
				    || !strncmp(str, TNC_SIGNATURE, 14)
				    || !strncmp(str, TNO_NOSAMPLES_SIGNATURE, 24)
				    || !strncmp(str, TCO_SIGNATURE, 14)
				    || !strncmp(str, TCC_SIGNATURE, 14)
				    || !strncmp(str, TMO_SIGNATURE, 14)
				    || !strncmp(str, TMC_SIGNATURE, 14)
				    || !strncmp(str, OSEXT1_SIGNATURE, 11)
				   )
				{
					ret = !0;
				}
			}

			fclose(f);
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
		return DEVICE_TYPE_83P;

	case CALC_TI89:
	case CALC_TI89T:
	case CALC_TI89T_USB:
		return DEVICE_TYPE_89;

	case CALC_TI92P:
	case CALC_V200:
		return DEVICE_TYPE_92P;

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
int TICALL tifiles_file_is_ti(const char *filename)
{
	char *e;

	if (filename != NULL)
	{
		// bug: check that file is not a FIFO
		if (!is_regfile(filename))
		{
			return 0;
		}

		if (   tifiles_file_has_ti_header(filename)
		    || tifiles_file_has_tib_header(filename)
		    || tifiles_file_has_tig_header(filename)
		    || tifiles_file_has_tifl_header(filename, NULL, NULL)
		    || tifiles_file_has_tno_header(filename))
		{
			return !0;
		}

		e = tifiles_fext_get(filename);

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
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
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
	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	if (!tifiles_file_is_group(filename) &&
	    !tifiles_file_is_backup(filename) &&
	    !tifiles_file_is_flash(filename) &&
	    !tifiles_file_is_tigroup(filename))
	{
		return !0;
	}

	return 0;
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
	int i;
	char *e = tifiles_fext_get(filename);

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif
	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (GROUP_FILE_EXT[i][0] != 0 && !g_ascii_strcasecmp(e, GROUP_FILE_EXT[i]))
		{
			return !0;
		}
	}

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
	int i;
	char *e = tifiles_fext_get(filename);

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif
	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (BACKUP_FILE_EXT[i][0] != 0 && !g_ascii_strcasecmp(e, BACKUP_FILE_EXT[i]))
		{
			return !0;
		}
	}

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
	uint8_t type;

	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	if (   tifiles_file_is_tib(filename)
	    || tifiles_file_is_tno(filename)
	    || (tifiles_file_has_tifl_header(filename, NULL, &type) && type == TI83p_AMS))
	{
		return !0;
	}

	return 0;
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
	uint8_t type;

	if (!tifiles_file_is_ti(filename))
	{
		return 0;
	}

	if (tifiles_file_has_tifl_header(filename, NULL, &type) && type == TI83p_APPL)
	{
		return !0;
	}

	return 0;
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
	return (tifiles_file_is_tib(filename) ||
	        tifiles_file_is_tno(filename) ||
	        tifiles_file_has_tifl_header(filename, NULL, NULL));
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
	char *e = tifiles_fext_get(filename);
	uint8_t ctype, dtype;

#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return 0;
	}
#endif

	if (target > CALC_MAX)
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
			FILE *f;
			uint8_t data[16];

			f = g_fopen(filename, "rb");
			if (f == NULL)
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
			TigContent *content;
			int ret, ok=0;
			unsigned int k;

			if (!tifiles_file_has_tig_header(filename))
			{
				return 0;
			}

			content = tifiles_content_create_tigroup(CALC_NONE, 0);
			ret = tifiles_file_read_tigroup(filename, content);
			if (ret)
			{
				tifiles_content_delete_tigroup(content);
				return 0;
			}

			for (k = 0; k < content->n_apps; k++)
			{
				TigEntry *te = content->app_entries[k];

				if(tifiles_calc_are_compat(te->content.regular->model, target))
				{
					ok++;
				}
			}

			for (k = 0; k < content->n_vars; k++)
			{
				TigEntry *te = content->var_entries[k];

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

	if (ext == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return CALC_NONE;
	}

	if (ext[0] != 0 && ext[1] != 0 && ext[2] != 0)
	{
		char c1 = g_ascii_tolower(ext[0]);
		char c2 = g_ascii_tolower(ext[1]);
		char c3 = g_ascii_tolower(ext[2]);

		if (c1 == '7' && c2 == '3')
		{
			type = CALC_TI73;
		}
		else if (c1 == '8')
		{
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
			else if (c2 == '5')
			{
				type = CALC_TI85;
			}
			else if (c2 == '6')
			{
				type = CALC_TI86;
			}
			else if (c2 == '9')
			{
				type = CALC_TI89;
			}
			// else fall through.
		}
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
		//else if (!g_ascii_strcasecmp(str, "tib"))
			//type = CALC_TI89;	// consider .tib as TI89
		else if (c1 == 't')
		{
			if (c2 == 'n' || c2 == 'c' || c2 == 'm')
			{
				if (c3 == 's' || c3 == 'c' || c3 == 'o')
				{
					type = CALC_NSPIRE;
				}
				// else fall through.
			}
			// else fall through.
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
	char *e = tifiles_fext_get(filename);
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
	char *e = tifiles_fext_get(filename);
#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return "";
	}
#endif
	if (   !g_ascii_strcasecmp(e, "tib")
	    || !g_ascii_strcasecmp(e, "tno") || !g_ascii_strcasecmp(e, "tnc")
	    || !g_ascii_strcasecmp(e, "tco") || !g_ascii_strcasecmp(e, "tcc")
	    || !g_ascii_strcasecmp(e, "tmo") || !g_ascii_strcasecmp(e, "tmc")
	  )
	{
		return _("OS upgrade");
	}

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
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI89T_USB:
			case CALC_TI92P:
			case CALC_V200:
				return _("Group/Backup");
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
			return tixx_byte2desc(NSP_CONST, NSP_MAXTYPES, tixx_fext2byte(NSP_CONST, NSP_MAXTYPES, e));
		case CALC_NONE:
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
	char *e = tifiles_fext_get(filename);
#ifdef CHECK_FILE_EXTENSIONS
	if (e[0] == 0)
	{
		return "";
	}
#endif
	if (   !g_ascii_strcasecmp(e, "tib")
	    || !g_ascii_strcasecmp(e, "tno") || !g_ascii_strcasecmp(e, "tnc")
	    || !g_ascii_strcasecmp(e, "tco") || !g_ascii_strcasecmp(e, "tcc")
	    || !g_ascii_strcasecmp(e, "tmo") || !g_ascii_strcasecmp(e, "tmc")
	   )
	{
		return _("OS upgrade");
	}

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
			case CALC_TI89:
			case CALC_TI89T:
			case CALC_TI89T_USB:
			case CALC_TI92P:
			case CALC_V200:
				return _("Group/Backup");
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
			return tixx_byte2icon(NSP_CONST, NSP_MAXTYPES, tixx_fext2byte(NSP_CONST, NSP_MAXTYPES, e));
		case CALC_NONE:
		default:
			return "";
	}

	return "";
}
