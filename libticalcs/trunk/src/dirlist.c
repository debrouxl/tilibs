/* Hey EMACS -*- linux-c -*- */
/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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

#include "calc_def.h"
#include "headers.h"
#include "intl2.h"


static gboolean free_varentry(GNode * node, gpointer data)
{
  if (node)
    free(node->data);
  return FALSE;
}

TIEXPORT void TICALL ticalc_dirlist_destroy(TNode ** tree)
{
  if (*tree != NULL) {
    g_node_traverse(*tree, G_IN_ORDER, G_TRAVERSE_ALL, -1,
		    free_varentry, NULL);
    g_node_destroy(*tree);
    *tree = NULL;
  }
}


static void dirlist_display_vars(TNode * tree)
{
  int i, j, k;
  TNode *vars = tree;

  DISPLAY(_
	  ("+---------+------------------+---------+----+----+--------+---------+\n"));
  DISPLAY(_
	  ("| Name    | B. name          | T. name |Attr|Type|Size    |Parent   |\n"));
  DISPLAY(_
	  ("+---------+------------------+---------+----+----+--------+---------+\n"));

  for (i = 0; i < g_node_n_children(vars); i++)	// parse folders
  {
    GNode *parent = g_node_nth_child(vars, i);
    TiVarEntry *fe = (TiVarEntry *) (parent->data);

    if (fe != NULL) {
      DISPLAY("| ");
      for (k = 0; k < 8; k++) {
	if (isprint((fe->name)[k]))
	  DISPLAY("%c", (fe->name)[k]);
	else
	  DISPLAY(" ");
      }
      DISPLAY("| ");
      for (k = 0; k < 8; k++) {
	DISPLAY("%02X", (uint8_t) (fe->name)[k]);
      }
      DISPLAY(" | ");
      for (k = 0; k < 8; k++) {
	if (isprint((fe->trans)[k]))
	  DISPLAY("%c", (fe->trans)[k]);
	else
	  DISPLAY(" ");
      }
      DISPLAY("|");
      DISPLAY(" %i  |", fe->attr);
      DISPLAY(" %02X |", fe->type);
      DISPLAY("%08X|", fe->size);
      DISPLAY("%s |\n", fe->folder);
    }

    for (j = 0; j < g_node_n_children(parent); j++)	//parse variables
    {
      GNode *child = g_node_nth_child(parent, j);
      TiVarEntry *ve = (TiVarEntry *) (child->data);

      DISPLAY("| ");
      for (k = 0; k < 8; k++) {
	if (isprint((ve->name)[k]))
	  DISPLAY("%c", (ve->name)[k]);
	else
	  DISPLAY(" ");
      }
      DISPLAY("| ");
      for (k = 0; k < 8; k++) {
	DISPLAY("%02X", (uint8_t) (ve->name)[k]);
      }
      DISPLAY(" | ");
      for (k = 0; k < 8; k++) {
	if (isprint((ve->trans)[k]))
	  DISPLAY("%c", (ve->trans)[k]);
	else
	  DISPLAY(" ");
      }
      DISPLAY("|");
      DISPLAY(" %i  |", ve->attr);
      DISPLAY(" %02X |", ve->type);
      DISPLAY("%08X|", ve->size);
      DISPLAY("%8s |\n", ve->folder);
    }
  }
  if (!i)
    DISPLAY(_("  No variables\n"));

  DISPLAY(_
	  ("+---------+------------------+---------+----+----+--------+---------+\n"));
}

