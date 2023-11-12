/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (c) 1999-2005  Romain Liévin
 *  Copyright (c) 2005  Benjamin Moody (ROM dumper)
 *  Copyright (c) 2006  Tyler Cassidy
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
	TI73/TI83+/TI84+ support.
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

#include "dbus_pkt.h"
#include "cmdz80.h"
#include "rom73.h"
#include "rom83p.h"
#include "rom84pc.h"
#include "romdump.h"
#include "keys83p.h"

#define SEND_RDY ti73_send_RDY
#define SEND_KEY ti73_send_KEY
#define SEND_SCR ti73_send_SCR
#define SEND_ACK ti73_send_ACK
#define SEND_VAR ti73_send_VAR
#define SEND_VAR2 ti73_send_VAR2
#define SEND_XDP ti73_send_XDP
#define SEND_REQ ti73_send_REQ
#define SEND_REQ2 ti73_send_REQ2
#define SEND_RTS ti73_send_RTS
#define SEND_CTS ti73_send_CTS
#define SEND_DEL ti73_send_DEL
#define SEND_VER ti73_send_VER
#define SEND_EOT ti73_send_EOT

#define RECV_ACK ti73_recv_ACK
#define RECV_VAR ti73_recv_VAR
#define RECV_VAR2 ti73_recv_VAR2
#define RECV_XDP ti73_recv_XDP
#define RECV_SKP ti73_recv_SKP
#define RECV_CTS ti73_recv_CTS

