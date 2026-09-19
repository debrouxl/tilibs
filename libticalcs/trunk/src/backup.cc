/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

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

/*
	Pseudo-backup support (common).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "cmd68k.h"

int tixx_recv_all_vars_backup(CalcHandle* handle, FileContent* content)
{
	int i, j, k;
	int i_max;
	GNode *vars, *apps;
	int nvars, ivars = 0;
	int b = 0;
	FileContent **group;
	FileContent *single;
	int ret;

	VALIDATE_HANDLE(handle);
	if (content == NULL)
	{
		ticalcs_critical("tixx_recv_backup: content is NULL");
		return -1;
	}
	VALIDATE_CALCFNCTS(handle->calc);

	// Do a directory list and check for something to backup
	ret = handle->calc->fncts.get_dirlist(handle, &vars, &apps);
	if (ret)
	{
		return ret;
	}
	nvars = ticalcs_dirlist_ve_count(vars);
	if (!nvars)
	{
		return ERR_NO_VARS;
	}

	handle->updat->cnt2 = handle->updat->cnt3 = 0;
	handle->updat->max2 = handle->updat->max3 = nvars;
	ticalcs_update_pbar(handle);

	// Check whether the last folder is empty
	b = g_node_n_children(g_node_nth_child(vars, g_node_n_children(vars) - 1));
	PAUSE(100); // needed by TI84+/USB

	// Create a group file
	k = 0;
	group = tifiles_content_create_group(nvars);

	// Receive all vars except for FLASH apps
	i_max = g_node_n_children(vars);
	for (i = 0; i < i_max; i++)
	{
		GNode *parent = g_node_nth_child(vars, i);

		int j_max = g_node_n_children(parent);
		for (j = 0; j < j_max; j++)
		{
			GNode *node = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (node->data);

			handle->updat->cnt2 = handle->updat->cnt3 = ++ivars;
			ticalcs_update_pbar(handle);

			// we need to group files !
			ret = handle->calc->fncts.is_ready(handle);
			if (ret)
			{
				goto end;
			}
			group[k] = tifiles_content_create_regular(handle->model);
			ret = handle->calc->fncts.recv_var(handle, MODE_NORMAL, group[k++], ve);
			if (ret)
			{
				goto end;
			}
		}
	}

end:
	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	if (!ret)
	{
		FileContent * cnt;
		ret = tifiles_group_contents(group, &cnt);
		if (!ret)
		{
			cnt->model = content->model;

			// Steal contents of cnt, then clean up.
			memcpy(content, cnt, sizeof(*content));
			cnt->num_entries = 0;
			cnt->entries = NULL;
			tifiles_content_delete_regular(cnt);

			tifiles_comment_set_group_sn(content->comment, sizeof(content->comment));
		}
	}

	tifiles_content_delete_group(group);

	return ret;
}

/*
	Pseudo full-backup support (common).
*/

