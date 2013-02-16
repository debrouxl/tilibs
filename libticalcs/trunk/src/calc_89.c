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
	TI89/TI92+/V200/TI89 Titanium support.
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
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "cmd89.h"
#include "rom89.h"
#include "romdump.h"
#include "keys89.h"
#include "keys92p.h"

// Screen coordinates of the TI89
#define TI89_ROWS          128
#define TI89_COLS          240
#define TI89_ROWS_VISIBLE  100
#define TI89_COLS_VISIBLE  160

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	TRYF(ti89_send_RDY(handle));
	TRYF(ti89_recv_ACK(handle, &status));

	return (MSB(status) & 0x01) ? ERR_NOT_READY : 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	TRYF(ti89_send_KEY(handle, key));
	TRYF(ti89_recv_ACK(handle, &key));
	PAUSE(50);

	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	unsigned int i;

	if(ve->type == TI89_APPL)
		return ERR_VOID_FUNCTION;

	// Go back to homescreen
	PAUSE(200);
	if(handle->model == CALC_TI89 || handle->model == CALC_TI89T)
	  {
	    TRYF(send_key(handle, KEY89_HOME));
	    TRYF(send_key(handle, KEY89_CLEAR));
	    TRYF(send_key(handle, KEY89_CLEAR));
	  }
	else if(handle->model == CALC_TI92P || handle->model == CALC_V200)
	  {
	    // TI92+ or V200
	    TRYF(send_key(handle, KEY92P_CTRL + KEY92P_Q));
	    TRYF(send_key(handle, KEY92P_CLEAR));
	    TRYF(send_key(handle, KEY92P_CLEAR));
	  }

	// Launch program by remote control
	for(i = 0; i < strlen(ve->folder); i++)
		TRYF(send_key(handle, (ve->folder)[i]));

    if(strcmp(ve->folder, ""))
		TRYF(send_key(handle, '\\'));

	for(i = 0; i < strlen(ve->name); i++)
		TRYF(send_key(handle, (ve->name)[i]));

	TRYF(send_key(handle, KEY89_LP));
	if(args)
	{
		for(i = 0; i < strlen(args); i++)
			TRYF(send_key(handle, args[i]));
	}
    TRYF(send_key(handle, KEY89_RP));

    TRYF(send_key(handle, KEY89_ENTER));
	PAUSE(200);

	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint32_t max_cnt;
	int err;
	uint8_t buf[TI89_COLS * TI89_ROWS / 8];

	sc->width = TI89_COLS;
	sc->height = TI89_ROWS;
	switch (handle->model) 
	{
	case CALC_TI89:
	case CALC_TI89T:
		sc->clipped_width = TI89_COLS_VISIBLE;
		sc->clipped_height = TI89_ROWS_VISIBLE;
		break;
	case CALC_TI92P:
	case CALC_V200:
		sc->clipped_width = TI89_COLS;
		sc->clipped_height = TI89_ROWS;
		break;
	default:
		sc->clipped_width = TI89_COLS;
		sc->clipped_height = TI89_ROWS;
		break;
	}

	TRYF(ti89_send_SCR(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	err = ti89_recv_XDP(handle, &max_cnt, buf);	// pb with checksum
	if(err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti89_send_ACK(handle));

	*bitmap = (uint8_t *)g_malloc(TI89_COLS * TI89_ROWS / 8);
	if(*bitmap == NULL) 
		return ERR_MALLOC;
	memcpy(*bitmap, buf, TI89_COLS * TI89_ROWS / 8);

	// Clip the unused part of the screen (nethertheless useable witha asm prog)
	if(((handle->model == CALC_TI89) || (handle->model == CALC_TI89T))
      && (sc->format == SCREEN_CLIPPED)) 
	{
		int i, j, k;

		for(i = 0, j = 0; j < TI89_ROWS_VISIBLE; j++)
			for(k = 0; k < (TI89_COLS_VISIBLE >> 3); k++)
				(*bitmap)[i++] = (*bitmap)[j * (TI89_COLS >> 3) + k];
	}

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	TreeInfo *ti;
	VarEntry info;
	uint32_t block_size;
	uint8_t buffer[65536];
    int i, j;
    uint8_t extra = (handle->model == CALC_V200) ? 8 : 0;
	GNode *root;

	(*apps) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = APP_NODE_NAME;
	(*apps)->data = ti;

    (*vars) = g_node_new(NULL);
	ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
	ti->model = handle->model;
	ti->type = VAR_NODE_NAME;
	(*vars)->data = ti;

	root = g_node_new(NULL);
	g_node_append(*apps, root);

    TRYF(ti89_send_REQ(handle, TI89_FDIR << 24, TI89_RDIR, ""));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_recv_VAR(handle, &info.size, &info.type, info.name));
    TRYF(ti89_send_ACK(handle));

    TRYF(ti89_send_CTS(handle));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_recv_XDP(handle, &block_size, buffer));
    TRYF(ti89_send_ACK(handle));

    TRYF(ti89_recv_EOT(handle));
    TRYF(ti89_send_ACK(handle));

	for(j = 4; j < (int)block_size;) 
	{
		VarEntry *fe = tifiles_ve_create();
        GNode *node;

        memcpy(fe->name, buffer + j, 8);
        fe->name[8] = '\0';
        fe->type = buffer[j + 8];
        fe->attr = buffer[j + 9];
        fe->size = buffer[j + 10] | (buffer[j + 11] << 8) | (buffer[j + 12] << 16);	// | (buffer[j+13] << 24);
        j += 14 + extra;
        strcpy(fe->folder, "");

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			fe->name, 
			tifiles_vartype2string(handle->model, fe->type),
			fe->attr,
			fe->size);

        if(fe->type == TI89_DIR)
		{
			node = g_node_new(fe);
			g_node_append(*vars, node);
		}
        else if(fe->type == TI89_APPL)
		{
			// AMS<2.08 returns FLASH apps
			continue;
		}
	}

	// get list of variables into each folder
	for(i = 0; i < (int)g_node_n_children(*vars); i++) 
	{
		GNode *folder = g_node_nth_child(*vars, i);
		char *folder_name = ((VarEntry *) (folder->data))->name;
		char *u1, *u2;

		ticalcs_info(_("Directory listing in %8s..."), folder_name);

		TRYF(ti89_send_REQ(handle, TI89_LDIR << 24, TI89_RDIR, folder_name));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_VAR(handle, &info.size, &info.type, info.name));
		TRYF(ti89_send_ACK(handle));

		TRYF(ti89_send_CTS(handle));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_XDP(handle, &block_size, buffer));
		TRYF(ti89_send_ACK(handle));

		TRYF(ti89_recv_EOT(handle));
		TRYF(ti89_send_ACK(handle));

		for(j = 4 + 14 + extra; j < (int)block_size;) 
		{
			VarEntry *ve = tifiles_ve_create();
			GNode *node;

			memcpy(ve->name, buffer + j, 8);
			ve->name[8] = '\0';
			ve->type = buffer[j + 8];
			ve->attr = buffer[j + 9];
			ve->size = buffer[j + 10] | (buffer[j + 11] << 8) | (buffer[j + 12] << 16);	// | (buffer[j+13] << 24);
			j += 14 + extra;
			strcpy(ve->folder, folder_name);

			ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			ve->name, 
			tifiles_vartype2string(handle->model, ve->type),
			ve->attr,
			ve->size);

			u1 = ticonv_varname_to_utf8(handle->model, ((VarEntry *) (folder->data))->name, -1);
			u2 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
			g_snprintf(update_->text, sizeof(update_->text), _("Parsing %s/%s"), u1, u2);
			g_free(u1); g_free(u2);
			update_label();

			if(ve->type == TI89_APPL) 
			{
				VarEntry arg;

				memset(&arg, 0, sizeof(arg));
				strcpy(arg.name, ve->name);
				if(!ticalcs_dirlist_ve_exist(*apps, &arg))
				{
					strcpy(ve->folder, "");
					node = g_node_new(ve);
					g_node_append(root, node);
				}
			} 
			else
			{
				if(!strcmp(ve->folder, "main") && 
					(!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
				{
					tifiles_ve_delete(ve);
				}
				else
				{
					node = g_node_new(ve);
					g_node_append(folder, node);
				}
			}
		}
		
		ticalcs_info("");
	}

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	*ram = *flash = -1;
	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint16_t status;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for(i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		uint8_t buffer[65536 + 4] = { 0 };
		uint8_t vartype = entry->type;
		char varname[18];

		if(entry->action == ACT_SKIP)
			continue;

		if((mode & MODE_LOCAL_PATH) && !(mode & MODE_BACKUP)) 
		{	
			// local & not backup
			strcpy(varname, entry->name);
		} 
		else 
		{
			// full or backup
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}

		utf8 = ticonv_varname_to_utf8(handle->model, varname, vartype);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		switch (entry->attr) 
		{
		//case ATTRB_NONE:     vartype = TI89_BKUP; break;
		case ATTRB_LOCKED:   vartype = 0x26; break;
		case ATTRB_PROTECTED:
		case ATTRB_ARCHIVED: vartype = 0x27; break;
		}

		TRYF(ti89_send_RTS(handle, entry->size, vartype, varname));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_CTS(handle));
		TRYF(ti89_send_ACK(handle));

		memcpy(buffer + 4, entry->data, entry->size);
		TRYF(ti89_send_XDP(handle, entry->size + 4, buffer));
		TRYF(ti89_recv_ACK(handle, &status));

		TRYF(ti89_send_EOT(handle));
		TRYF(ti89_recv_ACK(handle, NULL));

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
	char  varname[20];
	char *utf8;

	ve = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	TRYF(ti89_send_REQ(handle, 0, vr->type, varname));
	TRYF(ti89_recv_ACK(handle, &status));
	if(status != 0)
		return ERR_MISSING_VAR;

	TRYF(ti89_recv_VAR(handle, &ve->size, &ve->type, ve->name));
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_send_CTS(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	ve->data = tifiles_ve_alloc_data(ve->size + 4);
	TRYF(ti89_recv_XDP(handle, &unused, ve->data));
	memmove(ve->data, ve->data + 4, ve->size);
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_recv_EOT(handle));
	TRYF(ti89_send_ACK(handle));

	PAUSE(250);

	tifiles_content_add_entry(content, ve);

	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	// erase memory
	TRYF(ti89_send_VAR(handle, 0, TI89_BKUP, "main"));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_CTS(handle));
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_send_EOT(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	// next, send var(s)
	TRYF(send_var(handle, MODE_BACKUP, (FileContent *)content));

	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int i;
	uint16_t status;
	char *utf8;

	update_->cnt2 = 0;
	update_->max2 = content->num_entries;

	for(i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		uint8_t buffer[65536 + 4] = { 0 };
		uint8_t vartype = entry->type;
		char varname[18];

		if(entry->action == ACT_SKIP)
			continue;

		if((mode & MODE_LOCAL_PATH) && !(mode & MODE_BACKUP)) 
		{	
			// local & not backup
			strcpy(varname, entry->name);
		} 
		else 
		{
			// full or backup
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}

		utf8 = ticonv_varname_to_utf8(handle->model, varname, vartype);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti89_send_VAR(handle, entry->size, vartype, varname));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_CTS(handle));
		TRYF(ti89_send_ACK(handle));

		memcpy(buffer + 4, entry->data, entry->size);
		TRYF(ti89_send_XDP(handle, entry->size + 4, buffer));
		TRYF(ti89_recv_ACK(handle, &status));

		TRYF(ti89_send_EOT(handle));
		TRYF(ti89_recv_ACK(handle, NULL));

		if(mode & MODE_BACKUP) 
		{
			update_->cnt2 = i+1;
			update_->max2 = content->num_entries;
			update_->pbar();
		}
	}

	return 0;
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
		ve = content->entries[nvar-1] = tifiles_ve_create();
		strcpy(ve->folder, "main");	

		err = ti89_recv_VAR(handle, &ve->size, &ve->type, tipath);
		TRYF(ti89_send_ACK(handle));

		if(err == ERR_EOT)	// end of transmission
			goto exit;
		else
			content->num_entries = nvar;

		// from Christian (TI can send varname or fldname/varname)
        if((tiname = strchr(tipath, '\\')) != NULL) 
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

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		TRYF(ti89_send_CTS(handle));
		TRYF(ti89_recv_ACK(handle, NULL));

		ve->data = tifiles_ve_alloc_data(ve->size + 4);
		TRYF(ti89_recv_XDP(handle, &unused, ve->data));
		memmove(ve->data, ve->data + 4, ve->size);
		TRYF(ti89_send_ACK(handle));
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
	FlashContent *ptr;
	int i, nblocks;
	char *utf8;

	// send all headers except license
	for(ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if(ptr->data_type == TI89_LICENSE)
			continue;

		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		utf8 = ticonv_varname_to_utf8(handle->model, ptr->name, ptr->data_type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		if(ptr->data_type == TI89_AMS) 
		{
		  if(handle->model == CALC_TI89T || handle->model == CALC_V200)
		  {
			TRYF(ti89_send_RTS2(handle, ptr->data_length, ptr->data_type, ptr->hw_id));
		  }
		  else
		  {
			TRYF(ti89_send_RTS(handle, ptr->data_length, ptr->data_type, ""));
		  }
		} 
		else 
		{
			TRYF(ti89_send_RTS(handle, ptr->data_length, ptr->data_type, ptr->name));
		}

		nblocks = ptr->data_length / 65536;
		update_->max2 = nblocks+1;

		for(i = 0; i <= nblocks; i++) 
		{
			uint32_t length = (i != nblocks) ? 65536 : ptr->data_length % 65536;

			TRYF(ti89_recv_ACK(handle, NULL));

			TRYF(ti89_recv_CTS(handle));
			TRYF(ti89_send_ACK(handle));

			TRYF(ti89_send_XDP(handle, length, (ptr->data_part) + 65536 * i));
			TRYF(ti89_recv_ACK(handle, NULL));

			if(i != nblocks) 
			{
			  TRYF(ti89_send_CNT(handle));
			} 
			else 
			{
			  TRYF(ti89_send_EOT(handle));
			}

			update_->cnt2 = i;
			update_->pbar();
		}

		TRYF(ti89_recv_ACK(handle, NULL));
		ticalcs_info(_("Header sent completely."));
	}

	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	int i;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	content->model = handle->model;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(2 * 1024 * 1024);	// 2MB max
	content->data_type = vr->type;
	switch(handle->model)
	{
	case CALC_TI89:
	case CALC_TI89T: content->device_type = DEVICE_TYPE_89; break;
	case CALC_TI92P:
	case CALC_V200:  content->device_type = DEVICE_TYPE_92P; break;
	default: return ERR_FATAL_ERROR;
	}

	TRYF(ti89_send_REQ(handle, 0x00, vr->type, vr->name));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_VAR(handle, &content->data_length, &content->data_type, content->name));

	update_->cnt2 = 0;
	update_->max2 = vr->size;

	for(i = 0, content->data_length = 0;; i++) 
	{
		int err;
		uint32_t block_size;

		TRYF(ti89_send_ACK(handle));

		TRYF(ti89_send_CTS(handle));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_XDP(handle, &block_size, content->data_part + content->data_length));
		TRYF(ti89_send_ACK(handle));

		content->data_length += block_size;

		err = ti89_recv_CNT(handle);
		if(err == ERR_EOT)
			break;
		TRYF(err);

		update_->cnt2 += block_size;
		update_->pbar();
	}

	TRYF(ti89_send_ACK(handle));

	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	uint32_t varsize;
	uint8_t vartype;
	char varname[9];

	g_snprintf(update_->text, sizeof(update_->text), "ID-LIST");
	update_label();

	TRYF(ti89_send_REQ(handle, 0x0000, TI89_IDLIST, ""));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_VAR(handle, &varsize, &vartype, varname));
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_send_CTS(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_XDP(handle, &varsize, idlist));
	memcpy(idlist, idlist+8, varsize - 8);
	idlist[varsize - 8] = '\0';
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_recv_EOT(handle));
	TRYF(ti89_send_ACK(handle));

	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Go back to homescreen
	PAUSE(200);
	if(handle->model == CALC_TI89 || handle->model == CALC_TI89T)
	  {
	    TRYF(send_key(handle, KEY89_HOME));
	    TRYF(send_key(handle, KEY89_CLEAR));
	    TRYF(send_key(handle, KEY89_CLEAR));
	  }
	else if(handle->model == CALC_TI92P || handle->model == CALC_V200)
	  {
	    // TI92+ or V200
	    TRYF(send_key(handle, KEY92P_CTRL + KEY92P_Q));
	    TRYF(send_key(handle, KEY92P_CLEAR));
	    TRYF(send_key(handle, KEY92P_CLEAR));
	  }
	PAUSE(200);

	// Send dumping program
	TRYF(rd_send(handle, "romdump.89z", romDumpSize89, romDump89));
	PAUSE(1000);

	return 0;
}

