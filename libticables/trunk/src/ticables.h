/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TICABLES__
#define __TICABLES__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdints1.h"
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
# define libticables2_VERSION "1.3.5"
#else
# define libticables2_VERSION VERSION
#endif

/* Types */

#define DFLT_TIMEOUT  15	/* 1.5 second */
#define DFLT_DELAY    10	/* 10 micro-seconds */

#define ERROR_READ_TIMEOUT	4	/* fixed in error.h */
#define ERROR_WRITE_TIMEOUT	6	/* fixed in error.h */

/**
 * CableModel:
 *
 * An enumeration which contains the following cable types:
 **/
typedef enum 
{
	CABLE_NUL = 0,
	CABLE_GRY, CABLE_BLK, CABLE_PAR, CABLE_SLV, CABLE_USB,
	CABLE_VTI, CABLE_TIE, CABLE_ILP, CABLE_DEV,
	CABLE_TCPC, CABLE_TCPS,
	CABLE_MAX
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
 * UsbPid:
 *
 * An enumeration which contains the following devices:
 **/
typedef enum 
{
	PID_UNKNOWN		= 0,
	PID_TIGLUSB     = 0xE001,
	PID_TI89TM		= 0xE004,
	PID_TI84P		= 0xE003,
	PID_TI84P_SE    = 0xE008,
	PID_NSPIRE      = 0xE012,
} UsbPid;

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
 * ProbingMethod:
 *
 * Defines how to probe cables:
 **/
typedef enum
{
	PROBE_NONE	= 0,
	PROBE_FIRST = 1,
	PROBE_USB	= 2,
	PROBE_DBUS	= 4,
	PROBE_ALL	= 6,
} ProbingMethod;

/**
 * CableFamily:
 *
 * Defines the various types of devices supported and which can be
 * detected automatically; each family is generally incompatible with
 * the others in terms of protocol, file formats, or both.
 */
typedef enum
{
	CABLE_FAMILY_UNKNOWN = 0,
	CABLE_FAMILY_DBUS,          /* Traditional TI link protocol */
	CABLE_FAMILY_USB_TI8X,      /* Direct USB for TI-84 Plus, CSE, CE, etc. */
	CABLE_FAMILY_USB_TI9X,      /* Direct USB for TI-89 Titanium */
	CABLE_FAMILY_USB_NSPIRE     /* Direct USB for TI-Nspire series */
} CableFamily;

/**
 * CableVariant:
 *
 * Defines the various sub-types (for a given CableFamily) that are
 * supported and can be detected automatically.
 */
typedef enum
{
	CABLE_VARIANT_UNKNOWN = 0,
	CABLE_VARIANT_TIGLUSB,      /* CABLE_FAMILY_DBUS */
	CABLE_VARIANT_TI84P,        /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI84PSE,      /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI84PCSE,     /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI84PCE,      /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI83PCE,      /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI82A,        /* CABLE_FAMILY_USB_TI8X */
	CABLE_VARIANT_TI89TM,       /* CABLE_FAMILY_USB_TI9X */
	CABLE_VARIANT_NSPIRE        /* CABLE_FAMILY_USB_NSPIRE */
} CableVariant;

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
 * @set_raw: set both wires
 * @get_raw: read both wires
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
	int (*set_raw)  (CableHandle *, int);
	int (*get_raw)  (CableHandle *, int *);
	int (*set_device) (CableHandle*, const char*);
};

typedef int (*ticables_pre_send_hook_type)(CableHandle * handle, uint8_t * data, uint32_t len);
typedef int (*ticables_post_send_hook_type)(CableHandle * handle, uint8_t * data, uint32_t len, int retval);
typedef int (*ticables_pre_recv_hook_type)(CableHandle * handle, uint8_t * data, uint32_t len);
typedef int (*ticables_post_recv_hook_type)(CableHandle * handle, uint8_t * data, uint32_t len, int retval);

