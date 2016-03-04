/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	TI92 support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "cmd68k.h"
#include "keys92p.h"
#include "rom92f2.h"
#include "romdump.h"

#define SEND_RDY ti92_send_RDY
#define SEND_KEY ti92_send_KEY
#define SEND_SCR ti92_send_SCR
#define SEND_ACK ti92_send_ACK
#define SEND_VAR ti92_send_VAR
#define SEND_XDP ti92_send_XDP
#define SEND_REQ ti92_send_REQ
#define SEND_RTS ti92_send_RTS
#define SEND_CTS ti92_send_CTS
#define SEND_CNT ti92_send_CNT
#define SEND_EOT ti92_send_EOT

#define RECV_ACK ti92_recv_ACK
#define RECV_VAR ti92_recv_VAR
#define RECV_XDP ti92_recv_XDP
#define RECV_CTS ti92_recv_CTS
#define RECV_SKP ti92_recv_SKP
#define RECV_CNT ti92_recv_CNT
#define RECV_EOT ti92_recv_EOT

// Screen coordinates of the TI92
#define TI92_ROWS  128
#define TI92_COLS  240

static int		is_ready	(CalcHandle* handle)
{
	int ret;
	uint16_t status;

	ret = SEND_RDY(handle);
	if (!ret)
	{
		ret = RECV_ACK(handle, &status);
		if (!ret)
		{
			ret = (MSB(status) & 0x01) ? ERR_NOT_READY : 0;
		}
	}

	return ret;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	int ret;

	ret = SEND_KEY(handle, key);
	if (!ret)
	{
		ret = RECV_ACK(handle, &key);

		PAUSE(50);
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	int ret;

	// Go back to homescreen
	PAUSE(200);
	ret = send_key(handle, (KEY92P_CTRL + KEY92P_Q));
	if (!ret)
	{
		ret = send_key(handle, KEY92P_CLEAR);
		if (!ret)
		{
			ret = send_key(handle, KEY92P_CLEAR);
		}
	}

	if (!ret)
	{
		unsigned int i;
		// Launch program by remote control
		for (i = 0; !ret && i < strlen(ve->folder); i++)
		{
			ret = send_key(handle, (ve->folder)[i]);
		}

		if (!ret && strcmp(ve->folder, ""))
		{
			ret = send_key(handle, '\\');
		}

		for (i = 0; !ret && i < strlen(ve->name); i++)
		{
			ret = send_key(handle, (ve->name)[i]);
		}

		if (!ret)
		{
			ret = send_key(handle, KEY92P_LP);
			if (!ret)
			{
				if (args)
				{
					for (i = 0; !ret && i < strlen(args); i++)
					{
						ret = send_key(handle, args[i]);
					}
				}
				if (!ret)
				{
					ret = send_key(handle, KEY92P_RP);
					if (!ret)
					{
						ret = send_key(handle, KEY92P_ENTER);

						PAUSE(200);
					}
				}
			}
		}
	}

	return ret;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;

	*bitmap = (uint8_t *)ticalcs_alloc_screen(65537U);
	if (*bitmap == NULL)
	{
		return ERR_MALLOC;
	}

	sc->width = TI92_COLS;
	sc->height = TI92_ROWS;
	sc->clipped_width = TI92_COLS;
	sc->clipped_height = TI92_ROWS;
	sc->pixel_format = CALC_PIXFMT_MONO;

	ret = SEND_SCR(handle);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			uint16_t max_cnt;
			ret = RECV_XDP(handle, &max_cnt, *bitmap);
			if (!ret || ret == ERR_CHECKSUM) // problem with checksum
			{
				*bitmap = ticalcs_realloc_screen(*bitmap, TI92_COLS * TI92_ROWS / 8);
				ret = SEND_ACK(handle);
			}
		}
	}

	if (ret)
	{
		ticalcs_free_screen(*bitmap);
		*bitmap = NULL;
	}

	return ret;
}

