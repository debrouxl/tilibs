/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * TNode: N-way tree implementation.
 * Copyright (C) 1998 Tim Janik
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

#include <assert.h>
#include <stdlib.h>

#include "tnode.h"
#include "export.h"

// glib allocators
#define t_new0(struct_type, n_structs)		\
    ((struct_type *) calloc (sizeof (struct_type), n_structs))
#define t_free(struct_type)	\
	(free (struct_type))

// assertions
#define	t_return_if_fail(e)	     assert(e)
#define t_return_val_if_fail(e, val) { assert(e); if(!(e)) return (val); }


TNode*
t_node_new (tpointer data)
{
  TNode *node;

  node = t_new0 (TNode, 1);
  
  node->data = data;
  
  return node;
}

static void
t_nodes_free (TNode *root)
{
  TNode *node, *next;
  
  node = root;
  while (node != NULL)
    {
      next = node->next;
      t_nodes_free (node->children);
      t_free (node);
      node = next;
    }
}

void
t_node_destroy (TNode *root)
{
  t_return_if_fail (root != NULL);
  
  if (!T_NODE_IS_ROOT (root))
    t_node_unlink (root);
  
  t_nodes_free (root);
}

void
t_node_unlink (TNode *node)
{
  t_return_if_fail (node != NULL);
  
  if (node->prev)
    node->prev->next = node->next;
  else if (node->parent)
    node->parent->children = node->next;
  node->parent = NULL;
  if (node->next)
    {
      node->next->prev = node->prev;
      node->next = NULL;
    }
  node->prev = NULL;
}

TNode*
t_node_copy (TNode *node)
{
  TNode *new_node = NULL;
  
  if (node)
    {
      TNode *child;
      
      new_node = t_node_new (node->data);
      
      for (child = t_node_last_child (node); child; child = child->prev)
	t_node_prepend (new_node, t_node_copy (child));
    }
  
  return new_node;
}

TNode*
t_node_insert (TNode *parent,
	       tint   position,
	       TNode *node)
{
  t_return_val_if_fail (parent != NULL, node);
  t_return_val_if_fail (node != NULL, node);
  t_return_val_if_fail (T_NODE_IS_ROOT (node), node);
  
  if (position > 0)
    return t_node_insert_before (parent,
				 t_node_nth_child (parent, position),
				 node);
  else if (position == 0)
    return t_node_prepend (parent, node);
  else /* if (position < 0) */
    return t_node_append (parent, node);
}

TNode*
t_node_insert_before (TNode *parent,
		      TNode *sibling,
		      TNode *node)
{
  t_return_val_if_fail (parent != NULL, node);
  t_return_val_if_fail (node != NULL, node);
  t_return_val_if_fail (T_NODE_IS_ROOT (node), node);
  if (sibling)
    t_return_val_if_fail (sibling->parent == parent, node);
  
  node->parent = parent;
  
  if (sibling)
    {
      if (sibling->prev)
	{
	  node->prev = sibling->prev;
	  node->prev->next = node;
	  node->next = sibling;
	  sibling->prev = node;
	}
      else
	{
	  node->parent->children = node;
	  node->next = sibling;
	  sibling->prev = node;
	}
    }
  else
    {
      if (parent->children)
	{
	  sibling = parent->children;
	  while (sibling->next)
	    sibling = sibling->next;
	  node->prev = sibling;
	  sibling->next = node;
	}
      else
	node->parent->children = node;
    }

  return node;
}

TNode*
t_node_insert_after (TNode *parent,
		     TNode *sibling,
		     TNode *node)
{
  t_return_val_if_fail (parent != NULL, node);
  t_return_val_if_fail (node != NULL, node);
  t_return_val_if_fail (T_NODE_IS_ROOT (node), node);
  if (sibling)
    t_return_val_if_fail (sibling->parent == parent, node);

  node->parent = parent;

  if (sibling)
    {
      if (sibling->next)
	{
	  sibling->next->prev = node;
	}
      node->next = sibling->next;
      node->prev = sibling;
      sibling->next = node;
    }
  else
    {
      if (parent->children)
	{
	  node->next = parent->children;
	  parent->children->prev = node;
	}
      parent->children = node;
    }

  return node;
}

TNode*
t_node_prepend (TNode *parent,
		TNode *node)
{
  t_return_val_if_fail (parent != NULL, node);
  
  return t_node_insert_before (parent, parent->children, node);
}

