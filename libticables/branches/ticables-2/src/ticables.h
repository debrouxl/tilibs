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

#ifndef __TICABLE_DEFS__
#define __TICABLE_DEFS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdints.h"
#include "export.h"
#include "timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBTICABLES_VERSION "0.0.1"
#else
# define LIBTICABLES_VERSION VERSION
#endif

/* Types */

#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

typedef enum 
{
	CABLE_NUL = 0,
	CABLE_TGL, CABLE_SER, CABLE_PAR, CABLE_SLV,
	CABLE_VTL, CABLE_TIE, CABLE_VTI,
} TiCableModel;

/* Ports */
typedef enum 
{
	PORT_0 = 0,
	PORT_1, PORT_2, PORT_3, PORT_4,
} TiCablePort;

typedef enum 
{
	STATUS_NONE = 0, 
	STATUS_RX, STATUS_TX,
} TiCableStatus;



// namespace scheme: library_class_function like ticables_fext_get

	/****************/
	/* Entry points */
	/****************/
  
	TIEXPORT int TICALL ticables_library_init(void);
	TIEXPORT int TICALL ticables_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// ticables.c
	TIEXPORT const char* TICALL ticables_version_get (void);

	TIEXPORT int TICALL ticables_connection_new(void);
	TIEXPORT int TICALL ticables_connection_del(int handle);

	TIEXPORT int          TICALL ticables_model_set(int handle, TiCableModel model);
	TIEXPORT TiCableModel TICALL ticables_model_get(int handle);
	TIEXPORT int		  TICALL ticables_port_set(int handle, TiCablePort port);
	TIEXPORT TiCablePort  TICALL ticables_port_get(int handle);

	// link.c ??
	TIEXPORT int TICALL ticables_cable_open(int handle);
	TIEXPORT int TICALL ticables_cable_close(int handle);

	TIEXPORT int TICALL ticables_cable_send(int handle, uint8_t *data, uint16_t len);
	TIEXPORT int TICALL ticables_cable_recv(int handle, uint8_t *data, uint16_t len);

	TIEXPORT int TICALL ticables_cable_check(int handle, uint8_t *data);

	TIEXPORT int TICALL ticables_cable_reset(int handle);

	TIEXPORT int TICALL ticables_cable_probe(int handle);

	TIEXPORT int TICALL ticables_cable_set_d0(int handle, int state);
	TIEXPORT int TICALL ticables_cable_set_d1(int handle, int state);

	TIEXPORT int TICALL ticables_cable_get_d0(int handle);
	TIEXPORT int TICALL ticables_cable_get_d1(int handle);

	TIEXPORT int TICALL ticables_cable_progress(int handle, int *count, int *msec);

	// error.c
	TIEXPORT int         TICALL ticables_error_get (int number, char **message);

	// type2str.c
	TIEXPORT const char *TICALL ticables_model_to_string(TiCableModel model);
	TIEXPORT TiCableModel TICALL ticables_string_to_model (const char *str);

	TIEXPORT const char *TICALL ticables_port_to_string(TiCablePort port);
	TIEXPORT TiCablePort TICALL ticables_string_to_port(const char *str);

	//TIEXPORT const char *TICALL ticables_method_to_string(TiCableMethod method);

	//TIEXPORT const char *TICALL ticables_os_to_string(TiCableOs port);
	//TIEXPORT TiCableOs TICALL ticables_string_to_os(const char *str);

	// probe.c
  
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
