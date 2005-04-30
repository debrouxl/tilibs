/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifndef __TICABLES__
#define __TICABLES__

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

typedef struct 
{
	int		count;			// Number of bytes exchanged
	tiTIME	start;			// Time when transfer has begun
	tiTIME	current;		// Current time (free for use)
} TiDataRate;

typedef struct _Cable	TiCable;
typedef struct _Handle	TiHandle;

struct _Cable
{
	int				model;			// TiCableModel
	const char*		name;			// name like "SER"
	const char*		fullname;		// name like "BlackLink"
	const char*		description;	// name like "Serial cable"

	int (*prepare)	(TiHandle *);	// Detect and map I/O
	int (*probe)	(TiHandle *);	// Check if cable is present

	int (*open)		(TiHandle *);	// Open cable
	int (*close)	(TiHandle *);	// Close cable
	int (*reset)	(TiHandle *);	// Reset cable

	int (*send)		(TiHandle *, uint8_t);		// Send data
	int (*recv)		(TiHandle *, uint8_t *);	// Recv data
	int (*check)	(TiHandle *, int *);		// Check data arrival

	int (*set_d0)	(TiHandle *, int);	// Direct access to D0 wire
	int (*set_d1)	(TiHandle *, int);	// Direct access to D1 wire
	int (*get_d0)	(TiHandle *);
	int (*get_d1)	(TiHandle *);
};

struct _Handle
{
	TiCableModel	model;	// Cable model
	TiCablePort		port;	// Generic port
	int				timeout;// Timeout value for cables in 0.10 seconds
	int				delay;	// Time between 2 bits (home-made cables only)

	char *			device;	// The character device: COMx, ttySx, ...
	unsigned int	address;// I/O port base address

	const TiCable*	cable;	// Link cable
	TiDataRate		rate;	// Data rate during transfers

	void *			priv;	// Holding data

	int		open;			// Cable is open
	int		busy;			// Cable is busy
};

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

	TIEXPORT TiHandle* TICALL ticables_handle_new(TiCableModel, TiCablePort);
	TIEXPORT int       TICALL ticables_handle_del(TiHandle*);

	TIEXPORT int TICALL ticables_options_set_timeout(TiHandle*, int);
	TIEXPORT int TICALL ticables_options_set_delay(TiHandle*, int);

	TIEXPORT TiCableModel TICALL ticables_get_model(TiHandle*);
	TIEXPORT TiCablePort  TICALL ticables_get_port(TiHandle*);

	TIEXPORT int TICALL ticables_handle_show(TiHandle*);

	// link.c
	TIEXPORT int TICALL ticables_cable_open(TiHandle*);
	TIEXPORT int TICALL ticables_cable_close(TiHandle*);

	TIEXPORT int TICALL ticables_cable_send(TiHandle*, uint8_t *data, uint16_t len);
	TIEXPORT int TICALL ticables_cable_recv(TiHandle*, uint8_t *data, uint16_t len);

	TIEXPORT int TICALL ticables_cable_check(TiHandle*, TiCableStatus*);

	TIEXPORT int TICALL ticables_cable_set_d0(TiHandle*, int state);
	TIEXPORT int TICALL ticables_cable_set_d1(TiHandle*, int state);

	TIEXPORT int TICALL ticables_cable_get_d0(TiHandle*);
	TIEXPORT int TICALL ticables_cable_get_d1(TiHandle*);

	TIEXPORT int TICALL ticables_cable_progress(TiHandle*, int *count, int *msec);

	// error.c
	TIEXPORT int         TICALL ticables_error_get (int number, char **message);

	// type2str.c
	TIEXPORT const char *TICALL ticables_model_to_string(TiCableModel model);
	TIEXPORT TiCableModel TICALL ticables_string_to_model (const char *str);

	TIEXPORT const char *TICALL ticables_port_to_string(TiCablePort port);
	TIEXPORT TiCablePort TICALL ticables_string_to_port(const char *str);

	// probe.c
  
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