TNode*
t_node_get_root (TNode *node)
{
  t_return_val_if_fail (node != NULL, NULL);
  
  while (node->parent)
    node = node->parent;
  
  return node;
}

tboolean
t_node_is_ancestor (TNode *node,
		    TNode *descendant)
{
  t_return_val_if_fail (node != NULL, FALSE);
  t_return_val_if_fail (descendant != NULL, FALSE);
  
  while (descendant)
    {
      if (descendant->parent == node)
	return TRUE;
      
      descendant = descendant->parent;
    }
  
  return FALSE;
}

/* returns 1 for root, 2 for first level children,
 * 3 for children's children...
 */
tuint
t_node_depth (TNode *node)
{
  register tuint depth = 0;
  
  while (node)
    {
      depth++;
      node = node->parent;
    }
  
  return depth;
}

void
t_node_reverse_children (TNode *node)
{
  TNode *child;
  TNode *last;
  
  t_return_if_fail (node != NULL);
  
  child = node->children;
  last = NULL;
  while (child)
    {
      last = child;
      child = last->next;
      last->next = last->prev;
      last->prev = child;
    }
  node->children = last;
}

tuint
t_node_max_height (TNode *root)
{
  register TNode *child;
  register tuint max_height = 0;
  
  if (!root)
    return 0;
  
  child = root->children;
  while (child)
    {
      register tuint tmp_height;
      
      tmp_height = t_node_max_height (child);
      if (tmp_height > max_height)
	max_height = tmp_height;
      child = child->next;
    }
  
  return max_height + 1;
}

