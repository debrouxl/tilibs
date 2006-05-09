/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Utility functions for directory list (tree mangement).
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticonv.h"
#include "gettext.h"
#include "ticalcs.h"
#include "logging.h"

/* Dirlist format:

- Vars:

  top (data = TreeInfo)
  |
  + folder (data = NULL if TI8x, data = VarEntry if TI9x)
	  |
	  +- var1 (data = VarEntry)
	  +- var2 (data = VarEntry)

- Apps:

  top (data = TreeInfo)
  |
  +- app1 (data = VarEntry)
  +- app2 (data = VarEntry)

*/


static tboolean free_varentry(TNode* node, tpointer data)
{
	if(node)
	{
		if(node->data)
		{
			VarEntry* ve = node->data;

			//printf("<%p> [%s] [%s]", ve, ve->folder, ve->name);
			tifiles_ve_delete(ve);
		}
	}

	return FALSE;
}

/**
 * ticalcs_dirlist_destroy:
 * @tree: the tree to destroy (var or app).
 *
 * Destroy the whole tree create by #ticalcs_calc_get_dirlist.
 *
 * Return value: none.
 **/
TIEXPORT void TICALL ticalcs_dirlist_destroy(TNode** tree)
{
	if (*tree != NULL) 
	{
		if((*tree)->children != NULL)
		{
			t_node_traverse(*tree, T_IN_ORDER, G_TRAVERSE_LEAVES, -1, free_varentry, NULL);

			free((*tree)->data);
			t_node_destroy(*tree);

			*tree = NULL;
		}
	}
}

static void dirlist_display_vars(TNode* tree)
{
  TNode *vars = tree;
  TreeInfo *info = (TreeInfo *)(tree->data);
  int i, j, k;
  char *utf8;

  printf(  "+------------------+----------+----+----+----------+----------+\n");
  printf(_("| B. name          | T. name  |Attr|Type| Size     | Folder   |\n"));
  printf(  "+------------------+----------+----+----+----------+----------+\n");

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);
    VarEntry *fe = (VarEntry *) (parent->data);

    if (fe != NULL) 
	{
		utf8 = ticonv_varname_to_utf8(info->model, fe->name);

      printf("| ");
      for (k = 0; k < 8; k++)
		printf("%02X", (uint8_t) (fe->name)[k]);
      printf(" | ");	
      printf("%8s", utf8);
      printf(" | ");
      printf("%2i", fe->attr);
      printf(" | ");
      printf("%02X", fe->type);
      printf(" | ");
      printf("%08X", fe->size);
      printf(" | ");
      printf("%8s", fe->folder);
      printf(" |");
	  printf("\n");

	  g_free(utf8);
    }

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      TNode *child = t_node_nth_child(parent, j);
      VarEntry *ve = (VarEntry *) (child->data);

	  utf8 = ticonv_varname_to_utf8(info->model, ve->name);

      printf("| ");
      for (k = 0; k < 8; k++) 
		printf("%02X", (uint8_t) (ve->name)[k]);
      printf(" | ");
      printf("%8s", utf8);
      printf(" | ");
      printf("%2i", ve->attr);
      printf(" | ");
      printf("%02X", ve->type);
      printf(" | ");
      printf("%08X", ve->size);
      printf(" | ");
      printf("%8s", ve->folder);
      printf(" |");
	  printf("\n");

	  g_free(utf8);
    }
  }
  if (!i)
    printf(_("  No variables"));

  printf(_("+------------------+----------+----+----+----------+----------+"));
  printf("\n");
}

static void dirlist_display_apps(TNode* tree)
{
	TNode *apps = tree;
  TreeInfo *info = (TreeInfo *)(tree->data);
  int i, k;
  char *utf8;

  printf(  "+------------------+----------+----+----+----------+\n");
  printf(_("| B. name          | T. name  |Attr|Type| Size     |\n"));
  printf(  "+------------------+----------+----+----+----------+\n");

  for (i = 0; i < (int)t_node_n_children(apps); i++) 
  {
    TNode *child = t_node_nth_child(apps, i);
    VarEntry *ve = (VarEntry *) (child->data);

	utf8 = ticonv_varname_to_utf8(info->model, ve->name);

    printf("| ");
    for (k = 0; k < 8; k++)
      printf("%02X", (uint8_t) (ve->name)[k]);
    printf(" | ");
    printf("%8s", utf8);
    printf(" | ");
    printf("%2i", ve->attr);
    printf(" | ");
    printf("%02X", ve->type);
    printf(" | ");
    printf("%08X", ve->size);
    printf(" |");
	printf("\n");

	g_free(utf8);
  }
  if (!i)
  {
	printf(_("+ No applications  |          |    |    |          +"));
	printf("\n");
  }

  printf("+------------------+----------+----+----+----------+");
  printf("\n");
}

