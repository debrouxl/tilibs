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
	TI82/83/85/86 support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cctype>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "dbus_pkt.h"
#include "cmdz80.h"
#include "rom82.h"
#include "rom83.h"
#include "rom85.h"
#include "rom86.h"
#include "romdump.h"
#include "keys83.h"
#include "keys86.h"

#define SEND_RDY ti73_send_RDY
#define SEND_KEY(handle, scancode) ((handle->model < CALC_TI85) ? ti82_send_KEY(handle, scancode) : ti85_send_KEY(handle, scancode))
#define SEND_SCR(handle) ((handle->model < CALC_TI85) ? ti82_send_SCR(handle) : ti85_send_SCR(handle))
#define SEND_ACK(handle) ((handle->model < CALC_TI85) ? ti82_send_ACK(handle) : ti85_send_ACK(handle))
#define SEND_VAR(handle, varsize, vartype, varname) ((handle->model < CALC_TI85) ? ti82_send_VAR(handle, varsize, vartype, varname) : ti85_send_VAR(handle, varsize, vartype, varname))
#define SEND_XDP(handle, length, data) ((handle->model < CALC_TI85) ? ti82_send_XDP(handle, length, data) : ti85_send_XDP(handle, length, data))
#define SEND_REQ(handle, varsize, vartype, varname) ((handle->model < CALC_TI85) ? ti82_send_REQ(handle, varsize, vartype, varname) : ti85_send_REQ(handle, varsize, vartype, varname))
#define SEND_RTS(handle, varsize, vartype, varname) ((handle->model < CALC_TI85) ? ti82_send_RTS(handle, varsize, vartype, varname) : ti85_send_RTS(handle, varsize, vartype, varname))
#define SEND_CTS(handle) ((handle->model < CALC_TI85) ? ti82_send_CTS(handle) : ti85_send_CTS(handle))
#define SEND_EOT(handle) ((handle->model < CALC_TI85) ? ti82_send_EOT(handle) : ti85_send_EOT(handle))

#define RECV_ACK(handle, status) ((handle->model < CALC_TI85) ? ti82_recv_ACK(handle, status) : ti85_recv_ACK(handle, status))
#define RECV_VAR(handle, varsize, vartype, varname) ((handle->model < CALC_TI85) ? ti82_recv_VAR(handle, varsize, vartype, varname) : ti85_recv_VAR(handle, varsize, vartype, varname))
#define RECV_XDP(handle, length, data) ((handle->model < CALC_TI85) ? ti82_recv_XDP(handle, length, data) : ti85_recv_XDP(handle, length, data))
#define RECV_SKP(handle, rej_code) ((handle->model < CALC_TI85) ? ti82_recv_SKP(handle, rej_code) : ti85_recv_SKP(handle, rej_code))
#define RECV_ERR ti82_recv_ERR

// Screen coordinates of the TI-80
#define TI80_ROWS  48
#define TI80_COLS  64

// Screen coordinates of the TI-82
#define TI82_ROWS  64
#define TI82_COLS  96

// Screen coordinates of the TI-83
#define TI83_ROWS  64
#define TI83_COLS  96

// Screen coordinates of the TI-85
#define TI85_ROWS  64
#define TI85_COLS  128

// Screen coordinates of the TI-86
#define TI86_ROWS  64
#define TI86_COLS  128

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

