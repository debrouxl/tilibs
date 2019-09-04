/*
 * tifileutil -- program to create / manipulate TI data files
 *
 * Copyright (C) 2006-2010 Benjamin Moody (tipack, titools)
 * Copyright (C) 2010      Jon Sturm (titools)
 * Copyright (C) 1999-2009 Romain Lievin (tf2hex)
 * Copyright (C) 2009-2019 Lionel Debroux (tf2hex)
 * Copyright (C) 2019-2023 Lionel Debroux (tifileutil)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <locale.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tifiles.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "gettext.h"
#include "logging.h"

#define TIFILEUTIL_REQUIRES_LIBTICONV_VERSION   "1.1.6"
#define TIFILEUTIL_REQUIRES_LIBTIFILES2_VERSION  "1.1.8"

static char * progname = nullptr;

static int err_print(const char* func, int errcode)
{
	char* p;
	if (errcode) {
		tifiles_error_get(errcode, &p);
		tifileutil_critical("error in %s:\n%s\n", func, p);
		tifiles_error_free(p);
	}
	return errcode;
}

static int type_name_is_tokenized(CalcModel model, uint8_t type_id)
{
	if (model == CALC_TI73) {
		return (type_id != TI73_PRGM && type_id != TI73_ASM && type_id != TI73_DIR && type_id != TI73_AVAR && type_id < 0x20);
	}

	if (model == CALC_TI82 || model == CALC_TI83 || model == CALC_TI83P || model == CALC_TI84P || model == CALC_TI84PC) {
		return (   type_id != TI82_PRGM && type_id != TI82_PPGM && type_id != TI83p_APPOBJ && type_id != TI83p_APPVAR
		        && type_id != TI83p_TEMPPROGOBJ && type_id != TI83p_GRP && type_id < 0x20);
	}

	// Other models and types.
	return 0;
}

static int type_has_length(CalcModel model, uint8_t type_id)
{
	if (!ticonv_model_is_tiz80(model) && !ticonv_model_is_tiez80(model)) {
		return 0;
	}

	if (model == CALC_TI85 || model == CALC_TI86) {
		return (type_id == TI85_EQU || type_id == TI85_STRNG || type_id == TI85_PICT || type_id == TI85_PRGM);
	}

	if (model == CALC_TI82 || model == CALC_TI83) {
		return (type_id == TI82_YVAR || type_id == TI83_STRNG || type_id == TI82_PRGM || type_id == TI82_PPGM || type_id == TI82_PIC);
	}

	if (model == CALC_TI73) {
		return (type_id == TI73_EQU || type_id == TI73_STRNG || type_id == TI73_PRGM || type_id == TI73_ASM || type_id == TI73_PIC || type_id == TI73_AVAR);
	}

	return (type_id == TI83p_EQU || type_id == TI83p_STRNG || type_id == TI83p_PRGM || type_id == TI83p_ASM || type_id == TI83p_PIC || type_id == TI83p_APPVAR);
}

static int type_is_ti68k_oth(CalcModel model, uint8_t type_id)
{
	if (model == CALC_TI89 || model == CALC_TI92P || model == CALC_V200 || model == CALC_TI89T || model == CALC_TI89T_USB) {
		return (type_id == TI89_ZIP);
	}

	// Other models and types.
	return 0;
}

static int protect_type(CalcModel model, uint8_t type_id)
{
	return ((   tifiles_calc_is_ti8x(model)
	         && model != CALC_TI85
	         && model != CALC_TI86
	         && type_id == TI82_PRGM) ? type_id + 1
	                                  : type_id);
}

static int complexify_type(CalcModel model, uint8_t type_id)
{
	if (tifiles_calc_is_ti9x(model)) {
		return ((type_id == TI89_LIST || type_id == TI89_MAT) ? type_id + 1 : type_id);
	}

	if (model == CALC_TI85 || model == CALC_TI86) {
		return (  (type_id == TI85_REAL || type_id == TI85_VECTR
		        || type_id == TI85_LIST || type_id == TI85_MATRX
		        || type_id == TI85_CONS) ? type_id + 1
		                                 : type_id);
	}

	if (model == CALC_TI82 || model == CALC_TI73) {
		return type_id;
	}

	return ((type_id == TI83_REAL || type_id == TI83_LIST) ? type_id + TI83_CPLX : type_id);
}

typedef int (*FNCT_USAGE) (int *, char ***);
typedef int (*FNCT_MENU) (int *, char ***, unsigned int);

static int tifileutil_wrap_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s wrap [OPTIONS] [FILE | -]
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE
 -n NAME:       set on-calc variable name to NAME (TI-Z80, TI-eZ80, TI-68k)
 -f NAME:       set on-calc variable folder to NAME (TI-68k)
 -t TYPE:       set variable type to TYPE (e.g. 82p)
 -V VERSION:    set file version to VERSION (TI-Z80, TI-eZ80)
 -c COMMENT:    set file comment to COMMENT (strftime format string)
 -x EXTENSION:  set 1-4 chars "extension" for "other" variable type (TI-68k)
 -p:            protect program (TI-Z80, TI-eZ80)
 -C:            number/list/matrix is complex (TI-Z80, TI-eZ80)
 -a:            mark file for sending to archive (TI-Z80, TI-eZ80, TI-68k)
 -l:            mark file locked (TI-68k)
 -r:            raw mode (no length bytes)
 -v:            be verbose
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_wrap(int * argc, char *** argv, unsigned int offset)
{
	int ret;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	char * varname = nullptr;
	char * foldername = nullptr;
	char * vartype = nullptr;
	const char * comment = "Created by " PACKAGE " " PACKAGE_VERSION;
	const char * versionstr = nullptr;
	const char * extensionstr = nullptr;
	int protect = 0;
	int complexify = 0;
	int archive = 0;
	int lock = 0;
	int rawmode = 0;
	int verbose = 0;
	CalcModel model;
	uint8_t type_id;
	FILE * infile;
	FileContent * fc = nullptr;
	VarEntry * ve = nullptr;
	unsigned long dsize, dalloc = 1024;
	bool outfilename_is_allocated = false;
	bool varname_is_allocated = false;

	int i, j;
	char * p;
	const char * cp;
	time_t t;

	for (i = offset + 1; i < *argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						if ((*argv)[i][++j]) {
							outfilename = &((*argv)[i][j]);
						}
						else {
							outfilename = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'n':
						if ((*argv)[i][++j]) {
							varname = &((*argv)[i][j]);
						}
						else {
							varname = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'f':
						if ((*argv)[i][++j]) {
							foldername = &((*argv)[i][j]);
						}
						else {
							foldername = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 't':
						if ((*argv)[i][++j]) {
							vartype = &((*argv)[i][j]);
						}
						else {
							vartype = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'c':
						if ((*argv)[i][++j]) {
							comment = (const char *)&((*argv)[i][j]);
						}
						else {
							comment = (const char *)(*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'V':
						if ((*argv)[i][++j]) {
							versionstr = (const char *)&((*argv)[i][j]);
						}
						else {
							versionstr = (const char *)(*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'x':
						if ((*argv)[i][++j]) {
							extensionstr = (const char *)&((*argv)[i][j]);
						}
						else {
							extensionstr = (const char *)(*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'p':
						protect = 1;
					break;

					case 'C':
						complexify = 1;
					break;

					case 'a':
						archive = 1;
					break;

					case 'l':
						lock = 1;
					break;

					case 'r':
						rawmode = 1;
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical("%s: unknown option -%c\n", (*argv)[0], (*argv)[i][j]);
					return tifileutil_wrap_usage(argc, argv);
				}
			}
		}
		else if ((*argv)[i][0] != '-') {
			infilename = (const char *)(*argv)[i];
		}
	}

	/* Set default outfilename or vartype */

	if (!outfilename && vartype) {
		if (infilename) {
			outfilename = (char *)malloc(strlen(infilename) + strlen(vartype) + 2);
			if (!outfilename) {
				goto oom;
			}
			outfilename_is_allocated = true;
			strcpy(outfilename, infilename);
			if ((p = strrchr(outfilename, '.'))) {
				*p = 0;
			}
			strcat(outfilename, ".");
			strcat(outfilename, vartype);
		}
		else {
			outfilename = (char *)malloc(strlen(vartype) + 3);
			if (!outfilename) {
				goto oom;
			}
			outfilename_is_allocated = true;
			sprintf(outfilename, "a.%s", vartype);
		}
	}
	else if (outfilename && !vartype) {
		if ((p = strrchr(outfilename, '.'))) {
			vartype = &(p[1]);
		}
	}

	if (!vartype) {
		tifileutil_critical("%s: no variable type specified.", (*argv)[0]);
		return 1;
	}

	p = (char *)malloc(strlen(vartype) + 3);
	if (!p) {
oom:
		tifileutil_critical("%s: cannot allocate memory\n", (*argv)[0]);
		ret = 1;
		goto end;
	}
	sprintf(p, "a.%s", vartype);
	model = tifiles_file_get_model(p);
	free(p);

	if (!model) {
		tifileutil_critical("%s: invalid variable type %s\n", (*argv)[0], vartype);
		ret = 1;
		goto end;
	}

	type_id = tifiles_fext2vartype(model, vartype);
	cp = tifiles_vartype2fext(model, type_id);
	if (!cp || !cp[0]) {
		tifileutil_critical("%s: invalid variable type %s\n", (*argv)[0], vartype);
		ret = 1;
		goto end;
	}

	if (protect) {
		type_id = protect_type(model, type_id);
	}
	if (complexify) {
		type_id = complexify_type(model, type_id);
	}

	if (!varname) {
		p = strrchr(outfilename, '.');
		if (p) {
			*p = 0;
		}
		if (type_name_is_tokenized(model, type_id)) {
			varname = ticonv_varname_tokenize(model, outfilename, type_id);
			varname_is_allocated = true;
		}
		else {
			varname = (char *)ticonv_varname_strdup(outfilename);
			for (i = 0; outfilename[i]; i++) {
				if (outfilename[i] >= 'A' && outfilename[i] <= 'Z') {
					varname[i] = outfilename[i];
				}
				else if (outfilename[i] >= 'a' && outfilename[i] <= 'z') {
					varname[i] = outfilename[i] + 'A' - 'a';
				}
				else if (outfilename[i] >= '0' && outfilename[i] <= '9') {
					varname[i] = outfilename[i];
				}
				else {
					varname[i] = '[';
				}
			}
			varname[i] = 0;
			varname_is_allocated = true;
		}
		if (p) {
			*p = '.';
		}
	}

	if (infilename) {
		infile = fopen(infilename, "rb");
		if (!infile) {
			perror(infilename);
			ret = 2;
			goto end;
		}
	}
	else {
		infilename = "(standard input)";
		infile = stdin;
	}

	ve = tifiles_ve_create_alloc_data(dalloc);
	if (nullptr == ve) {
		goto oom;
	}
	if (!rawmode && type_has_length(model, type_id)) {
		dsize = 2;
	}
	else {
		dsize = 0;
	}
	ve->size = dsize;

	i = fgetc(infile);
	while (!feof(infile) && !ferror(infile)) {
		if (dsize >= dalloc) {
			dalloc += 1024;
			ve->size = dsize;
			VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
			if (nullptr == ve2) {
				goto oom;
			}
		}
		ve->data[dsize++] = i;
		i = fgetc(infile);
	}
	ve->size = dsize;

	if (infile != stdin) {
		fclose(infile);
	}

	if (!rawmode && type_has_length(model, type_id)) {
		ve->data[0] = (dsize - 2) & 0xff;
		ve->data[1] = ((dsize - 2) >> 8) & 0xff;
	}

	if (nullptr != extensionstr && extensionstr[0] != 0 && type_is_ti68k_oth(model, type_id)) {
		if (dalloc < dsize + 7) {
			dalloc += 7;
			ve->size = dsize;
			VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
			if (nullptr == ve2) {
				goto oom;
			}
		}
		ve->data[dsize++] = 0;
		ve->data[dsize++] = extensionstr[0];
		if (extensionstr[1] != 0) {
			ve->data[dsize++] = extensionstr[1];
			if (extensionstr[2] != 0) {
				ve->data[dsize++] = extensionstr[2];
				if (extensionstr[3] != 0) {
					ve->data[dsize++] = extensionstr[3];
				}
			}
		}
		ve->data[dsize++] = 0;
		ve->data[dsize++] = 0xF8; // OTH_TAG
		ve->size = dsize;
	}

	fc = tifiles_content_create_regular(model);

	if (comment) {
		if (strchr(comment, '%')) {
			time(&t);
			strftime(fc->comment, 40, comment, localtime(&t));
		}
		else {
			strncpy(fc->comment, comment, 40);
			fc->comment[40 - 1] = 0;
		}
	}

	memset(ve->folder, 0, FLDNAME_MAX);
	if (nullptr != foldername) {
		strncpy(ve->folder, foldername, VARNAME_MAX);
	}
	memset(ve->name, 0, VARNAME_MAX);
	strncpy(ve->name, varname, VARNAME_MAX);
	ve->type = type_id;
	ve->attr = (archive ? ATTRB_ARCHIVED : 0) | (lock ? ATTRB_LOCKED : 0);
	// Intentionally leverage atoi()'s lack of error checking: fall back to 0 if the version string isn't an integer.
	ve->version = (versionstr == nullptr ? 0 : atoi(versionstr));
	ve->size = dsize;

	// tifiles_content_add_entry returns the number of entries.
	i = !tifiles_content_add_entry(fc, ve);
	if (!i) {
		i = err_print("tifiles_file_write_regular", tifiles_file_write_regular(outfilename, fc, 0));

		if (!i && verbose) {
			tifiles_file_display_regular(fc);
		}

		/*i = */err_print("tifiles_content_del_entry", tifiles_content_del_entry(fc, ve));
		ve = nullptr;
	}

	ret = i ? 3 : 0;

