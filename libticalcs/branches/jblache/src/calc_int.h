/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef __CALC_INTERFACE__
#define __CALC_INTERFACE__

#include <stdio.h>

#include "calc_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********/
/* Methods */
/***********/

const char *ticalc_get_version();

void ticalc_set_update(struct ticalc_info_update *iu,
		       void (*f1) (void),
		       void (*f2) (void),
		       void (*f3) (void),
		       void (*f4) (const char *t, char *s),
		       void (*f5) (void),
		       void (*f6) (void),
		       int  (*f7) (char *cur_name, char *new_name));

void ticalc_set_calc(int type, 
		     struct ticalc_fncts *calc, 
		     struct ticable_link *link);
int ticalc_get_calc(int *type);

int ticalc_get_error(int err_num, char *error_msg);

int detect_calc(int *calc_type);
int ti89_92_92p_isready(int *calc_type);

int ticalc_open_ti_file(char *filename, char *mode, FILE **fd);
int ticalc_close_ti_file();

#ifdef __cplusplus
}
#endif

#endif




