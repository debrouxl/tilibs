/* Hey EMACS -*- linux-c -*- */

/*  libtiopers - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 2013 Lionel Debroux
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

#ifndef __TIOPERS__
#define __TIOPERS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ticables.h>
#include <ticalcs.h>

#include "export5.h"

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBOPERS_VERSION "1.0.0"
#else
# define LIBOPERS_VERSION VERSION
#endif

#define LIBOPERS_REQUIRES_LIBTIFILES_VERSION	"1.1.8"	// useless with pkg-config
#define LIBOPERS_REQUIRES_LIBTICABLES_VERSION	"1.3.6"	// useless with pkg-config
#define LIBOPERS_REQUIRES_LIBTICALCS_VERSION	"1.1.10"	// useless with pkg-config

typedef struct _OperHandle	OperHandle;

/**
 * CalcHandle:
 * @options: cable + calculator options
 * @unused1: unused member
 * @unused2: unused member
 * @unused3: unused member
 * @open: device has been opened
 * @busy: transfer is in progress
 * @cable: handle on cable used for this device
 * @cable_attached: set if a cable has been attached
 * @calc: handle on calc used for this device
 * @calc_attached: set if a calc has been attached
 * @priv: private per-handle data
 *
 * A structure used to store information as a handle.
 * !!! This structure is for private use !!!
 **/
struct _OperHandle
{
	DeviceOptions options;

	void*		unused1;
	void*		unused2;
	void*		unused3;

	int			open;
	int			busy;

	CableHandle* cable;
	int			cable_attached;
	CalcHandle* calc;
	int			calc_attached;

	struct {
	} priv;
};

// namespace scheme: library_class_function like tiopers_fext_get

	/****************/
	/* Entry points */
	/****************/
  
	TIEXPORT5 int          TICALL tiopers_library_init(void);
	TIEXPORT5 int          TICALL tiopers_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// tiopers.c
	TIEXPORT5 const char*  TICALL tiopers_version_get (void);

	TIEXPORT5 OperHandle*  TICALL tiopers_handle_new(void);
	TIEXPORT5 int          TICALL tiopers_handle_del(OperHandle *handle);
	TIEXPORT5 int          TICALL tiopers_handle_show(OperHandle *handle);

	TIEXPORT5 int          TICALL tiopers_cable_attach(OperHandle *handle, CableHandle*);
	TIEXPORT5 int          TICALL tiopers_cable_detach(OperHandle *handle);
	TIEXPORT5 CableHandle* TICALL tiopers_cable_get(OperHandle *handle);

	TIEXPORT5 int          TICALL tiopers_calc_attach(OperHandle *handle, CalcHandle*);
	TIEXPORT5 int          TICALL tiopers_calc_detach(OperHandle *handle);
	TIEXPORT5 CalcHandle*  TICALL tiopers_calc_get(OperHandle *handle);

	// device.c
	TIEXPORT5 int          TICALL tiopers_device_open(OperHandle *handle);
	TIEXPORT5 int          TICALL tiopers_device_close(OperHandle *handle);
	TIEXPORT5 int          TICALL tiopers_device_reset(OperHandle *handle);
	TIEXPORT5 int          TICALL tiopers_device_probe_usb(CableModel* cable_model, CablePort *port, CalcModel* calc_model);
	TIEXPORT5 int          TICALL tiopers_device_probe_all(OperHandle *handle, int ***result);

	// opers_xx.c
	TIEXPORT5 int         TICALL tiopers_recv_idlist (OperHandle * handle, uint8_t * idlist, char ** printable_version, const char * filename);
	TIEXPORT5 int         TICALL tiopers_dump_rom (OperHandle * handle, CalcDumpSize size, const char * filename);
	TIEXPORT5 int         TICALL tiopers_get_infos(OperHandle* handle, CalcInfos *infos, char * str, uint32_t maxlen);
	TIEXPORT5 void        TICALL tiopers_format_bytes(unsigned long value, char * buffer);

	// error.c
	TIEXPORT5 int          TICALL tiopers_error_get (int number, char **message);
	TIEXPORT5 int          TICALL tiopers_error_free (char *message);

	/************************/
	/* Deprecated functions */
	/************************/

#ifdef __cplusplus
}
#endif

#endif