/**
 * ticalcs_dirlist_display:
 * @tree: the tree to display (var or app).
 *
 * Display to stdout the tree content formatted in a tab.
 *
 * Return value: none.
 **/
TIEXPORT void TICALL ticalcs_dirlist_display(TNode* tree)
{
	TreeInfo *info = (TreeInfo *)(tree->data);
	char *node_name = info->type;
  
	if (tree == NULL)
		return;

    if (!strcmp(node_name, VAR_NODE_NAME))
		dirlist_display_vars(tree);
    else if (!strcmp(node_name, APP_NODE_NAME))
	    dirlist_display_apps(tree);
}

/**
 * ticalcs_dirlist_var_exist:
 * @tree: the tree to display (var or app).
 * @full_name: the full name of var to search for.
 *
 * Parse the tree for the given varname & folder.
 *
 * Return value: a pointer on the #VarEntry found or NULL if not found.
 **/
TIEXPORT VarEntry *TICALL ticalcs_dirlist_var_exist(TNode* tree, char *full_name)
{
	int i, j;
	TNode *vars = tree;
	char fldname[18];
	char varname[18];
	TreeInfo *info = (TreeInfo *)(tree->data);
	char *node_name = info->type;

	strcpy(fldname, tifiles_get_fldname(full_name));
	strcpy(varname, tifiles_get_varname(full_name));

	if (tree == NULL)
		return NULL;

	if (strcmp(node_name, VAR_NODE_NAME))
		return NULL;

	for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
	{
		TNode *parent = t_node_nth_child(vars, i);
		VarEntry *fe = (VarEntry *) (parent->data);

		if ((fe != NULL) && strcmp(fe->name, fldname))
			continue;

		for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
		{
			TNode *child = t_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (child->data);

			if (!strcmp(ve->name, varname))
				return ve;
		}
	}

	return NULL;
}

/**
 * ticalcs_dirlist_app_exist:
 * @tree: the tree to display (var or app).
 * @app_name: the name of app to search for.
 *
 * Parse the tree for the given application name.
 *
 * Return value: a pointer on the #VarEntry found or NULL if not found.
 **/
TIEXPORT VarEntry *TICALL ticalcs_dirlist_app_exist(TNode* tree, char *appname)
{
	int i;
	TNode *apps = tree;
	TreeInfo *info = (TreeInfo *)(apps->data);
	char *node_name = info->type;

	if (tree == NULL)
		return NULL;

	if (strcmp(node_name, APP_NODE_NAME))
		return NULL;

	for (i = 0; i < (int)t_node_n_children(apps); i++) 
	{
		TNode *child = t_node_nth_child(apps, i);
		VarEntry *ve = (VarEntry *) (child->data);

		if (!strcmp(ve->name, appname))
			return ve;
	}

	return NULL;
}

/**
 * ticalcs_dirlist_num_vars:
 * @tree: a tree (var or app).
 *
 * Count how many variables are listed in the tree.
 *
 * Return value: the number of variables.
 **/
TIEXPORT int TICALL ticalcs_dirlist_num_vars(TNode* tree)
{
	int i, j;
	TNode *vars = tree;
	int nvars = 0;
	TreeInfo *info = (TreeInfo *)(tree->data);
	char *node_name = info->type;

	if (tree == NULL)
		return 0;

	if (strcmp(node_name, VAR_NODE_NAME))
		return 0;

	for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
	{
		TNode *parent = t_node_nth_child(vars, i);

		for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
			nvars++;
	}

	return nvars;
}

/**
 * ticalcs_dirlist_mem_used:
 * @tree: a tree (var only).
 *
 * Count how much memory is used by variables listed in the tree.
 *
 * Return value: size of all variables in bytes.
 **/
TIEXPORT int TICALL ticalcs_dirlist_mem_used(TNode* tree)
{
	int i, j;
	TNode *vars = tree;
	uint32_t mem = 0;
	TreeInfo *info = (TreeInfo *)(tree->data);
	char *node_name = info->type;

	if (tree == NULL)
		return 0;

	if (strcmp(node_name, VAR_NODE_NAME))
		return 0;

	
	for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
	{
		TNode *parent = t_node_nth_child(vars, i);

		for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
		{
			TNode *child = t_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (child->data);

			mem += ve->size;
		}
	}

	return mem;
}

/**
 * ticalcs_dirlist_flash_used:
 * @tree: a tree (app only).
 *
 * Count how much memory is used by archived variables and apps listed in the trees.
 *
 * Return value: size of all FLASH in bytes.
 **/
