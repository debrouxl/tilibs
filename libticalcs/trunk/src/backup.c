/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "cmd89.h"

int tixx_recv_backup(CalcHandle* handle, BackupContent* content)
{
	int i, j, k;
	int i_max, j_max;
	GNode *vars, *apps;
	int nvars, ivars = 0;
	int b = 0;
	FileContent **group;
	FileContent *single;

	if(handle == NULL) return ERR_INVALID_HANDLE;
	if (content == NULL)
	{
		ticalcs_critical("tixx_recv_backup: content is NULL");
		return -1;
	}

	// Do a directory list and check for something to backup
	TRYF(handle->calc->get_dirlist(handle, &vars, &apps));
	nvars = ticalcs_dirlist_ve_count(vars);
	if(!nvars)
		return ERR_NO_VARS;

	update_->cnt2 = update_->cnt3 = 0;
	update_->max2 = update_->max3 = nvars;
	update_->pbar();

	// Check whether the last folder is empty
	b = g_node_n_children(g_node_nth_child(vars, g_node_n_children(vars) - 1));
	PAUSE(100); // needed by TI84+/USB

	// Create a group file
	k = 0;
	group = tifiles_content_create_group(nvars);

	// Receive all vars except for FLASH apps
	i_max = g_node_n_children(vars);
	for(i = 0; i < i_max; i++) 
	{
		GNode *parent = g_node_nth_child(vars, i);

		j_max = g_node_n_children(parent);
		for(j = 0; j < j_max; j++) 
		{
			GNode *node = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (node->data);

			update_->cnt2 = update_->cnt3 = ++ivars;
			update_->pbar();

			// we need to group files !
			TRYF(handle->calc->is_ready(handle));
			group[k] = tifiles_content_create_regular(handle->model);
			TRYF(handle->calc->recv_var(handle, 0, group[k++], ve));
		}
	}

	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	tifiles_group_contents(group, &single);
	tifiles_content_delete_group(group);

	// Swap content and single because we have a pointer on an allocated content
	{
		FileContent* cnt = (FileContent *)content;

		memcpy(content, single, sizeof(FileContent));
		cnt->entries = single->entries;
		strcpy(cnt->comment, tifiles_comment_set_group());
	}

	return 0;
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
TIEXPORT3 int TICALL ticalcs_calc_send_tigroup(CalcHandle* handle, TigContent* content, TigMode mode)
{
	TigEntry **ptr;
	GNode *vars, *apps;
	int nvars = 0;
	int napps = 0;

	if(handle == NULL) return ERR_INVALID_HANDLE;
	if (content == NULL)
	{
		ticalcs_critical("ticalcs_calc_send_tigroup: content is NULL");
		return -1;
	}

	TRYF(handle->calc->get_dirlist(handle, &vars, &apps));

	if((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
		nvars = content->n_vars;
	if(mode & TIG_FLASH)
		napps = content->n_apps;

	update_->cnt3 = 0;
	update_->max3 = nvars + napps;
	update_->pbar();

	if((handle->model == CALC_TI89 || handle->model == CALC_TI92P ||
		handle->model == CALC_TI89T || handle->model == CALC_V200) && (mode & TIG_BACKUP))
	{
		// erase memory
		TRYF(ti89_send_VAR(handle, 0, TI89_BKUP, "main"));
		TRYF(ti89_recv_ACK(handle, NULL));

		TRYF(ti89_recv_CTS(handle));
		TRYF(ti89_send_ACK(handle));

		TRYF(ti89_send_EOT(handle));
		TRYF(ti89_recv_ACK(handle, NULL));
	}

	// Send vars
	if((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
	{
		for(ptr = content->var_entries; *ptr; ptr++)
		{
			TigEntry *te = *ptr;

			update_->cnt3++;
			update_->pbar();

			if((te->content.regular->entries[0]->attr == ATTRB_ARCHIVED) && !(mode & TIG_ARCHIVE))
				continue;
			if((te->content.regular->entries[0]->attr != ATTRB_ARCHIVED) && !(mode & TIG_RAM))
				continue;

			TRYF(handle->calc->send_var(handle, MODE_BACKUP, te->content.regular));
		}
	}

	TRYF(handle->calc->is_ready(handle));

	// Send apps
	if(mode & TIG_FLASH)
	{
		for(ptr = content->app_entries; *ptr; ptr++)
		{
			TigEntry *te = *ptr;
			VarEntry ve;

			update_->cnt3++;
			update_->pbar();

			// can't overwrite apps so check before sending app
			memset(&ve, 0, sizeof(VarEntry));
			strcpy(ve.name, te->content.flash->name);
			if(!ticalcs_dirlist_ve_exist(apps, &ve))
				TRYF(handle->calc->send_app(handle, te->content.flash));
		}
	}

	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	return 0;
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
TIEXPORT3 int TICALL ticalcs_calc_recv_tigroup(CalcHandle* handle, TigContent* content, TigMode mode)
{
	int i, j;
	int i_max, j_max;
	GNode *vars, *apps;
	int nvars = 0;
	int napps = 0;
	int b = 0;

	if(handle == NULL) return ERR_INVALID_HANDLE;
	if (content == NULL)
	{
		ticalcs_critical("ticalcs_calc_send_tigroup: content is NULL");
		return -1;
	}

	update_->cnt3 = 0;
	update_->pbar();

	// Do a directory list and check for something to backup
	TRYF(handle->calc->get_dirlist(handle, &vars, &apps));
	if((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
		nvars = ticalcs_dirlist_ve_count(vars);
	if(mode & TIG_FLASH)
		napps = ticalcs_dirlist_ve_count(apps);

	update_->cnt3 = 0;
	update_->max3 = nvars + napps;
	update_->pbar();

	if(!nvars && !napps)
		return ERR_NO_VARS;

	// Check whether the last folder is empty
	b = g_node_n_children(g_node_nth_child(vars, g_node_n_children(vars) - 1));
	PAUSE(100); // needed by TI84+/USB

	// Receive all vars
	i_max = g_node_n_children(vars);
	if((mode & TIG_RAM) || (mode & TIG_ARCHIVE))
	for(i = 0; i < i_max; i++) 
	{
		GNode *parent = g_node_nth_child(vars, i);

		j_max = g_node_n_children(parent);
		for(j = 0; j < j_max; j++) 
		{
			GNode *node = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (node->data);
			TigEntry *te;
			char *filename;
			char *varname;
			char *fldname;

			PAUSE(100);
			TRYF(handle->calc->is_ready(handle));
			PAUSE(100);

			update_->cnt3++;
			update_->pbar();

			if(((mode & TIG_ARCHIVE) && (ve->attr == ATTRB_ARCHIVED)) ||
			   ((mode & TIG_RAM) && ve->attr != ATTRB_ARCHIVED))
			{
				fldname = ticonv_varname_to_filename(handle->model, ve->folder, -1);
				varname = ticonv_varname_to_filename(handle->model, ve->name, ve->type);
				if(handle->calc->features & FTS_FOLDER)
					filename = g_strconcat(fldname, ".", varname, ".", 
						tifiles_vartype2fext(handle->model, ve->type), NULL);
				else
					filename = g_strconcat(varname, ".", 
						tifiles_vartype2fext(handle->model, ve->type), NULL);
				g_free(fldname);
				g_free(varname);

				te = tifiles_te_create(filename, TIFILE_SINGLE, handle->model);
				g_free(filename);

				TRYF(handle->calc->recv_var(handle, 0, te->content.regular, ve));
				tifiles_content_add_te(content, te);
			}
		}
	}
	ticalcs_dirlist_destroy(&vars);

	// Receive all apps
	i_max = g_node_n_children(apps);
	if(mode & TIG_FLASH)
	for(i = 0; i < i_max; i++) 
	{
		GNode *parent = g_node_nth_child(apps, i);

		j_max = g_node_n_children(parent);
		for(j = 0; j < j_max; j++) 
		{
			GNode *node = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (node->data);
			TigEntry *te;
			char *filename;
			char *basename;

			TRYF(handle->calc->is_ready(handle));

			update_->cnt3++;
			update_->pbar();

			basename = ticonv_varname_to_filename(handle->model, ve->name, ve->type);
			filename = g_strconcat(basename, ".", tifiles_vartype2fext(handle->model, ve->type), NULL);
			g_free(basename);

			te = tifiles_te_create(filename, TIFILE_FLASH, handle->model);
			g_free(filename);

			TRYF(handle->calc->recv_app(handle, te->content.flash, ve));
			tifiles_content_add_te(content, te);
		}
	}	
	ticalcs_dirlist_destroy(&apps);

	return 0;
}
