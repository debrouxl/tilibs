/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
	TI92 support.
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

#include "cmd92.h"
#include "keys92p.h"
#include "rom92f2.h"
#include "romdump.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the TI92
#define TI92_ROWS  128
#define TI92_COLS  240

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	TRYF(ti92_send_RDY());
	TRYF(ti92_recv_ACK(&status));

	return (MSB(status) & 0x01) ? ERR_NOT_READY : 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti92_send_KEY(key));
	TRYF(ti92_recv_ACK(&key));
	
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint32_t max_cnt;
	int err;
	uint8_t buf[TI92_COLS * TI92_ROWS / 8];

	sc->width = TI92_COLS;
	sc->height = TI92_ROWS;
	sc->clipped_width = TI92_COLS;
	sc->clipped_height = TI92_ROWS;

	TRYF(ti92_send_SCR());
	TRYF(ti92_recv_ACK(NULL));

	err = ti92_recv_XDP(&max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti92_send_ACK());

	*bitmap = (uint8_t *) malloc(TI92_COLS * TI92_ROWS / 8);
	if(*bitmap == NULL)
		return ERR_MALLOC;
	memcpy(*bitmap, buf, TI92_COLS * TI92_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{
	TreeInfo *ti;
	VarEntry info;
	uint32_t unused;
	uint8_t buffer[65536];
	int err;
	char folder_name[9] = "";
	TNode *folder = NULL;
	char *utf8;

	// get list of folders & FLASH apps
    (*vars) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	(*apps) = t_node_new(NULL);
	ti = (TreeInfo *)malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

	TRYF(ti92_send_REQ(0, TI92_RDIR, ""));
	TRYF(ti92_recv_ACK(NULL));
	TRYF(ti92_recv_VAR(&info.size, &info.type, info.name));

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		TNode *node;

		TRYF(ti92_send_ACK());
		TRYF(ti92_send_CTS());

		TRYF(ti92_recv_ACK(NULL));
		TRYF(ti92_recv_XDP(&unused, buffer));

		memcpy(ve->name, buffer + 4, 8);	// skip 4 extra 00s
		ve->name[8] = '\0';
		ve->type = buffer[12];
		ve->attr = buffer[13];
		ve->size = buffer[14] | (buffer[15] << 8) | (buffer[16] << 16) | (buffer[17] << 24);
		strcpy(ve->folder, "");

		if (ve->type == TI92_DIR) 
		{
			strcpy(folder_name, ve->name);
			node = t_node_new(ve);
			folder = t_node_append(*vars, node);
		} 
		else 
		{
			strcpy(ve->folder, folder_name);

			if(!strcmp(ve->folder, "main") && 
					(!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
			{
				tifiles_ve_delete(ve);
			}
			else
			{
				node = t_node_new(ve);
				t_node_append(folder, node);
			}
		}

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			ve->name, 
			tifiles_vartype2string(handle->model, ve->type),
			ve->attr,
			ve->size);

		TRYF(ti92_send_ACK());
		err = ti92_recv_CNT();
		if (err == ERR_EOT)
			break;
		TRYF(err);

		utf8 = ticonv_varname_to_utf8(handle->model,ve->name);
		snprintf(update_->text, sizeof(update_->text), _("Parsing %s/%s"),
			((VarEntry *) (folder->data))->name, utf8);
		g_free(utf8);
		update_->label();
	}

	TRYF(ti92_send_ACK());

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	*ram = *flash = -1;
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	int i;
	int nblocks;

	TRYF(ti92_send_VAR(content->data_length, TI92_BKUP, content->rom_version));
	TRYF(ti92_recv_ACK(NULL));

	update_->cnt2 = 0;
	nblocks = content->data_length / 1024;
	handle->updat->max2 = nblocks;

	for (i = 0; i <= nblocks; i++) 
	{
		uint32_t length = (i != nblocks) ? 1024 : content->data_length % 1024;

		TRYF(ti92_send_VAR(length, TI92_BKUP, content->rom_version));
		TRYF(ti92_recv_ACK(NULL));

		TRYF(ti92_recv_CTS());
		TRYF(ti92_send_ACK());

		TRYF(ti92_send_XDP(length, content->data_part + 1024 * i));
		TRYF(ti92_recv_ACK(NULL));

		handle->updat->cnt2 = i;
		update_pbar();
	}

	TRYF(ti92_send_EOT());

	return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	uint32_t block_size;
	int block, err;
	uint32_t unused;
	uint16_t unused2;
	uint8_t *ptr;

	TRYF(ti92_send_REQ(0, TI92_BKUP, "main\\backup"));
	TRYF(ti92_recv_ACK(&unused2));

	content->model = CALC_TI92;
	strcpy(content->comment, tifiles_comment_set_backup());
	content->data_part = tifiles_ve_alloc_data(128 * 1024);
	content->type = TI92_BKUP;
	content->data_length = 0;

	for (block = 0;; block++) 
	{
		snprintf(update_->text, sizeof(update_->text), _("Block #%2i"), block);
		update_label();
    
		err = ti92_recv_VAR(&block_size, &content->type, content->rom_version);
		TRYF(ti92_send_ACK());

		if (err == ERR_EOT)
			break;
		TRYF(err);

		TRYF(ti92_send_CTS());
		TRYF(ti92_recv_ACK(NULL));

		ptr = content->data_part + content->data_length;
		TRYF(ti92_recv_XDP(&unused, ptr));
		memmove(ptr, ptr + 4, block_size);
		TRYF(ti92_send_ACK());
		content->data_length += block_size;
	}

	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint16_t status;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		uint8_t buffer[65536 + 4] = { 0 };
		char varname[18];

		if(entry->action == ACT_SKIP)
			continue;

		if (mode & MODE_LOCAL_PATH)
		  strcpy(varname, entry->name);
		else 
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);

		utf8 = ticonv_varname_to_utf8(handle->model, varname);
		snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti92_send_VAR(entry->size, entry->type, varname));
		TRYF(ti92_recv_ACK(NULL));

		TRYF(ti92_recv_CTS());
		TRYF(ti92_send_ACK());

		memcpy(buffer + 4, entry->data, entry->size);
		TRYF(ti92_send_XDP(entry->size + 4, buffer));
		TRYF(ti92_recv_ACK(&status));

		TRYF(ti92_send_EOT());
		TRYF(ti92_recv_ACK(NULL));

		ticalcs_info("");

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
	uint32_t unused;
	char varname[18];
	char *utf8;

	content->model = CALC_TI92;
	strcpy(content->comment, tifiles_comment_set_single());
	content->num_entries = 1;
	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);

	utf8 = ticonv_varname_to_utf8(handle->model, varname);
	snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	TRYF(ti92_send_REQ(0, vr->type, varname));
	TRYF(ti92_recv_ACK(&status));
	if (status != 0)
		return ERR_MISSING_VAR;

	TRYF(ti92_recv_VAR(&ve->size, &ve->type, ve->name));
	TRYF(ti92_send_ACK());

	TRYF(ti92_send_CTS());
	TRYF(ti92_recv_ACK(NULL));

	ve->data = tifiles_ve_alloc_data(ve->size + 4);
	TRYF(ti92_recv_XDP(&unused, ve->data));
	memmove(ve->data, ve->data + 4, ve->size);
	TRYF(ti92_send_ACK());

	TRYF(ti92_recv_EOT());
	TRYF(ti92_send_ACK());

	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return send_var(handle, mode, content);
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
{
	uint32_t unused;
	int nvar, err;
    char tipath[18];
    char *tiname;
	char *utf8;

	content->model = handle->model;

	// receive packets
	for(nvar = 1;; nvar++)
	{
		VarEntry *ve;

		content->entries = tifiles_ve_resize_array(content->entries, nvar+1);
		ve = content->entries[nvar-1] = tifiles_ve_create();;
		strcpy(ve->folder, "main");	

		err = ti92_recv_VAR(&ve->size, &ve->type, tipath);
		TRYF(ti92_send_ACK());

		if(err == ERR_EOT)	// end of transmission
			goto exit;
		else
			content->num_entries = nvar;

		// from Christian (TI can send varname or fldname/varname)
        if ((tiname = strchr(tipath, '\\')) != NULL) 
		{
			*tiname = '\0';
            strcpy(ve->folder, tipath);
            strcpy(ve->name, tiname + 1);
        }
        else 
		{
            strcpy(ve->folder, "main");
            strcpy(ve->name, tipath);
        }

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name);
		snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti92_send_CTS());
		TRYF(ti92_recv_ACK(NULL));

		ve->data = tifiles_ve_alloc_data(ve->size + 4);
		TRYF(ti92_recv_XDP(&unused, ve->data));
		memmove(ve->data, ve->data + 4, ve->size);
		TRYF(ti92_send_ACK());
	}

