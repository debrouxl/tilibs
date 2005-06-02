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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * This program shows how to use the libticalcs library. You can consider this
 * as an authoritative example. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/ticables.h"
#include "../src/tifiles.h"
#include "../src/ticalcs.h"

#define TRYF(x) { int aaa_; if((aaa_ = (x))) return aaa_; }

#undef VERSION
#define VERSION "Test program"

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
	CalcKey key = ticalcs_keys_92p('A');

	TRYF(ticalcs_calc_send_key(h, key.normal.value));
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
	TNode *vars, *apps;

	TRYF(ticalcs_calc_get_dirlist(h, &vars, &apps));
	//ticalcs_dirlist_display(vars);
	ticalcs_dirlist_display(apps);
	return 0;
}

static int send_backup(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;
	strcat(filename, ".");
	strcat(filename, tifiles_fext_of_backup(h->model));

	TRYF(ticalcs_calc_send_backup2(h, filename));
	return 0;
}

static int recv_backup(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;
	strcat(filename, ".");
	strcat(filename, tifiles_fext_of_backup(h->model));

	TRYF(ticalcs_calc_recv_backup2(h, filename));
	return 0;
}

static int send_var(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_send_var2(h, MODE_NORMAL, filename));
	return 0;
}

static int recv_var(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;
	VarEntry ve = { 0 };

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;

	printf("Enter folder name: ");
	ret = scanf("%s", ve.fld_name);
	if(ret < 1)
		return 0;

	printf("Enter variable name: ");
	ret = scanf("%s", ve.var_name);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_recv_var2(h, MODE_NORMAL, filename, &ve));
	return 0;
}

static int send_var_ns(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_send_var_ns2(h, MODE_NORMAL, filename));
	return 0;
}

static int recv_var_ns(CalcHandle* h)
{
	const char filename[1024] = "";
	int ret;
	VarEntry ve = { 0 };

	printf("Enter filename: ");
	ret = scanf("%s", filename);
	if(ret < 1)
		return 0;

	TRYF(ticalcs_calc_recv_var_ns2(h, MODE_NORMAL, filename, &ve));

	return 0;
}

static int del_var(CalcHandle* h)
{
	VarEntry ve = { 0 };

	TRYF(ticalcs_calc_del_var(h, &ve));
	return 0;
}

static int send_flash(CalcHandle *h)
{
	return 0;
}

static int recv_flash(CalcHandle *h)
{
	return 0;
}

static int recv_idlist(CalcHandle *h)
{
	return 0;
}

static int dump_rom(CalcHandle *h)
{
	return 0;
}

static int set_clock(CalcHandle *h)
{
	return 0;
}

static int get_clock(CalcHandle *h)
{
	return 0;
}

static const char *str_menu[18] = 
{
	"Exit",
	"Check whether calc is ready",
	"Send a key",
	"Do a screenshot",
	"Listing",
	"Send backup",
	"Recv backup",
	"Send var",
	"Recv var",
	"Delete var",
	"Send var (ns)",
	"Recv var (ns)",
	"Send flash",
	"Recv flash",
	"Get ID-LIST",
	"Dump ROM",
	"Set clock",
	"Get clock",
};

typedef int (*FNCT_MENU) (CalcHandle*);

static FNCT_MENU fnct_menu[18] = 
{
	NULL,
	is_ready,
	send_key,
	recv_screen,
	get_dirlist,
	send_backup,
	recv_backup,
	send_var,
	recv_var,
	del_var,
	send_var_ns,
	recv_var_ns,
	send_flash,
	recv_flash,
	recv_idlist,
	dump_rom,
	set_clock,
	get_clock,
};

int main(int argc, char **argv)
{
	CableHandle* cable;
	CalcHandle* calc;
	int err, i;
	int exit=0;
	int choice;

	// init libs
	ticables_library_init();
	ticalcs_library_init();

	// set cable
	cable = ticables_handle_new(CABLE_BLK, PORT_2);
	if(cable == NULL)
	    return -1;

	// set calc
	calc = ticalcs_handle_new(CALC_TI89T);
	if(calc == NULL)
		return -1;

	// attach cable to calc (and open cable)
	err = ticalcs_cable_attach(calc, cable);

/*
	printf("Wait 1 second...\n");
#if defined(__WIN32__) && !defined(__MINGW32__)
	Sleep(1000);
#else
	sleep(1);
#endif
	*/

	do
	{
restart:
		// Display menu
		printf("Choose an action:\n");
		for(i = 0; i < 18; i++)
			printf("%2i. %s\n", i, str_menu[i]);
		printf("Your choice: ");

		err = scanf("%i", &choice);
		if(err < 1)
			goto restart;
		printf("\n");

		if(choice == 0)
			exit = 1;

		// Process choice
		if(fnct_menu[choice])
			fnct_menu[choice](calc);
		printf("\n");

	} while(!exit);

	// detach cable (made by handle_del, too)
	err = ticalcs_cable_detach(calc);

	// remove calc & cable
	ticalcs_handle_del(calc);
	ticables_handle_del(cable);

  return 0;
}
