/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2009  Romain Lievin
 *  Copyright (C) 2009-2019  Lionel Debroux
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
 * This program shows how to use the libticalcs library. You can consider this
 * as an authoritative example. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRTOK_R
#define STRTOK strtok_r
#elif defined(HAVE_STRTOK_S)
#define STRTOK strtok_s
#else
#error This code needs strtok_r or strtok_s.
#endif

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define __STDC_FORMAT_MACROS
#include <cinttypes>

#ifdef _MSC_VER
#include <getopt.h>
#else
#include <unistd.h>
#endif

// Happens for some reason on MinGW 64 32-bit GCC 8 toolchain.
#ifndef SCNi8
#define SCNi8 "hhi"
#endif

#include "ticables.h"
#include "tifiles.h"
#include "../src/ticalcs.h"
#include "../src/nsp_cmd.h"
#include "../src/cmdz80.h"
#include "../src/cmd68k.h"
#include "../src/dusb_rpkt.h"
#include "../src/dusb_cmd.h"
#include "../src/keysnsp.h"
#include "../src/nsp_rpkt.h"
#include "../src/romdump.h"
#include "../src/error.h"

#undef VERSION
#define VERSION "Test program"

#define CURRENT_SCRIPT_VERSION 0

// Variables
#define INBUF_DATA_SIZE    262156
#define INBUF2_DATA_SIZE   2048
#define PKTDATA_DATA_SIZE  65542
#define FILENAME_DATA_SIZE 1023

static unsigned int version = CURRENT_SCRIPT_VERSION;
static unsigned int cfg_exit_on_failure = 0;
static char inbuf[INBUF_DATA_SIZE + 2];
static char inbuf2[INBUF2_DATA_SIZE + 2];
static uint8_t pktdata[2 * PKTDATA_DATA_SIZE];
static uint8_t pktdata2[PKTDATA_DATA_SIZE];
static uint32_t pktdata_len = 0;
static uint64_t registers[8+1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Utility macros and functions
#define str(s) #s
#define xstr(s) str(s)

#define scan_print_output_1(input_, scanformat_, printformat_, output1_, print1_) \
({ \
	int ret__ = 0; \
	if (NULL != input_ && input_[0] != 0) \
	{ \
		ret__ = sscanf(input_, scanformat_, output1_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_); \
		} \
	} \
	if (ret__ < 1) \
	{ \
		ret__ = scanf(scanformat_, output1_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_); \
		} \
	} \
	ret__; \
})

#define scan_print_output_2(input_, scanformat_, printformat_, output1_, output2_, print1_, print2_) \
({ \
	int ret__ = 0; \
	if (NULL != input_ && input_[0] != 0) \
	{ \
		ret__ = sscanf(input_, scanformat_, output1_, output2_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_, print2_); \
		} \
	} \
	if (ret__ < 1) \
	{ \
		ret__ = scanf(scanformat_, output1_, output2_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_, print2_); \
		} \
	} \
	ret__; \
})

#define scan_print_output_3(input_, scanformat_, printformat_, output1_, output2_, output3_, print1_, print2_, print3_) \
({ \
	int ret__ = 0; \
	if (NULL != input_ && input_[0] != 0) \
	{ \
		ret__ = sscanf(input_, scanformat_, output1_, output2_, output3_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_, print2_, print3_); \
		} \
	} \
	if (ret__ < 1) \
	{ \
		ret__ = scanf(scanformat_, output1_, output2_, output3_); \
		if (!isatty(0)) \
		{ \
			printf(printformat_ "\n", print1_, print2_, print3_); \
		} \
	} \
	ret__; \
})

static int read_filename(char * filename, const char * message)
{
	int ret;
	filename[0] = 0;
	printf("Enter filename%s: ", message);
	ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", filename, filename);
	return ret;
}

static int read_varname(CalcHandle* h, VarRequest *vr, const char *prompt)
{
	char buf[256];
	const char *s;
	int ret = 0;
	char * endptr;

	memset(vr, 0, sizeof(VarRequest));

	if (ticalcs_calc_features(h) & FTS_FOLDER)
	{
		printf("Enter%s folder name: ", prompt);
		ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", vr->folder, vr->folder);
		if (ret < 1)
		{
			fputs("Missing parameters\n", stderr);
			return 1;
		}
	}

	printf("Enter%s variable name: ", prompt);
	ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", vr->name, vr->name);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = 0;
	if (ticonv_model_is_tiz80(h->model) || ticonv_model_is_tiez80(h->model))
	{
		printf("Enter%s variable type: ", prompt);
		ret = scan_print_output_1("", "%255s", "%s", buf, buf);
		if (ret < 1)
		{
			fputs("Missing parameters\n", stderr);
			return 1;
		}

		ret = 0;
		// Special handling for types written in hex.
		errno = 0;
		vr->type = (uint8_t)strtoul(buf, &endptr, 16);

		if (errno != 0)
		{
			// The string doesn't seem to be a valid numeric value.
			// Let's try to parse a fext instead.
			vr->type = tifiles_fext2vartype(h->model, buf);
			s = tifiles_vartype2string(h->model, vr->type);
			if (s == NULL || *s == 0)
			{
				vr->type = tifiles_string2vartype(h->model, buf);
			}
		}
	}

	return ret;
}

static int build_raw_bytes_from_hex_string(char * buffer, uint32_t maxbuflen, unsigned char * data, uint32_t maxdatalen, uint32_t * length)
{
	int ret = 1;
	uint32_t i;
	uint8_t c = 0;
	int odd = 0;
	for (i = 0; i < maxbuflen; i++)
	{
		if (buffer[i] == 0)
		{
			//printf("Remaining \"%02X\"\n", c);
			if (odd)
			{
				puts("Odd number of nibbles in hex string, bailing out");
			}
			else
			{
				ret = 0;
			}
			break;
		}
		else if (buffer[i] >= '0' && buffer[i] <= '9')
		{
			c |= buffer[i] - '0';
		}
		else if (buffer[i] >= 'a' && buffer[i] <= 'f')
		{
			c |= buffer[i] - 'a' + 10;
		}
		else if (buffer[i] >= 'A' && buffer[i] <= 'F')
		{
			c |= buffer[i] - 'A' + 10;
		}
		else
		{
			continue;
		}
		if (odd)
		{
			if (*length < maxdatalen)
			{
				//printf("Serializing \"%02X\"\n", c);
				*data++ = c;
				odd = 0;
				c = 0;
			}
			else
			{
				printf("Reached max length of %lu bytes, bailing out\n", (unsigned long)maxdatalen);
				ret = 0;
				break;
			}
			(*length)++;
		}
		else
		{
			odd++;
			c <<= 4;
		}
	}

	return ret;
}

#define get_hex_input(buffer, bufferlen, data, datalen, outlength, message, maxstrlen) \
({ \
	int ret_ = 1; \
\
	buffer[0] = 0; \
	printf("Enter %s as hex string of up to %s bytes (non-hex characters ignored; CTRL+D to end):\n", message, maxstrlen); \
	ret_ = scan_print_output_1("", "%" maxstrlen "[^\x04]", "%s", buffer, buffer); \
	if (ret_ >= 1) \
	{ \
		if (fgetc(stdin) == EOF) {} /* Swallow a single Ctrl+D. */ \
		clearerr(stdin); \
\
		fputc('\n', stdout); \
		if (!build_raw_bytes_from_hex_string(buffer, bufferlen, data, datalen, outlength)) \
		{ \
			ret_ = 0; \
		} \
		else \
		{ \
			puts("Failed to build raw data, not dissected"); \
			ret_ = 1; \
		} \
	} \
	else \
	{ \
		*outlength = 0; \
	} \
\
	ret_; \
})

static int check_regno(unsigned int regno)
{
	if (!regno || regno >= (int)(sizeof(registers) / sizeof(registers[0])))
	{
		fputs("Invalid register number\n", stderr);
		return 1;
	}
	return 0;
}

