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

#ifndef __CALC_INTERFACE__
#define __CALC_INTERFACE__

#include <stdio.h>

#include "calc_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  int TICALL ticalc_init(void);
  int TICALL ticalc_exit(void);
  
  /***********/
  /* Methods */
  /***********/
  
  const char TICALL *ticalc_get_version();
  
  void TICALL ticalc_set_update(struct ticalc_info_update *iu,
				void (*start)   (void),
				void (*stop)    (void),
				void (*refresh) (void),
				void (*msg_box) (const char *t, char *s),
				void (*pbar)    (void),
				void (*label)   (void),
				int  (*choose)  (char *cur_name, 
						 char *new_name));

  // obsolete, use the 2 functions below  
  void TICALL ticalc_set_calc(int type, 
			      TicalcFncts *calc, 
			      LinkCable *link);
  void TICALL ticalc_set_cable(LinkCable *lc);
  void TICALL ticalc_set_calc2(int type, TicalcFncts *calc);

  int TICALL ticalc_get_calc(void);
  
  int TICALL ticalc_get_error(int err_num, char *error_msg);
  
  int TICALL detect_calc(int *calc_type);
  int TICALL ti89_92_92p_isready(int *calc_type);
  #define ticalc_89_92_92p_isready	ti89_92_92p_isready
  int TICALL ticalc_83p_89_92p_isready(int *calc_type);
  
  int TICALL ticalc_open_ti_file(char *filename, char *mode, FILE **fd);
  int TICALL ticalc_close_ti_file();
  
#ifdef __cplusplus
}
#endif

#endif




