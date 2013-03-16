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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ticables.h"
#include "tifiles.h"
#include "../src/ticalcs.h"
#include "../src/nsp_cmd.h"
#include "../src/cmd73.h"

#define TRYF(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

#undef VERSION
#define VERSION "Test program"

#ifdef _MSC_VER
# define SNULL	{ 0 }
#else
# define SNULL	{}
#endif

static int read_varname(CalcHandle* h, VarRequest *vr, const char *prompt)
{
	char buf[256];
	const char *s;
	int ret;
	unsigned long val;
	char * endptr;

	memset(vr, 0, sizeof(VarRequest));

	if(ticalcs_calc_features(h) & FTS_FOLDER)
	{
		printf("Enter%s folder name: ", prompt);
		ret = scanf("%1023s", vr->folder);
		if(ret < 1)
			return 0;
	}

	printf("Enter%s variable name: ", prompt);
	ret = scanf("%1023s", vr->name);
	if(ret < 1)
		return 0;

	if(tifiles_calc_is_ti8x(h->model))
	{
		printf("Enter%s variable type: ", prompt);
		ret = scanf("%255s", buf);
		if(ret < 1)
			return 0;

		// Special handling for types written in hex.
		errno = 0;
		vr->type = (uint8_t)strtoul(buf, &endptr, 16);

		if (errno != 0)
		{
			// The string doesn't seem to be a valid numeric value.
			// Let's try to parse a fext instead.
			vr->type = tifiles_fext2vartype(h->model, buf);
			s = tifiles_vartype2string(h->model, vr->type);
			if(s == NULL || *s == 0)
			{
				vr->type = tifiles_string2vartype(h->model, buf);
			}
		}
	}

	return 1;
}

static void print_lc_error(int errnum)
{
	char *msg;

	ticables_error_get(errnum, &msg);
	fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", 
		errnum, msg);

	free(msg);

}

static int is_ready(CalcHandle* h)
{
	int err;

	err = ticalcs_calc_isready(h);
	printf("Hand-held is %sready !\n", err ? "not " : "");

	return 0;
}

static int send_key(CalcHandle *h)
{
	const CalcKey *key = ticalcs_keys_92p('A');

	TRYF(ticalcs_calc_send_key(h, key->normal.value));
	return 0;
}

static int execute(CalcHandle *h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;
	    
	TRYF(ticalcs_calc_execute(h, &ve, ""));
	return 0;
}

static int recv_screen(CalcHandle *h)
{
	CalcScreenCoord sc = { SCREEN_CLIPPED, 0, 0, 0, 0 };
	uint8_t* bitmap = NULL;

	TRYF(ticalcs_calc_recv_screen(h, &sc, &bitmap));
	//free(bitmap);
	return 0;
}

static int get_dirlist(CalcHandle *h)
{
	GNode *vars, *apps;

	TRYF(ticalcs_calc_get_dirlist(h, &vars, &apps));
	ticalcs_dirlist_display(vars);
	ticalcs_dirlist_display(apps);
	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	return 0;
}

static int send_backup(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;
	strcat(filename, ".");
	strcat(filename, tifiles_fext_of_backup(h->model));

	TRYF(ticalcs_calc_send_backup2(h, filename));
	return 0;
}

static int recv_backup(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;
	strcat(filename, ".");
	strcat(filename, tifiles_fext_of_backup(h->model));

	TRYF(ticalcs_calc_recv_backup2(h, filename));
	return 0;
}

static int send_var(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;
	FileContent *content;
	VarEntry ve;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	if (h->model == CALC_NSPIRE)
	{
		memset(&ve, 0, sizeof(ve));
		if(!read_varname(h, &ve, ""))
			return 0;
	}

	content = tifiles_content_create_regular(h->model);
	TRYF(tifiles_file_read_regular(filename, content));
	if (h->model == CALC_NSPIRE)
	{
		strcpy(content->entries[0]->folder, ve.folder);
		strcpy(content->entries[0]->name, ve.name);
	}
	TRYF(ticalcs_calc_send_var(h, MODE_NORMAL, content));
	TRYF(tifiles_content_delete_regular(content));

	return 0;
}

static int recv_var(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;
	VarEntry ve;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;

	TRYF(ticalcs_calc_recv_var2(h, MODE_NORMAL, filename, &ve));
	return 0;
}

static int send_var_ns(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_send_var_ns2(h, MODE_NORMAL, filename));
	return 0;
}

static int recv_var_ns(CalcHandle* h)
{
	char filename[1024] = "";
	int ret;
	VarRequest *ve;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_recv_var_ns2(h, MODE_NORMAL, filename, &ve));

	return 0;
}

