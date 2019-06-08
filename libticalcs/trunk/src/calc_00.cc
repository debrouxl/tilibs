/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

/* Initialize the LinkCable structure with default functions */
/* This module can be used as sample code.*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"

int noop_is_ready (CalcHandle* handle)
{
	(void)handle;
	return 0;
}

int noop_send_key (CalcHandle* handle, uint32_t key)
{
	(void)handle, (void)key;
	return 0;
}

int noop_execute (CalcHandle* handle, VarEntry *ve, const char* args)
{
	(void)handle, (void)ve, (void)args;
	return 0;
}

int noop_recv_screen (CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	(void)handle, (void)sc, (void)bitmap;
	return 0;
}

int noop_get_dirlist (CalcHandle* handle, GNode** vars, GNode** apps)
{
	(void)handle, (void)vars, (void)apps;
	return 0;
}

int noop_get_memfree (CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	(void)handle, (void)ram, (void)flash;
	return 0;
}

int noop_send_backup (CalcHandle* handle, BackupContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_recv_backup (CalcHandle* handle, BackupContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_send_var (CalcHandle* handle, CalcMode mode, FileContent* content)
{
	(void)handle, (void)mode, (void)content;
	return 0;
}

int noop_recv_var (CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	(void)handle, (void)mode, (void)content, (void)vr;
	return 0;
}

int noop_send_var_ns (CalcHandle* handle, CalcMode mode, FileContent* content)
{
	(void)handle, (void)mode, (void)content;
	return 0;
}

int noop_recv_var_ns (CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** ve)
{
	(void)handle, (void)mode, (void)content, (void)ve;
	return 0;
}

int noop_send_flash (CalcHandle* handle, FlashContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_recv_flash (CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	(void)handle, (void)content, (void)vr;
	return 0;
}

int noop_send_os (CalcHandle* handle, FlashContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_recv_idlist (CalcHandle* handle, uint8_t* idlist)
{
	(void)handle, (void)idlist;
	return 0;
}

int noop_dump_rom_1 (CalcHandle* handle)
{
	(void)handle;
	return 0;
}

int noop_dump_rom_2 (CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	(void)handle, (void)size, (void)filename;
	return 0;
}

int noop_set_clock (CalcHandle* handle, CalcClock* _clock)
{
	(void)handle, (void)_clock;
	return 0;
}

int noop_get_clock (CalcHandle* handle, CalcClock* _clock)
{
	(void)handle, (void)_clock;
	return 0;
}

int noop_del_var (CalcHandle* handle, VarRequest* vr)
{
	(void)handle, (void)vr;
	return 0;
}

int noop_new_folder (CalcHandle* handle, VarRequest* vr)
{
	(void)handle, (void)vr;
	return 0;
}

int noop_get_version (CalcHandle* handle, CalcInfos* infos)
{
	(void)handle, (void)infos;
	return 0;
}

int noop_send_cert (CalcHandle* handle, FlashContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_recv_cert (CalcHandle* handle, FlashContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_rename_var (CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	(void)handle, (void)oldname, (void)newname;
	return 0;
}

int noop_change_attr (CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	(void)handle, (void)vr, (void)attr;
	return 0;
}

int noop_send_all_vars_backup (CalcHandle* handle, FileContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_recv_all_vars_backup (CalcHandle* handle, FileContent* content)
{
	(void)handle, (void)content;
	return 0;
}

int noop_send_lab_equipment_data (CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data)
{
	(void)handle, (void)model, (void)data;
	return 0;
}

int noop_get_lab_equipment_data (CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data)
{
	(void)handle, (void)model, (void)data;
	return 0;
}

extern const CalcFncts calc_00 = 
{
	CALC_NONE,
	"NONE",
	N_("Dummy hand-held"),
	N_("Dummy hand-held used when no calc is set"),
	FTS_NONE,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "",     /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "",     /* send_var_ns */
	 "",     /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "",     /* dump_rom_1 */
	 "",     /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 ""      /* get_lab_equipment_data */ },
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var,
	&noop_recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&noop_dump_rom_1,
	&noop_dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&noop_del_var,
	&noop_new_folder,
	&noop_get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data
};
