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

	#define LIBCALCS_REQUIRES_LIBFILES_VERSION  "0.5.8"	// useless with pkg-config
#define LIBCALCS_REQUIRES_LIBCABLES_VERSION "3.8.1"	// useless with pkg-config

#ifdef __WIN32__
# define LIBTICALCS_VERSION "4.5.9"
#else
# define LIBTICALCS_VERSION VERSION
#endif

/* Types */

#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

#define MAX_DESCRIPTORS	4	/* Maximum number of handles */

/**
 * TiCableModel:
 *
 * An enumeration which contains the following cable types:
 **/
typedef enum 
{
	CABLE_NUL = 0,
	CABLE_GRY, CABLE_BLK, CABLE_PAR, CABLE_SLV, CABLE_USB,
	CABLE_VTL, CABLE_TIE, CABLE_VTI,
} TiCableModel;

/**
 * TiCablePort:
 *
 * An enumeration which contains the following ports:
 **/
typedef enum 
{
	PORT_0 = 0,
	PORT_1, PORT_2, PORT_3, PORT_4,
} TiCablePort;

/**
 * TiCableStatus:
 *
 * An enumeration which contains the following values:
 **/
typedef enum 
{
	STATUS_NONE = 0, 
	STATUS_RX = 1, 
	STATUS_TX = 2,
} TiCableStatus;

/**
 * TiDataRate:
 * @count: number of bytes transferred
 * @start: the time when transfer started
 * @current: free of use
 * @stop: the time when transfer finished
 *
 * A structure used for benchmarks.
 * This structure is for private use !
 **/
typedef struct 
{
	int		count;
	tiTIME	start;
	tiTIME	current;
	tiTIME	stop;
} TiDataRate;

typedef struct _Cable	TiCable;
typedef struct _Handle TiCblHandle;

/**
 * TiCable:
 * @model: link cable model (TiCableModel).
 * @name: name of cable like "SER"
 * @fullname: complete name of cable like "BlackLink"
 * @description: description of cable like "serial cable"
 * @need_open: set if cable need to be 'open'/'close' before calling 'probe'
 * @prepare: detect and map I/O
 * @probe: check for cable presence
 * @open: open I/O port or device
 * @close: close I/O port or device
 * @reset: reset I/O port or device
 * @send: send data onto the cable
 * @recv: recv data from cable
 * @check: check for data arrival
 * @set_d0: set D0/red wire
 * @set_d1: set D1/white wire
 * @get_d0 get D0/red wire
 * @get_d1 set D1/red wire
 *
 * A structure used for handling a link cable.
 * This structure is for private use !
 **/
struct _Cable
{
	const int		model;			
	const char*		name;			
	const char*		fullname;		
	const char*		description;	

	const int		need_open;		

	int (*prepare)	(TiCblHandle *);

	int (*open)		(TiCblHandle *);
	int (*close)	(TiCblHandle *);
	int (*reset)	(TiCblHandle *);
	int (*probe)	(TiCblHandle *);

	int (*send)		(TiCblHandle *, uint8_t *, uint16_t);
	int (*recv)		(TiCblHandle *, uint8_t *, uint16_t);
	int (*check)	(TiCblHandle *, int *);

	int (*set_d0)	(TiCblHandle *, int);
	int (*set_d1)	(TiCblHandle *, int);
	int (*get_d0)	(TiCblHandle *);
	int (*get_d1)	(TiCblHandle *);
};

/**
 * TiCblHandle:
 * @model: cable model
 * @port: generic port
 * @timeout: timeout value in 0.1 seconds
 * @delay: inter-bit delay for serial/parallel cable in µs
 * @device: device name like COMx or ttySx (if used)
 * @address: I/O base address of device (if used)
 * @cable: a TiCable structure used by this handle
 * @rate: data rate during transfers
 * @priv: opaque data for internal/private use (static)
 * @priv2: idem (allocated)
 * @priv3: idem (static)
 * @open: set if cable has been open
 * @busy: set if cable is busy
 *
 * A structure used to store informations as an handle.
 * This structure is for private use !
 **/
struct _Handle
{
	TiCableModel	model;	
	TiCablePort		port;	
	int				timeout;
	int				delay;	

	char *			device;	
	unsigned int	address;

	const TiCable*	cable;	
	TiDataRate		rate;	

	void*			priv;	
	void*			priv2;	
	void*			priv3;	

	int				open;	
	int				busy;	
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

	TIEXPORT TiCblHandle* TICALL ticables_handle_new(TiCableModel, TiCablePort);
	TIEXPORT int       TICALL ticables_handle_del(TiCblHandle*);

	TIEXPORT int TICALL ticables_options_set_timeout(TiCblHandle*, int);
	TIEXPORT int TICALL ticables_options_set_delay(TiCblHandle*, int);

	TIEXPORT TiCableModel TICALL ticables_get_model(TiCblHandle*);
	TIEXPORT TiCablePort  TICALL ticables_get_port(TiCblHandle*);

	TIEXPORT int TICALL ticables_handle_show(TiCblHandle*);

	// link.c
	TIEXPORT int TICALL ticables_cable_open(TiCblHandle*);
	TIEXPORT int TICALL ticables_cable_close(TiCblHandle*);

	TIEXPORT int TICALL ticables_cable_probe(TiCblHandle*, unsigned int* result);

	TIEXPORT int TICALL ticables_cable_send(TiCblHandle*, uint8_t *data, uint16_t len);
	TIEXPORT int TICALL ticables_cable_recv(TiCblHandle*, uint8_t *data, uint16_t len);

	TIEXPORT int TICALL ticables_cable_check(TiCblHandle*, TiCableStatus*);

	TIEXPORT int TICALL ticables_cable_set_d0(TiCblHandle*, int state);
	TIEXPORT int TICALL ticables_cable_set_d1(TiCblHandle*, int state);

	TIEXPORT int TICALL ticables_cable_get_d0(TiCblHandle*);
	TIEXPORT int TICALL ticables_cable_get_d1(TiCblHandle*);

	TIEXPORT int TICALL ticables_cable_progress(TiCblHandle*, int *count, int *msec);

	// error.c
	TIEXPORT int         TICALL ticables_error_get (int number, char **message);

	// type2str.c
	TIEXPORT const char *TICALL ticables_model_to_string(TiCableModel model);
	TIEXPORT TiCableModel TICALL ticables_string_to_model (const char *str);

	TIEXPORT const char *TICALL ticables_port_to_string(TiCablePort port);
	TIEXPORT TiCablePort TICALL ticables_string_to_port(const char *str);
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
