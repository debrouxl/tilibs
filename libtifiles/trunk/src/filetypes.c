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
#include "error.h"
#include "rwfile.h"
#include "typesxx.h"

/****************/
/* Global types */
/****************/

static const char GROUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"XXx",
	"73g", "82g", "83g", "8Xg", "8Xg", "85g", "86g",
	"89g", "89g", "92g", "9Xg", "V2g", "8Xg", "89g",
	"XXx",
};

static const char BACKUP_FILE_EXT[CALC_MAX + 1][4] =
{
	"XXx",
	"73b", "82b", "83b", "8Xb", "8Xb", "85b", "86b",
	"89g", "89g", "92b", "9Xg", "V2g", "8Xg", "89g",
	"XXx",
};

static const char FLASH_APP_FILE_EXT[CALC_MAX + 1][4] =
{
	"XXx",
	"73k", "XXx", "XXx", "8Xk", "8Xk", "XXx", "XXx",
	"89k", "89k", "XXx", "9Xk", "V2k", "8Xk", "89k",
	"XXx",
};

static const char FLASH_OS_FILE_EXT[CALC_MAX + 1][4] =
{
	"XXx",
	"73u", "XXx", "XXx", "8Xu", "8Xu", "XXx", "XXx",
	"89u", "89u", "XXx", "9Xu", "V2u", "8Xu", "89u",
	"tno",
};