static int		send_key	(CalcHandle* handle, uint32_t key)
{
	int ret;
	uint16_t status;

	ret = SEND_KEY(handle, (uint16_t)key);
	if (!ret)
	{
		ret = RECV_ACK(handle, &status);
		if (handle->model != CALC_TI83 && !ret)
		{
			ret = RECV_ACK(handle, &status);
		}
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	int ret;

	// Go back to homescreen
	PAUSE(200);
	ret = send_key(handle, KEY83_Quit);
	if (!ret)
	{
		ret = send_key(handle, KEY83_Clear);
		if (!ret)
		{
			ret = send_key(handle, KEY83_Clear);
		}
	}

	if (!ret)
	{
		// Launch program by remote control
		if (ve->type == TI83_ASM)
		{
			ret = send_key(handle, KEY83_SendMBL);
			if (!ret)
			{
				ret = send_key(handle, KEY83_9);
			}
		}
		if (!ret)
		{
			ret = send_key(handle, KEY83_Exec);
			if (!ret)
			{
				unsigned int i;
				for (i = 0; !ret && i < strlen(ve->name); i++)
				{
					const CalcKey *ck = ticalcs_keys_83((ve->name)[i]);
					ret = send_key(handle, (uint32_t)(ck->normal.value));
				}

				if (!ret)
				{
					ret = send_key(handle, KEY83_Enter);

					PAUSE(200);
				}
			}
		}
	}

	return ret;
}

static int		recv_screen_80	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;
	uint8_t * buffer;

	*bitmap = (uint8_t *)ticalcs_alloc_screen(TI80_COLS * TI80_ROWS / 8);
	if (*bitmap == NULL)
	{
		return ERR_MALLOC;
	}

	sc->width = TI80_COLS;
	sc->height = TI80_ROWS;
	sc->clipped_width = TI80_COLS;
	sc->clipped_height = TI80_ROWS;
	sc->pixel_format = CALC_PIXFMT_MONO;

	ret = ti80_send_SCR(handle);
	if (!ret)
	{
		ret = ti80_recv_ACK(handle, NULL);
		if (!ret)
		{
			uint16_t max_cnt;
			ret = ti80_recv_XDP(handle, &max_cnt, (uint8_t *)handle->buffer);
			if (!ret)
			{
				int stripe, row, i = 0;
				buffer = (uint8_t *)(handle->buffer);
				for (stripe = 7; stripe >= 0; stripe--)
				{
					for (row = 0; row < TI80_ROWS; row++)
					{
						(*bitmap)[row * TI80_COLS / 8 + stripe] = buffer[i++];
					}
				}
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

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;

	*bitmap = (uint8_t *)ticalcs_alloc_screen(65537U);
	if (*bitmap == NULL)
	{
		return ERR_MALLOC;
	}

	if (handle->model == CALC_TI82)
	{
		sc->width = TI82_COLS;
		sc->height = TI82_ROWS;
		sc->clipped_width = TI82_COLS;
		sc->clipped_height = TI82_ROWS;
	}
	else if (handle->model == CALC_TI83)
	{
		sc->width = TI83_COLS;
		sc->height = TI83_ROWS;
		sc->clipped_width = TI83_COLS;
		sc->clipped_height = TI83_ROWS;
	}
	else if (handle->model == CALC_TI85)
	{
		sc->width = TI85_COLS;
		sc->height = TI85_ROWS;
		sc->clipped_width = TI85_COLS;
		sc->clipped_height = TI85_ROWS;
	}
	else
	{
		sc->width = TI86_COLS;
		sc->height = TI86_ROWS;
		sc->clipped_width = TI86_COLS;
		sc->clipped_height = TI86_ROWS;
	}
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
				*bitmap = (uint8_t *)ticalcs_realloc_screen(*bitmap, (handle->model < CALC_TI85) ? TI82_COLS * TI82_ROWS / 8 : TI85_COLS * TI85_ROWS / 8);
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

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	int ret;
	uint16_t unused;
	TreeInfo *ti;
	GNode *folder, *node;
	char *utf8;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}
	ti = (TreeInfo *)((*vars)->data);

	ret = SEND_REQ(handle, 0x0000, (handle->model == CALC_TI83) ? TI83_DIR : TI86_DIR, "\0\0\0\0\0\0\0");
	if (!ret)
	{
		ret = RECV_ACK(handle, &unused);
		if (!ret)
		{
			ret = RECV_XDP(handle, &unused, (uint8_t *)handle->buffer2);
			if (!ret)
			{
				ret = SEND_ACK(handle);
			}
		}
	}

	if (!ret)
	{
		VarEntry *ve;
		uint8_t * mem = (uint8_t *)handle->buffer2;

		folder = dirlist_create_append_node(NULL, vars);

		if (handle->model == CALC_TI83)
		{
			ti->mem_free = (((uint32_t)(mem[1])) << 8) | mem[0]; // Clamp mem_free to a 16-bit value.

			// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
			ve = tifiles_ve_create();
			ve->type = TI83_WINDW;
			node = dirlist_create_append_node(ve, &folder);
			if (node != NULL)
			{
				ve = tifiles_ve_create();
				ve->type = TI83_ZSTO;
				node = dirlist_create_append_node(ve, &folder);
				if (node != NULL)
				{
					ve = tifiles_ve_create();
					ve->type = TI83_TABLE;
					node = dirlist_create_append_node(ve, &folder);
				}
			}
		}
		else
		{
			ti->mem_free = (((uint32_t)(mem[0])) << 16) | (((uint32_t)(mem[1])) << 8) | mem[2];

			// Add permanent variables (Func, Pol, Param, DifEq, ZRCL as WIND, WIND, WIND, WIND, WIND)
			ve = tifiles_ve_create();
			ve->type = TI86_FUNC;
			node = dirlist_create_append_node(ve, &folder);
			if (node != NULL)
			{
				ve = tifiles_ve_create();
				ve->type = TI86_POL;
				node = dirlist_create_append_node(ve, &folder);
				if (node != NULL)
				{
					ve = tifiles_ve_create();
					ve->type = TI86_PARAM;
					node = dirlist_create_append_node(ve, &folder);
					if (node != NULL)
					{
						ve = tifiles_ve_create();
						ve->type = TI86_DIFEQ;
						node = dirlist_create_append_node(ve, &folder);
						if (node != NULL)
						{
							ve = tifiles_ve_create();
							ve->type = TI86_ZRCL;
							node = dirlist_create_append_node(ve, &folder);
						}
					}
				}
			}
		}

		if (!node)
		{
			ret = ERR_MALLOC;
		}
		else
		{
			for (;;)
			{
				uint16_t ve_size;
				int ret2;

				ve = tifiles_ve_create();
				ret = RECV_VAR(handle, &ve_size, &ve->type, ve->name);
				ve->size = ve_size;
				ret2 = SEND_ACK(handle);
				if (ret)
				{
					if (ret == ERR_EOT)	// end of transmission
					{
						ret = 0;
					}
error:
					tifiles_ve_delete(ve);
					break;
				}
				if (ret2)
				{
					ret = ret2;
					goto error;
				}

				node = dirlist_create_append_node(ve, &folder);
				if (!node)
				{
					return ERR_MALLOC;
				}

				utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
				ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s"), utf8);
				ticonv_utf8_free(utf8);
				ticalcs_update_label(handle);
			}
		}
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	int ret;
	uint16_t unused;

	*ram = -1;
	*flash = -1;

	ret = SEND_REQ(handle, 0x0000, (handle->model == CALC_TI83) ? TI83_DIR : TI86_DIR, "\0\0\0\0\0\0\0");
	if (!ret)
	{
		ret = RECV_ACK(handle, &unused);
		if (!ret)
		{
			ret = RECV_XDP(handle, &unused, (uint8_t *)handle->buffer2);
			if (!ret)
			{
				ret = SEND_EOT(handle);
				if (!ret)
				{
					uint8_t * mem = (uint8_t *)handle->buffer2;
					if (handle->model == CALC_TI83)
					{
						*ram = (((uint32_t)(mem[1])) << 8) | mem[0]; // Clamp mem_free to a 16-bit value.
					}
					else // if (handle->model == CALC_TI86)
					{
						*ram = (((uint32_t)(mem[0])) << 16) | (((uint32_t)(mem[1])) << 8) | mem[2];
					}
				}
			}
		}
	}

	return ret;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
	int ret;
	uint16_t length;
	char varname[9];
	uint8_t rej_code;
	uint16_t status;

	length = content->data_length1;
	varname[0] = LSB(content->data_length2);
	varname[1] = MSB(content->data_length2);
	varname[2] = LSB(content->data_length3);
	varname[3] = MSB(content->data_length3);
	varname[4] = LSB((handle->model != CALC_TI86) ? content->mem_address : content->data_length4);
	varname[5] = MSB((handle->model != CALC_TI86) ? content->mem_address : content->data_length4);
	varname[6] = 0;
	varname[7] = 0;
	varname[8] = 0;

	do
	{
		if (handle->model == CALC_TI83)
		{
			ret = SEND_RTS(handle, content->data_length1, TI83_BKUP, varname);
		}
		else
		{
			ret = SEND_VAR(handle, content->data_length1, (handle->model == CALC_TI82) ? TI82_BKUP : ((handle->model == CALC_TI85) ? TI85_BKUP : TI86_BKUP), varname);
		}
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
		}
		if (ret)
		{
			break;
		}

		if (handle->model == CALC_TI83)
		{
			ret = RECV_SKP(handle, &rej_code);
		}
		else
		{
			ticalcs_strlcpy(handle->updat->text, _("Waiting for user's action..."), sizeof(handle->updat->text));
			ticalcs_update_label(handle);

			do
			{
				// wait for user's action
				ticalcs_update_refresh(handle);
				if (ticalcs_update_canceled(handle))
				{
					ret = ERR_ABORT;
					break;
				}

				ret = RECV_SKP(handle, &rej_code);
			}
			while (ret == ERROR_READ_TIMEOUT);
		}

		if (!ret)
		{
			ret = SEND_ACK(handle);
		}
		if (ret)
		{
			break;
		}

		switch (rej_code)
		{
		case DBUS_REJ_EXIT:
		case DBUS_REJ_SKIP:
			ret = ERR_ABORT;
			break;
		case DBUS_REJ_MEMORY:
			ret = ERR_OUT_OF_MEMORY;
			// Fall through.
		case 0:                         // CTS
			break;
		default:
			ret = ERR_VAR_REJECTED;
			break;
		}

		if (ret)
		{
			break;
		}

		handle->updat->text[0] = 0;
		ticalcs_update_label(handle);

		handle->updat->cnt2 = 0;
		handle->updat->max2 = (handle->model != CALC_TI86) ? 3 : 4;
		ticalcs_update_pbar(handle);

		ret = SEND_XDP(handle, content->data_length1, content->data_part1);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
		}
		if (ret)
		{
			break;
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		ret = SEND_XDP(handle, content->data_length2, content->data_part2);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
		}
		if (ret)
		{
			break;
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		if (content->data_length3)
		{
			ret = SEND_XDP(handle, content->data_length3, content->data_part3);
			if (!ret)
			{
				ret = RECV_ACK(handle, &status);
			}
			if (ret)
			{
				break;
			}
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		if (handle->model == CALC_TI86)
		{
			ret = SEND_XDP(handle, content->data_length4, content->data_part4);
			if (!ret)
			{
				ret = RECV_ACK(handle, &status);
			}
			if (ret)
			{
				break;
			}
			handle->updat->cnt2++;
			ticalcs_update_pbar(handle);
		}

		if (handle->model == CALC_TI83)
		{
			ret = SEND_ACK(handle);
		}
		else if (handle->model == CALC_TI85)
		{
			ret = SEND_EOT(handle);
		}
	} while(0);

	return ret;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	int ret;
	char varname[9];

	content->model = handle->model;
	tifiles_comment_set_backup_sn(content->comment, sizeof(content->comment));

	if (handle->model == CALC_TI83)
	{
		ret = SEND_REQ(handle, 0x0000, TI83_BKUP, "\0\0\0\0\0\0\0");
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
		}
		if (ret)
		{
			return ret;
		}
	}
	else
	{
		ticalcs_strlcpy(handle->updat->text, _("Waiting for backup..."), sizeof(handle->updat->text));
		ticalcs_update_label(handle);
	}

	varname[0] = 0;
	do
	{
		ret = RECV_VAR(handle, &(content->data_length1), &content->type, varname);
		if (ret)
		{
			break;
		}
		content->data_length2 = (uint8_t)varname[0] | (((uint16_t)(uint8_t)varname[1]) << 8);
		content->data_length3 = (uint8_t)varname[2] | (((uint16_t)(uint8_t)varname[3]) << 8);
		if (handle->model != CALC_TI86)
		{
			content->mem_address  = (uint8_t)varname[4] | (((uint16_t)(uint8_t)varname[5]) << 8);
		}
		else
		{
			content->data_length4 = (uint8_t)varname[4] | (((uint16_t)(uint8_t)varname[5]) << 8);
		}
		ret = SEND_ACK(handle);
		if (!ret)
		{
			ret = SEND_CTS(handle);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
			}
		}
		if (ret)
		{
			break;
		}

		handle->updat->text[0] = 0;
		ticalcs_update_label(handle);

		handle->updat->cnt2 = 0;
		handle->updat->max2 = (handle->model != CALC_TI86) ? 3 : 4;
		ticalcs_update_pbar(handle);

		content->data_part1 = (uint8_t *)tifiles_ve_alloc_data(65536U);
		ret = RECV_XDP(handle, &content->data_length1, content->data_part1);
		if (!ret)
		{
			ret = SEND_ACK(handle);
		}
		if (ret)
		{
			break;
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		content->data_part2 = (uint8_t *)tifiles_ve_alloc_data(65536U);
		ret = RECV_XDP(handle, &content->data_length2, content->data_part2);
		if (!ret)
		{
			ret = SEND_ACK(handle);
		}
		if (ret)
		{
			break;
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		if (content->data_length3)
		{
			content->data_part3 = (uint8_t *)tifiles_ve_alloc_data(65536U);
			ret = RECV_XDP(handle, &content->data_length3, content->data_part3);
			if (!ret)
			{
				ret = SEND_ACK(handle);
			}
			if (ret)
			{
				break;
			}
		}
		else
		{
			content->data_part3 = NULL;
		}
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		if (handle->model != CALC_TI86)
		{
			content->data_part4 = NULL;
		}
		else
		{
			content->data_part4 = (uint8_t *)tifiles_ve_alloc_data(65536U);
			ret = RECV_XDP(handle, &content->data_length4, content->data_part4);
			if (!ret)
			{
				ret = SEND_ACK(handle);
			}
			if (ret)
			{
				break;
			}
			handle->updat->cnt2++;
			ticalcs_update_pbar(handle);
		}
	} while(0);

	return ret;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	unsigned int i;
	int ret = 0;
	uint8_t rej_code;
	uint16_t status;

	if ((mode & MODE_SEND_EXEC_ASM) && content->num_entries != 1)
	{
		ticalcs_critical("no variable to execute");
		return -1;
	}

	handle->updat->cnt2 = 0;
	handle->updat->max2 = content->num_entries;

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		uint16_t size;

		if (!ticalcs_validate_varentry(entry))
		{
			ticalcs_critical("%s: skipping invalid content entry #%u", __FUNCTION__, i);
			continue;
		}

		if (entry->action == ACT_SKIP)
		{
			ticalcs_info("%s: skipping variable #%u because requested", __FUNCTION__, i);
			continue;
		}

		if (entry->size >= 65536U)
		{
			ticalcs_critical("%s: oversized variable has size %u, clamping to 65535", __FUNCTION__, entry->size);
			size = 65535;
		}
		else
		{
			size = (uint16_t)entry->size;
		}

		ret = SEND_VAR(handle, size, entry->type, entry->name);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
		}
		if (ret)
		{
			break;
		}

		ticalcs_strlcpy(handle->updat->text, _("Waiting for user's action..."), sizeof(handle->updat->text));
		ticalcs_update_label(handle);

		do
		{
			// wait for user's action
			ticalcs_update_refresh(handle);
			if (ticalcs_update_canceled(handle))
			{
				ret = ERR_ABORT;
				break;
			}

			ret = RECV_SKP(handle, &rej_code);
		}
		while (ret == ERROR_READ_TIMEOUT);

		if (!ret)
		{
			ret = SEND_ACK(handle);
		}
		if (ret)
		{
			break;
		}

		switch (rej_code)
		{
		case DBUS_REJ_EXIT:
			ret = ERR_ABORT;
			break;
		case DBUS_REJ_SKIP:
			if (mode & MODE_SEND_EXEC_ASM)
			{
				ret = ERR_ABORT;
				break;
			}
			continue;
		case DBUS_REJ_MEMORY:
			ret = ERR_OUT_OF_MEMORY;
			// Fall through.
		case 0:                         // CTS
			break;
		default:
			ret = ERR_VAR_REJECTED;
			break;
		}

		if (ret)
		{
			break;
		}

		ticonv_varname_to_utf8_sn(handle->model, entry->name, handle->updat->text, sizeof(handle->updat->text), entry->type);
		ticalcs_update_label(handle);

		ret = SEND_XDP(handle, size, entry->data);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
		}
		if (ret)
		{
			break;
		}

		ticalcs_info("Sent variable #%u", i);

		handle->updat->cnt2 = i+1;
		handle->updat->max2 = content->num_entries;
		ticalcs_update_pbar(handle);
	}

	if (mode & MODE_SEND_EXEC_ASM)
	{
		ret = ti82_send_asm_exec(handle, content->entries[0]);
		if (!ret)
		{
			ret = RECV_ERR(handle, &status);
			if (!ret)
			{
				ret = SEND_ACK(handle);
			}
		}
	}
	else if ((mode & MODE_SEND_ONE_VAR) || (mode & MODE_SEND_LAST_VAR))
	{
		ret = SEND_EOT(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
		}
	}

	return ret;
}

