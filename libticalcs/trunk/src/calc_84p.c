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
	uint16_t pid[] = { PID84P_SCREENSHOT };
	CalcParm *param;

	sc->width = TI84P_COLS;
	sc->height = TI84P_ROWS;
	sc->clipped_width = TI84P_COLS;
	sc->clipped_height = TI84P_ROWS;

	TRYF(ti84p_params_request(handle, 1, pid, &param));
	if(!param->ok)
		return ERR_INVALID_PACKET;
	
	*bitmap = (uint8_t *) malloc(TI84P_COLS * TI84P_ROWS / 8);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, param->data, TI84P_COLS * TI84P_ROWS / 8);

	del_params_array(1, param);

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
	CalcParm param = { 0 };

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

	param.pid = PID84P_CLK_SEC;
	param.size = 4;
	param.data[0] = MSB(MSW(calc_time));
    param.data[1] = LSB(MSW(calc_time));
    param.data[2] = MSB(LSW(calc_time));
    param.data[3] = LSB(LSW(calc_time));
	ti84p_params_set(handle, &param);

	param.pid = PID84P_CLK_DATE_FMT;
	param.size = 1;
	param.data[0] = clock->date_format == 3 ? 0 : clock->date_format;
	ti84p_params_set(handle, &param);

	param.pid = PID84P_CLK_TIME_FMT;
	param.size = 1;
	param.data[0] = clock->time_format == 24 ? 1 : 0;
	ti84p_params_set(handle, &param);

	param.pid = PID84P_CLK_ON;
	param.size = 1;
	param.data[0] = clock->on;
	ti84p_params_set(handle, &param);	

	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	uint16_t pids[4] = { PID84P_CLK_SEC, PID84P_CLK_DATE_FMT, PID84P_CLK_TIME_FMT, PID84P_CLK_ON };
	CalcParm *params;

	uint32_t calc_time;
	struct tm ref, *cur;
	time_t r, c, now;

	// get raw clock
	snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
    update_label();

	TRYF(ti84p_params_request(handle, 4, pids, &params));
	if(!params[0].ok)
		return ERR_INVALID_PACKET;
	
	// and computes
	calc_time = (params[0].data[0] << 24) | (params[0].data[1] << 16) | 
				(params[0].data[2] <<  8) | (params[0].data[3] <<  0);

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

    clock->date_format = params[1].data[0] == 0 ? 3 : params[1].data[0];
    clock->time_format = params[2].data[0] ? 24 : 12;
	clock->on = params[3].data[0];

	del_params_array(1, params);

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
	OPS_ISREADY | OPS_CLOCK,
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
