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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(__WIN32__)
#include <windows.h>
#endif

#include "gettext.h"
#include "ticables.h"
#include "logging.h"
#include "error.h"
#include "link_xxx.h"

/*****************/
/* Internal data */
/*****************/

static TiCable const *const cables[] = 
{
	&cable_nul,/*
	&cable_tig,
	&cable_ser,
	&cable_slv,
	&cable_par,
	&cable_vti,
	&cable_tie,*/
	NULL
};

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticables_instance = 0;	// counts # of instances

/*
	This function should be the first one to call.
  	It tries to list available I/O resources for later use.
 */
TIEXPORT int TICALL tifiles_library_init(void)
{
	char locale_dir[65536];
	
#ifdef __WIN32__
  	HANDLE hDll;
  	int i;
  	
  	hDll = GetModuleHandle("ticables.dll");
  	GetModuleFileName(hDll, locale_dir, 65535);
  	
  	for (i = strlen(locale_dir); i >= 0; i--) {
    	if (locale_dir[i] == '\\')
      		break;
  	}
  	
  	locale_dir[i] = '\0';
  	strcat(locale_dir, "\\locale");
#else
	strcpy(locale_dir, LOCALEDIR);
#endif

	if (ticables_instance)
		return (++ticables_instance);
	ticables_info(_("ticables library version %s\n"), LIBTICABLES_VERSION);
  	errno = 0;

#if defined(ENABLE_NLS)
  	ticables_info("setlocale: <%s>\n", setlocale(LC_ALL, ""));
  	ticables_info("bindtextdomain: <%s>\n", bindtextdomain(PACKAGE, locale_dir));
  	//bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
  	ticables_info("textdomain: <%s>\n", textdomain(PACKAGE));
#endif

  	return (++ticables_instance);
}


/*
  	This function should be called when the libticables library is
  	no longer used.
 */
TIEXPORT int
TICALL ticable_exit(void)
{
  	return (--ticables_instance);
}

/***********/
/* Methods */
/***********/

/**
 * tifiles_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
TIEXPORT const char *TICALL ticables_version_get(void)
{
	return LIBTICABLES_VERSION;
}

TIEXPORT TiHandle* TICALL ticables_handle_new(TiCableModel model, TiCablePort port)
{
	TiHandle *handle = (TiHandle *)calloc(1, sizeof(TiHandle));

	handle->model = model;
	handle->port = port;

	return handle;
}

TIEXPORT int       TICALL ticables_handle_del(TiHandle* handle)
{
	free(handle);
	return 0;
}

TIEXPORT int TICALL ticables_options_set_timeout(TiHandle* handle, int timeout)
{
	return handle->timeout = timeout;
}
	
TIEXPORT int TICALL ticables_options_set_delay(TiHandle* handle, int delay)
{
	return handle->delay = delay;
}


TIEXPORT TiCableModel TICALL ticables_get_model(TiHandle* handle)
{
	return handle->model;
}

TIEXPORT TiCablePort  TICALL ticables_get_port(TiHandle* handle)
{
	return handle->port;
}

TIEXPORT int TICALL ticables_cable_open(TiHandle* handle)
{
	TiCable *cable = handle->cable;

	cable->open(handle);
	handle->open = 1;

	return 0;
}

TIEXPORT int TICALL ticables_cable_close(TiHandle* handle)
{
	TiCable *cable = handle->cable;

	cable->close(handle);
	handle->open = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_send(TiHandle* handle, uint8_t *data, uint16_t len)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0 && len > 0)
		return -1;

	handle->busy = 1;
	//cable->send(handle, data, len);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_recv(TiHandle* handle, uint8_t *data, uint16_t len)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0 && len > 0)
		return -1;

	handle->busy = 1;
	//cable->recv(handle, data, len);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_check(TiHandle* handle, uint8_t *data)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy)
		return -1;

	handle->busy = 1;
	//cable->check(handle, data);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_reset(TiHandle* handle)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	cable->reset(handle);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_probe(TiHandle* handle)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	cable->probe(handle);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_set_d0(TiHandle* handle, int state)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0 )
		return -1;

	handle->busy = 1;
	cable->set_d0(handle, state);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_set_d1(TiHandle* handle, int state)
{
	TiCable *cable = handle->cable;
	
	if (handle->open == 1 && handle->busy == 0)
		return -1;

	handle->busy = 1;
	cable->set_d1(handle, state);
	handle->busy = 0;

	return 0;
}

TIEXPORT int TICALL ticables_cable_get_d0(TiHandle* handle)
{
	return 0;
}

TIEXPORT int TICALL ticables_cable_get_d1(TiHandle* handle)
{
	return 0;
}

TIEXPORT int TICALL ticables_cable_progress(TiHandle* handle, int *count, int *msec)
{
	return 0;
}

/*
static void ticables_cable_show_infos(TiCableHandle *handle)
{
  	ticables_info(_("list of settings:\n"));
  	
  	ticables_info(_("  cable: %s\n"), ticables_model_to_string(handle->model));
  	
  	ticables_info(_("  port: %s\n"), ticables_port_to_string(handle->port));
  	
  	ticables_info(_("  method: %s\n"), ticables_method_to_string(handle->method));
  	  	
  	if((handle->model == LINK_PAR) || (handle->model == LINK_SER))
		if(handle->address != 0x000)
			ticables_info(_("  address: 0x%03x\n"), handle->address);
	
	ticables_info(_("  device name: %s\n"), handle->device);

	if((handle->model == LINK_PAR) || (handle->model == LINK_SER))
                ticables_info(_("  timeout value: %i\n"), handle->timeout);

	ticables_info(_("  delay value: %i\n"), handle->delay);
}
*/

/*
TIEXPORT int TICALL ticables_cable_set(TiCableHandle *handle)
{
	int ret;

	// remove link cable
	mapping_unregister_cable(lc);
	cable_type = type;

	// compile informations (I/O resources & OS platform) in order to 
  	// determine the best I/O method to use.
	ret = mapping_get_method(type, resources, &method);
	if(ret)
	{
		print_settings();
		return ret;
	}

  	// set the link cable
  	ret = mapping_register_cable(type, lc);
  	if(ret)
	{
		print_settings();
		return ret;
	}

  	// displays useful infos
  	ticables_cable_show_infos(handle);

  	return 0;
}
*/

