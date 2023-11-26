/*
 * tifileutil -- program to create / manipulate TI data files
 *
 * Copyright (C) 2006-2010 Benjamin Moody (tipack, titools)
 * Copyright (C) 2010      Jon Sturm (titools, tifileutils)
 * Copyright (C) 1999-2005 Thomas Nussbaumer (tt*)
 * Copyright (C) 1999-2009 Romain Lievin (general libti* API, tf2hex)
 * Copyright (C) 2009-2026 Lionel Debroux (tf2hex, tifileutil)
 * Copyright (C) 2015-2026 Adrien Bertrand (tivars_lib_cpp, tifileutil)
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
#include <strings.h>
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

typedef int (*FNCT_USAGE) (int *, char ***);
typedef int (*FNCT_MENU) (int *, char ***, unsigned int);

// Capture the value of an option: either the rest of the current argument
// (-oVALUE) or the next argument (-o VALUE); on a missing value, print an
// error and return the result of the given usage function.
#define GET_OPTION_ARG(var, optstr, usagefn) \
	do { \
		if ((*argv)[i][++j]) { \
			(var) = &((*argv)[i][j]); \
		} \
		else { \
			(var) = (*argv)[++i]; \
			if (nullptr == (*argv)[i]) { \
				tifileutil_critical(_("%s: option %s requires an argument."), (*argv)[0], (optstr)); \
				return (usagefn)(argc, argv); \
			} \
		} \
		j = strlen((*argv)[i]) - 1; \
	} while (0)

// Like GET_OPTION_ARG, but discard the value (used to skip the -m MODE
// option already consumed by the dispatcher).
#define GET_OPTION_ARG_IGNORE(optstr, usagefn) \
	do { \
		if (!(*argv)[i][++j]) { \
			if (nullptr == (*argv)[++i]) { \
				tifileutil_critical(_("%s: option %s requires an argument."), (*argv)[0], (optstr)); \
				return (usagefn)(argc, argv); \
			} \
		} \
		j = strlen((*argv)[i]) - 1; \
	} while (0)

// Capture the value of a long-form option: --option=VALUE or --option VALUE;
// on a missing value or an unknown suffix, print an error and return the
// result of the given usage function.
#define GET_LONG_OPTION_ARG(var, longopt, usagefn) \
	do { \
		if ((*argv)[i][sizeof(longopt) - 1] == '=') { \
			(var) = &((*argv)[i][sizeof(longopt)]); \
		} \
		else if (!(*argv)[i][sizeof(longopt) - 1]) { \
			(var) = (*argv)[++i]; \
			if (nullptr == (*argv)[i]) { \
				tifileutil_critical(_("%s: option %s requires an argument."), (*argv)[0], (longopt)); \
				return (usagefn)(argc, argv); \
			} \
		} \
		else { \
			tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][1]); \
			return (usagefn)(argc, argv); \
		} \
	} while (0)

static int err_print(char *** argv, const char* func, int errcode)
{
	char* p;
	if (errcode) {
		tifiles_error_get(errcode, &p);
		tifileutil_critical(_("%s: error in %s:\n%s"), (*argv)[0], func, p);
		tifiles_error_free(p);
	}
	return errcode;
}

static int hex_nibble(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

static char * hex_encode(const uint8_t * data, uint32_t length, int uppercase)
{
	char * hex = (char *)malloc(2 * (size_t)length + 1);
	const char * digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	uint32_t i;

	if (!hex) {
		return nullptr;
	}
	for (i = 0; i < length; i++) {
		hex[2 * i] = digits[data[i] >> 4];
		hex[2 * i + 1] = digits[data[i] & 0x0F];
	}
	hex[2 * (size_t)length] = 0;
	return hex;
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
Note: all options must precede the file name(s).
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
	struct stat st = {};
	unsigned long length, lenread, itemsperline = 8, minsize = 64;
	unsigned char data[65636]; // +100 for the header and footer.
	unsigned long i, j;
	unsigned long first_file = 0;

	if (nullptr == (*argv)[2]) {
		return tifileutil_dump_usage(argc, argv);
	}

	for (i = offset + 2; i < (unsigned long)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'f':
						GET_OPTION_ARG(format, "-f", tifileutil_dump_usage);
					break;

					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_dump_usage);
					break;

					case 'n':
						GET_OPTION_ARG(arrayname, "-n", tifileutil_dump_usage);
					break;

					case 's':
						GET_OPTION_ARG(arraysizename, "-s", tifileutil_dump_usage);
					break;

					case 'c':
						GET_OPTION_ARG(itemsperlinestr, "-c", tifileutil_dump_usage);
						errno = 0;
						itemsperline = strtoul(itemsperlinestr, nullptr, 0);
						if (errno != 0 || itemsperline < 1 || itemsperline > sizeof(data)) {
							tifileutil_critical(_("%s: invalid number of items per line."), (*argv)[0]);
							return ret;
						}
					break;

					case 'm':
						GET_OPTION_ARG(minsizestr, "-m", tifileutil_dump_usage);
						errno = 0;
						minsize = strtoul(minsizestr, nullptr, 0);
						if (errno != 0 || minsize > sizeof(data)) {
							tifileutil_critical(_("%s: invalid minimum size."), (*argv)[0]);
							return ret;
						}
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_dump_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (first_file) {
		infilename = (const char *)(*argv)[first_file];
	}

	if (nullptr != format && strcmp(format, "chexarray")) {
		tifileutil_critical(_("%s: unsupported format %s."), (*argv)[0], format);
		return ret;
	}
	if (!arrayname) {
		tifileutil_critical(_("%s: no array name specified."), (*argv)[0]);
		return tifileutil_dump_usage(argc, argv);
	}
	if (!arraysizename) {
		tifileutil_critical(_("%s: no size variable name specified."), (*argv)[0]);
		return tifileutil_dump_usage(argc, argv);
	}

	if (minsize < 64) {
		tifileutil_warning(_("%s: the specified minimum size is too small for a valid TI graphing calculator file."), (*argv)[0]);
	}

	// read input file
	if (nullptr == infilename || (infilename[0] == '-' && infilename[1] == 0)) {
		infile = stdin;
	}
	else {
		infile = fopen(infilename, "rb");
		if (nullptr == infile) {
			tifileutil_critical(_("%s: unable to open input file."), (*argv)[0]);
			return ret;
		}
	}

	if (!outfilename || (outfilename[0] == '-' && outfilename[1] == 0)) {
		outfile = stdout;
	}

	fstat(fileno(infile), &st);
	const unsigned long length = st.st_size;

	const unsigned long lenread = fread(data, sizeof(char), length < sizeof(data) ? length : sizeof(data), infile);

	if (infile != stdin) {
		fclose(infile);
	}

	if (outfile != stdout && verbose) {
		fprintf(stdout, "Read %lu bytes.\n", lenread);
	}

	if (length < minsize) {
		tifileutil_critical(_("%s: file is smaller than the specified minimum size."), (*argv)[0]);
		return ret;
	}

	if (lenread == length) {
		// write output file
		if (nullptr == outfile) {
			outfile = fopen(outfilename, "wt");
			if (nullptr == outfile) {
				tifileutil_critical(_("%s: unable to open output file."), (*argv)[0]);
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
		fprintf(outfile, "};\nstatic unsigned int %s = sizeof(%s)/sizeof(%s[0]); // %lu\n", arraysizename, arrayname, arrayname, length);

		if (outfile != stdout) {
			fclose(outfile);
		}

		ret = 0;
	}
	else {
		tifileutil_critical(_("%s: unable to read the entire input file."), (*argv)[0]);
		return ret;
	}

	return ret;
}

static int tifileutil_checksum_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s checksum [OPTIONS] FILE
Where OPTIONS may include:
 -f:            recompute the stored checksum and write a corrected file
 -o OUTFILE:    output corrected file to OUTFILE (required with -f)
 -v:            be verbose
Note: all options must precede the file name(s).
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_checksum(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	int fix = 0;
	int verbose = 0;
	CalcModel model;
	FileContent * fc = nullptr;

	int i, j;
	unsigned int first_file = 0;

	// Skip argv[1], the command name.
	for (i = offset + 2; i < *argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_checksum_usage);
					break;

					case 'f':
						fix = 1;
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_checksum_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (!first_file) {
		return tifileutil_checksum_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];
	if (fix && !outfilename) {
		tifileutil_critical(_("%s: no output file specified."), (*argv)[0]);
		return tifileutil_checksum_usage(argc, argv);
	}

	model = tifiles_file_get_model(infilename);
	if (!model) {
		tifileutil_critical(_("%s: invalid input file %s."), (*argv)[0], infilename);
		goto end;
	}

	fc = tifiles_content_create_regular(model);
	if (!fc) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}

	ret = err_print(argv, "tifiles_file_read_regular", tifiles_file_read_regular(infilename, fc));
	if (ret) {
		ret = 3;
		fc = nullptr; // tifiles_file_read_regular() frees the content on error.
		goto end;
	}

	printf("checksum: computed 0x%04X / stored 0x%04X\n", fc->checksum, fc->stored_checksum);
	if (fc->checksum != fc->stored_checksum) {
		tifileutil_critical(_("%s: checksum mismatch in %s: computed: %0x04X / stored: %0x04X."), (*argv)[0], infilename, fc->checksum, fc->stored_checksum);
		ret = 2;
	}
	else {
		ret = 0;
	}

	if (fix) {
		int ret2 = err_print(argv, "tifiles_file_write_regular", tifiles_file_write_regular(outfilename, fc, 0));
		if (ret2) {
			ret = 3;
		}
		else if (verbose) {
			tifileutil_info(_("%s: wrote corrected file %s."), (*argv)[0], outfilename);
		}
	}

end:
	if (fc) {
		tifiles_content_delete_regular(fc);
	}

	return ret;
}

// Set a file comment into @buf of @size bytes: expand the strftime format
// string when it contains '%' and @raw is unset, copy it literally otherwise.
static void tifileutil_comment_set(char * buf, size_t size, const char * comment, int raw)
{
	time_t t;

	if (!raw && strchr(comment, '%')) {
		time(&t);
		strftime(buf, size, comment, localtime(&t));
	}
	else {
		strncpy(buf, comment, size - 1);
		buf[size - 1] = 0;
	}
}

#include "wrap.cc"

#include "squish.cc"

#include "metadata.cc"

#include "group.cc"

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
	DEFINE_MENU_ENTRY("Dump data from file", dump),
	DEFINE_MENU_ENTRY("Check file checksum", checksum),
	DEFINE_MENU_ENTRY("Wrap data to variable", wrap),
	DEFINE_MENU_ENTRY("Unwrap data from variable", unwrap),
	DEFINE_MENU_ENTRY("Squish hex data to binary", squish),
	DEFINE_MENU_ENTRY("Unsquish binary data to hex", unsquish),
	DEFINE_MENU_ENTRY("Print or modify metadata", metadata),
	DEFINE_MENU_ENTRY("Group files into a tigroup", group),
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
		tifileutil_critical(_("%s: libticonv library version is %s but %s mini required."), (*argv)[0], ticonv_version_get(), TIFILEUTIL_REQUIRES_LIBTICONV_VERSION);
		return 1;
	}

	if (strcmp(tifiles_version_get(), TIFILEUTIL_REQUIRES_LIBTIFILES2_VERSION) < 0)
	{
		tifileutil_critical(_("%s: libtifiles library version is %s but %s mini required."), (*argv)[0], tifiles_version_get(), TIFILEUTIL_REQUIRES_LIBTIFILES2_VERSION);
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