static int parse_access_type(const char * datatype, unsigned int * endianness, unsigned int * bytes)
{
	if (datatype[0] == 'l' && datatype[1] == 'e')
	{
		*endianness = 0;
	}
	else if (datatype[0] == 'b' && datatype[1] == 'e')
	{
		*endianness = 1;
	}
	else
	{
		fputs("Unknown endianness\n", stderr);
		return 1;
	}

	if (datatype[2] == '0' && datatype[3] == '8')
	{
		*bytes = 1;
	}
	else if (datatype[2] == '1' && datatype[3] == '6')
	{
		*bytes = 2;
	}
	else if (datatype[2] == '3' && datatype[3] == '2')
	{
		*bytes = 4;
	}
	else if (datatype[2] == '6' && datatype[3] == '4')
	{
		*bytes = 8;
	}
	else
	{
		fputs("Unknown size\n", stderr);
		return 1;
	}

	return 0;
}

static int parse_check_uint32(const char * input, uint32_t * val, uint32_t maxval)
{
	int ret;
	unsigned int regno;

	if (nullptr == input)
	{
		return 1;
	}
	ret = sscanf(input, "r%u", &regno);
	if (ret < 1)
	{
		ret = !sscanf(input, "%" SCNi32, val);
		/*if (!ret)
		{
			printf("Found valid value %" PRIu32, *val);
		}*/
	}
	else
	{
		ret = check_regno(regno);
		if (!ret)
		{
			//printf("Found valid register %u\n", regno);
			*val = (uint32_t)registers[regno];
		}
	}

	if (!ret)
	{
		if (*val >= maxval)
		{
			fputs("Value too large\n", stderr);
			ret = 1;
		}
	}

	if (!ret)
	{
		printf("Valid value is %" PRIx32 " (%" PRIu32 ")\n", *val, *val);
	}
	else
	{
		fputs("Invalid value\n", stderr);
	}

	return ret;
}

static int parse_check_uint64(const char * input, uint64_t * val)
{
	int ret;
	unsigned int regno;

	if (nullptr == input)
	{
		return 1;
	}
	ret = sscanf(input, "r%u", &regno);
	if (ret < 1)
	{
		ret = !sscanf(input, "%" SCNi64, val);
		/*if (!ret)
		{
			printf("Found valid value %" PRIu64, *val);
		}*/
	}
	else
	{
		ret = check_regno(regno);
		if (!ret)
		{
			//printf("Found valid register %u\n", regno);
			*val = registers[regno];
		}
	}

	if (!ret)
	{
		printf("Valid value is %" PRIx64 " (%" PRIu64 ")\n", *val, *val);
	}
	else
	{
		fputs("Invalid value\n", stderr);
	}

	return ret;
}

static int parse_check_register(const char * input, unsigned int * regno)
{
	int ret;

	*regno = 0;
	ret = sscanf(input, "r%u", regno);
	if (ret == 1)
	{
		ret = check_regno(*regno);
	}
	else
	{
		fputs("Missing register\n", stderr);
		ret = 1;
	}

	if (!ret)
	{
		printf("Valid register is r%u", *regno);
	}

	return ret;
}

static int parse_reg_commands_args(char * input, uint64_t * val, uint32_t * regno)
{
	int ret;
	char * token;
	char * saveptr = NULL;

	token = STRTOK(input, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 1\n", stderr);
		return 1;
	}

	ret = parse_check_uint64(token, val);
	if (ret) return 1;

	token = STRTOK(NULL, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 2\n", stderr);
		return 1;
	}

	ret = parse_check_register(token, regno);
	if (ret) return 1;

	puts("Good");
	return ret;
}

static void hexdump(uint8_t * ptr, uint32_t length)
{
	fputs("\n\t", stdout);
	for (uint32_t i = 0; i < length;)
	{
		printf("%02X ", *ptr++);
		if (!(++i & 15))
		{
			fputs("\n\t", stdout);
		}
	}
	putchar('\n');
}

static void print_lc_error(int errnum)
{
	char *msg = NULL;

	ticables_error_get(errnum, &msg);
	fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", errnum, msg);
	ticables_error_free(msg);
}


// Worker functions
typedef int (*FNCT_MENU) (CalcHandle*, int, char *);

static int cfg_get(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int cfg_set(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int sleep(CalcHandle * h, int, char * input)
{
	int ret;
	unsigned int delay;

	printf("Enter delay in tenths of seconds: ");
	ret = scan_print_output_1(input, "%u", "%u", &delay, delay);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}
	ret = 0;

	if (delay >= 10)
	{
		sleep(delay / 10);
	}
	delay %= 10;
	if (delay != 0)
	{
		usleep(100000 * delay);
	}

	return ret;
}

static int timeout(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int delay(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int probe_calc(CalcHandle * h, int, char * input)
{
	int m = 0;
	int p = 0;
	int ret;
	CalcModel model = CALC_NONE;

	printf("Enter cable & port for probing (c p): ");
	ret = scan_print_output_2(input, "%d %d", "%d %d", &m, &p, m, p);
	if (ret < 2)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = ticalcs_probe((CableModel)m, (CablePort)p, &model, !0);
	//ticalcs_probe_calc(h->cable, &model);
	//ticalcs_probe_usb_calcs(h->cable, &model);
	printf("Found: <%s>\n", ticalcs_model_to_string(model));

	return ret;
}

static int is_ready(CalcHandle * h, int, char *)
{
	int ret;

	ret = ticalcs_calc_isready(h);
	printf("%d Hand-held is %sready !\n", ret, ret ? "not " : "");

	return ret;
}

static int send_key(CalcHandle * h, int, char * input)
{
	int ret;
	unsigned int key_value;

	printf("Enter key value to be sent: ");
	ret = scan_print_output_1(input, "%u", "%u", &key_value, key_value);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	return ticalcs_calc_send_key(h, key_value);
}

static int execute(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_execute(h, &ve, "");
	}

	return ret;
}

static int recv_screen(CalcHandle * h, int, char *)
{
	int ret;
	CalcInfos infos;

	memset((void *)&infos, 0, sizeof(infos));
	ret = ticalcs_calc_get_version(h, &infos);
	if (!ret)
	{
		CalcScreenCoord sc = { SCREEN_CLIPPED, 0, 0, 0, 0, CALC_PIXFMT_MONO };
		if (infos.bits_per_pixel == 1)
		{
			// Do nothing, sc already contains CALC_PIXFMT_MONO.
		}
		else if (infos.bits_per_pixel == 4)
		{
			sc.pixel_format = CALC_PIXFMT_GRAY_4;
		}
		else if (infos.bits_per_pixel == 16)
		{
			sc.pixel_format = CALC_PIXFMT_RGB_565_LE;
		}
		else
		{
			fprintf(stderr, "Unsupported bpp depth\n");
			ret = 1;
		}
		if (!ret)
		{
			uint8_t* bitmap = NULL;
			ret = ticalcs_calc_recv_screen_rgb888(h, &sc, &bitmap);
			if (!ret)
			{
				char filename[FILENAME_DATA_SIZE + 7];
				ret = read_filename(filename, " for outputting raw RGB888 pixels");
				if (ret < 1)
				{
					ret = 1;
				}
				else
				{
					ret = 1;
					FILE * f = fopen(filename, "w+b");
					if (NULL != f)
					{
						unsigned int count = sc.width * sc.height * 3;
						if (count > fwrite((void *)bitmap, 1, sc.width * sc.height * 3, f))
						{
							fprintf(stderr, "Error writing bitmap data\n");
						}
						else
						{
							ret = 0;
						}
						fclose(f);
					}
				}
				ticalcs_free_screen(bitmap);
			}
		}
	}

	return ret;
}

static int get_dirlist(CalcHandle * h, int, char *)
{
	int ret;
	GNode *vars, *apps;

	ret = ticalcs_calc_get_dirlist(h, &vars, &apps);
	if (!ret)
	{
		ticalcs_dirlist_display(vars);
		ticalcs_dirlist_display(apps);
		ticalcs_dirlist_destroy(&vars);
		ticalcs_dirlist_destroy(&apps);
	}

	return ret;
}

static int get_memfree(CalcHandle * h, int, char *)
{
	int ret;
	uint32_t ram, flash;

	ret = ticalcs_calc_get_memfree(h, &ram, &flash);
	if (!ret)
	{
		printf("RAM: %lu\tFlash: %lu\n", (unsigned long)ram, (unsigned long)flash);
	}

	return ret;
}

static int send_backup(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];

	ret = read_filename(filename, "");
	if (ret < 1)
	{
		return 1;
	}

	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	ret = ticalcs_calc_send_backup2(h, filename);

	return ret;
}

static int recv_backup(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];

	ret = read_filename(filename, " for backup");
	if (ret < 1)
	{
		return 1;
	}

	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	ret = ticalcs_calc_recv_backup2(h, filename);

	return ret;
}