static void dirlist_display_apps(TNode * tree)
{
  int i, k;
  TNode *apps = tree;

  DISPLAY(_("+----------+------------------+----+----+---------+\n"));
  DISPLAY(_("| Name     | B. name          |Attr|Type| Size    |\n"));
  DISPLAY(_("+----------+------------------+----+----+---------+\n"));

  for (i = 0; i < g_node_n_children(apps); i++) {
    GNode *child = g_node_nth_child(apps, i);

    TiVarEntry *ve = (TiVarEntry *) (child->data);

    DISPLAY("| ");
    for (k = 0; k < 8; k++) {
      if (isprint((ve->name)[k]))
	DISPLAY("%c", (ve->name)[k]);
      else
	DISPLAY(" ");
    }
    DISPLAY(" | ");
    for (k = 0; k < 8; k++) {
      DISPLAY("%02X", (uint8_t) (ve->name)[k]);
    }

    DISPLAY(" |");
    DISPLAY(" %i  |", ve->attr);
    DISPLAY(" %02X |", ve->type);
    DISPLAY("%08X |\n", ve->size);
  }
  if (!i)
    DISPLAY(_("  No applications\n"));

  DISPLAY(_("+----------+------------------+----+----+---------+\n"));
  DISPLAY("\n");
}


static void dirlist_display1(TNode * tree)
{
  TNode *vars, *apps;

  if (tree == NULL)
    return;

  // List variables
  vars = g_node_nth_child(tree, 0);
  if (vars == NULL)
    return;

  dirlist_display_vars(vars);

  // List applications
  apps = g_node_nth_child(tree, 1);
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
    DISPLAY("dirlist form #1\n");
    dirlist_display1(tree);
  } else {
    char *node_name = (char *) tree->data;

    DISPLAY("dirlist form #2\n");
    if (!strcmp(node_name, VAR_NODE_NAME))
      dirlist_display_vars(tree);
    else if (!strcmp(node_name, APP_NODE_NAME))
      dirlist_display_apps(tree);
    else {
      DISPLAY_ERROR("libticalcs: invalid tree !\n");
      DISPLAY_ERROR("Program halted before crashing...\n");
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
    vars = g_node_nth_child(tree, 0);
    if (vars == NULL)
      return NULL;
  } else {			// new format
    vars = tree;
    if (strcmp(vars->data, VAR_NODE_NAME))
      return 0;
  }

  for (i = 0; i < g_node_n_children(vars); i++)	// parse folders
  {
    GNode *parent = g_node_nth_child(vars, i);
    TiVarEntry *fe = (TiVarEntry *) (parent->data);

    if ((fe != NULL) && strcmp(fe->name, fldname))
      continue;

    for (j = 0; j < g_node_n_children(parent); j++)	//parse variables
    {
      GNode *child = g_node_nth_child(parent, j);
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
    apps = g_node_nth_child(tree, 1);
    if (apps == NULL)
      return NULL;
  } else {			// new format
    apps = tree;
    if (strcmp(apps->data, APP_NODE_NAME))
      return 0;
  }

  for (i = 0; i < g_node_n_children(apps); i++) {
    GNode *child = g_node_nth_child(apps, i);

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
    vars = g_node_nth_child(tree, 0);
    if (vars == NULL)
      return 0;
  } else {			// new format
    vars = tree;
    if (strcmp(vars->data, VAR_NODE_NAME))
      return 0;
  }

  for (i = 0; i < g_node_n_children(vars); i++)	// parse folders
  {
    GNode *parent = g_node_nth_child(vars, i);

    for (j = 0; j < g_node_n_children(parent); j++)	//parse variables
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
  vars = g_node_nth_child(tree, 0);
  if (vars == NULL)
    return 0;

  for (i = 0; i < g_node_n_children(vars); i++)	// parse folders
  {
    GNode *parent = g_node_nth_child(vars, i);

    for (j = 0; j < g_node_n_children(parent); j++)	//parse variables
    {
      GNode *child = g_node_nth_child(parent, j);
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

  *vars = g_node_new("Variables");
  *apps = g_node_new("Applications");

  err = tcf->directorylist(&tree, memory);
  if (err) {
    *vars = *apps = NULL;
    return err;
  }

  var_node = *vars = g_node_nth_child(tree, 0);
  var_node->data = VAR_NODE_NAME;

  app_node = *apps = g_node_nth_child(tree, 1);
  app_node->data = APP_NODE_NAME;

  g_node_unlink(var_node);
  g_node_unlink(app_node);
  g_node_destroy(tree);

  return 0;
}
