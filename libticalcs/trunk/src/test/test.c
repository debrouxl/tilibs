/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tnode.h"

int array[10000];
tboolean failed = FALSE;

#define t_print  printf
#define t_free   free

#define	TEST(m,cond)	/*T_STMT_START*/ { failed = !(cond); \
if (failed) \
  { if (!m) \
      t_print ("\n(%s:%d) failed for: %s\n", __FILE__, __LINE__, ( # cond )); \
    else \
      t_print ("\n(%s:%d) failed for: %s: (%s)\n", __FILE__, __LINE__, ( # cond ), (tchar*)m); \
      exit(1); \
  } \
} /*T_STMT_END*/

#define	C2P(c)		((tpointer) ((long) (c)))
#define	P2C(p)		((tchar) ((long) (p)))

#define GLIB_TEST_STRING "el dorado "
#define GLIB_TEST_STRING_5 "el do"

typedef struct {
	tuint age;
	tchar name[40];
} GlibTestInfo;

static tboolean
node_build_string (TNode    *node,
		   tpointer  data)
{
  tchar **p = data;
  tchar *string;
  tchar c[2] = "_";

  c[0] = P2C (node->data);

  //string = g_strconcat (*p ? *p : "", c, NULL);
  //replaced by this piece of code:
  if(*p)
    string = (tchar *)malloc(strlen(*p) + strlen(c) + 1);
  else
    string = (tchar *)malloc(strlen(c) + 1);
  strcpy(string, *p ? *p : ""); strcat(string, c);
  //
  t_free (*p);
  *p = string;

  return FALSE;
}

static void
t_node_test (void)
{
  TNode *root;
  TNode *node;
  TNode *node_B;
  TNode *node_D;
  TNode *node_F;
  TNode *node_G;
  TNode *node_J;
  tuint i;
  tchar *tstring;

  failed = FALSE;

  root = t_node_new (C2P ('A'));
  TEST (NULL, t_node_depth (root) == 1 && t_node_max_height (root) == 1);

  node_B = t_node_new (C2P ('B'));
  t_node_append (root, node_B);
  TEST (NULL, root->children == node_B);

  t_node_append_data (node_B, C2P ('E'));
  t_node_prepend_data (node_B, C2P ('C'));
  node_D = t_node_new (C2P ('D'));
  t_node_insert (node_B, 1, node_D); 

  node_F = t_node_new (C2P ('F'));
  t_node_append (root, node_F);
  TEST (NULL, root->children->next == node_F);

  node_G = t_node_new (C2P ('G'));
  t_node_append (node_F, node_G);
  node_J = t_node_new (C2P ('J'));
  t_node_prepend (node_G, node_J);
  t_node_insert (node_G, 42, t_node_new (C2P ('K')));
  t_node_insert_data (node_G, 0, C2P ('H'));
  t_node_insert (node_G, 1, t_node_new (C2P ('I')));

  TEST (NULL, t_node_depth (root) == 1);
  TEST (NULL, t_node_max_height (root) == 4);
  TEST (NULL, t_node_depth (node_G->children->next) == 4);
  TEST (NULL, t_node_n_nodes (root, T_TRAVERSE_LEAFS) == 7);
  TEST (NULL, t_node_n_nodes (root, T_TRAVERSE_NON_LEAFS) == 4);
  TEST (NULL, t_node_n_nodes (root, T_TRAVERSE_ALL) == 11);
  TEST (NULL, t_node_max_height (node_F) == 3);
  TEST (NULL, t_node_n_children (node_G) == 4);
  TEST (NULL, t_node_find_child (root, T_TRAVERSE_ALL, C2P ('F')) == node_F);
  TEST (NULL, t_node_find (root, T_LEVEL_ORDER, T_TRAVERSE_NON_LEAFS, C2P ('I')) == NULL);
  TEST (NULL, t_node_find (root, T_IN_ORDER, T_TRAVERSE_LEAFS, C2P ('J')) == node_J);

  for (i = 0; i < t_node_n_children (node_B); i++)
    {
      node = t_node_nth_child (node_B, i);
      TEST (NULL, P2C (node->data) == ('C' + i));
    }
  
  for (i = 0; i < t_node_n_children (node_G); i++)
    TEST (NULL, t_node_child_position (node_G, t_node_nth_child (node_G, i)) == i);

  /* we have built:                    A
   *                                 /   \
   *                               B       F
   *                             / | \       \
   *                           C   D   E       G
   *                                         / /\ \
   *                                       H  I  J  K
   *
   * for in-order traversal, 'G' is considered to be the "left"
   * child of 'F', which will cause 'F' to be the last node visited.
   */

  tstring = NULL;
  t_node_traverse (root, T_PRE_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "ABCDEFGHIJK") == 0);
  t_free (tstring); tstring = NULL;
  t_node_traverse (root, T_POST_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "CDEBHIJKGFA") == 0);
  t_free (tstring); tstring = NULL;
  t_node_traverse (root, T_IN_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "CBDEAHGIJKF") == 0);
  t_free (tstring); tstring = NULL;
  t_node_traverse (root, T_LEVEL_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "ABFCDEGHIJK") == 0);
  t_free (tstring); tstring = NULL;
  
  t_node_traverse (root, T_LEVEL_ORDER, T_TRAVERSE_LEAFS, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "CDEHIJK") == 0);
  t_free (tstring); tstring = NULL;
  t_node_traverse (root, T_PRE_ORDER, T_TRAVERSE_NON_LEAFS, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "ABFG") == 0);
  t_free (tstring); tstring = NULL;

  t_node_reverse_children (node_B);
  t_node_reverse_children (node_G);

  t_node_traverse (root, T_LEVEL_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "ABFEDCGKJIH") == 0);
  t_free (tstring); tstring = NULL;
  
  t_node_append (node_D, t_node_new (C2P ('L')));
  t_node_append (node_D, t_node_new (C2P ('M')));

  t_node_traverse (root, T_LEVEL_ORDER, T_TRAVERSE_ALL, -1, node_build_string, &tstring);
  TEST (tstring, strcmp (tstring, "ABFEDCGLMKJIH") == 0);
  t_free (tstring); tstring = NULL;

  t_node_destroy (root);

  /* allocation tests */

  root = t_node_new (NULL);
  node = root;

  for (i = 0; i < 2048; i++)
    {
      t_node_append (node, t_node_new (NULL));
      if ((i%5) == 4)
	node = node->children->next;
    }
  TEST (NULL, t_node_max_height (root) > 100);
  TEST (NULL, t_node_n_nodes (root, T_TRAVERSE_ALL) == 1 + 2048);

  t_node_destroy (root);
  
  if (failed)
    exit(1);
}


int
main (int   argc,
      char *argv[])
{
  t_node_test ();

  return 0;
}

