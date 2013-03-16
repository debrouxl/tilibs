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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	Nspire support thru DirectUsb link.
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

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

int nsp_reset = 0;

/*
	How things behave depepnding on OS version...
	- 1.1: no login request
	- 1.2 & 1.3: hand-held request LOGIN connection three seconds after device reset
	- 1.4: login request + service disconnect
	
  */

static int		is_ready	(CalcHandle* handle)
{
	static int rom_11 = 0;
	static int rom_14 = 0;

	// XXX debrouxl forcing a full sequence makes all operations a LOT slower (especially on
	// older OS), but fixes the 100% reproducible loss of connection after a few "Status"
	// operations or a single "List" operation, on my S-0907A non-CAS Nspire.
	// Tested with OS 1.1.9253, 1.2.2398, 1.3.2407, 1.4.11653, 1.6.4379, 1.7.2471, 1.7.1.50.
	//
	// A better fix is needed in the mid- and long-term.

	// checking for OS version and LOGIN packet
	//if(!nsp_reset)
	{
		// XXX debrouxl moving those two lines above the 'if(!nsp_reset)' test fixes connection
		// loss, but linking with at least 1.7.1.50 does not work properly after that: at least
		// directory listing and screenshot don't do anything beyond the "Status" probe.
		TRYF(nsp_addr_request(handle));
		TRYF(nsp_addr_assign(handle, NSP_DEV_ADDR));

		// XXX after commenting the following block of code, sending many Status or Dirlist
		// requests in short succession often triggers memory corruption (hangs, reboots,
		// a variable amount of black pixels on the screen) on (at least) Nspire (CAS) OS 1.7...
		{
			int old;
			int ret;

			ticalcs_info("  waiting for LOGIN request (OS >= 1.2 check)...");
			old = ticables_options_set_timeout(handle->cable, 40);	// 3s mini

			ret = nsp_cmd_r_login(handle);	// no call to TRYF(nsp_send_nack(handle)) because nack is managed in nsp_recv_data()

			ticables_options_set_timeout(handle->cable, old);
			if(ret)
			{
				ticalcs_info("OS = 1.1");
				rom_11 = !0;

				TRYF(nsp_addr_request(handle));
				TRYF(nsp_addr_assign(handle, NSP_DEV_ADDR));
			}
			else
			{
				ret = nsp_recv_disconnect(handle);
				if(ret)
				{
					ticalcs_info("OS = 1.2 or 1.3");
					rom_14 = 0;
				}
				else
				{
					ticalcs_info("OS = 1.4 or later");
					rom_14 = !0;
				}

			}
		}

		//nsp_reset = !0;
	}

	// Use ECHO packet as ready check
	{
		char str[] = "ready";
		uint32_t size;
		uint8_t *data;

		TRYF(nsp_session_open(handle, SID_ECHO));

		TRYF(nsp_cmd_s_echo(handle, strlen(str)+1, (uint8_t *)str));
		TRYF(nsp_cmd_r_echo(handle, &size, &data));
		g_free(data);

		TRYF(nsp_session_close(handle));
	}

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

static uint8_t* rle_uncompress(CalcScreenCoord* sc, const uint8_t *src, uint32_t size, int type)
{
	if (type == 0)
	{
		// Nspire (CAS) Clickpad & Touchpad, 4 bpp
		uint8_t *dst = g_malloc(sc->width * sc->height / 2);
		uint8_t *q;
		uint32_t i;

		for(i = 0, q = dst; i < size;)
		{
			int8_t rec = src[i++];

			if(rec >= 0)
			{
				// Positive count: "repeat 8-bit value" block.
				uint8_t cnt = ((uint8_t)rec) + 1;
				uint8_t val = src[i++];

				memset(q, val, cnt);
				q += cnt;
			}
			else
			{
				// Negative count: "verbatim" block of 8-bit values.
				uint8_t cnt = ((uint8_t)-rec) + 1;

				memcpy(q, src+i, cnt);
				q += cnt;
				i += cnt;
			}
		}

		return dst;
	}
	else // if (type == 1)
	{
		// Nspire (CAS) CX, 16 bpp
		uint8_t *dst = g_malloc(sc->width * sc->height * 2);
		uint8_t *q;
		uint32_t i;

		for(i = 0, q = dst; i < size;)
		{
			int8_t rec = src[i++];

			if(rec >= 0)
			{
				// Positive count: "repeat 32-bit value" block.
				uint8_t cnt = ((uint8_t)rec) + 1;
				uint32_t val;
				uint8_t j;

				memcpy(&val, src + i, sizeof(uint32_t));
				for (j = 0; j < cnt; j++)
				{
					//*((uint32_t *)q) = val;
					memcpy(q, &val, 4);
					q += 4;
				}
				i += 4;
			}
			else
			{
				// Negative count: "verbatim" block of 32-bit values.
				uint8_t cnt = ((uint8_t)-rec) + 1;

				memcpy(q, src + i, cnt * 4);
				q += cnt * 4;
				i += cnt * 4;
			}
		}

		return dst;
	}
}

// Forward declaration
static int		get_version	(CalcHandle* handle, CalcInfos* infos);

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint32_t size = 0;
	uint8_t cmd, *data;
	int retval;
	int type;
	CalcInfos infos;

	// First of all, we have to identify the Nspire model.
	retval = get_version(handle, &infos);
	if (!retval)
	{
		if (infos.bits_per_pixel == 4)
		{
			// Nspire (CAS) Clickpad or Touchpad.
			type = 0;
		}
		else if (infos.bits_per_pixel == 16)
		{
			// Nspire (CAS) CX.
			type = 1;
		}
		else
		{
			ticalcs_critical(_("Unknown calculator model with %d bpp\n"), infos.bits_per_pixel);
			return ERR_UNSUPPORTED; // THIS RETURNS !
		}

		// Do screenshot
		TRYF(nsp_session_open(handle, SID_SCREEN_RLE));

		TRYF(nsp_cmd_s_screen_rle(handle, 0));
		TRYF(nsp_cmd_r_screen_rle(handle, &cmd, &size, &data));
		sc->width = sc->clipped_width = (data[8] << 8) | data[9];
		sc->height = sc->clipped_height = (data[10] << 8) | data[11];
		//size = GUINT32_FROM_BE(*((uint32_t *)(data)));
		size = (  (((uint32_t)data[0]) << 24)
		        | (((uint32_t)data[1]) << 16)
		        | (((uint32_t)data[2]) <<  8)
		        | (((uint32_t)data[3])      ));
		TRYF(nsp_cmd_r_screen_rle(handle, &cmd, &size, &data));

		TRYF(nsp_session_close(handle));

		*bitmap = rle_uncompress(sc, data, size, type);
		g_free(data);

		if(*bitmap == NULL)
		{
			return ERR_MALLOC;
		}

		retval = 0;
	}

	return retval;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{
	TreeInfo *ti;
	int err;
	GNode *root, *folder = NULL;
	char varname[VARNAME_MAX];
	uint32_t varsize;
	uint8_t vartype;
	int i;

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

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));
	TRYF(nsp_cmd_s_dir_attributes(handle, "/"));
	TRYF(nsp_cmd_r_dir_attributes(handle, NULL, NULL, NULL));
	TRYF(nsp_session_close(handle));

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	TRYF(nsp_cmd_s_dir_enum_init(handle, "/"));
	TRYF(nsp_cmd_r_dir_enum_init(handle));

	for(;;)
	{
		VarEntry *fe;
		GNode *node;

		TRYF(nsp_cmd_s_dir_enum_next(handle));
		err = nsp_cmd_r_dir_enum_next(handle, varname, &varsize, &vartype);

		if (err == ERR_EOT)
			break;
		else if (err != 0)
			return err;

		fe = tifiles_ve_create();
		strcpy(fe->folder, varname);
		strcpy(fe->name, varname);
		fe->size = varsize;
		fe->type = vartype;
		fe->attr = ATTRB_NONE;

		node = g_node_new(fe);
		folder = g_node_append(*vars, node);

		ticalcs_info(_("Name: %s | Type: %8s | Attr: %i  | Size: %08X"),
			fe->name,
			tifiles_vartype2string(handle->model, fe->type),
			fe->attr,
			fe->size);
	}

	TRYF(nsp_cmd_s_dir_enum_done(handle));
	TRYF(nsp_cmd_r_dir_enum_done(handle));

	for(i = 0; i < (int)g_node_n_children(*vars); i++) 
	{
		char *folder_name;
		char *u1, *u2;

		folder = g_node_nth_child(*vars, i);
		folder_name = ((VarEntry *) (folder->data))->name;
		vartype = ((VarEntry *) (folder->data))->type;

		// Skip entries whose type is 0 (TNS), for example themes.csv on OS 3.0+.
		if (vartype == 0)
		{
			ticalcs_info(_("Not enumerating documents in %s because it's not a folder"), folder_name);
			continue;
		}

		ticalcs_info(_("Directory listing in <%s>..."), folder_name);

		TRYF(nsp_cmd_s_dir_enum_init(handle, folder_name));
		TRYF(nsp_cmd_r_dir_enum_init(handle));

		for(;;)
		{
			VarEntry *ve = tifiles_ve_create();
			GNode *node;
			char *ext;

			TRYF(nsp_cmd_s_dir_enum_next(handle));
			err = nsp_cmd_r_dir_enum_next(handle, varname, &varsize, &vartype);

			if (err == ERR_EOT)
				break;
			else if (err != 0)
				return err;

			ext = tifiles_fext_get(varname);
			strcpy(ve->folder, folder_name);
			ve->size = varsize;
			ve->type = tifiles_fext2vartype(handle->model, ext);
			ve->attr = ATTRB_NONE;
			// Just a sanity check
			if (ext)
			{
				// Did the file name have any non-empty extension ?
				if (*ext)
				{
					// Do we know about this file type ?
					if (ve->type < NSP_MAXTYPES)
					{
						// Then we can remove the exension.
						*(ext-1) = '\0';
					}
					// else don't remove the extension.
				}
				// else there is no extension to remove.
			}
			strcpy(ve->name, varname);

			node = g_node_new(ve);
			g_node_append(folder, node);

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
		}

		TRYF(nsp_cmd_s_dir_enum_done(handle));
		TRYF(nsp_cmd_r_dir_enum_done(handle));
	}

	TRYF(nsp_session_close(handle));

	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	uint32_t size;
	uint8_t cmd, *data;
	//int i;

	TRYF(nsp_session_open(handle, SID_DEV_INFOS));

	TRYF(nsp_cmd_s_dev_infos(handle, CMD_DI_VERSION));
	TRYF(nsp_cmd_r_dev_infos(handle, &cmd, &size, &data));

	//i = 0;
	//*flash = (uint32_t)GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	*flash = (  (((uint32_t)data[4]) << 24)
	          | (((uint32_t)data[5]) << 16)
	          | (((uint32_t)data[6]) <<  8)
	          | (((uint32_t)data[7])      ));

	//i = 16;
	//*ram = (uint32_t)GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	*ram = (  (((uint32_t)data[20]) << 24)
	        | (((uint32_t)data[21]) << 16)
	        | (((uint32_t)data[22]) <<  8)
	        | (((uint32_t)data[23])      ));

	g_free(data);
	TRYF(nsp_session_close(handle));

	return 0;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	char *utf8;
	uint8_t status;
	const char * dot_if_any = ".";

	update_->cnt2 = 0;
	update_->max2 = 1;
	update_->pbar();

	{
		VarEntry *ve = content->entries[0];
		gchar *path;
		int err;

		if(ve->action == ACT_SKIP)
			return 0;

		if(!strlen(ve->folder))
			return ERR_ABORT;

		TRYF(nsp_session_open(handle, SID_FILE_MGMT));

		// Don't add a dot if this file type is unknown.
		if (ve->type >= NSP_MAXTYPES)
			dot_if_any = "";

		path = g_strconcat("/", ve->folder, "/", ve->name, dot_if_any, 
			tifiles_vartype2fext(handle->model, ve->type), NULL);

		utf8 = ticonv_varname_to_utf8(handle->model, path, ve->type);
		g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
		g_free(utf8);
		update_label();

		err = nsp_cmd_s_put_file(handle, path, ve->size);
		g_free(path);
		if (err)
		{
			return err;
		}
		TRYF(nsp_cmd_r_put_file(handle));

		TRYF(nsp_cmd_s_file_contents(handle, ve->size, ve->data));
		TRYF(nsp_cmd_r_status(handle, &status));

		TRYF(nsp_session_close(handle));
	}

	return 0;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	char *path;
	uint8_t *data = NULL;
	VarEntry *ve;
	char *utf8;
	int err;
	const char * dot_if_any = ".";

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	// Don't add a dot if this file type is unknown.
	if (vr->type >= NSP_MAXTYPES)
		dot_if_any = "";

	path = g_strconcat("/", vr->folder, "/", vr->name, dot_if_any, 
		tifiles_vartype2fext(handle->model, vr->type), NULL);
	utf8 = ticonv_varname_to_utf8(handle->model, path, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
	update_label();

	err = nsp_cmd_s_get_file(handle, path);
	g_free(path);
	if (err)
	{
		return err;
	}
	TRYF(nsp_cmd_r_get_file(handle, &(vr->size)));

	TRYF(nsp_cmd_s_file_ok(handle));
	if (vr->size)
		TRYF(nsp_cmd_r_file_contents(handle, &(vr->size), &data));
	TRYF(nsp_cmd_s_status(handle, ERR_OK));

	content->model = handle->model;
	strcpy(content->comment, tifiles_comment_set_single());
	content->num_entries = 1;

	content->entries = tifiles_ve_create_array(1);
	ve = content->entries[0] = tifiles_ve_create();
	memcpy(ve, vr, sizeof(VarEntry));

	ve->data = tifiles_ve_alloc_data(ve->size);
	if (data && ve->data)
	{
		memcpy(ve->data, data, ve->size);
	}
	g_free(data);

	// XXX don't check the result of this call, to enable reception of variables from Nspires running OS >= 1.7.
	// Those versions send a martian packet:
	// * a src port never seen before in the conversation;
	// * an improper dest port;
	// * a 1-byte payload containing 02 (i.e. an invalid address for the next packet).
	// * .ack = 0x00 (instead of 0x0A).
	nsp_session_close(handle);

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
	uint8_t status, value;

	if(content == NULL)
		return -1;

	tifiles_hexdump(content->data_part + content->data_length - 16, 16);

	TRYF(nsp_session_open(handle, SID_OS_INSTALL));

	TRYF(nsp_cmd_s_os_install(handle, content->data_length));
	TRYF(nsp_cmd_r_os_install(handle));

	TRYF(nsp_cmd_s_os_contents(handle, 253, content->data_part));
	TRYF(nsp_cmd_r_status(handle, &status));
	TRYF(nsp_cmd_s_os_contents(handle, content->data_length - 253, content->data_part + 253));

	update_->cnt2 = 0;
	update_->max2 = 100;
	update_->pbar();

	do
	{
		TRYF(nsp_cmd_r_progress(handle, &value));

		update_->cnt2 = value;
		update_->pbar();
	} while(value < 100 );

	TRYF(nsp_session_close(handle));

	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	uint32_t size;
	uint8_t cmd, *data;

	TRYF(nsp_session_open(handle, SID_DEV_INFOS));

	TRYF(nsp_cmd_s_dev_infos(handle, CMD_DI_VERSION));
	TRYF(nsp_cmd_r_dev_infos(handle, &cmd, &size, &data));

	strncpy((char *)id, (char*)(data + 82), 28);

	g_free(data);
	TRYF(nsp_session_close(handle));

	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	uint32_t varsize;
	uint8_t *data;
	int err;
	FILE *f;

	ticalcs_info("FIXME: make ROM dumping work above OS 1.x");

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	f = fopen(filename, "wb");
	if (f == NULL)
	{
		return ERR_OPEN_FILE;
	}

	err = nsp_cmd_s_get_file(handle, "../phoenix/install/TI-Nspire.tnc");
	if (!err)
	{
		err = nsp_cmd_r_get_file(handle, &varsize);
		if (!err)
		{
			err = nsp_cmd_s_file_ok(handle);
			if (!err)
			{
				err = nsp_cmd_r_file_contents(handle, &varsize, &data);
				if (!err)
				{
					err = nsp_cmd_s_status(handle, ERR_OK);
					if (!err)
					{
						if (fwrite(data, varsize, 1, f) < 1)
						{
							err = ERR_SAVE_FILE;
						}
					}
				}
			}
		}
	}

	fclose(f);

	nsp_session_close(handle);
	return err;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	char *utf81, *utf82;
	char *path1, *path2;
	int err;
	const char * dot_if_any = ".";

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	// Don't add a dot if this file type is unknown.
	if (oldname->type >= NSP_MAXTYPES)
		dot_if_any = "";
	path1 = g_strconcat("/", oldname->folder, "/", oldname->name, dot_if_any,
		tifiles_vartype2fext(handle->model, oldname->type), NULL);

	dot_if_any = ".";
	// Don't add a dot if this file type is unknown.
	if (oldname->type >= NSP_MAXTYPES)
		dot_if_any = "";
	path2 = g_strconcat("/", newname->folder, "/", newname->name, dot_if_any,
		tifiles_vartype2fext(handle->model, newname->type), NULL);
	utf81 = ticonv_varname_to_utf8(handle->model, path1, oldname->type);
	utf82 = ticonv_varname_to_utf8(handle->model, path2, newname->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Renaming %s to %s..."), utf81, utf82);
	g_free(utf82);
	g_free(utf81);
	update_label();

	err = nsp_cmd_s_rename_file(handle, path1, path2);
	g_free(path2);
	g_free(path1);
	if (err)
	{
		return err;
	}
	TRYF(nsp_cmd_r_rename_file(handle));

	TRYF(nsp_session_close(handle));

	return 0;
}

static int		change_attr	(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char *utf8;
	char *path;
	int err;
	const char * dot_if_any = ".";

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	// Don't add a dot if this file type is unknown.
	if (vr->type >= NSP_MAXTYPES)
		dot_if_any = "";

	path = g_strconcat("/", vr->folder, "/", vr->name, dot_if_any,
		tifiles_vartype2fext(handle->model, vr->type), NULL);
	utf8 = ticonv_varname_to_utf8(handle->model, path, vr->type);
	g_snprintf(update_->text, sizeof(update_->text), _("Deleting %s..."), utf8);
	g_free(utf8);
	update_label();

	err = nsp_cmd_s_del_file(handle, path);
	g_free(path);
	if (err)
	{
		return err;
	}
	TRYF(nsp_cmd_r_del_file(handle));

	TRYF(nsp_session_close(handle));

	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	char *utf8;
	char *path;
	int err;

	TRYF(nsp_session_open(handle, SID_FILE_MGMT));

	path = g_strconcat("/", vr->folder, NULL);
	utf8 = ticonv_varname_to_utf8(handle->model, path, -1);
	g_snprintf(update_->text, sizeof(update_->text), _("Creating %s..."), utf8);
	g_free(utf8);
	update_label();

	err = nsp_cmd_s_new_folder(handle, path);
	g_free(path);
	if (err)
	{
		return err;
	}
	TRYF(nsp_cmd_r_new_folder(handle));

	TRYF(nsp_session_close(handle));

	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	uint32_t size;
	uint8_t cmd, *data;
	//int i;

	TRYF(nsp_session_open(handle, SID_DEV_INFOS));

	TRYF(nsp_cmd_s_dev_infos(handle, CMD_DI_MODEL));
	TRYF(nsp_cmd_r_dev_infos(handle, &cmd, &size, &data));

	strcpy(infos->product_name, (char*)data);
	infos->mask |= INFOS_PRODUCT_NAME;

	TRYF(nsp_cmd_s_dev_infos(handle, CMD_DI_VERSION));
	TRYF(nsp_cmd_r_dev_infos(handle, &cmd, &size, &data));

	infos->model = CALC_NSPIRE;

	//i = 0;
	//infos->flash_free = GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	infos->flash_free = (  (((uint64_t)data[ 0]) << 56)
	                     | (((uint64_t)data[ 1]) << 48)
	                     | (((uint64_t)data[ 2]) << 40)
	                     | (((uint64_t)data[ 3]) << 32)
	                     | (((uint64_t)data[ 4]) << 24)
	                     | (((uint64_t)data[ 5]) << 16)
	                     | (((uint64_t)data[ 6]) <<  8)
	                     | (((uint64_t)data[ 7])      ));
	infos->mask |= INFOS_FLASH_FREE;

	//i = 8;
	//infos->flash_phys = GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	infos->flash_phys = (  (((uint64_t)data[ 8]) << 56)
	                     | (((uint64_t)data[ 9]) << 48)
	                     | (((uint64_t)data[10]) << 40)
	                     | (((uint64_t)data[11]) << 32)
	                     | (((uint64_t)data[12]) << 24)
	                     | (((uint64_t)data[13]) << 16)
	                     | (((uint64_t)data[14]) <<  8)
	                     | (((uint64_t)data[15])      ));
	infos->mask |= INFOS_FLASH_PHYS;

	//i = 16;
	//infos->ram_free = GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	infos->ram_free = (  (((uint64_t)data[16]) << 56)
	                   | (((uint64_t)data[17]) << 48)
	                   | (((uint64_t)data[18]) << 40)
	                   | (((uint64_t)data[19]) << 32)
	                   | (((uint64_t)data[20]) << 24)
	                   | (((uint64_t)data[21]) << 16)
	                   | (((uint64_t)data[22]) <<  8)
	                   | (((uint64_t)data[23])      ));
	infos->mask |= INFOS_RAM_FREE;

	//i = 24;
	//infos->ram_phys = GUINT64_FROM_BE(*((uint64_t *)(data + i)));
	infos->ram_phys = (  (((uint64_t)data[24]) << 56)
	                   | (((uint64_t)data[25]) << 48)
	                   | (((uint64_t)data[26]) << 40)
	                   | (((uint64_t)data[27]) << 32)
	                   | (((uint64_t)data[28]) << 24)
	                   | (((uint64_t)data[29]) << 16)
	                   | (((uint64_t)data[30]) <<  8)
	                   | (((uint64_t)data[31])      ));
	infos->mask |= INFOS_RAM_PHYS;

	//i = 32;
	infos->battery = (data[32] == 0x01) ? 0 : 1;
	infos->mask |= INFOS_BATTERY;

	//i = 35;
	infos->clock_speed = data[35];
	infos->mask |= INFOS_CLOCK_SPEED;

	//i = 36;
	g_snprintf(infos->os_version, sizeof(infos->os_version), "%1i.%1i.%04i",
		data[36], data[37], (data[38] << 8) | data[39]);
	infos->mask |= INFOS_OS_VERSION;

	//i = 40;
	g_snprintf(infos->boot_version, sizeof(infos->boot_version), "%1i.%1i.%04i",
		data[40], data[41], (data[42] << 8) | data[43]);
	infos->mask |= INFOS_BOOT_VERSION;

	//i = 44;
	g_snprintf(infos->boot2_version, sizeof(infos->boot2_version), "%1i.%1i.%04i",
		data[44], data[45], (data[46] << 8) | data[47]);
	infos->mask |= INFOS_BOOT2_VERSION;

	//i = 48;
	//infos->hw_version = GUINT32_FROM_BE(*((uint32_t *)(data + i)));
	infos->hw_version = (  (((uint32_t)data[48]) << 24)
	                     | (((uint32_t)data[49]) << 16)
	                     | (((uint32_t)data[50]) <<  8)
	                     | (((uint32_t)data[51])      ));
	infos->mask |= INFOS_HW_VERSION;

	//i = 52;
	//infos->run_level = (uint8_t)GUINT16_FROM_BE(*((uint16_t *)(data + i)));
	infos->run_level = data[53];
	infos->mask |= INFOS_RUN_LEVEL;

	//i = 58;
	//infos->lcd_width = GUINT16_FROM_BE(*((uint16_t *)(data + i)));
	infos->lcd_width = (  (((uint16_t)data[58]) << 8)
	                    | (((uint16_t)data[59])     ));
	infos->mask |= INFOS_LCD_WIDTH;

	//i = 60;
	//infos->lcd_height = GUINT16_FROM_BE(*((uint16_t *)(data + i)));
	infos->lcd_height = (  (((uint16_t)data[60]) << 8)
	                     | (((uint16_t)data[61])     ));
	infos->mask |= INFOS_LCD_HEIGHT;

	//i = 62;
	infos->bits_per_pixel = data[62];
	infos->mask |= INFOS_BPP;

	//i = 64;
	infos->device_type = data[64];
	infos->mask |= INFOS_DEVICE_TYPE;

	memset(infos->main_calc_id, 0, sizeof(infos->main_calc_id));
	strncpy(infos->main_calc_id, (char*)(data + 82), 28);
	infos->mask |= INFOS_MAIN_CALC_ID;
	memset(infos->product_id, 0, sizeof(infos->product_id));
	strncpy(infos->product_id, (char*)(data + 82), 28);
	infos->mask |= INFOS_PRODUCT_ID;

	g_free(data);
	TRYF(nsp_session_close(handle));

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
	"Nspire",
	"Nspire handheld",
	N_("Nspire thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME | FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
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
	&rename_var,
	&change_attr
};
