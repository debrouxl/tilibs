/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "calc_def.h"
#include "headers.h"
#include "intl2.h"

TIEXPORT void TICALL ticalc_display_dirlist(TNode *tree)
{
  int i, j, k;
  TNode *vars, *apps;

  DISPLAY(_("--------+----------------+--------+----+----+--------+------\n"));
  DISPLAY(_("Name    |B. name         |T. name |Attr|Type|Size    |Parent\n"));
  DISPLAY(_("--------+----------------+--------+----+----+--------+------\n"));

  if(tree == NULL)
    return;

  // List variables
  vars = g_node_nth_child(tree, 0);
  if(vars == NULL)
    return;

  for(i=0; i<g_node_n_children(vars); i++) // parse folders
    {
      GNode *parent = g_node_nth_child(vars, i);
      TiVarEntry *fe = (TiVarEntry *)(parent->data);

      if(fe != NULL)
	{
	  for(k=0; k<8; k++)
	    {
	      if(isprint((fe->name)[k]))
		DISPLAY("%c", (fe->name)[k]);
	      else
		DISPLAY(" ");
	    }
	  DISPLAY("|");
	  for(k=0; k<8; k++)
	    {
	      DISPLAY("%02X", (uint8_t)(fe->name)[k]);
	    }
	  DISPLAY("|");
	  for(k=0; k<8; k++)
	    {
	      if(isprint((fe->trans)[k]))
		DISPLAY("%c", (fe->trans)[k]);
	      else
		DISPLAY(" ");
	    }
	  DISPLAY("|");
	  DISPLAY("%i   |",  fe->attr);
	  DISPLAY("%02X  |", fe->type);
	  DISPLAY("%08X|",   fe->size);
	  DISPLAY("%s\n",    fe->folder);
	}	  
      
      for(j=0; j<g_node_n_children(parent); j++) //parse variables
	{
	  GNode *child = g_node_nth_child(parent, j);
	  TiVarEntry *ve = (TiVarEntry *)(child->data);

	  for(k=0; k<8; k++)
	    {
	      if(isprint((ve->name)[k]))
		DISPLAY("%c", (ve->name)[k]);
	      else
		DISPLAY(" ");
	    }
	  DISPLAY("|");
	  for(k=0; k<8; k++)
	    {
	      DISPLAY("%02X", (uint8_t)(ve->name)[k]);
	    }
	  DISPLAY("|");
	  for(k=0; k<8; k++)
	    {
	      if(isprint((ve->trans)[k]))
		DISPLAY("%c", (ve->trans)[k]);
	      else
		DISPLAY(" ");
	    }
	  DISPLAY("|");
	  DISPLAY("%i   |",  ve->attr);
	  DISPLAY("%02X  |", ve->type);
	  DISPLAY("%08X|",   ve->size);
	  DISPLAY("%s\n",    ve->folder);
	}
    }
  if(!i)
    DISPLAY(_("No variables\n"));

  DISPLAY(_("--------+----------------+--------+----+----+--------+------\n"));
  
  // List applications
  apps = g_node_nth_child(tree, 1);
  if(apps == NULL)
    return;

  for(i=0; i<g_node_n_children(apps); i++)
    {
      GNode *child = g_node_nth_child(apps, i);

      TiVarEntry *ve = (TiVarEntry *)(child->data);

      for(k=0; k<8; k++)
	{
	  if(isprint((ve->name)[k]))
	    DISPLAY("%c", (ve->name)[k]);
	  else
	    DISPLAY(" ");
	}
      DISPLAY("|");
      for(k=0; k<8; k++)
	{
	  DISPLAY("%02X", (uint8_t)(ve->name)[k]);
	}
      DISPLAY("|");
      for(k=0; k<8; k++)
	{
	  if(isprint((ve->trans)[k]))
	    DISPLAY("%c", (ve->trans)[k]);
	  else
	    DISPLAY(" ");
	}
      DISPLAY("|");
      DISPLAY("%i   |",  ve->attr);
      DISPLAY("%02X  |", ve->type);
      DISPLAY("%08X|",   ve->size);
      DISPLAY("%s\n",    ve->folder);
    }
  if(!i)
    DISPLAY(_("No applications\n"));

  DISPLAY(_("--------+----------------+--------+----+----+--------+------\n"));
  DISPLAY("\n");
}