/**
 * CableHandle:
 * @model: cable model
 * @port: generic port
 * @timeout: timeout value in 0.1 seconds
 * @delay: inter-bit delay for serial/parallel cable in us
 * @device: device name like COMx or ttySx (if used)
 * @address: I/O base address of device (if used)
 * @cable: a Cable structure used by this handle
 * @rate: data rate during transfers
 * @priv: opaque data for internal/private use (static)
 * @priv2: idem (allocated)
 * @priv3: idem (static)
 * @open: set if cable has been open
 * @busy: set if cable is busy
 * @pre_send_hook: callback fired before sending a block of data
 * @post_send_hook: callback fired after sending a block of data.
 * @pre_recv_hook: callback fired before receiving a block of data
 * @post_recv_hook: callback fired after receiving a block of data.
 *
 * A structure used to store information as an handle.
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

	ticables_pre_send_hook_type pre_send_hook;
	ticables_post_send_hook_type post_send_hook;
	ticables_pre_recv_hook_type pre_recv_hook;
	ticables_post_recv_hook_type post_recv_hook;
};

/**
 * CableDeviceInfo:
 * @family: calculator family
 * @variant: calculator variant
 *
 * Information returned for each cable by ticables_get_usb_device_info.
 */
typedef struct
{
	CableFamily     family;
	CableVariant    variant;
} CableDeviceInfo;

/**
 * CableOptions:
 * @cable_model: model
 * @cable_port: port
 * @cable_timeout: timeout in tenth of seconds
 * @cable_delay: inter-bit delay in us
 * @calc_model: calculator model
 *
 * A convenient structure free of use by the user.
 **/
typedef struct
{
	CableModel      model;
	CablePort       port;
	unsigned int    timeout;
	unsigned int    delay;

	int             calc; // unused
} CableOptions;

