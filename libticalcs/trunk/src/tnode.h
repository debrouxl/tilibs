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
  * I have systematically replaced G... by T... (GNode => TNode).
  */

#ifndef __T_NODE_H__
#define __T_NODE_H__

// by roms (start)
typedef char         tchar;
typedef	int	     tint;
typedef unsigned int tuint;
typedef int          tboolean;
typedef void*	     tpointer;

#define FALSE   0
#define TRUE    (!FALSE)
// by roms (end)


typedef struct _TNode		TNode;

/* Tree traverse flags */
typedef enum
{
  T_TRAVERSE_LEAFS      = 1 << 0,
  T_TRAVERSE_NON_LEAFS  = 1 << 1,
  T_TRAVERSE_ALL        = T_TRAVERSE_LEAFS | T_TRAVERSE_NON_LEAFS,
  T_TRAVERSE_MASK       = 0x03
} TTraverseFlags;

/* Tree traverse orders */
typedef enum
{
  T_IN_ORDER,
  T_PRE_ORDER,
  T_POST_ORDER,
  T_LEVEL_ORDER
} TTraverseType;

typedef tboolean	(*TNodeTraverseFunc)	(TNode	       *node,
						 tpointer	data);
typedef void		(*TNodeForeachFunc)	(TNode	       *node,
						 tpointer	data);

/* N-way tree implementation
 */
struct _TNode
{
  tpointer data;
  TNode	  *next;
  TNode	  *prev;
  TNode	  *parent;
  TNode	  *children;
};

#define	 T_NODE_IS_ROOT(node)	(((TNode*) (node))->parent == NULL && \
				 ((TNode*) (node))->prev == NULL && \
				 ((TNode*) (node))->next == NULL)
#define	 T_NODE_IS_LEAF(node)	(((TNode*) (node))->children == NULL)

TNode*	 t_node_new		(tpointer	   data);
void	 t_node_destroy		(TNode		  *root);
void	 t_node_unlink		(TNode		  *node);
TNode*   t_node_copy            (TNode            *node);
TNode*	 t_node_insert		(TNode		  *parent,
				 tint		   position,
				 TNode		  *node);
TNode*	 t_node_insert_before	(TNode		  *parent,
				 TNode		  *sibling,
				 TNode		  *node);
TNode*   t_node_insert_after    (TNode            *parent,
				 TNode            *sibling,
				 TNode            *node); 
TNode*	 t_node_prepend		(TNode		  *parent,
				 TNode		  *node);
tuint	 t_node_n_nodes		(TNode		  *root,
				 TTraverseFlags	   flags);
TNode*	 t_node_get_root	(TNode		  *node);
tboolean t_node_is_ancestor	(TNode		  *node,
				 TNode		  *descendant);
tuint	 t_node_depth		(TNode		  *node);
TNode*	 t_node_find		(TNode		  *root,
				 TTraverseType	   order,
				 TTraverseFlags	   flags,
				 tpointer	   data);

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
void	 t_node_traverse	(TNode		  *root,
				 TTraverseType	   order,
				 TTraverseFlags	   flags,
				 tint		   max_depth,
				 TNodeTraverseFunc func,
				 tpointer	   data);

/* return the maximum tree height starting with `node', this is an expensive
 * operation, since we need to visit all nodes. this could be shortened by
 * adding `tuint height' to struct _TNode, but then again, this is not very
 * often needed, and would make t_node_insert() more time consuming.
 */
tuint	 t_node_max_height	 (TNode *root);

void	 t_node_children_foreach (TNode		  *node,
				  TTraverseFlags   flags,
				  TNodeForeachFunc func,
				  tpointer	   data);
void	 t_node_reverse_children (TNode		  *node);
tuint	 t_node_n_children	 (TNode		  *node);
TNode*	 t_node_nth_child	 (TNode		  *node,
				  tuint		   n);
TNode*	 t_node_last_child	 (TNode		  *node);
TNode*	 t_node_find_child	 (TNode		  *node,
				  TTraverseFlags   flags,
				  tpointer	   data);
tint	 t_node_child_position	 (TNode		  *node,
				  TNode		  *child);
tint	 t_node_child_index	 (TNode		  *node,
				  tpointer	   data);

TNode*	 t_node_first_sibling	 (TNode		  *node);
TNode*	 t_node_last_sibling	 (TNode		  *node);

#define	 t_node_prev_sibling(node)	((node) ? \
					 ((TNode*) (node))->prev : NULL)
#define	 t_node_next_sibling(node)	((node) ? \
					 ((TNode*) (node))->next : NULL)
#define	 t_node_first_child(node)	((node) ? \
					 ((TNode*) (node))->children : NULL)

#endif /* __T_NODE_H__ */