TIEXPORT
TiVarEntry* TICALL ticalc_check_if_var_exists(TNode *tree, char *full_name)
{
  int i, j;
  TNode *vars;
  char fldname[18];
  char varname[18];
  
  strcpy(fldname, tifiles_get_fldname(full_name));
  strcpy(varname, tifiles_get_varname(full_name));

  if(tree == NULL)
    return NULL;

  // List variables
  vars = g_node_nth_child(tree, 0);
  if(vars == NULL)
    return NULL;

  for(i=0; i<g_node_n_children(vars); i++) // parse folders
    {
      GNode *parent = g_node_nth_child(vars, i);
      TiVarEntry *fe = (TiVarEntry *)(parent->data);
      
      if( (fe != NULL) && strcmp(fe->name, fldname) )
	continue;
      
      for(j=0; j<g_node_n_children(parent); j++) //parse variables
	{
	  GNode *child = g_node_nth_child(parent, j);
	  TiVarEntry *ve = (TiVarEntry *)(child->data);

	  if(!strcmp(ve->name, varname))
	    return ve;
	}
    }
  
  return NULL;
}

TIEXPORT
TiVarEntry* TICALL ticalc_check_if_app_exists(TNode *tree, char *appname)
{
  int i;
  TNode *apps;

  if(tree == NULL)
    return NULL;
 
  // List applications
  apps = g_node_nth_child(tree, 1);
  if(apps == NULL)
    return NULL;

  for(i=0; i<g_node_n_children(apps); i++)
    {
      GNode *child = g_node_nth_child(apps, i);
      
      TiVarEntry *ve = (TiVarEntry *)(child->data);
      
      if(!strcmp(ve->name, appname))
	return ve;
    }
  
  return NULL;
}

TIEXPORT
int  TICALL ticalc_number_of_vars(TNode *tree)
{
  int i, j;
  TNode *vars;
  int nvars=0;
  
  if(tree == NULL)
    return 0;

  // List variables
  vars = g_node_nth_child(tree, 0);
  if(vars == NULL)
    return 0;

  for(i=0; i<g_node_n_children(vars); i++) // parse folders
    {
      GNode *parent = g_node_nth_child(vars, i);
      
      for(j=0; j<g_node_n_children(parent); j++) //parse variables
	{
	  nvars++;
	}
    }
  
  return nvars;
}

TIEXPORT
int  TICALL ticalc_memory_used(TNode *tree)
{
  int i, j;
  TNode *vars;
  uint32_t mem = 0;

  if(tree == NULL)
    return 0;

  // List variables
  vars = g_node_nth_child(tree, 0);
  if(vars == NULL)
    return 0;

  for(i=0; i<g_node_n_children(vars); i++) // parse folders
    {
      GNode *parent = g_node_nth_child(vars, i);
          
      for(j=0; j<g_node_n_children(parent); j++) //parse variables
	{
	  GNode *child = g_node_nth_child(parent, j);
	  TiVarEntry *ve = (TiVarEntry *)(child->data);

	  mem += ve->size;
	}
    }

  return mem;
}

/*
  The returned array is a NULL terminated array of string.
  It is pre-initialized with ACT_OVER(write).
*/
char** TICALL ticalc_create_action_array(int num_entries)
{
  char **ptr;
  int i;

  ptr = (char **)calloc(num_entries + 1, sizeof(char *));
  if(ptr == NULL)
    return NULL;

  for(i=0; i<num_entries; i++)
    {
      ptr[i] = (char *)calloc(18, sizeof(char));
      ptr[i][0] = ACT_OVER;
    }

  return ptr;
}

void   TICALL ticalc_destroy_action_array(char **array)
{
  int i;

  if(array == NULL)
    return;

  for(i=0; array[i] != NULL; i++)
      free(array[i]);

  free(array);
}