// namespace scheme: library_class_function like ticables_fext_get

	/****************/
	/* Entry points */
	/****************/

	TIEXPORT1 int TICALL ticables_library_init(void);
	TIEXPORT1 int TICALL ticables_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// ticables.c
	TIEXPORT1 const char * TICALL ticables_version_get(void);
	TIEXPORT1 uint32_t TICALL ticables_supported_cables(void);

	TIEXPORT1 CableHandle * TICALL ticables_handle_new(CableModel model, CablePort port);
	TIEXPORT1 int        TICALL ticables_handle_del(CableHandle *handle);

	TIEXPORT1 unsigned int TICALL ticables_options_set_timeout(CableHandle *handle, unsigned int timeout);
	TIEXPORT1 unsigned int TICALL ticables_options_set_delay(CableHandle *handle, unsigned int delay);

	TIEXPORT1 CableModel TICALL ticables_get_model(CableHandle *handle);
	TIEXPORT1 CablePort TICALL ticables_get_port(CableHandle *handle);
	TIEXPORT1 const char * TICALL ticables_get_device(CableHandle *handle);
	TIEXPORT1 unsigned int TICALL ticables_get_timeout(CableHandle *handle);
	TIEXPORT1 unsigned int TICALL ticables_get_delay(CableHandle *handle);

	TIEXPORT1 int TICALL ticables_handle_show(CableHandle *handle);

	// error.c
	TIEXPORT1 int         TICALL ticables_error_get(int number, char **message);
	TIEXPORT1 int         TICALL ticables_error_free(char *message);

	// link_xxx.c
	TIEXPORT1 int TICALL ticables_cable_open(CableHandle *handle);
	TIEXPORT1 int TICALL ticables_cable_close(CableHandle *handle);

	TIEXPORT1 int TICALL ticables_cable_reset(CableHandle *handle);
	TIEXPORT1 int TICALL ticables_cable_probe(CableHandle *handle, int *result);

	TIEXPORT1 int TICALL ticables_cable_send(CableHandle *handle, uint8_t *data, uint32_t len);
	TIEXPORT1 int TICALL ticables_cable_recv(CableHandle *handle, uint8_t *data, uint32_t len);

	TIEXPORT1 int TICALL ticables_cable_check(CableHandle *handle, CableStatus *status);

	TIEXPORT1 int TICALL ticables_cable_set_d0(CableHandle *handle, int state);
	TIEXPORT1 int TICALL ticables_cable_set_d1(CableHandle *handle, int state);

	TIEXPORT1 int TICALL ticables_cable_get_d0(CableHandle *handle);
	TIEXPORT1 int TICALL ticables_cable_get_d1(CableHandle *handle);

	TIEXPORT1 int TICALL ticables_cable_set_raw(CableHandle *handle, int state);
	TIEXPORT1 int TICALL ticables_cable_get_raw(CableHandle *handle, int *state);

	TIEXPORT1 int TICALL ticables_cable_set_device(CableHandle *handle, const char *device);


	TIEXPORT1 int TICALL ticables_progress_reset(CableHandle *handle);
	TIEXPORT1 int TICALL ticables_progress_get(CableHandle *handle, int *count, int *msec, float *rate);

	TIEXPORT1 int TICALL ticables_cable_put(CableHandle *handle, uint8_t data);
	TIEXPORT1 int TICALL ticables_cable_get(CableHandle *handle, uint8_t *data);

	TIEXPORT1 ticables_pre_send_hook_type TICALL ticables_cable_get_pre_send_hook(CableHandle *handle);
	TIEXPORT1 ticables_pre_send_hook_type TICALL ticables_cable_set_pre_send_hook(CableHandle *handle, ticables_pre_send_hook_type hook);
	TIEXPORT1 ticables_post_send_hook_type TICALL ticables_cable_get_post_send_hook(CableHandle *handle);
	TIEXPORT1 ticables_post_send_hook_type TICALL ticables_cable_set_post_send_hook(CableHandle *handle, ticables_post_send_hook_type hook);

	TIEXPORT1 ticables_pre_recv_hook_type TICALL ticables_cable_get_pre_recv_hook(CableHandle *handle);
	TIEXPORT1 ticables_pre_recv_hook_type TICALL ticables_cable_set_pre_recv_hook(CableHandle *handle, ticables_pre_recv_hook_type hook);
	TIEXPORT1 ticables_post_recv_hook_type TICALL ticables_cable_get_post_recv_hook(CableHandle *handle);
	TIEXPORT1 ticables_post_recv_hook_type TICALL ticables_cable_set_post_recv_hook(CableHandle *handle, ticables_post_recv_hook_type hook);

	// type2str.c
	TIEXPORT1 const char * TICALL ticables_model_to_string(CableModel model);
	TIEXPORT1 CableModel   TICALL ticables_string_to_model (const char *str);

	TIEXPORT1 const char * TICALL ticables_port_to_string(CablePort port);
	TIEXPORT1 CablePort    TICALL ticables_string_to_port(const char *str);

	TIEXPORT1 const char * TICALL ticables_usbpid_to_string(UsbPid pid);
	TIEXPORT1 UsbPid       TICALL ticables_string_to_usbpid(const char *str);

	// probe.c
	TIEXPORT1 int TICALL ticables_probing_do(int ***result, unsigned int timeout, ProbingMethod method);
	TIEXPORT1 int TICALL ticables_probing_finish(int ***result);
	TIEXPORT1 void TICALL ticables_probing_show(int **array);

	TIEXPORT1 int TICALL ticables_is_usb_enabled(void);

	TIEXPORT1 int TICALL ticables_get_usb_device_info(CableDeviceInfo **array, int *len);
	TIEXPORT1 int TICALL ticables_free_usb_device_info(CableDeviceInfo *array);

	TIEXPORT1 int TICALL ticables_get_usb_devices(int **array, int *len);
	TIEXPORT1 int TICALL ticables_free_usb_devices(int *array);

	/************************/
	/* Deprecated functions */
	/************************/

#ifdef __cplusplus
}
#endif

#endif
