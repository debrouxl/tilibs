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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* 
	This unit allows to trace bytes which are transferred between PC
	and TI calculator.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "stdints1.h"
#include "gettext.h"
#include "logging.h"
#include "data_log.h"

#include "log_hex.h"
#include "log_dbus.h"
#include "log_dusb.h"
#include "log_nsp.h"

int log_start(CableHandle *h)
{
	gchar *tmp;
	
	tmp = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, NULL);
	g_mkdir(tmp, 0);
	g_free(tmp);

	log_hex_start();

	if(h->model == CABLE_USB)
	{
		log_dusb_start();
		log_nsp_start();
	}
	if(h->model != CABLE_USB)
	{
		log_dbus_start();
	}

  	return 0;
}

int log_1(CableHandle *h, int dir, uint8_t data)
{
	log_hex_1(dir, data);

	if(h->model == CABLE_USB)
	{
		log_dusb_1(dir, data);
		log_nsp_1(dir, data);
	}
	if(h->model != CABLE_USB)
	{
		log_dbus_1(dir, data);
	}

  	return 0;
}

int log_N(CableHandle *h, int dir, uint8_t *data, int len)
{
	int i;

	//printf("<%i> ", len);
	for(i = 0; i < len; i++)
	{
		
		log_hex_1(dir, data[i]);
		log_dusb_1(dir, data[i]);
		log_dbus_1(dir, data[i]);
		log_nsp_1(dir, data[i]);
	}
  	
  	return 0;
}

int log_stop(CableHandle *h)
{
	log_hex_stop();

	if(h->model == CABLE_USB)
	{
		log_dusb_stop();
		log_nsp_stop();
	}
	if(h->model != CABLE_USB)
	{
		log_dbus_stop();
	}

  	return 0;
}
