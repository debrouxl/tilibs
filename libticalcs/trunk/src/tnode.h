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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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
 
 /*
  * Modified by roms for use in ticalcs library.
  * I needed tree manipulation routines without glib dependancies.
  */

#ifndef __T_NODE_H__
#define __T_NODE_H__

// by roms (start)

typedef int          gboolean;
typedef void*	     gpointer;
typedef	int	     gint;
typedef unsigned int guint;

#define FALSE   0
#define TRUE    (!FALSE)

// by roms (end)


typedef struct _GNode		GNode;

/* Tree traverse flags */
typedef enum
{
  G_TRAVERSE_LEAFS      = 1 << 0,
  G_TRAVERSE_NON_LEAFS  = 1 << 1,
  G_TRAVERSE_ALL        = G_TRAVERSE_LEAFS | G_TRAVERSE_NON_LEAFS,
  G_TRAVERSE_MASK       = 0x03
} GTraverseFlags;

/* Tree traverse orders */
typedef enum
{
  G_IN_ORDER,
  G_PRE_ORDER,
  G_POST_ORDER,
  G_LEVEL_ORDER
} GTraverseType;

typedef gboolean	(*GNodeTraverseFunc)	(GNode	       *node,
						 gpointer	data);
typedef void		(*GNodeForeachFunc)	(GNode	       *node,
						 gpointer	data);

/* N-way tree implementation
 */
struct _GNode
{
  gpointer data;
  GNode	  *next;
  GNode	  *prev;
  GNode	  *parent;
  GNode	  *children;
};

#define	 G_NODE_IS_ROOT(node)	(((GNode*) (node))->parent == NULL && \
				 ((GNode*) (node))->prev == NULL && \
				 ((GNode*) (node))->next == NULL)
#define	 G_NODE_IS_LEAF(node)	(((GNode*) (node))->children == NULL)

GNode*	 t_node_new		(gpointer	   data);
void	 t_node_destroy		(GNode		  *root);
void	 t_node_unlink		(GNode		  *node);
GNode*   t_node_copy            (GNode            *node);
GNode*	 t_node_insert		(GNode		  *parent,
				 gint		   position,
				 GNode		  *node);
GNode*	 t_node_insert_before	(GNode		  *parent,
				 GNode		  *sibling,
				 GNode		  *node);
GNode*   t_node_insert_after    (GNode            *parent,
				 GNode            *sibling,
				 GNode            *node); 
GNode*	 t_node_prepend		(GNode		  *parent,
				 GNode		  *node);
guint	 t_node_n_nodes		(GNode		  *root,
				 GTraverseFlags	   flags);
GNode*	 t_node_get_root	(GNode		  *node);
gboolean t_node_is_ancestor	(GNode		  *node,
				 GNode		  *descendant);
guint	 t_node_depth		(GNode		  *node);
GNode*	 t_node_find		(GNode		  *root,
				 GTraverseType	   order,
				 GTraverseFlags	   flags,
				 gpointer	   data);

/* convenience macros */
#define t_node_append(parent, node)				\
     t_node_insert_before ((parent), NULL, (node))
#define	t_node_insert_data(parent, position, data)		\
     t_node_insert ((parent), (position), t_node_new (data))
#define	t_node_insert_data_before(parent, sibling, data)	\
     t_node_insert_before ((parent), (sibling), t_node_new (data))
#define	t_node_prepend_data(parent, data)			\
     t_node_prepend ((parent), t_node_new (data))
#define	t_node_append_data(parent, data)			\
     t_node_insert_before ((parent), NULL, t_node_new (data))

/* traversal function, assumes that `node' is root
 * (only traverses `node' and its subtree).
 * this function is just a high level interface to
 * low level traversal functions, optimized for speed.
 */
void	 t_node_traverse	(GNode		  *root,
				 GTraverseType	   order,
				 GTraverseFlags	   flags,
				 gint		   max_depth,
				 GNodeTraverseFunc func,
				 gpointer	   data);

/* return the maximum tree height starting with `node', this is an expensive
 * operation, since we need to visit all nodes. this could be shortened by
 * adding `guint height' to struct _GNode, but then again, this is not very
 * often needed, and would make t_node_insert() more time consuming.
 */
guint	 t_node_max_height	 (GNode *root);

void	 t_node_children_foreach (GNode		  *node,
				  GTraverseFlags   flags,
				  GNodeForeachFunc func,
				  gpointer	   data);
void	 t_node_reverse_children (GNode		  *node);
guint	 t_node_n_children	 (GNode		  *node);
GNode*	 t_node_nth_child	 (GNode		  *node,
				  guint		   n);
GNode*	 t_node_last_child	 (GNode		  *node);
GNode*	 t_node_find_child	 (GNode		  *node,
				  GTraverseFlags   flags,
				  gpointer	   data);
gint	 t_node_child_position	 (GNode		  *node,
				  GNode		  *child);
gint	 t_node_child_index	 (GNode		  *node,
				  gpointer	   data);

GNode*	 t_node_first_sibling	 (GNode		  *node);
GNode*	 t_node_last_sibling	 (GNode		  *node);

#define	 t_node_prev_sibling(node)	((node) ? \
					 ((GNode*) (node))->prev : NULL)
#define	 t_node_next_sibling(node)	((node) ? \
					 ((GNode*) (node))->next : NULL)
#define	 t_node_first_child(node)	((node) ? \
					 ((GNode*) (node))->children : NULL)

#endif /* __G_NODE_H__ */
