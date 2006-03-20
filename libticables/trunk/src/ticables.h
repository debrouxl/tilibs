/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
#include "export1.h"
#include "timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCABLES_VERSION "0.0.7"
#else
# define LIBCABLES_VERSION VERSION
#endif

/* Types */

#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

#define ERROR_READ_TIMEOUT	4
#define ERROR_WRITE_TIMEOUT	6
#define OPEN_DELAYED
#define SLV_RESET

/**
 * CableModel:
 *
 * An enumeration which contains the following cable types:
 **/
typedef enum 
{
	CABLE_NUL = 0,
	CABLE_GRY, CABLE_BLK, CABLE_PAR, CABLE_SLV, CABLE_USB,
	CABLE_VTI, CABLE_TIE, CABLE_VTL, CABLE_ILP, CABLE_MAX,
} CableModel;

/**
 * CablePort:
 *
 * An enumeration which contains the following ports:
 **/
typedef enum 
{
	PORT_0 = 0,
	PORT_1, PORT_2, PORT_3, PORT_4,
} CablePort;

/**
 * CableStatus:
 *
 * An enumeration which contains the following values:
 **/
typedef enum 
{
	STATUS_NONE = 0, 
	STATUS_RX = 1, 
	STATUS_TX = 2,
} CableStatus;

/**
 * DataRate:
 * @count: number of bytes transferred
 * @start: the time when transfer started
 * @current: free of use
 * @stop: the time when transfer finished
 *
 * A structure used for benchmarks.
 * !!! This structure is for private use !!!
 **/
typedef struct 
{
	int		count;
	tiTIME	start;
	tiTIME	current;
	tiTIME	stop;
} DataRate;

typedef struct _CableFncts	 CableFncts;
typedef struct _CableHandle  CableHandle;

/**
 * Cable:
 * @model: link cable model (CableModel).
 * @name: name of cable like "SER"
 * @fullname: complete name of cable like "BlackLink"
 * @description: description of cable like "serial cable"
 * @need_open: set if cable need to be 'open'/'close' before calling 'probe'
 * @prepare: detect and map I/O
 * @probe: check for cable presence
 * @timeout: used to update timeout
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
 * !!! This structure is for private use !!! 
 **/
struct _CableFncts
{
	const int		model;			
	const char*		name;			
	const char*		fullname;		
	const char*		description;	

	const int		need_open;		

	int (*prepare)	(CableHandle *);

	int (*open)		(CableHandle *);
	int (*close)	(CableHandle *);
	int (*reset)	(CableHandle *);
	int (*probe)	(CableHandle *);
	int (*timeout)	(CableHandle *);

	int (*send)		(CableHandle *, uint8_t *, uint32_t);
	int (*recv)		(CableHandle *, uint8_t *, uint32_t);
	int (*check)	(CableHandle *, int *);

	int (*set_d0)	(CableHandle *, int);
	int (*set_d1)	(CableHandle *, int);
	int (*get_d0)	(CableHandle *);
	int (*get_d1)	(CableHandle *);
};

/**
 * CableHandle:
 * @model: cable model
 * @port: generic port
 * @timeout: timeout value in 0.1 seconds
 * @delay: inter-bit delay for serial/parallel cable in µs
 * @device: device name like COMx or ttySx (if used)
 * @address: I/O base address of device (if used)
 * @cable: a Cable structure used by this handle
 * @rate: data rate during transfers
 * @priv: opaque data for internal/private use (static)
 * @priv2: idem (allocated)
 * @priv3: idem (static)
 * @open: set if cable has been open
 * @busy: set if cable is busy
 *
 * A structure used to store informations as an handle.
 * !!! This structure is for private use !!!
 **/
struct _CableHandle
{
	CableModel		model;	
	CablePort		port;	
	unsigned int	timeout;
	unsigned int	delay;	

	char *			device;	
	unsigned int	address;

	CableFncts*		cable;	
	DataRate		rate;	

	void*			priv;	
	void*			priv2;	
	void*			priv3;	

	int				open;	
	int				busy;	
};

/**
 * CableOptions:
 * @cable_model: model
 * @cable_port: port
 * @cable_timeout: timeout in tenth of seconds
 * @cable_delay: inter-bit delay in µs
 * @calc_model: calculator model
 *
 * A convenient structure free of use by the user.
 **/
typedef struct
{
    CableModel      model;
    CablePort       port;
    int             timeout;
    int             delay;

    int             calc; // unused

} CableOptions;

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

	TIEXPORT CableHandle* TICALL ticables_handle_new(CableModel, CablePort);
	TIEXPORT int        TICALL ticables_handle_del(CableHandle*);

	TIEXPORT int TICALL ticables_options_set_timeout(CableHandle*, int);
	TIEXPORT int TICALL ticables_options_set_delay(CableHandle*, int);

	TIEXPORT CableModel TICALL ticables_get_model(CableHandle*);
	TIEXPORT CablePort  TICALL ticables_get_port(CableHandle*);

	TIEXPORT int TICALL ticables_handle_show(CableHandle*);

	// link_xxx.c
	TIEXPORT int TICALL ticables_cable_open(CableHandle*);
	TIEXPORT int TICALL ticables_cable_close(CableHandle*);

	TIEXPORT int TICALL ticables_cable_reset(CableHandle* handle);
	TIEXPORT int TICALL ticables_cable_probe(CableHandle*, int* result);

	TIEXPORT int TICALL ticables_cable_send(CableHandle*, uint8_t *data, uint32_t len);
	TIEXPORT int TICALL ticables_cable_recv(CableHandle*, uint8_t *data, uint32_t len);

	TIEXPORT int TICALL ticables_cable_check(CableHandle*, CableStatus*);

	TIEXPORT int TICALL ticables_cable_set_d0(CableHandle*, int state);
	TIEXPORT int TICALL ticables_cable_set_d1(CableHandle*, int state);

	TIEXPORT int TICALL ticables_cable_get_d0(CableHandle*);
	TIEXPORT int TICALL ticables_cable_get_d1(CableHandle*);

	TIEXPORT int TICALL ticables_progress_reset(CableHandle*);
	TIEXPORT int TICALL ticables_progress_get(CableHandle*, int* count, int* msec, float* rate);

	TIEXPORT int TICALL ticables_cable_put(CableHandle*, uint8_t data);
	TIEXPORT int TICALL ticables_cable_get(CableHandle*, uint8_t *data);

	// error.c
	TIEXPORT int         TICALL ticables_error_get (int number, char **message);

	// type2str.c
	TIEXPORT const char* TICALL ticables_model_to_string(CableModel model);
	TIEXPORT CableModel  TICALL ticables_string_to_model (const char *str);

	TIEXPORT const char* TICALL ticables_port_to_string(CablePort port);
	TIEXPORT CablePort   TICALL ticables_string_to_port(const char *str);

	// probe.c
	TIEXPORT int TICALL ticables_probing_do(int ***result, int);
	TIEXPORT int TICALL ticables_probing_finish(int ***result);
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef TICABLES_DEPRECATED
#endif

#ifdef __cplusplus
}
#endif

#endif
