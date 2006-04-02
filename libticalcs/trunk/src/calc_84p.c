/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Lievin
 *  Copyright (c) 2005, Benjamin Moody (ROM dumper)
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
	TI84+ support thru DirectUsb link.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ticonv.h"
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dusb_vpkt.h"
#include "cmd84p.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the TI83+
#define TI84P_ROWS  64
#define TI84P_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	TRYF(ti84p_mode_set(handle));
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	/*
	uint8_t req[] = { 0x00, 0x01, 0x00, 0x22 };
	uint8_t buf[1024];
	uint32_t size;
	uint16_t code;

	sc->width = TI84P_COLS;
	sc->height = TI84P_ROWS;
	sc->clipped_width = TI84P_COLS;
	sc->clipped_height = TI84P_ROWS;

	TRYF(ti84p_send_data(handle, sizeof(req), 0x0007, req));

	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0xbb00)
		return ERR_INVALID_OPC;

	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0x0008)
		return ERR_INVALID_OPC;

	// Allocate and copy into bitmap
	*bitmap = (uint8_t *) malloc(TI84P_COLS * TI84P_ROWS * sizeof(uint8_t) / 8);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, buf+7, size-7);
*/
	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* mem)
{
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** ve)
{
	return 0;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	return 0;
}

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{
	/*
	uint8_t buf[16] = { 0 };
	uint32_t size;
	uint16_t code;

	uint32_t calc_time;
	struct tm ref, cur;
	time_t r, c, now;

	time(&now);
	memcpy(&ref, localtime(&now), sizeof(struct tm));

	ref.tm_year = 1997 - 1900;
	ref.tm_mon = 0;
	ref.tm_yday = 0;
	ref.tm_mday = 1;
	ref.tm_wday = 3;
	ref.tm_hour = 0;
	ref.tm_min = 0;
	ref.tm_sec = 0;
	//ref.tm_isdst = 1;
	r = mktime(&ref);

	cur.tm_year = clock->year - 1900;
	cur.tm_mon = clock->month - 1;
	cur.tm_mday = clock->day;	
	cur.tm_hour = clock->hours;
	cur.tm_min = clock->minutes;
	cur.tm_sec = clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);
	
	calc_time = (uint32_t)difftime(c, r);

    snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
    update_label();

	buf[0] = 0x00; buf[1] = 0x25; buf[2] = 0x00; buf[3] = 0x04;
	buf[4] = MSB(MSW(calc_time));
    buf[5] = LSB(MSW(calc_time));
    buf[6] = MSB(LSW(calc_time));
    buf[7] = LSB(LSW(calc_time));
	TRYF(ti84p_send_data(handle, 8, 0x000e, buf));
	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0xaa00)
		return ERR_INVALID_OPC;

	buf[0] = 0x00; buf[1] = 0x27; buf[2] = 0x00; buf[3] = 0x01;
	buf[4] = clock->date_format == 3 ? 0 : clock->date_format;
	TRYF(ti84p_send_data(handle, 5, 0x000e, buf));
	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0xaa00)
		return ERR_INVALID_OPC;

	buf[0] = 0x00; buf[1] = 0x28; buf[2] = 0x00; buf[3] = 0x01;
	buf[4] = clock->time_format == 24 ? 1 : 0;
	TRYF(ti84p_send_data(handle, 5, 0x000e, buf));
	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0xaa00)
		return ERR_INVALID_OPC;
*/
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	/*
	uint8_t req[] = { 0x00, 0x04, 0x00, 0x25, 0x00, 0x27, 0x00, 0x28, 0x00, 0x24 };
	uint8_t buf[32];
	uint32_t size;
	uint16_t code;

	uint32_t calc_time;
	struct tm ref, *cur;
	time_t r, c, now;

	// get raw clock
	snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
    update_label();

	TRYF(ti84p_send_data(handle, sizeof(req), 0x0007, req));

	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0xbb00)
		return ERR_INVALID_OPC;

	TRYF(ti84p_recv_data(handle, &size, &code, buf));
	if(code != 0x0008)
		return ERR_INVALID_OPC;

	// and computes
	calc_time = (buf[7+0] << 24) | (buf[7+1] << 16) | (buf[7+2] << 8) | buf[7+3];

	time(&now);	// retrieve current DST setting
	memcpy(&ref, localtime(&now), sizeof(struct tm));;
	ref.tm_year = 1997 - 1900;
	ref.tm_mon = 0;
	ref.tm_yday = 0;
	ref.tm_mday = 1;
	ref.tm_wday = 3;
	ref.tm_hour = 0;
	ref.tm_min = 0;
	ref.tm_sec = 0;
	//ref.tm_isdst = 1;
	r = mktime(&ref);

	c = r + calc_time;
	cur = localtime(&c);

	clock->year = cur->tm_year + 1900;
	clock->month = cur->tm_mon + 1;
	clock->day = cur->tm_mday;
	clock->hours = cur->tm_hour;
	clock->minutes = cur->tm_min;
	clock->seconds = cur->tm_sec;

    clock->date_format = buf[16] == 0 ? 3 : buf[16];
    clock->time_format = buf[22] ? 24 : 12;
	//printf("(%i %i %i)\n", buf[16], buf[22], buf[28]);
*/
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

const CalcFncts calc_84p_usb = 
{
	CALC_TI84P_USB,
	"TI84+ (USB)",
	N_("TI-84 Plus thru DirectLink USB"),
	N_("TI-84 Plus thru DirectLink USB"),
	OPS_ISREADY,
	&is_ready,
	&send_key,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
