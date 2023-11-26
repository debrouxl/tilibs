/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
	TI89/TI92+/V200/TI89T/TI92 support.
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
#include "rom89.h"
#include "rom92f2.h"
#include "romdump.h"
#include "keys89.h"
#include "keys92p.h"

#define SEND_RDY(handle) (handle->model != CALC_TI92 ? ti89_send_RDY(handle) : ti92_send_RDY(handle))
#define SEND_KEY(handle, scancode) (handle->model != CALC_TI92 ? ti89_send_KEY(handle, scancode) : ti92_send_KEY(handle, scancode))
#define SEND_SCR(handle) (handle->model != CALC_TI92 ? ti89_send_SCR(handle) : ti92_send_SCR(handle))
#define SEND_ACK(handle) (handle->model != CALC_TI92 ? ti89_send_ACK(handle) : ti92_send_ACK(handle))
#define SEND_VAR(handle, varsize, vartype, varname) (handle->model != CALC_TI92 ? ti89_send_VAR(handle, varsize, vartype, varname) : ti92_send_VAR(handle, varsize, vartype, varname))
#define SEND_XDP(handle, length, data) (handle->model != CALC_TI92 ? ti89_send_XDP(handle, length, data) : ti92_send_XDP(handle, length, data))
#define SEND_REQ(handle, varsize, vartype, varname) (handle->model != CALC_TI92 ? ti89_send_REQ(handle, varsize, vartype, varname) : ti92_send_REQ(handle, varsize, vartype, varname))
#define SEND_RTS(handle, varsize, vartype, varname) (handle->model != CALC_TI92 ? ti89_send_RTS(handle, varsize, vartype, varname) : ti92_send_RTS(handle, varsize, vartype, varname))
#define SEND_RTS2 ti89_send_RTS2
#define SEND_CTS(handle) (handle->model != CALC_TI92 ? ti89_send_CTS(handle) : ti92_send_CTS(handle))
#define SEND_CNT(handle) (handle->model != CALC_TI92 ? ti89_send_CNT(handle) : ti92_send_CNT(handle))
#define SEND_DEL ti89_send_DEL
#define SEND_VER ti89_send_VER
#define SEND_EOT(handle) (handle->model != CALC_TI92 ? ti89_send_EOT(handle) : ti92_send_EOT(handle))

#define RECV_ACK(handle, status) (handle->model != CALC_TI92 ? ti89_recv_ACK(handle, status) : ti92_recv_ACK(handle, status))
#define RECV_VAR(handle, varsize, vartype, varname) (handle->model != CALC_TI92 ? ti89_recv_VAR(handle, varsize, vartype, varname) : ti92_recv_VAR(handle, varsize, vartype, varname))
#define RECV_XDP(handle, length, data) (handle->model != CALC_TI92 ? ti89_recv_XDP(handle, length, data) : ti92_recv_XDP(handle, length, data))
#define RECV_CTS(handle) (handle->model != CALC_TI92 ? ti89_recv_CTS(handle) : ti92_recv_CTS(handle))
#define RECV_SKP(handle, rej_code) (handle->model != CALC_TI92 ? ti89_recv_SKP(handle, rej_code) : ti92_recv_SKP(handle, rej_code))
#define RECV_CNT(handle) (handle->model != CALC_TI92 ? ti89_recv_CNT(handle) : ti92_recv_CNT(handle))
#define RECV_EOT(handle) (handle->model != CALC_TI92 ? ti89_recv_EOT(handle) : ti92_recv_EOT(handle))

// Screen coordinates of the TI-89 / TI-92+
#define TI89_ROWS          128
#define TI89_COLS          240
#define TI89_ROWS_VISIBLE  100
#define TI89_COLS_VISIBLE  160

#define TI92_ROWS  128
#define TI92_COLS  240