static int send_var(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	FileContent *content;
	VarEntry ve;

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = 0;
	if (h->model == CALC_NSPIRE)
	{
		memset(&ve, 0, sizeof(ve));
		if (!read_varname(h, &ve, ""))
		{
			return 1;
		}
	}

	content = tifiles_content_create_regular(h->model);
	if (content != NULL)
	{
		ret = tifiles_file_read_regular(filename, content);
		if (!ret)
		{
			if (h->model == CALC_NSPIRE)
			{
				strncpy(content->entries[0]->folder, ve.folder, sizeof(content->entries[0]->folder) - 1);
				content->entries[0]->folder[sizeof(content->entries[0]->folder) - 1] = 0;
				strncpy(content->entries[0]->name, ve.name, sizeof(content->entries[0]->name) - 1);
				content->entries[0]->name[sizeof(content->entries[0]->name) - 1] = 0;
			}
			ret = ticalcs_calc_send_var(h, MODE_NORMAL, content);
			tifiles_content_delete_regular(content);
		}
	}
	else
	{
		ret = ERR_MALLOC;
	}

	return ret;
}

static int recv_var(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	VarEntry ve;

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = 1;
	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_recv_var2(h, MODE_NORMAL, filename, &ve);
	}

	return ret;
}

static int send_var_ns(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = ticalcs_calc_send_var_ns2(h, MODE_NORMAL, filename);

	return ret;
}

static int recv_var_ns(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	VarRequest *ve;

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = ticalcs_calc_recv_var_ns2(h, MODE_NORMAL, filename, &ve);

	return ret;
}

static int send_app(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];

	ret = read_filename(filename, " for app");
	if (ret < 1)
	{
		return 1;
	}

	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_flash_app(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	ret = ticalcs_calc_send_app2(h, filename);

	return ret;
}

static int recv_app(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];
	VarEntry ve;

	ret = read_filename(filename, " for app");
	if (ret < 1)
	{
		return 1;
	}

	memset(&ve, 0, sizeof(ve));
	printf("Enter application name: ");
	ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", ve.name, ve.name);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		ret = 1;
	}
	else
	{
		ret = ticalcs_calc_recv_app2(h, filename, &ve);
	}

	return ret;
}

static int send_os(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for OS");
	if (ret < 1)
	{
		return 1;
	}

	ret = ticalcs_calc_send_os2(h, filename);

	return ret;
}

static int recv_idlist(CalcHandle * h, int, char *)
{
	int ret;
	uint8_t id[32];

	ret = ticalcs_calc_recv_idlist(h, id);
	if (!ret)
	{
		printf("IDLIST: <%s>\n", id);
	}

	return ret;
}

static int dump_rom(CalcHandle * h, int, char *)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for ROM / Flash dump");
	if (ret < 1)
	{
		return 1;
	}

	ret = ticalcs_calc_dump_rom_1(h);
	if (!ret)
	{
		ret = ticalcs_calc_dump_rom_2(h, ROMSIZE_AUTO, filename);
	}

	return ret;
}

static int set_clock(CalcHandle * h, int, char *)
{
	int ret;
	CalcClock clk;

	ret = ticalcs_calc_get_clock(h, &clk);
	if (!ret)
	{
		ret = ticalcs_calc_set_clock(h, &clk);
	}

	return ret;
}

static int get_clock(CalcHandle * h, int, char *)
{
	int ret;
	CalcClock clk;

	ret = ticalcs_calc_get_clock(h, &clk);
	if (!ret)
	{
		ticalcs_clock_show(h->model, &clk);
	}

	return ret;
}

static int del_var(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_del_var(h, &ve);
	}

	return ret;
}

static int new_folder(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!ticonv_model_is_tiz80(h->model) && !ticonv_model_is_tiez80(h->model))
	{
		printf("Enter folder name: ");
		ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", ve.folder, ve.folder);
		if (ret < 1)
		{
			fputs("Missing parameters\n", stderr);
			ret = 1;
		}
		else
		{
			ret = ticalcs_calc_new_fld(h, &ve);
		}
	}

	return ret;
}

static int get_version(CalcHandle * h, int, char *)
{
	int ret;
	CalcInfos infos;
	char str[2048];

	memset((void *)&infos, 0, sizeof(infos));
	ret = ticalcs_calc_get_version(h, &infos);
	if (!ret)
	{
		str[0] = 0;
		ret = ticalcs_infos_to_string(&infos, str, sizeof(str));
		if (!ret)
		{
			puts(str);
		}
	}

	return ret;
}

static int send_cert(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int recv_cert(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int rename_var(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry src;
	VarEntry dst;

	memset(&src, 0, sizeof(src));
	if (read_varname(h, &src, " current"))
	{
		memset(&dst, 0, sizeof(dst));
		if (read_varname(h, &dst, " new"))
		{
			ret = ticalcs_calc_rename_var(h, &src, &dst);
		}
	}

	return ret;
}

static int archive_var(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_change_attr(h, &ve, ATTRB_ARCHIVED);
	}

	return ret;
}

static int unarchive_var(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_change_attr(h, &ve, ATTRB_NONE);
	}

	return ret;
}

static int lock_var(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (read_varname(h, &ve, ""))
	{
		ret = ticalcs_calc_change_attr(h, &ve, ATTRB_LOCKED);
	}

	return ret;
}

