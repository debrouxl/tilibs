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

#ifndef GROUP_H
#define GROUP_H

#include <stdio.h>
#include "calc_ext.h"

// 89, 92, 92+
void generate_89_92_92p_single_file_header(FILE *file, int mask_mode, 
					   const char *id, TicalcVarInfo *v);
void generate_89_92_92p_group_file_header_from_varlist(FILE *file, 
						       int mask_mode, 
						       const char *id, 
						       TicalcVarInfo *list, 
						       int calc_type);

// 82, 83, 85, 86
void generate_82_83_85_86_single_file_header(FILE *file, int mask_mode,
                                           const char *id, TicalcVarInfo *v);
void generate_82_83_85_86_group_file_header_from_varlist(FILE *file,
                                                       int mask_mode,
                                                       const char *id,
                                                       TicalcVarInfo *list,
                                                       int calc_type);

int check_if_var_exist(TicalcVarInfo *vi, char *vn);

#endif