static tboolean
t_node_traverse_pre_order (TNode	    *node,
			   TTraverseFlags    flags,
			   TNodeTraverseFunc func,
			   tpointer	     data)
{
  if (node->children)
    {
      TNode *child;
      
      if ((flags & T_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      child = node->children;
      while (child)
	{
	  register TNode *current;
	  
	  current = child;
	  child = current->next;
	  if (t_node_traverse_pre_order (current, flags, func, data))
	    return TRUE;
	}
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_depth_traverse_pre_order (TNode		  *node,
				 TTraverseFlags	   flags,
				 tuint		   depth,
				 TNodeTraverseFunc func,
				 tpointer	   data)
{
  if (node->children)
    {
      TNode *child;
      
      if ((flags & T_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      depth--;
      if (!depth)
	return FALSE;
      
      child = node->children;
      while (child)
	{
	  register TNode *current;
	  
	  current = child;
	  child = current->next;
	  if (t_node_depth_traverse_pre_order (current, flags, depth, func, data))
	    return TRUE;
	}
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_traverse_post_order (TNode	     *node,
			    TTraverseFlags    flags,
			    TNodeTraverseFunc func,
			    tpointer	      data)
{
  if (node->children)
    {
      TNode *child;
      
      child = node->children;
      while (child)
	{
	  register TNode *current;
	  
	  current = child;
	  child = current->next;
	  if (t_node_traverse_post_order (current, flags, func, data))
	    return TRUE;
	}
      
      if ((flags & T_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_depth_traverse_post_order (TNode		   *node,
				  TTraverseFlags    flags,
				  tuint		    depth,
				  TNodeTraverseFunc func,
				  tpointer	    data)
{
  if (node->children)
    {
      depth--;
      if (depth)
	{
	  TNode *child;
	  
	  child = node->children;
	  while (child)
	    {
	      register TNode *current;
	      
	      current = child;
	      child = current->next;
	      if (t_node_depth_traverse_post_order (current, flags, depth, func, data))
		return TRUE;
	    }
	}
      
      if ((flags & T_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_traverse_in_order (TNode		   *node,
			  TTraverseFlags    flags,
			  TNodeTraverseFunc func,
			  tpointer	    data)
{
  if (node->children)
    {
      TNode *child;
      register TNode *current;
      
      child = node->children;
      current = child;
      child = current->next;
      
      if (t_node_traverse_in_order (current, flags, func, data))
	return TRUE;
      
      if ((flags & T_TRAVERSE_NON_LEAFS) &&
	  func (node, data))
	return TRUE;
      
      while (child)
	{
	  current = child;
	  child = current->next;
	  if (t_node_traverse_in_order (current, flags, func, data))
	    return TRUE;
	}
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_depth_traverse_in_order (TNode		 *node,
				TTraverseFlags	  flags,
				tuint		  depth,
				TNodeTraverseFunc func,
				tpointer	  data)
{
  if (node->children)
    {
      depth--;
      if (depth)
	{
	  TNode *child;
	  register TNode *current;
	  
	  child = node->children;
	  current = child;
	  child = current->next;
	  
	  if (t_node_depth_traverse_in_order (current, flags, depth, func, data))
	    return TRUE;
	  
	  if ((flags & T_TRAVERSE_NON_LEAFS) &&
	      func (node, data))
	    return TRUE;
	  
	  while (child)
	    {
	      current = child;
	      child = current->next;
	      if (t_node_depth_traverse_in_order (current, flags, depth, func, data))
		return TRUE;
	    }
	}
      else if ((flags & T_TRAVERSE_NON_LEAFS) &&
	       func (node, data))
	return TRUE;
    }
  else if ((flags & T_TRAVERSE_LEAFS) &&
	   func (node, data))
    return TRUE;
  
  return FALSE;
}

static tboolean
t_node_traverse_level (TNode		 *node,
		       TTraverseFlags	  flags,
		       tuint		  level,
		       TNodeTraverseFunc func,
		       tpointer	  data,
		       tboolean         *more_levels)
{
  if (level == 0) 
    {
      if (node->children)
	{
	  *more_levels = TRUE;
	  return (flags & T_TRAVERSE_NON_LEAFS) && func (node, data);
	}
      else
	{
	  return (flags & T_TRAVERSE_LEAFS) && func (node, data);
	}
    }
  else 
    {
      node = node->children;
      
      while (node)
	{
	  if (t_node_traverse_level (node, flags, level - 1, func, data, more_levels))
	    return TRUE;

	  node = node->next;
	}
    }

  return FALSE;
}

static tboolean
t_node_depth_traverse_level (TNode		 *node,
			     TTraverseFlags	  flags,
			     tuint		  depth,
			     TNodeTraverseFunc func,
			     tpointer	  data)
{
  tint level;
  tboolean more_levels;

  level = 0;  
  while (level != depth) 
    {
      more_levels = FALSE;
      if (t_node_traverse_level (node, flags, level, func, data, &more_levels))
	return TRUE;
      if (!more_levels)
	break;
      level++;
    }
  return FALSE;
}

void
t_node_traverse (TNode		  *root,
		 TTraverseType	   order,
		 TTraverseFlags	   flags,
		 tint		   depth,
		 TNodeTraverseFunc func,
		 tpointer	   data)
{
  t_return_if_fail (root != NULL);
  t_return_if_fail (func != NULL);
  t_return_if_fail (order <= T_LEVEL_ORDER);
  t_return_if_fail (flags <= T_TRAVERSE_MASK);
  t_return_if_fail (depth == -1 || depth > 0);
  
  switch (order)
    {
    case T_PRE_ORDER:
      if (depth < 0)
	t_node_traverse_pre_order (root, flags, func, data);
      else
	t_node_depth_traverse_pre_order (root, flags, depth, func, data);
      break;
    case T_POST_ORDER:
      if (depth < 0)
	t_node_traverse_post_order (root, flags, func, data);
      else
	t_node_depth_traverse_post_order (root, flags, depth, func, data);
      break;
    case T_IN_ORDER:
      if (depth < 0)
	t_node_traverse_in_order (root, flags, func, data);
      else
	t_node_depth_traverse_in_order (root, flags, depth, func, data);
      break;
    case T_LEVEL_ORDER:
      t_node_depth_traverse_level (root, flags, depth, func, data);
      break;
    }
}

static tboolean
t_node_find_func (TNode	  *node,
		  tpointer data)
{
  register tpointer *d = data;
  
  if (*d != node->data)
    return FALSE;
  
  *(++d) = node;
  
  return TRUE;
}

TNode*
t_node_find (TNode	       *root,
	     TTraverseType	order,
	     TTraverseFlags	flags,
	     tpointer		data)
{
  tpointer d[2];
  
  t_return_val_if_fail (root != NULL, NULL);
  t_return_val_if_fail (order <= T_LEVEL_ORDER, NULL);
  t_return_val_if_fail (flags <= T_TRAVERSE_MASK, NULL);
  
  d[0] = data;
  d[1] = NULL;
  
  t_node_traverse (root, order, flags, -1, t_node_find_func, d);
  
  return d[1];
}

static void
t_node_count_func (TNode	 *node,
		   TTraverseFlags flags,
		   tuint	 *n)
{
  if (node->children)
    {
      TNode *child;
      
      if (flags & T_TRAVERSE_NON_LEAFS)
	(*n)++;
      
      child = node->children;
      while (child)
	{
	  t_node_count_func (child, flags, n);
	  child = child->next;
	}
    }
  else if (flags & T_TRAVERSE_LEAFS)
    (*n)++;
}

tuint
t_node_n_nodes (TNode	      *root,
		TTraverseFlags flags)
{
  tuint n = 0;
  
  t_return_val_if_fail (root != NULL, 0);
  t_return_val_if_fail (flags <= T_TRAVERSE_MASK, 0);
  
  t_node_count_func (root, flags, &n);
  
  return n;
}

TNode*
t_node_last_child (TNode *node)
{
  t_return_val_if_fail (node != NULL, NULL);
  
  node = node->children;
  if (node)
    while (node->next)
      node = node->next;
  
  return node;
}

TIEXPORT  TNode*
TICALL t_node_nth_child (TNode *node,
		  tuint	 n)
{
  t_return_val_if_fail (node != NULL, NULL);
  
  node = node->children;
  if (node)
    while ((n-- > 0) && node)
      node = node->next;
  
  return node;
}

TIEXPORT tuint
TICALL t_node_n_children (TNode *node)
{
  tuint n = 0;
  
  t_return_val_if_fail (node != NULL, 0);
  
  node = node->children;
  while (node)
    {
      n++;
      node = node->next;
    }
  
  return n;
}

TNode*
t_node_find_child (TNode	 *node,
		   TTraverseFlags flags,
		   tpointer	  data)
{
  t_return_val_if_fail (node != NULL, NULL);
  t_return_val_if_fail (flags <= T_TRAVERSE_MASK, NULL);
  
  node = node->children;
  while (node)
    {
      if (node->data == data)
	{
	  if (T_NODE_IS_LEAF (node))
	    {
	      if (flags & T_TRAVERSE_LEAFS)
		return node;
	    }
	  else
	    {
	      if (flags & T_TRAVERSE_NON_LEAFS)
		return node;
	    }
	}
      node = node->next;
    }
  
  return NULL;
}

tint
t_node_child_position (TNode *node,
		       TNode *child)
{
  register tuint n = 0;
  
  t_return_val_if_fail (node != NULL, -1);
  t_return_val_if_fail (child != NULL, -1);
  t_return_val_if_fail (child->parent == node, -1);
  
  node = node->children;
  while (node)
    {
      if (node == child)
	return n;
      n++;
      node = node->next;
    }
  
  return -1;
}

tint
t_node_child_index (TNode   *node,
		    tpointer data)
{
  register tuint n = 0;
  
  t_return_val_if_fail (node != NULL, -1);
  
  node = node->children;
  while (node)
    {
      if (node->data == data)
	return n;
      n++;
      node = node->next;
    }
  
  return -1;
}

TNode*
t_node_first_sibling (TNode *node)
{
  t_return_val_if_fail (node != NULL, NULL);
  
  if (node->parent)
    return node->parent->children;
  
  while (node->prev)
    node = node->prev;
  
  return node;
}

TNode*
t_node_last_sibling (TNode *node)
{
  t_return_val_if_fail (node != NULL, NULL);
  
  while (node->next)
    node = node->next;
  
  return node;
}

void
t_node_children_foreach (TNode		 *node,
			 TTraverseFlags	  flags,
			 TNodeForeachFunc func,
			 tpointer	  data)
{
  t_return_if_fail (node != NULL);
  t_return_if_fail (flags <= T_TRAVERSE_MASK);
  t_return_if_fail (func != NULL);
  
  node = node->children;
  while (node)
    {
      register TNode *current;
      
      current = node;
      node = current->next;
      if (T_NODE_IS_LEAF (current))
	{
	  if (flags & T_TRAVERSE_LEAFS)
	    func (current, data);
	}
      else
	{
	  if (flags & T_TRAVERSE_NON_LEAFS)
	    func (current, data);
	}
    }
}