static int send_all_vars_backup(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int recv_all_vars_backup(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int send_lab_equipment_data(CalcHandle * h, int, char * input)
{
	int ret;
	const uint8_t * ptr;
	uint8_t * orig_ptr;
	CalcLabEquipmentData lab_equipment_data;
	int model;
	unsigned int vartype;

	printf("Enter calc model (usually 1-12, 16-17) and variable type (4 for lists): ");
	ret = scan_print_output_2(input, "%d %u", "%d %u", &model, &vartype, model, vartype);
	if (ret < 2)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	if (vartype != 4)
	{
		fputs("Variable type not supported for now\n", stderr);
		return 1;
	}

	printf("Enter list data as '{...}': \n");
	orig_ptr = (uint8_t *)inbuf + 1024 + 4;
	ptr = (const uint8_t *)fgets((char *)orig_ptr, 1024, stdin);

	ret = 1;
	if (NULL != ptr)
	{
		lab_equipment_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_STRING;
		lab_equipment_data.size = strlen((const char *)ptr) + 1;
		lab_equipment_data.items = 0;
		lab_equipment_data.data = ptr;
		lab_equipment_data.vartype = (uint8_t)vartype;
		ret = ticalcs_calc_send_lab_equipment_data(h, (CalcModel)model, &lab_equipment_data);
	}
	// else do nothing, we already set ret = 1 above.

	return ret;
}

static int get_lab_equipment_data(CalcHandle * h, int, char * input)
{
	int ret;
	CalcLabEquipmentData lab_equipment_data;
	int model;
	unsigned int vartype;

	printf("Enter calc model (usually 1-12, 16-17) and variable type (4 for lists): ");
	ret = scan_print_output_2(input, "%d %u", "%d %u", &model, &vartype, model, vartype);
	if (ret < 2)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	if (vartype != 4)
	{
		fputs("Variable type not supported for now\n", stderr);
		return 1;
	}

	memset((void *)&lab_equipment_data, 0, sizeof(lab_equipment_data));
	lab_equipment_data.vartype = (uint8_t)vartype;
	ret = ticalcs_calc_get_lab_equipment_data(h, (CalcModel)model, &lab_equipment_data);

	if (!ret)
	{
		printf("Received list data type %d size %" PRIu16 " vartype %" PRIu8 " items %" PRIu16 "\n", lab_equipment_data.type, lab_equipment_data.size, lab_equipment_data.vartype, lab_equipment_data.items);
		const uint8_t * ptr = lab_equipment_data.data;
		for (uint16_t i = 0; i < lab_equipment_data.size; i++)
		{
			printf("%02X ", ptr[i]);
		}
		putchar('\n');
	}
	while (0);

	return ret;
}

static int del_folder(CalcHandle * h, int, char *)
{
	int ret = 1;
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (ticonv_model_is_tinspire(h->model))
	{
		printf("Enter folder name: ");
		ret = scan_print_output_1("", "%" xstr(FILENAME_DATA_SIZE) "s", "%s", ve.folder, ve.folder);
		if (ret < 1)
		{
			fputs("Missing parameters\n", stderr);
			ret = 1;
		}
		else
		{
			ret = ticalcs_calc_del_fld(h, &ve);
		}
	}

	return ret;
}

static int buffer_set_data(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset = 0;
	uint32_t length = 0;

	printf("Enter offset: ");
	ret = parse_check_uint32(input, &offset, sizeof(pktdata) / 2);
	if (ret) return 1;

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata + offset, sizeof(pktdata) / 2, &length, "raw data", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		pktdata_len = offset + length;
	}

	return ret;
}

static int buffer_get_data(CalcHandle * h, int, char * input)
{
	int ret = 0;
	uint32_t length = pktdata_len;
	uint32_t offset = 0;
	char * token;
	char * saveptr = NULL;

	printf("Enter nothing, offset, or offset and length: ");
	token = STRTOK(input, " ", &saveptr);
	if (NULL != token)
	{
		// There's at least an offset.
		ret = parse_check_uint32(token, &offset, sizeof(pktdata));
		if (ret) return 1;

		token = STRTOK(NULL, " ", &saveptr);
		if (NULL != token)
		{
			// There's also a length.
			ret = parse_check_uint32(token, &length, sizeof(pktdata));
			if (ret) return 1;
		}
		// else do nothing, length was already initialized.
	}
	// else do nothing, offset and length were already initialized.

	if (offset + length >= sizeof(pktdata))
	{
		fputs("Offset + length is too large\n", stderr);
		return 1;
	}

	ret = 0;
	hexdump(pktdata + offset, length);

	return ret;
}

static int buffer_peek_data(CalcHandle * h, int, char * input)
{
	int ret;
	unsigned int endianness;
	unsigned int bytes;
	uint32_t offset;
	unsigned int regno;
	uint64_t val;
	char * token;
	char * saveptr = NULL;

	if (!pktdata_len)
	{
		fputs("No packet data is available, skipping\n", stderr);
		return 1;
	}

	// Parse input to retrieve access type (e.g. le32), offset, destination register.
	token = STRTOK(input, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 1\n", stderr);
		return 1;
	}

	ret = parse_access_type(token, &endianness, &bytes);
	if (ret) return 1;

	token = STRTOK(NULL, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 2\n", stderr);
		return 1;
	}

	ret = parse_check_uint32(token, &offset, sizeof(pktdata) - bytes);
	if (ret) return 1;

	token = STRTOK(NULL, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 3\n", stderr);
		return 1;
	}

	ret = parse_check_register(token, &regno);
	if (ret) return 1;

	puts("Good");

	val = 0;
	if (!endianness)
	{
		// Read LE: MSB are stored last.
		uint8_t * ptr = pktdata + offset + bytes;
		for (unsigned int i = 0; i < bytes; i++)
		{
			val |= *--ptr;
			if (i != bytes - 1)
			{
				val <<= 8;
			}
		}
	}
	else
	{
		// Read BE: MSB are stored first.
		uint8_t * ptr = pktdata + offset;
		for (unsigned int i = 0; i < bytes; i++)
		{
			val |= *ptr++;
			if (i != bytes - 1)
			{
				val <<= 8;
			}
		}
	}
	printf("val: %" PRIu64 " / %16" PRIx64 "\n", val, val);
	registers[regno] = val;

	return ret;
}

static int buffer_poke_data(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset;
	unsigned int endianness;
	unsigned int bytes;
	uint64_t val;
	char * token;
	char * saveptr = NULL;

	// Parse input to retrieve access type (e.g. le32), offset, destination register.
	token = STRTOK(input, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 1\n", stderr);
		return 1;
	}

	ret = parse_access_type(token, &endianness, &bytes);
	if (ret) return 1;

	token = STRTOK(NULL, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 2\n", stderr);
		return 1;
	}

	ret = parse_check_uint32(token, &offset, sizeof(pktdata) - bytes);
	if (ret) return 1;

	token = STRTOK(NULL, " ", &saveptr);
	if (NULL == token)
	{
		fputs("Missing parameter 3\n", stderr);
		return 1;
	}

	ret = parse_check_uint64(token, &val);
	if (ret) return 1;

	puts("Good");

	printf("val: %" PRIu64 " / %16" PRIx64 "\n", val, val);

	if (!endianness)
	{
		// Write LE: MSB are stored last.
		uint8_t * ptr = pktdata + offset;
		for (unsigned int i = 0; i < bytes; i++)
		{
			*ptr++ = (uint8_t)(val & 0xFF);
			val >>= 8;
		}
	}
	else
	{
		// Write BE: MSB are stored first.
		uint8_t * ptr = pktdata + offset + bytes;
		for (unsigned int i = 0; i < bytes; i++)
		{
			*--ptr = (uint8_t)(val & 0xFF);
			val >>= 8;
		}
	}

	// Update buffer size.
	if (offset > pktdata_len)
	{
		pktdata_len = offset + bytes;
	}

	return ret;
}

static int buffer_clear_data(CalcHandle * h, int, char * input)
{
	int ret = 0;

	memset((void *)pktdata, 0, sizeof(pktdata));
	pktdata_len = 0;

	return ret;
}

static /* __attribute__((always_inline)) */ int reg_op(CalcHandle * h, int, char * input, void (* op)(unsigned int regno, uint64_t val))
{
	int ret;
	uint64_t val;
	unsigned int regno;

	ret = parse_reg_commands_args(input, &val, &regno);
	if (ret) return 1;
	else
	{
		op(regno, val);
	}

	return ret;
}

static int reg_mov(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno]  = val; });
}

static int reg_add(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno] += val; });
}

static int reg_sub(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno] -= val; });
}

static int reg_and(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno] &= val; });
}

static int reg_or(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno] |= val; });
}

static int reg_xor(CalcHandle * h, int, char * input)
{
	return reg_op(h, 1, input, [](unsigned int regno, uint64_t val) { registers[regno] ^= val; });
}

static int raw_send_data(CalcHandle * h, int, char * input)
{
	int ret;

	ret = buffer_set_data(h, 1, (char *)"0");
	if (!ret)
	{
		printf("raw_send_data: %u\n", pktdata_len);
		ret = ticables_cable_send(ticalcs_cable_get(h), pktdata, pktdata_len);
	}

	return ret;
}

