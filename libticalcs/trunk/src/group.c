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

#include <stdio.h>
#include <string.h>

#include "calc_ext.h"
#include "const.h"

/* 
 *   This function generates the header for single variable files 
 */
void generate_89_92_92p_single_file_header(FILE *file, int mask_mode, 
					   const char *id, struct varinfo *v)
{
  char *varname=v->varname;
  char *folder=(v->folder)->varname;
  longword varsize=v->varsize;
  byte vartype=v->vartype;
  longword index;
  int i;
  char desc[43]="File received by tilp";

  fprintf(file, id);
  fprintf(file, "%c%c", 0x01, 0x00);
  for(i=0; i<strlen(folder); i++) fprintf(file, "%c", folder[i]);
  for(i=strlen(folder); i<8; i++) fprintf(file, "%c", '\0');
  for(i=0; i<40; i++) fprintf(file, "%c", desc[i]);
  fprintf(file, "%c%c", 0x01, 0x00);
  index=0x52;
  fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, 
	  (index & 0x00FF0000)>>16, (index & 0xFF000000)>>24);
  for(i=0; i<strlen(varname); i++) fprintf(file, "%c", varname[i]);
  for(i=strlen(varname); i<8; i++) fprintf(file, "%c", '\0');
  fprintf(file, "%c", vartype);
  if(mask_mode &  MODE_KEEP_ARCH_ATTRIB)
    fprintf(file, "%c", v->varlocked); // extended group file
  else
    fprintf(file, "%c", 0x00); // standard group file
  fprintf(file, "%c%c",  0x00, 0x00);
  index+=varsize+4+2; // 4 of nothing and 2 of block_size
  fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, 
	  (index & 0x00FF0000)>>16, (index & 0xFF000000)>>24);
  fprintf(file, "%c%c", 0xA5, 0x5A);

  return;
}


void generate_89_92_92p_group_file_header_from_varlist(FILE *file, 
						       int mask_mode, 
						       const char *id, 
						       struct varinfo *list,
						       int calc_type)
{
  longword index=0x52;
  int i;
  char desc[43]="Group received by tilp";
  char variable_name[9];
  char folder_name[9];
  char previous_folder[9];
  int j=0;
  long file_offset=0;
  word num_vars=0;
  struct varinfo *v;

  fprintf(file, id);
  fprintf(file, "%c%c", 0x01, 0x00);
  for(i=0; i<8; i++) fprintf(file, "%c", '\0');
  for(i=0; i<40; i++) fprintf(file, "%c", desc[i]);  
  /* This part counts the number of entries in the 89g table and the beginning of data part */
  strcpy(previous_folder, "");
  j=0;
  index=0x52;
  v=list;
  while(v != NULL)
    {
      if( (v->vartype == tixx_dir(calc_type)) || 
	  ((v->vartype) == tixx_flash(calc_type)) )
	{
	  v=v->next;
	  continue;
	}

      strcpy(folder_name, (v->folder)->varname);
      strcpy(variable_name, v->varname);
      if(strcmp(previous_folder, folder_name)) j++;
      j++;
      
      strcpy(previous_folder, (v->folder)->varname);
      v=v->next;
    }
  fprintf(file, "%c%c", LSB(j), MSB(j));
  j--;
  /* This part places entries in the 89g table */
  strcpy(previous_folder, "");
  index+=j*16;
  v=list; 
  while(v != NULL)
    {
      if( (v->vartype == tixx_dir(calc_type)) ||
	  ((v->vartype) == tixx_flash(calc_type)) )
	{
	  v=v->next;
	  continue;
	}

      strcpy(folder_name, (v->folder)->varname);
      strcpy(variable_name, v->varname);
      if(strcmp(previous_folder, folder_name))
	{
	  /* Folder entry */
	  fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, 
		  (index & 0x00FF0000)>>16, (index & 0xFF000000)>>24);
	  for(i=0; i<strlen(folder_name); i++) fprintf(file, "%c", folder_name[i]);
	  for(i=strlen(folder_name); i<8; i++) fprintf(file, "%c", '\0');
	  fprintf(file, "%c%c", tixx_dir(calc_type), 0x00);
	  if(file_offset != 0)
	    {
	      fseek(file, file_offset, SEEK_SET);
	      fprintf(file, "%c%c", LSB(num_vars), MSB(num_vars));
	      fseek(file, 0L, SEEK_END);
	      num_vars=0;
	    }
	  file_offset=ftell(file);
	  fprintf(file, "%c%c", 0x00, 0x00);
	}
      /* Varname entry */
      fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, 
	      (index & 0x00FF0000)>>16, (index & 0xFF000000)>>24);
      index+=v->varsize+6;
      for(i=0; i<strlen(variable_name); i++) fprintf(file, "%c", variable_name[i]);
      for(i=strlen(variable_name); i<8; i++) fprintf(file, "%c", '\0');
      fprintf(file, "%c", v->vartype);
      if(mask_mode &  MODE_KEEP_ARCH_ATTRIB)
	fprintf(file, "%c", v->varlocked); // extended group file
      else
	fprintf(file, "%c", 0x00); // standard group file

      fprintf(file, "%c%c", 0x00, 0x00);
      num_vars++;

      strcpy(previous_folder, folder_name);
      v=v->next;
    }
  fseek(file, file_offset, SEEK_SET);
  fprintf(file, "%c%c", LSB(num_vars), MSB(num_vars));
  fseek(file, 0L, SEEK_END);
  /* The last part */
  fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, 
	  (index & 0x00FF0000)>>16, (index & 0xFF000000)>>24);
  fprintf(file, "%c%c", 0xA5, 0x5A);

  return;
}


void generate_82_83_85_86_single_file_header(FILE *file, int mask_mode,
                                           const char *id, struct varinfo *v)
{
  return;
}


void generate_82_83_85_86_group_file_header_from_varlist(FILE *file,
                                                       int mask_mode,
                                                       const char *id,
                                                       struct varinfo *list,
                                                       int calc_type)
{
  return;
}

/*
 * Check whether a given variable exists in the directory list
 * - vi [in]: a linked list of vars/folders such as one provided by a dirlist
 * - vn [in]: the varname to search (local or full)
 */
int check_if_var_exist(VAR_INFO *vi, char *vn)
{
  VAR_INFO *ptr;
  char folder_name[9] = "";
  char variable_name[9] = "";
  int b = 0;
  char *s;
  int i;

  ptr=vi;
  ptr=ptr->next;
  while(ptr != NULL)
    {
      if(ptr->is_folder == FOLDER) { ptr=ptr->next; continue; }

      s = strchr(vn, '\\');
      if(s == NULL)
	{// no backslash -> just varname
	  if(!strcmp(ptr->varname, vn)) b=1;
	}
      else
	{// with backslash -> foldername
	  s++;
	  strcpy(variable_name, s);
	  i = strlen(vn) - strlen(s) - 1;
	  strncpy(folder_name, vn, i);
	  folder_name[i]='\0';
	  if(!strcmp(ptr->varname, variable_name) && 
	     !strcmp((ptr->folder)->varname, folder_name)) b=1;
	}
    
      ptr=ptr->next;
    }

  return (int)b;
}
