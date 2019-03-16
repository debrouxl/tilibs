/* Hey EMACS -*- linux-c -*- */
/* $Id: calc_89t.c 3810 2007-09-25 19:14:30Z roms $ */

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
	Nspire support thru DirectUsb link.
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

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

// Helper function for multiple functions below.
static gchar * build_path(CalcModel model, VarRequest * vr)
{
	const char * dot_if_any;
	const char * fext_if_any;
	gchar * path;

	// Don't add a dot if this file type is unknown or a folder.
	if (vr->type >= NSP_MAXTYPES || vr->type == NSP_DIR)
	{
		dot_if_any = "";
	}
	else
	{
		dot_if_any = ".";
	}

	// Don't add a file extension if this is a folder.
	if (vr->type == NSP_DIR)
	{
		fext_if_any = "";
	}
	else
	{
		fext_if_any = tifiles_vartype2fext(model, vr->type);
	}

	if (!strcmp(vr->folder, ""))
	{
		path = g_strconcat("/", vr->name, dot_if_any, fext_if_any, NULL);
	}
	else
	{
		path = g_strconcat("/", vr->folder, "/", vr->name, dot_if_any, fext_if_any, NULL);
	}

	return path;
}

// Helper macro for closing a session, but forwarding a prior error, if any.
#define DO_CLOSE_SESSION(handle) \
	do \
	{ \
		if (!ret) \
		{ \
			ret = nsp_session_close(handle); \
		} \
		else \
		{ \
			nsp_session_close(handle); \
		} \
	} while (0);


//int nsp_reset = 0;

/*
	How things behave depending on OS version...
	- 1.1: no login request
	- 1.2 & 1.3: hand-held request LOGIN connection three seconds after device reset
	- 1.4: login request + service disconnect
	
*/

static int		is_ready	(CalcHandle* handle)
{
	// If re-enabled, these ones should be moved to handle->priv.
	//static int rom_11 = 0;
	//static int rom_14 = 0;
	int ret;

	// XXX debrouxl forcing a full sequence makes all operations a LOT slower (especially on
	// older OS), but fixes the 100% reproducible loss of connection after a few "Status"
	// operations or a single "List" operation, on my S-0907A non-CAS Nspire.
	// Tested with OS 1.1.9253, 1.2.2398, 1.3.2407, 1.4.11653, 1.6.4379, 1.7.2471, 1.7.1.50.
	//
	// A better fix is needed in the mid- and long-term.

	// checking for OS version and LOGIN packet
	//if (!nsp_reset)
	do
	{
		static const char echostr[] = "ready";
		int old;
		uint32_t size;
		uint8_t *data;

		// XXX debrouxl moving those two calls above the 'if (!nsp_reset)' test fixes connection
		// loss, but linking with at least 1.7.1.50 does not work properly after that: at least
		// directory listing and screenshot don't do anything beyond the "Status" probe.
		ret = nsp_addr_request(handle);
		if (ret)
		{
			break;
		}
		ret = nsp_addr_assign(handle, NSP_DEV_ADDR);
		if (ret)
		{
			break;
		}

		// XXX when commenting this OS version detection code, sending many Status or Dirlist
		// requests in quick succession often triggers memory corruption (hangs, reboots,
		// a variable amount of black pixels on the screen) on (at least) Nspire (CAS) OS 1.7...
		ticalcs_info("  waiting for LOGIN request (OS >= 1.2 check)...");
		old = ticables_options_set_timeout(handle->cable, 40);	// 3s mini

		ret = nsp_cmd_r_login(handle);	// no call to nsp_send_nack(handle) because nack is managed in nsp_recv_data()

		ticables_options_set_timeout(handle->cable, old);

		if (ret)
		{
			ticalcs_info("OS = 1.1");
			//rom_11 = !0;

			ret = nsp_addr_request(handle);
			if (ret)
			{
				break;
			}
			ret = nsp_addr_assign(handle, NSP_DEV_ADDR);
			if (ret)
			{
				break;
			}
		}
		else
		{
			ret = nsp_recv_disconnect(handle);
			if (ret)
			{
				ticalcs_info("OS = 1.2 or 1.3");
				//rom_14 = 0;
			}
			else
			{
				ticalcs_info("OS = 1.4 or later");
				//rom_14 = !0;
			}
		}

		//nsp_reset = !0;

		// Now use ECHO packet as ready check

		ret = nsp_session_open(handle, NSP_SID_ECHO);
		if (!ret)
		{
			ret = nsp_cmd_s_echo(handle, sizeof(echostr), (uint8_t *)echostr);
			if (!ret)
			{
				ret = nsp_cmd_r_echo(handle, &size, &data);
				if (!ret)
				{
					g_free(data);
				}
			}

		}
	} while (0);

	return ret;
}