// same code as calc_92.c
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
    TRYF(send_key(handle, KEY89_LP));
    TRYF(send_key(handle, KEY89_RP));
    TRYF(send_key(handle, KEY89_ENTER));
	PAUSE(200);

	// Get dump
	TRYF(rd_dump(handle, filename));

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
    uint8_t buffer[16] = { 0 };
    uint16_t status;

    buffer[6] = _clock->year >> 8;
    buffer[7] = _clock->year & 0x00ff;
    buffer[8] = _clock->month;
    buffer[9] = _clock->day;
    buffer[10] = _clock->hours;
    buffer[11] = _clock->minutes;
    buffer[12] = _clock->seconds;
    buffer[13] = _clock->date_format;
    buffer[14] = _clock->time_format;
    buffer[15] = 0xff;

    g_snprintf(update_->text, sizeof(update_->text), _("Setting clock..."));
    update_label();

    TRYF(ti89_send_RTS(handle, 0x10, TI89_CLK, "Clock"));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_recv_CTS(handle));
    TRYF(ti89_send_ACK(handle));

    TRYF(ti89_send_XDP(handle, 0x10, buffer));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_send_EOT(handle));
    TRYF(ti89_recv_ACK(handle, &status));

	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint32_t varsize;
    uint8_t vartype;
   char varname[9];
    uint8_t buffer[32];

    g_snprintf(update_->text, sizeof(update_->text), _("Getting clock..."));
    update_label();

    TRYF(ti89_send_REQ(handle, 0x0000, TI89_CLK, "Clock"));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_recv_VAR(handle, &varsize, &vartype, varname));
    TRYF(ti89_send_ACK(handle));

    TRYF(ti89_send_CTS(handle));
    TRYF(ti89_recv_ACK(handle, NULL));

    TRYF(ti89_recv_XDP(handle, &varsize, buffer));
    TRYF(ti89_send_ACK(handle));

    TRYF(ti89_recv_EOT(handle));
    TRYF(ti89_send_ACK(handle));

    _clock->year = (buffer[6] << 8) | buffer[7];
    _clock->month = buffer[8];
    _clock->day = buffer[9];
    _clock->hours = buffer[10];
    _clock->minutes = buffer[11];
    _clock->seconds = buffer[12];
    _clock->date_format = buffer[13];
    _clock->time_format = buffer[14];

	return 0;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	return 0;
}

