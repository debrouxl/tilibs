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

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

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
#include "../src/dusb_rpkt.h"
#include "../src/keysnsp.h"
#include "../src/nsp_rpkt.h"
#include "../src/error.h"

#undef VERSION
#define VERSION "Test program"

#define scan_print_output(scanformat_, printformat_, output_, print_) \
({ \
	int ret__ = scanf(scanformat_, output_); \
	if (!isatty(0)) \
	{ \
		printf(printformat_ "\n", print_); \
	} \
	ret__; \
})

#define FILENAME_DATA_SIZE 1023
#define str(s) #s
#define xstr(s) str(s)

static int read_filename(char * filename, const char * message)
{
	int ret;
	filename[0] = 0;
	printf("Enter filename%s: ", message);
	ret = scan_print_output("%" xstr(FILENAME_DATA_SIZE) "s", "%s", filename, filename);
	return ret;
}

static int read_varname(CalcHandle* h, VarRequest *vr, const char *prompt)
{
	char buf[256];
	const char *s;
	int ret = 1;
	char * endptr;

	memset(vr, 0, sizeof(VarRequest));

	if (ticalcs_calc_features(h) & FTS_FOLDER)
	{
		printf("Enter%s folder name: ", prompt);
		ret = scan_print_output("%1023s", "%s", vr->folder, vr->folder);
	}

	if (ret >= 1)
	{
		printf("Enter%s variable name: ", prompt);
		ret = scan_print_output("%1023s", "%s", vr->name, vr->name);
		if (ret >= 1)
		{
			if (ticonv_model_is_tiz80(h->model) || ticonv_model_is_tiez80(h->model))
			{
				printf("Enter%s variable type: ", prompt);
				ret = scan_print_output("%255s", "%s", buf, buf);
				if (ret < 1)
				{
					return 0;
				}

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
		}
	}

	return ret;
}

static int build_raw_bytes_from_hex_string(char * buffer, uint32_t maxbuflen, unsigned char * data, uint32_t maxdatalen, uint32_t * length)
{
	uint32_t i;
	uint8_t c = 0;
	int odd = 0;
	for (i = 0; i < maxbuflen; i++)
	{
		if (buffer[i] == 0)
		{
			if (odd)
			{
				puts("Odd number of nibbles in hex string, bailing out");
			}
			return odd;
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
				*data++ = c;
				odd = 0;
				c = 0;
			}
			else
			{
				printf("Reached max length of %lu bytes, bailing out\n", (unsigned long)maxdatalen);
				return 0;
			}
			(*length)++;
		}
		else
		{
			odd++;
			c <<= 4;
		}
	}
	return 1;
}

#define get_hex_input(buffer, bufferlen, data, datalen, outlength, message, maxstrlen) \
({ \
	int ret_ = 1; \
\
	buffer[0] = 0; \
	printf("Enter %s as hex string of up to %s bytes (non-hex characters ignored; CTRL+D to end):\n", message, maxstrlen); \
	ret_ = scan_print_output("%" maxstrlen "[^\x04]", "%s", buffer, buffer); \
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
\
	ret_; \
})

static void print_lc_error(int errnum)
{
	char *msg = NULL;

	ticables_error_get(errnum, &msg);
	fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", errnum, msg);
	ticables_error_free(msg);
}

typedef int (*FNCT_MENU) (CalcHandle*);

static int is_ready(CalcHandle* h)
{
	int ret;

	ret = ticalcs_calc_isready(h);
	printf("Hand-held is %sready !\n", ret ? "not " : "");

	return ret;
}

static int send_key(CalcHandle *h)
{
	int ret;
	unsigned int key_value;

	printf("Enter key value to be sent: ");
	ret = scan_print_output("%u", "%u", &key_value, key_value);
	if (ret < 1)
	{
		return 0;
	}

	return ticalcs_calc_send_key(h, key_value);
}

static int execute(CalcHandle *h)
{
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_execute(h, &ve, "");
}