static int		get_dirlist_92	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	VarEntry info;
	int ret;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}

	TRYF(SEND_REQ(handle, 0, TI92_RDIR, "\0\0\0\0\0\0\0"));
	TRYF(RECV_ACK(handle, NULL));
	TRYF(RECV_VAR(handle, &info.size, &info.type, info.name));

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		GNode *folder = NULL;
		char folder_name[9];
		char *utf8;
		uint8_t * buffer = handle->buffer;
		uint16_t unused;

		TRYF(SEND_ACK(handle));
		TRYF(SEND_CTS(handle));

		TRYF(RECV_ACK(handle, NULL));
		TRYF(RECV_XDP(handle, &unused, buffer));

		memcpy(ve->name, buffer + 4, 8);	// skip 4 extra 00s
		ve->name[8] = '\0';
		ve->type = buffer[12];
		ve->attr = buffer[13];
		ve->size = buffer[14] | (((uint32_t)buffer[15]) << 8) | (((uint32_t)buffer[16]) << 16) | (((uint32_t)buffer[17]) << 24);
		ve->folder[0] = 0;

		if (ve->type == TI92_DIR) 
		{
			ticalcs_strlcpy(folder_name, ve->name, sizeof(folder_name));
			folder = dirlist_create_append_node(ve, vars);
		} 
		else 
		{
			ticalcs_strlcpy(ve->folder, folder_name, sizeof(ve->folder));

			if (!strcmp(ve->folder, "main") && (!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
			{
				tifiles_ve_delete(ve);
			}
			else
			{
				GNode *node = dirlist_create_append_node(ve, &folder);
				if (node == NULL)
				{
					return ERR_MALLOC;
				}
			}
		}

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			ve->name, 
			tifiles_vartype2string(handle->model, ve->type),
			ve->attr,
			ve->size);

		TRYF(SEND_ACK(handle));
		ret = RECV_CNT(handle);
		if (ret == ERR_EOT)
		{
			break;
		}
		if (ret)
		{
			return ret;
		}

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		ticalcs_slprintf(update_->text, sizeof(update_->text), _("Parsing %s/%s"), ((VarEntry *) (folder->data))->name, utf8);
		ticonv_utf8_free(utf8);
		update_->label();
	}

	return SEND_ACK(handle);
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	(void)handle;
	*ram = *flash = -1;
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	int i;
	int nblocks;

	TRYF(SEND_VAR(handle, content->data_length, TI92_BKUP, content->rom_version));
	TRYF(RECV_ACK(handle, NULL));

	update_->cnt2 = 0;
	nblocks = content->data_length / 1024;
	handle->updat->max2 = nblocks;

	for (i = 0; i <= nblocks; i++) 
	{
		uint32_t length = (i != nblocks) ? 1024 : content->data_length % 1024;

		TRYF(SEND_VAR(handle, length, TI92_BKUP, content->rom_version));
		TRYF(RECV_ACK(handle, NULL));

		TRYF(RECV_CTS(handle));
		TRYF(SEND_ACK(handle));

		TRYF(SEND_XDP(handle, length, content->data_part + 1024 * i));
		TRYF(RECV_ACK(handle, NULL));

		handle->updat->cnt2 = i;
		update_pbar();
	}

	return SEND_EOT(handle);
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	uint32_t block_size;
	int block, ret = 0;
	uint16_t unused;
	uint8_t *ptr;

	TRYF(SEND_REQ(handle, 0, TI92_BKUP, "main\\backup"));
	TRYF(RECV_ACK(handle, &unused));

	content->model = CALC_TI92;
	ticalcs_strlcpy(content->comment, tifiles_comment_set_backup(), sizeof(content->comment));
	content->data_part = tifiles_ve_alloc_data(128 * 1024);
	content->type = TI92_BKUP;
	content->data_length = 0;

	for (block = 0;; block++) 
	{
		int ret2;

		ticalcs_slprintf(update_->text, sizeof(update_->text), _("Block #%2i"), block);
		update_label();
    
		ret = RECV_VAR(handle, &block_size, &content->type, content->rom_version);
		ret2 = SEND_ACK(handle);

		if (ret)
		{
			if (ret == ERR_EOT)
			{
				ret = 0;
			}
			break;
		}
		if (ret2)
		{
			ret = ret2;
			break;
		}

		TRYF(SEND_CTS(handle));
		TRYF(RECV_ACK(handle, NULL));

		ptr = content->data_part + content->data_length;
		TRYF(RECV_XDP(handle, &unused, ptr));
		memmove(ptr, ptr + 4, block_size);
		TRYF(SEND_ACK(handle));
		content->data_length += block_size;
	}

	return ret;
}

