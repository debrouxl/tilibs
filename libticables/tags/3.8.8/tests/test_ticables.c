/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#include <unistd.h>
#endif
//#include <sys/time.h>

#ifdef HAVE_TILP_CABL_INT_H
# include <tilp/ticables.h>
#else
# include "../src/ticables.h"
#endif

#undef VERSION
#define VERSION "Test program"

void print_lc_error(int errnum)
{
	char msg[256] = "No error -> bug !\n";

	ticable_get_error(errnum, msg);
	fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", errnum,
		msg);
}

int main(int argc, char **argv)
{
	int err;
	uint8_t data;
	TicableLinkParam lp;
	TicableLinkCable lc;
	tiTIME ref, end;

	/* 
	   Display verbose informations in the shell (Linux) or 
	   in a console (Win32) 
	 */
	ticable_DISPLAY_settings(DSP_ON);

	// set cable
	ticable_init();

	// get default params
	ticable_get_default_param(&lp);
	// or set your own ones
	lp.delay = 10;
	lp.timeout = 20;
	lp.port = SERIAL_PORT_2;
	lp.method = IOM_AUTO;
	ticable_set_param(&lp);

	if((err=ticable_set_cable(LINK_SER, &lc))) {
		print_lc_error(err);
                return -1;
	}

	// Init port (usually at program startup)
	if ((err = lc.init())) {
		print_lc_error(err);
		return -1;
	}

	// open it (usually before each set of transfer)
	if ((err = lc.open())) {
		print_lc_error(err);
		return -1;
	}

	DISPLAY("Wait 1 second...\n");
#if defined(__WIN32__) && !defined(__MINGW32__)
	Sleep(1000);
#else
	sleep(1);
#endif

	/* 
	   Do a simple test with a TI89/92+ calculator
	 */
	// Poll for data
#ifdef POLL_TEST
	do {
		err = lc.check(&retval);
	}
	while (retval == STATUS_NONE);
	DISPLAY("One or more byte are available: %i %i\n", err, retval);
	lc.get(&data);
	DISPLAY("Data: %02X\n", data);
	return 0;
#endif

	// Check if calc is ready
	DISPLAY("Check if calc is OK...\n");
	
        toSTART(ref);
	err = lc.put(0x00);
	printf("Time: %1.1f\n", toCURRENT(ref));
	if(err)
		goto exit;
	err = lc.put(0x68);
	err = lc.put(0x00);
	err = lc.put(0x00);

	toSTART(ref);
	err = lc.get(&data);
	printf("Time: %1.1f\n", toCURRENT(ref));
	printf("Data: %02X\n", data);
	if(err)
		goto exit;
	err = lc.get(&data);
	printf("Data: %02X\n", data);
	err = lc.get(&data);
	printf("Data: %02X\n", data);
	err = lc.get(&data);
	printf("Data: %02X\n", data);

	DISPLAY("\n");
	goto exit;
	// Remote control: display a 'A' on the calc
	DISPLAY("Display a 'A' on the calc...\n");
	err = lc.put(0x08);	// 0x08: TI89, 0x09: TI92
	err = lc.put(0x87);
	err = lc.put('A');
	err = lc.put(0x00);
	printf("Data: %02X\n", data);
	err = lc.get(&data);
	printf("Data: %02X\n", data);
	err = lc.get(&data);
	printf("Data: %02X\n", data);
	err = lc.get(&data);
	printf("Data: %02X\n", data);
	err = lc.get(&data);

      exit:
	// Close port
	if ((err = lc.close()))
		print_lc_error(err);

	// Exit port (usually at program termination
	if ((err = lc.exit()))
		print_lc_error(err);

	return 0;
}