static int raw_send_data_from_buffer(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset = 0;
	uint32_t length = 0;
	char * token;
	char * saveptr = NULL;

	printf("Enter nothing, offset, or offset and length: ");
	token = STRTOK(input, " ", &saveptr);
	if (NULL != token)
	{
		// There's at least an offset.
		ret = parse_check_uint32(token, &offset, sizeof(pktdata) / 2);
		if (ret) return 1;

		token = STRTOK(NULL, " ", &saveptr);
		if (NULL != token)
		{
			// There's also a length.
			ret = parse_check_uint32(token, &length, sizeof(pktdata) / 2);
			if (ret) return 1;
		}
		// else do nothing, length was already initialized.
	}
	// else do nothing, offset and length were already initialized.

	ret = ticables_cable_send(ticalcs_cable_get(h), pktdata + offset, length);

	return ret;
}

static int raw_recv_data(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t length;

	printf("Enter length of raw data to be read: ");
	ret = parse_check_uint32(input, &length, sizeof(pktdata) / 2);
	if (ret) return 1;

	fprintf(stderr, "%u\n", length);
	ret = ticables_cable_recv(ticalcs_cable_get(h), pktdata, length);

	if (!ret)
	{
		pktdata_len = length;
		hexdump(pktdata, length);
	}

	return ret;
}

static int dbus_send_pkt(CalcHandle * h, int, char * input)
{
	int ret;
	uint8_t target = 0;
	uint8_t cmd = 0;
	uint16_t slen = 0;
	uint32_t length = 0;

	// Entering the length is necessary for e.g. the simple commands, see cmd68k.cc and cmdz80.cc.
	printf("Enter target (mid), cmd and length: ");
	ret = scan_print_output_3(input, "%" SCNi8 " %" SCNi8 " %" SCNi16, "%" PRIu8 " %" PRIu8 " %" PRIu16, &target, &cmd, &slen, target, cmd, slen);
	if (ret < 3)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	printf("%d\t%u\t%u\t%u\n", ret, target, cmd, slen);
	if (slen != 0)
	{
		ret = get_hex_input(inbuf, sizeof(inbuf), pktdata, sizeof(pktdata) / 2, &length, "raw data", xstr(INBUF_DATA_SIZE));
	}
	else
	{
		ret = 0;
		length = slen;
	}
	printf("%d\t%u\n", ret, length);
	if (!ret)
	{
		pktdata_len = length;
		if ((uint32_t)slen > length)
		{
			fputs("Note: declared length is larger than the given hex data\n", stderr);
		}
		// Special case for packets without data.
		ret = dbus_send(h, target, cmd, slen, slen == 0 ? NULL : pktdata);
		printf("%d\n", ret);
	}

	return ret;
}

static int dbus_recv_header(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset = 0;

	printf("Enter nothing or offset: ");
	ret = parse_check_uint32(input, &offset, sizeof(pktdata) / 2);
	// Don't need to test ret here, the argument is optional

	ret = dbus_recv_header(h, pktdata + offset, pktdata + offset + 1, (uint16_t *)(pktdata + offset + 2));
	if (!ret)
	{
		puts("Successfully received header");
		pktdata_len = offset + *(uint16_t *)(pktdata + offset + 2);
		hexdump(pktdata + offset, 4);
	}
	else
	{
		fputs("Failed to receive header\n", stderr);
	}

	return ret;
}

static int dbus_recv_data(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset = 0;
	uint32_t length = 0;
	uint16_t slength;
	char * token;
	char * saveptr = NULL;

	printf("Enter offset and length: ");
	token = STRTOK(input, " ", &saveptr);
	if (NULL != token)
	{
		// There's at least an offset.
		ret = parse_check_uint32(token, &offset, sizeof(pktdata));
		if (ret) return 1;

		token = STRTOK(NULL, " ", &saveptr);
		if (NULL != token)
		{
			// There's also a length.
			ret = parse_check_uint32(token, &length, sizeof(pktdata) / 2);
			if (ret) return 1;
		}
		else
		{
			fputs("Missing length\n", stderr);
			return 1;
		}
	}
	else
	{
		fputs("Missing offset\n", stderr);
		return 1;
	}

	if (offset + length >= sizeof(pktdata))
	{
		fputs("Offset + length is too large\n", stderr);
		return 1;
	}

	slength = (uint16_t)length;
	ret = dbus_recv_data(h, &slength, pktdata + offset);
	if (!ret)
	{
		puts("Successfully received data");
		pktdata_len = offset + slength;
		hexdump(pktdata + offset, slength);
	}
	else
	{
		fputs("Failed to receive data\n", stderr);
	}

	return ret;
}

static int dbus_recv(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t offset = 0;

	printf("Enter nothing or offset: ");
	ret = parse_check_uint32(input, &offset, sizeof(pktdata) / 2);
	// Don't need to test ret here, the argument is optional

	ret = dbus_recv(h, pktdata + offset, pktdata + offset + 1, (uint16_t *)(pktdata + offset + 2), pktdata + offset + 4);
	if (!ret)
	{
		puts("Successfully received header + data");
		pktdata_len = offset + *(uint16_t *)(pktdata + offset + 2);
		hexdump(pktdata + offset, pktdata_len - offset);
	}
	else
	{
		fputs("Failed to receive header + data\n", stderr);
	}

	return ret;
}

static int dusb_send_rpkt(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dusb_recv_rpkt(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int nsp_send_rpkt(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int nsp_recv_rpkt(CalcHandle * h, int, char *)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int rdmp_send_dumper(CalcHandle * h, int, char * input)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = rd_send_dumper2(h, filename);

	return ret;
}

static int rdmp_is_ready(CalcHandle * h, int, char * input)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for variable");
	if (ret < 1)
	{
		return 1;
	}

	ret = rd_is_ready(h);

	return ret;
}

static int rdmp_read_dump(CalcHandle * h, int, char * input)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 1];

	ret = read_filename(filename, " for output ROM dump");
	if (ret < 1)
	{
		return 1;
	}

	ret = rd_read_dump(h, filename);

	return ret;
}

static int dbus_dissect_pkt(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 1-12, 16-17): ");
	ret = scan_print_output_1(input, "%d", "%d", &model, model);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata2, sizeof(pktdata2), &length, "DBUS packet", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		ret = dbus_dissect((CalcModel)model, stderr, pktdata2, length);
	}

	return ret;
}

static int dusb_dissect_rpkt(CalcHandle * h, int, char * input)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	uint8_t first = 1; // Assume all packets are first packets.
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 13, 14, 18-21): ");
	ret = scan_print_output_1(input, "%d", "%d", &model, model);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata2, sizeof(((DUSBRawPacket *)0)->data), &length, "raw DUSB packet", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		ret = dusb_dissect((CalcModel)model, stderr, pktdata2, length, ep, &first);
	}

	return ret;
}

static int nsp_dissect_rpkt(CalcHandle * h, int, char * input)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 15): ");
	ret = scan_print_output_1(input, "%d", "%d", &model, model);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata2, sizeof(((NSPRawPacket *)0)->data), &length, "raw NSP packet", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		ret = nsp_dissect((CalcModel)model, stderr, pktdata2, length, ep);
	}

	return ret;
}