end:
	if (nullptr != ve) {
		tifiles_ve_delete(ve);
	}
	tifiles_content_delete_regular(fc);

	if (varname_is_allocated) {
		ticonv_varname_free(varname);
	}
	if (outfilename_is_allocated) {
		free(outfilename);
	}

	return ret;
}

static int tifileutil_unwrap_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s unwrap [OPTIONS] [FILE | -]
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE
 -d:            deep unwrapping (beyond computer-side header and footer)
 -v:            be verbose
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_unwrap(int * argc, char *** argv, unsigned int offset)
{
	int ret;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	FILE * infile;
	int deep = 0;
	int verbose = 0;

	int i, j;

	for (i = offset + 1; i < *argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						if ((*argv)[i][++j]) {
							outfilename = &((*argv)[i][j]);
						}
						else {
							outfilename = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'd':
						deep = 1;
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical("%s: unknown option -%c\n", (*argv)[0], (*argv)[i][j]);
					return tifileutil_unwrap_usage(argc, argv);
				}
			}
		}
		else if ((*argv)[i][0] != '-') {
			infilename = (const char *)(*argv)[i];
		}
	}

	if (infilename) {
		infile = fopen(infilename, "rb");
		if (!infile) {
			perror(infilename);
			ret = 2;
			goto end;
		}
	}
	else {
		infilename = "(standard input)";
		infile = stdin;
	}

	tifileutil_info("%s: unwrap infilename=%s outfilename=%s deep=%d verbose=%d.\n", (*argv)[0], infilename, outfilename, deep, verbose);
	tifileutil_critical("%s: unwrap not implemented yet.\n", (*argv)[0]);

	ret = 3;

