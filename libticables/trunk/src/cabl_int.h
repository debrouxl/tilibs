/*  ti_link - link program for TI calculators
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

#ifndef __CABLE_INTERFACE__
#define __CABLE_INTERFACE__

#include "cabl_def.h"

#ifdef __cplusplus
extern "C" {
#endif

  /*********************/  
  /* General functions */
  /*********************/
  const char *TIEXPORT ticable_get_version(void);
  
  int   TIEXPORT ticable_get_error(int err_num, char *error_msg);
  void  TIEXPORT ticable_set_param(const LinkParam *lp);
  int   TIEXPORT ticable_get_param(LinkParam *lp);
  int   TIEXPORT ticable_get_default_param(LinkParam *lp);
  int   TIEXPORT ticable_get_support(int cable_type);
  void  TIEXPORT ticable_set_cable(int type, LinkCable *lc);
  
  /*********************/
  /* Set/Get functions */
  /*********************/
  void  TIEXPORT ticable_set_timeout(int timeout_v);
  int   TIEXPORT ticable_get_timeout(void);

  void  TIEXPORT ticable_set_delay(int delay_v);
  int   TIEXPORT ticable_get_delayvoid();

  void  TIEXPORT ticable_set_baudrate(int br);
  int   TIEXPORT ticable_get_baudrate(void);

  void  TIEXPORT ticable_set_io_address(uint io_addr);
  uint  TIEXPORT ticable_get_io_address(void);

  void  TIEXPORT ticable_set_io_device(char *dev);
  char *TIEXPORT ticable_get_io_device(void);

  void  TIEXPORT ticable_set_hfc(int action);
  int   TIEXPORT ticable_get_hfc(void);

  void  TIEXPORT ticable_set_port(int port);
  int   TIEXPORT ticable_get_port(void);

  void  TIEXPORT ticable_set_method(int method);
  int   TIEXPORT ticable_get_method(void);

  /*******************/
  /* Probe functions */
  /*******************/
  int TIEXPORT ticable_detect_os(char **os_type);
  int TIEXPORT ticable_detect_port(PortInfo *pi);
  int TIEXPORT ticable_detect_cable(PortInfo *pi);

  /***********************/
  /* Verbosity functions */
  /***********************/
  int TIEXPORT DISPLAY(const char *format, ...);
  int TIEXPORT ticable_DISPLAY_settings(int op);
  
#ifdef __cplusplus
}
#endif

#endif