static int		send_key	(CalcHandle* handle, uint32_t key)
{
	int ret;

	ret = nsp_cmd_s_key(handle, key);

	return ret;
}

// Forward declaration
static int		get_version	(CalcHandle* handle, CalcInfos* infos);

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret;
	CalcInfos infos;

	// First of all, we have to identify the Nspire model.
	ret = get_version(handle, &infos);
	if (!ret)
	{
		if (infos.bits_per_pixel == 4)
		{
			// Nspire (CAS) Clickpad or Touchpad.
			sc->pixel_format = CALC_PIXFMT_GRAY_4;
		}
		else if (infos.bits_per_pixel == 16)
		{
			// Nspire (CAS) CX or CM.
			sc->pixel_format = CALC_PIXFMT_RGB_565_LE;
		}
		else
		{
			ticalcs_critical(_("Unknown calculator model with %d bpp\n"), infos.bits_per_pixel);
			return ERR_UNSUPPORTED; // THIS RETURNS !
		}

		// Do screenshot
		ret = nsp_session_open(handle, NSP_SID_SCREEN_RLE);
		if (!ret)
		{
			ret = nsp_cmd_s_screen_rle(handle, 0);
			if (!ret)
			{
				uint32_t size = 0;
				uint8_t cmd, *data;

				ret = nsp_cmd_r_screen_rle(handle, &cmd, &size, &data);
				if (!ret)
				{
					sc->width = sc->clipped_width = (((uint16_t)data[8]) << 8) | data[9];
					sc->height = sc->clipped_height = (((uint16_t)data[10]) << 8) | data[11];
					size = (  (((uint32_t)data[0]) << 24)
					        | (((uint32_t)data[1]) << 16)
					        | (((uint32_t)data[2]) <<  8)
					        | (((uint32_t)data[3])      ));
					g_free(data);

					if (sc->width > 320)
					{
						ticalcs_critical("%s: no calculator model known to this library has screens of width > 320 pixels", __FUNCTION__);
						ret = ERR_INVALID_PACKET;
					}
					else if (sc->height > 240)
					{
						ticalcs_critical("%s: no calculator model known to this library has screens of height > 240 pixels", __FUNCTION__);
						ret = ERR_INVALID_PACKET;
					}
					else if (size > 2 * sc->width * sc->height)
					{
						ticalcs_critical("%s: no calculator model known to this library uses more than 16 bpp", __FUNCTION__);
						ret = ERR_INVALID_PACKET;
					}
					else
					{
						ret = nsp_cmd_r_screen_rle(handle, &cmd, &size, &data);
						if (!ret)
						{
							uint32_t len = sc->width * sc->height * infos.bits_per_pixel / 8;
							uint8_t * dst = (uint8_t *)ticalcs_alloc_screen(len);
							if (dst != NULL)
							{
								ret = ticalcs_screen_nspire_rle_uncompress(sc->pixel_format, data, size, dst, len);
								if (!ret)
								{
									*bitmap = dst;
								}
								else
								{
									ticalcs_free_screen(dst);
								}
							}
							else
							{
								ret = ERR_MALLOC;
							}
							g_free(data);
						}
					}
				}
			}

			DO_CLOSE_SESSION(handle);
		}
	}

	return ret;
}