/**
 * ticalcs_calc_send_tigroup:
 * @handle: a previously allocated handle
 * @filename: name of file
 * @mode: which vars/apps to send
 *
 * Send a TiGroup file.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
int TICALL ticalcs_calc_send_tigroup(CalcHandle* handle, TigContent* content, TigMode mode)
{
	TigEntry **ptr;
	GNode *vars, *apps;
	int nvars = 0;
	int napps = 0;
	int ret;

	VALIDATE_HANDLE(handle);
	if (content == NULL)
	{
		ticalcs_critical("ticalcs_calc_send_tigroup: content is NULL");
		return -1;
	}
	VALIDATE_CALCFNCTS(handle->calc);

	ret = handle->calc->fncts.get_dirlist(handle, &vars, &apps);
	if (ret)
	{
		return ret;
	}

	if ((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
	{
		nvars = content->n_vars;
	}
	if (mode & TIG_FLASH)
	{
		napps = content->n_apps;
	}

	handle->updat->cnt3 = 0;
	handle->updat->max3 = nvars + napps;
	ticalcs_update_pbar(handle);

	if ((handle->model == CALC_TI89 || handle->model == CALC_TI92P ||
		handle->model == CALC_TI89T || handle->model == CALC_V200) && (mode & TIG_BACKUP))
	{
		// erase memory
		ret = ti89_send_VAR(handle, 0, TI89_BKUP, "main");
		if (!ret)
		{
			ret = ti89_recv_ACK(handle, NULL);
			if (!ret)
			{
				ret = ti89_recv_CTS(handle);
				if (!ret)
				{
					ret = ti89_send_ACK(handle);
					if (!ret)
					{
						ret = ti89_send_EOT(handle);
						if (!ret)
						{
							ret = ti89_recv_ACK(handle, NULL);
						}
					}
				}
			}
		}
	}

	if (!ret)
	{
		// Send vars
		if ((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
		{
			for (ptr = content->var_entries; *ptr; ptr++)
			{
				TigEntry *te = *ptr;

				handle->updat->cnt3++;
				ticalcs_update_pbar(handle);

				if ((te->content.regular->entries[0]->attr == ATTRB_ARCHIVED) && !(mode & TIG_ARCHIVE))
				{
					continue;
				}
				if ((te->content.regular->entries[0]->attr != ATTRB_ARCHIVED) && !(mode & TIG_RAM))
				{
					continue;
				}

				ret = handle->calc->fncts.send_var(handle, MODE_BACKUP, te->content.regular);
				if (ret)
				{
					break;
				}
			}
		}

		if (!ret)
		{
			ret = handle->calc->fncts.is_ready(handle);
			if (!ret)
			{
				// Send apps
				if (mode & TIG_FLASH)
				{
					for (ptr = content->app_entries; *ptr; ptr++)
					{
						TigEntry *te = *ptr;
						VarEntry ve;

						handle->updat->cnt3++;
						ticalcs_update_pbar(handle);

						// can't overwrite apps so check before sending app
						memset(&ve, 0, sizeof(VarEntry));
						ticalcs_strlcpy(ve.name, te->content.flash->name, sizeof(ve.name));
						if (!ticalcs_dirlist_ve_exist(apps, &ve))
						{
							ret = handle->calc->fncts.send_app(handle, te->content.flash);
							if (ret)
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	return ret;
}

/**
 * ticalcs_calc_recv_tigroup:
 * @handle: a previously allocated handle
 * @filename: name of file
 * @mode: which vars/apps to receive
 *
 * Receive a TiGroup file.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
int TICALL ticalcs_calc_recv_tigroup(CalcHandle* handle, TigContent* content, TigMode mode)
{
	int i, j;
	GNode *vars, *apps;
	int nvars = 0;
	int napps = 0;
	int b = 0;
	int ret;

	VALIDATE_HANDLE(handle);
	if (content == NULL)
	{
		ticalcs_critical("ticalcs_calc_send_tigroup: content is NULL");
		return -1;
	}
	VALIDATE_CALCFNCTS(handle->calc);

	handle->updat->cnt3 = 0;
	ticalcs_update_pbar(handle);

	// Do a directory list and check for something to backup
	ret = handle->calc->fncts.get_dirlist(handle, &vars, &apps);
	if (ret)
	{
		return ret;
	}

	if ((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
	{
		nvars = ticalcs_dirlist_ve_count(vars);
	}
	if (mode & TIG_FLASH)
	{
		napps = ticalcs_dirlist_ve_count(apps);
	}

	handle->updat->cnt3 = 0;
	handle->updat->max3 = nvars + napps;
	ticalcs_update_pbar(handle);

	if (!nvars && !napps)
	{
		ret = ERR_NO_VARS; // THIS RETURNS !
		goto end;
	}

	// Check whether the last folder is empty
	b = g_node_n_children(g_node_nth_child(vars, g_node_n_children(vars) - 1));
	PAUSE(100); // needed by TI84+/USB

	// Receive all vars
	if ((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
	{
		int i_max = g_node_n_children(vars);
		for (i = 0; i < i_max; i++)
		{
			GNode *parent = g_node_nth_child(vars, i);

			int j_max = g_node_n_children(parent);
			for (j = 0; j < j_max; j++)
			{
				GNode *node = g_node_nth_child(parent, j);
				VarEntry *ve = (VarEntry *) (node->data);
				TigEntry *te;

				PAUSE(100);
				ret = handle->calc->fncts.is_ready(handle);
				if (ret)
				{
					goto end;
				}
				PAUSE(100);

				handle->updat->cnt3++;
				ticalcs_update_pbar(handle);

				if (((mode & TIG_ARCHIVE) && (ve->attr == ATTRB_ARCHIVED)) ||
				    ((mode & TIG_RAM) && ve->attr != ATTRB_ARCHIVED))
				{
					char *filename;
					char *varname = ticonv_varname_to_filename(handle->model, ve->name, ve->type);
					char *fldname = ticonv_varname_to_filename(handle->model, ve->folder, -1);
					if (handle->calc->features & FTS_FOLDER)
					{
						filename = g_strconcat(fldname, ".", varname, ".", tifiles_vartype2fext(handle->model, ve->type), NULL);
					}
					else
					{
						filename = g_strconcat(varname, ".", tifiles_vartype2fext(handle->model, ve->type), NULL);
					}
					g_free(varname);
					g_free(fldname);

					te = tifiles_te_create(filename, TIFILE_SINGLE, handle->model);
					g_free(filename);
					if (te != NULL)
					{
						ret = handle->calc->fncts.recv_var(handle, MODE_NORMAL, te->content.regular, ve);
						if (ret)
						{
							tifiles_te_delete(te);
							goto end;
						}
						tifiles_content_add_te(content, te);
					}
					else
					{
						ret = ERR_MALLOC;
						goto end;
					}
				}
			}
		}
	}

	// Receive all apps
	if (mode & TIG_FLASH)
	{
		int i_max = g_node_n_children(apps);
		for(i = 0; i < i_max; i++) 
		{
			GNode *parent = g_node_nth_child(apps, i);

			int j_max = g_node_n_children(parent);
			for (j = 0; j < j_max; j++) 
			{
				GNode *node = g_node_nth_child(parent, j);
				VarEntry *ve = (VarEntry *) (node->data);
				TigEntry *te;
				char *filename;
				char *basename;

				ret = handle->calc->fncts.is_ready(handle);
				if (ret)
				{
					goto end;
				}

				handle->updat->cnt3++;
				ticalcs_update_pbar(handle);

				basename = ticonv_varname_to_filename(handle->model, ve->name, ve->type);
				filename = g_strconcat(basename, ".", tifiles_vartype2fext(handle->model, ve->type), NULL);
				g_free(basename);

				te = tifiles_te_create(filename, TIFILE_FLASH, handle->model);
				g_free(filename);
				if (te != NULL)
				{
					ret = handle->calc->fncts.recv_app(handle, te->content.flash, ve);
					if (ret)
					{
						tifiles_te_delete(te);
						goto end;
					}
					tifiles_content_add_te(content, te);
				}
				else
				{
					ret = ERR_MALLOC;
					goto end;
				}
			}
		}
	}
end:
	ticalcs_dirlist_destroy(&apps);
	ticalcs_dirlist_destroy(&vars);

	return ret;
}
