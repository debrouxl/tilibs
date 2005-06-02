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

#include "gettext.h"
#include "ticalcs.h"
#include "logging.h"

static tboolean free_varentry(TNode* node, tpointer data)
{
	if (node)
		free(node->data);
	return FALSE;
}

TIEXPORT void TICALL ticalcs_dirlist_destroy(TNode** tree)
{
	if (*tree != NULL) 
	{
		t_node_traverse(*tree, T_IN_ORDER, T_TRAVERSE_ALL, -1, free_varentry, NULL);
		t_node_destroy(*tree);
		*tree = NULL;
	}
}

static void dirlist_display_vars(TNode* tree)
{
  int i, j, k;
  TNode *vars = tree;

  printf(  "+------------------+----------+----+----+----------+----------+\n");
  printf(_("| B. name          | T. name  |Attr|Type| Size     | Folder   |\n"));
  printf(  "+------------------+----------+----+----+----------+----------+\n");

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);
    VarEntry *fe = (VarEntry *) (parent->data);

    if (fe != NULL) 
	{
      printf("| ");
      for (k = 0; k < 8; k++)
		printf("%02X", (uint8_t) (fe->var_name)[k]);
      printf(" | ");	
      printf("%8s", fe->name);
      printf(" | ");
      printf("%2i", fe->attr);
      printf(" | ");
      printf("%02X", fe->type);
      printf(" | ");
      printf("%08X", fe->size);
      printf(" | ");
      printf("%8s", fe->fld_name);
      printf(" |");
	  printf("\n");
    }

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      TNode *child = t_node_nth_child(parent, j);
      VarEntry *ve = (VarEntry *) (child->data);

      printf("| ");
      for (k = 0; k < 8; k++) 
		printf("%02X", (uint8_t) (ve->var_name)[k]);
      printf(" | ");
      printf("%8s", ve->name);
      printf(" | ");
      printf("%2i", ve->attr);
      printf(" | ");
      printf("%02X", ve->type);
      printf(" | ");
      printf("%08X", ve->size);
      printf(" | ");
      printf("%8s", ve->fld_name);
      printf(" |");
	  printf("\n");
    }
  }
  if (!i)
    printf(_("  No variables"));

  printf(_("+------------------+----------+----+----+----------+----------+"));
  printf("\n");
}

static void dirlist_display_apps(TNode* tree)
{
  int i, k;
  TNode *apps = tree;

  printf(  "+------------------+----------+----+----+----------+\n");
  printf(_("| B. name          | T. name  |Attr|Type| Size     |\n"));
  printf(  "+------------------+----------+----+----+----------+\n");

  for (i = 0; i < (int)t_node_n_children(apps); i++) 
  {
    TNode *child = t_node_nth_child(apps, i);

    VarEntry *ve = (VarEntry *) (child->data);

    printf("| ");
    for (k = 0; k < 8; k++)
      printf("%02X", (uint8_t) (ve->var_name)[k]);
    printf(" | ");
    printf("%8s", ve->name);
    printf(" | ");
    printf("%2i", ve->attr);
    printf(" | ");
    printf("%02X", ve->type);
    printf(" | ");
    printf("%08X", ve->size);
    printf(" |");
  }
  if (!i)
  {
	printf(_("+ No applications  |          |    |    |          +"));
	printf("\n");
  }

  printf("+------------------+----------+----+----+----------+");
  printf("\n");
}

TIEXPORT void TICALL ticalcs_dirlist_display(TNode* tree)
{
	char *node_name;
  
	if (tree == NULL)
		return;

	node_name = (char *) tree->data;

    if (!strcmp(node_name, VAR_NODE_NAME))
		dirlist_display_vars(tree);
    else if (!strcmp(node_name, APP_NODE_NAME))
	    dirlist_display_apps(tree);
}


TIEXPORT VarEntry *TICALL ticalcs_dirlist_var_exist(TNode* tree, char *full_name)
{
	int i, j;
	TNode *vars = tree;
	char fldname[18];
	char varname[18];

	strcpy(fldname, tifiles_get_fldname(full_name));
	strcpy(varname, tifiles_get_varname(full_name));

	if (tree == NULL)
		return NULL;

	if (strcmp(vars->data, VAR_NODE_NAME))
		return NULL;

	for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
	{
		TNode *parent = t_node_nth_child(vars, i);
		VarEntry *fe = (VarEntry *) (parent->data);

		if ((fe != NULL) && strcmp(fe->var_name, fldname))
			continue;

		for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
		{
			TNode *child = t_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (child->data);

			if (!strcmp(ve->var_name, varname))
				return ve;
		}
	}

	return NULL;
}


TIEXPORT VarEntry *TICALL ticalcs_dirlist_app_exist(TNode* tree, char *appname)
{
	int i;
	TNode *apps = tree;

	if (tree == NULL)
		return NULL;

	if (strcmp(apps->data, VAR_NODE_NAME))
		return NULL;

	for (i = 0; i < (int)t_node_n_children(apps); i++) 
	{
		TNode *child = t_node_nth_child(apps, i);
		VarEntry *ve = (VarEntry *) (child->data);

		if (!strcmp(ve->var_name, appname))
			return ve;
	}

	return NULL;
}

TIEXPORT int TICALL ticalcs_dirlist_num_vars(TNode* tree)
{
	int i, j;
	TNode *vars = tree;
	int nvars = 0;

	if (tree == NULL)
		return 0;

	if (strcmp(vars->data, VAR_NODE_NAME))
		return 0;

	for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
	{
		TNode *parent = t_node_nth_child(vars, i);

		for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
			nvars++;
	}

	return nvars;
}


TIEXPORT int TICALL ticalcs_dirlist_mem_used(TNode* tree)
{
	int i, j;
	TNode *vars = tree;
	uint32_t mem = 0;

	if (tree == NULL)
		return 0;

	if (strcmp(vars->data, VAR_NODE_NAME))
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

// Reminder of new format...
/*
int tixx_directorylist2(TNode** vars, TNode** apps, uint32_t * memory)
{
  TNode *tree;
  TNode *var_node, *app_node;
  int err;

  // Get old directory list
  err = tcf->directorylist(&tree, memory);
  if (err) {
    *vars = *apps = NULL;
    return err;
  }

  // Get Vars tree
  var_node = t_node_nth_child(tree, 0);
  var_node->data = strdup(VAR_NODE_NAME); // so that it can be freed !

  // Get Apps tree
  app_node = t_node_nth_child(tree, 1);
  app_node->data = strdup(APP_NODE_NAME);

  // Split trees
  t_node_unlink(var_node);
  t_node_unlink(app_node);
  t_node_destroy(tree);

  // Returns new trees
  *vars = var_node;
  *apps = app_node;

  return 0;
}
*/
