/*  libticables - link cable library, a part of the TiLP project
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

#ifndef __CABLE_INTERFACE__
#define __CABLE_INTERFACE__

#include <stdio.h>

#include "cabl_def.h"
#include "export.h"
#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

  /****************/
  /* Entry points */
  /****************/
  int TICALL ticable_init(void);
  int TICALL ticable_exit(void);

  /*********************/  
  /* General functions */
  /*********************/

  const char* TICALL ticable_get_version(void);
  
  int  TICALL ticable_get_error(int err_num, char *error_msg);
  int  TICALL ticable_set_param2(LinkParam lp);
  int  TICALL ticable_set_param(const LinkParam *lp);
  int  TICALL ticable_get_param(LinkParam *lp);
  int  TICALL ticable_get_default_param(LinkParam *lp);
  int  TICALL ticable_get_support(int cable_type);
  int  TICALL ticable_set_cable(int type, LinkCable *lc);

  int  TICALL ticable_get_datarate(TicableDataRate **ptdr);
  
  /*********************/
  /* Set/Get functions */
  /*********************/
  void  TICALL ticable_set_timeout(int timeout_v);
  int   TICALL ticable_get_timeout(void);

  void  TICALL ticable_set_delay(int delay_v);
  int   TICALL ticable_get_delay(void);

  void  TICALL ticable_set_baudrate(int br);
  int   TICALL ticable_get_baudrate(void);

  void  TICALL ticable_set_io_address(uint io_addr);
  uint  TICALL ticable_get_io_address(void);

  void  TICALL ticable_set_io_device(char *dev);
  char* TICALL ticable_get_io_device(void);

  void  TICALL ticable_set_hfc(int action);
  int   TICALL ticable_get_hfc(void);

  void  TICALL ticable_set_port(int port);
  int   TICALL ticable_get_port(void);

  void  TICALL ticable_set_method(int method);
  int   TICALL ticable_get_method(void);

  /*******************/
  /* Probe functions */
  /*******************/
  int TICALL ticable_detect_os(char **os_type);
  int TICALL ticable_detect_port(PortInfo *pi);
  int TICALL ticable_detect_cable(PortInfo *pi);
  int TICALL ticable_detect_all(char **os, PortInfo *pi);

  /***********************/
  /* Verbosity functions */
  /***********************/
  int TICALL DISPLAY(const char *format, ...);
  int TICALL ticable_DISPLAY_settings(int op);
  int TICALL DISPLAY_ERROR(const char *format, ...);
  FILE* TICALL ticable_DISPLAY_set_output_to_stream(FILE *stream);
  FILE* TICALL ticable_DISPLAY_set_output_to_file(char *filename);
  int TICALL ticable_DISPLAY_close_file();

#ifdef __cplusplus
}
#endif

#endif