static const char CERTIF_FILE_EXT[CALC_MAX + 1][4] =
{
	"XXx",
	"73q", "XXx", "XXx", "8Xq", "8Xq", "XXx", "XXx",
	"89q", "89q", "XXx", "9Xq", "V2q", "8Xq", "89q",
	"XXx",
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
 * Return value: a file extenstion as string (like "83g").
 **/
TIEXPORT2 const char *TICALL tifiles_fext_of_group (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "XXx";
		case CALC_TI73:
			return "73g";
		case CALC_TI82:
			return "82g";
		case CALC_TI83:
			return "83g";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xg";
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
			return "XXx";
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
 * Return value: a file extenstion as string (like "83b").
 **/
TIEXPORT2 const char *TICALL tifiles_fext_of_backup (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??b";
		case CALC_TI73:
			return "73b";
		case CALC_TI82:
			return "82b";
		case CALC_TI83:
			return "83b";
		case CALC_TI83P:
		case CALC_TI84P:
			return "8Xb";
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
		case CALC_TI84P_USB:
			return "8Xg";
		case CALC_NSPIRE:
			return "XXx";
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
 * Return value: a file extenstion as string (like "89k").
 **/
TIEXPORT2 const char *TICALL tifiles_fext_of_flash_app (CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??k";
		case CALC_TI73:
			return "73k";
		case CALC_TI82:
			return "XXx";
		case CALC_TI83:
			return "XXx";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xk";
		case CALC_TI85:
			return "XXx";
		case CALC_TI86:
			return "XXx";
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89k";
		case CALC_TI92:
			return "XXx";
		case CALC_TI92P:
			return "9Xk";
		case CALC_V200:
			return "V2k";
		case CALC_NSPIRE:
			return "XXx";
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
 * Return value: a file extenstion as string (like "89u").
 **/
TIEXPORT2 const char *TICALL tifiles_fext_of_flash_os(CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??u";
		case CALC_TI73:
			return "73u";
		case CALC_TI82:
			return "XXx";
		case CALC_TI83:
			return "XXx";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xu";
		case CALC_TI85:
			return "XXx";
		case CALC_TI86:
			return "XXx";
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89u";
		case CALC_TI92:
			return "XXx";
		case CALC_TI92P:
			return "9Xu";
		case CALC_V200:
			return "V2u";
		case CALC_NSPIRE:
			return "tno";
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
 * Return value: a file extenstion as string (like "89q").
 **/
TIEXPORT2 const char *TICALL tifiles_fext_of_certif(CalcModel model)
{
	switch (model)
	{
		case CALC_NONE:
			return "??q";
		case CALC_TI73:
			return "73q";
		case CALC_TI82:
			return "XXx";
		case CALC_TI83:
			return "XXx";
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return "8Xq";
		case CALC_TI85:
			return "XXx";
		case CALC_TI86:
			return "XXx";
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return "89q";
		case CALC_TI92:
			return "XXx";
		case CALC_TI92P:
			return "9Xq";
		case CALC_V200:
			return "V2q";
		case CALC_NSPIRE:
			return "XXx";
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
TIEXPORT2 char *TICALL tifiles_fext_get(const char *filename)
{
	char *d = NULL;

	if (filename != NULL)
	{
		d = strrchr(filename, '.');
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
 * Need to be freed when no longer needed.
 **/
TIEXPORT2 char *TICALL tifiles_fext_dup(const char *filename)
{
	return g_strdup(tifiles_fext_get(filename));
}

/**********************/
/* Signature checking */
/**********************/

static int tifiles_file_has_ti_header(const char *filename)
{
	FILE *f;
	char buf[9];
	char *p;

	if (filename != NULL)
	{
		f = g_fopen(filename, "rb");
		if (f != NULL)
		{
			fread_8_chars(f, buf);
			fclose(f);
			for(p = buf; *p != '\0'; p++)
			{
				*p = toupper(*p);
			}

			if (!strcmp(buf, "**TI73**") || !strcmp(buf, "**TI82**") ||
			    !strcmp(buf, "**TI83**") || !strcmp(buf, "**TI83F*") ||
			    !strcmp(buf, "**TI85**") || !strcmp(buf, "**TI86**") ||
			    !strcmp(buf, "**TI89**") || !strcmp(buf, "**TI92**") ||
			    !strcmp(buf, "**TI92P*") || !strcmp(buf, "**V200**") ||
			    !strcmp(buf, "**TIFL**")) {
				return !0;
			}

			if(!strncmp(buf, "*TI", 3))
			{
				return !0;
			}
		}
	}

	return 0;
}

#define TIB_SIGNATURE	"Advanced Mathematics Software"

static int tifiles_file_has_tib_header(const char *filename)
{
	FILE *f;
	char str[128];
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
		return 0;

	if(g_ascii_strcasecmp(e, "tib"))
		return 0;

	f = g_fopen(filename, "rb");
	if(f != NULL)
	{
		fread_n_chars(f, 22, str);
		fread_n_chars(f, strlen(TIB_SIGNATURE), str);
		fclose(f);
		str[strlen(TIB_SIGNATURE)] = '\0';
		if(!strcmp(str, TIB_SIGNATURE))
		{
			return !0;
		}
	}

	return 0;
}

#define TIG_SIGNATURE	"PK\x03\x04"	// 0x04034b50
#define TIG_SIGNATURE2	"PK\x05\x06"	// 0x06054b50

static int tifiles_file_has_tig_header(const char *filename)
{
	FILE *f;
	char str[5];
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
	  return 0;

	if(g_ascii_strcasecmp(e, "tig"))
		return 0;

	f = g_fopen(filename, "rb");
	if(f != NULL)
	{
		fread_n_chars(f, strlen(TIG_SIGNATURE), str);
		fclose(f);
		str[strlen(TIG_SIGNATURE)] = '\0';
		if(!strcmp(str, TIG_SIGNATURE) || !strcmp(str, TIG_SIGNATURE2))
		{
			return !0;
		}
	}

	return 0;
}

#define TNO_SIGNATURE           "TI-Nspire.tno "
#define TNO_NOSAMPLES_SIGNATURE "TI-Nspire.nosamples.tno "
#define TNC_SIGNATURE           "TI-Nspire.tnc "
#define TCO_SIGNATURE           "TI-Nspire.tco "
#define TCC_SIGNATURE           "TI-Nspire.tcc "
#define TMO_SIGNATURE           "TI-Nspire.tmo "
#define TMC_SIGNATURE           "TI-Nspire.tmc "

TIEXPORT2 int TICALL tifiles_file_has_tno_header(const char *filename)
{
	FILE *f;
	char str[128];
	char *e = tifiles_fext_get(filename);
	int ret = 0;

	if (!strcmp(e, ""))
	{
		return ret;
	}

	if(   g_ascii_strcasecmp(e, "tno") && g_ascii_strcasecmp(e, "tnc")
	   && g_ascii_strcasecmp(e, "tco") && g_ascii_strcasecmp(e, "tcc")
	   && g_ascii_strcasecmp(e, "tmo") && g_ascii_strcasecmp(e, "tmc")
	  )
	{
		return ret;
	}

	f = g_fopen(filename, "rb");
	if(f == NULL)
	{
		return ret;
	}

	if (fread_n_chars(f, 63, str) == 0)
	{
		if (   !strncmp(str, TNO_SIGNATURE, 14)
		    || !strncmp(str, TNC_SIGNATURE, 14)
		    || !strncmp(str, TNO_NOSAMPLES_SIGNATURE, 24)
		    || !strncmp(str, TCO_SIGNATURE, 14)
		    || !strncmp(str, TCC_SIGNATURE, 14)
		    || !strncmp(str, TMO_SIGNATURE, 14)
		    || !strncmp(str, TMC_SIGNATURE, 14)
		   )
		{
			ret = !0;
		}
	}

	fclose(f);
	return ret;
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
		return 0;

	if (S_ISREG(buf.st_mode))
		return !0;
	else
		return 0;
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
TIEXPORT2 int TICALL tifiles_file_is_ti(const char *filename)
{
	char *e;

	if (filename != NULL)
	{
		// bug: check that file is not a FIFO
		if (!is_regfile(filename))
			return 0;

		if(tifiles_file_has_ti_header(filename))
			return !0;

		if(tifiles_file_has_tib_header(filename))
			return !0;

		if(tifiles_file_has_tig_header(filename))
			return !0;

		if(tifiles_file_has_tno_header(filename))
			return !0;

		e = tifiles_fext_get(filename);

		if (!strcmp(e, ""))
			return 0;

		if(!g_ascii_strcasecmp(e, "tns"))
			return !0;
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
TIEXPORT2 int TICALL tifiles_file_is_single(const char *filename)
{
	if (!tifiles_file_is_ti(filename))
		return 0;

	if (!tifiles_file_is_group(filename) &&
	    !tifiles_file_is_backup(filename) &&
	    !tifiles_file_is_flash(filename) &&
	    !tifiles_file_is_tigroup(filename))
		return !0;

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
TIEXPORT2 int TICALL tifiles_file_is_group(const char *filename)
{
	int i;
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
		return 0;

	if (!tifiles_file_is_ti(filename))
		return 0;

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (!g_ascii_strcasecmp(e, GROUP_FILE_EXT[i]))
			return !0;
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
TIEXPORT2 int TICALL tifiles_file_is_regular(const char *filename)
{
	if (!tifiles_file_is_ti(filename))
		return 0;

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
TIEXPORT2 int TICALL tifiles_file_is_backup(const char *filename)
{
	int i;
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
		return 0;

	if (!tifiles_file_is_ti(filename))
		return 0;

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (!g_ascii_strcasecmp(e, BACKUP_FILE_EXT[i]))
			return !0;
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
TIEXPORT2 int TICALL tifiles_file_is_os(const char *filename)
{
	int i;
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
		return 0;

	if (!tifiles_file_is_ti(filename))
		return 0;

	if(tifiles_file_is_tib(filename))
		return !0;

	if(tifiles_file_is_tno(filename))
		return !0;

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (!g_ascii_strcasecmp(e, FLASH_OS_FILE_EXT[i]))
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
TIEXPORT2 int TICALL tifiles_file_is_app(const char *filename)
{
	int i;
	char *e = tifiles_fext_get(filename);

	if (!strcmp(e, ""))
		return 0;

	if (!tifiles_file_is_ti(filename))
		return 0;

	for (i = 1; i < CALC_MAX + 1; i++)
	{
		if (!g_ascii_strcasecmp(e, FLASH_APP_FILE_EXT[i]))
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
TIEXPORT2 int TICALL tifiles_file_is_flash(const char *filename)
{
	return tifiles_file_is_os(filename) || tifiles_file_is_app(filename);
}

/**
 * tifiles_file_is_tib:
 * @filename: a filename as string.
 *
 * Check whether file is a TIB formatted file.
 *
 * Return value: a boolean value.
 **/
TIEXPORT2 int TICALL tifiles_file_is_tib(const char *filename)
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
TIEXPORT2 int TICALL tifiles_file_is_tigroup(const char *filename)
{
	return tifiles_file_has_tig_header(filename);
}

TIEXPORT2 int TICALL tifiles_file_is_tig(const char *filename)
{
	return tifiles_file_is_tigroup(filename);
}

TIEXPORT2 int TICALL tifiles_file_is_tno(const char *filename)
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
TIEXPORT2 int TICALL tifiles_file_test(const char *filename, FileClass type, CalcModel target)
{
	char *e;

	if (!tifiles_file_is_ti(filename))
		return 0;

	e = tifiles_fext_get(filename);
	if (!strcmp(e, ""))
		return 0;

	if(target > CALC_MAX)
	{
		tifiles_critical("%s: invalid target argument", __FUNCTION__);
		return 0;
	}

	if(type & TIFILE_SINGLE)
	{
		if(target && !g_ascii_strncasecmp(e, GROUP_FILE_EXT[target], 2))
			return !0;
		else
			return tifiles_file_is_single(filename);
	}
	
	if(type & TIFILE_GROUP)
	{
		if(target && !g_ascii_strcasecmp(e, GROUP_FILE_EXT[target]))
			return !0;
		else
			return tifiles_file_is_group(filename);
	}
	
	if(type & TIFILE_REGULAR)
	{
		return tifiles_file_test(filename, TIFILE_SINGLE, target) ||
				tifiles_file_test(filename, TIFILE_GROUP, target);
	}
	
	if(type & TIFILE_BACKUP)
	{
		if(target && !g_ascii_strcasecmp(e, BACKUP_FILE_EXT[target]))
			return !0;
		else
			return tifiles_file_is_backup(filename);
	}
	
	if(type & TIFILE_OS)
	{
		if(target && !g_ascii_strcasecmp(e, FLASH_OS_FILE_EXT[target]))
		{
			return !0;
		}
		else if(target && tifiles_file_is_tib(filename))
		{
			FILE *f;
			uint8_t data[16];

			f = g_fopen(filename, "rb");
			if(f == NULL)
				return 0;

			fread_n_chars(f, 16, (char *)data);
			fclose(f);

			switch(data[8])
			{
				case 1: if(target != CALC_TI92P) return 0;
				case 3: if(target != CALC_TI89)  return 0;
				case 8: if(target != CALC_V200)  return 0;
				case 9: if(target != CALC_TI89T) return 0;
			}

			return !0;
		}
		else
			return tifiles_file_is_os(filename);
	}
	
	if(type & TIFILE_APP)
	{
		if(target && !g_ascii_strcasecmp(e, FLASH_APP_FILE_EXT[target]))
			return !0;
		else
			return tifiles_file_is_app(filename);
	}
	
	if(type & TIFILE_FLASH)
	{
		return tifiles_file_test(filename, TIFILE_OS, target) || tifiles_file_test(filename, TIFILE_APP, target);
	}
	
	if(type & TIFILE_TIGROUP)
	{
		if(target)
		{
			// No easy/light way for this part: we have to load the whole file
			// and to parse the TigEntry structures.
			TigContent *content;
			int ret, ok=0;
			int k;

			if(!tifiles_file_has_tig_header(filename))
				return 0;

			content = tifiles_content_create_tigroup(CALC_NONE, 0);
			ret = tifiles_file_read_tigroup(filename, content);
			if(ret) return 0;

			for (k = 0; k < content->n_apps; k++)
			{
				TigEntry *te = content->app_entries[k];

				if(tifiles_calc_are_compat(te->content.regular->model, target))
					ok++;
			}

			for (k = 0; k < content->n_vars; k++)
			{
				TigEntry *te = content->var_entries[k];

				if(tifiles_calc_are_compat(te->content.regular->model, target))
					ok++;
			}

			tifiles_content_delete_tigroup(content);
			return ok;
		}
		else
			return tifiles_file_is_tigroup(filename);
	}

	return 0;
}

/********/
/* Misc */
/********/

/* Note: a better way should be to open the file and read the signature */
/**
 * tifiles_file_get_model:
 * @filename: a filename as string.
 *
 * Returns the calculator model targetted for this file.
 *
 * Return value: a model taken in #CalcModel.
 **/
TIEXPORT2 CalcModel TICALL tifiles_file_get_model(const char *filename)
{
	char *ext = tifiles_fext_get(filename);
	int type = CALC_NONE;
	char str[4];

	if (!strcmp(ext, ""))
		return CALC_NONE;

	strncpy(str, ext, 2);
	str[2] = '\0';

	if (!g_ascii_strcasecmp(str, "73"))
		type = CALC_TI73;
	else if (!g_ascii_strcasecmp(str, "82"))
		type = CALC_TI82;
	else if (!g_ascii_strcasecmp(str, "83"))
		type = CALC_TI83;
	else if (!g_ascii_strcasecmp(str, "8x"))
		type = CALC_TI83P;
	else if (!g_ascii_strcasecmp(str, "85"))
		type = CALC_TI85;
	else if (!g_ascii_strcasecmp(str, "86"))
		type = CALC_TI86;
	else if (!g_ascii_strcasecmp(str, "89"))
		type = CALC_TI89;
	else if (!g_ascii_strcasecmp(str, "92"))
		type = CALC_TI92;
	else if (!g_ascii_strcasecmp(str, "9X"))
		type = CALC_TI92P;
	else if (!g_ascii_strcasecmp(str, "V2"))
		type = CALC_V200;
	//else if (!g_ascii_strcasecmp(str, "tib"))
		//type = CALC_TI89;	// consider .tib as TI89
	else if (!g_ascii_strcasecmp(str, "tn") || !g_ascii_strcasecmp(str, "tc") || !g_ascii_strcasecmp(str, "tm"))
		type = CALC_NSPIRE;
	else
		type = CALC_NONE;

	return type;
}

/**
 * tifiles_file_get_class:
 * @filename: a filename as string.
 *
 * Returns the file class (single, group, backup, flash).
 *
 * Return value: a value in #FileClass.
 **/
TIEXPORT2 FileClass TICALL tifiles_file_get_class(const char *filename)
{
	if (tifiles_file_is_single(filename))
		return TIFILE_SINGLE;
	else if (tifiles_file_is_group(filename))
		return TIFILE_GROUP;
	else if (tifiles_file_is_backup(filename))
		return TIFILE_BACKUP;
	else if (tifiles_file_is_flash(filename))
		return TIFILE_FLASH;
	else if (tifiles_file_is_tigroup(filename))
		return TIFILE_TIGROUP;
	else
		return 0;
}

/**
 * tifiles_file_get_type:
 * @filename: a filename as string.
 *
 * Returns the type of file (function, program, ...).
 *
 * Return value: a string like "Assembly Program" (localized).
 **/
TIEXPORT2 const char *TICALL tifiles_file_get_type(const char *filename)
{
	char *ext;

	ext = tifiles_fext_get(filename);
	if (!strcmp(ext, ""))
		return "";

	if (!g_ascii_strcasecmp(ext, "tib"))
		return _("OS upgrade");

	if(   !g_ascii_strcasecmp(ext, "tno") || !g_ascii_strcasecmp(ext, "tnc")
	   || !g_ascii_strcasecmp(ext, "tco") || !g_ascii_strcasecmp(ext, "tcc")
	   || !g_ascii_strcasecmp(ext, "tmo") || !g_ascii_strcasecmp(ext, "tmc")
	  )
		return _("OS upgrade");

	if (!tifiles_file_is_ti(filename))
		return "";

	if(tifiles_file_is_tigroup(filename))
		return _("TIGroup");

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
			return ti73_byte2desc(ti73_fext2byte(ext));
		case CALC_TI82:
			return ti82_byte2desc(ti82_fext2byte(ext));
		case CALC_TI83:
			return ti83_byte2desc(ti83_fext2byte(ext));
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return ti83p_byte2desc(ti83p_fext2byte(ext));
		case CALC_TI85:
			return ti85_byte2desc(ti85_fext2byte(ext));
		case CALC_TI86:
			return ti86_byte2desc(ti86_fext2byte(ext));
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return ti89_byte2desc(ti89_fext2byte(ext));
		case CALC_TI92:
			return ti92_byte2desc(ti92_fext2byte(ext));
		case CALC_TI92P:
			return ti92p_byte2desc(ti92p_fext2byte(ext));
		case CALC_V200:
			return v200_byte2desc(v200_fext2byte(ext));
#endif
		case CALC_NSPIRE:
			return nsp_byte2desc(nsp_fext2byte(ext));
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
TIEXPORT2 const char *TICALL tifiles_file_get_icon(const char *filename)
{
	char *ext;

	ext = tifiles_fext_get(filename);
	if (!strcmp(ext, ""))
		return "";

	if (!g_ascii_strcasecmp(ext, "tib"))
		return _("OS upgrade");

	if (   !g_ascii_strcasecmp(ext, "tno") || !g_ascii_strcasecmp(ext, "tnc")
	    || !g_ascii_strcasecmp(ext, "tco") || !g_ascii_strcasecmp(ext, "tcc")
	    || !g_ascii_strcasecmp(ext, "tmo") || !g_ascii_strcasecmp(ext, "tmc")
	   )
		return _("OS upgrade");

	if (!tifiles_file_is_ti(filename))
		return "";

	if(tifiles_file_is_tigroup(filename))
		return _("TIGroup");

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
			return ti73_byte2icon(ti73_fext2byte(ext));
		case CALC_TI82:
			return ti82_byte2icon(ti82_fext2byte(ext));
		case CALC_TI83:
			return ti83_byte2icon(ti83_fext2byte(ext));
		case CALC_TI83P:
		case CALC_TI84P:
		case CALC_TI84P_USB:
			return ti83p_byte2icon(ti83p_fext2byte(ext));
		case CALC_TI85:
			return ti85_byte2icon(ti85_fext2byte(ext));
		case CALC_TI86:
			return ti86_byte2icon(ti86_fext2byte(ext));
#endif
#ifndef DISABLE_TI9X
		case CALC_TI89:
		case CALC_TI89T:
		case CALC_TI89T_USB:
			return ti89_byte2icon(ti89_fext2byte(ext));
		case CALC_TI92:
			return ti92_byte2icon(ti92_fext2byte(ext));
		case CALC_TI92P:
			return ti92p_byte2icon(ti92p_fext2byte(ext));
		case CALC_V200:
			return v200_byte2icon(v200_fext2byte(ext));
#endif
		case CALC_NSPIRE:
			return nsp_byte2icon(nsp_fext2byte(ext));
		case CALC_NONE:
		default:
			return "";
	}

	return "";
}