// Screen coordinates of the TI83+
#define TI73_ROWS  64
#define TI73_COLS  96
#define TI84PC_ROWS 240
#define TI84PC_COLS 320

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
		ret = RECV_ACK(handle, &status);	// when the key is received
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);	// after it completes the resulting action
		}
	}

	return ret;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	int ret;

	if (handle->model == CALC_TI73 && ve->type == TI73_ASM)
	{
		return ERR_VOID_FUNCTION;
	}

	// Go back to homescreen
	PAUSE(200);
	ret = send_key(handle, KEY83P_Quit);
	if (!ret)
	{
		ret = send_key(handle, KEY83P_Clear);
		if (!ret)
		{
			ret = send_key(handle, KEY83P_Clear);
		}
	}

	if (!ret)
	{
		// Launch program by remote control
		if (ve->type == TI83p_ASM)
		{
			ret = send_key(handle, KEY83P_Asm);
		}
		if (!ret)
		{
			ret = send_key(handle, KEY83P_Exec);
			if (!ret)
			{
				unsigned int i;
				for (i = 0; !ret && i < strlen(ve->name); i++)
				{
					const CalcKey *ck = ticalcs_keys_83p((ve->name)[i]);
					ret = send_key(handle, (uint32_t)(ck->normal.value));
				}

				if (!ret)
				{
					ret = send_key(handle, KEY83P_Enter);

					PAUSE(200);
				}
			}
		}
	}

	return ret;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;
	uint8_t *data = (uint8_t *)ticalcs_alloc_screen(65542U);
	if (data == NULL)
	{
		return ERR_MALLOC;
	}

	ret = SEND_SCR(handle);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			uint16_t pktsize;
			ret = RECV_XDP(handle, &pktsize, data);
			if (!ret || ret == ERR_CHECKSUM) // problem with checksum
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					if (pktsize == TI73_COLS * TI73_ROWS / 8)
					{
						/* TI-73 / 83+ / 84+ */
						sc->width = TI73_COLS;
						sc->height = TI73_ROWS;
						sc->clipped_width = TI73_COLS;
						sc->clipped_height = TI73_ROWS;
						sc->pixel_format = CALC_PIXFMT_MONO;
						*bitmap = (uint8_t *)ticalcs_realloc_screen(data, TI73_COLS * TI73_ROWS / 8);
					}
					else
					{
						/* TI-84+CSE */
						uint32_t size = pktsize;

						sc->width = TI84PC_COLS;
						sc->height = TI84PC_ROWS;
						sc->clipped_width = TI84PC_COLS;
						sc->clipped_height = TI84PC_ROWS;
						sc->pixel_format = CALC_PIXFMT_RGB_565_LE;

						while (1)
						{
							ret = RECV_XDP(handle, &pktsize, (uint8_t *)handle->buffer2);
							if (ret == ERR_EOT)
							{
								ret = SEND_ACK(handle);
								break;
							}

							*bitmap = (uint8_t *)ticalcs_realloc_screen(data, size + pktsize);
							if (*bitmap != NULL)
							{
								data = *bitmap;
								memcpy(data + size, handle->buffer2, pktsize);
								size += pktsize;

								ret = SEND_ACK(handle);
								if (ret)
								{
									break;
								}

								handle->updat->max1 = TI84PC_COLS * TI84PC_ROWS * 2;
								handle->updat->cnt1 = size;
								ticalcs_update_pbar(handle);
							}
							else
							{
								ticalcs_free_screen(data);
								ret = ERR_MALLOC;
								break;
							}
						}

						if (!ret)
						{
							*bitmap = (uint8_t *)ticalcs_alloc_screen(TI84PC_ROWS * TI84PC_COLS * 2);
							ret = ticalcs_screen_84pcse_rle_uncompress(data, size, *bitmap, TI84PC_ROWS * TI84PC_COLS * 2);
						}
					}
				}
			}
		}
	}

	if (ret)
	{
		ticalcs_free_screen(data);
		*bitmap = NULL;
	}

	return ret;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	int ret;
	TreeInfo *ti;
	GNode *folder, *root, *node;
	uint16_t unused;
	uint32_t memory;
	char *utf8;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}
	ti = (TreeInfo *)((*vars)->data);

	ret = SEND_REQ(handle, 0x0000, TI73_DIR, "\0\0\0\0\0\0\0", 0x00, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, &unused);
		if (!ret)
		{
			ret = RECV_XDP(handle, &unused, (uint8_t *)handle->buffer2);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					uint8_t * mem = (uint8_t *)handle->buffer2;
					memory = (((uint32_t)(mem[1])) << 8) | mem[0]; // Clamp mem_free to a 16-bit value.
				}
			}
		}
	}
	if (ret)
	{
		return ret;
	}

	ti->mem_free = memory;

	folder = dirlist_create_append_node(NULL, vars);
	if (!folder)
	{
		return ERR_MALLOC;
	}
	root = dirlist_create_append_node(NULL, apps);
	if (!root)
	{
		return ERR_MALLOC;
	}

	// Add permanent variables (Window, RclWindow, TblSet aka WINDW, ZSTO, TABLE)
	{
		VarEntry *ve;

		ve = tifiles_ve_create();
		ve->type = TI84p_WINDW;
		node = dirlist_create_append_node(ve, &folder);

		if (node != NULL)
		{
			if (handle->model != CALC_TI73)
			{
				ve = tifiles_ve_create();
				ve->type = TI84p_ZSTO;
				node = dirlist_create_append_node(ve, &folder);
			}

			if (node != NULL)
			{
				ve = tifiles_ve_create();
				ve->type = TI84p_TABLE;
				node = dirlist_create_append_node(ve, &folder);
			}
		}
	}

	if (!node)
	{
		return ERR_MALLOC;
	}

	for (;;) 
	{
		VarEntry *ve = tifiles_ve_create();
		uint16_t ve_size;
		int ret2;

		ret = RECV_VAR(handle, &ve_size, &ve->type, ve->name, &ve->attr, &ve->version);
		ve->size = ve_size;
		ret2 = SEND_ACK(handle);
		if (ret)
		{
			if (ret == ERR_EOT)
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

		if (ve->type == TI73_APPL)
		{
			/* Size is reported as a number of pages -- compute amount
			   of space consumed (the actual application may be
			   somewhat smaller.)  Note: the MSB of the "size" word is
			   the application's starting page number. */
			ve->size = (ve->size & 0xff) * 0x4000;
		}

		node = dirlist_create_append_node(ve, (ve->type != TI73_APPL) ? &folder : &root);
		if (!node)
		{
			return ERR_MALLOC;
		}

		utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
		ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Parsing %s"), utf8);
		ticonv_utf8_free(utf8);
		ticalcs_update_label(handle);
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	int ret;
	uint16_t unused;

	*ram = -1;
	*flash = -1;

	ret = SEND_REQ(handle, 0x0000, TI73_DIR, "\0\0\0\0\0\0\0", 0x00, 0x00);
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
					*ram = (((uint32_t)(mem[1])) << 8) | mem[0]; // Clamp mem_free to a 16-bit value.
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
	varname[4] = LSB(content->mem_address);
	varname[5] = MSB(content->mem_address);
	varname[6] = 0;
	varname[7] = 0;
	varname[8] = 0;

	do
	{
		ret = SEND_RTS(handle, content->data_length1, TI73_BKUP, varname, 0x00, content->version);
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
		if (!ret)
		{
			switch (rej_code)
			{
			case DBUS_REJ_EXIT:
			case DBUS_REJ_SKIP:
				return ERR_ABORT;
			case DBUS_REJ_MEMORY:
				return ERR_OUT_OF_MEMORY;
			case DBUS_REJ_VERSION:
				return ERR_VAR_VERSION;
			case 0:						// CTS
				break;
			default:
				return ERR_VAR_REJECTED;
			}

			handle->updat->cnt2 = 0;
			handle->updat->max2 = 3;
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

			ret = SEND_XDP(handle, content->data_length3, content->data_part3);
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

			ret = SEND_ACK(handle);
		}
	} while(0);

	return ret;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
	int ret;
	char varname[9];
	uint8_t attr, ver;

	content->model = handle->model;
	tifiles_comment_set_backup_sn(content->comment, sizeof(content->comment));

	varname[0] = 0;
	ret = SEND_REQ(handle, 0x0000, TI73_BKUP, "\0\0\0\0\0\0\0", 0x00, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
	}
	if (ret)
	{
		return ret;
	}

	do
	{
		ret = RECV_VAR(handle, &content->data_length1, &content->type, varname, &attr, &ver);
		if (ret)
		{
			break;
		}

		content->data_length2 = (uint8_t)varname[0] | (((uint16_t)(uint8_t)varname[1]) << 8);
		content->data_length3 = (uint8_t)varname[2] | (((uint16_t)(uint8_t)varname[3]) << 8);
		content->mem_address  = (uint8_t)varname[4] | (((uint16_t)(uint8_t)varname[5]) << 8);
		content->version = ver;
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

		handle->updat->cnt2 = 0;
		handle->updat->max2 = 3;
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
		handle->updat->cnt2++;
		ticalcs_update_pbar(handle);

		content->data_part4 = NULL;
	} while(0);

	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
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

		ret = SEND_RTS(handle, size, entry->type, entry->name, entry->attr, entry->version);
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
			break;
		case DBUS_REJ_VERSION:
			ret = ERR_VAR_VERSION;
			// Fall through.
		case 0:					// CTS
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
		ret = SEND_REQ(handle, (uint16_t)vr->size, vr->type, vr->name, vr->attr, vr->version);
		if (!ret)
		{
			ret = RECV_ACK(handle, &unused);
			if (!ret)
			{
				ret = RECV_VAR(handle, &ve_size, &ve->type, ve->name, &ve->attr, &ve->version);
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

			if (handle->model != CALC_TI73 && ve->type == TI83p_PIC)
			{
				if (ve->version >= 0xA)
				{
					content->model = CALC_TI84PC;
				}
				else
				{
					content->model = CALC_TI83P;
				}
			}

			ret = SEND_ACK(handle);
		}
	} while(0);

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos);

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	int ret = 0;
	FlashContent *ptr;
	unsigned int i, j;
	uint16_t size;
	int cpu15mhz = 0;

	// search for data header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_AMS || ptr->data_type == TI83p_APPL)
		{
			break;
		}
	}
	if (ptr == NULL)
	{
		return -1;
	}

	if (ptr->data_type == TI83p_AMS)
	{
		size = 0x100;
	}
	else if (ptr->data_type == TI83p_APPL)
	{
		size = 0x80;
	}
	else
	{
		return -1;
	}

	// check for 83+ Silver Edition (not usable in boot mode, sic!)
	if (handle->model != CALC_TI73 && ptr->data_type == TI83p_APPL)
	{
		CalcInfos infos;

		ret = get_version(handle, &infos);
		if (ret)
		{
			return ret;
		}
		cpu15mhz = infos.hw_version & 1;

		if (!infos.battery)
		{
			ticalcs_info("%s", _("Battery low, stopping flash app transfer"));
			return -1;
		}
	}

	ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
	ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

	ticonv_varname_to_utf8_sn(handle->model, ptr->name, handle->updat->text, sizeof(handle->updat->text), ptr->data_type);
	ticalcs_update_label(handle);

	handle->updat->cnt2 = 0;
	handle->updat->max2 = ptr->data_length;

	for (i = 0; !ret && i < ptr->num_pages; i++)
	{
		FlashPage *fp = ptr->pages[i];

		if ((ptr->data_type == TI83p_AMS) && (i == 1))	// need relocation ?
		{
			fp->addr = 0x4000;
		}

		for (j = 0; !ret && j < fp->size; j += size)
		{
			uint16_t addr = fp->addr + j;
			uint8_t* data = fp->data + j;

			ret = SEND_VAR2(handle, size, ptr->data_type, fp->flag, addr, fp->page);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
			}
			if (ret)
			{
				break;
			}

			if (handle->model == CALC_TI73 && ptr->data_type == TI83p_APPL)
			{
				ret = RECV_CTS(handle, 0);
			}	 // Depends on OS version?
			else
			{
				ret = RECV_CTS(handle, 10);
			}
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_XDP(handle, size, data);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							handle->updat->cnt2 += size;
							ticalcs_update_pbar(handle);
						}
					}
				}
			}
		}

		/* Note: 
			TI83+SE, TI84+ and TI84+SE don't need a pause (otherwise transfer fails).
			TI73 and TI83+ need a pause (otherwise transfer fails).
			Delay also causes OS transfers to fail on the 15Mhz calcs and unneeded for OS's
		*/
		if (!ret && !cpu15mhz && ptr->data_type == TI83p_APPL)
		{
			if (i == 1)
			{
				PAUSE(1000);		// This pause is NEEDED !
			}
			if (i == ptr->num_pages - 2)
			{
				PAUSE(2500);		// This pause is NEEDED !
			}
		}
	}

	if (!ret)
	{
		ret = SEND_EOT(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
		}
	}

	return ret;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	int ret;
	FlashPage *fp;
	uint16_t data_addr;
	uint16_t old_page = 0;
	uint16_t data_page;
	uint16_t data_length;
	uint8_t data_type;
	uint32_t size;
	int first_block;
	int page;
	int offset;
	uint8_t buffer[FLASH_PAGE_SIZE + 4];

	ticonv_varname_to_utf8_sn(handle->model, vr->name, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	content->model = handle->model;
	ticalcs_strlcpy(content->name, vr->name, sizeof(content->name));
	content->data_type = vr->type;
	content->device_type = handle->model == CALC_TI73 ? DEVICE_TYPE_73 : DEVICE_TYPE_83P;
	content->num_pages = 2048;	// TI83+ has 512 KB of FLASH max
	content->pages = tifiles_fp_create_array(content->num_pages);

	page = 0;
	fp = content->pages[page] = tifiles_fp_create();

	ret = SEND_REQ2(handle, 0x00, TI73_APPL, vr->name, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
	}
	if (ret)
	{
		return ret;
	}

	handle->updat->cnt2 = 0;
	handle->updat->max2 = vr->size;

	for (size = 0, first_block = 1, offset = 0; !ret;)
	{
		char name[9];
		int ret2;

		ret = RECV_VAR2(handle, &data_length, &data_type, name, &data_addr, &data_page);
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

		if (first_block)
		{
			old_page = data_page;

			fp->addr = data_addr & 0x4000;
			fp->page = data_page;
		}
		if (old_page != data_page)
		{
			fp->addr = data_addr & 0x4000;
			fp->page = old_page;
			fp->flag = 0x80;
			fp->size = offset;
			fp->data = (uint8_t *)tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
			memcpy(fp->data, buffer, fp->size);

			page++;
			offset = 0;
			old_page = data_page;

			fp = content->pages[page] = tifiles_fp_create();
		}

		ret = SEND_CTS(handle);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_XDP(handle, &data_length, &buffer[offset]);
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

		if (first_block)
		{
			first_block = 0;

			/* compute actual application size */
			if (buffer[0] == 0x80 && buffer[1] == 0x0f)
			{
				uint32_t len = ((uint32_t)(buffer[2])) << 24 | ((uint32_t)(buffer[3])) << 16 | ((uint32_t)(buffer[4])) << 8 | (uint32_t)(buffer[5]);
				handle->updat->max2 = len + 75;
			}
		}

		size += data_length;
		offset += data_length;

		handle->updat->cnt2 = size;
		ticalcs_update_pbar(handle);
	}

	fp->addr = data_addr & 0x4000;
	fp->page = old_page;
	fp->flag = 0x80;
	fp->size = offset;
	fp->data = (uint8_t *)tifiles_fp_alloc_data(FLASH_PAGE_SIZE);
	memcpy(fp->data, buffer, fp->size);
	page++;

	content->num_pages = page;

	return ret;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	int ret;
	uint16_t unused;
	uint16_t varsize;
	uint8_t vartype;
	char varname[9];
	uint8_t varattr;
	uint8_t version;
	uint8_t data[16];
	int i;

	ticalcs_strlcpy(handle->updat->text, "ID-LIST", sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	ret = SEND_REQ(handle, 0x0000, TI73_IDLIST, "\0\0\0\0\0\0\0", 0x00, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, &unused);
		if (!ret)
		{
			ret = RECV_VAR(handle, &varsize, &vartype, varname, &varattr, &version);
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
							ret = RECV_XDP(handle, &varsize, data);
							if (!ret)
							{
								ret = SEND_ACK(handle);
								if (!ret)
								{
									i = data[9];
									data[9] = data[10];
									data[10] = i;

									for (i = 4; i < varsize; i++)
									{
										sprintf((char *)&id[2 * (i-4)], "%02x", data[i]);
									}
									id[7*2] = '\0';
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
	// Send dumping program
	if (handle->model == CALC_TI73)
	{
		return rd_send_dumper(handle, "romdump.73p", romDumpSize73, romDump73);
	}
	else
	{
		CalcInfos infos;

		int ret = get_version(handle, &infos);
		if (!ret)
		{
			if (infos.hw_version < 5)
			{
				ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize8Xp, romDump8Xp);
			}
			else
			{
				ret = rd_send_dumper(handle, "romdump.8Xp", romDumpSize84pc, romDump84pc);
			}
		}

		return ret;
	}
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	static const uint16_t keys_83p[] = {
		0x40, 0x09, 0x09, 0xFC9C, /* Quit, Clear, Clear, Asm( */
		0xDA, 0xAB, 0xA8, 0xA6,   /* prgm, R, O, M */
		0x9D, 0xAE, 0xA6, 0xA9,   /* D, U, M, P */
		0x86, 0x05 };             /* ), Enter */

	static const uint16_t keys_73[] = {
		0x40, 0x09, 0x09, 0xDA,   /* Quit, Clear, Clear, prgm */
		0xAB, 0xA8, 0xA6, 0x9D,   /* R, O, M, D, */
		0xAE, 0xA6, 0xA9, 0x05 }; /* U, M, P, Enter */

	int ret = 0;
	const uint16_t *keys;
	unsigned int i, nkeys;

	if (handle->model == CALC_TI73)
	{
		keys = keys_73;
		nkeys = sizeof(keys_73) / sizeof(keys_73[0]);
	}
	else
	{
		keys = keys_83p;
		nkeys = sizeof(keys_83p) / sizeof(keys_83p[0]);
	}

	// Launch program by remote control
	PAUSE(200);
	for (i = 0; !ret && i < nkeys - 1; i++)
	{
		ret = send_key(handle, (uint32_t)(keys[i]));
		PAUSE(100);
	}

	if (!ret)
	{
		// This fixes a 100% reproducible timeout: send_key normally requests an ACK,
		// but when the program is running, no ACK is sent. Therefore, hit the Enter key
		// without requesting an ACK.
		ret = SEND_KEY(handle, keys[i]);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL); // when the key is received
			if (!ret)
			{
				PAUSE(1000);

				// Get dump
				// (Normally there would be another ACK after the program exits,
				// but the ROM dumper disables that behavior)
				ret = rd_read_dump(handle, filename);
			}
		}
	}

	return ret;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	int ret;
	uint8_t buffer[9];
	uint32_t calc_time;

	struct tm ref, cur;
	time_t r, c, now;

	time(&now);	// retrieve current DST setting
#ifdef HAVE_LOCALTIME_R
	localtime_r(&now, &ref);
#else
	memcpy(&ref, localtime(&now), sizeof(struct tm));
#endif

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
	//printf("%s\n", asctime(&ref));

	cur.tm_year = _clock->year - 1900;
	cur.tm_mon = _clock->month - 1;
	cur.tm_mday = _clock->day;
	cur.tm_hour = _clock->hours;
	cur.tm_min = _clock->minutes;
	cur.tm_sec = _clock->seconds;
	cur.tm_isdst = 1;
	c = mktime(&cur);
	//printf("%s\n", asctime(&cur));

	calc_time = (uint32_t)difftime(c, r);

	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = MSB(MSW(calc_time));
	buffer[3] = LSB(MSW(calc_time));
	buffer[4] = MSB(LSW(calc_time));
	buffer[5] = LSB(LSW(calc_time));
	buffer[6] = _clock->date_format;
	buffer[7] = _clock->time_format;
	buffer[8] = 0xff;

	ticalcs_strlcpy(handle->updat->text, _("Setting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	ret = SEND_RTS(handle, 13, TI73_CLK, "\0x08\0\0\0\0\0\0\0", 0x00, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_CTS(handle, 13);
			if (!ret)
			{
				ret = SEND_ACK(handle);
				if (!ret)
				{
					ret = SEND_XDP(handle, 9, buffer);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = SEND_EOT(handle);
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
	int ret;
	uint16_t varsize;
	uint8_t vartype;
	uint8_t varattr;
	uint8_t version;
	char varname[9];
	uint8_t * buffer = (uint8_t *)handle->buffer2;
	uint32_t calc_time;

	struct tm ref, cur;
	time_t r, c, now;

	ticalcs_strlcpy(handle->updat->text, _("Getting clock..."), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	ret = SEND_REQ(handle, 0x0000, TI73_CLK, "\0x08\0\0\0\0\0\0\0", 0x00, 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			ret = RECV_VAR(handle, &varsize, &vartype, varname, &varattr, &version);
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
							ret = RECV_XDP(handle, &varsize, buffer);
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

	if (!ret)
	{
		calc_time = (((uint32_t)(buffer[2])) << 24) | (((uint32_t)(buffer[3])) << 16) | (((uint32_t)(buffer[4])) << 8) | (uint32_t)(buffer[5]);
		//printf("<%08x>\n", time);

		time(&now);	// retrieve current DST setting
#ifdef HAVE_LOCALTIME_R
		localtime_r(&now, &ref);
#else
		memcpy(&ref, localtime(&now), sizeof(struct tm));
#endif
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
		//printf("%s\n", asctime(&ref));

		c = r + calc_time;
#ifdef HAVE_LOCALTIME_R
		localtime_r(&c, &cur);
#else
		memcpy(&cur, localtime(&c), sizeof(struct tm));
#endif
		//printf("%s\n", asctime(cur));

		_clock->year = cur.tm_year + 1900;
		_clock->month = cur.tm_mon + 1;
		_clock->day = cur.tm_mday;
		_clock->hours = cur.tm_hour;
		_clock->minutes = cur.tm_min;
		_clock->seconds = cur.tm_sec;

		_clock->date_format = buffer[6];
		_clock->time_format = buffer[7];
	}

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	int ret;
	char *utf8;

	utf8 = ticonv_varname_to_utf8(handle->model, vr->name, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	ret = SEND_DEL(handle, (uint16_t)vr->size, vr->type, vr->name, vr->attr);
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

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	int ret;
	uint16_t length;
	uint8_t * buffer = (uint8_t *)handle->buffer2;

	ret = SEND_VER(handle);
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
		if (length >= 11)
		{
			if (handle->model == CALC_TI73)
			{
				ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1x.%02x", buffer[0], buffer[1]);
				ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1x.%02x", buffer[2], buffer[3]);
			}
			else
			{
				ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1i.%02i", buffer[0], buffer[1]);
				ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1i.%02i", buffer[2], buffer[3]);
			}
			infos->battery = (buffer[4] & 1) ? 0 : 1;
			infos->hw_version = buffer[5];
			switch(buffer[5])
			{
			case 0: infos->model = CALC_TI83P; break;
			case 1: infos->model = CALC_TI83P; break;
			case 2: infos->model = CALC_TI84P; break;
			case 3: infos->model = CALC_TI84P; break;
			case 5: infos->model = CALC_TI84PC; break;
			default: infos->model = CALC_TI84PC; break; // If new models ever arise, they'll probably be 84+CSE or newer anyway.
			}
			infos->language_id = buffer[6];
			infos->sub_lang_id = buffer[7];
			infos->mask = (InfosMask)(INFOS_BOOT_VERSION | INFOS_OS_VERSION | INFOS_BATTERY_ENOUGH | INFOS_HW_VERSION | INFOS_CALC_MODEL | INFOS_LANG_ID | INFOS_SUB_LANG_ID);

			tifiles_hexdump(buffer, length);
			ticalcs_info(_("  OS: %s"), infos->os_version);
			ticalcs_info(_("  BIOS: %s"), infos->boot_version);
			ticalcs_info(_("  HW: %i"), infos->hw_version);
			ticalcs_info(_("  Battery: %s"), infos->battery ? _("good") : _("low"));
		}
		else
		{
			ticalcs_warning("%s", _("Bad data length for version information"));
		}
	}

	return ret;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	int ret = 0;
	FlashContent *ptr;
	int i, nblocks;
	uint16_t size = 0xE8;

	// search for cert header
	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->data_type == TI83p_CERT)
		{
			break;
		}
	}

	if (ptr != NULL)
	{
		// send content
		ticalcs_info(_("FLASH name: \"%s\""), ptr->name);
		ticalcs_info(_("FLASH size: %i bytes."), ptr->data_length);

		nblocks = ptr->data_length / size;
		handle->updat->max2 = nblocks;

		ret = SEND_VAR2(handle, size, ptr->data_type, 0x04, 0x4000, 0x00);
		if (!ret)
		{
			ret = RECV_ACK(handle, NULL);
			if (!ret)
			{
				ret = RECV_CTS(handle, 10);
				if (!ret)
				{
					ret = SEND_ACK(handle);
				}
			}
		}

		for (i = 0; !ret && i <= nblocks; i++)
		{
			uint16_t length = size;

			ret = SEND_XDP(handle, length, (ptr->data_part) + length * i);
			if (!ret)
			{
				ret = RECV_ACK(handle, NULL);
				if (!ret)
				{
					ret = RECV_CTS(handle, size);
					if (!ret)
					{
						ret = SEND_ACK(handle);
						if (!ret)
						{
							handle->updat->cnt2 = i;
							ticalcs_update_pbar(handle);
						}
					}
				}
			}
		}

		if (!ret)
		{
			ret = SEND_EOT(handle);
			ticalcs_info("%s", _("Header sent completely."));
		}
	}

	return ret;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	int ret;
	int i;
	uint8_t buf[256];

	ticalcs_strlcpy(handle->updat->text, _("Receiving certificate"), sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	content->model = handle->model;
	content->name[0] = 0;
	content->data_type = TI83p_CERT;
	content->device_type = 0x73;
	content->num_pages = 0;
	content->data_part = (uint8_t *)tifiles_ve_alloc_data(2 * 1024 * 1024);	// 2MB max

	ret = SEND_REQ2(handle, 0x00, TI83p_GETCERT, "\0\0\0\0\0\0\0", 0x00);
	if (!ret)
	{
		ret = RECV_ACK(handle, NULL);
		if (!ret)
		{
			// No need to take busy around this libticables call, it's already been taken by ticalcs_calc_recv_cert() before this is reached.
			ret = ticables_cable_recv(handle->cable, buf, 4);	//VAR w/ no header
			if (!ret)
			{
				ticalcs_info(" TI->PC: VAR");
				ret = SEND_ACK(handle);

				for (i = 0, content->data_length = 0; !ret; i++)
				{
					uint16_t block_size;

					ret = SEND_CTS(handle);
					if (!ret)
					{
						ret = RECV_ACK(handle, NULL);
						if (!ret)
						{
							ret = RECV_XDP(handle, &block_size, content->data_part);
							if (!ret)
							{
								ret = SEND_ACK(handle);
								if (!ret)
								{
									content->data_length += block_size;

									handle->updat->cnt2 += block_size;
									ticalcs_update_pbar(handle);
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

extern const CalcFncts calc_73 =
{
	CALC_TI73,
	"TI73",
	"TI-73",
	"TI-73",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS |
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_VERSION | OPS_OS /*| OPS_LABEQUIPMENTDATA*/ |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_BACKUP,
	PRODUCT_ID_TI73,
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
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_83p =
{
	CALC_TI83P,
	"TI83+",
	"TI-83 Plus",
	"TI-83 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS |
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_DELVAR | OPS_VERSION | OPS_OS /*| OPS_LABEQUIPMENTDATA*/ |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	PRODUCT_ID_TI83P,
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
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_84p =
{
	CALC_TI84P,
	"TI84+",
	"TI-84 Plus",
	"TI-84 Plus",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS |
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_OS /*| OPS_LABEQUIPMENTDATA*/ |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	PRODUCT_ID_TI84P,
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
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};

extern const CalcFncts calc_84pcse =
{
	CALC_TI84PC,
	"TI84+CSE",
	"TI-84 Plus Color Silver Edition",
	"TI-84 Plus Color Silver Edition",
	OPS_ISREADY | OPS_KEYS | OPS_SCREEN | OPS_DIRLIST | OPS_BACKUP | OPS_VARS |
	OPS_FLASH | OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK | OPS_DELVAR | OPS_VERSION | OPS_OS /*| OPS_LABEQUIPMENTDATA*/ |
	FTS_SILENT | FTS_MEMFREE | FTS_FLASH | FTS_CERT | FTS_BACKUP,
	PRODUCT_ID_TI84PCSE,
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
	 "",     /* send_all_vars_backup */
	 "",     /* recv_all_vars_backup */
	 "",     /* send_lab_equipment_data */
	 "",     /* get_lab_equipment_data */
	 "",     /* del_folder */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&noop_send_var_ns,
	&noop_recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&noop_new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
	&noop_rename_var,
	&noop_change_attr,
	&noop_send_all_vars_backup,
	&noop_recv_all_vars_backup,
	&noop_send_lab_equipment_data,
	&noop_get_lab_equipment_data,
	&noop_del_folder,
};
