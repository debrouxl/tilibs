/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#ifndef __TICABLE_INTERFACE__
#define __TICABLE_INTERFACE__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# include <inttypes.h>
#endif

#include "export.h"
#include "cabl_def.h"

#ifdef __cplusplus
extern "C" {
#endif

	/****************/
  	/* Entry points */
	/****************/

  TIEXPORT int TICALL ticable_init(void);
  TIEXPORT int TICALL ticable_exit(void);

	/*********************/
  	/* General functions */
	/*********************/

  TIEXPORT const char *TICALL ticable_get_version(void);

  TIEXPORT int TICALL ticable_get_error(int err_num, char *error_msg);

  TIEXPORT int TICALL ticable_get_default_param(TicableLinkParam * lp);
  TIEXPORT int TICALL ticable_set_param(const TicableLinkParam * lp);
  TIEXPORT int TICALL ticable_get_param(TicableLinkParam * lp);

  TIEXPORT int TICALL ticable_set_cable(TicableType type,
					TicableLinkCable * lc);

  TIEXPORT int TICALL ticable_get_support(TicableType cable_type);

  TIEXPORT int TICALL ticable_get_datarate(TicableDataRate ** ptdr);

	/*********************/
  	/* Set/Get functions */
	/*********************/

  TIEXPORT void TICALL ticable_set_timeout(int timeout_v);
  TIEXPORT int TICALL ticable_get_timeout(void);

  TIEXPORT void TICALL ticable_set_delay(int delay_v);
  TIEXPORT int TICALL ticable_get_delay(void);

  TIEXPORT void TICALL ticable_set_baudrate(TicableBaudRate br);
  TIEXPORT int TICALL ticable_get_baudrate(void);

  TIEXPORT void TICALL ticable_set_io_address(unsigned int io_addr);
  TIEXPORT unsigned int TICALL ticable_get_io_address(void);

  TIEXPORT void TICALL ticable_set_io_device(char *dev);
  TIEXPORT char *TICALL ticable_get_io_device(void);

  TIEXPORT void TICALL ticable_set_hfc(TicableHfc hfc);
  TIEXPORT int TICALL ticable_get_hfc(void);

  TIEXPORT void TICALL ticable_set_port(TicablePort port);
  TIEXPORT int TICALL ticable_get_port(void);

  TIEXPORT void TICALL ticable_set_method(TicableMethod method);
  TIEXPORT int TICALL ticable_get_method(void);

	/*******************/
  	/* Probe functions */
	/*******************/

  TIEXPORT int TICALL ticable_detect_os(char **os_type);
  TIEXPORT int TICALL ticable_detect_port(TicablePortInfo * pi);
  TIEXPORT int TICALL ticable_detect_cable(TicablePortInfo * pi);
  TIEXPORT int TICALL ticable_detect_all(char **os, TicablePortInfo * pi);

	/***********************/
  	/* Verbosity functions */
	/***********************/

  TIEXPORT int TICALL DISPLAY(const char *format, ...);
  TIEXPORT int TICALL DISPLAY_ERROR(const char *format, ...);
  TIEXPORT int TICALL ticable_verbose_settings(TicableDisplay);
  TIEXPORT int TICALL ticable_verbose_set_file(const char *filename);
  TIEXPORT int TICALL ticable_verbose_flush_file(void);

	/**************************************/
  	/* Direct access functions (reserved) */
	/**************************************/

  TIEXPORT int TICALL ticable_link_init(void);
  TIEXPORT int TICALL ticable_link_open(void);
  TIEXPORT int TICALL ticable_link_put(uint8_t data);
  TIEXPORT int TICALL ticable_link_get(uint8_t * data);
  TIEXPORT int TICALL ticable_link_probe(void);
  TIEXPORT int TICALL ticable_link_close(void);
  TIEXPORT int TICALL ticable_link_exit(void);
  TIEXPORT int TICALL ticable_link_check(int *status);

  TIEXPORT int TICALL ticable_link_set_red(int w);
  TIEXPORT int TICALL ticable_link_set_white(int w);
  TIEXPORT int TICALL ticable_link_get_red(void);
  TIEXPORT int TICALL ticable_link_get_white(void);

  TIEXPORT int TICALL ticable_link_set_cable(TicableType type);

	/************************/
  	/* Deprecated functions */
	/************************/

  TIEXPORT int TICALL ticable_set_param2(TicableLinkParam lp);
  TIEXPORT int TICALL ticable_DISPLAY_settings(TicableDisplay);

	/****************************/
  	/* Type to String functions */
	/****************************/

  TIEXPORT const char *TICALL ticable_cabletype_to_string(TicableType
							  type);
  TIEXPORT const char *TICALL ticable_baudrate_to_string(TicableBaudRate
							 br);
  TIEXPORT const char *TICALL ticable_hfc_to_string(TicableHfc hfc);
  TIEXPORT const char *TICALL ticable_port_to_string(TicablePort port);
  TIEXPORT const char *TICALL ticable_method_to_string(TicableMethod
						       method);

#ifdef __cplusplus
}
#endif
#endif