static int send_flash(CalcHandle *h)
{
	char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;
	strcat(filename, ".");
	strcat(filename, tifiles_fext_of_flash_app(h->model));

	TRYF(ticalcs_calc_send_app2(h, filename));
	return 0;
}

static int recv_flash(CalcHandle *h)
{
	char filename[1024] = "";
	int ret;
	VarEntry ve;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	memset(&ve, 0, sizeof(ve));
	printf("Enter application name: ");
	ret = scanf("%1023s", ve.name);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_recv_app2(h, filename, &ve));
	return 0;
}

static int send_os(CalcHandle *h)
{
	char filename[1024] = "";
	int ret;
	char * message;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_send_os2(h, filename));

	return 0;
}

static int recv_idlist(CalcHandle *h)
{
	uint8_t id[32];

	TRYF(ticalcs_calc_recv_idlist(h, id));
	printf("IDLIST: <%s>", id);

	return 0;
}

static int dump_rom(CalcHandle *h)
{
	char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_dump_rom_1(h));
	TRYF(ticalcs_calc_dump_rom_2(h, 0, filename));
	return 0;
}

static int set_clock(CalcHandle *h)
{
	printf("Not implemented yet !\n");
	return 0;
}

static int get_clock(CalcHandle *h)
{
	CalcClock clk;

	TRYF(ticalcs_calc_get_clock(h, &clk));
	ticalcs_clock_show(h->model, &clk);

	return 0;
}

static int del_var(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;
	    
	TRYF(ticalcs_calc_del_var(h, &ve));
	return 0;
}

static int rename_var(CalcHandle* h)
{
	VarEntry src;
	VarEntry dst;
	int ret;

	memset(&src, 0, sizeof(src));
	if(!read_varname(h, &src, " current"))
		return 0;
	memset(&dst, 0, sizeof(dst));
	if(!read_varname(h, &dst, " new"))
		return 0;

	TRYF(ticalcs_calc_rename_var(h, &src, &dst));
	return 0;
}

static int archive_var(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;

	TRYF(ticalcs_calc_change_attr(h, &ve, ATTRB_ARCHIVED));
	return 0;
}

static int unarchive_var(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;

	TRYF(ticalcs_calc_change_attr(h, &ve, ATTRB_NONE));
	return 0;
}

static int lock_var(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(!read_varname(h, &ve, ""))
		return 0;

	TRYF(ticalcs_calc_change_attr(h, &ve, ATTRB_LOCKED));
	return 0;
}

static int new_folder(CalcHandle* h)
{
	VarEntry ve;
	int ret;

	memset(&ve, 0, sizeof(ve));
	if(tifiles_calc_is_ti9x(h->model))
	{
		printf("Enter folder name: ");
		ret = scanf("%1023s", ve.folder);
		if(ret < 1) return 0;
	}

	TRYF(ticalcs_calc_new_fld(h, &ve));
	return 0;
}

static int get_version(CalcHandle *h)
{
	CalcInfos infos;

	TRYF(ticalcs_calc_get_version(h, &infos));
	//ticalcs_clock_show(h->model, &clk);

	return 0;
}

static int probe_calc(CalcHandle *h)
{
	int m;
	int p;
	int ret;
	CalcModel model;

	printf("Enter cable & port for probing (c p): ");
	ret = scanf("%d %d", &m, &p);
	if(ret < 2)
		return 0;

	ticalcs_probe((CableModel)m, (CablePort)p, &model, !0);
	//ticalcs_probe_calc(h->cable, &model);
	//ticalcs_probe_usb_calcs(h->cable, &model);
	printf("Found: <%s>", ticalcs_model_to_string(model));

	return 0;
}

static int nsp_send_key(CalcHandle *h)
{
	static const uint8_t HOME[] = {0x00, 0xFD, 0x00};
	static const uint8_t A[] = {97, 102, 0};

	nsp_cmd_s_keypress_event(h, HOME);
	nsp_cmd_s_keypress_event(h, A);
	nsp_cmd_s_keypress_event(h, HOME);

	return 0;
}

