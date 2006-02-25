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
  + folder (= NULL if TI8x, data = VarEntry if TI9x)
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
#if 0
	if(node)
	{
		//printf("<%p> ", node);
		if(node->data)
		{
			VarEntry* ve = node->data;

			//printf("<<%p>> ", ve);
			//printf("<%s>\n", tifiles_transcode_varname_static(CALC_TI84P, ve->name, ve->type));
		}
	}
#else
	//if (node)	if(node->data) tifiles_ve_delete(node->data);
#endif

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
		t_node_traverse(*tree, T_IN_ORDER, G_TRAVERSE_LEAVES, -1, free_varentry, NULL);
		t_node_destroy(*tree);
		*tree = NULL;
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
		utf8 = ticonv_varname_to_utf8(info->model, fe->name, fe->type);

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

	  utf8 = ticonv_varname_to_utf8(info->model, ve->name, ve->type);

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

	utf8 = ticonv_varname_to_utf8(info->model, ve->name, ve->type);

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

	if (tree == NULL)
		return 0;

	if (strcmp(((TreeInfo *)(vars->data))->type, VAR_NODE_NAME))
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
 * ticalcs_dirlist_merge:
 * @src: source tree.
 * @st: destination tree.
 *
 * Merge two trees into a single one ie dst = src + dst;
 *
 * Return value: destination tree (%dst pointer).
 **/
TIEXPORT TNode* TICALL ticalcs_dirlist_merge(TNode* src, TNode* dst)
{
	TreeInfo *src_info = (TreeInfo *)(src->data);
	TreeInfo *dst_info = (TreeInfo *)(dst->data);
	int i, j;
	int k, l;
	int found = 0;
	
	if (src == NULL || dst == NULL)
		return NULL;

	if (strcmp(src_info->type, dst_info->type))
		return NULL;

	// Parse folders
	for (i = 0; i < (int)t_node_n_children(src); i++)
	{
		TNode *parent_src = t_node_nth_child(src, i);
		VarEntry *fe_src = (VarEntry *) (parent_src->data);

		TNode *parent_dst;
		VarEntry *fe_dst;

		// and seach for existing folder in dst
		for (found = 0, k = 0; k < (int)t_node_n_children(dst); k++)
		{
			parent_dst = t_node_nth_child(dst, k);
			fe_dst = (VarEntry *) (parent_dst->data);

			if(!strcmp(fe_dst->name, fe_src->name))
			{
				found = !0;
				break;
			}
		}
		
		// folder doesn't exist ? => create !
		if(!found)
		{	
			fe_dst = tifiles_ve_create();
			strcpy(fe_dst->name, fe_src->name);
			fe_dst->type = TI89_DIR;

			parent_dst = t_node_new(fe_dst);
			t_node_append(dst, parent_dst);
		}

		// next, add variables bneath this folder
		
		if (fe_src != NULL) 
		{
			for (j = 0; j < (int)t_node_n_children(parent_src); j++)	//parse variables
			{
				TNode *child_src = t_node_nth_child(parent_src, j);
				VarEntry *ve_src = (VarEntry *) (child_src->data);

				TNode *child_dst;
				VarEntry *ve_dst;

				ve_dst = tifiles_ve_dup(ve_src);
				child_dst = t_node_new(ve_dst);
				t_node_append(parent_dst, child_dst);
			}
			fe_dst->size = j;
		}
	}

	return NULL;
}