end:
	return ret;
}

static int tifileutil_dump_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s dump [OPTIONS] [FILE | -]
Where OPTIONS may include:
 -f FORMAT:     format result as FORMAT (only 'chexarray' is supported for now)
 -o OUTFILE:    output result to OUTFILE
 -n NAME:       set C hex array name to NAME
 -s NAME:       set C hex size variable name to NAME
 -c COUNT:      print COUNT items per line of the array
 -m MINSIZE:    set minimum file size
 -v:            be verbose
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_dump(int * argc, char *** argv, unsigned int offset)
{
	int ret = 1;

	const char * format = nullptr;
	const char * infilename = nullptr;
	const char * outfilename = nullptr;
	const char * arrayname = nullptr;
	const char * arraysizename = nullptr;
	const char * itemsperlinestr = nullptr;
	const char * minsizestr = nullptr;
	int verbose = 0;
	FILE * infile, * outfile = nullptr;
	struct stat st;
	unsigned long length, lenread, itemsperline = 8, minsize = 64;
	unsigned char data[65636]; // +100 for the header and footer.
	unsigned long i, j;

	if (nullptr == (*argv)[2]) {
		return tifileutil_dump_usage(argc, argv);
	}

	for (i = offset + 3; i < (unsigned long)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'f':
						if ((*argv)[i][++j]) {
							format = &((*argv)[i][j]);
						}
						else {
							format = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'o':
						if ((*argv)[i][++j]) {
							outfilename = &((*argv)[i][j]);
						}
						else {
							outfilename = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'n':
						if ((*argv)[i][++j]) {
							arrayname = &((*argv)[i][j]);
						}
						else {
							arrayname = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 's':
						if ((*argv)[i][++j]) {
							arraysizename = &((*argv)[i][j]);
						}
						else {
							arraysizename = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
					break;

					case 'c':
						if ((*argv)[i][++j]) {
							itemsperlinestr = &((*argv)[i][j]);
						}
						else {
							itemsperlinestr = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
						errno = 0;
						itemsperline = strtoul(itemsperlinestr, nullptr, 0);
						if (errno != 0 || itemsperline < 1 || itemsperline > sizeof(data)) {
							tifileutil_critical("%s: invalid number of items per line.\n", (*argv)[0]);
							return ret;
						}
					break;

					case 'm':
						if ((*argv)[i][++j]) {
							minsizestr = &((*argv)[i][j]);
						}
						else {
							minsizestr = (*argv)[++i];
						}
						j = strlen((*argv)[i]) - 1;
						errno = 0;
						minsize = strtoul(minsizestr, nullptr, 0);
						if (errno != 0 || minsize > sizeof(data)) {
							tifileutil_critical("%s: invalid minimum size.\n", (*argv)[0]);
							return ret;
						}
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical("%s: unknown option -%c\n", (*argv)[0], (*argv)[i][j]);
					return tifileutil_dump_usage(argc, argv);
				}
			}
		}
		else if ((*argv)[i][0] != '-') {
			infilename = (const char *)(*argv)[i];
		}
	}

	if (nullptr != format && strcmp(format, "chexarray")) {
		tifileutil_critical("%s: unsupported format %s\n", (*argv)[0], format);
		return ret;
	}

	if (minsize < 64) {
		tifileutil_warning("%s: the specified minimum size is too small for a valid TI graphing calculator file.\n", (*argv)[0]);
	}

	// read input file
	if (nullptr == infilename || (infilename[0] == '-' && infilename[1] == 0)) {
		infile = stdin;
	}
	else {
		infile = fopen(infilename, "rb");
		if (nullptr == infile) {
			tifileutil_critical("%s: unable to open input file.\n", (*argv)[0]);
			return ret;
		}
	}

	if (!outfilename || (outfilename[0] == '-' && outfilename[1] == 0)) {
		outfile = stdout;
	}

	fstat(fileno(infile), &st);
	length = st.st_size;

	lenread = fread(data, sizeof(char), length < sizeof(data) ? length : sizeof(data), infile);

	if (infile != stdin) {
		fclose(infile);
	}

	if (outfile != stdout && verbose) {
		fprintf(stdout, "Read %ld bytes.\n", lenread);
	}

	if (length < minsize) {
		tifileutil_critical("%s: file is smaller than the specified minimum size.\n", (*argv)[0]);
		return ret;
	}

	if (lenread == length) {
		// write output file
		if (nullptr == outfile) {
			outfile = fopen(outfilename, "wt");
			if (nullptr == outfile) {
				tifileutil_critical("%s: unable to open output file.\n", (*argv)[0]);
				return ret;
			}
		}

		// Produce hex digits while checking / computing checksum.
		fprintf(outfile, "static unsigned char %s[] = {\n", arrayname);
		for (i = 0; i < length; i += itemsperline) {
			for (j = 0; (j < itemsperline) && (i + j < length); j++) {
				fprintf(outfile, "0x%02x, ", data[i + j]);
			}
			fputc('\n', outfile);
		}
		// TODO: 2 last digits.
		fprintf(outfile, "};\nstatic unsigned int %s = sizeof(%s)/sizeof(%s[0]); // %ld\n", arraysizename, arrayname, arrayname, length);

		if (outfile != stdout) {
			fclose(outfile);
		}

		ret = 0;
	}
	else {
		tifileutil_critical("%s: unable to read the entire input file.\n", (*argv)[0]);
		return ret;
	}

	return ret;
}

/*
 -n NAME:       set on-calc variable name to NAME
 -f NAME:       set on-calc variable folder to NAME
 -t TYPE:       set variable type to TYPE (e.g. 82p)
 -V VERSION:    set file version to VERSION (TI-Z80, TI-eZ80)
 -c COMMENT:    set file comment to COMMENT (strftime format string)
 -a:            mark file for sending to archive (TI-Z80, TI-eZ80, TI-68k)
 -l:            mark file locked (TI-68k)
*/
static int tifileutil_metadata_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s metadata -m MODE [OPTIONS] [FILE | -]
Where MODE may be:
 gather:        print metadata from a file under a parsable "key": "value" form
 apply:         apply metadata from the "key": "value" form generated by gather or a compatible tool
 get:           retrieve a single piece of metadata from a file.
 set:           set one or more pieces of metadata on a file.

Generic OPTIONS may include:
 -o FILE:     output result to FILE
 -v:          be verbose

OPTIONS for get mode may include (a single option at a time):
 -n:            get on-calc variable name (TI-Z80, TI-eZ80, TI-68k)
 -f:            get on-calc folder name (TI-68k)
 -t:            get variable type as integer (TI-Z80, TI-eZ80, TI-68k)
 -h:            get file magic number (8 bytes)
 -V:            get file version (TI-Z80, TI-eZ80)
 -c:            get file comment
 -a:            get file archived information (TI-Z80, TI-eZ80, TI-68k)
 -l:            get file locked information (TI-68k)
 -C:            get stored data checksum and computed data checksum

OPTIONS for set mode may include (multiple options at a time):
 -n NAME:       set on-calc variable name to NAME (TI-Z80, TI-eZ80, TI-68k)
 -f NAME:       set on-calc folder name to NAME (TI-68k)
 -t TYPE:       set variable type to TYPE integer (TI-Z80, TI-eZ80, TI-68k)
 -h MAGIC:      set file magic number to MAGIC
 -V VERSION:    set file version to VERSION (TI-Z80, TI-eZ80)
 -c COMMENT:    set file comment to COMMENT
 -a 0/1:        set file archived information to 0/1 (TI-Z80, TI-eZ80, TI-68k)
 -l 0/1:        set file locked information to 0/1 (TI-68k) - note: archived implies locked
 -C VALUE|fix:  set stored data checksum to the given value, or compute the data checksum and set it as stored data checksum.
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_metadata(int * argc, char *** argv, unsigned int offset)
{
	int ret;

	tifileutil_critical("%s: metadata not implemented yet.\n", (*argv)[0]);

	ret = 1;

	return ret;
}

typedef struct
{
	const char * desc;
	const char * scriptable_name;
	uint32_t scriptable_name_len;
	FNCT_MENU fnct;
	FNCT_USAGE usage;
} menu_entry;

static menu_entry fnct_menu[] =
{
#define DEFINE_MENU_ENTRY(desc, method) \
	{ desc, #method, sizeof(#method) - 1, tifileutil_##method, tifileutil_##method##_usage }
// Used for signaling that a newline should be printed
#define NULL_ENTRY \
	{ nullptr, nullptr, 0, nullptr, nullptr }

	// IMPORTANT NOTES: for backwards compatibility, after a scriptable name was defined, it shall never be changed.
	NULL_ENTRY,                                                                                // 0
	{ "Exit now", "exit", 4, nullptr, nullptr },
	DEFINE_MENU_ENTRY("Wrap data to variable", wrap),
	DEFINE_MENU_ENTRY("Unwrap data from variable", unwrap),
	DEFINE_MENU_ENTRY("Dump data from file", dump),
	//DEFINE_MENU_ENTRY("Print or modify metadata", metadata),
	NULL_ENTRY,
#undef NULL_ENTRY
#undef DEFINE_MENU_ENTRY
};

static int tifileutil_scan_parameters(int * argc, char *** argv)
{
	int ret = 0;
	unsigned int choice = 0;

	// tipack compatibility mode.
	if (!strcmp((*argv)[0], "tipack")) {
		ret = tifileutil_wrap(argc, argv, 0U);
	}
	else if (*argc >= 2) {
		if (!strcmp((*argv)[1], "-h") || !strcmp((*argv)[1], "--help")) {
			goto print_usage;
		}
		else if (!strcmp((*argv)[1], "-v") || !strcmp((*argv)[1], "--version")) {
			fputs(PACKAGE " " PACKAGE_VERSION "\n", stderr);
		}
		for (unsigned int i = 1; i < sizeof(fnct_menu) / sizeof(fnct_menu[0]); i++) {
			if (nullptr != fnct_menu[i].scriptable_name) {
				if (!strcmp((*argv)[1], fnct_menu[i].scriptable_name)) {
					choice = i;
					break;
				}
			}
		}
		if (choice != 0) {
			if (nullptr != fnct_menu[choice].fnct) {
				ret = (*fnct_menu[choice].fnct)(argc, argv, 0U);
			}
			else {
				ret = 0;
			}
		}
		else {
			ret = 1;
print_usage:
			fputs(PACKAGE " " PACKAGE_VERSION "\n", stderr);
			for (unsigned int i = 1; i < sizeof(fnct_menu) / sizeof(fnct_menu[0]); i++) {
				if (nullptr != fnct_menu[i].usage) {
					(void)((*fnct_menu[i].usage)(argc, argv));
				}
			}
		}
	}
	else {
		goto print_usage;
	}

	return ret;
}

static int tifileutil_init(int * argc, char *** argv)
{
	int ret = 0;

	/* Check the version of libraries and init framework */
	if (strcmp(ticonv_version_get(), TIFILEUTIL_REQUIRES_LIBTICONV_VERSION) < 0)
	{
		tifileutil_critical(_("libticonv library version is %s but %s mini required.\n"), ticonv_version_get(), TIFILEUTIL_REQUIRES_LIBTICONV_VERSION);
		return 1;
	}

	if (strcmp(tifiles_version_get(), TIFILEUTIL_REQUIRES_LIBTIFILES2_VERSION) < 0)
	{
		tifileutil_critical(_("libtifiles library version is %s but %s mini required.\n"), tifiles_version_get(), TIFILEUTIL_REQUIRES_LIBTIFILES2_VERSION);
		return 1;
	}

	tifiles_library_init();

	/* Init i18n support */
#if 0 // #ifdef ENABLE_NLS
	tifileutil_info("setlocale: %s", setlocale(LC_ALL, ""));
  	tifileutil_info("bindtextdomain: %s", bindtextdomain(PACKAGE, LOCALEDIR));
  	bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	tifileutil_info("textdomain: %s", textdomain(PACKAGE));
#endif

	ret = tifileutil_scan_parameters(argc, argv);

	return ret;
}

static int tifileutil_exit()
{
	tifiles_library_exit();

	return 0;
}

int main(int argc, char ** argv)
{
	int ret;

#ifdef ENABLE_NLS
//	char * locale_dir = g_strconcat(SHARE_DIR, G_DIR_SEPARATOR_S, LOCALEDIR, "/", nullptr);
#endif

	// Force GLib 2.32+ to print info and debug messages like older versions did, unless this variable is already set.
	// No effect on earlier GLib versions.
	// Commented out because this interferes with stdout mode for e.g. dump.
	//g_setenv("G_MESSAGES_DEBUG", "all", /* overwrite = */ FALSE);

	progname = argv[0];

	ret = tifileutil_init(&argc, &argv);

	tifileutil_exit();

	return ret;
}