static int		change_attr	(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	TRYF(ti89_send_DEL(handle, vr->size, vr->type, varname));
	TRYF(ti89_recv_ACK(handle, NULL));
	TRYF(ti89_recv_ACK(handle, NULL));

	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char varname[18];
	char *utf8;

	tifiles_build_fullname(handle->model, varname, vr->folder, "a1234567");
	utf8 = ticonv_varname_to_utf8(handle->model, vr->folder, -1);
	g_snprintf(update_->text, sizeof(update_->text), _("Creating %s..."), utf8);
	g_free(utf8);
	update_label();

	// send empty expression
	TRYF(ti89_send_RTS(handle, 0x10, 0x00, varname));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_CTS(handle));
	TRYF(ti89_send_ACK(handle));

	TRYF(ti89_send_XDP(handle, 0x10, data));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_send_EOT(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	PAUSE(250);

	// delete 'a1234567' variable
	strcpy(vr->name, "a1234567");
	TRYF(del_var(handle, vr));

	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint32_t length;
	uint8_t buf[32];

	TRYF(ti89_send_VER(handle));
	TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_send_CTS(handle));
    TRYF(ti89_recv_ACK(handle, NULL));

	TRYF(ti89_recv_XDP(handle, &length, buf));
    TRYF(ti89_send_ACK(handle));

	memset(infos, 0, sizeof(CalcInfos));
	g_snprintf(infos->os_version, 5, "%1i.%02i", buf[0], buf[1]);
	g_snprintf(infos->boot_version, 5, "%1i.%02i", buf[2], buf[3]);
	infos->battery = buf[4] == 1 ? 0 : 1;
	switch(buf[13])
	{
	case 1:
	case 3: infos->hw_version = buf[5] + 1; break;
	case 8: infos->hw_version = buf[5]; break;
	case 9: infos->hw_version = buf[5] + 1; break;
	}
	switch(buf[13])
	{
	case 1: infos->model = CALC_TI92P; break;
	case 3: infos->model = CALC_TI89; break;
	case 8: infos->model = CALC_V200; break;
	case 9: infos->model = CALC_TI89T; break;
	}
	infos->language_id = buf[6];
	infos->sub_lang_id = buf[7];
	infos->mask = INFOS_BOOT_VERSION | INFOS_OS_VERSION | INFOS_BATTERY | INFOS_HW_VERSION | INFOS_CALC_MODEL | INFOS_LANG_ID | INFOS_SUB_LANG_ID;

	tifiles_hexdump(buf, length);
	ticalcs_info(_("  OS: %s"), infos->os_version);
	ticalcs_info(_("  BIOS: %s"), infos->boot_version);
	ticalcs_info(_("  Battery: %s"), infos->battery ? "good" : "low");

	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	return send_flash(handle, content);
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	VarEntry ve;
	int ret = 0;

	memset(&ve, 0, sizeof(VarEntry));
	ve.type = TI89_GETCERT;
	strcpy(ve.name, "");

	ret = recv_flash(handle, content, &ve);

	// fix up for certificate
	memmove(content->data_part, content->data_part + 4, content->data_length - 4);
	content->data_type = TI89_CERTIF;
	switch(handle->model)
	{
	case CALC_TI89:  content->device_type = DEVICE_TYPE_89; break;
	case CALC_TI89T: content->device_type = DEVICE_TYPE_89; break;
	case CALC_TI92P: content->device_type = DEVICE_TYPE_92P; break;
	case CALC_V200:  content->device_type = DEVICE_TYPE_92P; break;
	default: content->device_type = DEVICE_TYPE_89; break;
	}
	strcpy(content->name, "");

	return ret;
}

extern int tixx_recv_backup(CalcHandle* handle, BackupContent* content);

const CalcFncts calc_89 = 
{
	CALC_TI89,
	"TI89",
	"TI-89",
	"TI-89",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
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
	&rename_var,
	&change_attr
};

const CalcFncts calc_92p = 
{
	CALC_TI92P,
	"TI92+",
	"TI-92 Plus",
	"TI-92 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
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
	&rename_var,
	&change_attr
};

const CalcFncts calc_89t = 
{
	CALC_TI89T,
	"Titanium",
	"TI-89 Titanium",
	"TI-89 Titanium",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP | 
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
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
	&rename_var,
	&change_attr
};

const CalcFncts calc_v2 = 
{
	CALC_V200,
	"V200",
	"V200PLT",
	N_("V200 Portable Learning Tool"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
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
	&rename_var,
	&change_attr
};
