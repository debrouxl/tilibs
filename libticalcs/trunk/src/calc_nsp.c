/* Hey EMACS -*- linux-c -*- */
/* $Id: calc_89t.c 3810 2007-09-25 19:14:30Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (C) 2006  Kevin Kofler
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
	NSPire support thru DirectUsb link.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

static int		is_ready	(CalcHandle* handle)
{
	static int checked = 0;

	// Init once
	if(!checked)
	{
		TRYC(nsp_addr_request(handle));
		TRYC(nsp_addr_assign(handle, NSP_DEV_ADDR));
		checked++;
	}

	// Use ECHO packet for is_ready

	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char *args)
{


	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint32_t size;
	uint8_t cmd, *data;

	TRYC(nsp_session_open(handle, SID_SCREEN_RLE));

	TRYC(cmd_s_screen_rle(handle, 0));

	TRYC(cmd_r_screen_rle(handle, &cmd, &size, &data));
	sc->width = sc->clipped_width = (data[8] << 8) | data[9];
	sc->height = sc->clipped_height = (data[10] << 8) | data[11];

	TRYC(cmd_r_screen_rle(handle, &cmd, &size, &data));
	printf("%i bytes\n", size);

	*bitmap = (uint8_t *)g_malloc(sc->width * sc->height / 2);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, data, size/*sc->width * sc->height / 2*/);
	
	g_free(data);
	TRYC(nsp_session_close(handle));

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
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

static int		send_backup	(CalcHandle* handle, BackupContent* content)
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

static int		send_os    (CalcHandle* handle, FlashContent* content)
{

	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{


	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{



	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{


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
	uint32_t size;
	uint8_t cmd, *data;
	int i;

	TRYC(nsp_session_open(handle, SID_DEV_INFOS));

	TRYC(cmd_s_dev_infos(handle, DI_MODEL));
	TRYC(cmd_r_dev_infos(handle, &cmd, &size, &data));

	strncpy(infos->product_name, (char*)data, 10);
	infos->mask |= INFOS_MAIN_CALC_ID;

	TRYC(cmd_s_dev_infos(handle, DI_VERSION));
	TRYC(cmd_r_dev_infos(handle, &cmd, &size, &data));

	i = 4;
	infos->ram_free = GINT32_FROM_BE(*((uint64_t *)(data + i)));
	infos->mask |= INFOS_RAM_FREE;

	i = 36;
	g_snprintf(infos->os_version, sizeof(infos->boot_version), "%1i.%1i.%04i", 
		data[i+0], data[i+1], (data[i+2] << 8) | data[i+3]);
	infos->mask |= INFOS_OS_VERSION;

	i = 40;
	g_snprintf(infos->boot_version, sizeof(infos->boot_version), "%1i.%1i.%04i", 
		data[i+0], data[i+1], (data[i+2] << 8) | data[i+3]);
	infos->mask |= INFOS_BOOT_VERSION;

	i = 44;
	g_snprintf(infos->boot2_version, sizeof(infos->boot_version), "%1i.%1i.%04i", 
		data[i+0], data[i+1], (data[i+2] << 8) | data[i+3]);
	infos->mask |= INFOS_BOOT2_VERSION;

	i = 82;
	strncpy(infos->main_calc_id, (char*)(data + 84), 28);
	infos->mask |= INFOS_MAIN_CALC_ID;

	g_free(data);
	TRYC(nsp_session_close(handle));
	
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

extern int tixx_recv_backup(CalcHandle* handle, BackupContent* content);

const CalcFncts calc_nsp = 
{
	CALC_NSPIRE,
	"NSPire",
	"NSpire handheld",
	N_("NSPire thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN,
	{"", "", "1P", "1L", "", "2P1L", "2P1L", "2P1L", "1P1L", "2P1L", "1P1L", "2P1L", "2P1L",
		"2P", "1L", "2P", "", "", "1L", "1L", "", "1L", "1L" },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&tixx_recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_os,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