static int ti83p_dump(CalcHandle * h, int, char * input)
{
	int ret;
	char filename[FILENAME_DATA_SIZE + 7];
	int page;
	uint16_t length;
	FILE *f;

	filename[0] = 0;
	printf("Enter page number for dumping: ");
	ret = scan_print_output_1(input, "%d", "%d", &page, page);
	if (ret < 1)
	{
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = read_filename(filename, " for memory page dump");
	if (ret < 1)
	{
		return 1;
	}

	f = fopen(filename, "wb");
	if (f != NULL)
	{
		//memset(pktdata, 0, sizeof(pktdata));

		if (   (ti73_send_DUMP(h, (uint16_t)page) == 0)
		    && (ti73_recv_ACK(h, NULL) == 0)
		    && (ti73_recv_XDP(h, &length, pktdata) == 0)
		    && (ti73_send_ACK(h) == 0))
		{
			fwrite(pktdata, length, 1, f);
		}
		fclose(f);
	}

	return ret;
}

static int ti83p_eke(CalcHandle * h, int, char *)
{
	int ret;

	ret = ti73_send_EKE(h);
	if (!ret)
	{
		puts("EKE successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
		}
	}

	return ret;
}

static int ti83p_dke(CalcHandle * h, int, char *)
{
	int ret;

	ret = ti73_send_DKE(h);
	if (!ret)
	{
		puts("DKE successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
		}
	}

	return ret;
}

static int ti83p_eld(CalcHandle * h, int, char *)
{
	int ret;

	ret = ti73_send_ELD(h);
	if (!ret)
	{
		puts("ELD successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
		}
	}

	return ret;
}

static int ti83p_dld(CalcHandle * h, int, char *)
{
	int ret;

	ret = ti73_send_DLD(h);
	if (!ret)
	{
		puts("DLD successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
		}
	}

	return ret;
}

static int ti83p_gid(CalcHandle * h, int, char *)
{
	int ret;
	uint16_t length;

	ret = ti73_send_GID(h);
	if (!ret)
	{
		puts("GID successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
			ret = ti73_recv_XDP(h, &length, pktdata2);
			if (!ret)
			{
				puts("XDP received");
				ret = ti73_send_ACK(h);
				if (!ret)
				{
					puts("ACK received");
					for (uint16_t i = 0; i < length; i++)
					{
						printf("%02X ", pktdata2[i]);
					}
					putchar('\n');
				}
			}
		}
	}

	return ret;
}

static int ti83p_rid(CalcHandle * h, int, char *)
{
	int ret;
	uint16_t length;

	ret = ti73_send_RID(h);
	if (!ret)
	{
		puts("RID successfully sent");
		ret = ti73_recv_SID(h, &length, pktdata2);
		if (!ret)
		{
			puts("SID receive");
			ret = ti73_send_ACK(h);
			if (!ret)
			{
				puts("ACK received");
				for (uint16_t i = 0; i < length; i++)
				{
					printf("%02X ", pktdata2[i]);
				}
				putchar('\n');
			}
		}
	}

	return ret;
}

static int ti83p_sid(CalcHandle * h, int, char *)
{
	int ret;
	uint32_t length = 0;

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata, 32, &length, "raw data", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		pktdata_len = length;
		if (length >= 32)
		{
			ret = ti73_send_SID(h, pktdata);
			if (!ret)
			{
				puts("SID successfully sent");
				ret = ti73_recv_ACK(h, NULL);
				if (!ret)
				{
					puts("ACK successfully received");
				}
			}
		}
		else
		{
			fputs("Insufficient length\n", stderr);
		}
	}

	return ret;
}

static int dusb_get_param_ids(CalcHandle * h, int, char *)
{
	int ret;
	uint32_t length = 0;

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata2, 512 * 2, &length, "parameter IDs", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		if ((length & 1) == 0)
		{
			DUSBCalcParam **params;

			params = dusb_cp_new_array(h, length / 2);
			ret = dusb_cmd_s_param_request(h, length / 2, (uint16_t *)pktdata2);
			if (!ret)
			{
				ret = dusb_cmd_r_param_data(h, length / 2, params);
				if (!ret)
				{
					for (uint32_t i = 0; i < length / 2; i++)
					{
						printf("%04X\t%s\t%04X (%u)\n\t\t", params[i]->id, params[i]->ok ? "OK" : "NOK", params[i]->size, params[i]->size);
						if (params[i]->ok && params[i]->size > 0 && params[i]->data != NULL)
						{
							uint16_t j;
							uint8_t * ptr = params[i]->data;
							for (j = 0; j < params[i]->size;)
							{
								printf("%02X ", *ptr++);
								if (!(++j & 15))
								{
									printf("\n\t\t");
								}
							}
							putchar('\n');
						}
					}
				}
			}
			dusb_cp_del_array(h, params, length / 2);
		}
		else
		{
			puts("Odd number of bytes in hex string, bailing out");
		}
	}

	return ret;
}

static int dusb_set_param_id(CalcHandle * h, int, char * input)
{
	int ret;
	uint32_t length = 0;
	unsigned int param_id;

	uint8_t * data = (uint8_t *)dusb_cp_alloc_data(h, 2048);

	printf("Enter DUSB parameter ID to be set (usually < 0x60): ");
	ret = scan_print_output_1(input, "%u", "%u", &param_id, param_id);
	if (ret < 1)
	{
		dusb_cp_free_data(h, data);
		fputs("Missing parameters\n", stderr);
		return 1;
	}

	ret = get_hex_input(inbuf, sizeof(inbuf), pktdata2, sizeof(pktdata2), &length, "raw data", xstr(INBUF_DATA_SIZE));
	if (!ret)
	{
		DUSBCalcParam *param;

		param = dusb_cp_new_ex(h, param_id, length, data);
		ret = dusb_cmd_s_param_set(h, param);
		dusb_cp_del(h, param); // This frees data.

		if (!ret)
		{
			ret = dusb_cmd_r_data_ack(h);
		}
	}
	else
	{
		dusb_cp_free_data(h, data);
		puts("Failed to build raw data, not sent");
	}

	return ret;
}

static int nsp_send_key(CalcHandle * h, int, char *)
{
	int ret;
	static const uint8_t A[] = {97, 102, 0};
	static const uint8_t ONE[] = {49, 115, 0};
	static const uint8_t THREE[] = {51, 51, 0};
	static const uint8_t FIVE[] = {53, 82, 0};
	static const uint8_t SEVEN[] = {55, 113, 0};
	static const uint8_t NINE[] = {57, 49, 0};

	do
	{
		ret = ticalcs_calc_send_key(h, KEYNSP_HOME); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, A); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_ZERO); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, ONE); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_TWO); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, THREE); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_FOUR); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, FIVE); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_SIX); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, SEVEN); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_EIGHT); if (ret) break;
		ret = nsp_cmd_s_keypress_event(h, NINE); if (ret) break;
		ret = ticalcs_calc_send_key(h, KEYNSP_HOME);
	} while(0);

	return ret;
}

