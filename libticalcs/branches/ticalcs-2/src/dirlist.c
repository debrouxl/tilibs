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
  Utility functions for directory list (tree)
  They support both format (old and new format introduced in v4.4.3)
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "gettext.h"

#include "calc_def.h"
#include "headers.h"
#include "tnode.h"
#include "printl.h"

static tboolean free_varentry(TNode * node, tpointer data)
{
  if (node)
    free(node->data);
  return FALSE;
}

TIEXPORT void TICALL ticalc_dirlist_destroy(TNode ** tree)
{
  if (*tree != NULL) {
    t_node_traverse(*tree, T_IN_ORDER, T_TRAVERSE_ALL, -1,
		    free_varentry, NULL);
    t_node_destroy(*tree);
    *tree = NULL;
  }
}


static void dirlist_display_vars(TNode * tree)
{
  int i, j, k;
  TNode *vars = tree;

  printl2(0,   "+------------------+----------+----+----+----------+----------+\n");
  printl2(0, _("| B. name          | T. name  |Attr|Type| Size     | Parent   |\n"));
  printl2(0,   "+------------------+----------+----+----+----------+----------+\n");

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);
    TiVarEntry *fe = (TiVarEntry *) (parent->data);

    if (fe != NULL) {
      printl2(0, "| ");
      for (k = 0; k < 8; k++)
	printl2(0, "%02X", (uint8_t) (fe->name)[k]);
      printl2(0, " | ");	
      printl2(0, "%8s", fe->trans);
      printl2(0, " | ");
      printl2(0, "%2i", fe->attr);
      printl2(0, " | ");
      printl2(0, "%02X", fe->type);
      printl2(0, " | ");
      printl2(0, "%08X", fe->size);
      printl2(0, " | ");
      printl2(0, "%8s", fe->folder);
      printl2(0, " |\n");
    }

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      TNode *child = t_node_nth_child(parent, j);
      TiVarEntry *ve = (TiVarEntry *) (child->data);

      printl2(0, "| ");
      for (k = 0; k < 8; k++) {
	printl2(0, "%02X", (uint8_t) (ve->name)[k]);
      }
      printl2(0, " | ");
      printl2(0, "%8s", ve->trans);
      printl2(0, " | ");
      printl2(0, "%2i", ve->attr);
      printl2(0, " | ");
      printl2(0, "%02X", ve->type);
      printl2(0, " | ");
      printl2(0, "%08X", ve->size);
      printl2(0, " | ");
      printl2(0, "%8s", ve->folder);
      printl2(0, " |\n");
    }
  }
  if (!i)
    printl2(0, _("  No variables\n"));

  printl2(0, _
	  ("+------------------+----------+----+----+----------+----------+\n"));
}

static void dirlist_display_apps(TNode * tree)
{
  int i, k;
  TNode *apps = tree;

  printl2(0,   "+------------------+----------+----+----+----------+\n");
  printl2(0, _("| B. name          | T. name  |Attr|Type| Size     |\n"));
  printl2(0,   "+------------------+----------+----+----+----------+\n");

  for (i = 0; i < (int)t_node_n_children(apps); i++) {
    TNode *child = t_node_nth_child(apps, i);

    TiVarEntry *ve = (TiVarEntry *) (child->data);

    printl2(0, "| ");
    for (k = 0; k < 8; k++) {
      printl2(0, "%02X", (uint8_t) (ve->name)[k]);
    }
    printl2(0, " | ");
    printl2(0, "%8s", ve->trans);
    printl2(0, " | ");
    printl2(0, "%2i", ve->attr);
    printl2(0, " | ");
    printl2(0, "%02X", ve->type);
    printl2(0, " | ");
    printl2(0, "%08X", ve->size);
    printl2(0, " |\n");
  }
  if (!i)
    printl2(0, _("  No applications\n"));

  printl2(0, "+------------------+----------+----+----+----------+\n");
  printl2(0, "\n");
}


static void dirlist_display1(TNode * tree)
{
  TNode *vars, *apps;

  if (tree == NULL)
    return;

  // List variables
  vars = t_node_nth_child(tree, 0);
  if (vars == NULL)
    return;

  dirlist_display_vars(vars);

  // List applications
  apps = t_node_nth_child(tree, 1);
  if (apps == NULL)
    return;

  dirlist_display_apps(apps);
}