// Helper function for get_dirlist, it does the bulk of the work.
static int enumerate_folder(CalcHandle* handle, GNode** vars, const char * folder_name)
{
	int ret;

	ticalcs_info("enumerate_folder<%s>\n", folder_name);

	do
	{
		char varname[VARNAME_MAX];

		ret = nsp_cmd_s_dir_enum_init(handle, folder_name);
		if (ret)
		{
			break;
		}
		ret = nsp_cmd_r_dir_enum_init(handle);
		if (ret)
		{
			break;
		}

		for (;;)
		{
			VarEntry *fe;
			GNode *node;
			uint32_t varsize;
			uint8_t vartype;

			ret = nsp_cmd_s_dir_enum_next(handle);
			if (ret)
			{
				break;
			}
			ret = nsp_cmd_r_dir_enum_next(handle, varname, &varsize, &vartype);

			if (ret == ERR_EOT)
			{
				ret = 0;
				break;
			}
			else if (ret != 0)
			{
				break;
			}

			fe = tifiles_ve_create();

			ticalcs_strlcpy(fe->folder, folder_name + 1, sizeof(fe->folder)); // Skip leading /
			fe->size = varsize;
			fe->type = vartype;
			fe->attr = ATTRB_NONE;

			// We might have to remove some extensions.
			if (fe->type == NSP_TNS)
			{
				char * ext = tifiles_fext_get(varname);
				// Just a sanity check
				if (ext)
				{
					// Did the file name have any non-empty extension ?
					if (*ext)
					{
						// Then we can remove the exension.
						*(ext-1) = '\0';
					}
					// else there is no extension to remove.
				}
				// else there is no extension to remove.
			}
			// else don't remove the extension.
			ticalcs_strlcpy(fe->name, varname, sizeof(fe->name));

			node = dirlist_create_append_node(fe, vars);
			if (!node)
			{
				ret = ERR_MALLOC;
				break;
			}

			ticalcs_info(_("Name: %s | Type: %8s | Attr: %i  | Size: %08X"),
				fe->name,
				tifiles_vartype2string(handle->model, fe->type),
				fe->attr,
				fe->size);
		}

		while (!ret)
		{
			int i;

			ret = nsp_cmd_s_dir_enum_done(handle);
			if (ret)
			{
				break;
			}
			ret = nsp_cmd_r_dir_enum_done(handle);
			if (ret)
			{
				break;
			}

			// Enumerate elements of root folder.
			for (i = 0; i < (int)g_node_n_children(*vars); i++) 
			{
				char new_folder_name[FLDNAME_MAX + 4];
				const char * separator_if_any;
				GNode * folder = g_node_nth_child(*vars, i);
				uint8_t vartype = ((VarEntry *)(folder->data))->type;

				// Don't recurse into regular files (type 0, TNS or e.g. themes.csv on OS 3.0+).
				if (vartype == NSP_TNS)
				{
					ticalcs_info(_("Not enumerating documents in %s because it's not a folder\n"), ((VarEntry *)(folder->data))->name);
					continue;
				}

				// Prevent names from starting with "//".
				if (strcmp(folder_name, "/"))
				{
					separator_if_any = "/";
				}
				else
				{
					separator_if_any = "";
				}

				ticalcs_slprintf(new_folder_name, sizeof(new_folder_name), "%s%s%s", folder_name, separator_if_any, ((VarEntry *)(folder->data))->name);
				new_folder_name[FLDNAME_MAX] = 0;

				ticalcs_info(_("Directory listing in <%s>...\n"), new_folder_name);

				ret = enumerate_folder(handle, &folder, new_folder_name);
				if (ret)
				{
					break;
				}
			}
			break;
		}
	} while (0);

	return ret;
}

static int get_dirlist (CalcHandle* handle, GNode** vars, GNode** apps)
{
	GNode *root;
	int ret;

	ret = dirlist_init_trees(handle, vars, apps);
	if (ret)
	{
		return ret;
	}

	root = g_node_new(NULL);
	if (!root)
	{
		return ERR_MALLOC;
	}
	g_node_append(*apps, root);

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (!ret)
	{
		ret = nsp_cmd_s_dir_attributes(handle, "/");
		if (!ret)
		{
			ret = nsp_cmd_r_dir_attributes(handle, NULL, NULL, NULL);
			if (!ret)
			{
				ret = nsp_session_close(handle);
				if (!ret)
				{
					ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
					if (!ret)
					{
						ret = enumerate_folder(handle, vars, "/");

						// Fall through for closing session.
					}
				}
			}
		}

		DO_CLOSE_SESSION(handle);
	}

	return ret;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	int ret;

	ret = nsp_session_open(handle, NSP_SID_DEV_INFOS);
	if (!ret)
	{
		ret = nsp_cmd_s_dev_infos(handle, NSP_CMD_DI_VERSION);
		if (!ret)
		{
			uint32_t size;
			uint8_t cmd, *data;

			ret = nsp_cmd_r_dev_infos(handle, &cmd, &size, &data);
			if (!ret)
			{
				if (size >= 24)
				{
					*flash = (  (((uint32_t)data[4]) << 24)
					          | (((uint32_t)data[5]) << 16)
					          | (((uint32_t)data[6]) <<  8)
					          | (((uint32_t)data[7])      ));

					*ram = (  (((uint32_t)data[20]) << 24)
					        | (((uint32_t)data[21]) << 16)
					        | (((uint32_t)data[22]) <<  8)
					        | (((uint32_t)data[23])      ));
				}
				else
				{
					ret = ERR_INVALID_PACKET;
					*flash = 0;
					*ram = 0;
				}

				g_free(data);
			}
		}

		DO_CLOSE_SESSION(handle);
	}

	return ret;
}