static int		is_ready	(CalcHandle* handle)
{
	uint16_t status;

	int ret = SEND_RDY(handle);
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

static int		send_key	(CalcHandle* handle, uint32_t key)
{
	uint16_t status;

	int ret = SEND_KEY(handle, (uint16_t)key);
	if (!ret)
	{
		ret = RECV_ACK(handle, &status);

		PAUSE(50);
	}

	return ret;
}

static int go_to_homescreen(CalcHandle * handle)
{
	int ret;

	PAUSE(200);
	if (handle->model == CALC_TI89 || handle->model == CALC_TI89T)
	{
		ret = send_key(handle, KEY89_HOME);
		if (!ret)
		{
			ret = send_key(handle, KEY89_CLEAR);
			if (!ret)
			{
				ret = send_key(handle, KEY89_CLEAR);
			}
		}
	}
	else if (handle->model == CALC_TI92 || handle->model == CALC_TI92P || handle->model == CALC_V200)
	{
		ret = send_key(handle, KEY92P_CTRL + KEY92P_Q);
		if (!ret)
		{
			ret = send_key(handle, KEY92P_CLEAR);
			if (!ret)
			{
				ret = send_key(handle, KEY92P_CLEAR);
			}
		}
	}
	else
	{
		ret = ERR_VOID_FUNCTION;
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	int ret = 0;

	if (ve->type == TI89_APPL)
	{
		return ERR_VOID_FUNCTION;
	}

	ret = go_to_homescreen(handle);

	if (!ret)
	{
		unsigned int i;
		// Launch program by remote control
		for (i = 0; !ret && i < strlen(ve->folder); i++)
		{
			ret = send_key(handle, (uint32_t)(uint8_t)((ve->folder)[i]));
		}

		if (!ret && strcmp(ve->folder, ""))
		{
			ret = send_key(handle, (uint32_t)'\\');
		}

		for (i = 0; !ret && i < strlen(ve->name); i++)
		{
			ret = send_key(handle, (uint32_t)(uint8_t)((ve->name)[i]));
		}

		if (!ret)
		{
			ret = send_key(handle, KEY89_LP);
			if (!ret)
			{
				if (args)
				{
					for (i = 0; !ret && i < strlen(args); i++)
					{
						ret = send_key(handle, (uint32_t)(uint8_t)(args[i]));
					}
				}
				if (!ret)
				{
					ret = send_key(handle, KEY89_RP);
					if (!ret)
					{
						ret = send_key(handle, KEY89_ENTER);

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
	*bitmap = (uint8_t *)ticalcs_alloc_screen(65537U);
	if (*bitmap == nullptr)
	{
		return ERR_MALLOC;
	}

	sc->width = TI89_COLS;
	sc->height = TI89_ROWS;
	if (handle->model == CALC_TI89 || handle->model == CALC_TI89T)
	{
		sc->clipped_width = TI89_COLS_VISIBLE;
		sc->clipped_height = TI89_ROWS_VISIBLE;
	}
	else
	{
		sc->clipped_width = TI89_COLS;
		sc->clipped_height = TI89_ROWS;
	}
	sc->pixel_format = CALC_PIXFMT_MONO;

	int ret = SEND_SCR(handle);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			uint16_t max_cnt;
			ret = RECV_XDP(handle, &max_cnt, *bitmap);
			if (!ret || ret == ERR_CHECKSUM) // problem with checksum
			{
				// Clip the unused part of the screen (nevertheless usable with asm programs)
				if (   ((handle->model == CALC_TI89) || (handle->model == CALC_TI89T))
				    && (sc->format == SCREEN_CLIPPED))
				{
					int i, j;

					for (i = 0, j = 0; j < TI89_ROWS_VISIBLE; j++)
					{
						for (int k = 0; k < (TI89_COLS_VISIBLE >> 3); k++)
						{
							(*bitmap)[i++] = (*bitmap)[j * (TI89_COLS >> 3) + k];
						}
					}
				}

				*bitmap = (uint8_t *)ticalcs_realloc_screen(*bitmap, TI89_COLS * TI89_ROWS / 8);
				ret = SEND_ACK(handle);
			}
		}
	}

	if (ret)
	{
		ticalcs_free_screen(*bitmap);
		*bitmap = nullptr;
	}

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	VarEntry info;
	uint16_t block_size;
	uint8_t * buffer = (uint8_t *)handle->buffer2;
	int j;
	const uint8_t extra = (handle->model == CALC_V200) ? 8 : 0;
	GNode *root, *node = nullptr;

	int ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}

	root = dirlist_create_append_node(nullptr, apps);
	if (!root)
	{
		return ERR_MALLOC;
	}

	ret = SEND_REQ(handle, ((uint32_t)TI89_FDIR) << 24, TI89_RDIR, "\0\0\0\0\0\0\0");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &info.size, &info.type, info.name);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_CTS(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = RECV_XDP(handle, &block_size, buffer);
							if (!ret)
							{
								ret = SEND_ACK(handle);
								if (!ret)
								{
									ret = RECV_EOT(handle);
									if (!ret)
									{
										ret = SEND_ACK(handle);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (ret)
	{
		return ret;
	}

	// First step: list of folders, and FlashApps on some OS versions.
	for (j = 4; j < (int)block_size;)
	{
		VarEntry *fe = tifiles_ve_create();

		memcpy(fe->name, buffer + j, 8);
		fe->name[8] = '\0';
		fe->type = buffer[j + 8];
		fe->attr = buffer[j + 9];
		fe->size = (  (((uint32_t)buffer[j + 10])      )
		            | (((uint32_t)buffer[j + 11]) <<  8)
		            | (((uint32_t)buffer[j + 12]) << 16)); // | (((uint32_t)buffer[j + 13]) << 24);
		j += 14 + extra;
		fe->folder[0] = 0;

		ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"), 
			fe->name, 
			tifiles_vartype2string(handle->model, fe->type),
			fe->attr,
			fe->size);

		if (fe->type == TI89_DIR)
		{
			node = dirlist_create_append_node(fe, vars);
			if (!node)
			{
				tifiles_ve_delete(fe);
				break;
			}
		}
		else /*if (fe->type == TI89_APPL)*/
		{
			// AMS < 2.08 returns FlashApps; also, get rid of all other types.
			tifiles_ve_delete(fe);
		}
	}

	if (!node)
	{
		return ERR_MALLOC;
	}

	// get list of variables into each folder
	for (int i = 0; i < (int)g_node_n_children(*vars); i++)
	{
		GNode *folder = g_node_nth_child(*vars, i);
		char *folder_name = ((VarEntry *) (folder->data))->name;

		ticalcs_info(_("Directory listing in %8s..."), folder_name);

		ret = SEND_REQ(handle, ((uint32_t)TI89_LDIR) << 24, TI89_RDIR, folder_name);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_VAR(handle, &info.size, &info.type, info.name);
				if (!ret)
				{
					ret = SEND_ACK(handle);
					if (!ret)
					{
						ret = SEND_CTS(handle);
						if (!ret)
						{
							ret = RECV_ACK(handle, NULL);
							if (!ret)
							{
								ret = RECV_XDP(handle, &block_size, buffer);
								if (!ret)
								{
									ret = SEND_ACK(handle);
									if (!ret)
									{
										ret = RECV_EOT(handle);
										if (!ret)
										{
											ret = SEND_ACK(handle);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (!ret)
		{
			for (j = 4 + 14 + extra; j < (int)block_size;)
			{
				VarEntry *ve = tifiles_ve_create();

				memcpy(ve->name, buffer + j, 8);
				ve->name[8] = '\0';
				ve->type = buffer[j + 8];
				ve->attr = buffer[j + 9];
				ve->size = (  (((uint32_t)buffer[j + 10])      )
					    | (((uint32_t)buffer[j + 11]) <<  8)
					    | (((uint32_t)buffer[j + 12]) << 16)); // | (((uint32_t)buffer[j + 13]) << 24);
				j += 14 + extra;
				ticalcs_strlcpy(ve->folder, folder_name, sizeof(ve->folder));

				ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"),
				ve->name,
				tifiles_vartype2string(handle->model, ve->type),
				ve->attr,
				ve->size);

				char* u1 = ticonv_varname_to_utf8(handle->model, ((VarEntry*)(folder->data))->name, -1);
				char* u2 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
				ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s/%s"), u1, u2);
				ticonv_utf8_free(u2);
				ticonv_utf8_free(u1);
				ticalcs_update_label(handle);

				if (ve->type == TI89_APPL)
				{
					VarEntry arg;

					memset(&arg, 0, sizeof(arg));
					ticalcs_strlcpy(arg.name, ve->name, sizeof(arg.name));
					if (!ticalcs_dirlist_ve_exist(*apps, &arg))
					{
						ve->folder[0] = 0;
						node = dirlist_create_append_node(ve, &root);
						if (!node)
						{
							return ERR_MALLOC;
						}
					}
				}
				else
				{
					if (!strcmp(ve->folder, "main") && (!strcmp(ve->name, "regcoef") || !strcmp(ve->name, "regeq")))
					{
						tifiles_ve_delete(ve);
					}
					else
					{
						node = dirlist_create_append_node(ve, &folder);
						if (!node)
						{
							return ERR_MALLOC;
						}
					}
				}
			}

			ticalcs_info(" ");
		}
	}

	return ret;
}

static int		get_dirlist_92	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	VarEntry info;
	GNode *folder = nullptr;
	char folder_name[9];

	int ret = dirlist_init_trees(handle, vars, apps);
	if (!ret)
	{
		ret = SEND_REQ(handle, 0, TI92_RDIR, "\0\0\0\0\0\0\0");
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_VAR(handle, &info.size, &info.type, info.name);
			}
		}
	}
	if (ret)
	{
		return ret;
	}

	for (;;)
	{
		uint8_t * buffer = (uint8_t *)handle->buffer2;
		uint16_t unused;

		folder_name[8] = 0;

		ret = SEND_ACK(handle);
		if (!ret)
		{
			ret = SEND_CTS(handle);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
				if (!ret)
				{
					ret = RECV_XDP(handle, &unused, buffer);
				}
			}
		}
		if (ret)
		{
			break;
		}

		VarEntry* ve = tifiles_ve_create();
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
				ve = nullptr;
			}
			else
			{
				const GNode *node = dirlist_create_append_node(ve, &folder);
				if (node == nullptr)
				{
					return ERR_MALLOC;
				}
			}
		}

		if (ve != nullptr)
		{
			ticalcs_info(_("Name: %8s | Type: %8s | Attr: %i  | Size: %08X"),
				ve->name,
				tifiles_vartype2string(handle->model, ve->type),
				ve->attr,
				ve->size);
		}

		ret = SEND_ACK(handle);
		if (!ret)
		{
			ret = RECV_CNT(handle);
			if (ret)
			{
				if (ret == ERR_EOT)
				{
					ret = SEND_ACK(handle);
				}
				break;
			}
		}

		if (ve != nullptr && folder != nullptr)
		{
			char * utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
			ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s/%s"), ((VarEntry *) (folder->data))->name, utf8);
			ticonv_utf8_free(utf8);
			ticalcs_update_label(handle);
		}
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	(void)handle;
	*ram = *flash = -1;
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	int ret = SEND_VAR(handle, content->data_length, TI92_BKUP, content->rom_version);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			handle->updat->cnt2 = 0;
			const unsigned int nblocks = content->data_length / 1024;
			handle->updat->max2 = nblocks;

			for (unsigned int i = 0; !ret && i <= nblocks; i++) 
			{
				const uint32_t length = (i != nblocks) ? 1024 : content->data_length % 1024;

				ret = SEND_VAR(handle, length, TI92_BKUP, content->rom_version);
				if (!ret)
				{
					ret = RECV_ACK(handle, NULL);
					if (!ret)
					{
						ret = RECV_CTS(handle);
						if (!ret)
						{
							ret = SEND_ACK(handle);
							if (!ret)
							{
								ret = SEND_XDP(handle, length, content->data_part + 1024 * i);
								if (!ret)
								{
									ret = RECV_ACK(handle, NULL);
								}
							}
						}
					}
				}

				handle->updat->cnt2 = i;
				ticalcs_update_pbar(handle);
			}

			if (!ret)
			{
				ret = SEND_EOT(handle);
			}
		}
	}

	return ret;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	uint32_t block_size;
	int ret = 0;
	uint16_t unused;

	ret = SEND_REQ(handle, 0, TI92_BKUP, "main\\backup");
	if (!ret)
	{
		ret = RECV_ACK(handle, &unused);
		if (!ret)
		{
			content->model = CALC_TI92;
			tifiles_comment_set_backup_sn(content->comment, sizeof(content->comment));
			content->data_part = (uint8_t *)tifiles_ve_alloc_data(128 * 1024);
			content->type = TI92_BKUP;
			content->data_length = 0;

			for (int block = 0; !ret; block++) 
			{
				ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Block #%2i"), block);
				ticalcs_update_label(handle);

				ret = RECV_VAR(handle, &block_size, &content->type, content->rom_version);
				const int ret2 = SEND_ACK(handle);

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

				ret = SEND_CTS(handle);
				if (!ret)
				{
					ret = RECV_ACK(handle, NULL);
					if (!ret)
					{
						uint8_t* ptr = content->data_part + content->data_length;
						ret = RECV_XDP(handle, &unused, ptr);
						if (!ret)
						{
							memmove(ptr, ptr + 4, block_size);
							ret = SEND_ACK(handle);
							if (!ret)
							{
								content->data_length += block_size;
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int ret = 0;
	uint16_t status;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = content->num_entries;

	for (unsigned int i = 0; !ret && i < content->num_entries; i++)
	{
		uint8_t * buffer = (uint8_t *)handle->buffer2;
		char varname[18];

		VarEntry* entry = content->entries[i];
		if (!ticalcs_validate_varentry(entry))
		{
			ticalcs_critical("%s: skipping invalid content entry #%u", __FUNCTION__, i);
			continue;
		}

		uint8_t vartype = entry->type;

		if (entry->action == ACT_SKIP)
		{
			continue;
		}

		if ((mode & MODE_LOCAL_PATH) && !(mode & MODE_BACKUP))
		{
			// local & not backup
			ticalcs_strlcpy(varname, entry->name, sizeof(varname));
		} 
		else 
		{
			// full or backup
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}

		ticonv_varname_to_utf8_sn(handle->model, varname, handle->updat->text, sizeof(handle->updat->text), vartype);
		ticalcs_update_label(handle);

		switch (entry->attr) 
		{
		//case ATTRB_NONE:     vartype = TI89_BKUP; break;
		case ATTRB_LOCKED:   vartype = 0x26; break;
		case ATTRB_PROTECTED:
		case ATTRB_ARCHIVED: vartype = 0x27; break;
		}

		uint32_t size = entry->size;
		if (size >= 65536U)
		{
			ticalcs_critical("%s: oversized variable has size %u, clamping to 65535", __FUNCTION__, size);
			size = 65535;
		}

		ret = SEND_RTS(handle, size, vartype, varname);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_CTS(handle);
				if (!ret)
				{
					ret = SEND_ACK(handle);
					if (!ret)
					{
						buffer[0] = 0;
						buffer[1] = 0;
						buffer[2] = 0;
						buffer[3] = 0;
						memcpy(buffer + 4, entry->data, size);
						ret = SEND_XDP(handle, size + 4, buffer);
						if (!ret)
						{
							ret = RECV_ACK(handle, &status);
							if (!ret)
							{
								ret = SEND_EOT(handle);
								if (!ret)
								{
									ret = RECV_ACK(handle, NULL);
									if (!ret)
									{
										ticalcs_info("Sent variable #%u", i);
										handle->updat->cnt2 = i+1;
										handle->updat->max2 = content->num_entries;
										ticalcs_update_pbar(handle);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	int ret = 0;
	uint16_t status;
	uint16_t unused;
	char varname[20];

	content->model = handle->model;
	VarEntry* ve = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));
	ve->data = nullptr;

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	ticonv_varname_to_utf8_sn(handle->model, varname, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	ret = SEND_REQ(handle, 0, vr->type, varname);
	if (!ret)
	{
		ret = RECV_ACK(handle, &status);
		if (!ret)
		{
			if (status != 0)
			{
				return ERR_MISSING_VAR;
			}

			ret = RECV_VAR(handle, &ve->size, &ve->type, ve->name);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_CTS(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size + 4);
							ret = RECV_XDP(handle, &unused, ve->data);
							if (!ret)
							{
								memmove(ve->data, ve->data + 4, ve->size);
								ret = SEND_ACK(handle);
								if (!ret)
								{
									ret = RECV_EOT(handle);
									if (!ret)
									{
										ret = SEND_ACK(handle);
										if (!ret)
										{
											PAUSE(250);
											tifiles_content_add_entry(content, ve);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (ret)
	{
		tifiles_ve_delete(ve);
	}

	return ret;
}

static int		send_all_vars_backup	(CalcHandle* handle, FileContent* content)
{
	// erase memory
	int ret = SEND_VAR(handle, 0, TI89_BKUP, "main");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_CTS(handle);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_EOT(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							// next, send var(s)
							ret = send_var(handle, MODE_BACKUP, content);
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int ret = 0;
	uint16_t status;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = content->num_entries;

	for (unsigned int i = 0; !ret && i < content->num_entries; i++)
	{
		uint8_t * buffer = (uint8_t *)handle->buffer2;
		char varname[18];

		VarEntry* entry = content->entries[i];
		if (!ticalcs_validate_varentry(entry))
		{
			ticalcs_critical("%s: skipping invalid content entry #%u", __FUNCTION__, i);
			continue;
		}

		const uint8_t vartype = entry->type;

		if (entry->action == ACT_SKIP)
		{
			ticalcs_info("%s: skipping variable #%u because requested", __FUNCTION__, i);
			continue;
		}

		if ((mode & MODE_LOCAL_PATH) && ((handle->model == CALC_TI92) || !(mode & MODE_BACKUP)))
		{
			// local & not backup
			ticalcs_strlcpy(varname, entry->name, sizeof(varname));
		} 
		else 
		{
			// full or backup
			tifiles_build_fullname(handle->model, varname, entry->folder, entry->name);
		}

		ticonv_varname_to_utf8_sn(handle->model, varname, handle->updat->text, sizeof(handle->updat->text), vartype);
		ticalcs_update_label(handle);

		uint32_t size = entry->size;
		if (size >= 65536U)
		{
			ticalcs_critical("%s: oversized variable has size %u, clamping to 65535", __FUNCTION__, size);
			size = 65535;
		}

		ret = SEND_VAR(handle, size, vartype, varname);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_CTS(handle);
				if (!ret)
				{
					ret = SEND_ACK(handle);
					if (!ret)
					{
						buffer[0] = 0;
						buffer[1] = 0;
						buffer[2] = 0;
						buffer[3] = 0;
						memcpy(buffer + 4, entry->data, size);
						ret = SEND_XDP(handle, size + 4, buffer);
						if (!ret)
						{
							ret = RECV_ACK(handle, &status);
							if (!ret)
							{
								ret = SEND_EOT(handle);
								if (!ret)
								{
									ret = RECV_ACK(handle, NULL);
									if (!ret)
									{
										ticalcs_info("Sent variable #%u", i);

										if (mode & MODE_BACKUP || handle->model == CALC_TI92)
										{
											handle->updat->cnt2 = i+1;
											handle->updat->max2 = content->num_entries;
											ticalcs_update_pbar(handle);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
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

		ticalcs_strlcpy(ve->folder, "main", sizeof(ve->folder));

		ret = RECV_VAR(handle, &ve->size, &ve->type, tipath);
		const int ret2 = SEND_ACK(handle);

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
		if ((tiname = strchr(tipath, '\\')) != nullptr)
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

		ticonv_varname_to_utf8_sn(handle->model, ve->name, handle->updat->text, sizeof(handle->updat->text), ve->type);
		ticalcs_update_label(handle);

		ret = SEND_CTS(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size + 4);
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
		*vr = nullptr;
	}
	else
	{
		*vr = tifiles_ve_dup(content->entries[0]);
	}

	return ret;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	int ret = 0;

	// send all headers except license
	for (FlashContent* ptr = content; !ret && ptr != nullptr; ptr = ptr->next)
	{
		if (ptr->data_type == TI89_LICENSE)
		{
			continue;
		}

		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		ticonv_varname_to_utf8_sn(handle->model, ptr->name, handle->updat->text, sizeof(handle->updat->text), ptr->data_type);
		ticalcs_update_label(handle);

		if (ptr->data_type == TI89_AMS)
		{
			if (handle->model == CALC_TI89T || handle->model == CALC_V200)
			{
				ret = SEND_RTS2(handle, ptr->data_length, ptr->data_type, ptr->hw_id);
			}
			else
			{
				ret = SEND_RTS(handle, ptr->data_length, ptr->data_type, "\0\0\0\0\0\0\0");
			}
		} 
		else 
		{
			ret = SEND_RTS(handle, ptr->data_length, ptr->data_type, ptr->name);
		}

		if (!ret)
		{
			const int nblocks = ptr->data_length / 65536;
			handle->updat->max2 = nblocks+1;

			for (int i = 0; !ret && i <= nblocks; i++)
			{
				const uint32_t length = (i != nblocks) ? 65536 : ptr->data_length % 65536;

				ret = RECV_ACK(handle, NULL);
				if (!ret)
				{
					ret = RECV_CTS(handle);
					if (!ret)
					{
						ret = SEND_ACK(handle);
						if (!ret)
						{
							ret = SEND_XDP(handle, length, (ptr->data_part) + 65536 * i);
							if (!ret)
							{
								ret = RECV_ACK(handle, NULL);
								if (!ret)
								{
									if (i != nblocks)
									{
										ret = SEND_CNT(handle);
									} 
									else 
									{
										ret = SEND_EOT(handle);
									}

									handle->updat->cnt2 = i;
									ticalcs_update_pbar(handle);
								}
							}
						}
					}
				}
			}

			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
				ticalcs_info("%s", _("Header sent completely."));
			}
		}
	}

	return ret;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	int i;
	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	content->model = handle->model;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(4 * 1024 * 1024);	// 4MB max
	content->data_type = vr->type;
	switch(handle->model)
	{
	case CALC_TI89:
	case CALC_TI89T: content->device_type = DEVICE_TYPE_89; break;
	case CALC_TI92P:
	case CALC_V200:  content->device_type = DEVICE_TYPE_92P; break;
	default: return ERR_FATAL_ERROR;
	}

	int ret = SEND_REQ(handle, 0x00, vr->type, vr->name);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &content->data_length, &content->data_type, content->name);
			if (!ret)
			{

				handle->updat->cnt2 = 0;
				handle->updat->max2 = vr->size;

				for (i = 0, content->data_length = 0; !ret; i++) 
				{
					uint16_t block_size;

					ret = SEND_ACK(handle);
					if (!ret)
					{
						ret = SEND_CTS(handle);
						if (!ret)
						{
							ret = RECV_ACK(handle, NULL);
							if (!ret)
							{
								ret = RECV_XDP(handle, &block_size, content->data_part + content->data_length);
								if (!ret)
								{
									ret = SEND_ACK(handle);
									if (!ret)
									{
										content->data_length += block_size;

										ret  = RECV_CNT(handle);
										if (ret)
										{
											if (ret  == ERR_EOT)
											{
												ret = 0;
											}
											break;
										}

										handle->updat->cnt2 += block_size;
										ticalcs_update_pbar(handle);
									}
								}
							}
						}
					}
				}

				if (!ret)
				{
					ret = SEND_ACK(handle);
				}
			}
		}
	}

	return ret;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	uint32_t varsize;
	uint16_t pktsize;
	uint8_t vartype;
	char varname[9];

	ticalcs_strlcpy(handle->updat->text, "ID-LIST", sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	int ret = SEND_REQ(handle, 0x0000, TI89_IDLIST, "\0\0\0\0\0\0\0");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &varsize, &vartype, varname);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_CTS(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = RECV_XDP(handle, &pktsize, idlist);
							if (!ret)
							{
								memmove(idlist, idlist + 8, pktsize - 8);
								idlist[pktsize - 8] = '\0';
								ret = SEND_ACK(handle);
								if (!ret)
								{
									ret = RECV_EOT(handle);
									if (!ret)
									{
										ret = SEND_ACK(handle);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	int ret = 0;

	ret = go_to_homescreen(handle);
	PAUSE(200);

	if (!ret)
	{
		// Send dumping program
		if (handle->model != CALC_TI92)
		{
			ret = rd_send_dumper(handle, "romdump.89z", romDumpSize89, romDump89);
		}
		else
		{
			ret = rd_send_dumper(handle, "romdump.92p", romDumpSize92, romDump92);
		}
		PAUSE(1000);
	}

	return ret;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret = 0;
	static const uint16_t keys[] = {
		'm', 'a', 'i', 'n', '\\',
		'r', 'o', 'm', 'd', 'u', 'm', 'p',
		KEY89_LP, KEY89_RP, KEY89_ENTER
	};

	// Launch program by remote control
	for (unsigned int i = 0; !ret && i < sizeof(keys) / sizeof(keys[0]); i++)
	{
		ret = send_key(handle, (uint32_t)(keys[i]));
	}
	PAUSE(200);

	// Get dump
	if (!ret)
	{
		ret = rd_read_dump(handle, filename);
	}

	return ret;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint8_t buffer[16];
	uint16_t status;

	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = 0;
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

	ticalcs_strlcpy(handle->updat->text, _("Setting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	int ret = SEND_RTS(handle, 0x10, TI89_CLK, "Clock");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_CTS(handle);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_XDP(handle, 0x10, buffer);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = SEND_EOT(handle);
							if (!ret)
							{
								ret = RECV_ACK(handle, &status);
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	uint32_t varsize;
	uint16_t pktsize;
	uint8_t vartype;
	char varname[9];

	ticalcs_strlcpy(handle->updat->text, _("Getting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	int ret = SEND_REQ(handle, 0x0000, TI89_CLK, "Clock");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &varsize, &vartype, varname);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_CTS(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							uint8_t * buffer = (uint8_t *)(handle->buffer2);
							ret = RECV_XDP(handle, &pktsize, buffer);
							if (!ret)
							{
								ret = SEND_ACK(handle);
								if (!ret)
								{
									ret = RECV_EOT(handle);
									if (!ret)
									{
										ret = SEND_ACK(handle);
										if (!ret)
										{
											_clock->year = (((uint16_t)buffer[6]) << 8) | buffer[7];
											_clock->month = buffer[8];
											_clock->day = buffer[9];
											_clock->hours = buffer[10];
											_clock->minutes = buffer[11];
											_clock->seconds = buffer[12];
											_clock->date_format = buffer[13];
											_clock->time_format = buffer[14];
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char varname[18];

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	char* utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	int ret = SEND_DEL(handle, vr->size, vr->type, varname);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
		}
	}

	return ret;
}

static int		del_var_92		(CalcHandle* handle, VarRequest* vr)
{
	int ret = 0;
	unsigned int i;
	static const uint16_t keys[] = {
		KEY92P_ON, KEY92P_ESC, KEY92P_ESC, KEY92P_ESC,
		KEY92P_2ND + KEY92P_ESC, KEY92P_2ND + KEY92P_ESC, KEY92P_CTRL + KEY92P_Q, KEY92P_CLEAR, KEY92P_CLEAR,
		'd', 'e', 'l', 'v', 'a', 'r',
		KEY92P_SPACE
	};
	char varname[18];

	tifiles_build_fullname(handle->model, varname, vr->folder, vr->name);
	char* utf8 = ticonv_varname_to_utf8(handle->model, varname, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	for (i = 0; !ret && i < sizeof(keys) / sizeof(keys[0]); i++)
	{
		ret = send_key(handle, (uint32_t)(keys[i]));
	}
	for (i = 0; !ret && i < strlen(varname); i++)
	{
		ret = send_key(handle, (uint32_t)(uint8_t)(varname[i]));
	}

	if (!ret)
	{
		ret = send_key(handle, KEY92P_ENTER);
	}

	return ret;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	uint8_t data[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23 };
	char varname[18];

	tifiles_build_fullname(handle->model, varname, vr->folder, "a1234567");
	char* utf8 = ticonv_varname_to_utf8(handle->model, vr->folder, -1);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Creating %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	// send empty expression
	int ret = SEND_RTS(handle, 0x10, 0x00, varname);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_CTS(handle);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_XDP(handle, 0x10, data);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = SEND_EOT(handle);
							if (!ret)
							{
								ret = RECV_ACK(handle, NULL);
								if (!ret)
								{
									PAUSE(250);
									// delete 'a1234567' variable
									ticalcs_strlcpy(vr->name, "a1234567", sizeof(vr->name));
									ret = ((handle->model != CALC_TI92) ? del_var : del_var_92)(handle, vr);
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint16_t length;
	uint8_t * buffer = (uint8_t *)handle->buffer2;

	int ret = SEND_VER(handle);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = SEND_CTS(handle);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
				if (!ret)
				{
					ret = RECV_XDP(handle, &length, buffer);
					if (!ret)
					{
						ret = SEND_ACK(handle);
					}
				}
			}
		}
	}
	if (!ret)
	{
		memset(infos, 0, sizeof(CalcInfos));
		if (length >= 14)
		{
			ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1d.%02d", buffer[0], buffer[1]);
			ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1d.%02d", buffer[2], buffer[3]);
			infos->battery = buffer[4] >= 1 ? 0 : 1;
			switch(buffer[13])
			{
			case 1: infos->hw_version = buffer[5] + 1; infos->model = CALC_TI92P; break;
			case 3: infos->hw_version = buffer[5] + 1; infos->model = CALC_TI89; break;
			case 5: infos->hw_version = buffer[5]; infos->model = CALC_CBL2; break; // Tentative
			case 6: infos->hw_version = buffer[5]; infos->model = CALC_LABPRO; break; // Tentative
			case 7: infos->hw_version = buffer[5]; infos->model = CALC_TIPRESENTER; break; // Tentative
			case 8: infos->hw_version = buffer[5]; infos->model = CALC_V200; break;
			case 9: infos->hw_version = buffer[5] + 1; infos->model = CALC_TI89T; break;
			}
			infos->language_id = buffer[6];
			infos->sub_lang_id = buffer[7];
			infos->mask = (InfosMask)(INFOS_BOOT_VERSION | INFOS_OS_VERSION | INFOS_BATTERY_ENOUGH | INFOS_HW_VERSION | INFOS_CALC_MODEL | INFOS_LANG_ID | INFOS_SUB_LANG_ID);

			tifiles_hexdump(buffer, length);
			ticalcs_info(_("  OS: %s"), infos->os_version);
			ticalcs_info(_("  BIOS: %s"), infos->boot_version);
			ticalcs_info(_("  Battery: %s"), infos->battery ? "good" : "low");
		}
		else
		{
			ticalcs_warning("%s", _("Bad data length for version information"));
		}
	}

	return ret;
}

static int		get_version_92	(CalcHandle* handle, CalcInfos* infos)
{
	uint32_t size;
	uint8_t type;
	char name[32];

	int ret = SEND_REQ(handle, 0, TI92_BKUP, "main\\version");
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &size, &type, name);
			if (!ret)
			{
				ret = SEND_EOT(handle);
				if (!ret)
				{
					memset(infos, 0, sizeof(CalcInfos));
					strncpy(infos->os_version, name, 4);
					infos->os_version[4] = 0;
					infos->hw_version = 1;
					infos->mask = (InfosMask)(INFOS_OS_VERSION | INFOS_HW_VERSION);

					ticalcs_info(_("  OS: %s"), infos->os_version);
					ticalcs_info(_("  Battery: %s"), infos->battery ? "good" : "low");
				}
			}
		}
	}

	return ret;
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
	ve.name[0] = 0;

	ret = recv_flash(handle, content, &ve);
	if (!ret)
	{
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
		content->name[0] = 0;
	}

	return ret;
}

extern const CalcFncts calc_89 =
{
	CALC_TI89,
	"TI89",
	"TI-89",
	"TI-89",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS | OPS_LABEQUIPMENTDATA |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT | FTS_NONSILENT,
	PRODUCT_ID_TI89,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "2P1L", /* send_all_vars_backup */
	 "2P1L"  /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
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
	&noop_rename_var,
	&noop_change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_92p =
{
	CALC_TI92P,
	"TI92+",
	"TI-92 Plus",
	"TI-92 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS | OPS_LABEQUIPMENTDATA |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT | FTS_NONSILENT,
	PRODUCT_ID_TI92P,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "2P1L", /* send_all_vars_backup */
	 "2P1L"  /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
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
	&noop_rename_var,
	&noop_change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_89t =
{
	CALC_TI89T,
	"Titanium",
	"TI-89 Titanium",
	"TI-89 Titanium",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP | 
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS | OPS_LABEQUIPMENTDATA |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT | FTS_NONSILENT,
	PRODUCT_ID_TI89T,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "2P1L", /* send_all_vars_backup */
	 "2P1L"  /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
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
	&noop_rename_var,
	&noop_change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_v2 =
{
	CALC_V200,
	"V200",
	"V200PLT",
	N_("V200 Personal Learning Tool"),
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | 
	OPS_FLASH | OPS_IDLIST | OPS_CLOCK | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_OS | OPS_LABEQUIPMENTDATA |
	FTS_SILENT | FTS_FOLDER | FTS_FLASH | FTS_CERT | FTS_NONSILENT,
	PRODUCT_ID_TIV200,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "",     /* send_backup */
	 "",     /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "2P1L", /* send_app */
	 "2P1L", /* recv_app */
	 "2P",   /* send_os */
	 "1L",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
	 "1L",   /* new_folder */
	 "",     /* get_version */
	 "1L",   /* send_cert */
	 "1L",   /* recv_cert */
	 "",     /* rename */
	 "",     /* chattr */
	 "2P1L", /* send_all_vars_backup */
	 "2P1L"  /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
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
	&noop_rename_var,
	&noop_change_attr,
	&send_all_vars_backup,
	&tixx_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_92 =
{
	CALC_TI92,
	"TI92",
	"TI-92",
	"TI-92",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR | OPS_NEWFLD | OPS_VERSION | OPS_LABEQUIPMENTDATA |
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
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist_92,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_ns,
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
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_cbl2 =
{
	CALC_CBL2,
	"CBL2",
	"CBL2",
	"CBL2",
	OPS_ISREADY /*| OPS_DIRLIST | OPS_VARS*/ |
	OPS_VERSION /*| OPS_OS*/ | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
	PRODUCT_ID_CBL2,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
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
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_labpro =
{
	CALC_LABPRO,
	"LABPRO",
	"LabPro",
	"LabPro",
	OPS_ISREADY /*| OPS_DIRLIST | OPS_VARS*/ |
	OPS_VERSION /*| OPS_OS*/ | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
	PRODUCT_ID_LABPRO,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
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
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&tixx_send_lab_equipment_data,
	&tixx_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_tipresenter =
{
	CALC_TIPRESENTER,
	"TIPRESENTER",
	"TI-Presenter",
	"TI-Presenter",
	OPS_ISREADY | OPS_VERSION /*| OPS_OS*/ |
	FTS_SILENT,
	PRODUCT_ID_TIPRESENTER,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&noop_get_dirlist,
	&noop_get_memfree,
	&noop_send_backup,
	&noop_recv_backup,
	&noop_send_var_ns,
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
	&get_version,
	&noop_send_cert,
	&noop_recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};
