/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
  TIEXPORT const char *TICALL ticalc_get_version();

  TIEXPORT int TICALL ticalc_get_error(int err_num, char *error_msg);

  TIEXPORT void TICALL ticalc_set_update(TicalcInfoUpdate * iu,
					 void (*start) (void),
					 void (*stop) (void),
					 void (*refresh) (void),
					 void (*pbar) (void),
					 void (*label) (void));

  TIEXPORT void TICALL ticalc_set_cable(TicableLinkCable * lc);
  TIEXPORT void TICALL ticalc_set_calc(TicalcType type,
				       TicalcFncts * calc);

  TIEXPORT int TICALL ticalc_return_calc(void);
  TIEXPORT int TICALL ticalc_get_calc(TicalcType * type);

  // probe.c
  TIEXPORT int TICALL ticalc_detect_calc(TicalcType * calc_type);
  TIEXPORT int TICALL ticalc_isready(TicalcType * calc_type);

  // dirlist.c
  TIEXPORT void TICALL ticalc_dirlist_destroy(TNode ** tree);
  TIEXPORT void TICALL ticalc_dirlist_display(TNode * tree);

  TIEXPORT int TICALL ticalc_dirlist_numvars(TNode * tree);
  TIEXPORT int TICALL ticalc_dirlist_memused(TNode * tree);
  TIEXPORT TiVarEntry *TICALL ticalc_check_if_var_exists(TNode * tree, char
							 *varname);
  TIEXPORT TiVarEntry *TICALL ticalc_check_if_app_exists(TNode * tree, char
							 *appname);
  TIEXPORT char **TICALL ticalc_action_create_array(int num_entries);
  TIEXPORT void TICALL ticalc_action_destroy_array(char **array);

  // tikeys.c
  TIEXPORT const TicalcKey TICALL ticalc_73_keys(unsigned char
						 ascii_code);
  TIEXPORT const TicalcKey TICALL ticalc_83p_keys(unsigned char
						  ascii_code);
  TIEXPORT const TicalcKey TICALL ticalc_89_keys(unsigned char
						 ascii_code);
  TIEXPORT const TicalcKey TICALL ticalc_92p_keys(unsigned char
						  ascii_code);

  // clock.c
  TIEXPORT const char *TICALL ticalc_clock_format2date(int value);
  TIEXPORT int TICALL ticalc_clock_date2format(const char *format);

  // type2str.c
  TIEXPORT const char *TICALL ticalc_screen_to_string(TicalcScreenFormat format);
  TIEXPORT TicalcScreenFormat TICALL ticalc_string_to_screen(const char *str);

  TIEXPORT const char *TICALL ticalc_path_to_string(TicalcPathType type);
  TIEXPORT TicalcPathType TICALL ticalc_string_to_path(const char *str);
  
  TIEXPORT const char *TICALL ticalc_action_to_string(TicalcAction action);
  
  // printl.c
  TIEXPORT TICALC_PRINTL TICALL ticalc_set_printl(TICALC_PRINTL new_printl);

	/**************/
	/* Deprecated */
	/**************/

#define ticalc_flash_isready         ticalc_isready
#define ticalc_89_92_92p_isready     ticalc_flash_isready
#define ticalc_73_83p_89_92p_isready ticalc_flash_isready

#define ticalc_display_dirlist ticalc_dirlist_display;
#define ticalc_number_of_vars  ticalc_dirlist_numvars;
#define ticalc_memory_used     ticalc_dirlist_memused;
#define ticalc_create_action_array  ticalc_action_create_array
#define ticalc_destroy_action_array ticalc_action_destroy_array

#define ticalc_format_to_date ticalc_clock_format2date
#define ticalc_date_to_format ticalc_clock_date2format

#ifdef __cplusplus
}
#endif
#endif