static int ti83pfamily_dump(CalcHandle *h)
{
	char filename[1024] = "";
	uint8_t buffer[0x4100];
	int page;
	int ret;
	uint16_t length;
	FILE *f;

	printf("Enter page number for dumping: ");
	ret = scanf("%d", &page);
	if(ret < 1)
		return 0;

	printf("Enter filename: ");
	ret = scanf("%1023s", filename);
	if(ret < 1)
		return 0;

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

#define NITEMS	29

static const char *str_menu[NITEMS] = 
{
	"Exit",
	"Check whether calc is ready",
	"Send a key",
	"Execute program",
	"Do a screenshot",
	"Listing",
	"Send backup",
	"Recv backup",
	"Send var",
	"Recv var",
	"Send var (ns)",
	"Recv var (ns)",
	"Send flash",
	"Recv flash",
	"Send OS",
	"Get ID-LIST",
	"Dump ROM",
	"Set clock",
	"Get clock",
	"Delete var",
	"Rename var",
	"Archive var",
	"Unarchive var",
	"Lock var",
	"New folder",
	"Get version",
	"Probe calc",
	"Nspire-specific send key",
	"83+-family-specific memory dump"
};

typedef int (*FNCT_MENU) (CalcHandle*);

static FNCT_MENU fnct_menu[NITEMS] = 
{
	NULL,
	is_ready,
	send_key,
	execute,
	recv_screen,
	get_dirlist,
	send_backup,
	recv_backup,
	send_var,
	recv_var,
	send_var_ns,
	recv_var_ns,
	send_flash,
	recv_flash,
	send_os,
	recv_idlist,
	dump_rom,
	set_clock,
	get_clock,
	del_var,
	rename_var,
	archive_var,
	unarchive_var,
	lock_var,
	new_folder,
	get_version,
	probe_calc,
	nsp_send_key,
	ti83pfamily_dump
};

int main(int argc, char **argv)
{
	CableModel cable_model = CABLE_NUL;
	int port_number = 1;
	CalcModel calc_model = CALC_NONE;
	CableHandle* cable;
	CalcHandle* calc;
	int err, i;
	int do_exit=0;
	int choice;
	char* colon;

	while((i = getopt(argc, argv, "c:m:")) != -1)
	{
		if(i == 'c')
		{
			colon = strchr(optarg, ':');
			if(colon)
			{
				*colon = 0;
				port_number = atoi(colon + 1);
			}
			cable_model = ticables_string_to_model(optarg);
		}
		else if(i == 'm')
		{
			calc_model = ticalcs_string_to_model(optarg);
		}
		else
		{
			fprintf(stderr, "Usage: %s [-c CABLE[:PORT]] [-m CALC]\n", argv[0]);
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
	if(cable_model == CABLE_NUL)
	{
		int *pids, npids;

		ticables_get_usb_devices(&pids, &npids);

		if(npids < 1)
		{
			fprintf(stderr, "No supported USB cable found\n");
			return 1;
		}

		switch(pids[0])
		{
		case PID_TIGLUSB:
			cable_model = CABLE_SLV;
			break;

		case PID_TI84P:
		case PID_TI84P_SE:
			cable_model = CABLE_USB;
			calc_model = CALC_TI84P_USB;
			break;

		case PID_TI89TM:
			cable_model = CABLE_USB;
			calc_model = CALC_TI89T_USB;
			break;

		case PID_NSPIRE:
			cable_model = CABLE_USB;
			calc_model = CALC_NSPIRE;
			break;

		default:
			fprintf(stderr, "Unrecognized PID %04x\n", pids[0]);
			return 1;
		}

		free(pids);
	}

	cable = ticables_handle_new(cable_model, port_number);
	if(cable == NULL)
	{
		fprintf(stderr, "ticables_handle_new failed\n");
		return -1;
	}

	// set calc
	if(calc_model == CALC_NONE)
	{
		if(ticalcs_probe(cable_model, port_number, &calc_model, 1))
		{
			fprintf(stderr, "No calculator found\n");
			return 1;
		}
	}

	calc = ticalcs_handle_new(calc_model);
	if(calc == NULL)
	{
		fprintf(stderr, "ticalcs_handle_new failed\n");
		return -1;
	}

	// attach cable to calc (and open cable)
	err = ticalcs_cable_attach(calc, cable);

	do
	{
restart:
		// Display menu
		printf("Choose an action:\n");
		for(i = 0; i < NITEMS; i++)
			printf("%2i. %s\n", i, str_menu[i]);
		printf("Your choice: ");

		err = scanf("%i", &choice);
		if(err < 1)
			goto restart;
		printf("\n");

		if(choice == 0)
			do_exit = 1;

		// Process choice
		if(fnct_menu[choice])
			fnct_menu[choice](calc);
		printf("\n");

	} while(!do_exit);

	// detach cable (made by handle_del, too)
	err = ticalcs_cable_detach(calc);

	// remove calc & cable
	ticalcs_handle_del(calc);
	ticables_handle_del(cable);

	return 0;
}
