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

#include "calc_def.h"

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
  
  // intrface.c
  const char TICALL *ticalc_get_version();
  
  int        TICALL ticalc_get_error(int err_num, char *error_msg);
  
  void       TICALL ticalc_set_update(TicalcInfoUpdate *iu,
				      void (*start)   (void),
				      void (*stop)    (void),
				      void (*refresh) (void),
				      void (*pbar)    (void),
				      void (*label)   (void));
       
  void TICALL ticalc_set_cable(TicableLinkCable *lc);
  void TICALL ticalc_set_calc(int type, TicalcFncts *calc);  

  int  TICALL ticalc_return_calc(void);
  int  TICALL ticalc_get_calc(int *type);
  
  // probe.c
  int  TICALL ticalc_detect_calc(int *calc_type);
  int  TICALL ticalc_flash_isready(int *calc_type);         // preferred
#define ticalc_89_92_92p_isready     ticalc_flash_isready   // obsolete
#define ticalc_73_83p_89_92p_isready ticalc_flash_isready

  // dirlist.c
  void   TICALL ticalc_display_dirlist(TNode *tree);
  int    TICALL ticalc_number_of_vars(TNode *tree);
  int    TICALL ticalc_memory_used(TNode *tree);
  TiVarEntry* TICALL ticalc_check_if_var_exists(TNode *tree, char *varname);
  TiVarEntry* TICALL ticalc_check_if_app_exists(TNode *tree, char *appname);
  char** TICALL ticalc_create_action_array(int num_entries);
  void   TICALL ticalc_destroy_action_array(char **array);

  // tikeys.c
  const struct ti_key TICALL ticalc_73_keys (unsigned char ascii_code);
  const struct ti_key TICALL ticalc_83p_keys(unsigned char ascii_code);
  const struct ti_key TICALL ticalc_89_keys (unsigned char ascii_code);
  const struct ti_key TICALL ticalc_92p_keys(unsigned char ascii_code);

  // clock.c
  const char* TICALL ticalc_format_to_date(int value);
  int         TICALL ticalc_date_to_format(const char *format);

#ifdef __cplusplus
}
#endif

#endif