static int		send_var_92	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	unsigned int i;
	uint16_t status;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		uint8_t * buffer = handle->buffer;
		uint8_t vartype = entry->type;
		char varname[18];

		if (entry->action == ACT_SKIP)
		{
			continue;
		}

		if (mode & MODE_LOCAL_PATH)
		{
			// local & not backup
			ticalcs_strlcpy(varname, entry->name, sizeof(varname));
		}
		else
		{
			// full or backup
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}

		ticonv_varname_to_utf8_sn(handle->model, varname, update_->text, sizeof(update_->text), vartype);
		update_label();

		TRYF(SEND_VAR(handle, entry->size, vartype, varname));
		TRYF(RECV_ACK(handle, NULL));

		TRYF(RECV_CTS(handle));
		TRYF(SEND_ACK(handle));

		memcpy(buffer + 4, entry->data, entry->size);
		TRYF(SEND_XDP(handle, entry->size + 4, buffer));
		TRYF(RECV_ACK(handle, &status));

		TRYF(SEND_EOT(handle));
		TRYF(RECV_ACK(handle, NULL));

		ticalcs_info("Sent variable #%u", i);

		update_->cnt2 = i+1;
		update_->max2 = content->num_entries;
		update_->pbar();
	}

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	uint16_t status;
	VarEntry *ve;
	uint16_t unused;
	char varname[18];

	content->model = CALC_TI92;
	ticalcs_strlcpy(content->comment, tifiles_comment_set_single(), sizeof(content->comment));
	content->num_entries = 1;
	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);

	ticonv_varname_to_utf8_sn(handle->model, varname, update_->text, sizeof(update_->text), vr->type);
	update_label();

	TRYF(SEND_REQ(handle, 0, vr->type, varname));
	TRYF(RECV_ACK(handle, &status));
	if (status != 0)
	{
		return ERR_MISSING_VAR;
	}

	TRYF(RECV_VAR(handle, &ve->size, &ve->type, ve->name));
	TRYF(SEND_ACK(handle));

	TRYF(SEND_CTS(handle));
	TRYF(RECV_ACK(handle, NULL));

	ve->data = tifiles_ve_alloc_data(ve->size + 4);
	TRYF(RECV_XDP(handle, &unused, ve->data));
	memmove(ve->data, ve->data + 4, ve->size);
	TRYF(SEND_ACK(handle));

	TRYF(RECV_EOT(handle));
	return SEND_ACK(handle);
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return send_var_92(handle, mode, content);
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
{
	uint16_t unused;
	int nvar, ret = 0;
	char tipath[18];
	char *tiname;

	content->model = handle->model;
	content->num_entries = 0;

	// receive packets
	for (nvar = 0;; nvar++)
	{
		VarEntry *ve = tifiles_ve_create();
		int ret2;

		ticalcs_strlcpy(ve->folder, "main", sizeof(ve->folder));

		ret = RECV_VAR(handle, &ve->size, &ve->type, tipath);
		ret2 = SEND_ACK(handle);

		if (ret)
		{
			if (ret == ERR_EOT)	// end of transmission
			{
				ret = 0;
			}
			goto error;
		}
		if (ret2)
		{
			ret = ret2;
			goto error;
		}

		// from Christian (calculator can send varname or fldname/varname)
		if ((tiname = strchr(tipath, '\\')) != NULL) 
		{
			*tiname = '\0';
			ticalcs_strlcpy(ve->folder, tipath, sizeof(ve->folder));
			ticalcs_strlcpy(ve->name, tiname + 1, sizeof(ve->name));
		}
		else 
		{
			ticalcs_strlcpy(ve->folder, "main", sizeof(ve->folder));
			ticalcs_strlcpy(ve->name, tipath, sizeof(ve->name));
		}

		ticonv_varname_to_utf8_sn(handle->model, ve->name, update_->text, sizeof(update_->text), ve->type);
		update_label();

		ret = SEND_CTS(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ve->data = tifiles_ve_alloc_data(ve->size + 4);
				ret = RECV_XDP(handle, &unused, ve->data);
				if (!ret)
				{
					memmove(ve->data, ve->data + 4, ve->size);
					ret = SEND_ACK(handle);
				}
			}
		}

		if (!ret)
		{
			tifiles_content_add_entry(content, ve);
		}
		else
		{
error:
			tifiles_ve_delete(ve);
			break;
		}
	}

	if (nvar > 1)
	{
		*vr = NULL;
	}
	else
	{
		*vr = tifiles_ve_dup(content->entries[0]);
	}

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	int err;
	// Go back to homescreen
	PAUSE(200);
	TRYF(send_key(handle, (KEY92P_CTRL + KEY92P_Q)));
	TRYF(send_key(handle, KEY92P_CLEAR));
	TRYF(send_key(handle, KEY92P_CLEAR));
	PAUSE(200);

	// Send dumping program
	err = rd_send(handle, "romdump.92p", romDumpSize92, romDump92);
	PAUSE(1000);

	return err;
}

