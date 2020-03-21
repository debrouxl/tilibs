/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

static int read_varname(CalcHandle* h, VarRequest *vr, const char *prompt)
{
	char buf[256];
	const char *s;
	int ret;
	char * endptr;

	memset(vr, 0, sizeof(VarRequest));

	if (ticalcs_calc_features(h) & FTS_FOLDER)
	{
		printf("Enter%s folder name: ", prompt);
		ret = scanf("%1023s", vr->folder);
		if (ret < 1)
		{
			return 0;
		}
	}

	printf("Enter%s variable name: ", prompt);
	ret = scanf("%1023s", vr->name);
	if (ret < 1)
	{
		return 0;
	}

	if (tifiles_calc_is_ti8x(h->model))
	{
		printf("Enter%s variable type: ", prompt);
		ret = scanf("%255s", buf);
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

	return 1;
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
				printf("Odd number of nibbles in hex string, bailing out");
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
				printf("Reached max length, bailing out");
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

static void print_lc_error(int errnum)
{
	char *msg;

	ticables_error_get(errnum, &msg);
	fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", errnum, msg);

	free(msg);
}

typedef int (*FNCT_MENU) (CalcHandle*);

static int is_ready(CalcHandle* h)
{
	int ret;

	ret = ticalcs_calc_isready(h);
	printf("Hand-held is %sready !\n", ret ? "not " : "");

	return 0;
}

static int send_key(CalcHandle *h)
{
	int ret;
	unsigned int key_value;

	printf("Enter key value to be sent: ");
	ret = scanf("%i", &key_value);
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
	if(!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_execute(h, &ve, "");
}

static int recv_screen(CalcHandle *h)
{
	CalcScreenCoord sc = { SCREEN_CLIPPED, 0, 0, 0, 0, CALC_PIXFMT_MONO };
	uint8_t* bitmap = NULL;
	int ret;

	ret = ticalcs_calc_recv_screen(h, &sc, &bitmap);
	ticalcs_free_screen(bitmap);
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

static int send_backup(CalcHandle* h)
{
	char filename[1030];
	char filename2[1040];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}
	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	return ticalcs_calc_send_backup2(h, filename);
}

static int recv_backup(CalcHandle* h)
{
	char filename[1030];
	char filename2[1040];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}
	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_backup(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	return ticalcs_calc_recv_backup2(h, filename);
}

static int send_var(CalcHandle* h)
{
	char filename[1030];
	int ret;
	FileContent *content;
	VarEntry ve;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	if (h->model == CALC_NSPIRE)
	{
		memset(&ve, 0, sizeof(ve));
		if(!read_varname(h, &ve, ""))
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
		ret = -1;
	}

	return ret;
}

static int recv_var(CalcHandle* h)
{
	char filename[1030];
	int ret;
	VarEntry ve;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
	{
		return 0;
	}

	return ticalcs_calc_recv_var2(h, MODE_NORMAL, filename, &ve);
}

static int send_var_ns(CalcHandle* h)
{
	char filename[1030];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	return ticalcs_calc_send_var_ns2(h, MODE_NORMAL, filename);
}

static int recv_var_ns(CalcHandle* h)
{
	char filename[1030];
	int ret;
	VarRequest *ve;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	return ticalcs_calc_recv_var_ns2(h, MODE_NORMAL, filename, &ve);
}

static int send_flash(CalcHandle *h)
{
	char filename[1030];
	char filename2[1040];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}
	snprintf(filename2, sizeof(filename2) - 1, "%s.%s", filename, tifiles_fext_of_flash_app(h->model));
	filename2[sizeof(filename2) - 1] = 0;

	return ticalcs_calc_send_app2(h, filename);
}

static int recv_flash(CalcHandle *h)
{
	char filename[1030];
	int ret;
	VarEntry ve;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	memset(&ve, 0, sizeof(ve));
	printf("Enter application name: ");
	ret = scanf("%1023s", ve.name);
	if (ret < 1)
	{
		return 0;
	}

	return ticalcs_calc_recv_app2(h, filename, &ve);
}

static int send_os(CalcHandle *h)
{
	char filename[1030];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	return ticalcs_calc_send_os2(h, filename);
}

static int recv_idlist(CalcHandle *h)
{
	uint8_t id[32];
	int ret;

	ret = ticalcs_calc_recv_idlist(h, id);
	if (!ret)
	{
		printf("IDLIST: <%s>", id);
	}

	return ret;
}

static int dump_rom(CalcHandle *h)
{
	char filename[1030];
	int ret;

	filename[0] = 0;
	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

	ret = ticalcs_calc_dump_rom_1(h);
	if (!ret)
	{
		ret = ticalcs_calc_dump_rom_2(h, ROMSIZE_AUTO, filename);
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

static int new_folder(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if (!tifiles_calc_is_ti8x(h->model))
	{
		printf("Enter folder name: ");
		ret = scanf("%1023s", ve.folder);
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

	ret = ticalcs_calc_get_version(h, &infos);
	if (!ret)
	{
		str[0] = 0;
		ret = ticalcs_infos_to_string(&infos, str, sizeof(str));
		if (!ret)
		{
			printf("%s\n", str);
		}
	}

	return ret;
}

static int probe_calc(CalcHandle *h)
{
	int m;
	int p;
	int ret;
	CalcModel model;

	printf("Enter cable & port for probing (c p): ");
	ret = scanf("%d %d", &m, &p);
	if (ret < 2)
	{
		return 0;
	}

	ticalcs_probe((CableModel)m, (CablePort)p, &model, !0);
	//ticalcs_probe_calc(h->cable, &model);
	//ticalcs_probe_usb_calcs(h->cable, &model);
	printf("Found: <%s>", ticalcs_model_to_string(model));

	return 0;
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

static int ti83pfamily_dump(CalcHandle *h)
{
	char filename[1024];
	uint8_t buffer[0x4100];
	int page;
	int ret;
	uint16_t length;
	FILE *f;

	filename[0] = 0;
	printf("Enter page number for dumping: ");
	ret = scanf("%d", &page);
	if (ret < 1)
	{
		return 0;
	}

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if (ret < 1)
	{
		return 0;
	}

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

	return 0;
}

static int ti83pfamily_eke(CalcHandle *h)
{
	if (   (ti73_send_EKE(h) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0))
	{
		printf("EKE successfully sent\n");
	}

	return 0;
}

static int ti83pfamily_dke(CalcHandle *h)
{
	if (   (ti73_send_DKE(h) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0))
	{
		printf("DKE successfully sent\n");
	}

	return 0;
}

static int ti83pfamily_eld(CalcHandle *h)
{
	if (   (ti73_send_ELD(h) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0))
	{
		printf("ELD successfully sent\n");
	}

	return 0;
}

static int ti83pfamily_dld(CalcHandle *h)
{
	if (   (ti73_send_DLD(h) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0))
	{
		printf("DLD successfully sent\n");
	}

	return 0;
}

static int ti83pfamily_gid(CalcHandle *h)
{
	uint8_t buffer[0x10];
	uint16_t length;
	uint16_t i;

	if (   (ti73_send_GID(h) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0)
	    && (ti73_recv_XDP(h, &length, buffer) == 0)
	    && (ti73_send_ACK(h) == 0))
	{
		printf("GID successfully sent\n");
		for (i = 0; i < length; i++)
		{
			printf("%02X ", buffer[i]);
		}
		puts("\n");
	}

	return 0;
}

static int ti83pfamily_rid(CalcHandle *h)
{
	uint8_t buffer[0x30];
	uint16_t length;
	uint16_t i;

	if (   (ti73_send_RID(h) == 0)
	    && (ti73_recv_SID(h, &length, buffer) == 0)
	    && (ti73_send_ACK(h) == 0))
	{
		printf("RID/SID successful\n");
		for (i = 0; i < length; i++)
		{
			printf("%02X ", buffer[i]);
		}
		puts("\n");
	}

	return 0;
}

static int ti83pfamily_sid(CalcHandle *h)
{
	static uint8_t DATA[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
	};

	if (   (ti73_send_SID(h, DATA) == 0)
	    && (ti73_recv_ACK(h, NULL) == 0))
	{
		printf("SID successful\n");
	}

	return 0;
}

#include "../src/dusb_cmd.h"

static int dbus_dissect_pkt(CalcHandle *h)
{
	int ret;
	char buffer[262144 + 14];
	uint8_t data[65536 + 6];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 1-12, 16-17): ");
	ret = scanf("%d", &model);
	if (ret < 1)
	{
		return 0;
	}

	buffer[0] = 0;
	printf("Enter DBUS packet as hex string of up to 4 * max packet size (non-hex characters ignored; CTRL+D to end):\n");
	ret = scanf("%262156[^\x04]", buffer);
	if (ret < 1)
	{
		return 0;
	}

	fputc('\n', stdout);
	if (!build_raw_bytes_from_hex_string(buffer, sizeof(buffer) / sizeof(buffer[0]), data, sizeof(data) / sizeof(data[0]), &length))
	{
		if (dbus_dissect((CalcModel)model, stderr, data, length) == 0)
		{
			printf("Dissection successful\n");
		}
		else
		{
			printf("Dissection failed\n");
		}
	}
	else
	{
		printf("Failed to build raw data, not dissected\n");
	}

	return 0;
}

static int dusb_dissect_rpkt(CalcHandle *h)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	uint8_t first = 1; // Assume all packets are first packets.
	char buffer[65536 + 2];
	uint8_t data[sizeof(((DUSBRawPacket *)0)->data)];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 13, 14, 18-21): ");
	ret = scanf("%d", &model);
	if (ret < 1)
	{
		return 0;
	}

	buffer[0] = 0;
	printf("Enter raw DUSB packet as hex string of up to 4 * max raw packet size (non-hex characters ignored; CTRL+D to end):\n");
	ret = scanf("%65536[^\x04]", buffer);
	if (ret < 1)
	{
		return 0;
	}

	fputc('\n', stdout);
	if (!build_raw_bytes_from_hex_string(buffer, sizeof(buffer) / sizeof(buffer[0]), data, sizeof(data) / sizeof(data[0]), &length))
	{
		if (dusb_dissect((CalcModel)model, stderr, data, length, ep, &first) == 0)
		{
			printf("Dissection successful\n");
		}
		else
		{
			printf("Dissection failed\n");
		}
	}
	else
	{
		printf("Failed to build raw data, not dissected\n");
	}

	return 0;
}

static int nsp_dissect_rpkt(CalcHandle *h)
{
	int ret;
	uint8_t ep = 2; // Assume PC -> TI.
	char buffer[65536 + 2];
	uint8_t data[sizeof(((NSPRawPacket *)0)->data)];
	uint32_t length = 0;
	int model;

	printf("Enter calc model (usually 15): ");
	ret = scanf("%d", &model);
	if (ret < 1)
	{
		return 0;
	}

	buffer[0] = 0;
	printf("Enter raw NSP packet as hex string of up to 4 * max raw packet size (non-hex characters ignored; CTRL+D to end):\n");
	ret = scanf("%65536[^\x04]", buffer);
	if (ret < 1)
	{
		return 0;
	}

	fputc('\n', stdout);
	if (!build_raw_bytes_from_hex_string(buffer, sizeof(buffer) / sizeof(buffer[0]), data, sizeof(data) / sizeof(data[0]), &length))
	{
		if (nsp_dissect((CalcModel)model, stderr, data, length, ep) == 0)
		{
			printf("Dissection successful\n");
		}
		else
		{
			printf("Dissection failed\n");
		}
	}
	else
	{
		printf("Failed to build raw data, not dissected\n");
	}

	return 0;
}

static int dusb_get_param_ids(CalcHandle * h)
{
	int ret;
	char buffer[4096 + 2];
	uint16_t data[1024];
	uint32_t length = 0;

	buffer[0] = 0;
	printf("Enter hex string containing parameter IDs which will be requested to the calculator (non-hex characters ignored; CTRL+D to end):\n");
	ret = scanf("%4096[^\x04]", buffer);
	if (ret < 1)
	{
		return 0;
	}

	fputc('\n', stdout);
	if (!build_raw_bytes_from_hex_string(buffer, sizeof(buffer) / sizeof(buffer[0]), (uint8_t *)data, sizeof(data), &length))
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
					unsigned int i;
					for (i = 0; i < length / 2; i++)
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
			printf("Odd number of bytes in hex string, bailing out");
		}
	}
	else
	{
		printf("Failed to build raw data, not sent\n");
	}

	return 0;
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
	ret = scanf("%i", &param_id);
	if (ret < 1)
	{
		dusb_cp_free_data(h, data);
		return 0;
	}

	buffer[0] = 0;
	printf("Enter raw DUSB packet as hex string of up to 4 * max raw packet size (non-hex characters ignored; CTRL+D to end):\n");
	ret = scanf("%4096[^\x04]", buffer);
	if (ret < 1)
	{
		dusb_cp_free_data(h, data);
		return 0;
	}

	fputc('\n', stdout);
	if (!build_raw_bytes_from_hex_string(buffer, sizeof(buffer) / sizeof(buffer[0]), (uint8_t *)data, 2048, &length))
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
		printf("Failed to build raw data, not sent\n");
	}

	return 0;
}