TIEXPORT int TICALL ticalcs_dirlist_flash_used(TNode* vars, TNode* apps)
{
	int i, j;
	uint32_t mem = 0;
	TreeInfo *info1 = (TreeInfo *)(vars->data);
	TreeInfo *info2 = (TreeInfo *)(apps->data);
	char *node_name_1 = info1->type;
	char *node_name_2 = info2->type;

	if (!vars && !apps)
		return 0;

	if (!strcmp(node_name_1, VAR_NODE_NAME))
	{
		for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
		{
			TNode *parent = t_node_nth_child(vars, i);

			for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
			{
				TNode *child = t_node_nth_child(parent, j);
				VarEntry *ve = (VarEntry *) (child->data);

				if(ve->attr == ATTRB_ARCHIVED)
					mem += ve->size;
			}
		}
	}

	if (!strcmp(node_name_2, APP_NODE_NAME))
	{
		for (i = 0; i < (int)t_node_n_children(apps); i++) 
		{
			TNode *child = t_node_nth_child(apps, i);
			VarEntry *ve = (VarEntry *) (child->data);

			mem += ve->size;
		}
	}

	return mem;
}

/**
 * ticalcs_dirlist_add_entry:
 * @tree: source tree.
 * @entry: entry to add.
 *
 * Add an entry into the main tree (if it doesn't exist yet).
 *
 * Return value: the tree (%dst pointer).
 **/
TIEXPORT TNode* TICALL ticalcs_dirlist_entry_add(TNode* tree, VarEntry *entry)
{
	TreeInfo *info = (TreeInfo *)(tree->data);
	int i, j;
	int found = 0;

	TNode *parent = NULL;
	VarEntry *fe = NULL;

	TNode *child;
	VarEntry *ve;

	char *folder;
	
	if (tree == NULL)
		return NULL;

	if (strcmp(info->type, VAR_NODE_NAME))
		return NULL;

	if(!strcmp(entry->folder, "") && tifiles_has_folder(info->model))
		folder = "main";
	else
		folder = entry->folder;

	// Parse folders
	for (found = 0, i = 0; i < (int)t_node_n_children(tree); i++)
	{
		parent = t_node_nth_child(tree, i);
		fe = (VarEntry *) (parent->data);

		if(fe == NULL)
			break;

		if(!strcmp(fe->name, folder))
		{
			found = !0;
			break;
		}
	}
		
	// folder doesn't exist ? => create !
	if(!found && fe)
	{	
		fe = tifiles_ve_create();
		strcpy(fe->name, entry->folder);
		fe->type = TI89_DIR;

		parent = t_node_new(fe);
		t_node_append(tree, parent);
	}

	if(!strcmp(entry->name, ""))
		return tree;

	// next, add variables beneath this folder
	for(found = 0, j = 0; j < (int)t_node_n_children(parent); j++)
	{
		child = t_node_nth_child(parent, j);
		ve = (VarEntry *) (child->data);

		if(!strcmp(ve->name, entry->name))
		{
			found = !0;
			break;
		}
	}

	if(!found)
	{
		ve = tifiles_ve_dup(entry);
		child = t_node_new(ve);
		t_node_append(parent, child);
	}

	if(fe && found)
		fe->size++;

	return tree;
}

/**
 * ticalcs_dirlist_entry_del:
 * @tree: source tree.
 * @entry: entry to remove.
 *
 * Remove an entry into the main tree (if it doesn't exist yet).
 *
 * Return value: the tree (%dst pointer).
 **/
TIEXPORT TNode* TICALL ticalcs_dirlist_entry_del(TNode* tree, VarEntry *entry)
{
	TreeInfo *info = (TreeInfo *)(tree->data);
	int i, j;
	int found = 0;

	TNode *parent = NULL;
	VarEntry *fe = NULL;

	TNode *child;
	VarEntry *ve;

	char *folder;
	
	if (tree == NULL)
		return NULL;

	if (strcmp(info->type, VAR_NODE_NAME))
		return NULL;

	if(!strcmp(entry->folder, "") && tifiles_has_folder(info->model))
		folder = "main";
	else
		folder = entry->folder;

	// Parse folders
	for (found = 0, i = 0; i < (int)t_node_n_children(tree); i++)
	{
		parent = t_node_nth_child(tree, i);
		fe = (VarEntry *) (parent->data);

		if(fe == NULL)
			break;

		if(!strcmp(fe->name, folder))
		{
			found = !0;
			break;
		}
	}

	if(!found && fe)
		return NULL;
		
	// next, delete variables beneath this folder
	for(found = 0, j = 0; j < (int)t_node_n_children(parent); j++)
	{
		child = t_node_nth_child(parent, j);
		ve = (VarEntry *) (child->data);

		if(!strcmp(ve->name, entry->name))
		{
			found = !0;
			break;
		}
	}

	if(found)
	{
		tifiles_ve_delete(child->data);
		t_node_destroy(child);
	}

	if(fe && found)
		fe->size--;

	return tree;
}