exit:
	nvar--;
	if(nvar > 1) 
		*vr = NULL;
	else
		*vr = tifiles_ve_dup(content->entries[0]);

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

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	TRYF(rd_send(handle, "romdump.92p", romDumpSize92, romDump92));
	PAUSE(1000);

	return 0;
}

// same code as calc_89.c
static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	// Launch program by remote control
	PAUSE(200);
	TRYF(send_key(handle, KEY92P_CLEAR));
	PAUSE(50);
	TRYF(send_key(handle, KEY92P_CLEAR));
	PAUSE(50);
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
	TRYF(rd_dump(handle, filename));

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
	int i;
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname);
	snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	send_key(handle, KEY92P_ON);
	send_key(handle, KEY92P_ESC);
	send_key(handle, KEY92P_ESC);
	send_key(handle, KEY92P_ESC);
	send_key(handle, 4360/*KEY92P_QUIT*/);
	send_key(handle, 4360/*KEY92P_QUIT*/);
	send_key(handle, 8273/*KEY92P_HOME*/);
	send_key(handle, KEY92P_CLEAR);
	send_key(handle, KEY92P_CLEAR);
	send_key(handle, KEY92P_d);
	send_key(handle, KEY92P_e);
	send_key(handle, KEY92P_l);
	send_key(handle, KEY92P_v);
	send_key(handle, KEY92P_a);
	send_key(handle, KEY92P_r);
	send_key(handle, KEY92P_SPACE);

	for(i = 0; i < (int)strlen(varname); i++)
		send_key(handle, varname[i]);

	send_key(handle, KEY92P_ENTER);

	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, "a1234567");
	utf8 = ticonv_varname_to_utf8(handle->model, vr->folder);
	snprintf(update_->text, sizeof(update_->text), _("Creating %s..."), utf8);
	g_free(utf8);
	update_label();

	// send empty expression
	TRYF(ti92_send_RTS(0x10, 0x00, varname));
	TRYF(ti92_recv_ACK(NULL));

	TRYF(ti92_recv_CTS());
	TRYF(ti92_send_ACK());

	TRYF(ti92_send_XDP(0x10, data));
	TRYF(ti92_recv_ACK(NULL));

	TRYF(ti92_send_EOT());
	TRYF(ti92_recv_ACK(NULL));

	// delete 'a1234567' variable
	strcpy(vr->name, "a1234567");
	TRYF(del_var(handle, vr));

	return 0;
}

static int		get_version(CalcHandle* handle, CalcInfos* infos)
{
	uint32_t size;
	uint8_t type;
	char name[32];

	TRYF(ti92_send_REQ(0, TI92_BKUP, "main\\version"));
	TRYF(ti92_recv_ACK(NULL));
    
	TRYF(ti92_recv_VAR(&size, &type, name));
	TRYF(ti92_send_EOT());

	memset(infos, 0, sizeof(CalcInfos));
	strncpy(infos->os_version, name, 4);
	infos->hw_version = 1;
	infos->mask = INFOS_OS_VERSION | INFOS_HW_VERSION;

	ticalcs_info(_("  OS: %s"), infos->os_version);
	ticalcs_info(_("  Battery: %s"), infos->battery ? "good" : "low");

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

const CalcFncts calc_92 = 
{
	CALC_TI92,
	"TI92",
	N_("TI-92"),
	N_("TI-92"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION |
	FTS_SILENT | FTS_FOLDER | FTS_BACKUP,
	{"", "", "1P", "1L", "", "2P", "1P1L", "2P1L", "1P1L", "2P1L", "1P1L", "", "",
	"", "", "2P", "", "", "1L", "1L", "", "", "" },
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
	&send_flash,
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