// same code as calc_89.c
static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	// Launch program by remote control
	TRYF(send_key(handle, 'm'));
	TRYF(send_key(handle, 'a'));
	TRYF(send_key(handle, 'i'));
	TRYF(send_key(handle, 'n'));
	TRYF(send_key(handle, '\\'));
	TRYF(send_key(handle, 'r'));
	TRYF(send_key(handle, 'o'));
	TRYF(send_key(handle, 'm'));
	TRYF(send_key(handle, 'd'));
	TRYF(send_key(handle, 'u'));
	TRYF(send_key(handle, 'm'));
	TRYF(send_key(handle, 'p'));
	TRYF(send_key(handle, KEY92P_LP));
	TRYF(send_key(handle, KEY92P_RP));
	TRYF(send_key(handle, KEY92P_ENTER));
	PAUSE(200);

	// Get dump
	return rd_dump(handle, filename);
}

static int		del_var_92		(CalcHandle* handle, VarRequest* vr)
{
	int i;
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	ticalcs_slprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	update_label();

	send_key(handle, KEY92P_ON);
	send_key(handle, KEY92P_ESC);
	send_key(handle, KEY92P_ESC);
	send_key(handle, KEY92P_ESC);
	send_key(handle, KEY92P_2ND + KEY92P_ESC);
	send_key(handle, KEY92P_2ND + KEY92P_ESC);
	send_key(handle, KEY92P_CTRL + KEY92P_Q);
	send_key(handle, KEY92P_CLEAR);
	send_key(handle, KEY92P_CLEAR);
	send_key(handle, 'd');
	send_key(handle, 'e');
	send_key(handle, 'l');
	send_key(handle, 'v');
	send_key(handle, 'a');
	send_key(handle, 'r');
	send_key(handle, KEY92P_SPACE);

	for (i = 0; i < (int)strlen(varname); i++)
	{
		send_key(handle, varname[i]);
	}

	send_key(handle, KEY92P_ENTER);

	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, "a1234567");
	utf8 = ticonv_varname_to_utf8(handle->model, vr->folder, -1);
	ticalcs_slprintf(update_->text, sizeof(update_->text), _("Creating %s..."), utf8);
	ticonv_utf8_free(utf8);
	update_label();

	// send empty expression
	TRYF(SEND_RTS(handle, 0x10, 0x00, varname));
	TRYF(RECV_ACK(handle, NULL));

	TRYF(RECV_CTS(handle));
	TRYF(SEND_ACK(handle));

	TRYF(SEND_XDP(handle, 0x10, data));
	TRYF(RECV_ACK(handle, NULL));

	TRYF(SEND_EOT(handle));
	TRYF(RECV_ACK(handle, NULL));

	PAUSE(250);

	// delete 'a1234567' variable
	ticalcs_strlcpy(vr->name, "a1234567", sizeof(vr->name));
	return del_var_92(handle, vr);
}

static int		get_version_92	(CalcHandle* handle, CalcInfos* infos)
{
	uint32_t size;
	uint8_t type;
	char name[32];

	TRYF(SEND_REQ(handle, 0, TI92_BKUP, "main\\version"));
	TRYF(RECV_ACK(handle, NULL));
    
	TRYF(RECV_VAR(handle, &size, &type, name));
	TRYF(SEND_EOT(handle));

	memset(infos, 0, sizeof(CalcInfos));
	strncpy(infos->os_version, name, 4);
	infos->os_version[4] = 0;
	infos->hw_version = 1;
	infos->mask = INFOS_OS_VERSION | INFOS_HW_VERSION;

	ticalcs_info(_("  OS: %s"), infos->os_version);
	ticalcs_info(_("  Battery: %s"), infos->battery ? "good" : "low");

	return 0;
}

const CalcFncts calc_92 = 
{
	CALC_TI92,
	"TI92",
	"TI-92",
	"TI-92",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION |
	FTS_SILENT | FTS_FOLDER | FTS_BACKUP | FTS_NONSILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P",   /* send_backup */
	 "1P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "",     /* send_all_vars_backup */
	 ""      /* recv_all_vars_backup */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist_92,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_92,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&del_var_92,
	&new_folder,
	&get_version_92,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
};