TIEXPORT void TICALL ticalc_dirlist_display(TNode * tree)
{
  if (tree == NULL)
    return;

  // Determine tree format
  if (tree->data == NULL) {
    printl2(0, "dirlist form #1: vars & apps\n");
    dirlist_display1(tree);
  } else {
    char *node_name = (char *) tree->data;

    if (!strcmp(node_name, VAR_NODE_NAME)) {
	    printl2(0, "dirlist form #2: vars\n");
	    dirlist_display_vars(tree);
    } else if (!strcmp(node_name, APP_NODE_NAME)) {
	    printl2(0, "dirlist form #2: apps\n");
	    dirlist_display_apps(tree);
    } else {
      printl2(2, "invalid tree !\n");
      printl2(2, "Program halted before crashing...\n");
      exit(-1);
    }
  }
}


TIEXPORT TiVarEntry *TICALL ticalc_check_if_var_exists(TNode * tree,
						       char *full_name)
{
  int i, j;
  TNode *vars;
  char fldname[18];
  char varname[18];

  strcpy(fldname, tifiles_get_fldname(full_name));
  strcpy(varname, tifiles_get_varname(full_name));

  if (tree == NULL)
    return NULL;

  if (tree->data == NULL) {	// old format
    vars = t_node_nth_child(tree, 0);
    if (vars == NULL)
      return NULL;
  } else {			// new format
    vars = tree;
    if (strcmp(vars->data, VAR_NODE_NAME))
      return 0;
  }

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);
    TiVarEntry *fe = (TiVarEntry *) (parent->data);

    if ((fe != NULL) && strcmp(fe->name, fldname))
      continue;

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      TNode *child = t_node_nth_child(parent, j);
      TiVarEntry *ve = (TiVarEntry *) (child->data);

      if (!strcmp(ve->name, varname))
	return ve;
    }
  }

  return NULL;
}


TIEXPORT TiVarEntry *TICALL ticalc_check_if_app_exists(TNode * tree,
						       char *appname)
{
  int i;
  TNode *apps;

  if (tree == NULL)
    return NULL;

  if (tree->data == NULL) {	// old format
    apps = t_node_nth_child(tree, 1);
    if (apps == NULL)
      return NULL;
  } else {			// new format
    apps = tree;
    if (strcmp(apps->data, APP_NODE_NAME))
      return 0;
  }

  for (i = 0; i < (int)t_node_n_children(apps); i++) {
    TNode *child = t_node_nth_child(apps, i);

    TiVarEntry *ve = (TiVarEntry *) (child->data);

    if (!strcmp(ve->name, appname))
      return ve;
  }

  return NULL;
}


TIEXPORT int TICALL ticalc_dirlist_numvars(TNode * tree)
{
  int i, j;
  TNode *vars;
  int nvars = 0;

  if (tree == NULL)
    return 0;

  // List variables
  if (tree->data == NULL) {	// old format
    vars = t_node_nth_child(tree, 0);
    if (vars == NULL)
      return 0;
  } else {			// new format
    vars = tree;
    if (strcmp(vars->data, VAR_NODE_NAME))
      return 0;
  }

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      nvars++;
    }
  }

  return nvars;
}


TIEXPORT int TICALL ticalc_dirlist_memused(TNode * tree)
{
  int i, j;
  TNode *vars;
  uint32_t mem = 0;

  if (tree == NULL)
    return 0;

  // List variables
  vars = t_node_nth_child(tree, 0);
  if (vars == NULL)
    return 0;

  for (i = 0; i < (int)t_node_n_children(vars); i++)	// parse folders
  {
    TNode *parent = t_node_nth_child(vars, i);

    for (j = 0; j < (int)t_node_n_children(parent); j++)	//parse variables
    {
      TNode *child = t_node_nth_child(parent, j);
      TiVarEntry *ve = (TiVarEntry *) (child->data);

      mem += ve->size;
    }
  }

  return mem;
}

/*
  The returned array is a NULL terminated array of string.
  It is pre-initialized with ACT_OVER(write).
*/
TIEXPORT char **TICALL ticalc_action_create_array(int num_entries)
{
  char **ptr;
  int i;

  ptr = (char **) calloc(num_entries + 1, sizeof(char *));
  if (ptr == NULL)
    return NULL;

  for (i = 0; i < num_entries; i++) {
    ptr[i] = (char *) calloc(18, sizeof(char));
    ptr[i][0] = ACT_OVER;
  }

  return ptr;
}

TIEXPORT void TICALL ticalc_action_destroy_array(char **array)
{
  int i;

  if (array == NULL)
    return;

  for (i = 0; array[i] != NULL; i++)
    free(array[i]);

  free(array);
}

/*
  Implement new tree format by using old functions (maintains compatibility)
  Simply break the tree into 2 sub-trees but take care of memory.
 */
TicalcFncts *tcf;

int tixx_directorylist2(TNode ** vars, TNode ** apps, uint32_t * memory)
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