static int nsp_remote_mgmt(CalcHandle * h, int, char *)
{
	int ret;
	static uint8_t data[9] = { 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

	ret = is_ready(h, 1, NULL);
	if (!ret)
	{
		printf("Packet 1\n");
		ret = nsp_cmd_s_generic_data(h, sizeof(data) / sizeof(data[0]), data, NSP_SID_REMOTE_MGMT, 0x00);
		// Packets returned by the calculator have weird data nowadays.
		if (!ret || ret == ERR_INVALID_PACKET)
		{
			uint32_t size;
			uint8_t * data2 = NULL;
			printf("Packet 2\n");
			ret = nsp_cmd_r_generic_data(h, &size, &data2);
			g_free(data2);
		}
	}

	return ret;
}

static ticables_event_hook_type old_ticables_event_hook;
static ticalcs_event_hook_type old_ticalcs_event_hook;

static int cables_event_hook(CableHandle * handle, uint32_t event_count, const CableEventData * event, void * user_pointer)
{
	int ret = 0;

	if (NULL != handle && NULL != event)
	{
		// TODO
	}
	if (old_ticables_event_hook)
	{
		ret = old_ticables_event_hook(handle, event_count, event, user_pointer);
	}

	return ret;
}

static int calcs_event_hook(CalcHandle * handle, uint32_t event_count, const CalcEventData * event, void * user_pointer)
{
	int ret = 0;

	if (NULL != handle && NULL != event)
	{
		// TODO
	}
	if (old_ticalcs_event_hook)
	{
		ret = old_ticalcs_event_hook(handle, event_count, event, user_pointer);
	}

	return ret;
}

typedef struct
{
	const char * desc;
	const char * scriptable_name;
	uint32_t scriptable_name_len;
	FNCT_MENU fnct;
} menu_entry;

static menu_entry fnct_menu[] =
{
#define DEFINE_MENU_ENTRY(desc, method) \
	{ desc, #method, sizeof(#method) - 1, method }
// Used for signaling that a newline should be printed
#define NULL_ENTRY \
	{ NULL, NULL, 0, NULL }

	// IMPORTANT NOTES: for backwards compatibility, after a scriptable name was defined, it shall never be changed.
	{ "GEN  Exit now", "exit", 4, NULL },                                                      // 0
	{ "GEN  Exit if previous operation failed", "exit_if_failed", 14, NULL },
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("CFG  Get config value", cfg_get),
	DEFINE_MENU_ENTRY("CFG  Set config value", cfg_set),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("GEN  Sleep for tenths of seconds", sleep),
	DEFINE_MENU_ENTRY("GEN  Set timeout in tenths of seconds", timeout),                       // 5
	DEFINE_MENU_ENTRY("GEN  Set delay in us", delay),
	NULL_ENTRY,
	// Probing.
	DEFINE_MENU_ENTRY("GEN  Probe calc", probe_calc),
	NULL_ENTRY,
	// Front-ends for ticalcs_calc_*() which use function pointers in CalcFncts instances.
	DEFINE_MENU_ENTRY("FNCT Check whether calc is ready", is_ready),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send a key", send_key),
	DEFINE_MENU_ENTRY("FNCT Execute program", execute),                                        // 10
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Do a screenshot", recv_screen),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Get directory listing", get_dirlist),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Get available memory", get_memfree),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send backup", send_backup),
	DEFINE_MENU_ENTRY("FNCT Recv backup", recv_backup),                                        // 15
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send var", send_var),
	DEFINE_MENU_ENTRY("FNCT Recv var", recv_var),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send var (ns)", send_var_ns),
	DEFINE_MENU_ENTRY("FNCT Recv var (ns)", recv_var_ns),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send app", send_app),                                              // 20
	DEFINE_MENU_ENTRY("FNCT Recv app", recv_app),
	DEFINE_MENU_ENTRY("FNCT Send OS", send_os),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Get ID-LIST", recv_idlist),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Dump ROM", dump_rom),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Set clock", set_clock),                                            // 25
	DEFINE_MENU_ENTRY("FNCT Get clock", get_clock),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Delete var", del_var),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT New folder", new_folder),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Get version", get_version),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send certificate stuff", send_cert),                               // 30
	DEFINE_MENU_ENTRY("FNCT Recv certificate stuff", recv_cert),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Rename var", rename_var),
	// These three functions wrap ticalcs_calc_change_attr()
	DEFINE_MENU_ENTRY("FNCT Archive var", archive_var),
	DEFINE_MENU_ENTRY("FNCT Unarchive var", unarchive_var),
	DEFINE_MENU_ENTRY("FNCT Lock var", lock_var),                                              // 35
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send all variables backup", send_all_vars_backup),
	DEFINE_MENU_ENTRY("FNCT Recv all variables backup", recv_all_vars_backup),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Send({...}) (send list data)", send_lab_equipment_data),
	DEFINE_MENU_ENTRY("FNCT Get (get list data)", get_lab_equipment_data),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("FNCT Del folder", del_folder),                                          // 40
	NULL_ENTRY,
	// Front-ends for raw packet sending / receiving / manipulation functions.
	DEFINE_MENU_ENTRY("BUF  Enter data bytes into packet buffer", buffer_set_data),
	DEFINE_MENU_ENTRY("BUF  Dump data bytes from packet buffer", buffer_get_data),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("BUF  Peek data in the packet buffer", buffer_peek_data),
	DEFINE_MENU_ENTRY("BUF  Poke data in the packet buffer", buffer_poke_data),
	DEFINE_MENU_ENTRY("BUF  Clear packet buffer", buffer_clear_data),                          // 45
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("BUF  MOV register/value to register", reg_mov),
	DEFINE_MENU_ENTRY("BUF  ADD register/value to register", reg_add),
	DEFINE_MENU_ENTRY("BUF  SUB register/value to register", reg_sub),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("BUF  AND register/value to register", reg_and),
	DEFINE_MENU_ENTRY("BUF  OR register/value to register", reg_or),                           // 50
	DEFINE_MENU_ENTRY("BUF  XOR register/value to register", reg_xor),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("RAW  Send raw data bytes (interactive)", raw_send_data),
	DEFINE_MENU_ENTRY("RAW  Send raw data bytes from packet buffer", raw_send_data_from_buffer),
	DEFINE_MENU_ENTRY("RAW  Recv raw data bytes", raw_recv_data),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DBUS Send packet", dbus_send_pkt),                                      // 55
	DEFINE_MENU_ENTRY("DBUS Recv header", dbus_recv_header),
	DEFINE_MENU_ENTRY("DBUS Recv data", dbus_recv_data),
	DEFINE_MENU_ENTRY("DBUS Recv header+data", dbus_recv),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DUSB Send raw packet", dusb_send_rpkt),
	DEFINE_MENU_ENTRY("DUSB Recv raw packet", dusb_recv_rpkt),                                 // 60
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("NSP  Send raw packet", nsp_send_rpkt),
	DEFINE_MENU_ENTRY("NSP  Recv raw packet", nsp_recv_rpkt),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("RDMP Send ROM dumper", rdmp_send_dumper),
	DEFINE_MENU_ENTRY("RDMP Check whether ready", rdmp_is_ready),
	DEFINE_MENU_ENTRY("RDMP Read ROM dump", rdmp_read_dump),                                   // 65
	NULL_ENTRY,
	// Front-ends for dissection functions.
	DEFINE_MENU_ENTRY("DIS  Dissect DBUS raw packet", dbus_dissect_pkt),
	DEFINE_MENU_ENTRY("DIS  Dissect DUSB raw packet", dusb_dissect_rpkt),
	DEFINE_MENU_ENTRY("DIS  Dissect NSP raw packet", nsp_dissect_rpkt),
	NULL_ENTRY,
	// Front-ends for protocol-specific capabilities.
	DEFINE_MENU_ENTRY("DBUS 83+ Memory dump", ti83p_dump),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DBUS 83+ Enable key echo", ti83p_eke),                                  // 70
	DEFINE_MENU_ENTRY("DBUS 83+ Disable key echo", ti83p_dke),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DBUS 83+ Enable lockdown", ti83p_eld),
	DEFINE_MENU_ENTRY("DBUS 83+ Disable lockdown", ti83p_dld),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DBUS 83+ Get standard calculator ID", ti83p_gid),
	DEFINE_MENU_ENTRY("DBUS 83+ Get some 32-byte memory area", ti83p_rid),                     // 75
	DEFINE_MENU_ENTRY("DBUS 83+ Set some 32-byte memory area", ti83p_sid),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("DUSB Get parameter IDs", dusb_get_param_ids),
	DEFINE_MENU_ENTRY("DUSB Set parameter ID", dusb_set_param_id),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("NSP  Send key (specific and generic)", nsp_send_key),
	NULL_ENTRY,
	DEFINE_MENU_ENTRY("NSP  Test remote management stuff", nsp_remote_mgmt),                   // 80
	NULL_ENTRY,
#undef NULL_ENTRY
#undef DEFINE_MENU_ENTRY
};