static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	uint8_t status;
	gchar *path;
	int ret;
	VarEntry * entry;

	handle->updat->cnt2 = 0;
	handle->updat->max2 = 1;
	ticalcs_update_pbar(handle);

	entry = content->entries[0];

	if (!ticalcs_validate_varentry(entry))
	{
		ticalcs_critical("%s: skipping invalid content entry #0", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if (entry->action == ACT_SKIP)
	{
		return 0;
	}

	//if (!strlen(entry->folder))
	//{
	//	return ERR_ABORT;
	//}

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (ret)
	{
		return ret;
	}

	path = build_path(handle->model, entry);

	ticonv_varname_to_utf8_sn(handle->model, path, handle->updat->text, sizeof(handle->updat->text), entry->type);
	ticalcs_update_label(handle);

	ret = nsp_cmd_s_put_file(handle, path, entry->size);
	g_free(path);
	if (!ret)
	{
		ret = nsp_cmd_r_put_file(handle);
		if (!ret)
		{
			ret = nsp_cmd_s_file_contents(handle, entry->size, entry->data);
			if (!ret)
			{
				ret = nsp_cmd_r_status(handle, &status);
			}
		}
	}

	DO_CLOSE_SESSION(handle);

	return ret;
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	char *path;
	int ret;

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (ret)
	{
		return ret;
	}

	path = build_path(handle->model, vr);
	ticonv_varname_to_utf8_sn(handle->model, path, handle->updat->text, sizeof(handle->updat->text), vr->type);
	ticalcs_update_label(handle);

	ret = nsp_cmd_s_get_file(handle, path);
	g_free(path);
	if (!ret)
	{
		ret = nsp_cmd_r_get_file(handle, &(vr->size));
		if (!ret)
		{
			ret = nsp_cmd_s_file_ok(handle);
			if (!ret)
			{
				uint8_t *data = NULL;

				if (vr->size)
				{
					ret = nsp_cmd_r_file_contents(handle, &(vr->size), &data);
				}
				if (!ret)
				{
					ret = nsp_cmd_s_status(handle, NSP_ERR_OK);
					if (!ret)
					{
						VarEntry *ve;

						content->model = handle->model;
						tifiles_comment_set_single_sn(content->comment, sizeof(content->comment));
						content->num_entries = 1;

						content->entries = tifiles_ve_create_array(1);
						ve = content->entries[0] = tifiles_ve_create();
						memcpy(ve, vr, sizeof(VarEntry));

						ve->data = (uint8_t *)tifiles_ve_alloc_data(ve->size);
						if (data && ve->data)
						{
							memcpy(ve->data, data, ve->size);
						}
					}
					g_free(data);
				}
			}
		}
	}

	// Close session at the end.
	// XXX don't check the result of this call, to enable reception of variables from Nspires running OS >= 1.7.
	// Those versions send a martian packet:
	// * a src port never seen before in the conversation;
	// * an improper dest port;
	// * a 1-byte payload containing 02 (i.e. an invalid address for the next packet).
	// * .ack = 0x00 (instead of 0x0A).
	nsp_session_close(handle);

	return ret;
}

static int		send_os    (CalcHandle* handle, FlashContent* content)
{
	int ret;

	if (content == NULL)
	{
		return -1;
	}

	tifiles_hexdump(content->data_part + content->data_length - 16, 16);

	do
	{
		ret = nsp_session_open(handle, NSP_SID_OS_INSTALL);
		if (!ret)
		{
			uint8_t value;

			ret = nsp_cmd_s_os_install(handle, content->data_length);
			if (ret)
			{
				break;
			}
			ret = nsp_cmd_r_os_install(handle);
			if (!ret)
			{
				uint8_t status;
				ret = nsp_cmd_s_os_contents(handle, 253, content->data_part);
				if (ret)
				{
					break;
				}
				ret = nsp_cmd_r_status(handle, &status);
				if (ret)
				{
					break;
				}
				ret = nsp_cmd_s_os_contents(handle, content->data_length - 253, content->data_part + 253);
				if (ret)
				{
					break;
				}
			}

			handle->updat->cnt2 = 0;
			handle->updat->max2 = 100;
			ticalcs_update_pbar(handle);

			do
			{
				ret = nsp_cmd_r_progress(handle, &value);
				if (ret)
				{
					break;
				}

				handle->updat->cnt2 = value;
				ticalcs_update_pbar(handle);
			} while (value < 100);

			DO_CLOSE_SESSION(handle);
		}
	} while (0);

	return ret;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* id)
{
	int ret;

	ret = nsp_session_open(handle, NSP_SID_DEV_INFOS);
	if (!ret)
	{
		ret = nsp_cmd_s_dev_infos(handle, NSP_CMD_DI_VERSION);
		if (!ret)
		{
			uint32_t size;
			uint8_t cmd, *data;

			ret = nsp_cmd_r_dev_infos(handle, &cmd, &size, &data);
			if (!ret)
			{
				if (size >= 110)
				{
					strncpy((char *)id, (char*)(data + 82), 28);
					id[28] = 0;
				}
				else
				{
					ret = ERR_INVALID_PACKET;
					id[0] = 0;
				}
				g_free(data);
			}
		}

		DO_CLOSE_SESSION(handle);
	}

	return ret;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	(void)handle;
	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	int ret;
	FILE *f;

	ticalcs_info("FIXME: make ROM dumping work above OS 1.x, using the Fron method");

	f = fopen(filename, "wb");
	if (f == NULL)
	{
		return ERR_OPEN_FILE;
	}

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (!ret)
	{
		ret = nsp_cmd_s_get_file(handle, "../phoenix/install/TI-Nspire.tnc");
		if (!ret)
		{
			uint32_t varsize;

			ret = nsp_cmd_r_get_file(handle, &varsize);
			if (!ret)
			{
				ret = nsp_cmd_s_file_ok(handle);
				if (!ret)
				{
					uint8_t *data;

					ret = nsp_cmd_r_file_contents(handle, &varsize, &data);
					if (!ret)
					{
						ret = nsp_cmd_s_status(handle, NSP_ERR_OK);
						if (!ret)
						{
							if (fwrite(data, varsize, 1, f) < 1)
							{
								ret = ERR_SAVE_FILE;
							}
						}
						g_free(data);
					}
				}
			}
		}

		DO_CLOSE_SESSION(handle);
	}

	fclose(f);

	return ret;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	char *utf8;
	char *path;
	int ret;

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (ret)
	{
		return ret;
	}

	path = build_path(handle->model, vr);
	utf8 = ticonv_varname_to_utf8(handle->model, path, vr->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Deleting %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	ret = nsp_cmd_s_del_file(handle, path);
	g_free(path);
	if (!ret)
	{
		ret = nsp_cmd_r_del_file(handle);
	}

	DO_CLOSE_SESSION(handle);

	return ret;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	char *utf8;
	char *path;
	int ret;

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (ret)
	{
		return ret;
	}

	path = g_strconcat("/", vr->folder, NULL);
	utf8 = ticonv_varname_to_utf8(handle->model, path, -1);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Creating %s..."), utf8);
	ticonv_utf8_free(utf8);
	ticalcs_update_label(handle);

	ret = nsp_cmd_s_new_folder(handle, path);
	g_free(path);
	if (!ret)
	{
		ret = nsp_cmd_r_new_folder(handle);
	}

	DO_CLOSE_SESSION(handle);

	return ret;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	int ret;

	ret = nsp_session_open(handle, NSP_SID_DEV_INFOS);
	if (ret)
	{
		return ret;
	}

	do
	{
		uint32_t size;
		uint8_t cmd, *data;
		unsigned int infos_mask = 0;

		ret = nsp_cmd_s_dev_infos(handle, NSP_CMD_DI_MODEL);
		if (ret)
		{
			break;
		}
		ret = nsp_cmd_r_dev_infos(handle, &cmd, &size, &data);
		if (ret)
		{
			break;
		}

		ticalcs_strlcpy(infos->product_name, (char *)data, size < sizeof(infos->product_name) ? size + 1 : sizeof(infos->product_name));
		infos_mask = INFOS_PRODUCT_NAME;

		g_free(data);

		ret = nsp_cmd_s_dev_infos(handle, NSP_CMD_DI_VERSION);
		if (ret)
		{
			break;
		}
		ret = nsp_cmd_r_dev_infos(handle, &cmd, &size, &data);
		if (ret)
		{
			break;
		}

		if (size < 110)
		{
			ret = ERR_INVALID_PACKET;
			break;
		}

		infos->flash_free = (  (((uint64_t)data[ 0]) << 56)
		                     | (((uint64_t)data[ 1]) << 48)
		                     | (((uint64_t)data[ 2]) << 40)
		                     | (((uint64_t)data[ 3]) << 32)
		                     | (((uint64_t)data[ 4]) << 24)
		                     | (((uint64_t)data[ 5]) << 16)
		                     | (((uint64_t)data[ 6]) <<  8)
		                     | (((uint64_t)data[ 7])      ));
		infos_mask |= INFOS_FLASH_FREE;

		infos->flash_phys = (  (((uint64_t)data[ 8]) << 56)
		                     | (((uint64_t)data[ 9]) << 48)
		                     | (((uint64_t)data[10]) << 40)
		                     | (((uint64_t)data[11]) << 32)
		                     | (((uint64_t)data[12]) << 24)
		                     | (((uint64_t)data[13]) << 16)
		                     | (((uint64_t)data[14]) <<  8)
		                     | (((uint64_t)data[15])      ));
		infos_mask |= INFOS_FLASH_PHYS;

		infos->ram_free = (  (((uint64_t)data[16]) << 56)
		                   | (((uint64_t)data[17]) << 48)
		                   | (((uint64_t)data[18]) << 40)
		                   | (((uint64_t)data[19]) << 32)
		                   | (((uint64_t)data[20]) << 24)
		                   | (((uint64_t)data[21]) << 16)
		                   | (((uint64_t)data[22]) <<  8)
		                   | (((uint64_t)data[23])      ));
		infos_mask |= INFOS_RAM_FREE;

		infos->ram_phys = (  (((uint64_t)data[24]) << 56)
		                   | (((uint64_t)data[25]) << 48)
		                   | (((uint64_t)data[26]) << 40)
		                   | (((uint64_t)data[27]) << 32)
		                   | (((uint64_t)data[28]) << 24)
		                   | (((uint64_t)data[29]) << 16)
		                   | (((uint64_t)data[30]) <<  8)
		                   | (((uint64_t)data[31])      ));
		infos_mask |= INFOS_RAM_PHYS;

		infos->battery = (data[32] == 0x01) ? 0 : 1;
		infos_mask |= INFOS_BATTERY_ENOUGH;

		infos->clock_speed = data[35];
		infos_mask |= INFOS_CLOCK_SPEED;

		ticalcs_slprintf(infos->os_version, sizeof(infos->os_version), "%1i.%1i.%04i", data[36], data[37], (((int)data[38]) << 8) | data[39]);
		infos_mask |= INFOS_OS_VERSION;

		ticalcs_slprintf(infos->boot_version, sizeof(infos->boot_version), "%1i.%1i.%04i", data[40], data[41], (((int)data[42]) << 8) | data[43]);
		infos_mask |= INFOS_BOOT_VERSION;

		ticalcs_slprintf(infos->boot2_version, sizeof(infos->boot2_version), "%1i.%1i.%04i", data[44], data[45], (((int)data[46]) << 8) | data[47]);
		infos_mask |= INFOS_BOOT2_VERSION;

		infos->hw_version = (  (((uint32_t)data[48]) << 24)
		                     | (((uint32_t)data[49]) << 16)
		                     | (((uint32_t)data[50]) <<  8)
		                     | (((uint32_t)data[51])      ));
		infos_mask |= INFOS_HW_VERSION;

		infos->run_level = data[53];
		infos_mask |= INFOS_RUN_LEVEL;

		infos->lcd_width = (  (((uint16_t)data[58]) << 8)
		                    | (((uint16_t)data[59])     ));
		infos_mask |= INFOS_LCD_WIDTH;

		infos->lcd_height = (  (((uint16_t)data[60]) << 8)
		                     | (((uint16_t)data[61])     ));
		infos_mask |= INFOS_LCD_HEIGHT;

		infos->bits_per_pixel = data[62];
		infos_mask |= INFOS_BPP;

		infos->device_type = data[64];
		infos_mask |= INFOS_DEVICE_TYPE;

		memset(infos->main_calc_id, 0, sizeof(infos->main_calc_id));
		strncpy(infos->main_calc_id, (char*)(data + 82), 28);
		infos_mask |= INFOS_MAIN_CALC_ID;
		memset(infos->product_id, 0, sizeof(infos->product_id));
		strncpy(infos->product_id, (char*)(data + 82), 28);
		infos_mask |= INFOS_PRODUCT_ID;

		infos->model = CALC_NONE;
		if (infos->product_id[0] == '0')
		{
			if (infos->product_id[1] == 'C')
			{
				infos->model = CALC_NSPIRE_TOUCHPAD_CAS; // Could also be CALC_NSPIRE_CLICKPAD_CAS.
			}
			else if (infos->product_id[1] == 'D')
			{
				infos->model = CALC_NSPIRE_CRADLE;
			}
			else if (infos->product_id[1] == 'E')
			{
				infos->model = CALC_NSPIRE_TOUCHPAD; // Could also be CALC_NSPIRE_CLICKPAD.
			}
			else if (infos->product_id[1] == 'F')
			{
				infos->model = CALC_NSPIRE_CX_CAS;
			}
		}
		else if (infos->product_id[1] == '1')
		{
			if (infos->product_id[1] == '0')
			{
				infos->model = CALC_NSPIRE_CX;
			}
			else if (infos->product_id[1] == '1')
			{
				infos->model = CALC_NSPIRE_CMC_CAS;
			}
			else if (infos->product_id[1] == '2')
			{
				infos->model = CALC_NSPIRE_CMC;
			}
			else if (infos->product_id[1] == 'C')
			{
				infos->model = CALC_NSPIRE_CXII_CAS;
			}
			else if (infos->product_id[1] == 'D')
			{
				infos->model = CALC_NSPIRE_CXII;
			}
			else if (infos->product_id[1] == 'E')
			{
				infos->model = CALC_NSPIRE_CXIIT;
			}
		}

		if (infos->model == CALC_NONE)
		{
			ticalcs_warning("Unknown Nspire product ID %c%c - setting generic model", infos->product_id[0], infos->product_id[1]);
			infos->model = CALC_NSPIRE;
		}

		infos->mask = (InfosMask)infos_mask;

		g_free(data);
	} while (0);

	DO_CLOSE_SESSION(handle);

	return ret;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	char *utf81, *utf82;
	char *path1, *path2;
	int ret;

	ret = nsp_session_open(handle, NSP_SID_FILE_MGMT);
	if (ret)
	{
		return ret;
	}

	path1 = build_path(handle->model, oldname);
	path2 = build_path(handle->model, newname);
	utf81 = ticonv_varname_to_utf8(handle->model, path1, oldname->type);
	utf82 = ticonv_varname_to_utf8(handle->model, path2, newname->type);
	ticalcs_slprintf(handle->updat->text, sizeof(handle->updat->text), _("Renaming %s to %s..."), utf81, utf82);
	ticonv_utf8_free(utf82);
	ticonv_utf8_free(utf81);
	ticalcs_update_label(handle);

	ret = nsp_cmd_s_rename_file(handle, path1, path2);
	g_free(path2);
	g_free(path1);
	if (!ret)
	{
		ret = nsp_cmd_r_rename_file(handle);
	}

	DO_CLOSE_SESSION(handle);

	return ret;
}

#define CALC_NSP_COMMON_COUNTERS \
{ \
	"",     /* is_ready */ \
	"",     /* send_key */ \
	"",     /* execute */ \
	"1P",   /* recv_screen */ \
	"1L",   /* get_dirlist */ \
	"",     /* get_memfree */ \
	"",     /* send_backup */ \
	"",     /* recv_backup */ \
	"2P1L", /* send_var */ \
	"1P1L", /* recv_var */ \
	"2P1L", /* send_var_ns */ \
	"1P1L", /* recv_var_ns */ \
	"2P1L", /* send_app */ \
	"2P1L", /* recv_app */ \
	"2P",   /* send_os */ \
	"1L",   /* recv_idlist */ \
	"2P",   /* dump_rom_1 */ \
	"2P",   /* dump_rom_2 */ \
	"",     /* set_clock */ \
	"",     /* get_clock */ \
	"1L",   /* del_var */ \
	"1L",   /* new_folder */ \
	"",     /* get_version */ \
	"1L",   /* send_cert */ \
	"1L",   /* recv_cert */ \
	"",     /* rename */ \
	"",     /* chattr */ \
	"2P1L", /* send_all_vars_backup */ \
	"2P1L"  /* recv_all_vars_backup */ \
}

#define CALC_NSP_COMMON_FPTRS \
{ \
	&is_ready, \
	&send_key, \
	&noop_execute, \
	&recv_screen, \
	&get_dirlist, \
	&get_memfree, \
	&noop_send_backup, \
	&noop_recv_backup, \
	&send_var, \
	&recv_var, \
	&noop_send_var_ns, \
	&noop_recv_var_ns, \
	&noop_send_flash, \
	&noop_recv_flash, \
	&send_os, \
	&recv_idlist, \
	&dump_rom_1, \
	&dump_rom_2, \
	&noop_set_clock, \
	&noop_get_clock, \
	&del_var, \
	&new_folder, \
	&get_version, \
	&noop_send_cert, \
	&noop_recv_cert, \
	&rename_var, \
	&noop_change_attr, \
	&noop_send_all_vars_backup, \
	&tixx_recv_all_vars_backup \
}

extern const CalcFncts calc_nsp = 
{
	CALC_NSPIRE,
	"Nspire (generic)",
	"Nspire handheld",
	N_("Nspire thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CAS, // FIXME the Nspire series spans multiple product IDs.
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cradle = 
{
	CALC_NSPIRE_CRADLE,
	"Nspire Cradle",
	"Nspire Cradle",
	N_("Nspire Cradle thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_LABCRADLE,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_clickpad = 
{
	CALC_NSPIRE_CLICKPAD,
	"Nspire Clickpad",
	"Nspire Clickpad handheld",
	N_("Nspire Clickpad thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_clickpad_cas = 
{
	CALC_NSPIRE_CLICKPAD_CAS,
	"Nspire Clickpad CAS",
	"Nspire Clickpad CAS handheld",
	N_("Nspire Clickpad CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CAS,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_touchpad = 
{
	CALC_NSPIRE_TOUCHPAD,
	"Nspire Touchpad",
	"Nspire Touchpad handheld",
	N_("Nspire Touchpad thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_touchpad_cas = 
{
	CALC_NSPIRE_TOUCHPAD_CAS,
	"Nspire Touchpad CAS",
	"Nspire Touchpad CAS handheld",
	N_("Nspire Touchpad CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CAS,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cx = 
{
	CALC_NSPIRE_CX,
	"Nspire CX",
	"Nspire CX handheld",
	N_("Nspire CX thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CX,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cx_cas = 
{
	CALC_NSPIRE_CX_CAS,
	"Nspire CX CAS",
	"Nspire CX CAS handheld",
	N_("Nspire CX CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CX_CAS,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cmc = 
{
	CALC_NSPIRE_CMC,
	"Nspire CM-C",
	"Nspire CM-C handheld",
	N_("Nspire CM-C thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CMC,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cmc_cas = 
{
	CALC_NSPIRE_CMC_CAS,
	"Nspire CM-C CAS",
	"Nspire CM-C CAS handheld",
	N_("Nspire CM-C CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CMC_CAS,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cxii = 
{
	CALC_NSPIRE_CXII,
	"Nspire CX II",
	"Nspire CX II handheld",
	N_("Nspire CX II thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CXII,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cxii_cas = 
{
	CALC_NSPIRE_CXII_CAS,
	"Nspire CX II CAS",
	"Nspire CX II CAS handheld",
	N_("Nspire CX II CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CXII_CAS,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cxiit = 
{
	CALC_NSPIRE_CXIIT,
	"Nspire CX II-T",
	"Nspire CX II-T handheld",
	N_("Nspire CX II-T thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CXIIT,
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};

extern const CalcFncts calc_nsp_cxiit_cas = 
{
	CALC_NSPIRE_CXIIT_CAS,
	"Nspire CX II-T CAS",
	"Nspire CX II-T CAS handheld",
	N_("Nspire CX II-T CAS thru DirectLink"),
	OPS_ISREADY | OPS_VERSION | OPS_SCREEN | OPS_IDLIST | OPS_DIRLIST | OPS_VARS | OPS_OS |
	OPS_ROMDUMP | OPS_NEWFLD | OPS_DELVAR | OPS_RENAME |
	FTS_SILENT | FTS_MEMFREE | FTS_FOLDER,
	PRODUCT_ID_NSPIRE_CXII_CAS, // The CX II CAS and the CX II-T CAS use the same product ID.
	CALC_NSP_COMMON_COUNTERS,
	CALC_NSP_COMMON_FPTRS
};