static int recv_screen(CalcHandle *h)
{
	CalcInfos infos;
	int ret;

	memset((void *)&infos, 0, sizeof(infos));
	ret = ticalcs_calc_get_version(h, &infos);
	if (!ret)
	{
		CalcScreenCoord sc = { SCREEN_CLIPPED, 0, 0, 0, 0, CALC_PIXFMT_MONO };
		if (infos.bits_per_pixel == 1)
		{
			// Do nothing, sc already contains CALC_PIXFMT_MONO;
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
				if (ret >= 1)
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

static int get_dirlist(CalcHandle *h)
{
	GNode *vars, *apps;
	int ret;

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

static int get_memfree(CalcHandle *h)
{
	uint32_t ram, flash;
	int ret;

	ret = ticalcs_calc_get_memfree(h, &ram, &flash);
	if (!ret)
	{
		printf("RAM: %lu\tFlash: %lu\n", (unsigned long)ram, (unsigned long)flash);
	}

	return ret;
}

static int send_backup(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];
	int ret;

	ret = read_filename(filename, "");
	if (ret >= 1)
	{
		snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
		filename2[sizeof(filename2) - 1] = 0;

		return ticalcs_calc_send_backup2(h, filename);
	}
	return ret;
}

static int recv_backup(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];
	int ret;

	ret = read_filename(filename, " for backup");
	if (ret < 1)
	{
		snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
		filename2[sizeof(filename2) - 1] = 0;

		return ticalcs_calc_recv_backup2(h, filename);
	}
	return ret;
}

static int send_var(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;
	FileContent *content;
	VarEntry ve;

	ret = read_filename(filename, " for variable");
	if (ret >= 1)
	{
		if (h->model == CALC_NSPIRE)
		{
			memset(&ve, 0, sizeof(ve));
			if (!read_varname(h, &ve, ""))
			{
				return 0;
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
	}
	return ret;
}

static int recv_var(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;
	VarEntry ve;

	ret = read_filename(filename, " for variable");
	if (ret >= 1)
	{
		memset(&ve, 0, sizeof(ve));
		if (!read_varname(h, &ve, ""))
		{
			return 0;
		}

		return ticalcs_calc_recv_var2(h, MODE_NORMAL, filename, &ve);
	}
	return ret;
}

static int send_var_ns(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;

	ret = read_filename(filename, " for variable");
	if (ret >= 1)
	{
		return ticalcs_calc_send_var_ns2(h, MODE_NORMAL, filename);
	}
	return ret;
}

static int recv_var_ns(CalcHandle* h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;
	VarRequest *ve;

	ret = read_filename(filename, " for variable");
	if (ret >= 1)
	{
		return ticalcs_calc_recv_var_ns2(h, MODE_NORMAL, filename, &ve);
	}
	return ret;
}

static int send_app(CalcHandle *h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	char filename2[FILENAME_DATA_SIZE + 11];
	int ret;

	ret = read_filename(filename, " for app");
	if (ret >= 1)
	{
		snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_flash_app(h->model));
		filename2[sizeof(filename2) - 1] = 0;

		return ticalcs_calc_send_app2(h, filename);
	}
	return ret;
}

static int recv_app(CalcHandle *h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;
	VarEntry ve;

	ret = read_filename(filename, " for app");
	if (ret >= 1)
	{
		memset(&ve, 0, sizeof(ve));
		printf("Enter application name: ");
		ret = scan_print_output("%1023s", "%s", ve.name, ve.name);
		if (ret >= 1)
		{
			return ticalcs_calc_recv_app2(h, filename, &ve);
		}
	}
	return ret;
}

static int send_os(CalcHandle *h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;

	ret = read_filename(filename, " for OS");
	if (ret >= 1)
	{
		return ticalcs_calc_send_os2(h, filename);
	}
	return ret;
}

static int recv_idlist(CalcHandle *h)
{
	uint8_t id[32];
	int ret;

	ret = ticalcs_calc_recv_idlist(h, id);
	if (!ret)
	{
		printf("IDLIST: <%s>\n", id);
	}

	return ret;
}

static int dump_rom(CalcHandle *h)
{
	char filename[FILENAME_DATA_SIZE + 1];
	int ret;

	ret = read_filename(filename, " for ROM / Flash dump");
	if (ret >= 1)
	{
		ret = ticalcs_calc_dump_rom_1(h);
		if (!ret)
		{
			ret = ticalcs_calc_dump_rom_2(h, ROMSIZE_AUTO, filename);
		}
	}
	return ret;
}

static int set_clock(CalcHandle *h)
{
	CalcClock clk;
	int ret;

	ret = ticalcs_calc_get_clock(h, &clk);
	if (!ret)
	{
		ret = ticalcs_calc_set_clock(h, &clk);
	}

	return ret;
}

static int get_clock(CalcHandle *h)
{
	CalcClock clk;
	int ret;

	ret = ticalcs_calc_get_clock(h, &clk);
	if (!ret)
	{
		ticalcs_clock_show(h->model, &clk);
	}

	return ret;
}

static int del_var(CalcHandle* h)
{
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_del_var(h, &ve);
}

static int new_folder(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if (!ticonv_model_is_tiz80(h->model) && !ticonv_model_is_tiez80(h->model))
	{
		printf("Enter folder name: ");
		ret = scan_print_output("%1023s", "%s", ve.folder, ve.folder);
		if (ret < 1)
		{
			return 0;
		}
	}

	return ticalcs_calc_new_fld(h, &ve);
}

static int get_version(CalcHandle *h)
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

static int send_cert(CalcHandle *h)
{
	int ret = -1;

	fprintf(stderr, "Not implemented yet\n");

	return ret;
}

static int recv_cert(CalcHandle *h)
{
	int ret = -1;

	fprintf(stderr, "Not implemented yet\n");

	return ret;
}

static int rename_var(CalcHandle* h)
{
	VarEntry src;
	VarEntry dst;

	memset(&src, 0, sizeof(src));
	if (!read_varname(h, &src, " current"))
	{
		return 0;
	}
	memset(&dst, 0, sizeof(dst));
	if (!read_varname(h, &dst, " new"))
	{
		return 0;
	}

	return ticalcs_calc_rename_var(h, &src, &dst);
}

static int archive_var(CalcHandle* h)
{
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_change_attr(h, &ve, ATTRB_ARCHIVED);
}

static int unarchive_var(CalcHandle* h)
{
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_change_attr(h, &ve, ATTRB_NONE);
}

static int lock_var(CalcHandle* h)
{
	VarEntry ve;

	memset(&ve, 0, sizeof(ve));
	if (!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_change_attr(h, &ve, ATTRB_LOCKED);
}

static int send_all_vars_backup(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented yet\n", stderr);

	return ret;
}

static int recv_all_vars_backup(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented yet\n", stderr);

	return ret;
}

static int probe_calc(CalcHandle *h)
{
	int m = 0;
	int p = 0;
	int ret;
	CalcModel model = CALC_NONE;

	printf("Enter cable & port for probing (c p): ");
	ret = scanf("%d %d", &m, &p);
	if (!isatty(0))
	{
		printf("%d %d", m, p);
	}
	if (ret < 2)
	{
		return 0;
	}

	ret = ticalcs_probe((CableModel)m, (CablePort)p, &model, !0);
	//ticalcs_probe_calc(h->cable, &model);
	//ticalcs_probe_usb_calcs(h->cable, &model);
	printf("Found: <%s>\n", ticalcs_model_to_string(model));

	return ret;
}

static int raw_send_data(CalcHandle *h)
{
	int ret;
	char buffer[262144 + 12 + 2];
	uint8_t data[65536 + 6];
	uint32_t length = 0;

	ret = get_hex_input(buffer, sizeof(buffer), data, sizeof(data), &length, "raw data", "262156");
	if (!ret)
	{
		ret = ticables_cable_send(ticalcs_cable_get(h), data, length);
	}

	return ret;
}

static int raw_recv_data(CalcHandle *h)
{
	int ret;
	uint32_t length;
	uint8_t data[65536 + 6];
	uint8_t * ptr;

	printf("Enter length of raw data to be read: ");
	ret = scan_print_output("%u", "%u", &length, length);
	if (ret < 1)
	{
		return 0;
	}
	if ((int32_t)length < 0)
	{
		fputs("Negative length, skipping", stderr);
		return 0;
	}
	fprintf(stderr, "%u\n", length);
	ret = ticables_cable_recv(ticalcs_cable_get(h), data, length);

	fputs("\n\t", stdout);
	ptr = data;
	for (uint32_t i = 0; i < length;)
	{
		printf("%02X ", *ptr++);
		if (!(++i & 15))
		{
			fputs("\n\t", stdout);
		}
	}
	putchar('\n');

	return ret;
}

static int dbus_send_pkt(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dbus_recv_header(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dbus_recv_data(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dusb_send_rpkt(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dusb_recv_rpkt(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int nsp_send_rpkt(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int nsp_recv_rpkt(CalcHandle *h)
{
	int ret = -1;

	fputs("Not implemented\n", stderr);

	return ret;
}

static int dbus_dissect_pkt(CalcHandle *h)
{
	int ret;
	char buffer[262144 + 12 + 2];
	uint8_t data[65536 + 6];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 1-12, 16-17): ");
	ret = scan_print_output("%d", "%d", &model, model);
	if (ret < 1)
	{
		return 0;
	}

	ret = get_hex_input(buffer, sizeof(buffer), data, sizeof(data), &length, "DBUS packet", "262156");
	if (!ret)
	{
		ret = dbus_dissect((CalcModel)model, stderr, data, length);
	}

	return ret;
}

static int dusb_dissect_rpkt(CalcHandle *h)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	uint8_t first = 1; // Assume all packets are first packets.
	char buffer[4096 + 2];
	uint8_t data[sizeof(((DUSBRawPacket *)0)->data)];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 13, 14, 18-21): ");
	ret = scan_print_output("%d", "%d", &model, model);
	if (ret < 1)
	{
		return 0;
	}

	ret = get_hex_input(buffer, sizeof(buffer), data, sizeof(data), &length, "raw DUSB packet", "4096");
	if (!ret)
	{
		ret = dusb_dissect((CalcModel)model, stderr, data, length, ep, &first);
	}

	return ret;
}

static int nsp_dissect_rpkt(CalcHandle *h)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	char buffer[1536 + 2];
	uint8_t data[sizeof(((NSPRawPacket *)0)->data)];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 15): ");
	ret = scan_print_output("%d", "%d", &model, model);
	if (ret < 1)
	{
		return 0;
	}

	ret = get_hex_input(buffer, sizeof(buffer), data, sizeof(data), &length, "raw NSP packet", "1536");
	if (!ret)
	{
		ret = nsp_dissect((CalcModel)model, stderr, data, length, ep);
	}

	return ret;
}

static int ti83p_dump(CalcHandle *h)
{
	char filename[FILENAME_DATA_SIZE + 7];
	uint8_t buffer[0x4100];
	int page;
	int ret;
	uint16_t length;
	FILE *f;

	filename[0] = 0;
	printf("Enter page number for dumping: ");
	ret = scan_print_output("%d", "%d", &page, page);
	if (ret < 1)
	{
		return 0;
	}

	ret = read_filename(filename, " for memory page dump");
	if (ret >= 1)
	{
		f = fopen(filename, "wb");
		if (f != NULL)
		{
			memset(buffer, 0, sizeof(buffer));

			if (   (ti73_send_DUMP(h, (uint16_t)page) == 0)
			    && (ti73_recv_ACK(h, NULL) == 0)
			    && (ti73_recv_XDP(h, &length, buffer) == 0)
			    && (ti73_send_ACK(h) == 0))
			{
				fwrite(buffer, length, 1, f);
			}
			fclose(f);
		}
	}

	return ret;
}

static int ti83p_eke(CalcHandle *h)
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

static int ti83p_dke(CalcHandle *h)
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

static int ti83p_eld(CalcHandle *h)
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

static int ti83p_dld(CalcHandle *h)
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

static int ti83p_gid(CalcHandle *h)
{
	uint8_t buffer[0x10];
	uint16_t length;
	int ret;

	ret = ti73_send_GID(h);
	if (!ret)
	{
		puts("GID successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received");
			ret = ti73_recv_XDP(h, &length, buffer);
			if (!ret)
			{
				puts("XDP received");
				ret = ti73_send_ACK(h);
				if (!ret)
				{
					puts("ACK received");
					for (uint16_t i = 0; i < length; i++)
					{
						printf("%02X ", buffer[i]);
					}
					putchar('\n');
				}
			}
		}
	}

	return ret;
}

static int ti83p_rid(CalcHandle *h)
{
	uint8_t buffer[0x30];
	uint16_t length;
	int ret;

	ret = ti73_send_RID(h);
	if (!ret)
	{
		puts("RID successfully sent");
		ret = ti73_recv_SID(h, &length, buffer);
		if (!ret)
		{
			puts("SID receive");
			ret = ti73_send_ACK(h);
			if (!ret)
			{
				puts("ACK received");
				for (uint16_t i = 0; i < length; i++)
				{
					printf("%02X ", buffer[i]);
				}
				putchar('\n');
			}
		}
	}

	return ret;
}

static int ti83p_sid(CalcHandle *h)
{
	static uint8_t DATA[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
	};
	int ret;

	ret = ti73_send_SID(h, DATA);
	if (!ret)
	{
		puts("SID successfully sent");
		ret = ti73_recv_ACK(h, NULL);
		if (!ret)
		{
			puts("ACK received successful");
		}
	}

	return ret;
}

#include "../src/dusb_cmd.h"

static int dusb_get_param_ids(CalcHandle * h)
{
	int ret;
	char buffer[4096 + 2];
	uint16_t data[512];
	uint32_t length = 0;

	ret = get_hex_input(buffer, sizeof(buffer), (uint8_t *)data, sizeof(data), &length, "parameter IDs", "4096");
	if (!ret)
	{
		if ((length & 1) == 0)
		{
			DUSBCalcParam **params;

			params = dusb_cp_new_array(h, length / 2);
			ret = dusb_cmd_s_param_request(h, length / 2, data);
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

static int dusb_set_param_id(CalcHandle * h)
{
	int ret;
	char buffer[4096 + 2];
	uint8_t * data;
	uint32_t length = 0;
	unsigned int param_id;

	data = (uint8_t *)dusb_cp_alloc_data(h, 2048);

	printf("Enter DUSB parameter ID to be set (usually < 0x60): ");
	ret = scan_print_output("%u", "%u", &param_id, param_id);
	if (ret < 1)
	{
		dusb_cp_free_data(h, data);
		return 0;
	}

	ret = get_hex_input(buffer, sizeof(buffer), data, sizeof(data), &length, "raw data", "4096");
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

static int nsp_send_key(CalcHandle *h)
{
	//static const uint8_t HOME[] = {0x00, 0xFD, 0x00};
	static const uint8_t A[] = {97, 102, 0};
	//static const uint8_t ZERO[] = {48, 80, 0};
	static const uint8_t ONE[] = {49, 115, 0};
	//static const uint8_t TWO[] = {50, 83, 0};
	static const uint8_t THREE[] = {51, 51, 0};
	//static const uint8_t FOUR[] = {52, 114, 0};
	static const uint8_t FIVE[] = {53, 82, 0};
	//static const uint8_t SIX[] = {54, 50, 0};
	static const uint8_t SEVEN[] = {55, 113, 0};
	//static const uint8_t EIGHT[] = {56, 81, 0};
	static const uint8_t NINE[] = {57, 49, 0};

	ticalcs_calc_send_key(h, KEYNSP_HOME);
	nsp_cmd_s_keypress_event(h, A);
	ticalcs_calc_send_key(h, KEYNSP_ZERO);
	nsp_cmd_s_keypress_event(h, ONE);
	ticalcs_calc_send_key(h, KEYNSP_TWO);
	nsp_cmd_s_keypress_event(h, THREE);
	ticalcs_calc_send_key(h, KEYNSP_FOUR);
	nsp_cmd_s_keypress_event(h, FIVE);
	ticalcs_calc_send_key(h, KEYNSP_SIX);
	nsp_cmd_s_keypress_event(h, SEVEN);
	ticalcs_calc_send_key(h, KEYNSP_EIGHT);
	nsp_cmd_s_keypress_event(h, NINE);
	ticalcs_calc_send_key(h, KEYNSP_HOME);

	return 0;
}

static int nsp_remote_mgmt(CalcHandle * h)
{
	int retval = 0;
	static uint8_t data[9] = { 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

	is_ready(h);

	printf("Packet 1\n");
	retval = nsp_cmd_s_generic_data(h, sizeof(data) / sizeof(data[0]), data, NSP_SID_REMOTE_MGMT, 0x00);
	// Packets returned by the calculator have weird data nowadays.
	if (!retval || retval == ERR_INVALID_PACKET)
	{
		uint32_t size;
		uint8_t * data2 = NULL;
		printf("Packet 2\n");
		retval = nsp_cmd_r_generic_data(h, &size, &data2);
		g_free(data2);
	}

	return retval;
}

static ticables_event_hook_type old_ticables_event_hook;
static ticalcs_event_hook_type old_ticalcs_event_hook;

static int cables_event_hook(CableHandle * handle, uint32_t event_count, const CableEventData * event, void * user_pointer)
{
	int retval = 0;
	if (NULL != handle && NULL != event)
	{
		// TODO
	}
	if (old_ticables_event_hook)
	{
		retval = old_ticables_event_hook(handle, event_count, event, user_pointer);
	}
	return retval;
}

static int calcs_event_hook(CalcHandle * handle, uint32_t event_count, const CalcEventData * event, void * user_pointer)
{
	int retval = 0;
	if (NULL != handle && NULL != event)
	{
		// TODO
	}
	if (old_ticalcs_event_hook)
	{
		retval = old_ticalcs_event_hook(handle, event_count, event, user_pointer);
	}
	return retval;
}

static struct
{
	const char * desc;
	const char * scriptable_name;
	FNCT_MENU fnct;
} fnct_menu[] =
{
#define DEFINE_MENU_ENTRY(desc, method) \
	{ desc, #method, method }

	// IMPORTANT NOTE: for backwards compatibility, after a scriptable name was defined, it shall never be changed.
	{ "Exit", "exit", NULL },                                              // 0
	// Front-ends for ticalcs_calc_*() which use function pointers in CalcFncts instances.
	DEFINE_MENU_ENTRY("Check whether calc is ready", is_ready),
	DEFINE_MENU_ENTRY("Send a key", send_key),
	DEFINE_MENU_ENTRY("Execute program", execute),
	DEFINE_MENU_ENTRY("Do a screenshot", recv_screen),
	DEFINE_MENU_ENTRY("Get directory listing", get_dirlist),
	DEFINE_MENU_ENTRY("Get available memory", get_memfree),
	DEFINE_MENU_ENTRY("Send backup", send_backup),
	DEFINE_MENU_ENTRY("Recv backup", recv_backup),
	DEFINE_MENU_ENTRY("Send var", send_var),
	DEFINE_MENU_ENTRY("Recv var", recv_var),                               // 10
	DEFINE_MENU_ENTRY("Send var (ns)", send_var_ns),
	DEFINE_MENU_ENTRY("Recv var (ns)", recv_var_ns),
	DEFINE_MENU_ENTRY("Send app", send_app),
	DEFINE_MENU_ENTRY("Recv app", recv_app),
	DEFINE_MENU_ENTRY("Send OS", send_os),
	DEFINE_MENU_ENTRY("Get ID-LIST", recv_idlist),
	DEFINE_MENU_ENTRY("Dump ROM", dump_rom),
	DEFINE_MENU_ENTRY("Set clock", set_clock),
	DEFINE_MENU_ENTRY("Get clock", get_clock),
	DEFINE_MENU_ENTRY("Delete var", del_var),                              // 20
	DEFINE_MENU_ENTRY("New folder", new_folder),
	DEFINE_MENU_ENTRY("Get version", get_version),
	DEFINE_MENU_ENTRY("Send certificate stuff", send_cert),
	DEFINE_MENU_ENTRY("Recv certificate stuff", recv_cert),
	DEFINE_MENU_ENTRY("Rename var", rename_var),
	DEFINE_MENU_ENTRY("Archive var", archive_var),     //
	DEFINE_MENU_ENTRY("Unarchive var", unarchive_var), // These three functions wrap ticalcs_calc_change_attr()
	DEFINE_MENU_ENTRY("Lock var", lock_var),           //
	DEFINE_MENU_ENTRY("Send all variables backup", send_all_vars_backup),
	DEFINE_MENU_ENTRY("Recv all variables backup", recv_all_vars_backup),  // 30
	// Probing.
	DEFINE_MENU_ENTRY("Probe calc", probe_calc),
	// Front-ends for raw packet sending functions.
	DEFINE_MENU_ENTRY("Send raw data bytes", raw_send_data),
	DEFINE_MENU_ENTRY("Recv raw data bytes", raw_recv_data),
	DEFINE_MENU_ENTRY("Send DBUS packet", dbus_send_pkt),
	DEFINE_MENU_ENTRY("Recv DBUS header", dbus_recv_header),
	DEFINE_MENU_ENTRY("Recv DBUS data", dbus_recv_data),
	DEFINE_MENU_ENTRY("Send DUSB raw packet", dusb_send_rpkt),
	DEFINE_MENU_ENTRY("Recv DUSB raw packet", dusb_recv_rpkt),
	DEFINE_MENU_ENTRY("Send NSP raw packet", nsp_send_rpkt),
	DEFINE_MENU_ENTRY("Recv NSP raw packet", nsp_recv_rpkt),               // 40
	// Front-ends for dissection functions.
	DEFINE_MENU_ENTRY("DBUS: dissect packet", dbus_dissect_pkt),
	DEFINE_MENU_ENTRY("DUSB: dissect raw packet", dusb_dissect_rpkt),
	DEFINE_MENU_ENTRY("NSP: dissect raw packet", nsp_dissect_rpkt),
	// Front-ends for protocol-specific capabilities.
	DEFINE_MENU_ENTRY("83+-family-specific memory dump", ti83p_dump),
	DEFINE_MENU_ENTRY("83+-family-specific enable key echo", ti83p_eke),
	DEFINE_MENU_ENTRY("83+-family-specific disable key echo", ti83p_dke),
	DEFINE_MENU_ENTRY("83+-family-specific enable lockdown", ti83p_eld),
	DEFINE_MENU_ENTRY("83+-family-specific disable lockdown", ti83p_dld),
	DEFINE_MENU_ENTRY("83+-family-specific get standard calculator ID", ti83p_gid),
	DEFINE_MENU_ENTRY("83+-family-specific retrieve some 32-byte memory area", ti83p_rid), // 50
	DEFINE_MENU_ENTRY("83+-family-specific set some 32-byte memory area", ti83p_sid),
	DEFINE_MENU_ENTRY("DUSB: get parameter IDs", dusb_get_param_ids),
	DEFINE_MENU_ENTRY("DUSB: set parameter ID", dusb_set_param_id),
	DEFINE_MENU_ENTRY("Nspire-specific and generic send key", nsp_send_key),
	DEFINE_MENU_ENTRY("NSP: test remote management stuff", nsp_remote_mgmt),

#undef DEFINE_MENU_ENTRY
};

int main(int argc, char **argv)
{
	CableModel cable_model = CABLE_NUL;
	CablePort port_number = PORT_1;
	CalcModel calc_model = CALC_NONE;
	CableHandle* cable = NULL;
	CalcHandle* calc = NULL;
	int err = 0, i;
	int do_exit = 0;
	char* colon;
	int do_probe = 1;

	while ((i = getopt(argc, argv, "c:m:n")) != -1)
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
			goto end;
		}

		if (ndevices > 1)
		{
			fprintf(stderr, "More than one device is not supported by test_ticalcs_2 at the moment\n");
			ticables_free_usb_device_info(list);
			goto end;
		}

		if (list[0].family == CABLE_FAMILY_UNKNOWN)
		{
			fprintf(stderr, "Unknown cable\n");
			ticables_free_usb_device_info(list);
			goto end;
		}

		cable_model = list[0].family == CABLE_FAMILY_DBUS ? CABLE_SLV : CABLE_USB;
		calc_model = ticalcs_device_info_to_model(&list[0]);

		ticables_free_usb_device_info(list);
	}

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
		char input[32 + 2];
		unsigned int choice;
		// Display menu
		printf("Choose an action:\n");
		for (i = 0; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
		{
			printf("%2d. %s\n", i, fnct_menu[i].desc);
		}
		printf("Your choice: ");

		err = scan_print_output("%32s", "%s", input, input);
		//printf("%d %02X\n", err, input[0]);
		if (err < 1)
		{
			// Couldn't read a single char; try again, in a rate-limited way.
#ifdef __WIN32__
			Sleep(1000);
#else
			sleep(1);
#endif
			continue;
		}
		else
		{
			//printf("%d %02X %u\n", err, input[0], choice);
			if (input[0] == 0x04)
			{
				// End of input ?
				choice = 0;
			}
			else
			{
				err = sscanf(input, "%u", &choice);
				if (err < 1)
				{
					for (i = 0; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
					{
						if (!strcmp(fnct_menu[i].scriptable_name, input))
						{
							choice = i;
							break;
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
			err = 0;
			// Process choice
			if (choice < sizeof(fnct_menu)/sizeof(fnct_menu[0]) && fnct_menu[choice].fnct)
			{
				err = (fnct_menu[choice].fnct)(calc);
			}
			if (err)
			{
				fprintf(stderr, "Function %d returned %d\n", choice, err);
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

	return err;
}