int main(int argc, char **argv)
{
	CableModel cable_model = CABLE_NUL;
	CablePort port_number = PORT_1;
	CalcModel calc_model = CALC_NONE;
	CableHandle* cable = NULL;
	CalcHandle* calc = NULL;
	int err = 0, preverr = 0, i;
	unsigned int j;
	int do_exit = 0;
	char* colon;
	int do_probe = 1;
	int quiet = 0;
	int first = 1;

	while ((i = getopt(argc, argv, "c:m:nq")) != -1)
	{
		if (i == 'c')
		{
			colon = strchr(optarg, ':');
			if (colon)
			{
				*colon = 0;
				port_number = (CablePort)atoi(colon + 1);
			}
			cable_model = ticables_string_to_model(optarg);
		}
		else if (i == 'm')
		{
			calc_model = ticalcs_string_to_model(optarg);
		}
		else if (i == 'n')
		{
			do_probe = 0;
		}
		else if (i == 'q')
		{
			quiet = 1;
		}
		else
		{
			fprintf(stderr, "Usage: %s [-n (no probe)] [-c CABLE[:PORT]] [-m CALC]\n", argv[0]);
			return 1;
		}
	}

	// Force GLib 2.32+ to print info and debug messages like older versions did, unless this variable is already set.
	// No effect on earlier GLib versions.
	g_setenv("G_MESSAGES_DEBUG", "all", /* overwrite = */ FALSE);

	// init libs
	ticables_library_init();
	ticalcs_library_init();

	// set cable
	if (do_probe && cable_model == CABLE_NUL)
	{
		int ndevices;
		CableDeviceInfo *list;

		// This function is outdated because it yields inaccurate results for modern hardware, e.g. the TI-eZ80 series,
		// which uses the same USB PID as the 84+SE.
		//ticables_get_usb_devices(&pids, &npids);
		ticables_get_usb_device_info(&list, &ndevices);

		if (ndevices < 1)
		{
			fprintf(stderr, "No supported USB cable found\n");
			ticables_free_usb_device_info(list);
			goto end2;
		}

		if (ndevices > 1)
		{
			fprintf(stderr, "More than one device is not supported by test_ticalcs_2 at the moment\n");
			ticables_free_usb_device_info(list);
			goto end2;
		}

		if (list[0].family == CABLE_FAMILY_UNKNOWN)
		{
			fprintf(stderr, "Unknown cable\n");
			ticables_free_usb_device_info(list);
			goto end2;
		}

		cable_model = list[0].family == CABLE_FAMILY_DBUS ? CABLE_SLV : CABLE_USB;
		calc_model = ticalcs_device_info_to_model(&list[0]);

		ticables_free_usb_device_info(list);
	}

	printf("Cable is %d (%s), port is #%d\n", cable_model, ticables_model_to_string(cable_model), port_number);
	cable = ticables_handle_new(cable_model, port_number);
	if (cable == NULL)
	{
		fprintf(stderr, "ticables_handle_new failed\n");
		goto end;
	}
	old_ticables_event_hook = ticables_cable_set_event_hook(cable, cables_event_hook);

	// set calc
	if (do_probe && calc_model == CALC_NONE)
	{
		if (ticalcs_probe(cable_model, port_number, &calc_model, 1))
		{
			fprintf(stderr, "No calculator found\n");
			goto end;
		}
	}

	printf("Calc is %d (%s)\n", calc_model, ticalcs_model_to_string(calc_model));
	calc = ticalcs_handle_new(calc_model);
	if (calc == NULL)
	{
		fprintf(stderr, "ticalcs_handle_new failed\n");
		goto end;
	}
	old_ticalcs_event_hook = ticalcs_calc_set_event_hook(calc, calcs_event_hook);

	// attach cable to calc (and open cable)
	err = ticalcs_cable_attach(calc, cable);
	if (err != 0)
	{
		fprintf(stderr, "ticalcs_cable_attach failed %d\n", err);
		goto end;
	}

	ticables_options_set_timeout(cable, 50);

	do
	{
		char * ptr;
		unsigned int choice;
		// Display menu
		printf("Choose an action:\n");
		if (!quiet)
		{
			for (i = 0, j = 0; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
			{
				if (NULL != fnct_menu[i].desc)
				{
					printf("%2d. %-45s", j, fnct_menu[i].desc);
					j++;
				}
				else
				{
					putchar('\n');
				}
			}
			printf("Your choice: ");
		}

		inbuf2[0] = 0;
		ptr = fgets(inbuf2, 128, stdin);

		//printf("%d %02X\n", err, inbuf2[0]);
		if (NULL == ptr)
		{
/*			// Couldn't read a single char; try again, in a rate-limited way.
#ifdef __WIN32__
			Sleep(1000);
#else
			sleep(1);
#endif*/
			choice = 0;
		}
		else
		{
			// Swallow newline.
			if (inbuf2[strlen(inbuf2) - 1] == '\n')
			{
				inbuf2[strlen(inbuf2) - 1] = 0;
			}
			printf("%02X\t\"%s\"\n", inbuf2[0], inbuf2);

			// Special handling for some commands.
			if (first)
			{
				first = 0;
				if (!isatty(0))
				{
					if (!strncmp(inbuf2, "version ", sizeof("version ") - 1))
					{
						err = sscanf(inbuf2 + sizeof("version ") - 1, "%i", &version);
						if (err < 1 || ((int32_t)version < 0))
						{
							fputs("Invalid version command\n", stderr);
							choice = 0;
						}
						else if (version > CURRENT_SCRIPT_VERSION)
						{
							fputs("Script requests newer version than this client supports\n", stderr);
							choice = 0;
						}
						else
						{
							puts("Good version command");
							continue;
						}
					}
					else
					{
						fputs("Scripts must start with version command\n", stderr);
						choice = 0;
					}
				}
			}

			//printf("%d %02X %u\n", err, inbuf2[0], choice);
			if (inbuf2[0] == 0x00)
			{
				// Single newline, do nothing.
				continue;
				//choice = UINT_MAX;
			}
			else if (inbuf2[0] == 0x04 || !strcmp(inbuf2, "exit"))
			{
				// End of input ?
				choice = 0;
			}
			else if (!strcmp(inbuf2, "exit_if_failed"))
			{
				if (preverr != 0)
				{
					puts("The previous operation failed, exiting");
					choice = 0;
				}
			}
			else if (!strncmp(inbuf2, "version ", sizeof("version ") - 1))
			{
				fputs("Swallowing spurious version command\n", stderr);
				continue;
			}
			else
			{
				puts("Scanning");
				err = sscanf(inbuf2, "%u", &choice);
				if (err < 1)
				{
					for (i = 2, j = 2; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
					{
						// strstr and the second check are used to cope with names which are prefixes of others, e.g. send_var / send_var_ns,
						// while allowing for other parameters on the same line.
						if (NULL != fnct_menu[i].scriptable_name)
						{
							if (   (strstr(inbuf2, fnct_menu[i].scriptable_name) == inbuf2)
							    && (inbuf2[fnct_menu[i].scriptable_name_len] != '_')
							   )
							{
								choice = j;
								break;
							}
							j++;
						}
					}
				}
				// else an integer was successfully read, fall through. Bounds checking is handled below.
			}
		}

		if (choice == 0)
		{
			do_exit = 1;
		}
		else
		{
			printf("Choice is %u\n", choice);
			preverr = 0;
			// Process choice
			if (choice < sizeof(fnct_menu)/sizeof(fnct_menu[0]))
			{
				for (i = 2, j = 2; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
				{
					if (NULL != fnct_menu[i].fnct)
					{
						if (j == choice)
						{
							// Swallow function name
							printf("Will invoke %s\n", fnct_menu[i].scriptable_name);
							ptr = strchr(inbuf2, ' ');
							preverr = (fnct_menu[i].fnct)(calc, 1, nullptr == ptr ? ptr : ptr + 1);
							break;
						}
						j++;
					}
				}
			}
			if (preverr)
			{
				fprintf(stderr, "Function %d returned %d\n", choice, preverr);
				if (cfg_exit_on_failure)
				{
					fputs("Function failed and exit on failure enabled, bailing out\n", stderr);
					do_exit = 1;
				}
			}
		}
		printf("\n");
	} while (!do_exit);

	// detach cable (made by handle_del, too)
	err = ticalcs_cable_detach(calc);

end:
	// remove calc & cable
	ticalcs_handle_del(calc);
	ticables_handle_del(cable);

end2:
	ticalcs_library_exit();
	ticables_library_exit();

	return err;
}