static int test_nspire_remote_mgmt(CalcHandle * h)
{
	int retval = 0;
	uint8_t data[9] = { 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

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
	FNCT_MENU fnct;
} fnct_menu[] =
{
	{ "Exit", NULL },
	{ "Check whether calc is ready", is_ready },
	{ "Send a key", send_key },
	{ "Execute program", execute },
	{ "Do a screenshot", recv_screen },
	{ "Listing", get_dirlist },
	{ "Send backup", send_backup },
	{ "Recv backup", recv_backup },
	{ "Send var", send_var },
	{ "Recv var", recv_var },
	{ "Send var (ns)", send_var_ns },
	{ "Recv var (ns)", recv_var_ns },
	{ "Send flash", send_flash },
	{ "Recv flash", recv_flash },
	{ "Send OS", send_os },
	{ "Get ID-LIST", recv_idlist },
	{ "Dump ROM", dump_rom },
	{ "Set clock", set_clock },
	{ "Get clock", get_clock },
	{ "Delete var", del_var },
	{ "Rename var", rename_var },
	{ "Archive var", archive_var },
	{ "Unarchive var", unarchive_var },
	{ "Lock var", lock_var },
	{ "New folder", new_folder },
	{ "Get version", get_version },
	{ "Probe calc", probe_calc },
	{ "Nspire-specific and generic send key", nsp_send_key },
	{ "83+-family-specific memory dump", ti83pfamily_dump },
	{ "83+-family-specific enable key echo", ti83pfamily_eke },
	{ "83+-family-specific disable key echo", ti83pfamily_dke },
	{ "83+-family-specific enable lockdown", ti83pfamily_eld },
	{ "83+-family-specific disable lockdown", ti83pfamily_dld },
	{ "83+-family-specific get standard calculator ID", ti83pfamily_gid },
	{ "83+-family-specific retrieve some 32-byte memory area", ti83pfamily_rid },
	{ "83+-family-specific set some 32-byte memory area", ti83pfamily_sid },
	{ "DBUS: dissect packet", dbus_dissect_pkt },
	{ "DUSB: dissect raw packet", dusb_dissect_rpkt },
	{ "DUSB: get parameter IDs", dusb_get_param_ids },
	{ "DUSB: set parameter ID", dusb_set_param_id },
	{ "NSP: dissect raw packet", nsp_dissect_rpkt },
	{ "NSP: test remote management stuff", test_nspire_remote_mgmt },
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
	unsigned int choice;
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
		// Display menu
		printf("Choose an action:\n");
		for(i = 0; i < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])); i++)
		{
			printf("%2d. %s\n", i, fnct_menu[i].desc);
		}
		printf("Your choice: ");

		err = scanf("%u", &choice);
		if (err < 1)
		{
			continue;
		}
		printf("\n");

		if (choice == 0)
		{
			do_exit = 1;
		}

		// Process choice
		if (choice < sizeof(fnct_menu)/sizeof(fnct_menu[0]) && fnct_menu[choice].fnct)
		{
			err = (fnct_menu[choice].fnct)(calc);
		}
		if (choice && err)
		{
			fprintf(stderr, "Function %d returned %d\n", choice, err);
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