static int		send_var_8285	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return send_var_ns(handle, mode, content);
}

static int		send_var_8386	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	int ret = 0;
	unsigned int i;
	uint8_t rej_code;
	uint16_t status;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = content->num_entries;

	for (i = 0; !ret && i < content->num_entries; i++)
	{
		VarEntry *entry = content->entries[i];
		uint16_t size;

		if (!ticalcs_validate_varentry(entry))
		{
			ticalcs_critical("%s: skipping invalid content entry #%u", __FUNCTION__, i);
			continue;
		}

		if (entry->action == ACT_SKIP)
		{
			ticalcs_info("%s: skipping variable #%u because requested", __FUNCTION__, i);
			continue;
		}

		if (entry->size >= 65536U)
		{
			ticalcs_critical("%s: oversized variable has size %u, clamping to 65535", __FUNCTION__, entry->size);
			size = 65535;
		}
		else
		{
			size = (uint16_t)entry->size;
		}

		ret = SEND_RTS(handle, size, entry->type, entry->name);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
			if (!ret)
			{
				ret = RECV_SKP(handle, &rej_code);
				if (!ret)
				{
					ret = SEND_ACK(handle);
				}
			}
		}
		if (ret)
		{
			break;
		}

		switch (rej_code)
		{
		case DBUS_REJ_EXIT:
			ret = ERR_ABORT;
			break;
		case DBUS_REJ_SKIP:
			continue;
		case DBUS_REJ_MEMORY:
			ret = ERR_OUT_OF_MEMORY;
			// Fall through.
		case 0:                         // CTS
			break;
		default:
			ret = ERR_VAR_REJECTED;
			break;
		}

		if (ret)
		{
			break;
		}

		ticonv_varname_to_utf8_sn(handle->model, entry->name, handle->updat->text, sizeof(handle->updat->text), entry->type);
		ticalcs_update_label(handle);

		ret = SEND_XDP(handle, size, entry->data);
		if (!ret)
		{
			ret = RECV_ACK(handle, &status);
			if (!ret)
			{
				ret = SEND_EOT(handle);
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

	return ret;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	int ret;
	uint16_t unused;
	VarEntry *ve;
	uint16_t ve_size;

	content->model = handle->model;
	tifiles_comment_set_single_sn(content->comment, sizeof(content->comment));
	content->num_entries = 1;
	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	do
	{
		// silent request
		ret = SEND_REQ(handle, (uint16_t)vr->size, vr->type, vr->name);
		if (!ret)
		{
			ret = RECV_ACK(handle, &unused);
			if (!ret)
			{
				ret = RECV_VAR(handle, &ve_size, &ve->type, ve->name);
			}
		}
		if (ret)
		{
			break;
		}

		ve->size = ve_size;

		ret = SEND_ACK(handle);
		if (!ret)
		{
			ret = SEND_CTS(handle);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
			}
		}
		if (ret)
		{
			break;
		}

		ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size);
		ret = RECV_XDP(handle, &ve_size, ve->data);
		if (!ret)
		{
			ve->size = ve_size;
			ret = SEND_ACK(handle);
		}
	} while(0);

	return ret;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
{
	int nvar = 0;
	int ret = 0;
	uint16_t ve_size;

	ticalcs_strlcpy(handle->updat->text, _("Waiting for var(s)..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	content->model = handle->model;
	content->num_entries = 0;

	for (nvar = 0;; nvar++)
	{
		VarEntry *ve = tifiles_ve_create();
		int ret2;

		do
		{
			ticalcs_update_refresh(handle);
			if (ticalcs_update_canceled(handle))
			{
				ret = ERR_ABORT;
				goto error;
			}

			ret = RECV_VAR(handle, &ve_size, &(ve->type), ve->name);
			ve->size = ve_size;
		}
		while (ret == ERROR_READ_TIMEOUT);

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

		ret = SEND_CTS(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ticonv_varname_to_utf8_sn(handle->model, ve->name, handle->updat->text, sizeof(handle->updat->text), ve->type);
				ticalcs_update_label(handle);

				ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size);
				ret = RECV_XDP(handle, &ve_size, ve->data);
				if (!ret)
				{
					ve->size = ve_size;
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

	if (nvar == 1)
	{
		tifiles_comment_set_single_sn(content->comment, sizeof(content->comment));
		*vr = tifiles_ve_dup(content->entries[0]);
	}
	else
	{
		tifiles_comment_set_group_sn(content->comment, sizeof(content->comment));
		*vr = NULL;
	}

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	if (handle->model == CALC_TI82)
	{
		return rd_send_dumper(handle, "romdump.82p", romDumpSize82, romDump82);
	}
	else if (handle->model == CALC_TI83)
	{
		return rd_send_dumper(handle, "romdump.83p", romDumpSize83, romDump83);
	}
	else if (handle->model == CALC_TI85)
	{
		return rd_send_dumper(handle, "romdump.85s", romDumpSize85, romDump85);
	}
	else
	{
		return rd_send_dumper(handle, "romdump.86p", romDumpSize86, romDump86);
	}
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret = 0;
	unsigned int i;

	if (handle->model == CALC_TI83)
	{
		static const uint16_t keys[] = {
			0x40, 0x09, 0x09,       /* Quit, Clear, Clear, */
			0xFE63, 0x97, 0xDA,     /* Send(, 9, prgm */
			0xAB, 0xA8, 0xA6, 0x9D, /* R, O, M, D */
			0xAE, 0xA6, 0xA9, 0x05  /* U, M, P, Enter */
		};

		// Launch program by remote control
		for (i = 0; !ret && i < sizeof(keys) / sizeof(keys[0]); i++)
		{
			ret = send_key(handle, (uint32_t)(keys[i]));
			PAUSE(100);
		}
	}
	else if (handle->model == CALC_TI86)
	{
		static const uint16_t keys[] = {
			0x76, 0x08, 0x08,       /* Quit, Clear, Clear, */
			0x28, 0x3A, 0x34, 0x11, /* A, S, M, (, */
			0x39, 0x36, 0x34, 0x2B, /* R, O, M, D, */
			0x56, 0x4E, 0x51, 0x12, /* u, m, p, ), */
			0x06                    /* Enter */
		};
		uint16_t dummy;

		// Launch program by remote control
		for (i = 0; !ret && i < (sizeof(keys) / sizeof(keys[0])) - 1; i++)
		{
			ret = send_key(handle, (uint32_t)(keys[i]));
		}

		if (!ret)
		{
			ret = SEND_KEY(handle, keys[i]);
			if (!ret)
			{
				ret = RECV_ACK(handle, &dummy);
			}
			PAUSE(200);
		}
	}

	if (!ret)
	{
		// Get dump
		ret = rd_read_dump(handle, filename);

		// TI-86: normally there would be another ACK after the program exits, but the ROM dumper disables that behaviour.
	}

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	int ret = 0;
	static const uint16_t keys[] = {
		0x40, 0x09, 0x09,			/* Quit, Clear, Clear, */
		0x3e, 0x9d, 0x04,			/* Catalog, D, Down */
		0x04, 0x04, 0x05		/* Down, Down, Enter */
	};
	unsigned int i;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	// Input keys by remote control
	for (i = 0; !ret && i < sizeof(keys) / sizeof(keys[0]); i++)
	{
		ret = send_key(handle, (uint32_t)(keys[i]));
	}

	for (i = 0; !ret && i < strlen(vr->name); i++)
	{
		char c = toupper(vr->name[i]);

		if (isdigit(c))
		{
			ret = send_key(handle, (uint32_t)(0x008e + c - '0'));
		}
		else
		{
			ret = send_key(handle, (uint32_t)(0x009a + c - 'A'));
		}
	}

	if (!ret)
	{
		ret = send_key(handle, 0x0005);	// Enter
	}

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	int ret;
	static CalcLabEquipmentData lab_equipment_data_1 = { CALC_LAB_EQUIPMENT_DATA_TYPE_STRING, 4, 1, (const uint8_t *)"{7}", 0, 0, 4 }; // Request status command.
	CalcLabEquipmentData lab_equipment_data_2;

	// Use the TI-68k format because it's easier to deal with.
	ret = tixx_send_lab_equipment_data(handle, CALC_TI89, &lab_equipment_data_1);
	if (!ret)
	{
		ret = tixx_get_lab_equipment_data(handle, CALC_TI89, &lab_equipment_data_2);
		if (!ret)
		{
			memset((void *)infos, 0, sizeof(*infos));
			if (lab_equipment_data_2.type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST)
			{
				uint32_t item_count;
				const char * string_data;
				double * raw_values;
				// Parse list data to extract version number and other stuff, deducing the model.
				ret = tixx_convert_lab_equipment_data_ti68k_raw_list_to_string(&lab_equipment_data_2, &item_count, &raw_values, &string_data);
				if (!ret)
				{
					if (item_count >= 3)
					{
						unsigned int infos_mask = 0;
						unsigned long device_code = (unsigned long)raw_values[0];

						switch (device_code)
						{
						case 1UL: infos->model = CALC_CBL; break;
						case 5UL: infos->model = CALC_CBL2; break; // This should be handled by another code path.
						case 6UL: infos->model = CALC_LABPRO; break; // Ditto.
						case 7UL: infos->model = CALC_TIPRESENTER; break; // Tentative value, ditto.
						case 10UL: infos->model = CALC_CBR; break;
						case 11UL: infos->model = CALC_CBR2; break;
						default: ticalcs_critical("Unexpected device code %lu", device_code); break;
						}

						ticalcs_info("Found model %d", infos->model);
						if (infos->model == CALC_CBR || infos->model == CALC_CBR2 || infos->model == CALC_CBL2 || infos->model == CALC_LABPRO)
						{
							infos->battery = raw_values[2] > 0 ? 1 : 0;
							if (infos->model == CALC_CBR || infos->model == CALC_CBR2)
							{
								sprintf(infos->os_version, "%04u", (unsigned int)(raw_values[0] * 10000));
							}
							else
							{
								sprintf(infos->os_version, "%05u", (unsigned int)(raw_values[0] * 100000));
							}
							infos_mask = INFOS_BATTERY | INFOS_OS_VERSION;
							if (item_count >= 17)
							{
								sprintf(infos->user_defined_id, "%10.10g", raw_values[16]);
								infos_mask |= INFOS_USER_DEFINED_ID;
							}
						}
						if (infos->model != CALC_NONE)
						{
							infos_mask |= INFOS_CALC_MODEL;
						}
						infos->mask = (InfosMask)infos_mask;
					}
					else
					{
						ticalcs_warning("Expected at least 3 items in the list returned by the lab equipment");
					}
					tixx_free_converted_lab_equipment_data_string((void *)string_data);
					tixx_free_converted_lab_equipment_data_fpvals(raw_values);
				}
			}
			else
			{
				// Internal error.
				ticalcs_critical("XXX should have received TI-68k raw list");
				ret = ERR_INVALID_PARAMETER;
			}
			tifiles_hexdump(lab_equipment_data_2.data, lab_equipment_data_2.size);
		}
	}

	return ret;
}

extern const CalcFncts calc_80 =
{
	CALC_TI80,
	"TI80",
	"TI-80",
	"TI-80 ViewScreen",
	OPS_SCREEN,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
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
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&recv_screen_80,
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
	&noop_send_flash,
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
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_82 =
{
	CALC_TI82,
	"TI82",
	"TI-82",
	"TI-82",
	OPS_SCREEN | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP | /*OPS_LABEQUIPMENTDATA |*/
	FTS_BACKUP | FTS_NONSILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
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
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&recv_screen,
	&noop_get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_8285,
	&noop_recv_var,
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
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_83 =
{
	CALC_TI83,
	"TI83",
	"TI-83",
	"TI-83",
	OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	OPS_DELVAR | /*OPS_LABEQUIPMENTDATA |*/
	FTS_SILENT | FTS_MEMFREE | FTS_BACKUP,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "1L",   /* get_dirlist */
	 "",     /* get_memfree */
	 "2P",   /* send_backup */
	 "2P",   /* recv_backup */
	 "2P1L", /* send_var */
	 "1P1L", /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",   /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "1L",   /* del_var */
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
	&noop_is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_8386,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&noop_set_clock,
	&noop_get_clock,
	&del_var,
	&noop_new_folder,
	&noop_get_version,
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

extern const CalcFncts calc_85 =
{
	CALC_TI85,
	"TI85",
	"TI-85",
	"TI-85",
	OPS_SCREEN | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP | /*OPS_LABEQUIPMENTDATA |*/
	FTS_BACKUP | FTS_NONSILENT,
	PRODUCT_ID_NONE,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
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
	&noop_is_ready,
	&noop_send_key,
	&noop_execute,
	&recv_screen,
	&noop_get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_8285,
	&noop_recv_var,
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
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_86 =
{
	CALC_TI86,
	"TI86",
	"TI-86",
	"TI-86",
	OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP | /*OPS_LABEQUIPMENTDATA |*/
	FTS_SILENT | FTS_MEMFREE | FTS_BACKUP,
	PRODUCT_ID_NONE,
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
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "2P",   /* dump_rom_1 */
	 "2P",   /* dump_rom_2 */
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
	&noop_is_ready,
	&send_key,
	&noop_execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var_8386,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&noop_send_flash,
	&noop_recv_flash,
	&noop_send_os,
	&noop_recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
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
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_cbl =
{
	CALC_CBL,
	"CBL",
	"CBL",
	"CBL",
	OPS_ISREADY | OPS_DIRLIST /*| OPS_VARS*/ | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&get_dirlist,
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

extern const CalcFncts calc_cbr =
{
	CALC_CBR,
	"CBR",
	"CBR",
	"CBR",
	OPS_ISREADY | OPS_DIRLIST /*| OPS_VARS*/ | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&get_dirlist,
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

extern const CalcFncts calc_cbr2 =
{
	CALC_CBR2,
	"CBR2",
	"CBR2",
	"CBR2",
	OPS_ISREADY | OPS_DIRLIST /*| OPS_VARS*/ | OPS_VERSION | OPS_LABEQUIPMENTDATA |
	FTS_SILENT,
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
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&noop_send_key,
	&noop_execute,
	&noop_recv_screen,
	&get_dirlist,
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
