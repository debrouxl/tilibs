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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifndef __WIN32__
#include <unistd.h>
#else
#include <conio.h>
#endif

#include "../src/ticables.h"

#undef VERSION
#define VERSION "Test program"

static void print_lc_error(int errnum)
{
	char *msg;

	if (!ticables_error_get(errnum, &msg))
	{
		fprintf(stderr, "Link cable error (code %i)...\n<<%s>>\n", errnum, msg);
		ticables_error_free(msg);
	}
}

# define  to_START(ref)         { (ref) = ((1000*clock()) / CLOCKS_PER_SEC); }
# define  to_CURRENT(ref)       ( (1000*clock()) / CLOCKS_PER_SEC - (ref) )
# define  to_ELAPSED(ref, max)  ( TO_CURRENT(ref) > (100*(max)) )

#if defined(__LINUX__) || defined(__MACOSX__) || defined(__BSD__)
# include <unistd.h>
# define PAUSE(x)  usleep(1000*(x))
#elif defined(__WIN32__)
# include <windows.h>
# define PAUSE(x)  Sleep((x))
#endif

int main(int argc, char **argv)
{
	CableHandle *handle;
	int err;
	//	int i, j;
	//	uint8_t buf[65536], data;
	//	int status, result;
	//	uint8_t scr[3840 + 6];
	//	int **probing = NULL;

#if 0
	tiTIME ref, end;
	unsigned long k;

	to_START(ref);
	for(k = 0; k < 1000000; k++) printf(" ");
	to_START(end);
	printf("%lu %lu\n", ref, end);
	printf("%lu\n", to_CURRENT(ref));
	printf("%i\n", to_ELAPSED(ref, 60));

	return 0;
#endif

	printf("USB support: %i\n", ticables_is_usb_enabled());

	// init lib
	ticables_library_init();

	print_lc_error(1);

#if 0
	ticables_probing_do(&probing, 5, PROBE_ALL);
	for (i = 1; i <= 7; i++)
	{
		printf("%i: %i %i %i %i\n", i, probing[i][1], probing[i][2], probing[i][3], probing[i][4]);
	}
	ticables_probing_finish(&probing);
#endif

#if 0
	{
		int *list = NULL;
		int i, n;
		int *p;

		ticables_get_usb_devices(&list, &n);
		printf("List of devices:\n");
		for(i = 0; i < n; i++)
		{
			printf("%i: %04x\n", i, list[i]);
		}
	}
#endif

	// set cable
	handle = ticables_handle_new(CABLE_PAR, PORT_1);
	if (handle == nullptr)
	{
		return -1;
	}

	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_handle_show(handle);

	// open cable
	err = ticables_cable_open(handle);
	if (err)
	{
		print_lc_error(err);
	}
	if (err)
	{
		return -1;
	}
#if 0
	// simple test with DirectLink hand-helds (buf size req/neg)
	buf[0]=0x00; buf[1]=0x00; buf[2]=0x00; buf[3]=0x04;
	buf[4]=0x01;
	buf[5]=0x00; buf[6]=0x00; buf[7]=0x04; buf[8]=0x00;
	err = ticables_cable_send(handle, buf, 9);
	if (err)
	{
		print_lc_error(err);
	}

	// display answer
	memset(buf, 0, sizeof(buf));
	err = ticables_cable_recv(handle, buf, 9);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 9; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");
#endif

#if 0
	// mode set
	i = 0;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x10;
	buf[i++]=0x04;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x0a;
	buf[i++]=0x00; buf[i++]=0x01;
	buf[i++]=0x00; buf[i++]=0x03; 
	buf[i++]=0x00; buf[i++]=0x01;
	buf[i++]=0x00; buf[i++]=0x00; 
	buf[i++]=0x00; buf[i++]=0x00; 
	buf[i++]=0x07; buf[i++]=0xd0;

	err = ticables_cable_send(handle, buf, i);
	if (err)
	{
		print_lc_error(err);
	}

	err = ticables_cable_recv(handle, buf, 7);
	if (err)
	{
		print_lc_error(err);
	}

	for(i = 0; i < 7; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");

	// mode ack
	err = ticables_cable_recv(handle, buf, 15);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 15; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");

	i = 0;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x02;
	buf[i++]=0x05;
	buf[i++]=0xe0; buf[i++]=0x00;

	err = ticables_cable_send(handle, buf, i);
	if (err)
	{
		print_lc_error(err);
	}

	PAUSE(500);
#endif

#if 0
	// param req (TI84+ only)
	i = 0;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0xa;
	buf[i++]=0x04;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x04;
	buf[i++]=0x00; buf[i++]=0x07;
	buf[i++]=0x00; buf[i++]=0x01; 
	buf[i++]=0x00; buf[i++]=0x22;

	err = ticables_cable_send(handle, buf, i);
	if (err)
	{
		print_lc_error(err);
	}

	err = ticables_cable_recv(handle, buf, 7);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 7; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");

	// delay ack
	err = ticables_cable_recv(handle, buf, 15);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 15; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");

	i = 0;
	buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x02;
	buf[i++]=0x05;
	buf[i++]=0xe0; buf[i++]=0x00;

	err = ticables_cable_send(handle, buf, i);
	if (err)
	{
		print_lc_error(err);
	}

	// param data
	for (j = 0; j < 3; j++)
	{
		err = ticables_cable_recv(handle, buf, 255);
		if (err)
		{
			print_lc_error(err);
		}

		for (i = 0; i < 16; i++)
		{
			printf("%02x ", buf[i]);
		}
		printf("\n");

		i = 0;
		buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x02;
		buf[i++]=0x05;
		buf[i++]=0xe0; buf[i++]=0x00;

		err = ticables_cable_send(handle, buf, i);
		if (err)
		{
			print_lc_error(err);
		}

	}
	{
		err = ticables_cable_recv(handle, buf, 36);
		if (err)
		{
			print_lc_error(err);
		}

		for (i = 0; i < 16; i++)
		{
			printf("%02x ", buf[i]);
		}
		printf("\n");

		i = 0;
		buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x02;
		buf[i++]=0x05;
		buf[i++]=0xe0; buf[i++]=0x00;

		err = ticables_cable_send(handle, buf, i);
		if (err)
		{
			print_lc_error(err);
		}

	}
	PAUSE(500);
#endif

#if 0
	// do a simple test with a TI89/92+ calculator
	buf[0] = 0x08; buf[1] = 0x68; buf[2] = 0x00; buf[3] = 0x00;	// RDY
	err = ticables_cable_send(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	// display answer
	memset(buf, 0xff, 4);
	err = ticables_cable_recv(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 4; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");
#endif

#if 0
	// do a screendump
	buf[0] = 0x08;  buf[1] = 0x6D; buf[2] = 0x00; buf[3] = 0x00;	// SCR
	err = ticables_cable_send(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	memset(buf, 0xff, 4);
	err = ticables_cable_recv(handle, buf, 4);	// ACK
	if (err)
	{
		print_lc_error(err);
	}

	err = ticables_cable_recv(handle, scr, 0x0f00 + 6);	// XDP
	if (err)
	{
		print_lc_error(err);
	}

	printf("%02x %02x\n", scr[2], scr[3]);

	buf[0] = 0x08;  buf[1] = 0x56; buf[2] = 0x00; buf[3] = 0x00;	// ACK
	err = ticables_cable_send(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

#endif

#if 0
	// simple test for data arrival detection
	buf[0] = 0x08;  buf[1] = 0x87; buf[2] = 'A'; buf[3] = 0x00;		// KEY
	err = ticables_cable_send(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	for(status = 0; !status;)
	{
		err = ticables_cable_check(handle, &status);
		if (err)
		{
			print_lc_error(err);
		}
	}

	// display answer
	memset(buf, 0xff, 4);
	err = ticables_cable_recv(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 4; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");
#endif

#if 0
	for(status = 0; !status;)
	{
		//fprintf(stdout, "$\n");
		//fflush(stdout);
		err = ticables_cable_check(handle, &status);
		if (err)
		{
			print_lc_error(err);
		}
	}

	// display answer
	memset(buf, 0xff, 4);
	err = ticables_cable_recv(handle, buf, 4);
	if (err)
	{
		print_lc_error(err);
	}

	for (i = 0; i < 4; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");
#endif

	// close cable
	ticables_cable_close(handle);
	// release handle
	ticables_handle_del(handle);

	// exit lib
	ticables_library_exit();
#ifdef __WIN32__
	while(!kbhit());
#endif
	return 0;
}

	// Remote control: display a 'A' on the calc
	/*
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
*/
